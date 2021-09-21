/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     output_effect.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/output_effect.h>

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/snd/capture.h>
#include <nnsys/snd/config.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#define SAMPLING_RATE  32000

#define SURROUND_MIX_DELAY       2
#define HEADEPHONE_MIX_DELAY    24
#define HEADEPHONE_SHIFT         2

#define DECAY_RATIO    0x3000

#define S16_MAX ((s16)0x7fff)
#define S16_MIN ((s16)-0x8000)

/******************************************************************************
	type definition
 ******************************************************************************/
struct CallbackInfo;

typedef void (*EffectFunc)(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    struct CallbackInfo* info
);

typedef struct CallbackInfo
{
    NNSSndCaptureOutputEffectType type;
    EffectFunc effectFunc;
    NNSSndCaptureCallback cbFunc;
    void* cbArg;
    NNSSndCaptureCallback cbPostFunc;
    void* cbPostArg;
    union {
        s32 surround[ SURROUND_MIX_DELAY ];
        s32 headphone[ HEADEPHONE_MIX_DELAY ][2];
    } buffer;
} CallbackInfo;

/******************************************************************************
	static variable
 ******************************************************************************/

static CallbackInfo sCallbackInfo = {
    (NNSSndCaptureOutputEffectType)-1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/******************************************************************************
	static function declaration
 ******************************************************************************/
static void EffectCallback(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    NNSSndCaptureFormat format,
    void* arg
);
static void SurroundProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
);
static void NothingProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
);
#ifndef SDK_SMALL_BUILD
static void HeadphoneProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
);
static void MonoProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
);
#endif /* SDK_SMALL_BUILD */

static s16 Cut_S32toS16( s32 val );

/******************************************************************************
	inline function
 ******************************************************************************/

static NNS_SND_INLINE s16 Cut_S32toS16( s32 val )
{
    if ( val < S16_MIN ) val = S16_MIN;
    else if ( val > S16_MAX ) val = S16_MAX;
    return (s16)val;
}

/******************************************************************************
	public function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStartOutputEffect

  Description:  start output effect

  Arguments:    buffer  - effect buffer address
                bufSize - effect buffer size in bytes
                type    - effect type

  Returns:      return TRUE if success
                return FALSE if fail
 *---------------------------------------------------------------------------*/
BOOL NNS_SndCaptureStartOutputEffect(    
    void* buffer,
    u32 bufSize,
    NNSSndCaptureOutputEffectType type
)
{
    NNS_ASSERTMSG( ( (u32)buffer & 0x1f ) == 0, "buffer address must be aligned to 32 bytes boundary." );
    NNS_ASSERTMSG( (     bufSize & 0x7f ) == 0, "buffer size must be aligned to 128 bytes boundary.");
    
    NNS_SndCaptureChangeOutputEffect( type );
    
    return NNS_SndCaptureStartEffect(
        buffer,
        bufSize,
        NNS_SND_CAPTURE_FORMAT_PCM16,
        SAMPLING_RATE,
        2,
        EffectCallback,
        &sCallbackInfo
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStopOutputEffect

  Description:  stop surround

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureStopOutputEffect( void )
{
    if ( sCallbackInfo.type == NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND ) {
        SNDi_SetSurroundDecay( 0 );
    }
    
    NNS_SndCaptureStopEffect();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureChangeOutputEffect

  Description:  change output effect type

  Arguments:    type - output effect

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureChangeOutputEffect( NNSSndCaptureOutputEffectType type )
{
    OSIntrMode enabled;
    
    if ( type == sCallbackInfo.type ) return;
    
    if ( sCallbackInfo.type == NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND ) {
        SNDi_SetSurroundDecay( 0 );
    }
    
    enabled = OS_DisableInterrupts();
    
    MI_CpuClear16( & sCallbackInfo.buffer, sizeof( sCallbackInfo.buffer ) );
    sCallbackInfo.type = type;

    switch( type ) {
    case NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND:
        sCallbackInfo.effectFunc = SurroundProc;
        break;
#ifndef SDK_SMALL_BUILD
    case NNS_SND_CAPTURE_OUTPUT_EFFECT_HEADPHONE:
        sCallbackInfo.effectFunc = HeadphoneProc;
        break;
    case NNS_SND_CAPTURE_OUTPUT_EFFECT_MONO:
        sCallbackInfo.effectFunc = MonoProc;
        break;
    case NNS_SND_CAPTURE_OUTPUT_EFFECT_NORMAL:
        sCallbackInfo.effectFunc = NothingProc;
        break;
#endif /* SDK_SMALL_BUILD */
    default:
        sCallbackInfo.effectFunc = NothingProc;
        break;
    }
    
    (void)OS_RestoreInterrupts( enabled );
    
    if ( type == NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND ) {
        SNDi_SetSurroundDecay( DECAY_RATIO );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureSetOutputEffectCallback

  Description:  set output effect callback function

  Arguments:    func - callback function.
                arg  - function argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureSetOutputEffectCallback( NNSSndCaptureCallback func, void* arg )
{
    OSIntrMode enabled = OS_DisableInterrupts();;
    
    sCallbackInfo.cbFunc = func;
    sCallbackInfo.cbArg  = arg;

    (void)OS_RestoreInterrupts( enabled );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureSetPostOutputEffectCallback

  Description:  set post output effect callback function

  Arguments:    func - callback function.
                arg  - function argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureSetPostOutputEffectCallback( NNSSndCaptureCallback func, void* arg )
{
    OSIntrMode enabled = OS_DisableInterrupts();;
    
    sCallbackInfo.cbPostFunc = func;
    sCallbackInfo.cbPostArg  = arg;

    (void)OS_RestoreInterrupts( enabled );
}

/******************************************************************************
	static function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         EffectCallback

  Description:  surround process

  Arguments:    bufferL_p - L buffer
                bufferR_p - R buffer
                len       - buffer length
                format    - sample format
                arg       - user data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void EffectCallback(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    NNSSndCaptureFormat format,
    void* arg
)
{
    CallbackInfo* info = (CallbackInfo*)arg;

    if ( info->cbFunc != NULL ) {
        info->cbFunc( bufferL_p, bufferR_p, len, format, info->cbArg );
    }
    
    info->effectFunc( bufferL_p, bufferR_p, len, info );
    
    if ( info->cbPostFunc != NULL ) {
        info->cbPostFunc( bufferL_p, bufferR_p, len, format, info->cbPostArg );
    }
    
    DC_FlushRange( bufferL_p, len );
    DC_FlushRange( bufferR_p, len );
}

/*---------------------------------------------------------------------------*
  Name:         NothingProc

  Description:  ’²¿’¤â’¤·’¤Ê’¤¤’¥¨’¥Õ’¥§’¥¯’¥È

  Arguments:    bufferL_p - L buffer
                bufferR_p - R buffer
                len       - buffer length
                info      - callback info

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NothingProc(
    void* /*bufferL_p*/,
    void* /*bufferR_p*/,
    u32 /*len*/,
    CallbackInfo* /*info*/
)
{
    // do nothing
}

/*---------------------------------------------------------------------------*
  Name:         SurroundProc

  Description:  surround process

  Arguments:    bufferL_p - L buffer
                bufferR_p - R buffer
                len       - buffer length
                info      - callback info

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SurroundProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
)
{
    s16* lp = (s16*)bufferL_p;
    s16* rp = (s16*)bufferR_p;
    s16 org[ SURROUND_MIX_DELAY ];
    const unsigned long samples = len >> 1 ; // 16bit PCM
    long i;
    
#if SURROUND_MIX_DELAY >= 1
    // store mix sample data
    for ( i = 0; i < SURROUND_MIX_DELAY; i++ ) {
        s32 temp = lp[ i + samples - SURROUND_MIX_DELAY ] - rp[ i + samples - SURROUND_MIX_DELAY ];
        org[i] = Cut_S32toS16(temp);
    }
#endif
    
    // Mixing
    {
        s16* lpi = &lp[samples - 1];
        s16* rpi = &rp[samples - 1];
        for ( ; lpi >= &lp[SURROUND_MIX_DELAY] ; lpi--, rpi-- )
        {
            s32 diff = lpi[ - SURROUND_MIX_DELAY ] - rpi[ - SURROUND_MIX_DELAY ];
            s32 templ = *lpi + diff;
            s32 tempr = *rpi - diff;
            
            if ( diff >= 0 )
            {
                if ( templ < (s32)S16_MAX ) *lpi = (s16)templ;
                else *lpi = S16_MAX;
                if ( tempr > (s32)S16_MIN ) *rpi = (s16)tempr;
                else *rpi = S16_MIN;
            }
            else
            {
                if ( templ > (s32)S16_MIN ) *lpi = (s16)templ;
                else *lpi = S16_MIN;
                if ( tempr < (s32)S16_MAX ) *rpi = (s16)tempr;
                else *rpi = S16_MAX;
            }
        }
    }
    
#if SURROUND_MIX_DELAY >= 1
    for ( i = SURROUND_MIX_DELAY - 1 ; i >= 0 ; i-- )
    {
        s32 temp = lp[i] + info->buffer.surround[i];
        lp[i] = Cut_S32toS16( temp );
        temp = rp[i] - info->buffer.surround[i];
        rp[i] = Cut_S32toS16( temp );
    }
    
    // store mix sample data
    for ( i = 0; i < SURROUND_MIX_DELAY; i++ ) {
        info->buffer.surround[i] = org[i];
    }
#endif
}

#ifndef SDK_SMALL_BUILD

/*---------------------------------------------------------------------------*
  Name:         HeadphoneProc

  Description:  headphone process

  Arguments:    bufferL_p - L buffer
                bufferR_p - R buffer
                len       - buffer length
                info      - callback info

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void HeadphoneProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* info
)
{
	s16* lp = (s16*)bufferL_p;
	s16* rp = (s16*)bufferR_p;
	const unsigned long samples = len >> 1 ; // 16bit PCM
	int i;
    s32 l;
    s32 r;
	int offset;
    unsigned long rest_samples;
    
    offset = 0;
    while ( samples > HEADEPHONE_MIX_DELAY + offset )
    {
        for ( i = 0 ; i < HEADEPHONE_MIX_DELAY ; i++ )
        {
            const register int x = i + offset;
            
            l = lp[ x ] + info->buffer.headphone[i][0];
            r = rp[ x ] + info->buffer.headphone[i][1];
            
            lp[ x ] = Cut_S32toS16( l );
            rp[ x ] = Cut_S32toS16( r );
            info->buffer.headphone[i][0] = ( r + 1 ) >> HEADEPHONE_SHIFT;
            info->buffer.headphone[i][1] = ( l + 1 ) >> HEADEPHONE_SHIFT;
        }
        offset += HEADEPHONE_MIX_DELAY;
    }
    
    rest_samples = samples - offset;
    for ( i = 0 ; i < rest_samples ; i++ ) {
        const int x = i + offset;
        
        l = lp[ x ] + info->buffer.headphone[i][0]; 
        r = rp[ x ] + info->buffer.headphone[i][1];
        lp[ x ] = Cut_S32toS16( l );
        rp[ x ] = Cut_S32toS16( r );
    }
    
    for ( i = 0 ; i < HEADEPHONE_MIX_DELAY - rest_samples ; i++ ) {
        info->buffer.headphone[i][0] = info->buffer.headphone[ i + rest_samples ][0];
        info->buffer.headphone[i][1] = info->buffer.headphone[ i + rest_samples ][1];
    }
    
    for ( i = 0 ; i < rest_samples ; i++ ) {
        const long x = (long)( i + HEADEPHONE_MIX_DELAY - rest_samples );
        
        info->buffer.headphone[ x ][0] = ( rp[ i + offset ] + 1 ) >> HEADEPHONE_SHIFT;
        info->buffer.headphone[ x ][1] = ( lp[ i + offset ] + 1 ) >> HEADEPHONE_SHIFT;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonoProc

  Description:  monoral process

  Arguments:    bufferL_p - L buffer
                bufferR_p - R buffer
                len       - buffer length
                info      - callback info

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonoProc(
    void* bufferL_p,
    void* bufferR_p,
    u32 len,
    CallbackInfo* /*info*/
)
{
	s16* lp = (s16*)bufferL_p;
	s16* rp = (s16*)bufferR_p;
	const unsigned long samples = len >> 1 ; // 16bit PCM
    register s32 x;
	int i;
    
    for ( i = 0 ; i < samples ; i++ )
    {
        x = lp[ i ];
        x += rp[ i ];
        x ++;
        x >>= 1;
        lp[ i ] = (s16)x;
    }
    MI_CpuCopyFast( bufferL_p, bufferR_p, len );    
}

#endif /* SDK_SMALL_BUILD */

/*====== End of output_effect.c ======*/

