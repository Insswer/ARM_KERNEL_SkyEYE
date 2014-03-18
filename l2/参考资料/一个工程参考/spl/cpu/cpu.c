#include <asm/cpu.h>
#include <asm/clocks.h>
#include <ns16550.h>
#include <config.h>
#include <io.h>
#include <led.h>
#include <asm/rom_hal.h>


/* See also ARM Ref. Man. */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */
#define C1_WB		(1<<3)		/* merging write buffer on/off */
#define C1_BIG_ENDIAN	(1<<7)		/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13) /* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3)	/* must be 111b for R/W */

#define PL310_POR	5

static void disable_watchdog(void)
{
	int pending = 1;
	/* see trm for details */
	writel(WD_UNLOCK1, WD2_BASE + WSPR);
	do {
		pending = readl(WD2_BASE + WWPS);
	} while (pending);
	writel(WD_UNLOCK2, WD2_BASE + WSPR);
}

static void errata_i727_workaround(void)
{
	u32 reg;
	/* Errata i727
	  force self-refresh immediately when coming back from warm reset
	  set EMIF1 ddr self refresh mode and clear self refresh timer.
	*/
	reg = readl(EMIF1_BASE + EMIF_PWR_MGMT_CTRL);
	reg &= ~EMIF_PWR_MGMT_CTRL_LP_MODE_MASK;
	reg |= EMIF_PWR_MGMT_CTRL_LP_MODE_REFRESH;
	reg &= ~EMIF_PWR_MGMT_CTRL_SR_TIM_MASK;
	writel(reg,EMIF1_BASE + EMIF_PWR_MGMT_CTRL);

	/* set EMIF2 ddr self refresh mode and clear self refresh timer.*/
	reg = readl(EMIF2_BASE + EMIF_PWR_MGMT_CTRL);
	reg &= ~EMIF_PWR_MGMT_CTRL_LP_MODE_MASK;
	reg |= EMIF_PWR_MGMT_CTRL_LP_MODE_REFRESH;
	reg &= ~EMIF_PWR_MGMT_CTRL_SR_TIM_MASK;
	writel(reg,EMIF2_BASE + EMIF_PWR_MGMT_CTRL); 

	readl(EMIF1_BASE + EMIF_PWR_MGMT_CTRL);
	readl(EMIF2_BASE + EMIF_PWR_MGMT_CTRL);
	
	
}


static void omap_vc_init(u8 hscll, u8 hsclh, u8 scll, u8 sclh)
{
	u32 val;

	val = 0x00 << PRM_VC_CFG_I2C_MODE_HSMCODE_SHIFT;
	if (hscll || hsclh)
		val |= PRM_VC_CFG_I2C_MODE_HSMODEEN_BIT;

	writel(val, PRM_VC_CFG_I2C_MODE);

	hscll &= PRM_VC_CFG_I2C_CLK_HSCLL_MASK;
	hsclh &= PRM_VC_CFG_I2C_CLK_HSCLH_MASK;
	scll &= PRM_VC_CFG_I2C_CLK_SCLL_MASK;
	sclh &= PRM_VC_CFG_I2C_CLK_SCLH_MASK;

	val = hscll << PRM_VC_CFG_I2C_CLK_HSCLL_SHIFT |
	    hsclh << PRM_VC_CFG_I2C_CLK_HSCLH_SHIFT |
	    scll << PRM_VC_CFG_I2C_CLK_SCLL_SHIFT |
	    sclh << PRM_VC_CFG_I2C_CLK_SCLH_SHIFT;
	writel(val, PRM_VC_CFG_I2C_CLK);
}


static int omap_vc_bypass_send_value(u8 sa, u8 reg_addr, u8 reg_data)
{
	/*
	 * Unfortunately we need to loop here instead of a defined time
	 * use arbitary large value
	 */
	u32 timeout = 0xFFFF;
	u32 reg_val;

	sa &= PRM_VC_VAL_BYPASS_SLAVEADDR_MASK;
	reg_addr &= PRM_VC_VAL_BYPASS_REGADDR_MASK;
	reg_data &= PRM_VC_VAL_BYPASS_DATA_MASK;

	/* program VC to send data */
	reg_val = sa << PRM_VC_VAL_BYPASS_SLAVEADDR_SHIFT |
	    reg_addr << PRM_VC_VAL_BYPASS_REGADDR_SHIFT |
	    reg_data << PRM_VC_VAL_BYPASS_DATA_SHIFT;
	writel(reg_val, PRM_VC_VAL_BYPASS);

	/* Signal VC to send data */
	writel(reg_val | PRM_VC_VAL_BYPASS_VALID_BIT, PRM_VC_VAL_BYPASS);

	/* Wait on VC to complete transmission */
	do {
		reg_val = readl(PRM_VC_VAL_BYPASS) &
		    PRM_VC_VAL_BYPASS_VALID_BIT;
		if (!reg_val)
			break;

		spin_delay(100);
	} while (--timeout);

	/* Cleanup PRM int status reg to leave no traces of interrupts */
	reg_val = readl(PRM_IRQSTATUS_MPU);
	writel(reg_val, PRM_IRQSTATUS_MPU);

	/* In case we can do something about it in future.. */
	if (!timeout)
		return -1;

	/* All good.. */
	return 0;
}

static void do_scale_tps62361(u32 reg, u32 val)
{
	u32 l = 0;

	/*
	 * Select SET1 in TPS62361:
	 * VSEL1 is grounded on board. So the following selects
	 * VSEL1 = 0 and VSEL0 = 1
	 */

	/* set GPIO-7 direction as output */
	l = readl(0x4A310134);
	l &= ~(1 << TPS62361_VSEL0_GPIO);
	writel(l, 0x4A310134);

	omap_vc_bypass_send_value(TPS62361_I2C_SLAVE_ADDR, reg, val);

	/* set GPIO-7 data-out */
	l = 1 << TPS62361_VSEL0_GPIO;
	writel(l, 0x4A310194);

}


static void scale_vcore_omap4460(unsigned int rev)
{
	u32 volt;

	/* vdd_core - TWL6030 VCORE 1 - OPP100 - 1.127V */
	omap_vc_bypass_send_value(TWL6030_SRI2C_SLAVE_ADDR,
				  TWL6030_SRI2C_REG_ADDR_VCORE1, 0x22);

	/* vdd_mpu - TPS62361 - OPP100 - 1.210V (roundup from 1.2V) */
	volt = 1210;
	volt -= TPS62361_BASE_VOLT_MV;
	volt /= 10;
	do_scale_tps62361(TPS62361_REG_ADDR_SET1, volt);

	/* vdd_iva - TWL6030 VCORE 2 - OPP50  - 0.950V */
	omap_vc_bypass_send_value(TWL6030_SRI2C_SLAVE_ADDR,
				  TWL6030_SRI2C_REG_ADDR_VCORE2, 0x14);
}

unsigned int cortex_a9_rev(void)
{

	unsigned int i;

	asm ("mrc p15, 0, %0, c0, c0, 0" : "=r" (i));

	return i;
}

unsigned int omap_revision(void)
{
	/*
	 * For some of the ES2/ES1 boards ID_CODE is not reliable:
	 * Also, ES1 and ES2 have different ARM revisions
	 * So use ARM revision for identification
	 */
	unsigned int rev = cortex_a9_rev();
	unsigned int proc_version = 0;

	switch (rev) {
		case MIDR_CORTEX_A9_R0P1:
			return OMAP4430_ES1_0;
		case MIDR_CORTEX_A9_R1P2:
			rev = readl(CONTROL_ID_CODE);
			switch (rev) {
				case OMAP4_CONTROL_ID_CODE_ES2_2:
					return OMAP4430_ES2_2;
				case OMAP4_CONTROL_ID_CODE_ES2_1:
					return OMAP4430_ES2_1;
				case OMAP4_CONTROL_ID_CODE_ES2_0:
					return OMAP4430_ES2_0;
				default:
					return OMAP4430_ES2_0;
			}
		case MIDR_CORTEX_A9_R1P3:
			return OMAP4430_ES2_3;
		case MIDR_CORTEX_A9_R2P10:
			rev = readl(CONTROL_ID_CODE);
			/* For 4460/4470 skip Version Number
			 * and use Ramp System Number only.
			 * There isn't a difference between v1.0/1.1
			 * for x-loader
			 */
			proc_version = (rev >> 28) & 0xf;
			rev &= OMAP4_CONTROL_ID_CODE_RAMP_MASK;
			switch (rev) {
				case OMAP4_CONTROL_ID_CODE_4460_ES1:
					switch (proc_version) {
						case 0x0:
							return OMAP4460_ES1_0;
						case 0x2:
							return OMAP4460_ES1_1;
						default:
							return OMAP44XX_SILICON_ID_INVALID;
					}
				case OMAP4_CONTROL_ID_CODE_4470_ES1:
					return OMAP4470_ES1_0;
				default:
					return OMAP44XX_SILICON_ID_INVALID;
			}
		default:
			return OMAP44XX_SILICON_ID_INVALID;
	}
}


static void scale_vcores(void)
{
	unsigned int rev = omap_revision();
	omap_vc_init(0x00, 0x00, 0x60, 0x26);
	scale_vcore_omap4460(rev);
}


void s_init(void)
{
	/* fix some problems */
	errata_i727_workaround();
	set_muxconf_regs();
	init_gpio_led();
	setup_console();

	putc('K');
	led_off(1);
	led_off(2);
	scale_vcores();	
	/* setup uart for debug */
	ddr_init();
	prcm_init();
}

u32 get_device_type(void)
{
	/*
	 * Retrieve the device type: GP/EMU/HS/TST stored in
	 * CONTROL_STATUS
	 */
	return (readl(CONTROL_STATUS) & DEVICE_MASK) >> 8;
}


/* Defines for ROM Services */
#define ROM_SERVICE_PL310_AUXCR		0x109
#define ROM_SERVICE_PL310_POR		0x113

/*
 * Define only official TI *REQUIRED* PPA services here.
 * 	PPA_SERVICE_xyz
 *
 * IMPORTANT: PPA service ID may change based on PPA used,
 */
#define PPA_SERVICE_PL310_POR		0x23



/*FIX_ME*/
void cpu_init (void)
{
	unsigned int es_revision;

	es_revision = omap_revision();

	/* OMAP 4430 ES1.0 is the only device rev that does not support
	 * modifying PL310.POR. Thus this is will be applied for any 4430 rev
	 * (except 1.0) and any 4460 */
	if (es_revision > OMAP4430_ES1_0 && get_device_type() != GP_DEVICE) {
		/* Set PL310 Prefetch Offset Register w/PPA svc*/
		omap_smc_ppa(PPA_SERVICE_PL310_POR, 0, 0x7, 1, PL310_POR);
		/* Enable L2 data prefetch */
		omap_smc_rom(ROM_SERVICE_PL310_AUXCR,
			readl(OMAP44XX_PL310_AUX_CTRL) | 0x10000000);
	/* This ROM svc is availble only for OMAP4430 ES2.2 or any 4460 */
	} else if (es_revision > OMAP4430_ES2_1) {
		/* Set PL310 Prefetch Offset Register using ROM svc */
		omap_smc_rom(ROM_SERVICE_PL310_POR, PL310_POR);
		/* Enable L2 data prefetch */
		omap_smc_rom(ROM_SERVICE_PL310_AUXCR,
			readl(OMAP44XX_PL310_AUX_CTRL) | 0x10000000);
	}
	/*
	 * Errata: i684
	 * For all ESx.y trimmed and untrimmed units
	 * Override efuse with LPDDR P:16/N:16 and
	 * smart IO P:0/N:0 as per recomendation
	 * OMAP4470 CPU types not impacted.
	 */
	if (es_revision < OMAP4470_ES1_0)
		writel(0x00084000, SYSCTRL_PADCONF_CORE_EFUSE_2);

	/* For ES2.2
	 * 1. If unit does not have SLDO trim, set override
	 * and force max multiplication factor to ensure
	 * proper SLDO voltage at low OPP's
	 * 2. Trim VDAC value for TV out as recomended to avoid
	 * potential instabilities at low OPP's. VDAC is absent
	 * on OMAP4470 CPUs.
	 */

	/*if MPU_VOLTAGE_CTRL is 0x0 unit is not trimmed*/
	if ((es_revision >= OMAP4460_ES1_0) &&
		(((readl(IVA_LDOSRAM_VOLTAGE_CTRL) &
					   ~(0x3e0)) == 0x0)) ||
		((es_revision >= OMAP4430_ES2_2) &&
			 (es_revision < OMAP4460_ES1_0) &&
			 (!(readl(IVA_LDOSRAM_VOLTAGE_CTRL))))) {
		/* Set M factor to max (2.7) */
		writel(0x0401040f, IVA_LDOSRAM_VOLTAGE_CTRL);
		writel(0x0401040f, MPU_LDOSRAM_VOLTAGE_CTRL);
		writel(0x0401040f, CORE_LDOSRAM_VOLTAGE_CTRL);
		if (es_revision < OMAP4470_ES1_0)
			writel(0x000001c0, SYSCTRL_PADCONF_CORE_EFUSE_1);
	}

}

