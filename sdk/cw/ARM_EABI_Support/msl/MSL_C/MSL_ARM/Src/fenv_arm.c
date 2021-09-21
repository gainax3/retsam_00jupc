/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:57:36 $
 * $Revision: 1.5.4.2 $
 */

/* $Id: fenv_arm.c,v 1.5.4.2 2006/02/09 16:57:36 cgalvan Exp $ */

#include <fenv.h>
#include <signal.h>
#if _MSL_C99
_MSL_BEGIN_EXTERN_C

/* Provide a constant default floating point environment */

fenv_t _FE_DFL_ENV = 
  { 0, /* round to nearest, FP exceptions and flush-to-zero disabled, scalar operations, stride = 1 */
    (__ieee_handler_t)0, 
    (__ieee_handler_t)0, 
    (__ieee_handler_t)0, 
    (__ieee_handler_t)0, 
    (__ieee_handler_t)0
  };

/*
 * Exception functions.
 */
/* 	
FEGETEXCEPTFLAG 
DESCRIPTION:The fegetexceptflag function stores an implementation-defined 
            representation of the states of the floating-point status flags 
            indicated by the argument excepts in the object pointed to by the 
            argument flagp. 
*/
void fegetexceptflag(fexcept_t *flagp, int excepts)
  {  
  fenv_t * crtEnv;
  
  /* save the sticky flags and the exception enable flags */
  excepts &= FE_ALL_EXCEPT;  
  flagp->statusword = __ieee_status(0,0) & ( excepts | (excepts << 8) ); 
  
  /* save the exception enable flags */
  crtEnv = __rt_fp_env_addr();
  if (excepts & FE_INVALID)   flagp->invalid_handler   = crtEnv->invalid_handler;
  if (excepts & FE_DIVBYZERO) flagp->divbyzero_handler = crtEnv->divbyzero_handler;
  if (excepts & FE_OVERFLOW)  flagp->overflow_handler  = crtEnv->overflow_handler;
  if (excepts & FE_UNDERFLOW) flagp->underflow_handler = crtEnv->underflow_handler;
  if (excepts & FE_INEXACT)   flagp->inexact_handler   = crtEnv->inexact_handler;
  }

/* 	
FESETEXCEPTFLAG 
DESCRIPTION:The fesetexceptflag function sets the floating-point status flags 
            indicated by the argument excepts to the states stored in the object 
            pointed to by flagp. The value of *flagp shall have been set by a
            previous call to fegetexceptflag whose 2nd argument represented at 
            least those floating-point exceptions represented by the argument 
            excepts. This function does not raise floating-point exceptions, but 
            only sets the state of the flags.
*/
void fesetexceptflag(const fexcept_t *flagp, int excepts)
  {
  fenv_t * crtEnv;

  excepts &= FE_ALL_EXCEPT;  
  excepts |= (excepts << 8); /* Restore both sticky and exception enable flags */
  __ieee_status(excepts, flagp->statusword & excepts); 
  
  /* restore the exception enable flags */
  crtEnv = __rt_fp_env_addr();
  if (excepts & FE_INVALID)   crtEnv->invalid_handler   = flagp->invalid_handler;
  if (excepts & FE_DIVBYZERO) crtEnv->divbyzero_handler = flagp->divbyzero_handler;
  if (excepts & FE_OVERFLOW)  crtEnv->overflow_handler  = flagp->overflow_handler;
  if (excepts & FE_UNDERFLOW) crtEnv->underflow_handler = flagp->underflow_handler;
  if (excepts & FE_INEXACT)   crtEnv->inexact_handler   = flagp->inexact_handler;
  }

/*
FECLEAREXCEPT  
DESCRIPTION:The feclearexcept function clears the supported floating-point 
            exceptions represented by its argument. 
*/
void feclearexcept(int excepts)
  {
  __ieee_status(excepts & FE_ALL_EXCEPT,0);
  }

/*
  Helper function: raiseEnabledExceptions
  DESCRIPTION: Call the handlers for the exceptions having "enable" bits in the exceptionMask
*/ 

static void raiseEnabledExceptions(int exceptionMask)
  {
  __ieee_value_t dummy = {0};
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  if (exceptionMask & FE_IEEE_MASK_INVALID)
    {
    if (crtEnv->invalid_handler == (__ieee_handler_t)0) 
      raise(SIGFPE);
    else 
      (crtEnv->invalid_handler)(dummy, dummy, FE_EX_FN_RAISE);
    }

  if (exceptionMask & FE_IEEE_MASK_DIVBYZERO)
    {
    if (crtEnv->divbyzero_handler == (__ieee_handler_t)0) 
      raise(SIGFPE);
    else 
      (crtEnv->divbyzero_handler)(dummy, dummy, FE_EX_FN_RAISE);
    }

  if (exceptionMask & FE_IEEE_MASK_OVERFLOW)
    {
    if (crtEnv->overflow_handler == (__ieee_handler_t)0) 
      raise(SIGFPE);
    else 
      (crtEnv->overflow_handler)(dummy, dummy, FE_EX_FN_RAISE);
    }

  if (exceptionMask & FE_IEEE_MASK_UNDERFLOW)
    {
    if (crtEnv->underflow_handler == (__ieee_handler_t)0) 
      raise(SIGFPE);
    else 
      (crtEnv->underflow_handler)(dummy, dummy, FE_EX_FN_RAISE);
    }

  if (exceptionMask & FE_IEEE_MASK_INEXACT)
    {
    if (crtEnv->inexact_handler == (__ieee_handler_t)0) 
      raise(SIGFPE);
    else 
      (crtEnv->inexact_handler)(dummy, dummy, FE_EX_FN_RAISE);
    }
  }  
  
/* 	
FERAISEEXCEPT 
DESCRIPTION:The feraiseexcept function raises the supported floating-point 
            exceptions represented by the argument. The order in which these 
            floating-point exceptions are raised is unspecified, except as 
            stated in ISO/IEC 9899:1999 F.7.6.  Whether the feraiseexcept 
            function additionally raises the "inexact" floating-point
            exception whenever it raise the "overflow" or "underflow" 
            floating-point exception is implementation-defined. 
*/
void feraiseexcept(int excepts)
  {
  excepts &= FE_ALL_EXCEPT;  

  /* Set the sticky flags, retrieve the exception enable bits, invoke handlers */
  raiseEnabledExceptions( __ieee_status(excepts, excepts) & (excepts << 8) );
  }

/* 	
FETESTEXCEPT 
DESCRIPTION: The fetestexcept function determines which of a specified subset of 
             the floating-point exception flags are currently set.  The excepts 
             argument specifies the floating-point status flags to be queried.
*/
int fetestexcept(int excepts)
  {
  return __ieee_status(0,0) & excepts & FE_ALL_EXCEPT;
  }

/*
 * Rounding functions.
 */

/* 
FEGETROUND 
DESCRIPTION:The fegetround function returns the value of the rounding direction 
            macro representing the current rounding direction or a negative 
            number if there is no such rounding macro or the current rounding
            direction is not determinable.
*/  
int fegetround(void)
  {
  return __ieee_status(0,0) & FE_IEEE_ROUND_MASK;
  }

/* 
FESETROUND 
DESCRIPTION:fesetround function establishes the rounding direction represented 
            by its argument round.  If the argument is not equal to the value of 
            a rounding direction macro, the rounding direction is not changed.
*/
int fesetround(int round)
  {
  __ieee_status(FE_IEEE_ROUND_MASK, round & FE_IEEE_ROUND_MASK);
  return round != (__ieee_status(0,0) & FE_IEEE_ROUND_MASK);
  }

/*
 * Full environment access.
 */

/* 
FEGETENV
DESCRIPTION: The fegetenv function stores the current floating-point environment 
             in the object pointed to by envp.
*/
void fegetenv(fenv_t *envp)
  {
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  envp -> statusword = __ieee_status(0,0);
  envp -> inexact_handler   = crtEnv->inexact_handler;
  envp -> overflow_handler  = crtEnv->overflow_handler;
  envp -> underflow_handler = crtEnv->underflow_handler;
  envp -> invalid_handler   = crtEnv->invalid_handler;
  envp -> divbyzero_handler = crtEnv->divbyzero_handler;        
  }  

/* 
FESETENV 
DESCRIPTION:The fesetenv function establishes the floating-point environment 
            represented by the object pointed to by envp. The argument envp 
            shall point to an object set by a call to fegetenv or feholdenv. 
            Note that fesetenv merely installs the state of the floating-point 
            status flags represented through its argument, and does not raise 
            these floating-point exceptions.
*/
void fesetenv(const fenv_t *envp)
  {
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  __ieee_status(0xFFFFFFFF, envp -> statusword);
  crtEnv->inexact_handler   = envp -> inexact_handler;  
  crtEnv->overflow_handler  = envp -> overflow_handler;
  crtEnv->underflow_handler = envp -> underflow_handler;
  crtEnv->invalid_handler   = envp -> invalid_handler;
  crtEnv->divbyzero_handler = envp -> divbyzero_handler;        
  }

/*
FEHOLDEXCEPT 
DESCRIPTION:The feholdexcept function saves the current floating-point 
            environment in the object pointed to by envp, clears the 
            floating-point status flags, and then installs a non-stop (continues
            on floating-point exceptions) mode, if available, for all 
            floating-point exceptions. It returns zero if and only if non-stop 
            floating-point exception handling was successfully installed.
*/
int feholdexcept(fenv_t *envp)
  {
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  envp -> statusword = __ieee_status(FE_IEEE_MASK_ALL_EXCEPT | FE_IEEE_ALL_EXCEPT,0);
  envp -> inexact_handler   = crtEnv->inexact_handler;
  envp -> overflow_handler  = crtEnv->overflow_handler;
  envp -> underflow_handler = crtEnv->underflow_handler;
  envp -> invalid_handler   = crtEnv->invalid_handler;
  envp -> divbyzero_handler = crtEnv->divbyzero_handler;
  return 0;
  }
  
/* 
FEUPDATEENV 
DESCRIPTION:The feupdateenv function saves the currently raised floating-point 
            exceptions in its automatic storage, installs the floating-point 
            environment represented by the object pointed to by envp, and then 
            raises the saved floating-point exceptions.  The argument envp shall
            point to an object set by a call to feholdexcept or fegetenv.
*/
void feupdateenv(const fenv_t *envp)
  {
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  crtEnv->inexact_handler   = envp -> inexact_handler;  
  crtEnv->overflow_handler  = envp -> overflow_handler;
  crtEnv->underflow_handler = envp -> underflow_handler;
  crtEnv->invalid_handler   = envp -> invalid_handler;
  crtEnv->divbyzero_handler = envp -> divbyzero_handler;        
  raiseEnabledExceptions(envp->statusword & __ieee_status(0xFFFFFFFF, envp->statusword) << 8);
  }

_MSL_END_EXTERN_C
#endif /*_MSL_C99*/

/* Change record: 
 * cc  040128 Added C99 flag
 */
