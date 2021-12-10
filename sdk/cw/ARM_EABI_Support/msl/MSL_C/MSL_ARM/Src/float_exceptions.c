/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:57:36 $
 * $Revision: 1.5.4.2 $
 */

/* $Id: float_exceptions.c,v 1.5.4.2 2006/02/09 16:57:36 cgalvan Exp $ */
#include <fenv.h>
#include <signal.h>
#if _MSL_C99

_MSL_BEGIN_EXTERN_C

/* 
   The following functions are called from fplib when encountering trapped
   exceptions. They should set the rounding mode in the edata parameter if it
   has not already been set and invoke the user-defined handler or raise SIGFPE 
   if there is no handler installed.
   
   The edata parameters always contains a single exception bit that is set.
   (OVERFLOW/UNDERFLOW and INEXACT are never simultaneously set)
 */

int __fp_handler_32_to_32(int arg, unsigned int edata);
int __fp_handler_64_to_32(long long arg, unsigned int edata);
long long __fp_handler_32_to_64(int arg, unsigned int edata);
long long __fp_handler_64_to_64(long long arg, unsigned int edata);
int __fp_handler_binary_32_to_32(int arg1, int arg2, unsigned int edata);
long long __fp_handler_binary_64_to_64(long long arg1, long long arg2, unsigned int edata);
int __fp_handler_binary_64_to_32(long long arg1, long long arg2, unsigned int edata);

static __ieee_value_t callHandler(
          __ieee_value_t aOp1,                        
          __ieee_value_t aOp2,
          unsigned int edata)
{
  __ieee_handler_t aHandler;
  fenv_t * crtEnv;

  crtEnv = __rt_fp_env_addr();
  
/* Set rounding mode if not specified by the calling routine */

  if ((edata & FE_IEEE_ROUND_MASK) == 0)
    edata |= __ieee_status(0,0) & FE_IEEE_ROUND_MASK;

/* Select the exception handler to call. Assert that only one exception is
   signalled at a time (no underflow + inexact or overflow + inexact) */

  switch (edata & FE_EX_EXCEPT_MASK) 
    {
  case FE_EX_INEXACT:
    aHandler = crtEnv->inexact_handler;
    break;
  case FE_EX_INVALID:
    aHandler = crtEnv->invalid_handler;
    break;
  case FE_EX_OVERFLOW:
    aHandler = crtEnv->overflow_handler;
    break;
  case FE_EX_UNDERFLOW:
    aHandler = crtEnv->underflow_handler;
    break;
  case FE_EX_DIVBYZERO:
    aHandler = crtEnv->divbyzero_handler;
    break;
  default:
    aHandler = (__ieee_handler_t)0;
    }
    
  if(aHandler == (__ieee_handler_t)0)
    raise(SIGFPE);

  return (aHandler)(aOp1, aOp2, edata);
}                        

int __fp_handler_32_to_32(int arg, unsigned int edata)
{
  __ieee_value_t op1, res;
  op1.i = arg;
  res=callHandler(op1,op1,edata);
  return res.i; 
}

int __fp_handler_64_to_32(long long arg, unsigned int edata)
{
  __ieee_value_t op1, res;
  op1.l = arg;
  res=callHandler(op1,op1,edata);
  return res.i; 
}

long long __fp_handler_32_to_64(int arg, unsigned int edata)
{
  __ieee_value_t op1, res;
  op1.i = arg;
  res=callHandler(op1,op1,edata);
  return res.l; 
}

long long __fp_handler_64_to_64(long long arg, unsigned int edata)
{
  __ieee_value_t op1, res;
  op1.l = arg;
  res=callHandler(op1,op1,edata);
  return res.l; 
}

int __fp_handler_binary_32_to_32(int arg1, int arg2, unsigned int edata)
{
  __ieee_value_t op1, op2, res;
  op1.i = arg1;
  op2.i = arg2;
  res=callHandler(op1,op2,edata);
  return res.i; 
}

long long __fp_handler_binary_64_to_64(long long arg1, long long arg2, unsigned int edata)
{
  __ieee_value_t op1, op2, res;
  op1.l = arg1;
  op2.l = arg2;
  res=callHandler(op1,op2,edata);
  return res.l; 
}

int __fp_handler_binary_64_to_32(long long arg1, long long arg2, unsigned int edata)
{
  __ieee_value_t op1, op2, res;
  op1.l = arg1;
  op2.l = arg2;
  res=callHandler(op1,op2,edata);
  return res.i; 
}

_MSL_END_EXTERN_C
#endif /* _MSL_C99 */

/* Change record: 
 * cc  040128 Added C99 flag
 */