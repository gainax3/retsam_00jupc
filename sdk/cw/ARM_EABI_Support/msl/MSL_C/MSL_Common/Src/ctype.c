/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:46 $
 * $Revision: 1.31.2.2 $
 */

#include <ansi_parms.h>

#undef	_MSL_USE_INLINE
#define	_MSL_USE_INLINE 1
#undef	_MSL_INLINE
#define	_MSL_INLINE _MSL_DO_NOT_INLINE

#include <ctype.h>
#include <ctype_api.h>

/* ----- Table for Pro 7 and earlier (do not use in new code!) ----- */
#if __dest_os != __mac_os_x /* Backwards compatibility not needed on Mach-O */
#define octrl	0x01
#define omotn	0x02
#define ospac	0x04
#define opunc	0x08
#define odigi	0x10
#define ohexd	0x20
#define olowc	0x40
#define ouppc	0x80
#define odhex	ohexd | odigi
#define ouhex	ohexd | ouppc
#define olhex	ohexd | olowc

#if !_MSL_C_LOCALE_ONLY	
_MSL_IMP_EXP_C const unsigned char __ctype_map[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, omotn, omotn, omotn, omotn, omotn, octrl, octrl, /* 0- */
	octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, /* 1- */
	ospac, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, /* 2- */
	odhex, odhex, odhex, odhex, odhex, odhex, odhex, odhex, odhex, odhex, opunc, opunc, opunc, opunc, opunc, opunc, /* 3- */
	opunc, ouhex, ouhex, ouhex, ouhex, ouhex, ouhex, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, /* 4- */
	ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, opunc, opunc, opunc, opunc, opunc, /* 5- */
	opunc, olhex, olhex, olhex, olhex, olhex, olhex, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, /* 6- */
	olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, opunc, opunc, opunc, opunc, octrl  /* 7- */
#if __dest_os == __mac_os
	,
	ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, /* 8- */
	olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, /* 9- */
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, olowc, opunc, opunc, opunc, opunc, opunc, opunc, ouppc, ouppc, /* A- */
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, olowc, olowc, /* B- */
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, ospac, ouppc, ouppc, ouppc, ouppc, olowc, /* C- */ /*- mm 980603 -*/
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, olowc, ouppc, opunc, opunc, opunc, opunc, olowc, olowc, /* D- */
	opunc, opunc, opunc, opunc, opunc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, /* E- */
	opunc, ouppc, ouppc, ouppc, ouppc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, 000    /* F- */
#elif __dest_os == __win32_os || __dest_os == __wince_os								/* begin insert */	 /*- mm 020122 -*/
	,
	octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, /* 8- */
	octrl, octrl, opunc, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, octrl, /* 9- */
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, /* A- */
	opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, opunc, /* B- */
	ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, /* C- */ 
	ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, opunc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, ouppc, olowc, /* D- */
	olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, /* E- */
	olowc, olowc, olowc, olowc, olowc, olowc, olowc, opunc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, olowc, /* F- */
																						/* end insert */	 /*- mm 020122 -*/
#endif                           /*- mm 970708 -*/
};
#endif /* _MSL_C_LOCALE_ONLY */

#endif /* __dest_os != __mac_os_x */
/* ----------------------------------------------------------------- */

#define alph	__msl_alpha
#define blnk	__msl_blank
#define cntl	__msl_cntrl
#define digi	__msl_digit | __msl_graph | __msl_print
#define grph	__msl_graph
#define lowc	__msl_lower | __msl_alpha | __msl_graph | __msl_print
#define prnt	__msl_print
#define punc	__msl_punct | __msl_graph | __msl_print
#define spac	__msl_space
#define uppc	__msl_upper | __msl_alpha | __msl_graph | __msl_print
#define hexd	__msl_xdigit
#define dhex	hexd | digi
#define uhex	hexd | uppc
#define lhex	hexd | lowc
#define ctbl	cntl | blnk
#define ctsp	cntl | spac
#define sblp	spac | blnk | prnt
#define csbl	cntl | spac | blnk

#if !_MSL_C_LOCALE_ONLY
const unsigned short	__msl_ctype_map[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, csbl, ctsp, ctsp, ctsp, ctsp, cntl, cntl, /* 0- */
	cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, /* 1- */
	sblp, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, /* 2- */
	dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, punc, punc, punc, punc, punc, punc, /* 3- */
	punc, uhex, uhex, uhex, uhex, uhex, uhex, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, /* 4- */
	uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, punc, punc, punc, punc, punc, /* 5- */
	punc, lhex, lhex, lhex, lhex, lhex, lhex, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* 6- */
	lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, punc, punc, punc, punc, cntl  /* 7- */
#if __dest_os == __mac_os || __dest_os == __mac_os_x
	,
	uppc, uppc, uppc, uppc, uppc, uppc, uppc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* 8- */
	lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* 9- */
	punc, punc, punc, punc, punc, punc, punc, lowc, punc, punc, punc, punc, punc, punc, uppc, uppc, /* A- */
	punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, lowc, lowc, /* B- */
	punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, sblp, uppc, uppc, uppc, uppc, lowc, /* C- */ /*- mm 980603 -*/
	punc, punc, punc, punc, punc, punc, punc, punc, lowc, uppc, punc, punc, punc, punc, lowc, lowc, /* D- */
	punc, punc, punc, punc, punc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, /* E- */
	punc, uppc, uppc, uppc, uppc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc  /* F- */
#elif __dest_os == __win32_os || __dest_os == __wince_os								/* begin insert */	 /*- mm 020122 -*/
	,
	cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, /* 8- */
	cntl, cntl, punc, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, /* 9- */
	punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, /* A- */
	punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, /* B- */
	uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, /* C- */ 
	uppc, uppc, uppc, uppc, uppc, uppc, uppc, punc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, lowc, /* D- */
	lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* E- */
	lowc, lowc, lowc, lowc, lowc, lowc, lowc, punc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* F- */
																						/* end insert */	 /*- mm 020122 -*/
#endif                           /*- mm 970708 -*/
};

#endif /* _MSL_C_LOCALE_ONLY */

/* begin mm 011130 */
const unsigned short	__ctype_mapC[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, csbl, ctsp, ctsp, ctsp, ctsp, cntl, cntl, /* 0- */
	cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, cntl, /* 1- */
	sblp, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, punc, /* 2- */
	dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, dhex, punc, punc, punc, punc, punc, punc, /* 3- */
	punc, uhex, uhex, uhex, uhex, uhex, uhex, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, /* 4- */
	uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, uppc, punc, punc, punc, punc, punc, /* 5- */
	punc, lhex, lhex, lhex, lhex, lhex, lhex, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, /* 6- */
	lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, lowc, punc, punc, punc, punc, cntl  /* 7- */
};
/* end mm 011130 */

#if !_MSL_C_LOCALE_ONLY
const unsigned char __lower_map[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,/* 0- */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,/* 1- */
	' ',  '!',  '"',  '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', /* 2- */
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', /* 3- */
	'@',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o', /* 4- */
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '[',  '\\', ']',  '^',  '_', /* 5- */
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o', /* 6- */
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0x7F,/* 7- */
#if __dest_os == __mac_os || __dest_os == __mac_os_x  /*- Begin mm 030107 insertion -*/
	0x8A, 0x8C, 0x8D, 0x8E, 0x96, 0x9A, 0x9F, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, /* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, /* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xBE, 0xBF, /* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, /* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0x88, 0x8B, 0x9B, 0xCF, 0xCF, /* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD8, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, /* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0x89, 0x90, 0x87, 0x91, 0x8F, 0x92, 0x94, 0x95, 0x93, 0x97, 0x99, /* E- */
	0xF0, 0x98, 0x9C, 0x9E, 0x9D, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, /* F- */

#if 0   /* The following is a translation of the above table */
	'ä',  'å',  'ç',  'é',  'ñ',  'ö',  'ü',  'á',  'à',  'â',  'ä',  'ã',  'å',  'ç',  'é',  'è', /* 8- */
	'ê',  'ë',  'í',  'ì',  'î',  'ï',  'ñ',  'ó',  'ò',  'ô',  'ö',  'õ',  'ú',  'ù',  'û',  'ü', /* 9- */
	'†',  '°',  '¢',  '£',  '§',  '•',  '¶',  'ß',  '®',  '©',  '™',  '´',  '¨',  '≠',  'æ',  'ø', /* A- */
	'∞',  '±',  '≤',  '≥',  '¥',  'µ',  '∂',  '∑',  '∏',  'π',  '∫',  'ª',  'º',  'Ω',  'æ',  'ø', /* B- */
	'¿',  '¡',  '¬',  '√',  'ƒ',  '≈',  '∆',  '«',  '»',  '…',  ' ',  'à',  'ã',  'õ',  'œ',  'œ', /* C- */
	'–',  '—',  '“',  '”',  '‘',  '’',  '÷',  '◊',  'ÿ',  'ÿ',  '⁄',  '€',  '‹',  '›',  'ﬁ',  'ﬂ', /* D- */
	'‡',  '·',  '‚',  '„',  '‰',  'â',  'ê',  'á',  'ë',  'è',  'í',  'î',  'ï',  'ì',  'ó',  'ô', /* E- */
	'',  'ò',  'ú',  'û',  'ù',  'ı',  'ˆ',  '˜',  '¯',  '˘',  '˙',  '˚',  '¸',  '˝',  '˛',  'ˇ'  /* F- */
#endif		/*- End mm 030107 insertion -*/
#elif __dest_os == __win32_os || __dest_os == __wince_os								/* begin insert */	 /*- mm 020122 -*/
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* C- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xD7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xDE, 0xDF,/* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF /* F- */
#else
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,/* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF /* F- */
#endif																					/* end insert */	 /*- mm 020122 -*/
};

#endif /* _MSL_C_LOCALE_ONLY */

/* begin mm 011130 */
const unsigned char __lower_mapC[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,/* 0- */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,/* 1- */
	' ',  '!',  '"',  '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', /* 2- */
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', /* 3- */
	'@',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o', /* 4- */
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '[',  '\\', ']',  '^',  '_', /* 5- */
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o', /* 6- */
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0x7F,/* 7- */
#if !_MSL_C_LOCALE_ONLY	
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,/* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF /* F- */
#endif /* _MSL_C_LOCALE_ONLY */

};
/* end mm 011130 */

#if !_MSL_C_LOCALE_ONLY	
const unsigned char __upper_map[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,/* 0- */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,/* 1- */
	' ',  '!',  '"',  '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', /* 2- */
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', /* 3- */
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', /* 4- */
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\', ']',  '^',  '_', /* 5- */
	'`',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', /* 6- */
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '{',  '|',  '}',  '~',  0x7F,/* 7- */
#if __dest_os == __mac_os || __dest_os == __mac_os_x		/*- Begin mm 030107 insertion -*/
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0xE7, 0xCB, 0xE5, 0x80, 0xCC, 0x81, 0x82, 0x83, 0xE9, /* 8- */
	0xE6, 0xE8, 0xEA, 0xED, 0xEB, 0xEC, 0x84, 0xEE, 0xF1, 0xEF, 0x85, 0xCD, 0xF2, 0xF4, 0xF3, 0x86, /* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, /* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xAE, 0xAF, /* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCE, /* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD9, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, /* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, /* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, /* F- */

#if 0   /* The following is a translation of the above table */
	'Ä',  'Å',  'Ç',  'É',  'Ñ',  'Ö',  'Ü',  'Á',  'À',  'Â',  'Ä',  'Ã',  'Å',  'Ç',  'É',  'È', /* 8- */
	'Ê',  'Ë',  'Í',  'Ì',  'Î',  'Ï',  'Ñ',  'Ó',  'Ò',  'Ô',  'Ö',  'Õ',  'Ú',  'Ù',  'Û',  'Ü', /* 9- */  /*- mm 970922 -*/
	'†',  '°',  '¢',  '£',  '§',  '•',  '¶',  'ß',  '®',  '©',  '™',  '´',  '¨',  '≠',  'Æ',  'Ø', /* A- */
	'∞',  '±',  '≤',  '≥',  '¥',  'µ',  '∂',  '∑',  '∏',  'π',  '∫',  'ª',  'º',  'Ω',  'Æ',  'Ø', /* B- */
	'¿',  '¡',  '¬',  '√',  'ƒ',  '≈',  '∆',  '«',  '»',  '…',  ' ',  'À',  'Ã',  'Õ',  'Œ',  'Œ', /* C- */
	'–',  '—',  '“',  '”',  '‘',  '’',  '÷',  '◊',  'Ÿ',  'Ÿ',  '⁄',  '€',  '‹',  '›',  'ﬁ',  'ﬂ', /* D- */
	'‡',  '·',  '‚',  '„',  '‰',  'Â',  'Ê',  'Á',  'Ë',  'È',  'Í',  'Î',  'Ï',  'Ì',  'Ó',  'Ô', /* E- */
	'',  'Ò',  'Ú',  'Û',  'Ù',  'ı',  'ˆ',  '˜',  '¯',  '˘',  '˙',  '˚',  '¸',  '˝',  '˛',  'ˇ'  /* F- */
#endif			/*- End mm 030107 insertion -*/
#elif __dest_os == __win32_os || __dest_os == __wince_os								/* begin insert */	 /*- mm 020122 -*/
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,/* D- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* E- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xF7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xFE, 0xFF /* F- */
#else
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,/* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF /* F- */
#endif																					/* end insert */	 /*- mm 020122 -*/
};

#endif /* _MSL_C_LOCALE_ONLY */

/* begin mm 011130 */
const unsigned char __upper_mapC[__msl_cmap_size] = {
/*   -0    -1    -2    -3    -4    -5    -6    -7    -8    -9    -A    -B    -C    -D    -E    -F       */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,/* 0- */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,/* 1- */
	' ',  '!',  '"',  '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', /* 2- */
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', /* 3- */
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', /* 4- */
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\', ']',  '^',  '_', /* 5- */
	'`',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', /* 6- */
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '{',  '|',  '}',  '~',  0x7F,/* 7- */
#if !_MSL_C_LOCALE_ONLY	
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,/* 8- */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,/* 9- */
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,/* A- */
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,/* B- */
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,/* C- */
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,/* D- */
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,/* E- */
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF /* F- */
#endif /* _MSL_C_LOCALE_ONLY */
};
/* end mm 011130 */


/* Change record:
 * JFH 950607 First code release.
 * JFH 951215 Added definitions for Mac's upper-128 characters - for compatibility
 *			  with old libs.
 * JFH 960122 Bracketed isXXX(), toupper(), and tolower() by #ifndef __cplusplus
 *			  (they are inlined in <ctype.h>).
 * JFH 960213 Changed the __ctype_map entries for 'ﬁ' (ligature fi) and 'ﬂ' (ligature fl) (0xDE and 0xDF) to lowc.
 * JFH 960221 Changed the __ctype_map entry for 'ß' (0xA7) to lowc also.
 * mani970101 Make isprint() do the right thing for characters greater than 0x7f.
 * mm  970708 Inserted Be changes
 * mm  970922 Corrected lowercase to uppercase mapping.
 * mm  970925 Added tolower and toupper to allow for detection of EOF character
 * hh  980205 removed __cplusplus
 * MM  980603 Changed __ctype_map so that non-line-breaking space 0xCA is space and not punct.  MW07352
 * mf  000322 Definitions of all of these are now inlined inside cctype header
 * cc  000326 Removed dest_os to be_os 
 * cc  010919 Added define for _INLINE to make functions available at link time
 * mm  011130 Added maps for C locale
 * mm  020122 Added maps for extended character locales for Windows
 * JWW 020226 Changed the tab character to be both a motion and a space
 * JWW 020305 Changed classification tables to have more bits for improved typing
 * JWW 020308 Recognize __mac_os_x in unnamed "" locale tables and fixed classification for 0xFF
 * cc  020521 Added const keyword for platforms with limited RAM space
 * cc  021001 Added support for _MSL_C_LOCALE_ONLY
 * mm  030107 Changed form of tables so to avoid misinterpretation by 3.1 compiler
 */