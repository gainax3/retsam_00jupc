/*
 *	ROMCopy.c		-	Routine to copy from ROM to RAM.
 *
 *	Copyright  2004 Metrowerks, Inc. All Rights Reserved.
 *
 *  Author: Jan Kobler, jkobler@metrowerks.com
 *  Created: 2003.09.19
 *
 *  This is based on the ROMCopy.c file from CodeWarrior for Coldfire 4.0
 */

#include "ROMCopy.h"

/*
 * Routine to flush cache follow the ROM to RAM copy
 */
void __flush_cache(unsigned long dst, unsigned long size)
{
	//TBD
}

/*
 *	Routine to copy a single section from ROM to RAM ...
 */
void __copy_rom_section(unsigned long dst, unsigned long src, unsigned long size)
{
	unsigned long len = size;

	const int size_int = sizeof(int);
	const int mask_int = sizeof(int)-1;

	const int size_short = sizeof(short);
	const int mask_short = sizeof(short)-1;
	
	const int size_char = sizeof(char);
	
	if( dst == src || size == 0)
	{
		return;	
	}

	
	while( len > 0)
	{
	
		if( !(src & mask_int) && !(dst & mask_int) && len >= size_int)
		{
			*((int *)dst)  = *((int*)src);
			dst += size_int;
			src += size_int;
			len -= size_int;
		}
		else if( !(src & mask_short) && !(dst & mask_short) && len >= size_short)
		{
			*((short *)dst)  = *((short*)src);
			dst += size_short;
			src += size_short;
			len -= size_short;
		}
		else
		{
			*((char *)dst)  = *((char*)src);
			dst += size_char;
			src += size_char;
			len -= size_char;
		}
	}
}

/*
 *	Routine that copies all sections the user marked as ROM into
 *	their target RAM addresses ...
 *
 *	__S_romp is defined in the linker command file
 *  It is a table of RomInfo
 *	structures.  The final entry in the table has all-zero
 *	fields.
 */
void __copy_rom_sections_to_ram(void)
{

	int				index;

	/*
	 *	Go through the entire table, copying sections from ROM to RAM.
	 */
	for (index = 0;
		 __S_romp[index].Source != 0 ||
		 __S_romp[index].Target != 0 ||
		 __S_romp[index].Size != 0;
		 ++index)
	{
		__copy_rom_section( __S_romp[index].Target,
							__S_romp[index].Source,
							__S_romp[index].Size );
							
		__flush_cache( __S_romp[index].Target, __S_romp[index].Size);

							
	} 	
}