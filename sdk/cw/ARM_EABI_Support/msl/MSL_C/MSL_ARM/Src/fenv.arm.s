;/* MSL
; * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
; *
; * $Date: 2006/02/09 16:57:36 $
; * $Revision: 1.2.4.2 $
; */
;
;/* 
;	SUMMARY: definitions for the following
;
;		FECLEAREXCEPT - see C99 std for exact definitions of functions.
;		FEGETEXCEPTFLAG 
;		FERAISEEXCEPT 
;		FESETEXCEPTFLAG
;		FETESTEXCEPT 
;		
;		FEGETROUND 
;		FESETROUND
;		
;		FEGETENV
;		FEHOLDEXCEPT 
;		FESETENV 
;		FEUPDATEENV 
;
;
;	NOTE: Functions that modify the FPSCR have a processor-wide effect - they are not thread-safe.
;
;*/

;/* DEFAULT FP ENVRONMENT */

;fenv_t  _FE_DFL_ENV =  0x00000000;	;/* round to nearest, FP exceptions and flush-to-zero disabled, scalar operations, stride = 1 */
		.bss
_FE_DFL_ENV:	.space	4

;/* DEFINE FUNCTIONS */

		.extern	__SoftFPSCR__

		.text

;/*
;FECLEAREXCEPT  
;DESCRIPTION: The feclearexcept function clears the supported floating-point exceptions represented by its argument. 
;*/
;_MSL_IMP_EXP_C void feclearexcept ( int excepts )

		.global	feclearexcept

feclearexcept:

#if	defined(__HARDWARE_FP)
#if	defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		and		r0,r0,#0x0000001F	;/* ignore attempts to change non-exception flag bits */
		bic		r1,r1,r0			;/* clear bits set in excepts */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r1,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r1			;/* store to FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR address
		str		r1,[r2]				; store to FPSCR
#endif
		bx		lr


;/* 	
;FEGETEXCEPTFLAG 
;DESCRIPTION: The fegetexceptflag function stores an implementation-defined representation of the states of the
;             floating-point status flags indicated by the argument excepts in the object pointed to by the argument flagp. 
;*/
;_MSL_IMP_EXP_C void fegetexceptflag (fexcept_t *flagp, int excepts)

		.global	fegetexceptflag

fegetexceptflag:

#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r2,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r2,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR
		ldr		r2,[r2]
#endif
		and		r1,r1,#0x0000001F	;/* ignore attempts to return non-exception flag bits */
		and		r2,r2,r1			;/* use excepts to mask FPSCR bits */
		str		r2,[r0]				;/* return result in *flagp */
		bx		lr

;/* 	
;FERAISEEXCEPT 
;DESCRIPTION: The feraiseexcept function raises the supported floating-point exceptions represented by the argument.
;             The order in which these floating-point exceptions are raised is unspecified, except as stated in
;             ISO/IEC 9899:1999 F.7.6.  Whether the feraiseexcept function additionally raises the "inexact" floating-point
;             exception whenever it raise the "overflow" or "underflow" floating-point exception is implementation-defined. 
;*/
;_MSL_IMP_EXP_C void feraiseexcept (int excepts )

		.global	feraiseexcept

feraiseexcept:

;	;/* WARNING - This does not raise exceptions; it merely sets the sticky bits for them */

#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		and		r0,r0,#0x0000001F	;/* ignore attempts to change non-exception flag bits */
		orr		r1,r1,r0			;/* use except bits to set FPSCR flag bits */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r1,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r1			;/* store to FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR address
		str 	r1,[r2]				; store to FPSCR
#endif
		bx		lr

;/* 	
;FESETEXCEPTFLAG 
;DESCRIPTION: The fesetexceptflag function sets the floating-point status flags indicated by the argument excepts to
;             the states stored in the object pointed to by flagp. The value of *flagp shall have been set by a
;             previous call to fegetexceptflag whose 2nd argument represented at least those floating-point exceptions
;             represented by the argument excepts. This function does not raise floating-point exceptions, but only set
;             the state of the flags.
;*/
;_MSL_IMP_EXP_C void fesetexceptflag (const fexcept_t *flagp, int excepts)
		.global	fesetexceptflag

fesetexceptflag:
		ldr		r0,[r0]				;/* load *flagp */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r2,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r2,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR
		ldr		r2,[r2]
#endif
		and		r1,r1,#0x0000001F	;/* ignore attempts to set non-exception flag bits */
		and		r0,r0,r1			;/* use excepts bits to mask *flagp bits */
		bic		r2,r2,r1			;/* use excepts bits to clear FPSCR bits */
		orr		r2,r2,r0			;/* use masked *flagp bits to set FPSCR bits */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r2,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r2			;/* store to FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR address
		str 	r2,[r1]				; store to FPSCR
#endif
		bx		lr

;/* 	
;FETESTEXCEPT 
;DESCRIPTION: The fetestexcept function determines which of a specified subset of the floating-point exception flags
;             are currently set.  The excepts argument specifies the floating-point status flags to be queried.
;*/
;_MSL_IMP_EXP_C int fetestexcept(int excepts)

		.global	fetestexcept

fetestexcept:
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		and		r0,r0,#0x0000001F	;/* ignore attempts to test non-exception flag bits */
		and		r0,r1,r0			;/* return FPSR bit values for set except bits */
		bx		lr

;/* 
;FEGETROUND 
;DESCRIPTION: The fegetround function returns the value of the rounding direction macro representing the current
;             rounding direction or a negative number if there is no such rounding macro or the current rounding
;             direction is not determinable.
;*/  
;_MSL_IMP_EXP_C int fegetround (void)

		.global	fegetround

fegetround:

#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r0,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r0,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r0,=__SoftFPSCR__	; load FPSCR
		ldr		r0,[r0]
#endif
		and		r0,r0,#0x00C00000	;/* return rounding mode bits */
		bx		lr

;/* 
;FESETROUND 
;DESCRIPTION: fesetround function establishes the rounding direction represented by its argument round.  If the
;             argument is not equal to the value of a rounding direction macro, the rounding direction is not changed.
;*/
;_MSL_IMP_EXP_C int fesetround (int round )

		.global	fesetround

fesetround:
		and		r1,r0,#0x00C00000	;/* check that round is a rounding mode (only set bits are in bits[23:22]) */
		subs	r1,r1,r0
		movne	r0,#-1				;/* if not, return -1 */
		bxne	lr				
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r2,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r2,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR
		ldr		r2,[r2]
#endif
		bic		r2,r2,#0x00C0000	;/* clear rounding mode */
		orr		r2,r2,r0			;/* set bits as in except */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r2,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r2			;/* store to FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR address
		str 	r2,[r1]				; store to FPSCR
#endif
		mov		r0,#0				;/* return 0 */
		bx		lr

;/* 
;FEGETENV
;DESCRIPTION: The fegetenv function stores the current floating-point environment in the object pointed to by envp.
;*/
;_MSL_IMP_EXP_C void fegetenv (fenv_t *envp)

		.global	fegetenv

fegetenv:

#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		str		r1,[r0]				;/* store in *flagp */
		bx		lr

;/*
;FEHOLDEXCEPT 
;DESCRIPTION: The feholdexcept function saves the current floating-point environment in the object pointed to by envp,
;			 clears the floating-point status flags, and then installs a non-stop (continues on floating-point
;			 exceptions) mode, if available, for all floating-point exceptions.
;			 It returns zero if and only if non-stop floating-point exception handling was successfully installed.
;*/
;_MSL_IMP_EXP_C int feholdexcept (fenv_t *envp)

		.global	feholdexcept

feholdexcept:

#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		str		r1,[r0]				;/* store in *envp */
		bic		r1,r1,#0x0000001F	;/* clear status flags */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r1,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r1			;/* store to FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR address
		str 	r1,[r2]				; store to FPSCR
#endif
		mov		r0,#-1				;/* signal that "non-stop" was not installed */
		bx		lr

;/* 
;FESETENV 
;DESCRIPTION: The fesetenv function establishes the floating-point environment represented by the object pointed to by
;			 envp. The argument envp shall point to an object set by a call to fegetenv or feholdenv, or equal a
;			 floating-point macro.  Note that fesetenv merely installs the state of the floating-point status flags
;			 represented through its argument, and does not raise these floating-point exceptions.
;*/
;_MSL_IMP_EXP_C void fesetenv (const fenv_t *envp)

		.global	fesetenv

fesetenv:
		ldr		r0,[r0]				;/* load *envp */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r0,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r0			;/* store to FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR address
		str 	r0,[r1]				; store to FPSCR
#endif
		bx		lr

;/* 
;FEUPDATEENV 
;DESCRIPTION: The feupdateenv function saves the currently raised floating-point exceptions in its automatic storage,
;			 installs the floating-point environment represented by the object pointed to by envp, and then raises
;			 the saved floating-point exceptions.  The argument envp shall point to an object set by a call to
;			 feholdexcept or fegetenv, or equal a floating-point macro.
;*/
;_MSL_IMP_EXP_C void feupdateenv (const fenv_t *envp)

		.global	feupdateenv
		
feupdateenv:
		ldr		r0,[r0]				;/* load *envp */
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mrc		p10,7,r1,c1,c0,0	;/* load FPSCR */
#else
		fmrx	r1,FPSCR			;/* load FPSCR */
#endif
#else
		ldr		r1,=__SoftFPSCR__	; load FPSCR
		ldr		r1,[r1]
#endif
		and		r1,r1,#0x0000001F	;/* add the "raised" exceptions of the FPSR to *envp */
		orr		r1,r1,r0
#if	defined(__HARDWARE_FP)
#if defined(__ARMCC_VERSION)
		mcr		p10,7,r1,c1,c0,0	;/* store to FPSCR */
#else
		fmxr	FPSCR,r1			;/* store to FPSCR */
#endif
#else
		ldr		r2,=__SoftFPSCR__	; load FPSCR address
		str 	r1,[r2]				; store to FPSCR
#endif
		bx		lr
