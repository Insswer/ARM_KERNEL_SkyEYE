#include <asm/cpu.h>
#include <asm/clocks.h>
#include <ddr.h>
#include <io.h>
#include <emif.h>


/* Timing regs for Elpida */
struct ddr_regs ddr_regs_elpida2G_400_mhz = {
	.tim1		= 0x10eb065a,
	.tim2		= 0x20370dd2,
	.tim3		= 0x00b1c33f,
	.phy_ctrl_1	= 0x849FF408,
	.ref_ctrl	= 0x00000618,
	.config_init	= 0x80000eb1,
	.config_final	= 0x80001ab1,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

/*
 * 400 MHz + 2 CS = 1 GB
 */
struct ddr_regs ddr_regs_elpida2G_400_mhz_2cs = {
	/* tRRD changed from 10ns to 12.5ns because of the tFAW requirement*/
	.tim1		= 0x10eb0662,
	.tim2		= 0x20370dd2,
	.tim3		= 0x00b1c33f,
	.phy_ctrl_1	= 0x049FF408,
	.ref_ctrl	= 0x00000618,
	.config_init	= 0x80800eb9,
	.config_final	= 0x80801ab9,
	.zq_config	= 0xd00b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

struct ddr_regs ddr_regs_elpida4G_400_mhz_1cs = {
	.tim1		= 0x10eb0662,
	.tim2		= 0x20370dd2,
	.tim3		= 0x00b1c33f,
	.phy_ctrl_1	= 0x449FF408,
	.ref_ctrl	= 0x00000618,
	.config_init	= 0x80800eb2,
	.config_final	= 0x80801ab2,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

/*
 * Set Read Latency value to 0xB and DLL delay to 0x37
 * according to OMAP4470 LPDDR interface configuration
 * update for 466 MHz
 */
struct ddr_regs ddr_regs_elpida4G_466_mhz_1cs = {
	.tim1		= 0x130F376B,
	.tim2		= 0x3041105A,
	.tim3		= 0x00F543CF,
	.phy_ctrl_1	= 0x449FF408,
	.ref_ctrl	= 0x0000071B,
	.config_init	= 0x80800eb2,
	.config_final	= 0x80801ab2,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

struct ddr_regs ddr_regs_elpida2G_380_mhz = {
	.tim1		= 0x10cb061a,
	.tim2		= 0x20350d52,
	.tim3		= 0x00b1431f,
	.phy_ctrl_1	= 0x049FF408,
	.ref_ctrl	= 0x000005ca,
	.config_init	= 0x80800eb1,
	.config_final	= 0x80801ab1,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

struct ddr_regs ddr_regs_elpida2G_200_mhz = {
	.tim1		= 0x08648309,
	.tim2		= 0x101b06ca,
	.tim3		= 0x0048a19f,
	.phy_ctrl_1	= 0x849FF405,
	.ref_ctrl	= 0x0000030c,
	.config_init	= 0x80000eb1,
	.config_final	= 0x80000eb1,
	.zq_config	= 0x500b3215,
	.mr1		= 0x23,
	.mr2		= 0x1
};

struct ddr_regs ddr_regs_elpida2G_200_mhz_2cs = {
	.tim1		= 0x08648309,
	.tim2		= 0x101b06ca,
	.tim3		= 0x0048a19f,
	.phy_ctrl_1	= 0x049FF405,
	.ref_ctrl	= 0x0000030c,
	.config_init	= 0x80800eb9,
	.config_final	= 0x80800eb9,
	.zq_config	= 0xD00b3215,
	.mr1		= 0x23,
	.mr2		= 0x1
};


void ddr_init(void)
{
	u32 rev;
	struct ddr_regs *ddr_regs = 0;
	rev = omap_revision();
	if (rev == OMAP4430_ES1_0)
		ddr_regs = &ddr_regs_elpida2G_380_mhz;
	else if (rev == OMAP4430_ES2_0)
		ddr_regs = &ddr_regs_elpida2G_200_mhz_2cs;
	else if (rev >= OMAP4430_ES2_1 && rev < OMAP4470_ES1_0)
		ddr_regs = &ddr_regs_elpida2G_400_mhz_2cs;
	else if (rev >= OMAP4470_ES1_0)
		ddr_regs = &ddr_regs_elpida4G_400_mhz_1cs;
	/*
	 * DMM DMM_LISA_MAP_i registers fields description
	 * [31:24] SYS_ADDR
	 * [22:20] SYS_SIZE
	 * [19:18] SDRC_INTLDMM
	 * [17:16] SDRC_ADDRSPC
	 * [9:8] SDRC_MAP
	 * [7:0] SDRC_ADDR
	 */

	/* TI Errata i614 - DMM Hang Issue During Unmapped Accesses
	 * CRITICALITY: Medium
	 * REVISIONS IMPACTED: OMAP4430 all
	 * DESCRIPTION
	 * DMM replies with an OCP error response in case of unmapped access.
	 * DMM can generate unmapped access at the end of a mapped section.
	 * A hang occurs if an unmapped access is issued after a mapped access.
	 *
	 * WORKAROUND
	 * Define LISA section to have response error when unmapped addresses.
	 * Define LISA section in such a way that all transactions reach the
	 * EMIF, which will return an error response.
	 *
	 * For W/A configure DMM Section 0 size equal to 2048 MB and map it on
	 * SDRC_ADDRSPC=0, and move original SDRAM configuration
	 * to section 1 with higher priority.
	 *
	 * This W/A has been applied for all OMAP4 CPUs - It doesn't any harm
	 * on 4460, and even desirable than hitting an un-mapped area in DMM.
	 * This makes the code simpler and easier to understand and
	 * the settings will be more uniform.
	 **/
	/* TRAP for catching accesses to the umapped memory */
//	__raw_writel(0x80720100, DMM_BASE + DMM_LISA_MAP_0);

//	__raw_writel(0x00000000, DMM_BASE + DMM_LISA_MAP_2);
	/* TRAP for catching accesses to the memory actually used by TILER */
//	__raw_writel(0xFF020100, DMM_BASE + DMM_LISA_MAP_1);


	writel(0x0, DMM_BASE + DMM_LISA_MAP_3);
	writel(0x0, DMM_BASE + DMM_LISA_MAP_2);
	writel(0x0, DMM_BASE + DMM_LISA_MAP_1);
	writel(0x0, DMM_BASE + DMM_LISA_MAP_0);

	writel(0x80640300, DMM_BASE + DMM_LISA_MAP_3);
	writel(0x0, DMM_BASE + DMM_LISA_MAP_2);
	writel(0x0, DMM_BASE + DMM_LISA_MAP_1);
	writel(0xff020100, DMM_BASE + DMM_LISA_MAP_0);

	if (rev == OMAP4430_ES1_0)
		/* original DMM configuration
		 * - 512 MB, 128 byte interleaved, EMIF1&2, SDRC_ADDRSPC=0 */
		writel(0x80540300, DMM_BASE + DMM_LISA_MAP_3);
	else if (rev < OMAP4460_ES1_0)
		/* original DMM configuration
		 * - 1024 MB, 128 byte interleaved, EMIF1&2, SDRC_ADDRSPC=0 */
		writel(0x80640300, DMM_BASE + DMM_LISA_MAP_3);
	else {
		/* OMAP4460 and higher: original DMM configuration
		 * - 1024 MB, 128 byte interleaved, EMIF1&2, SDRC_ADDRSPC=0 */
//		__raw_writel(0x80640300, DMM_BASE + DMM_LISA_MAP_3);

		writel(0x80640300, MA_BASE + DMM_LISA_MAP_3);
		writel(0x00000000, MA_BASE + DMM_LISA_MAP_2);
		writel(0x00000000, MA_BASE + DMM_LISA_MAP_1);
		writel(0xff020100, MA_BASE + DMM_LISA_MAP_0);
	}

	/* same memory part on both EMIFs */
	do_ddr_init(ddr_regs, ddr_regs);

	/* Pull Dn enabled for "Weak driver control" on LPDDR
	 * Interface.
	 */
	if (rev >= OMAP4460_ES1_0) {
		writel(0x9c9c9c9c, CONTROL_LPDDR2IO1_0);
		writel(0x9c9c9c9c, CONTROL_LPDDR2IO1_1);
		writel(0x9c989c00, CONTROL_LPDDR2IO1_2);
		writel(0xa0888c03, CONTROL_LPDDR2IO1_3);
		writel(0x9c9c9c9c, CONTROL_LPDDR2IO2_0);
		writel(0x9c9c9c9c, CONTROL_LPDDR2IO2_1);
		writel(0x9c989c00, CONTROL_LPDDR2IO2_2);
		writel(0xa0888c03, CONTROL_LPDDR2IO2_3);
	}


}

/*****************************************
 * Routine: ddr_init
 * Description: Configure DDR
 * EMIF1 -- CS0 -- DDR1 (256 MB)
 * EMIF2 -- CS0 -- DDR2 (256 MB)
 *****************************************/
void do_ddr_init(struct ddr_regs *emif1_ddr_regs,struct ddr_regs *emif2_ddr_regs)
{
	unsigned int rev;
	rev = omap_revision();

	if (rev == OMAP4430_ES1_0)
	{
		/* Configure the Control Module DDRIO device */
		writel(0x1c1c1c1c, 0x4A100638);
		writel(0x1c1c1c1c, 0x4A10063c);
		writel(0x1c1c1c1c, 0x4A100640);
		writel(0x1c1c1c1c, 0x4A100648);
		writel(0x1c1c1c1c, 0x4A10064c);
		writel(0x1c1c1c1c, 0x4A100650);
		/* LPDDR2IO set to NMOS PTV */
		writel(0x00ffc000, 0x4A100704);
	} else if (rev == OMAP4430_ES2_0) {
		writel(0x9e9e9e9e, 0x4A100638);
		writel(0x9e9e9e9e, 0x4A10063c);
		writel(0x9e9e9e9e, 0x4A100640);
		writel(0x9e9e9e9e, 0x4A100648);
		writel(0x9e9e9e9e, 0x4A10064c);
		writel(0x9e9e9e9e, 0x4A100650);
		/* LPDDR2IO set to NMOS PTV */
		writel(0x00ffc000, 0x4A100704);
	} else if (rev >= OMAP4430_ES2_1) {
		writel(0x7c7c7c7c, 0x4A100638);
		writel(0x7c7c7c7c, 0x4A10063c);
		writel(0x7c787c00, 0x4A100640);
		writel(0x7c7c7c7c, 0x4A100648);
		writel(0x7c7c7c7c, 0x4A10064c);
		writel(0x7c787c00, 0x4A100650);
		/*
		 * Adjust Internal Vref controls to reduce leakage
		 * for chip retention (Core OSWR)
		 */
		writel(0xa388bc03, 0x4A100644);
		writel(0xa388bc03, 0x4A100654);
		/* LPDDR2IO set to NMOS PTV */
		/* To be updated according to Process */
		/*__raw_writel(0x00ffc000, 0x4A100704); */
	}

	/* DDR needs to be initialised @ 19.2 MHz
	 * So put core DPLL in bypass mode
	 * Configure the Core DPLL but don't lock it
	 */
	configure_core_dpll_no_lock();

	/* No IDLE: BUG in SDC */
	writel(0x0, EMIF1_BASE + EMIF_PWR_MGMT_CTRL);
	writel(0x0, EMIF2_BASE + EMIF_PWR_MGMT_CTRL);

	/* Configure EMIF1 */
	emif_config(EMIF1_BASE, emif1_ddr_regs);

	/* Configure EMIF2 */
	emif_config(EMIF2_BASE, emif2_ddr_regs);
	/* Lock Core using shadow CM_SHADOW_FREQ_CONFIG1 */
	lock_core_dpll_shadow();
	/* TODO: SDC needs few hacks to get DDR freq update working */

	/* Set DLL_OVERRIDE = 0 */
	writel(0x0, CM_DLL_CTRL);

	spin_delay(200);

	/* Check for DDR PHY ready for EMIF1 & EMIF2 */
	while(((readl(EMIF1_BASE + EMIF_STATUS) & 0x04) != 0x04)
		|| ((readl(EMIF2_BASE + EMIF_STATUS) & 0x04) != 0x04));

	/* Reprogram the DDR PYHY Control register */
	/* PHY control values */

	sr32(CM_MEMIF_EMIF_1_CLKCTRL, 0, 32, 0x1);
        sr32(CM_MEMIF_EMIF_2_CLKCTRL, 0, 32, 0x1);

	/* Put the Core Subsystem PD to ON State */

	/* No IDLE: BUG in SDC */
	writel(0x80000000, EMIF1_BASE + EMIF_PWR_MGMT_CTRL);
	writel(0x80000000, EMIF2_BASE + EMIF_PWR_MGMT_CTRL);

	/* SYSTEM BUG:
	 * In n a specific situation, the OCP interface between the DMM and
	 * EMIF may hang.
	 * 1. A TILER port is used to perform 2D burst writes of
	 * 	 width 1 and height 8
	 * 2. ELLAn port is used to perform reads
	 * 3. All accesses are routed to the same EMIF controller
	 *
	 * Work around to avoid this issue REG_SYS_THRESH_MAX value should
	 * be kept higher than default 0x7. As per recommondation 0x0A will
	 * be used for better performance with REG_LL_THRESH_MAX = 0x00
	 */
	if (omap_revision() >= OMAP4460_ES1_0) {
		writel(0x0A300000, EMIF1_BASE + EMIF_L3_CONFIG);
		writel(0x0A300000, EMIF2_BASE + EMIF_L3_CONFIG);
	} else {
		writel(0x0A0000FF, EMIF1_BASE + EMIF_L3_CONFIG);
		writel(0x0A0000FF, EMIF2_BASE + EMIF_L3_CONFIG);
	}

	reset_phy(EMIF1_BASE);
	reset_phy(EMIF2_BASE);
}



