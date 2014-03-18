#ifndef __CLOCK_44XX_H__
#define __CLOCK_44XX_H__

#define PLL_STOP        1         /* PER & IVA */
#define PLL_MN_POWER_BYPASS 	4
#define PLL_LOW_POWER_BYPASS   5  /* MPU, IVA & CORE */
#define PLL_FAST_RELOCK_BYPASS 6  /* CORE */
#define PLL_LOCK        7         /* MPU, IVA, CORE & PER */

/* CM_IDLEST_DPLL fields */
#define ST_DPLL_CLK_MASK                1

/* The following configurations are OPP and SysClk value independant
 * and hence are defined here. 
 */

/* CORE DPLL */
#define CORE_M3X2      2        /* 332MHz : CM_CLKSEL1_EMU */
#define CORE_SSI_DIV   3        /* 221MHz : CM_CLKSEL_CORE */
#define CORE_FUSB_DIV  2        /* 41.5MHz: */
#define CORE_L4_DIV    2        /*  83MHz : L4 */
#define CORE_L3_DIV    2        /* 166MHz : L3 {DDR} */
#define GFX_DIV        2        /*  83MHz : CM_CLKSEL_GFX */
#define WKUP_RSM       2        /* 41.5MHz: CM_CLKSEL_WKUP */

/* PER DPLL */
#define PER_M6X2       3         /* 288MHz: CM_CLKSEL1_EMU */
#define PER_M5X2       4         /* 216MHz: CM_CLKSEL_CAM */
#define PER_M4X2       9         /* 96MHz : CM_CLKSEL_DSS-dss1 */
#define PER_M3X2       16        /* 54MHz : CM_CLKSEL_DSS-tv */

#define CLSEL1_EMU_VAL ((CORE_M3X2 << 16) | (PER_M6X2 << 24) | (0x0a50))

#endif
