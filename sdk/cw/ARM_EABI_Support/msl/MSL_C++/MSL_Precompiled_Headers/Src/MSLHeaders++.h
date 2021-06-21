/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 *  $Date: 2006/02/09 17:03:35 $ 
 *  $Revision: 1.9.2.2 $ 
 */

/*
 *	Prefix file for MSL C++ projects which pulls in the most common precompiled
 *	MSL Header files.
 *
 *	This prefix file is ONLY for C++ projects, it will not work for C projects unless
 *	C++ is activated.
 */

#if __MACH__
	#if _MSL_USING_MW_C_HEADERS
		#include <MSLHeaders.Mach-O.mch>
	#else
		#include <MSL-BSDHeaders.Mach-O.mch>
	#endif
#elif __POWERPC__ && !__PPC_EABI__
	#include <MSLHeaders.PPC.mch>
#elif __INTEL__
	#include <MSLHeaders.x86.mch>
#elif defined(__arm)
	#if __option(little_endian)
		#ifndef __thumb
			#include <MSLHeaders_ARM_LE.mch>
		#else
			#include <MSLHeaders_Thumb_LE.mch>
		#endif
	#else
		#ifndef __thumb
			#include <MSLHeaders_ARM_BE.mch>
		#else
			#include <MSLHeaders_Thumb_BE.mch>
		#endif
	#endif
#elif __PALMOS_TRAPS__
	#include <MSLHeaders.Palm_OS.68k.mch>
#else
	#error Unknown platform!
#endif


/* Change record:
 * 001214 JWW Added Mach-O precompiled header case
 * 030609 JWW Hooked up ARM and PalmOS to use MSLHeaders++.h
 */