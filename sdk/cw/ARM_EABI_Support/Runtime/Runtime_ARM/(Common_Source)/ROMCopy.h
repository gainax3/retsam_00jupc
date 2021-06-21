/*
 *	ROMCopy.h		-	Routine to copy from ROM to RAM.
 *
 *	Copyright 2004 Metrowerks, Inc. All Rights Reserved.
 *
 *  Author: Jan Kobler, jkobler@metrowerks.com
 *  Created: 2003.09.19
 *
 *  This is based on the ROMCopy.h file from CodeWarrior for Coldfire 4.0
 *
 */

#ifndef __ROMCOPY_H__
#define __ROMCOPY_H__

/* format of the ROM table info entry ... */
typedef struct RomInfo {
	unsigned long	Source;
	unsigned long	Target;
	unsigned long 	Size;
} RomInfo;

/* imported data */
extern RomInfo __S_romp[];		/* linker defined symbol */

/* exported routines */

extern void __copy_rom_section(unsigned long dst, unsigned long src, unsigned long size);
extern void __copy_rom_sections_to_ram(void);
extern void __flush_cache(unsigned long dst, unsigned long size);
#endif
