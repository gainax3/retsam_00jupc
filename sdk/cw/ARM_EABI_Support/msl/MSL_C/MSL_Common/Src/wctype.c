/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:36 $
 * $Revision: 1.42.2.2 $
 */

/*
 *	Routines
 *	--------
 *		iswalnum		
 *		iswalpha		
 *		iswcntrl		
 *		iswdigit		
 *		iswgraph		
 *		iswlower		
 *		iswprint		
 *		iswpunct		
 *		iswspace		
 *		iswupper		
 *		iswxdigit		
 *		towlower
 *		towupper
 *      wctob        
 */

#pragma ANSI_strict off

#include <ansi_parms.h>

#undef	_MSL_USE_INLINE
#define	_MSL_USE_INLINE 1
#undef	_MSL_INLINE
#define	_MSL_INLINE _MSL_DO_NOT_INLINE
  
#if _MSL_WIDE_CHAR	/*- mm 980204 -*/

#include <stdio.h>
#include <wctype.h>
#include <wctype_api.h>
#include <wchar.h>	/*- mm 000201 -*/
#include <string.h> /*- mm 980629 -*/

/* ----- Table for Pro 7 and earlier (do not use in new code!) ----- */
#if __dest_os != __mac_os_x /* Backwards compatibility not needed on Mach-O */
#define owctrl	0x01
#define owmotn	0x02
#define owspac	0x04
#define owpunc	0x08
#define owdigi	0x10
#define owhexd	0x20
#define owlowc	0x40
#define owuppc	0x80
#define owdhex	owhexd | owdigi
#define owuhex	owhexd | owuppc
#define owlhex	owhexd | owlowc

#if !_MSL_C_LOCALE_ONLY

_MSL_IMP_EXP_C wchar_t	__wctype_map[__msl_wcmap_size] = 
{
/*   -0     -1     -2     -3     -4     -5     -6     -7     -8     -9     -A     -B     -C     -D     -E     -F       */
	owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owmotn, owmotn, owmotn, owmotn, owmotn, owctrl, owctrl,
	owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl,
	owspac, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc,
	owdhex, owdhex, owdhex, owdhex, owdhex, owdhex, owdhex, owdhex, owdhex, owdhex, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc,
	owpunc, owuhex, owuhex, owuhex, owuhex, owuhex, owuhex, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc,
	owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owpunc, owpunc, owpunc, owpunc, owpunc,
	owpunc, owlhex, owlhex, owlhex, owlhex, owlhex, owlhex, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc,
	owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owpunc, owpunc, owpunc, owpunc, owctrl
#if __dest_os == __mac_os
	,
	owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc,
	owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owlowc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owuppc, owuppc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owlowc, owlowc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owuppc, owuppc, owuppc, owuppc, owlowc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owlowc, owuppc, owpunc, owpunc, owpunc, owpunc, owlowc, owlowc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc,
	owpunc, owuppc, owuppc, owuppc, owuppc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc
#elif __dest_os == __win32_os || __dest_os == __wince_os															/*- mm 020219 -*/
 	,
	owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl,
	owctrl, owctrl, owpunc, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl, owctrl,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc,
	owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc, owpunc,
	owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc,
	owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owpunc, owuppc, owuppc, owuppc, owuppc, owuppc, owuppc, owlowc, owlowc,
	owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc,
	owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owpunc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc, owlowc
#endif                           /*- mm 970708 -*/ /*- cc 010326 -*/
};
#endif /* _MSL_C_LOCALE_ONLY */
#endif /* __dest_os != __mac_os_x */
/* ----------------------------------------------------------------- */

#define walph	__msl_walpha
#define wblnk	__msl_wblank
#define wcntl	__msl_wcntrl
#define wdigi	__msl_wdigit | __msl_wgraph | __msl_wprint
#define wgrph	__msl_wgraph
#define wlowc	__msl_wlower | __msl_walpha | __msl_wgraph | __msl_wprint
#define wprnt	__msl_wprint
#define wpunc	__msl_wpunct | __msl_wgraph | __msl_wprint
#define wspac	__msl_wspace
#define wuppc	__msl_wupper | __msl_walpha | __msl_wgraph | __msl_wprint
#define whexd	__msl_wxdigit
#define wdhex	whexd | wdigi
#define wuhex	whexd | wuppc
#define wlhex	whexd | wlowc
#define wctbl	wcntl | wblnk
#define wctsp	wcntl | wspac
#define wsblp	wspac | wblnk | wprnt
#define wcsbl	wcntl | wspac | wblnk

#if !_MSL_C_LOCALE_ONLY	
const unsigned short	__msl_wctype_map[__msl_wcmap_size] = 
{
/*   -0     -1     -2     -3     -4     -5     -6     -7     -8     -9     -A     -B     -C     -D     -E     -F       */
	wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcsbl, wctsp, wctsp, wctsp, wctsp, wcntl, wcntl, /* 0- */
	wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, /* 1- */
	wsblp, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 2- */
	wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 3- */
	wpunc, wuhex, wuhex, wuhex, wuhex, wuhex, wuhex, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, /* 4- */
	wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 5- */
	wpunc, wlhex, wlhex, wlhex, wlhex, wlhex, wlhex, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* 6- */
	wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wpunc, wpunc, wpunc, wpunc, wcntl  /* 7- */
#if __dest_os == __mac_os || __dest_os == __mac_os_x
	,
	wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* 8- */
	wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* 9- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wlowc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wuppc, wuppc, /* A- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wlowc, wlowc, /* B- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wsblp, wuppc, wuppc, wuppc, wuppc, wlowc, /* C- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wlowc, wuppc, wpunc, wpunc, wpunc, wpunc, wlowc, wlowc, /* D- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, /* E- */
	wpunc, wuppc, wuppc, wuppc, wuppc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc  /* F- */
#elif __dest_os == __win32_os || __dest_os == __wince_os
 	,
	wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, /* 8- */
	wcntl, wcntl, wpunc, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, /* 9- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* A- */
	wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* B- */
	wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, /* C- */ 
	wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wpunc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wlowc, /* D- */
	wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* E- */
	wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wpunc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* F- */
#endif                           /*- mm 970708 -*/ /*- cc 010326 -*/
};

#endif /* _MSL_C_LOCALE_ONLY */

/* begin addition */ /*- mm 011130 -*/	
const unsigned short	__wctype_mapC[__msl_wcmap_size] = 
{
/*   -0     -1     -2     -3     -4     -5     -6     -7     -8     -9     -A     -B     -C     -D     -E     -F       */
	wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcsbl, wctsp, wctsp, wctsp, wctsp, wcntl, wcntl, /* 0- */
	wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, wcntl, /* 1- */
	wsblp, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 2- */
	wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wdhex, wpunc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 3- */
	wpunc, wuhex, wuhex, wuhex, wuhex, wuhex, wuhex, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, /* 4- */
	wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wuppc, wpunc, wpunc, wpunc, wpunc, wpunc, /* 5- */
	wpunc, wlhex, wlhex, wlhex, wlhex, wlhex, wlhex, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, /* 6- */
	wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wlowc, wpunc, wpunc, wpunc, wpunc, wcntl  /* 7- */
};
/* end addition */ /*- mm 011130 -*/	
#if !_MSL_C_LOCALE_ONLY	
const wchar_t __wlower_map[__msl_wcmap_size] = 																				/*- mm 020219 -*/
{
/*   -0      -1      -2      -3      -4      -5      -6      -7      -8      -9      -A      -B      -C      -D      -E      -F       */
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	L' ',  L'!',  L'"',  L'#',  L'$',  L'%',  L'&',  L'\'',  L'(',  L')',  L'*',  L'+',  L',',  L'-',  L'.',  L'/',
	L'0',  L'1',  L'2',  L'3',  L'4',  L'5',  L'6',  L'7',  L'8',  L'9',  L':',  L';',  L'<',  L'=',  L'>',  L'?',
	L'@',  L'a',  L'b',  L'c',  L'd',  L'e',  L'f',  L'g',  L'h',  L'i',  L'j',  L'k',  L'l',  L'm',  L'n',  L'o',
	L'p',  L'q',  L'r',  L's',  L't',  L'u',  L'v',  L'w',  L'x',  L'y',  L'z',  L'[',  L'\\',  L']',  L'^',  L'_',
	L'`',  L'a',  L'b',  L'c',  L'd',  L'e',  L'f',  L'g',  L'h',  L'i',  L'j',  L'k',  L'l',  L'm',  L'n',  L'o',
	L'p',  L'q',  L'r',  L's',  L't',  L'u',  L'v',  L'w',  L'x',  L'y',  L'z',  L'{',  L'|',  L'}',  L'~',  0x007F,
#if __dest_os == __mac_os || __dest_os == __mac_os_x  /*- Begin mm 030107 insertion -*/
	0x008A, 0x008C, 0x008D, 0x008E, 0x0096, 0x009A, 0x009F, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F, 
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00BE, 0x00BF, 
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x0088, 0x008B, 0x009B, 0x00CF, 0x00CF, 
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D8, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x0089, 0x0090, 0x0087, 0x0091, 0x008F, 0x0092, 0x0094, 0x0095, 0x0093, 0x0097, 0x0099, 
	0x00F0, 0x0098, 0x009C, 0x009E, 0x009D, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF, 
#if 0   /* The following is a Mac translation of the above table */
	L'ä',  L'å',  L'ç',  L'é',  L'ñ',  L'ö',  L'ü',  L'á',  L'à',  L'â',  L'ä',  L'ã',  L'å',  L'ç',  L'é',  L'è',
	L'ê',  L'ë',  L'í',  L'ì',  L'î',  L'ï',  L'ñ',  L'ó',  L'ò',  L'ô',  L'ö',  L'õ',  L'ú',  L'ù',  L'û',  L'ü',
	L'†',  L'°',  L'¢',  L'£',  L'§',  L'•',  L'¶',  L'ß',  L'®',  L'©',  L'™',  L'´',  L'¨',  L'≠',  L'æ',  L'ø',
	L'∞',  L'±',  L'≤',  L'≥',  L'¥',  L'µ',  L'∂',  L'∑',  L'∏',  L'π',  L'∫',  L'ª',  L'º',  L'Ω',  L'æ',  L'ø',
	L'¿',  L'¡',  L'¬',  L'√',  L'ƒ',  L'≈',  L'∆',  L'«',  L'»',  L'…',  L' ',  L'à',  L'ã',  L'õ',  L'œ',  L'œ',
	L'–',  L'—',  L'“',  L'”',  L'‘',  L'’',  L'÷',  L'◊',  L'ÿ',  L'ÿ',  L'⁄',  L'€',  L'‹',  L'›',  L'ﬁ',  L'ﬂ',   /*- mm 020228 -*/
	L'‡',  L'·',  L'‚',  L'„',  L'‰',  L'â',  L'ê',  L'á',  L'ë',  L'è',  L'í',  L'î',  L'ï',  L'ì',  L'ó',  L'ô',
	L'',  L'ò',  L'ú',  L'û',  L'ù',  L'ı',  L'ˆ',  L'˜',  L'¯',  L'˘',  L'˙',  L'˚',  L'¸',  L'˝',  L'˛',  L'ˇ'	/*- mm 980629 -*/
#endif	
#elif __dest_os == __win32_os || __dest_os == __wince_os
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F, 
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00D7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00DE, 0x00DF, 
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF, 

#if 0  /* The following is a Windows translation of the above table */
	L'Ä',  L'Å',  L'Ç',  L'É',  L'Ñ',  L'Ö',  L'Ü',  L'á',  L'à',  L'â',  L'ä',  L'ã',  L'å',  L'ç',  L'é',  L'è', /*- mm 020219 -*/
	L'ê',  L'ë',  L'í',  L'ì',  L'î',  L'ï',  L'ñ',  L'ó',  L'ò',  L'ô',  L'ö',  L'õ',  L'ú',  L'ù',  L'û',  L'ü', 
	L'†',  L'°',  L'¢',  L'£',  L'§',  L'•',  L'¶',  L'ß',  L'®',  L'©',  L'™',  L'´',  L'¨',  L'≠',  L'Æ',  L'Ø', 
	L'∞',  L'±',  L'≤',  L'≥',  L'¥',  L'µ',  L'∂',  L'∑',  L'∏',  L'π',  L'∫',  L'ª',  L'º',  L'Ω',  L'æ',  L'ø', 
	L'‡',  L'·',  L'‚',  L'„',  L'‰',  L'Â',  L'Ê',  L'Á',  L'Ë',  L'È',  L'Í',  L'Î',  L'Ï',  L'Ì',  L'Ó',  L'Ô',
	L'',  L'Ò',  L'Ú',  L'Û',  L'Ù',  L'ı',  L'ˆ',  L'◊',  L'¯',  L'˘',  L'˙',  L'˚',  L'¸',  L'˝',  L'ﬁ',  L'ﬂ',
	L'‡',  L'·',  L'‚',  L'„',  L'‰',  L'Â',  L'Ê',  L'Á',  L'Ë',  L'È',  L'Í',  L'Î',  L'Ï',  L'Ì',  L'Ó',  L'Ô',
	L'',  L'Ò',  L'Ú',  L'Û',  L'Ù',  L'ı',  L'ˆ',  L'˜',  L'¯',  L'˘',  L'˙',  L'˚',  L'¸',  L'˝',  L'˛',  L'ˇ',	/*- mm 020219 -*/
#endif	/*- End mm 030107 insertion -*/
#else
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
#endif
};
#endif /* _MSL_C_LOCALE_ONLY */

/* begin addition */ /*- mm 011130 -*/	
const wchar_t __wlower_mapC[__msl_wcmap_size] = 
{
/*   -0      -1      -2      -3      -4      -5      -6      -7      -8      -9      -A      -B      -C      -D      -E      -F       */
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	L' ',  L'!',  L'"',  L'#',  L'$',  L'%',  L'&',  L'\'', L'(',  L')',  L'*',  L'+',  L',',  L'-',  L'.',  L'/',					/*- mm 020219 -*/		
	L'0',  L'1',  L'2',  L'3',  L'4',  L'5',  L'6',  L'7',  L'8',  L'9',  L':',  L';',  L'<',  L'=',  L'>',  L'?',					/*- mm 020219 -*/
	L'@',  L'a',  L'b',  L'c',  L'd',  L'e',  L'f',  L'g',  L'h',  L'i',  L'j',  L'k',  L'l',  L'm',  L'n',  L'o',					/*- mm 020219 -*/
	L'p',  L'q',  L'r',  L's',  L't',  L'u',  L'v',  L'w',  L'x',  L'y',  L'z',  L'[',  L'\\', L']',  L'^',  L'_',					/*- mm 020219 -*/
	L'`',  L'a',  L'b',  L'c',  L'd',  L'e',  L'f',  L'g',  L'h',  L'i',  L'j',  L'k',  L'l',  L'm',  L'n',  L'o',
	L'p',  L'q',  L'r',  L's',  L't',  L'u',  L'v',  L'w',  L'x',  L'y',  L'z',  L'{',  L'|',  L'}',  L'~',  0x007F,
#if !_MSL_C_LOCALE_ONLY
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
#endif /* _MSL_C_LOCALE_ONLY */
};
/* end addition */ /*- mm 011130 -*/	

#if !_MSL_C_LOCALE_ONLY
const wchar_t __wupper_map[__msl_wcmap_size] = 
{
/*   -0      -1      -2      -3      -4      -5      -6      -7      -8      -9      -A      -B      -C      -D      -E      -F       */
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	L' ',  L'!',  L'"',  L'#',  L'$',  L'%',  L'&',  L'\'', L'(',  L')',  L'*',  L'+',  L',',  L'-',  L'.',  L'/',					/*- mm 020219 -*/
	L'0',  L'1',  L'2',  L'3',  L'4',  L'5',  L'6',  L'7',  L'8',  L'9',  L':',  L';',  L'<',  L'=',  L'>',  L'?',					/*- mm 020219 -*/
	L'@',  L'A',  L'B',  L'C',  L'D',  L'E',  L'F',  L'G',  L'H',  L'I',  L'J',  L'K',  L'L',  L'M',  L'N',  L'O',					/*- mm 020219 -*/
	L'P',  L'Q',  L'R',  L'S',  L'T',  L'U',  L'V',  L'W',  L'X',  L'Y',  L'Z',  L'[',  L'\\', L']',  L'^',  L'_',					/*- mm 020219 -*/
	L'`',  L'A',  L'B',  L'C',  L'D',  L'E',  L'F',  L'G',  L'H',  L'I',  L'J',  L'K',  L'L',  L'M',  L'N',  L'O',
	L'P',  L'Q',  L'R',  L'S',  L'T',  L'U',  L'V',  L'W',  L'X',  L'Y',  L'Z',  L'{',  L'|',  L'}',  L'~',  0x007F,
#if __dest_os == __mac_os || __dest_os == __mac_os_x	/*- Begin mm 030107 insertion -*/
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x00E7, 0x00CB, 0x00E5, 0x0080, 0x00CC, 0x0081, 0x0082, 0x0083, 0x00E9, 
	0x00E6, 0x00E8, 0x00EA, 0x00ED, 0x00EB, 0x00EC, 0x0084, 0x00EE, 0x00F1, 0x00EF, 0x0085, 0x00CD, 0x00F2, 0x00F4, 0x00F3, 0x0086, 
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00AE, 0x00AF, 
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CE, 
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D9, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF, 

#if 0  /* following is a Mac translation of the above table */
	L'Ä',  L'Å',  L'Ç',  L'É',  L'Ñ',  L'Ö',  L'Ü',  L'Á',  L'À',  L'Â',  L'Ä',  L'Ã',  L'Å',  L'Ç',  L'É',  L'È',
	L'Ê',  L'Ë',  L'Í',  L'Ì',  L'Î',  L'Ï',  L'Ñ',  L'Ó',  L'Ò',  L'Ô',  L'Ö',  L'Õ',  L'Ú',  L'Ù',  L'Û',  L'Ü',  /*- mm 970922 -*/
	L'†',  L'°',  L'¢',  L'£',  L'§',  L'•',  L'¶',  L'ß',  L'®',  L'©',  L'™',  L'´',  L'¨',  L'≠',  L'Æ',  L'Ø',
	L'∞',  L'±',  L'≤',  L'≥',  L'¥',  L'µ',  L'∂',  L'∑',  L'∏',  L'π',  L'∫',  L'ª',  L'º',  L'Ω',  L'Æ',  L'Ø',
	L'¿',  L'¡',  L'¬',  L'√',  L'ƒ',  L'≈',  L'∆',  L'«',  L'»',  L'…',  L' ',  L'À',  L'Ã',  L'Õ',  L'Œ',  L'Œ',
	L'–',  L'—',  L'“',  L'”',  L'‘',  L'’',  L'÷',  L'◊',  L'Ÿ',  L'Ÿ',  L'⁄',  L'€',  L'‹',  L'›',  L'ﬁ',  L'ﬂ',					/*- mm 020219 -*/
	L'‡',  L'·',  L'‚',  L'„',  L'‰',  L'Â',  L'Ê',  L'Á',  L'Ë',  L'È',  L'Í',  L'Î',  L'Ï',  L'Ì',  L'Ó',  L'Ô',					/*- mm 020219 -*/
	L'',  L'Ò',  L'Ú',  L'Û',  L'Ù',  L'ı',  L'ˆ',  L'˜',  L'¯',  L'˘',  L'˙',  L'˚',  L'¸',  L'˝',  L'˛',  L'ˇ'					/*- mm 980629 -*/   
#endif
#elif __dest_os == __win32_os || __dest_os == __wince_os
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F, 
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00F7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00FE, 0x00FF, 

#if 0  /* The following is a Windows translation of the above table */
	L'Ä',  L'Å',  L'Ç',  L'É',  L'Ñ',  L'Ö',  L'Ü',  L'á',  L'à',  L'â',  L'ä',  L'ã',  L'å',  L'ç',  L'é',  L'è', 					/*- mm 020219 -*/
	L'ê',  L'ë',  L'í',  L'ì',  L'î',  L'ï',  L'ñ',  L'ó',  L'ò',  L'ô',  L'ö',  L'õ',  L'ú',  L'ù',  L'û',  L'ü', 					/*- mm 020219 -*/
	L'†',  L'°',  L'¢',  L'£',  L'§',  L'•',  L'¶',  L'ß',  L'®',  L'©',  L'™',  L'´',  L'¨',  L'≠',  L'Æ',  L'Ø', 					/*- mm 020219 -*/
	L'∞',  L'±',  L'≤',  L'≥',  L'¥',  L'µ',  L'∂',  L'∑',  L'∏',  L'π',  L'∫',  L'ª',  L'º',  L'Ω',  L'æ',  L'ø',					/*- mm 020219 -*/
	L'¿',  L'¡',  L'¬',  L'√',  L'ƒ',  L'≈',  L'∆',  L'«',  L'»',  L'…',  L' ',  L'À',  L'Ã',  L'Õ',  L'Œ',  L'œ',					/*- mm 020219 -*/
	L'–',  L'—',  L'“',  L'”',  L'‘',  L'’',  L'÷',  L'◊',  L'ÿ',  L'Ÿ',  L'⁄',  L'€',  L'‹',  L'›',  L'ﬁ',  L'ﬂ',					/*- mm 020219 -*/
	L'¿',  L'¡',  L'¬',  L'√',  L'ƒ',  L'≈',  L'∆',  L'«',  L'»',  L'…',  L' ',  L'À',  L'Ã',  L'Õ',  L'Œ',  L'œ',					/*- mm 020219 -*/
	L'–',  L'—',  L'“',  L'”',  L'‘',  L'’',  L'÷',  L'˜',  L'ÿ',  L'Ÿ',  L'⁄',  L'€',  L'‹',  L'›',  L'˛',  L'ˇ'					/*- mm 020219 -*/
#endif	/*- End mm 030107 insertion -*/
#else
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
#endif
};
#endif /* _MSL_C_LOCALE_ONLY */


/* begin addition */ /*- mm 011130 -*/	
const wchar_t __wupper_mapC[__msl_wcmap_size] = 
{
/*   -0      -1      -2      -3      -4      -5      -6      -7      -8      -9      -A      -B      -C      -D      -E      -F       */
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	L' ',  L'!',  L'"',  L'#',  L'$',  L'%',  L'&',  L'\'',  L'(',  L')',  L'*',  L'+',  L',',  L'-',  L'.',  L'/',					/*- mm 020219 -*/
	L'0',  L'1',  L'2',  L'3',  L'4',  L'5',  L'6',  L'7',  L'8',  L'9',  L':',  L';',  L'<',  L'=',  L'>',  L'?',					/*- mm 020219 -*/
	L'@',  L'A',  L'B',  L'C',  L'D',  L'E',  L'F',  L'G',  L'H',  L'I',  L'J',  L'K',  L'L',  L'M',  L'N',  L'O',					/*- mm 020219 -*/
	L'P',  L'Q',  L'R',  L'S',  L'T',  L'U',  L'V',  L'W',  L'X',  L'Y',  L'Z',  L'[',  L'\\',  L']',  L'^',  L'_',					/*- mm 020219 -*/
	L'`',  L'A',  L'B',  L'C',  L'D',  L'E',  L'F',  L'G',  L'H',  L'I',  L'J',  L'K',  L'L',  L'M',  L'N',  L'O',
	L'P',  L'Q',  L'R',  L'S',  L'T',  L'U',  L'V',  L'W',  L'X',  L'Y',  L'Z',  L'{',  L'|',  L'}',  L'~',  0x007F,
#if !_MSL_C_LOCALE_ONLY
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
#endif /* _MSL_C_LOCALE_ONLY */
};
/* end addition */ /*- mm 011130 -*/	

#ifndef __cplusplus
	int _MSL_CDECL 		wctob(wint_t c) { return((c & 0xFF00) ? (int)EOF : (int)c);}  	/*- mm 980625 -*/
	wint_t _MSL_CDECL 	btowc(int c){ return((c & 0x8000) ? WEOF : (wint_t)c);}	/*- mm 010529 -*/ /*- ejs 030417 -*/
#endif

/*- mm 980629 -*/
wctype_t _MSL_CDECL wctype(const char *property)
{
	if (strcmp(property, "alnum") == 0)
		return(__msl_walnum);
	if (strcmp(property, "alpha") == 0)
		return(__msl_walpha);
	if (strcmp(property, "cntrl") == 0)
		return(__msl_wcntrl);
	if (strcmp(property, "digit") == 0)
		return(__msl_wdigit);
	if (strcmp(property, "graph") == 0)
		return(__msl_wgraph);
	if (strcmp(property, "lower") == 0)
		return(__msl_wlower);
	if (strcmp(property, "print") == 0)
		return(__msl_wprint);
	if (strcmp(property, "punct") == 0)
		return(__msl_wpunct);
	if (strcmp(property, "space") == 0)
		return(__msl_wspace);
	if (strcmp(property, "upper") == 0)
		return(__msl_wupper);
	if (strcmp(property, "xdigit") == 0)
		return(__msl_wxdigit);
	return(0);   /* if all else fails */
}

int _MSL_CDECL iswctype(wint_t wc, wctype_t desc)
{
	if ((wc >= 0) && (wc < __msl_wcmap_size))
		return (int)(_MSL_CWMAP_ACCESS[wc] & desc);
	else
		return 0;
}
/* end mm 980629a */
#endif /* #if _MSL_WIDE_CHAR */				/*- mm 981030 -*/

/* Change record:
 * JCM 980121 First code release
 * mm  980625 Defined wctob.
 * mm  980626 Considerable rewrite of the wc classification functions, towlower and towupper to 
 *            match C9x
 * mm  980629 Implemented wctype() and iswctype()
 * mm  980629 Correction to __wupper_map and __wlower_map
 * mm  981030 Added #if _MSL_WIDE_CHAR wrappers
 * mm  000201 Added #include of cwchar now that prototype is in cwchar to accord with C Standard
 * cc  000326 removed dest_os to be_os
 * mm  010529 Added btowc()
 * mm  011130 As part of locale implementation, added wctype_mapC, wupper_mapC, and wlower_mapC
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * mm  020118 Corrected implementation of iswctype() for new locale
 * mm  020219 Corrected implementation of tables for windows locales
 * JWW 020226 Changed the tab character to be both a motion and a space
 * mm  020228 More corrections of tables, this time for Mac locale
 * JWW 020305 Changed classification tables to have more bits for improved typing
 * JWW 020308 Recognize __mac_os_x in unnamed "" locale tables and fixed classification for 0xFF
 * cc  020521 Added const keyword for platforms with limited RAM space
 * blc 020927 Fixed implicit conversion errors
 * cc  021001 Added support for _MSL_C_LOCALE_ONLY
 * cc  021022 Added _MSL_CWMAP_ACCESS macro to iswctype
 * mm  030107 Changed form of tables so to avoid misinterpretation by 3.1 compiler
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030305 Added _MSL_C99 wrapper
 * ejs 030523 btowc() uses int, not char type
 */