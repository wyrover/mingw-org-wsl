/**
 * @file fesetenv.c
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <fenv.h>
#include <float.h>
#include "cpu_features.h"

/* 7.6.4.3
   The fesetenv function establishes the floating-point environment
   represented by the object pointed to by envp. The argument envp
   points to an object set by a call to fegetenv or feholdexcept, or
   equal the macro FE_DFL_ENV or an implementation-defined environment
   macro. Note that fesetenv merely installs the state of the exception
   flags represented through its argument, and does not raise these
   exceptions.
 */

extern void (*_imp___fpreset)( void ) ;

int fesetenv (const fenv_t * envp)
{
  /* Default mxcsr status is to mask all exceptions.  All other bits
     are zero.  */
     
  unsigned int _csr = FE_ALL_EXCEPT << __MXCSR_EXCEPT_MASK_SHIFT /*= 0x1f80 */; 
  
  if (envp == FE_PC64_ENV)
   /*
    *  fninit initializes the control register to 0x37f,
    *  the status register to zero and the tag word to 0FFFFh.
    *  The other registers are unaffected.
    */
    __asm__ ("fninit");

  else if (envp == FE_PC53_ENV)
   /*
    * MS _fpreset() does same *except* it sets control word
    * to 0x27f (53-bit precison).
    * We force calling _fpreset in msvcrt.dll
    */

   (*_imp___fpreset)();

  else if (envp == FE_DFL_ENV)
    /* Use the choice made at app startup */ 
    _fpreset();

  else
    {
      __asm__ ("fldenv %0;" : : "m" (*envp));
       /* Setting the reserved high order bits of MXCSR causes a segfault */
       _csr = envp ->__mxcsr & 0xffff;
    }

  /* Set MXCSR */   
   if (__HAS_SSE)
     __asm__ volatile ("ldmxcsr %0" : : "m" (_csr));
 
  return 0;
}
