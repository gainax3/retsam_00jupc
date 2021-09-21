/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:26 $
 * $Revision: 1.62.2.2 $
 */

/*
 *	Routines
 *	--------
 *		setlocale
 *		localeconv
 */

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>											/*- mm 020606 -*/
#include <cwctype>											/*- mm 011130 -*/
#include <cstdlib>											/*- mm 011130 -*/
#include <mbstring.h>										/*- mm 011130 -*/
#include <ctype_api.h>
#include <wctype_api.h>
#include <locale_api.h>
#include <ctype.h>											/*- mm 020606 -*/
#include <critical_regions.h>
#include <msl_thread_local_data.h>

#if !_MSL_C_LOCALE_ONLY

struct lconv  __lconv =
{
	"."			 /* decimal_point		*/,
	""			 /* thousands_sep		*/,
	""			 /* grouping			*/,										/*- mm 020606 -*/
	""			 /* mon_decimal_point	*/,
	""			 /* mon_thousands_sep	*/,
	""			 /* mon_grouping		*/,										/*- mm 020606 -*/
	""			 /* positive_sign		*/,
	""			 /* negative_sign		*/,
	""			 /* currency_symbol		*/,
	CHAR_MAX	 /* frac_digits			*/,
	CHAR_MAX	 /* p_cs_precedes		*/,
	CHAR_MAX	 /* n_cs_precedes		*/,
	CHAR_MAX	 /* p_sep_by_space		*/,
	CHAR_MAX	 /* n_sep_by_space		*/,
	CHAR_MAX	 /* p_sign_posn			*/,
	CHAR_MAX	 /* n_sign_posn			*/,
	""			 /* int_curr_symbol		*/,
	CHAR_MAX	 /* int_frac_digits		*/,
#if _MSL_C99											/*- mm 030528 -*/
	CHAR_MAX 	 /* int_p_cs_precedes	*/, 
	CHAR_MAX 	 /* int_n_cs_precedes	*/,
	CHAR_MAX 	 /* int_p_sep_by_space	*/,
	CHAR_MAX 	 /* int_n_sep_by_space	*/, 
	CHAR_MAX 	 /* int_p_sign_posn		*/,
	CHAR_MAX 	 /* int_n_sign_posn		*/
#endif /* _MSL_C99 */								/*- mm 030528 -*/
};

#endif /*!_MSL_C_LOCALE_ONLY */

#if _MSL_C_LOCALE_ONLY

#if _MSL_WIDE_CHAR
struct _loc_ctype_cmpt _loc_ctyp_C =
{
	__mbtowc_noconv,
	__wctomb_noconv
};
#endif /* _MSL_WIDE_CHAR */

#else

struct _loc_ctype_cmpt _loc_ctyp_C =
{
	"C",
	(unsigned short *)&__ctype_mapC[0],
	(unsigned char *)&__upper_mapC[0],
	(unsigned char *)&__lower_mapC[0],
#if _MSL_WIDE_CHAR
#if _MSL_C99											/*- mm 030528 -*/
	(unsigned short *)&__wctype_mapC[0],
	(wchar_t *)&__wupper_mapC[0],
	(wchar_t *)&__wlower_mapC[0],
#endif	/* _MSL_C99 */								/*- mm 030321 -*/
	__mbtowc_noconv,
	__wctomb_noconv
#endif /* _MSL_WIDE_CHAR */
};

struct _loc_ctype_cmpt _loc_ctyp_I =
{
	"",												/*- mm 020108 -*/
	(unsigned short *)&__msl_ctype_map[0],
	(unsigned char *)&__upper_map[0],
	(unsigned char *)&__lower_map[0],
#if _MSL_WIDE_CHAR
#if _MSL_C99											/*- mm 030528 -*/
	(unsigned short *)&__msl_wctype_map[0],
	(wchar_t *)&__wupper_map[0],
	(wchar_t *)&__wlower_map[0],
#endif /* _MSL_C99 */
	__mbtowc_noconv,
	__wctomb_noconv
#endif /* _MSL_WIDE_CHAR */
};

struct _loc_ctype_cmpt _loc_ctyp_C_UTF_8 =
{
	"C-UTF-8",										/*- mm 020108 -*/
	(unsigned short *)&__ctype_mapC[0],
	(unsigned char *)&__upper_mapC[0],
	(unsigned char *)&__lower_mapC[0],
#if _MSL_WIDE_CHAR
#if _MSL_C99											/*- mm 030528 -*/
	(unsigned short *)&__wctype_mapC[0],
	(wchar_t *)&__wupper_mapC[0],
	(wchar_t *)&__wlower_mapC[0],
#endif /* _MSL_C99 */
	__utf8_to_unicode,
	__unicode_to_UTF8
#endif /* _MSL_WIDE_CHAR */
};

#endif /* _MSL_C_LOCALE_ONLY */

unsigned short char_coll_tableC[0x60] = /* begin rewrite of C locale collate table */  			/*- 040525 -*/
            /* The collation order resulting from this table is:											*/
            /* !"#$%&'()*+,-./:;<=>?@[\]^_{|}~0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz*/

/*         				                      Second digit															*/
/*					0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F     */
/* First digit																										*/
/*      2         space   !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /     */
				 {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
/*		3           0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?     */
                  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
/*		4           @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O     */
                  0x17, 0x2b, 0x2d, 0x2f, 0x31, 0x33, 0x35, 0x37, 0x39, 0x3b, 0x3d, 0x3f, 0x41, 0x43, 0x45, 0x47,
/*		5           P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
                  0x49, 0x4b, 0x4d, 0x4f, 0x51, 0x53, 0x55, 0x57, 0x59, 0x5b, 0x5d, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
/*		6           `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o     */
                  0x00, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48,
/*		7           p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~    del    */
                  0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x1d, 0x1e, 0x1f, 0x20};
/* end rewrite of C locale collate table */      /*- 040525 -*/

struct _loc_coll_cmpt _loc_coll_C =
{
#if !_MSL_C_LOCALE_ONLY
	"C",    								/* component name     	*/ 	/*- mm 020718 -*/
#endif
	0x20,									/* char_start_value  	*/ 	/*- mm 020718 -*/	/*- 040525 -*/
	0x6e,									/* char_coll_tab_size	*/ 	/*- mm 020718 -*/   /*- 040525 -*/
	0,                                      /* char_spec_accents    */ 	/*- mm 020718 -*/
	(unsigned short*)&char_coll_tableC[0],	/* char_coll_table_ptr	*/ 	/*- mm 020718 -*/
#if !_MSL_C_LOCALE_ONLY	
	NULL    								/* wchar_coll_seq_ptr 	*/ 	/*- mm 020718 -*/
#endif
};

#if !_MSL_C_LOCALE_ONLY
struct _loc_mon_cmpt _loc_mon_C =
{
	"C",
	"",  		/* mon_decimal_point	*/
	"",  		/* mon_thousands_sep	*/ 
	"",  		/* mon_grouping			*/
	"",  		/* positive_sign		*/
	"",  		/* negative_sign		*/ 
	"",			/* currency_symbol		*/
	CHAR_MAX,	/* frac_digits			*/ 
	CHAR_MAX,	/* p_cs_precedes		*/ 
	CHAR_MAX,	/* n_cs_precedes		*/  
	CHAR_MAX,	/* p_sep_by_space		*/  
	CHAR_MAX,	/* n_sep_by_space		*/  
	CHAR_MAX,	/* p_sign_posn			*/  
	CHAR_MAX,	/* n_sign_posn			*/  
	"",			/* int_curr_symbol		*/ 
	CHAR_MAX,	/* int_frac_digits		*/ 
	CHAR_MAX,	/* int_p_cs_precedes	*/  
	CHAR_MAX,	/* int_n_cs_precedes	*/ 
	CHAR_MAX,	/* int_p_sep_by_space	*/ 
	CHAR_MAX,	/* int_n_sep_by_space	*/ 
	CHAR_MAX,	/* int_p_sign_posn		*/ 
	CHAR_MAX	/* int_n_sign_posn		*/ 
};

struct _loc_num_cmpt _loc_num_C =
{
	"C",
	".", 		/* decimal_point	*/
	"",			/* thousands_sep	*/
	""			/* grouping			*/
};
#endif /* _MSL_C_LOCALE_ONLY */

struct _loc_time_cmpt  _loc_tim_C =
{
#if !_MSL_C_LOCALE_ONLY
	"C",
#endif /* _MSL_C_LOCALE_ONLY */
	"AM|PM",						/*  am_pm			*/ /*- mm 021204 -*/
	"%a %b %e %T %Y",					/*- mm 020606 -*/  /*- mm 021204 -*/
	"%I:%M:%S %p",						/*- mm 020606 -*/
	"%m/%d/%y",							/*- mm 020606 -*/  /*- mm 021204 -*/
	"%T",       												/*  datetime_fmts in the order %c|%r|%x|%X */
	"Sun|Sunday|Mon|Monday|Tue|Tuesday|Wed|Wednesday"
					"|Thu|Thursday|Fri|Friday|Sat|Saturday",	/*  day_names 		*/
	"Jan|January|Feb|February|Mar|March"
       "|Apr|April|May|May|Jun|June"
       "|Jul|July|Aug|August|Sep|September"
       "|Oct|October|Nov|November|Dec|December",				/*  month_names		*/
    ""															/*  time zone       */ /*- mm 020710 -*/
};

#if _MSL_C_LOCALE_ONLY

struct __locale  _current_locale =
{
	&_loc_tim_C, 			/* time_component		*/
	&_loc_coll_C 			/* collate_component	*/			/*- cc 030327 -*/ 
#if _MSL_WIDE_CHAR
	,						/* comma				*/
	&_loc_ctyp_C			/* ctype_component		*/			/*- cc 030327 -*/ 
#endif
};

#else

struct __locale  _current_locale =								/*- mm 011205 -*/
{
	NULL,                   /* next_locale 			*/
#if (_MSL_DEFAULT_LOCALE == _MSL_LOCALE_C)						/*- mm 020212 -*/
	"C",					/* locale_name			*/
#elif (_MSL_DEFAULT_LOCALE == _MSL_LOCALE_CUTF8)				/*- mm 020118 -*/ /*- mm 020212 -*/
	"C|C-UTF-8|C|C|C",		/* locale_name    		*/			/*- mm 020212 -*/
#else															/*- mm 020212 -*/
	"C||C|C|C",				/* locale_name    		*/			/*- mm 020118 -*/
#endif															/*- mm 020118 -*/
	&_loc_coll_C, 			/* collate_component	*/
#if (_MSL_DEFAULT_LOCALE == _MSL_LOCALE_C)						/*- mm 020212 -*/
	&_loc_ctyp_C,			/* ctype_component		*/
#elif (_MSL_DEFAULT_LOCALE == _MSL_LOCALE_CUTF8)				/*- mm 020118 -*/ /*- mm 020212 -*/
	&_loc_ctyp_C_UTF_8,		/* ctype_component		*/			/*- mm 020212 -*/
#else															/*- mm 020212 -*/
	&_loc_ctyp_I,			/* ctype_component		*/			/*- mm 020212 -*/
#endif															/*- mm 020118 -*/
	&_loc_mon_C, 			/* monetary_component	*/
	&_loc_num_C, 			/* numeric_component	*/
	&_loc_tim_C 			/* time_component		*/
};

#endif /* _MSL_C_LOCALE_ONLY */

#if !_MSL_C_LOCALE_ONLY

struct __locale _preset_locales[3] =								/*- mm 011205 -*/
{
	{
		&_preset_locales[1],    /* next_locale 			*/
		"C",					/* locale_name			*/
		&_loc_coll_C, 			/* collate_component	*/
		&_loc_ctyp_C,			/* ctype_component		*/
		&_loc_mon_C, 			/* monetary_component	*/
		&_loc_num_C, 			/* numeric_component	*/
		&_loc_tim_C 			/* time_component		*/
	},
	{
		&_preset_locales[2],   	/* next_locale 			*/
		"",						/* locale_name			*/			/*- mm 020118 -*/
		&_loc_coll_C, 			/* collate_component	*/
		&_loc_ctyp_I,			/* ctype_component		*/
		&_loc_mon_C, 			/* monetary_component	*/
		&_loc_num_C, 			/* numeric_component	*/
		&_loc_tim_C 			/* time_component		*/
	},
	{
		NULL,   				/* next_locale 			*/
		"C-UTF-8",				/* locale_name			*/			/*- mm 020118 -*/
		&_loc_coll_C, 			/* collate_component	*/
		&_loc_ctyp_C_UTF_8,		/* ctype_component		*/
		&_loc_mon_C, 			/* monetary_component	*/
		&_loc_num_C, 			/* numeric_component	*/
		&_loc_tim_C 			/* time_component		*/
	},
};

/*- end mm 011130 insertion -*/

#if !_MSL_THREADSAFE || !_MSL_LOCALDATA_AVAILABLE
	static struct lconv public_lconv;						/*- mm 010503 -*/ /*- cc 011128 -*/
#endif

#if _MSL_OS_DISK_FILE_SUPPORT
	static struct __locale * BuildLocale(const char *);						/*- mm 020606 -*/
	static int char_to_index(char*);										/*- mm 020606 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */

char * _MSL_CDECL setlocale(int category, const char * locale)
{
/*- begin mm 011130 rewrite -*/
	struct __locale * locptr;										/*- mm 011205 -*/
	int       c_locale_is_composite, index;
	char *	  l_name_start;
	char *    l_name_end;
	char      name_list[_LOCALE_CMPT_COUNT][_LOCALE_NAME_LEN];
	int       cmpt_macro_vals[_LOCALE_CMPT_COUNT] = {LC_COLLATE,  LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME};
	struct __locale *    current_locale_ptr;										/*- mm 011205 -*/
	
	current_locale_ptr = &_MSL_LOCALDATA(_current_locale);
	
	if ((locale == NULL) || (strcmp(locale, current_locale_ptr->locale_name) == 0))
	{
		locptr = current_locale_ptr;												/*- mm 011130 -*/
		switch(category)
		{
			case LC_ALL:
				return(current_locale_ptr->locale_name);
			case LC_COLLATE:
				return(current_locale_ptr->coll_cmpt_ptr->CmptName);
			case LC_CTYPE:
				return(current_locale_ptr->ctype_cmpt_ptr->CmptName);
			case LC_MONETARY:
				return(current_locale_ptr->mon_cmpt_ptr->CmptName);
			case LC_NUMERIC:
				return(current_locale_ptr->num_cmpt_ptr->CmptName);
			case LC_TIME:
				return(current_locale_ptr->time_cmpt_ptr->CmptName);
			default:
				return(NULL);
		}
	}

	/* split locale name into components */
	l_name_start = (char*)locale;
	for(index = 0; index < 5; index++)
	{
		l_name_end = strchr(l_name_start, '|');
		if (l_name_end == NULL)
		{
			strcpy(name_list[index++], l_name_start);
			break;
		}
		else
		{
			strncpy(name_list[index], l_name_start, (size_t)(l_name_end - l_name_start));
			name_list[index][l_name_end - l_name_start] = '\0';
			l_name_start = l_name_end + 1;
		}
	}
	
	if (index == 1)
		c_locale_is_composite = 0;
	else
		if (index == _LOCALE_CMPT_COUNT)
			c_locale_is_composite = 1;
		else     /* given locale name is not valid */
			return(NULL);
	
	if (!c_locale_is_composite) 
	{
		locptr = &_preset_locales[0];
		while(locptr != NULL)  /* search existing locales for given name */
		{
			if (strcmp(locale, locptr->locale_name) == 0)
				break;
			locptr = locptr->next_locale;
		}
		#if _MSL_OS_DISK_FILE_SUPPORT
		if (locptr == NULL)
			locptr = BuildLocale(locale);								/*- mm 020606 -*/
		#endif /* _MSL_OS_DISK_FILE_SUPPORT */
		if (locptr == NULL)												/*- mm 020606 -*/
			return(NULL);
		else
		{
			switch(category)
			{
				case LC_ALL:
					strcpy(current_locale_ptr->locale_name, locptr->locale_name);		
					current_locale_ptr->coll_cmpt_ptr = locptr->coll_cmpt_ptr;					
					current_locale_ptr->ctype_cmpt_ptr = locptr->ctype_cmpt_ptr;				
					current_locale_ptr->mon_cmpt_ptr = locptr->mon_cmpt_ptr;					
					current_locale_ptr->num_cmpt_ptr = locptr->num_cmpt_ptr;					
					current_locale_ptr->time_cmpt_ptr = locptr->time_cmpt_ptr;					
					memcpy((void *)&_MSL_LOCALDATA(__lconv).mon_decimal_point, (void*)&(locptr->mon_cmpt_ptr)->mon_decimal_point,
													sizeof(struct _loc_mon_cmpt_vals));
					memcpy((void *)&_MSL_LOCALDATA(__lconv).decimal_point, (void*)&(locptr->num_cmpt_ptr)->decimal_point,
													sizeof(struct _loc_num_cmpt_vals));
				return(current_locale_ptr->locale_name);
				case LC_COLLATE:
					current_locale_ptr->coll_cmpt_ptr = locptr->coll_cmpt_ptr;
					break;
				case LC_CTYPE:
					current_locale_ptr->ctype_cmpt_ptr = locptr->ctype_cmpt_ptr;
					break;
				case LC_MONETARY:
					current_locale_ptr->mon_cmpt_ptr = locptr->mon_cmpt_ptr;
					memcpy((void *)&_MSL_LOCALDATA(__lconv).mon_decimal_point, (void*)&(locptr->mon_cmpt_ptr)->mon_decimal_point,
													sizeof(struct _loc_mon_cmpt_vals));
					break;
				case LC_NUMERIC:
					current_locale_ptr->num_cmpt_ptr = locptr->num_cmpt_ptr;
					memcpy((void *)&_MSL_LOCALDATA(__lconv).decimal_point, (void*)&(locptr->num_cmpt_ptr)->decimal_point,
													sizeof(struct _loc_num_cmpt_vals));
					break;
				case LC_TIME:
					current_locale_ptr->time_cmpt_ptr = locptr->time_cmpt_ptr;
					break;
				default:
					return(NULL);
			}
		}
	}
	else
	{
		for (index = 0; index < _LOCALE_CMPT_COUNT; index++)
			if (cmpt_macro_vals[index] & category)
				setlocale(cmpt_macro_vals[index], name_list[index]);
		strcpy(current_locale_ptr->locale_name, locale);
		return(current_locale_ptr->locale_name);
	}
	
	/* construct new locale name */
	strcpy(current_locale_ptr->locale_name, current_locale_ptr->coll_cmpt_ptr->CmptName);
	if ((strcmp(current_locale_ptr->coll_cmpt_ptr->CmptName, current_locale_ptr->ctype_cmpt_ptr->CmptName) != 0) ||
		 (strcmp(current_locale_ptr->coll_cmpt_ptr->CmptName, current_locale_ptr->mon_cmpt_ptr->CmptName) != 0) ||
	     (strcmp(current_locale_ptr->coll_cmpt_ptr->CmptName, current_locale_ptr->num_cmpt_ptr->CmptName) != 0) ||
		 (strcmp(current_locale_ptr->coll_cmpt_ptr->CmptName, current_locale_ptr->time_cmpt_ptr->CmptName) != 0))
	{
		strcat(current_locale_ptr->locale_name, "|");
		strcat(current_locale_ptr->locale_name, current_locale_ptr->ctype_cmpt_ptr->CmptName);
		strcat(current_locale_ptr->locale_name, "|");
		strcat(current_locale_ptr->locale_name, current_locale_ptr->mon_cmpt_ptr->CmptName);
		strcat(current_locale_ptr->locale_name, "|");
		strcat(current_locale_ptr->locale_name, current_locale_ptr->num_cmpt_ptr->CmptName);
		strcat(current_locale_ptr->locale_name, "|");
		strcat(current_locale_ptr->locale_name, current_locale_ptr->time_cmpt_ptr->CmptName);
	}
	
	return(current_locale_ptr->locale_name);
}
/*- end mm 011130 rewrite -*/


struct lconv * _MSL_CDECL localeconv(void)
{
	struct lconv * Lconv_ptr;											/*- mm 020617 -*/
	struct __locale *current_locale_ptr;								/*- mm 020617 -*/
	
	Lconv_ptr = &_MSL_LOCALDATA(__lconv);
	current_locale_ptr = &_MSL_LOCALDATA(_current_locale);
	
/* begin changes  */  /*- mm 020617 -*/
	Lconv_ptr->decimal_point      = current_locale_ptr->num_cmpt_ptr->decimal_point;
	Lconv_ptr->thousands_sep      = current_locale_ptr->num_cmpt_ptr->thousands_sep;
	Lconv_ptr->grouping           = current_locale_ptr->num_cmpt_ptr->grouping;
	Lconv_ptr->mon_decimal_point  = current_locale_ptr->mon_cmpt_ptr->mon_decimal_point;
	Lconv_ptr->mon_thousands_sep  = current_locale_ptr->mon_cmpt_ptr->mon_thousands_sep;
	Lconv_ptr->mon_grouping       = current_locale_ptr->mon_cmpt_ptr->mon_grouping;
	Lconv_ptr->positive_sign      = current_locale_ptr->mon_cmpt_ptr->positive_sign;
	Lconv_ptr->negative_sign      = current_locale_ptr->mon_cmpt_ptr->negative_sign;
	Lconv_ptr->currency_symbol    = current_locale_ptr->mon_cmpt_ptr->currency_symbol;
	Lconv_ptr->frac_digits        = current_locale_ptr->mon_cmpt_ptr->frac_digits;
	Lconv_ptr->p_cs_precedes      = current_locale_ptr->mon_cmpt_ptr->p_cs_precedes;
	Lconv_ptr->n_cs_precedes      = current_locale_ptr->mon_cmpt_ptr->n_cs_precedes;
	Lconv_ptr->p_sep_by_space     = current_locale_ptr->mon_cmpt_ptr->p_sep_by_space;
	Lconv_ptr->n_sep_by_space     = current_locale_ptr->mon_cmpt_ptr->n_sep_by_space;
	Lconv_ptr->p_sign_posn        = current_locale_ptr->mon_cmpt_ptr->p_sign_posn;
	Lconv_ptr->n_sign_posn        = current_locale_ptr->mon_cmpt_ptr->n_sign_posn;
	Lconv_ptr->int_curr_symbol    = current_locale_ptr->mon_cmpt_ptr->int_curr_symbol;
	Lconv_ptr->int_frac_digits    = current_locale_ptr->mon_cmpt_ptr->int_frac_digits;
#if _MSL_C99											/*- mm 030528 -*/
	Lconv_ptr->int_p_cs_precedes  = current_locale_ptr->mon_cmpt_ptr->int_p_cs_precedes; 
	Lconv_ptr->int_n_cs_precedes  = current_locale_ptr->mon_cmpt_ptr->int_n_cs_precedes; 
	Lconv_ptr->int_p_sep_by_space = current_locale_ptr->mon_cmpt_ptr->int_p_sep_by_space; 
	Lconv_ptr->int_n_sep_by_space = current_locale_ptr->mon_cmpt_ptr->int_n_sep_by_space; 
	Lconv_ptr->int_p_sign_posn    = current_locale_ptr->mon_cmpt_ptr->int_p_sign_posn; 
	Lconv_ptr->int_n_sign_posn    = current_locale_ptr->mon_cmpt_ptr->int_n_sign_posn;
#endif /* _MSL_C99 */									/*- mm 030528 -*/
	
	return(Lconv_ptr);
/* end changes */ /*- mm 020617 -*/
}
/*- begin 020606 rewrite -*/

#define  ctype_narrow 				   0
#define  ctype_wide 				  14
#define  codecvt_narrow 			  26
#define  codecvt_wide 				  42
#define  numeric_narrow 			  56
#define  numeric_wide 				  72
#define  collate_narrow 			  86
#define  collate_wide 				 102
#define  time_narrow 				 116
#define  time_wide 					 129
#define  money_local_narrow 		 140
#define  money_international_narrow  160
#define  money_local_wide 			 188
#define  money_international_wide 	 206
#define  set_1 						 232
#define  set_2 						 239
#define  alpha   0
#define  cntrl   6
#define  digit  12
#define  graph  18
#define  lower  24
#define  print  30
#define  punct  36
#define  space  42
#define  upper  48
#define  xdigit 54

/* The following is a modified version  of fgets, to handle the differing line endings MacOS, MachO, and Windows */ 	/*- mm 020911 -*/
static char * xfgets(char * s, int n, FILE * stream);

static char * xfgets(char * s, int n, FILE * stream)
{
	char *	p = s;
	int			c;
	
  	__begin_critical_region(files_access);		 
	
	if (n)
	do
	{
		c = __getc(stream);
		if (c == '\r')
			c = '\n';
		
		if (c == EOF)
			if (stream->state.eof && p != s)
				break;
			else
			{
				__end_critical_region(files_access);		
				return(NULL);
			}
		
		*p++ = c;
	}
	while ((c != '\n') && --n);
  	__end_critical_region(files_access);		
	
	*p = 0;
	
	return(s);
}
/* end of  mm 020911  insert -*/										/*- mm 020911 -*/

#if _MSL_OS_DISK_FILE_SUPPORT

static struct __locale * BuildLocale(const char * locale)
{
	FILE * imp_file;
	
	char   line[100];
	char   line2[100];
	char * lineptr = line;
	struct __locale * NewLocale;
	struct __locale * locptr;
	int    end_flag = 0;
	int    state;
	int    collate_width = 0;
	int    time_width = 0;
	char   abbrev_days[7][4] = {"", "", "", "", "", "", ""};
	char   days[7][10] = {"", "", "", "", "", "", ""};
	char   abbrev_months[12][4] = {"", "", "", "", "", "", "", "", "", "", "", ""};
	char   months[12][10] = {"", "", "", "", "", "", "", "", "", "", "", ""};
	char   templine[200];
	unsigned int   tempcollate1[256];							 	/*- mm 020718 -*/
	char * chptr;
	int    i;
	int	   ignorable, quote_found, French_accents;					/*- mm 020718 -*/
	char   id[20];
	char   str[20];													/*- mm 020617 -*/
	char   mon_format[5] = "";
	int    parens = 0;
	int    symbol_found = 0;
	int    value_found = 0;
	char * chrptr;
	int    sign_posn = CHAR_MAX;
	char   token[32];												/*- mm 020731 -*/
	char * chrvalue;
	char * val;
	int    value;
	int    coll_weight1, coll_weight2, coll_weight3;				/*- mm 020718 -*/
	int    start_index, finish_index;
	int    start_val_index, finish_val_index;
	unsigned short * us_ptr;
#if _MSL_WIDE_CHAR && _MSL_C99										/*- mm 030204 -*/ /*- mm 030321 -*/
	wchar_t * wct_ptr;
#endif	/* _MSL_WIDE_CHAR	&& _MSL_C99 */							/*- mm 030204 -*/ /*- mm 030321 -*/
	unsigned char * uc_ptr;
	char   commands[] = "|alpha|blank|cntrl|digit|graph|lower|print|punct|space|upper|xdigit|";
	int    value_elements[11] =  {__msl_alpha, __msl_blank, __msl_cntrl, __msl_digit, __msl_graph, 
				   __msl_lower, __msl_print, __msl_punct, __msl_space, __msl_upper, __msl_xdigit};

	char   keys[] = "$ctype_narrow|$ctype_wide|$codecvt_narrow|$codecvt_wide|$numeric_narrow|"
					"$numeric_wide|$collate_narrow|$collate_wide|$time_narrow|$time_wide|"
					"$money_local_narrow|$money_international_narrow|$money_local_wide|"
					"$money_international_wide|$set 1|$set 2|";
	struct __locale *    current_locale_ptr;										
	
	current_locale_ptr = &_MSL_LOCALDATA(_current_locale);
	
		imp_file = fopen(locale, "r");
	
	if (imp_file == NULL)
		return(NULL);
		
	locptr = &_preset_locales[2];
	while(locptr->next_locale != NULL)
		locptr = locptr->next_locale;

	NewLocale = malloc(sizeof(struct __locale));
	if (NewLocale == NULL)
		return(NULL);
	locptr->next_locale = NewLocale;
	NewLocale->next_locale = NULL;
	strcpy(NewLocale->locale_name, locale);
	NewLocale->coll_cmpt_ptr  = _preset_locales[0].coll_cmpt_ptr;		
	NewLocale->ctype_cmpt_ptr = _preset_locales[0].ctype_cmpt_ptr;			
	NewLocale->mon_cmpt_ptr   = _preset_locales[0].mon_cmpt_ptr;					
	NewLocale->num_cmpt_ptr   = _preset_locales[0].num_cmpt_ptr;				
	NewLocale->time_cmpt_ptr  = _preset_locales[0].time_cmpt_ptr;;					
	
	while(xfgets(line, 100, imp_file) != NULL)								/*- mm 020911 -*/
	{
		strcpy(line2, line);
		line2[strlen(line2)-1] = '\0';
		
		line[strlen(line)-1] = '\0';
		if (strlen(line) == 0)
			continue;
		if (line[0] == '$')
			state = strstr(keys, line) - &keys[0];
		switch(state)
		{
			case  codecvt_wide:
			case  ctype_wide:
			case  ctype_narrow:
			{
				
				if (NewLocale->ctype_cmpt_ptr  == _preset_locales[0].ctype_cmpt_ptr)
				{
					NewLocale->ctype_cmpt_ptr = malloc(sizeof(struct _loc_ctype_cmpt));
					strcpy(NewLocale->ctype_cmpt_ptr->CmptName, locale);
					NewLocale->ctype_cmpt_ptr->ctype_map_ptr  = _loc_ctyp_C.ctype_map_ptr;		/*- mm 020617 -*/
					NewLocale->ctype_cmpt_ptr->upper_map_ptr  = _loc_ctyp_C.upper_map_ptr;		/*- mm 020617 -*/
					NewLocale->ctype_cmpt_ptr->lower_map_ptr  = _loc_ctyp_C.lower_map_ptr;		/*- mm 020617 -*/
#if _MSL_WIDE_CHAR 
#if _MSL_C99																	/*- mm 030321 -*/
					NewLocale->ctype_cmpt_ptr->wctype_map_ptr = _loc_ctyp_C.wctype_map_ptr;		/*- mm 020617 -*/
					NewLocale->ctype_cmpt_ptr->wupper_map_ptr = _loc_ctyp_C.wupper_map_ptr;		/*- mm 020617 -*/
					NewLocale->ctype_cmpt_ptr->wlower_map_ptr = _loc_ctyp_C.wlower_map_ptr;		/*- mm 020617 -*/
#endif	/* _MSL_C99 */														/*- mm 030321 -*/
					NewLocale->ctype_cmpt_ptr->decode_mb 	  = _loc_ctyp_C.decode_mb;			/*- mm 020617 -*/
					NewLocale->ctype_cmpt_ptr->encode_wc 	  = _loc_ctyp_C.encode_wc;			/*- mm 020617 -*/
#endif /* _MSL_WIDE_CHAR */ 
					continue;
				}
				if (state == codecvt_wide)
#if _MSL_WIDE_CHAR					/*- mm 020617 -*/
				{
					if (strcmp(id, "UTF-8") == 0)
					{
						NewLocale->ctype_cmpt_ptr->decode_mb = _loc_ctyp_C_UTF_8.decode_mb;
						NewLocale->ctype_cmpt_ptr->encode_wc = _loc_ctyp_C_UTF_8.encode_wc;
					}
					break;				/*- mm 020617 -*/
				}					/*- mm 020617 -*/
#else								/*- mm 030204 -*/
				break;				/*- mm 030204 -*/
#endif

				strcpy(id, strtok(line, " ["));
				if (strcmp(id, "ctype") == 0)
				{
					if (state == ctype_narrow)
					{
					 	if (NewLocale->ctype_cmpt_ptr->ctype_map_ptr == _loc_ctyp_C.ctype_map_ptr)		/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->ctype_map_ptr = malloc(sizeof(unsigned short[__msl_cmap_size]));
						us_ptr = (unsigned short *) NewLocale->ctype_cmpt_ptr->ctype_map_ptr;
					}
#if _MSL_WIDE_CHAR && _MSL_C99																			/*- mm 030321 -*/
					if (state == ctype_wide)
					{ 
					 	if (NewLocale->ctype_cmpt_ptr->wctype_map_ptr == _loc_ctyp_C.wctype_map_ptr)	/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->wctype_map_ptr = malloc(sizeof(wchar_t[__msl_wcmap_size]));
						us_ptr = (unsigned short *) NewLocale->ctype_cmpt_ptr->wctype_map_ptr;
					}
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */																/*- mm 030321 -*/
					strcpy(id, strtok(NULL, "]"));
					strcpy(token, strtok(NULL, " "));
					if (strcmp(token, "=") != 0)
					{
						fprintf(stderr, "Character '=' expected but %s found\n", token);
						abort();
					}
					value = 0;
					chptr = strtok(NULL, " ");
					while(chptr != NULL)
					{
						strcpy(token, chptr);
						value |= value_elements[(strstr(commands, token) - &commands[0]) / 6];
						if ((chptr = strtok(NULL, " ")) == NULL)
							break;
						chptr = strtok(NULL, " ");
					}
					start_index = char_to_index(id);
					if ((chptr = strchr(id, '-')) != NULL)
						finish_index = char_to_index(chptr + 1);
					else
						finish_index = start_index;
					for (; start_index <= finish_index; start_index++)
						us_ptr[start_index] = value;
				}
				else if (strcmp(id, "lower") == 0)
				{
					if (state == ctype_narrow)
					{
						if (NewLocale->ctype_cmpt_ptr->lower_map_ptr == _loc_ctyp_C.lower_map_ptr)		/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->lower_map_ptr  = malloc(sizeof(unsigned char[__msl_cmap_size]));
					}
					uc_ptr = (unsigned char *) NewLocale->ctype_cmpt_ptr->lower_map_ptr;
#if _MSL_WIDE_CHAR && _MSL_C99																			/*- mm 030321 -*/
					if (state == ctype_wide)
					{ 
					 	if (NewLocale->ctype_cmpt_ptr->wlower_map_ptr == _loc_ctyp_C.wlower_map_ptr)		/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->wlower_map_ptr = malloc(sizeof(wchar_t[__msl_wcmap_size]));
					}
					wct_ptr = (wchar_t *) NewLocale->ctype_cmpt_ptr->wlower_map_ptr;
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */																	/*- mm 030321 -*/
					strcpy(id, strtok(NULL, "]"));
					strcpy(token, strtok(NULL, " "));
					if (strcmp(token, "=") != 0)
					{
						fprintf(stderr, "Character '=' expected but %s found\n", token);
						abort();
					}
					chptr = strtok(NULL, " ");
					start_val_index = char_to_index(chptr);
					if ((chptr = strchr(id, '-')) != NULL)
						finish_val_index = char_to_index(chptr + 1);
					else
						finish_val_index = start_index;
					
					start_index = char_to_index(id);
					if ((chptr = strchr(id, '-')) != NULL)
						finish_index = char_to_index(chptr + 1);
					else
						finish_index = start_index;
					for (; start_index <= finish_index; start_index++)
						if (state == ctype_narrow)
							uc_ptr[start_index] = start_val_index++;
#if _MSL_WIDE_CHAR && _MSL_C99													/*- mm 030204 -*/ /*- mm 030321 -*/
						else
							wct_ptr[start_index] = start_val_index++;
#endif	/* _MSL_WIDE_CHAR && _MSL_C99 */										/*- mm 030204 -*/ /*- mm 030321 -*/
				}
				else if (strcmp(id, "upper") == 0)
				{
					if (state == ctype_narrow)
						if (NewLocale->ctype_cmpt_ptr->upper_map_ptr == _loc_ctyp_C.upper_map_ptr)		/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->upper_map_ptr  = malloc(sizeof(unsigned char[__msl_cmap_size]));
					uc_ptr = (unsigned char *) NewLocale->ctype_cmpt_ptr->upper_map_ptr;
#if _MSL_WIDE_CHAR && _MSL_C99													/*- mm 030321 -*/
					if (state == ctype_wide)
					{ 
					 	if (NewLocale->ctype_cmpt_ptr->wupper_map_ptr == _loc_ctyp_C.wupper_map_ptr)		/*- mm 020617 -*/
							NewLocale->ctype_cmpt_ptr->wupper_map_ptr = malloc(sizeof(wchar_t[__msl_wcmap_size]));
					}
					wct_ptr = (wchar_t *) NewLocale->ctype_cmpt_ptr->wupper_map_ptr;
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */											/*- mm 030321 -*/
					strcpy(id, strtok(NULL, "]"));
					strcpy(token, strtok(NULL, " "));
					if (strcmp(token, "=") != 0)
					{
						fprintf(stderr, "Character '=' expected but %s found\n", token);
						abort();
					}
					chptr = strtok(NULL, " ");
					start_val_index = char_to_index(chptr);
					if ((chptr = strchr(id, '-')) != NULL)
						finish_val_index = char_to_index(chptr + 1);
					else
						finish_val_index = start_index;
					
					start_index = char_to_index(id);
					if ((chptr = strchr(id, '-')) != NULL)
						finish_index = char_to_index(chptr + 1);
					else
						finish_index = start_index;
					for (; start_index <= finish_index; start_index++)
						if (state == ctype_narrow)
							uc_ptr[start_index] = start_val_index++;
#if _MSL_WIDE_CHAR	&& _MSL_C99											/*- mm 030204 -*/ /*- mm 030321 -*/
						else
							wct_ptr[start_index] = start_val_index++;
#endif	/* _MSL_WIDE_CHAR && _MSL_C99 */								/*- mm 030204 -*/ /*- mm 030321 -*/
				}
						
				break;
			}
			
			case  numeric_narrow:
			{
				if (NewLocale->num_cmpt_ptr  == _preset_locales[0].num_cmpt_ptr)
				{
					NewLocale->num_cmpt_ptr = malloc(sizeof(struct _loc_num_cmpt));
					strcpy(NewLocale->num_cmpt_ptr->CmptName, locale);
					NewLocale->num_cmpt_ptr->decimal_point 	  = _loc_num_C.decimal_point;		/*- mm 020617 -*/
					NewLocale->num_cmpt_ptr->thousands_sep 	  = _loc_num_C.thousands_sep;		/*- mm 020617 -*/
					NewLocale->num_cmpt_ptr->grouping	 	  = _loc_num_C.grouping;			/*- mm 020617 -*/
					continue;
				}
				strcpy(id, strtok(line, " "));
				if (*strtok(NULL, " ") != '=')
					return(NULL);
				if (strcmp(id, "grouping") == 0)
				{
					i = 0;
					while ((chrptr = strtok(NULL, "|")) != NULL)
					{
						strcpy(str, chrptr);
						token[i++] = atoi(str);
						token[i] = '\0';
					}
					chrvalue = malloc(strlen(token)+1);				/*- mm 020731 -*/
					strcpy(chrvalue, token);
					NewLocale->num_cmpt_ptr->grouping = chrvalue;
				}
				else
				{
					chrvalue = malloc(16);							/*- mm 020731 -*/
					val      = strtok(NULL, " \"");					/*- mm 020731 -*/
					if (val == NULL)								/*- mm 020731 -*/	
						strcpy(chrvalue, "");						/*- mm 020731 -*/
					else											/*- mm 020731 -*/
						strcpy(chrvalue, val);						/*- mm 020731 -*/
					if (strcmp(chrvalue, "'") == 0)
						strcpy(chrvalue, " ");
					val = malloc(strlen(chrvalue)+1);				/*- mm 020704 -*/
					strcpy(val, chrvalue);							/*- mm 020704 -*/
					if (strcmp(id, "decimal_point") == 0)
						NewLocale->num_cmpt_ptr->decimal_point = val;
					else if (strcmp(id, "thousands_sep") == 0)
						NewLocale->num_cmpt_ptr->thousands_sep = val;/*- mm 020704 -*/
					free(chrvalue);								/*- mm 020617 -*/
				}
				break;
			}


			case  collate_wide:
				collate_width = 1;
			case  collate_narrow:
			{
				if (NewLocale->coll_cmpt_ptr  == _preset_locales[0].coll_cmpt_ptr)
				{
					NewLocale->coll_cmpt_ptr = malloc(sizeof(struct _loc_coll_cmpt));
					strcpy(NewLocale->coll_cmpt_ptr->CmptName, locale);
/* Begin changes for reading collate spec */    	/*- mm 020718 -*/
					NewLocale->coll_cmpt_ptr->char_start_value     = _loc_coll_C.char_start_value;		/*- mm 020708 -*/
					NewLocale->coll_cmpt_ptr->char_coll_tab_size   = _loc_coll_C.char_coll_tab_size;	/*- mm 020708 -*/
					NewLocale->coll_cmpt_ptr->char_spec_accents    = _loc_coll_C.char_spec_accents;
					NewLocale->coll_cmpt_ptr->char_coll_table_ptr  = _loc_coll_C.char_coll_table_ptr;	/*- mm 020708 -*/
					NewLocale->coll_cmpt_ptr->wchar_coll_seq_ptr   = _loc_coll_C.wchar_coll_seq_ptr;	/*- mm 020617 -*/
					for (i = 0; i < 256; i++)
						tempcollate1[i] = 0;
					coll_weight1    = 0;
					coll_weight2    = 0;
					coll_weight3    = 0;
					ignorable   	= 1;
					quote_found 	= 0;
					French_accents  = 0;
					continue;
				}
				if (collate_width == 1)
					break;
				for (i = 0; i < strlen(line); i++)
				{
					char currchar = line[i];
					switch (line[i])
					{
						case ' ':
							break;
						case '<':
							ignorable = 0;
							coll_weight1++;
							coll_weight2 = 0;
							coll_weight3 = 0;
							break;
						case ';':
							coll_weight1++;
							coll_weight2 += 0x0100;
							coll_weight3 = 0;
							break;
						case ',':
							coll_weight1++;
							coll_weight3 += 0x1000;
							break;
						case '=':
#if _MSL_CONSOLE_SUPPORT
							printf("    = found\n");
#endif /* _MSL_CONSOLE_SUPPORT */
							break;
						case '@':
							French_accents = 1;
							break;
						case '\"':
						{
							if (quote_found == 0)
								quote_found = 1;
							else
							{
								int first_entry = 0;
								int last_entry  = 255;
								unsigned short * table_ptr;
								while(tempcollate1[first_entry++] == 0)
									;
								first_entry--;
								while(tempcollate1[last_entry--] == 0)
									;
								last_entry++;
								NewLocale->coll_cmpt_ptr->char_start_value     = first_entry;
								NewLocale->coll_cmpt_ptr->char_coll_tab_size   = last_entry - first_entry + 1;
								NewLocale->coll_cmpt_ptr->char_spec_accents    = French_accents;
								table_ptr                                      = (unsigned short*)malloc(2 * NewLocale->coll_cmpt_ptr->char_coll_tab_size);
								NewLocale->coll_cmpt_ptr->char_coll_table_ptr  = (unsigned short*)table_ptr;
								for (i = 0; i < NewLocale->coll_cmpt_ptr->char_coll_tab_size; i++)
									table_ptr[i] = tempcollate1[first_entry++];

							}
									
							break;
						}
						case '\'':
#if _MSL_CONSOLE_SUPPORT
							printf("    \' found\n");
#endif /* _MSL_CONSOLE_SUPPORT */
							break;
						case '&':
#if _MSL_CONSOLE_SUPPORT
							printf("    & found\n");
#endif /* _MSL_CONSOLE_SUPPORT */
							break;
						default:
							if (ignorable == 0)
								tempcollate1[(unsigned char)line[i]] = coll_weight1 | coll_weight2 | coll_weight3;
							break;
					}
				}
/* End changes for reading collate spec */    	/*- mm 020718 -*/
				break;
			}

			case  time_narrow:
			{
				char formats[4][20] = {"", "", "", ""};
				char format_list[100] = "";
				if (NewLocale->time_cmpt_ptr  == _preset_locales[0].time_cmpt_ptr)
				{
					NewLocale->time_cmpt_ptr = malloc(sizeof(struct _loc_time_cmpt));
					strcpy(NewLocale->time_cmpt_ptr->CmptName, locale);
					NewLocale->time_cmpt_ptr->am_pm 	  	   	= _loc_tim_C.am_pm;						/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->DateTime_Format	= _loc_tim_C.DateTime_Format;			/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->Twelve_hr_format	= _loc_tim_C.Twelve_hr_format;			/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->Date_Format		= _loc_tim_C.Date_Format;				/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->Time_Format		= _loc_tim_C.Time_Format;				/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->Day_Names 	   	= _loc_tim_C.Day_Names;					/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->MonthNames 	   	= _loc_tim_C.MonthNames;				/*- mm 020617 -*/
					NewLocale->time_cmpt_ptr->TimeZone 	   	   	= _loc_tim_C.TimeZone;					/*- mm 020617 -*/
					continue;
				}
				strcpy(id, strtok(line, " "));
				if (*strtok(NULL, " ") != '=')
					return(NULL);
				if (strcmp(id, "abbrev_weekday") == 0)
				{
					for (i = 0; i < 7; i++)
					{
						strcpy(abbrev_days[i], strtok(NULL, " "));
						if (abbrev_days[i] == NULL)
						{
							xfgets(line, 100, imp_file);				/*- mm 020911 -*/
							i--;
						}
					}
					if (days[0][0] != '\0')
					{
						templine[0] = '\0';
						for (i = 0; i < 7; i++)
						{
							strcat(templine, abbrev_days[i]);
							strcat(templine, "|");
							strcat(templine, days[i]); 
							strcat(templine, "|");
						}
						templine[strlen(templine)-1] = '\0';						/*- mm 020704 -*/
						NewLocale->time_cmpt_ptr->Day_Names = malloc(strlen(templine));
						strcpy(NewLocale->time_cmpt_ptr->Day_Names, templine);
					}
				}
				else if (strcmp(id, "weekday") == 0)
				{
					for (i = 0; i < 7; i++)
					{
						strcpy(days[i], strtok(NULL, " "));
						if (days[i] == NULL)
						{
							xfgets(line, 100, imp_file);						/*- mm 020911 -*/
							i--;
						}
					}
					if (abbrev_days[0][0] != '\0')
					{
						templine[0] = '\0';
						for (i = 0; i < 7; i++)
						{
							strcat(templine, abbrev_days[i]);
							strcat(templine, "|");
							strcat(templine, days[i]); 
							strcat(templine, "|");
						}
						templine[strlen(templine)-1] = '\0';							/*- mm 020704 -*/
						NewLocale->time_cmpt_ptr->Day_Names = malloc(strlen(templine));
						strcpy(NewLocale->time_cmpt_ptr->Day_Names, templine);
					}
				}
				else if (strcmp(id, "abbrev_monthname") == 0)
				{
					for (i = 0; i < 12; i++)
					{
						strcpy(abbrev_months[i], strtok(NULL, " "));
						if (abbrev_months[i] == NULL)
						{
							xfgets(line, 100, imp_file);								/*- mm 020911 -*/
							i--;
						}
					}
					if (months[0][0] != '\0')
					{
						templine[0] = '\0';
						for (i = 0; i < 12; i++)
						{
							strcat(templine, abbrev_months[i]);
							strcat(templine, "|");
							strcat(templine, months[i]); 
							strcat(templine, "|");
						}
						templine[strlen(templine)-1] = '\0';							/*- mm 020704 -*/
						NewLocale->time_cmpt_ptr->Day_Names = malloc(strlen(templine));
						strcpy(NewLocale->time_cmpt_ptr->MonthNames, templine);
					}
				}
				else if (strcmp(id, "monthname") == 0)
				{
					chptr = strtok(NULL, " ");
					for (i = 0; i < 12; i++)
					{
						if (chptr != NULL)
						{
							strcpy(months[i], chptr);
							chptr = strtok(NULL, " ");
						}
						else
						{
							xfgets(line, 100, imp_file);								/*- mm 020911 -*/
							i--;
							chptr = strtok(line, " ");
						}
					}
					if (abbrev_months[0][0] != '\0')
					{
						templine[0] = '\0';
						for (i = 0; i < 12; i++)
						{
							strcat(templine, abbrev_months[i]);
							strcat(templine, "|");
							strcat(templine, months[i]); 
							strcat(templine, "|");
						}
						templine[strlen(templine)-1] = '\0';							/*- mm 020704 -*/
						NewLocale->time_cmpt_ptr->MonthNames = malloc(strlen(templine));
						strcpy(NewLocale->time_cmpt_ptr->MonthNames, templine);
					}
				}
				else if (strcmp(id, "date_time") == 0)
				{
					chptr = strtok(NULL, "\"");
					NewLocale->time_cmpt_ptr->DateTime_Format = malloc(strlen(chptr)+1);
					strcpy(NewLocale->time_cmpt_ptr->DateTime_Format, chptr);
				}
				else if (strcmp(id, "am_pm") == 0)
				{
					templine[0] = '\0';
					chptr = strtok(NULL, " ");
					strcat(templine, chptr);
					strcat(templine, "|");
					chptr = strtok(NULL, " ");
					strcat(templine, chptr);
					NewLocale->time_cmpt_ptr->am_pm = malloc(strlen(templine)+1);			/*- mm 020731 -*/
					strcpy(NewLocale->time_cmpt_ptr->am_pm, templine);
				}
				else if (strcmp(id, "time_12hour") == 0)
				{
					chptr = strtok(NULL, "\"");
					NewLocale->time_cmpt_ptr->Twelve_hr_format = malloc(strlen(chptr)+1);	/*- mm 020731 -*/
					strcpy(NewLocale->time_cmpt_ptr->Twelve_hr_format, chptr);
				}
				else if (strcmp(id, "date") == 0)
				{
					chptr = strtok(NULL, "\"");
					NewLocale->time_cmpt_ptr->Date_Format = malloc(strlen(chptr)+1);		/*- mm 020731 -*/
					strcpy(NewLocale->time_cmpt_ptr->Date_Format, chptr);
				}
				else if (strcmp(id, "time") == 0)
				{
					chptr = strtok(NULL, "\"");
					NewLocale->time_cmpt_ptr->Time_Format = malloc(strlen(chptr)+1);		/*- mm 020731 -*/
					strcpy(NewLocale->time_cmpt_ptr->Time_Format, chptr);
				}
				else if (strcmp(id, "time_zone") == 0)
				{
					templine[0] = '\0';
					chptr = strchr(line2, '=') + 1;
					chptr = strchr(chptr, '\"');
					if (chptr != NULL)
					{
						chptr = strtok(NULL, "\"");
						chptr = strtok(NULL, "\"");
						if (chptr != NULL)
							strcat(templine, "|");
					}
					else
					{
						chptr = strtok(NULL, " ");
						strcat(templine, chptr);
						chptr = strtok(NULL, " ");
						if (chptr != NULL)
						{
							strcat(templine, "|");
							strcat(templine, chptr);
						}
					}
					NewLocale->time_cmpt_ptr->TimeZone = malloc(strlen(templine)+1);	/*- mm 020731 -*/
					strcpy(NewLocale->time_cmpt_ptr->TimeZone, templine);
				}
								
				break;
			}
						
			case  money_local_narrow:
			case  money_international_narrow:
			{
				if (NewLocale->mon_cmpt_ptr  ==_preset_locales[0].mon_cmpt_ptr)
				{
					NewLocale->mon_cmpt_ptr = malloc(sizeof(struct _loc_mon_cmpt));
					strcpy(NewLocale->mon_cmpt_ptr->CmptName, locale);
					NewLocale->mon_cmpt_ptr->mon_decimal_point 	= "";
					NewLocale->mon_cmpt_ptr->mon_thousands_sep 	= "";
					NewLocale->mon_cmpt_ptr->mon_grouping 	   	= "";			/*- mm 020604 -*/
					NewLocale->mon_cmpt_ptr->positive_sign 	   	= "";
					NewLocale->mon_cmpt_ptr->negative_sign 	   	= "";
					NewLocale->mon_cmpt_ptr->currency_symbol 	= "";
					NewLocale->mon_cmpt_ptr->frac_digits 	   	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->p_cs_precedes 		= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->n_cs_precedes 	   	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->p_sep_by_space 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->n_sep_by_space 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->p_sign_posn 	   	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->n_sign_posn 	   	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_curr_symbol 	= "";
					NewLocale->mon_cmpt_ptr->int_frac_digits 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_p_cs_precedes 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_n_cs_precedes 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_p_sep_by_space = CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_n_sep_by_space = CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_p_sign_posn 	= CHAR_MAX;
					NewLocale->mon_cmpt_ptr->int_n_sign_posn 	= CHAR_MAX;
					continue;
				}
				strcpy(id, strtok(line, " "));
				val = strtok(NULL, " ");										/*- mm 020731 -*/
				if ((val == NULL) || (*val != '='))								/*- mm 020731 -*/
					continue;
					
				chrptr = strtok(NULL, "|\"");									/*- mm 020704 -*/
				if (chrptr == NULL)												/*- mm 020704 -*/
					strcpy(token, "");											/*- mm 020704 -*/
				else															/*- mm 020704 -*/
					strcpy(token, chrptr);										/*- mm 020704 -*/
				if (state == money_local_narrow)
				{
					if (strcmp(id, "decimal_point") == 0)
					{
						NewLocale->mon_cmpt_ptr->mon_decimal_point = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->mon_decimal_point, token);
					}
					else if (strcmp(id, "thousands_sep") == 0)
					{
						NewLocale->mon_cmpt_ptr->mon_thousands_sep = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->mon_thousands_sep, token);
					}
					else if (strcmp(id, "grouping") == 0)
					{
						i = 0;
						while (chrptr != NULL)
						{
							str[i++] = atoi(token);
							str[i] = '\0';
							chrptr = strtok(NULL, " |");
																							/*- mm 020731 -*/
							if (chrptr == NULL)												/*- mm 020704 -*//*- mm 020731 -*/
								strcpy(token, "");											/*- mm 020704 -*//*- mm 020731 -*/
							else															/*- mm 020704 -*//*- mm 020731 -*/
								strcpy(token, chrptr);										/*- mm 020704 -*//*- mm 020731 -*/
						}
						chrvalue = malloc(strlen(str)+1);
						strcpy(chrvalue, str);
						NewLocale->mon_cmpt_ptr->mon_grouping = chrvalue;
					}
					else if (strcmp(id, "curr_symbol") == 0)
					{
						NewLocale->mon_cmpt_ptr->currency_symbol = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->currency_symbol, token);
					}
					else if (strcmp(id, "positive_sign") == 0)
					{
						NewLocale->mon_cmpt_ptr->positive_sign = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->positive_sign, token);
					}
					else if (strcmp(id, "negative_sign") == 0)
					{
						NewLocale->mon_cmpt_ptr->negative_sign = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->negative_sign, token);
					}
					else if (strcmp(id, "frac_digits") == 0)
						NewLocale->mon_cmpt_ptr->frac_digits = token[0] - '0';
					else
					{
						chrptr = strtok(token, " ");			/*- mm 020704 -*/
						strcpy(token, chrptr); 					/*- mm 020704 -*/
						for (i = 0; i < 4; i++)
						{
							if (strcmp(token, "symbol") == 0)
							{
								symbol_found = 1;
								if (value_found)
									if (strcmp(id, "pos_format") == 0)
										NewLocale->mon_cmpt_ptr->p_cs_precedes = '\0';
									else
										NewLocale->mon_cmpt_ptr->n_cs_precedes = '\0';
							}
							else if (strcmp(token, "value") == 0)
							{
								value_found = 1;
								if (symbol_found)
									if (strcmp(id, "pos_format") == 0)
										NewLocale->mon_cmpt_ptr->p_cs_precedes = '\1';
									else
										NewLocale->mon_cmpt_ptr->n_cs_precedes = '\1';
							}
							else if (strcmp(token, "space") == 0)
							{
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->p_sep_by_space = '\1';
								else
									NewLocale->mon_cmpt_ptr->n_sep_by_space = '\1';
							}
							else if (strcmp(token, "none") == 0)
							{
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->p_sep_by_space = '\0';
								else
									NewLocale->mon_cmpt_ptr->n_sep_by_space = '\0';
							}
							else if (strcmp(token, "sign") == 0)
							{
								if (token[i] == '(')
									sign_posn = '\0';
								else if (symbol_found && value_found)
									sign_posn = '\1';
								else if (!symbol_found && !value_found)
									sign_posn = '\2';
								else if (!symbol_found)
									sign_posn = '\3';
								else
									sign_posn = '\4';
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->p_sign_posn = sign_posn;
								else
									NewLocale->mon_cmpt_ptr->n_sign_posn = sign_posn;
							}
							mon_format[i+1] = '\0';
							chrptr = strtok(NULL, " ");
							if (chrptr == NULL)
								break;
							strcpy(token, chrptr );
						}
					}
				}
				else
				{
					if (strcmp(id, "curr_symbol") == 0)
					{
						NewLocale->mon_cmpt_ptr->int_curr_symbol = malloc(strlen(token)+1);	/*- mm 020731 -*/
						strcpy(NewLocale->mon_cmpt_ptr->int_curr_symbol, token);
					}
					else if (strcmp(id, "frac_digits") == 0)
						NewLocale->mon_cmpt_ptr->int_frac_digits = token[0] - '0';
					else if ((strcmp(id, "pos_format") == 0) || (strcmp(id, "neg_format") == 0))
					{

						for (i = 0; i < 4; i++)
						{
							if (strcmp(token, "symbol") == 0)
							{
								mon_format[i] = '$';
								symbol_found = 1;
								if (value_found)
									if (strcmp(id, "pos_format") == 0)
										NewLocale->mon_cmpt_ptr->int_p_cs_precedes = '\0';
									else
										NewLocale->mon_cmpt_ptr->int_n_cs_precedes = '\0';
							}
							else if (strcmp(token, "value") == 0)
							{
								mon_format[i] = 'V';
								value_found = 1;
								if (symbol_found)
									if (strcmp(id, "pos_format") == 0)
										NewLocale->mon_cmpt_ptr->int_p_cs_precedes = '\1';
									else
										NewLocale->mon_cmpt_ptr->int_n_cs_precedes = '\1';
							}
							else if (strcmp(token, "space") == 0)
							{
								mon_format[i] = ' ';
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->int_p_sep_by_space = '\1';
								else
									NewLocale->mon_cmpt_ptr->int_n_sep_by_space = '\1';
							}
							else if (strcmp(token, "none") == 0)
							{
								mon_format[i] = 'n';
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->int_p_sep_by_space = '\0';
								else
									NewLocale->mon_cmpt_ptr->int_n_sep_by_space = '\0';
							}
							else if (strcmp(token, "sign") == 0)
							{
								mon_format[i] = '-';
								if (token[i] == '(')
									sign_posn = '\0';
								else if (symbol_found && value_found)
									sign_posn = '\1';
								else if (!symbol_found && !value_found)
									sign_posn = '\2';
								else if (!symbol_found)
									sign_posn = '\3';
								else
									sign_posn = '\4';
								if (strcmp(id, "pos_format") == 0)
									NewLocale->mon_cmpt_ptr->int_p_sign_posn = sign_posn;
								else
									NewLocale->mon_cmpt_ptr->int_n_sign_posn = sign_posn;
							}
							mon_format[i+1] = '\0';
							chrptr = strtok(NULL, " ");
							if (chrptr == NULL)
								break;
							strcpy(token, chrptr );
						}
					}
				}
				break;
			}
			
			/* The following categories that may occur in C++ locale files are ignored */
			case  codecvt_narrow:				/*- mm 020617 -*/
			case  time_wide:
			case  money_international_wide:
			case  money_local_wide:
			case  numeric_wide:
			case  set_1:
			case  set_2:
				break;
			
			default:
			{
				locptr->next_locale = NULL;
				free(NewLocale);
				return(NULL);
			}

		}
		
	}
	if (!NewLocale->coll_cmpt_ptr)
		NewLocale->coll_cmpt_ptr  = (&_preset_locales[0])->coll_cmpt_ptr;			
	if (!NewLocale->ctype_cmpt_ptr)
		NewLocale->ctype_cmpt_ptr = (&_preset_locales[0])->ctype_cmpt_ptr;				
	if (!NewLocale->mon_cmpt_ptr)
		NewLocale->mon_cmpt_ptr   = (&_preset_locales[0])->mon_cmpt_ptr;					
	if (!NewLocale->num_cmpt_ptr)
		NewLocale->num_cmpt_ptr   = (&_preset_locales[0])->num_cmpt_ptr;					
	if (!NewLocale->time_cmpt_ptr)
		NewLocale->time_cmpt_ptr  = (&_preset_locales[0])->time_cmpt_ptr;					
		
	return(NewLocale);
}

static int char_to_index(char * char_spec)
{
	int result = 0;
	char* chptr = char_spec;
	
	while(*chptr == ' ')
		chptr++;
	if (*chptr == '\'')
		chptr++;
	if (*chptr == '\\')
	{
		chptr++;
		switch (*chptr)
		{
			case 'n':
				result = '\n';
				break;
			case 't':
				result = '\t';
				break;
			case 'v':
				result = '\v';
				break;
			case 'b':
				result = '\b';
				break;
			case 'r':
				result = '\r';
				break;
			case 'f':
				result = '\f';
				break;
			case 'a':
				result = '\a';
				break;
			case '\\':
				result = '\\';
				break;
			case '\?':
				result = '\?';
				break;
			case '\"':
				result = '\"';
				break;
			case '\'':
				result = '\'';
				break;
			case 'u':
			case 'x':
			case 'X':
			{
				chptr++;
				while(*chptr != '\0')
				{
					if (isxdigit(*chptr))
					{
						result <<= 4;
						if (isdigit(*chptr))
							result += *chptr - '0';
						else
							result += toupper(*chptr) - 'A' + 10;
					}
					else
						break;
					chptr++;
				}
				break;
			}
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			{
	
				while(*chptr != '\0')
				{
					if ((*chptr >= '0') && (*chptr <= '7'))
					{
						result <<= 3;
						result += *chptr - '0';
					}
					else
						break;
					chptr++;
				}
				break;
			}
			default:
				break;
		}
		
	}
	else
		result = (unsigned char)*chptr;		
		
	return(result);
}

#endif /* _MSL_OS_DISK_FILE_SUPPORT */
/*- end 020606 rewrite -*/

#endif /* _MSL_C_LOCALE_ONLY */

/* Change record:
 * JFH 950612 First code release.
 * mm  010503 Code for thread local data in localeconv.
 * mm  010507 Reorganized the structure lconv to match C99
 * mm  010521 Added _MWMT wrappers
 * cc  010531 Added _GetThreadLocalData's flag
 * cc  011128 Made __tls _MSL_TLS
 * mm  011130 Additions and changes for implementation of locale
 * cc  011203 Added _MSL_CDECL for new name mangling
 * mm  011205 Changed _LOCALE to __locale
 * mm  020108 Corrected name of implementation locale
 * mm  020118 Corrected some errors in locale implementation
 * JWW 020130 Changed _MWMT to _MSL_THREADSAFE for consistency's sake
 * mm  020212 Added choice of ctype component
 * JWW 020304 Fixed the unnamed locale to be named "" instead of " "
 * JWW 020305 Changed to use new "wider is better" ctype classification tables
 * mm  020404 Added __NO_WIDE_CHAR wrappers to ctype locale components
 * JWW 020604 Added _MSL_LOCALDATA macro to make multithread code easier to read
 * mm  020606 Changes to allow reading of user-defined locale specification file.
 * mm  020626 Deleted IsDaylightSaving from _loc_tim_C
 * mm  020704 Some minor corrections to reading a locale specification file
 * mm  020710 Added time zone field initialization to locale time component.
 * mm  020718 Addec changes for reading collate specification
 * mm  020731 Changes to allow for working of Windows string functions with NULL arguments
 * mm  020911 Changes to allow locale specification files for different platforms
 * BLC 020924 Fixed implicit conversion warning
 * cc  021001 Added support for _MSL_C_LOCALE_ONLY
 * JWW 021031 Made thread local data available for Mach-O
 * mm  021204 Changes to make C locale match TC1 to C99
 * mm  030204 Inserted some missing wide char wrappers
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030321 Added _MSL_C99 wrappers
 * cc  030327 Added  missing _loc_coll_C and _loc_ctyp_C to _current_locale
 * cc  040103 Blocked _loc_coll_C with __NO_WIDE_CHAR 
 * mm  030528 Added _MSL_C99 wrappers
 * mm  040525 Rewrite of C locale collate table
 */