#ifndef __ROM_HAL_H__
#define __ROM_HAL_H__

#include <io.h>
#include <asm/cpu.h>


/* Public ROM code API base addres changes bewteen OMAP44xx families, so in
 * order to use common code, we use following trick to determine base address
 * HAWKEYE for OMAP4430 is Bx5x while OMAP4460 is Bx4x and OMAP4470 is Bx7x
 */
#define PUBLIC_API_BASE ( \
  ((readl(CONTROL_ID_CODE)>>12) & 0x00F0) == 0x0050 ? \
  0x00028400 : 0x00030400)

#define PUBLIC_API_SEC_ENTRY                            (0x00)

/*
 * omap_smc_ppa() - Entry to ROM code's routine Pub2SecDispatcher.
 * @appl_id:	HAL Service number
 * @proc_id:	for ppa services usually 0.
 * @flag:	service priority
 * @...pargs:	Depending on the PPA service used.
 *
 * This routine manages the entry to secure HAL API.
 *        ----- Use only with MMU disabled! -----
 */
typedef u32 (** const PUBLIC_SEC_ENTRY_Pub2SecDispatcher_pt) \
               (u32 appl_id, u32 proc_ID, u32 flag, ...);
#define omap_smc_ppa \
      (*(PUBLIC_SEC_ENTRY_Pub2SecDispatcher_pt) \
                                   (PUBLIC_API_BASE+PUBLIC_API_SEC_ENTRY))



#endif
