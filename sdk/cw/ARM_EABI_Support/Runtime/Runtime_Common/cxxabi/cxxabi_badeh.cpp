/* Metrowerks Runtime Library
 * Copyright © 2003-2005 Metrowerks Corporation.  All rights reserved.
 *
 * Purpose:  Implementation of the portable Itanium C++ ABI
 *           see http://www.codesourcery.com/cxx-abi/
 *
 * $Date: 2005/03/07 21:32:38 $
 * $Revision: 1.2 $
 */

#include <cxxabi.h>
#include <typeinfo>

//	Raise a bad_cast exception (lib.bad.cast, 18.5.2).
//	Does not return (normally), and implementations may note this fact, e.g. with pragmas. 
extern "C" void __cxa_bad_cast ()
{
	throw std::bad_cast();
}


//	Raise a bad_typeid exception (lib.bad.typeid, 18.5.3).
//	Does not return (normally), and implementations may note this fact, e.g. with pragmas. 
extern "C" void __cxa_bad_typeid ()
{
	throw std::bad_typeid();
}
