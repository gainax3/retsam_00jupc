/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/05/13 15:53:42 $ 
 * $Revision: 1.1 $ 
 */ 


/*
 *	UnwindInfo.h	-	C++ Exception Table Unwind Info for ARM family
 *
 *	THEORY OF OPERATION
 *
 *	The Exception Table for a function contains a target-specific variable-length structure
 *	that includes all information necessary to unwind the stack frame for the function.
 *	An ARM stack frame looks like this:
 *
 *				HIGH ADDRESSES	|						|
 *								|						|
 *								|						|
 *								|-----------------------|
 *								|	incoming arguments	|
 *								|		> 4 words		|
 *				Caller's SP   =>|-----------------------|
 *								|	incoming arguments	|	Optional flushback area for
 *								|	 	(4 words)		|	arguments passed in a1-a4
 *								|-----------------------|
 *								|	saved link register	|
 *								|-----------------------|
 *								|	saved GPRs v1-v8	|
 *								|-----------------------|
 *								|		Callee's		|
 *								|	local variables		|
 *								|  and spill locations	|
 *								|-----------------------|
 *								|	outgoing arguments	|
 *								|		> 4 words		|
 *				Callee's SP   =>|-----------------------|
 *								|						|
 *								|						|
 *				LOW ADDRESSES	|						|
 *
 *	In order to unwind the stack frame, then, we need the following information:
 *
 *	(1)	Frame Size		Since there's no back-link in the frame, we need this to
 *						pop to the caller's frame.
 *
 *	(2)	Argument Size	When we transfer control to a catch-block, the thrower's frame
 *						is still on the stack (because the exception temporary was
 *						allocated there) and if the thrower's argument area is smaller
 *						than the catcher's then we need to increase it before jumping
 *						to the catch-block. Since this information is only necessary for
 *						routines containing a catch-block, so we can tie this to the
 *						"Separate FP" flag described below.
 *
 *	(3)	Saved GPRs		We need this to restore any of the nonvolatile registers (v1-v8)
 *						that were saved in the frame.
 *
 *	(4)	ARM or Thumb?	We need to know if the routine corresponding to the frame was compiled
 *						into Thumb or ARM instructions, in order to properly transfer control
 *						back into a catcher. This flag also tells us which register is being
 *						used as the Frame Pointer.
 *
 *	(5)	Dynamic Stack?	We need to know if the frame uses a separate frame pointer--v8 on ARM,
 *						v4 on Thumb--for accessing locals etc. This will be true for any function
 *						which calls alloca() or which contains a catch-block.
 *
 *	(6)	Has Flushback?	We need to know if the 16-byte flushback area is present in the frame.
 *
 *	We represent the Unwind Info using a variable-length structure:
 *
 *	|<------1 bit------->|<-----1 bit---->|<-----1 bit---->|<--5 bits-->|
 *	+--------------------+----------------+----------------+------------+
 *	| ARM(0) or Thumb(1) | Dynamic Stack? | Has Flushback? | (reserved) |
 *	+--------------------+----------------+----------------+------------+
 *	| Saved Register Mask (v8 v7 v6 v5 v4 v3 v2 v1)						|
 *	+-------------------------------------------------------------------+
 *	| Frame Size (1/2/3/4 bytes as needed)								|
 *	+-------------------------------------------------------------------+
 *	| Argument Size (0/1/2/3/4 bytes as needed) 				 		|
 *	+-------------------------------------------------------------------+
 *
 *	The Frame Size and Outgoing Argument Area Size are represented using the variable-length
 *	unsigned integer format. For most functions, these will occupy only 1 byte each, so the
 *	entire Unwind Info for a function will typically occupy 3 bytes.
 *
 */

#ifndef __UNWINDINFO_H__
#define __UNWINDINFO_H__


#define	UNWIND_FRAME_IS_THUMB		0x80	/*	routine corresponding to frame uses Thumb code	*/
#define	UNWIND_FRAME_IS_DYNAMIC		0x40	/*	routine has a dynamic stack frame and uses a separate FP	*/
#define	UNWIND_FRAME_HAS_FLUSHBACK	0x20	/*	flushback area is present in frame	*/


#endif
