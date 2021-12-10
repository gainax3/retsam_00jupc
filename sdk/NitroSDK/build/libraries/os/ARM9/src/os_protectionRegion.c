/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_protectionRegion.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_protectionRegion.c,v $
  Revision 1.11  2005/01/11 07:42:23  takano_makoto
  fix copyright header.

  Revision 1.10  2005/01/05 02:19:11  yada
  fix OS_DisableWriteBufferForProtectionRegion().  orr -> bic

  Revision 1.9  2004/11/02 10:40:48  yada
  add definition about protection region for convinience

  Revision 1.8  2004/09/30 00:12:04  yada
  fix typo

  Revision 1.7  2004/09/02 06:30:11  yada
  only change comment

  Revision 1.6  2004/04/06 04:31:27  yada
  fix small bug

  Revision 1.5  2004/03/24 00:36:19  yada
  SDK_BB îpé~Ç…Ç¬Ç´èCê≥

  Revision 1.4  2004/02/09 11:17:22  yasu
  include code32.h

  Revision 1.3  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.2  2004/01/18 02:25:37  yada
  ÉCÉìÉfÉìÉgìôÇÃêÆå`

  Revision 1.1  2004/01/16 07:57:15  yada
  protectionUnit ãLèq


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/code32.h>

//================================================================================
//               Instruction Cache for each protection region
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableICacheForProtectionRegion
                OS_DisableICacheForProtectionRegion

  Description:  enable/disable instruction cache for each protection region

  Arguments:    flags :  Each bit shows the region to be enable/disable.
                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_EnableICacheForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c2, c0, 1
    orr     r1, r1, r0
    mcr     p15, 0, r1, c2, c0, 1
    bx      lr
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
asm void OS_DisableICacheForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c2, c0, 1
    bic     r1, r1, r0
    mcr     p15, 0, r1, c2, c0, 1
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetICacheEnableFlagsForProtectionRegion

  Description:  get instruction cache status for each protection region

  Arguments:    None

  Returns:      instruction cache status.
                bit for each protection region, means 1... enable, 0... disable

                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

 *---------------------------------------------------------------------------*/
asm u32 OS_GetICacheEnableFlagsForProtectionRegion( void )
{
    mrc     p15, 0, r0, c2, c0, 1
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetIPermissionsForProtectionRegion

  Description:  set instruction access limitation for each protection region

  Arguments:    setMask : set mask bit, which is or-ed by mask.
                flags   : flag bit, which is or-ed by access limitation.

                   (mask)
                          OS_PRn_ACCESS_MASK (n=0-7)

                   (access limitation)
                          OS_PRn_ACCESS_NA    no permission to access
                          OS_PRn_ACCESS_RW    read and write permission
                          OS_PRn_ACCESS_RO    read only permission
                          (n=0-7)

                ex.
                    In case to set region3 readonly and region6 no permission, call
                    OS_SetIPermissionsForProtectionRegion(
                             OS_PR3_ACCESS_MASK | OS_PR6_ACCESS_MASK,
                             OS_PR3_ACCESS_RO | OS_PR6_ACCESS_NA );

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_SetIPermissionsForProtectionRegion( register u32 setMask, register u32 flags )
{
    mrc     p15, 0, r2, c5, c0, 3
    bic     r2, r2, r0
    orr     r2, r2, r1
    mcr     p15, 0, r2, c5, c0, 3
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetIPermissionsForProtectionRegion

  Description:  get instruction access limitation for each protection region

  Arguments:    None

  Returns:      Return value & OS_PRn_ACCESS_MASK (n=0-7) shows access limitation.

                   (access limitation)
                          OS_PRn_ACCESS_NA    no permission to access
                          OS_PRn_ACCESS_RW    read and write permission
                          OS_PRn_ACCESS_RO    read only permission
                          (n=0-7)

 *---------------------------------------------------------------------------*/
asm u32 OS_GetIPermissionsForProtectionRegion( void )
{
    mrc     p15, 0, r0, c5, c0, 3
    bx      lr
}

//================================================================================
//               Data Cache for each protection region
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableDCacheForProtectionRegion
                OS_DisableDCacheForProtectionRegion

  Description:  enable/disable data cache for each protection region

  Arguments:    flags :  Each bit shows the region to be enable/disable.
                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_EnableDCacheForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c2, c0, 0
    orr     r1, r1, r0
    mcr     p15, 0, r1, c2, c0, 0
    bx      lr
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
asm void OS_DisableDCacheForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c2, c0, 0
    bic     r1, r1, r0
    mcr     p15, 0, r1, c2, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetDCacheEnableFlagsForProtectionRegion

  Description:  get data cache status for each protection region

  Arguments:    None

  Returns:      data cache status.
                bit for each protection region, means 1... enable, 0... disable

                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

 *---------------------------------------------------------------------------*/
asm u32 OS_GetDCacheEnableFlagsForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r0, c2, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetDPermissionsForProtectionRegion

  Description:  set data access limitation for each protection region

  Arguments:    setMask : set mask bit, which is or-ed by mask.
                flags   : flag bit, which is or-ed by access limitation.

                   (mask)
                          OS_PRn_ACCESS_MASK (n=0-7)

                   (access limitation)
                          OS_PRn_ACCESS_NA    no permission to access
                          OS_PRn_ACCESS_RW    read and write permission
                          OS_PRn_ACCESS_RO    read only permission
                          (n=0-7)

                ex.
                    In case to set region3 readonly and region6 no permission, call
                    OS_SetDPermissionsForProtectionRegion(
                             OS_PR3_ACCESS_MASK | OS_PR6_ACCESS_MASK,
                             OS_PR3_ACCESS_RO | OS_PR6_ACCESS_NA );

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_SetDPermissionsForProtectionRegion( register u32 setMask, register u32 flags )
{
    mrc     p15, 0, r2, c5, c0, 2
    bic     r2, r2, r0
    orr     r2, r2, r1
    mcr     p15, 0, r2, c5, c0, 2
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetDPermissionsForProtectionRegion

  Description:  get data access limitation for each protection region

  Arguments:    None

  Returns:      Return value & OS_PRn_ACCESS_MASK (n=0-7) shows access limitation.

                   (access limitation)
                          OS_PRn_ACCESS_NA    no permission to access
                          OS_PRn_ACCESS_RW    read and write permission
                          OS_PRn_ACCESS_RO    read only permission
                          (n=0-7)

 *---------------------------------------------------------------------------*/
asm u32 OS_GetDPermissionsForProtectionRegion( void )
{
    mrc     p15, 0, r0, c5, c0, 2
    bx      lr
}

//================================================================================
//               Write buffer for each protection region
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableWriteBufferForProtectionRegion
                OS_DisableWriteBufferForProtectionRegion

  Description:  enable/disable write buffer for each protection region

  Arguments:    flags :  Each bit shows the region to be enable/disable.
                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_EnableWriteBufferForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c3, c0, 0
    orr     r1, r1, r0
    mcr     p15, 0, r1, c3, c0, 0
    bx      lr
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
asm void OS_DisableWriteBufferForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r1, c3, c0, 0
    bic     r1, r1, r0
    mcr     p15, 0, r1, c3, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetWriteBufferEnableFlagsForProtectionRegion

  Description:  get write buffer status for each protection region

  Arguments:    None

  Returns:      write buffer status.
                bit for each protection region, means 1... enable, 0... disable

                           bit0(LSB) ... protection region 0
                           bit1      ... protection region 1
                             :                  :
                           bit7      ... protection region 7

 *---------------------------------------------------------------------------*/
asm u32 OS_GetWriteBufferEnableFlagsForProtectionRegion( register u32 flags )
{
    mrc     p15, 0, r0, c3, c0, 0
    bx      lr
}

//================================================================================
//          protection region
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetProtectionRegion*

  Description:  set up parameter of each protection region

  Arguments:    param    :   parameter ( created from base address and size )

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_SetProtectionRegion0( u32 param )
{
    mcr     p15, 0, r0, c6, c0, 0
    bx      lr
}

asm void OS_SetProtectionRegion1( u32 param )
{
    mcr     p15, 0, r0, c6, c1, 0
    bx      lr
}

asm void OS_SetProtectionRegion2( u32 param )
{
    mcr     p15, 0, r0, c6, c2, 0
    bx      lr
}

asm void OS_SetProtectionRegion3( u32 param )
{
    mcr     p15, 0, r0, c6, c3, 0
    bx      lr
}

asm void OS_SetProtectionRegion4( u32 param )
{
    mcr     p15, 0, r0, c6, c4, 0
    bx      lr
}

asm void OS_SetProtectionRegion5( u32 param )
{
    mcr     p15, 0, r0, c6, c5, 0
    bx      lr
}

asm void OS_SetProtectionRegion6( u32 param )
{
    mcr     p15, 0, r0, c6, c6, 0
    bx      lr
}

asm void OS_SetProtectionRegion7( u32 param )
{
    mcr     p15, 0, r0, c6, c7, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetProtectionRegionParam

  Description:  get parameter of each protection region

  Arguments:    None

  Returns:      parameter of protection region ( created from base address and size )
 *---------------------------------------------------------------------------*/
asm u32 OS_GetProtectionRegion0( void )
{
    mrc     p15, 0, r0, c6, c0, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion1( void )
{
    mrc     p15, 0, r0, c6, c1, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion2( void )
{
    mrc     p15, 0, r0, c6, c2, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion3( void )
{
    mrc     p15, 0, r0, c6, c3, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion4( void )
{
    mrc     p15, 0, r0, c6, c4, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion5( void )
{
    mrc     p15, 0, r0, c6, c5, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion6( void )
{
    mrc     p15, 0, r0, c6, c6, 0
    bx      lr
}

asm u32 OS_GetProtectionRegion7( void )
{
    mrc     p15, 0, r0, c6, c7, 0
    bx      lr
}

#include <nitro/codereset.h>
