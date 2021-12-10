/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Image.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.27 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Image.h>


static void InitializeVRamLocation_ ( NNSG2dVRamLocation* pVramLocation );
static void SetVramLocation_        ( NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr );
static u32  GetVramLocation_        ( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type );
static BOOL IsImageReadyToUse_      ( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type );



//------------------------------------------------------------------------------
// You have to call this method before using a new NNSG2dVRamLocation instance.
// Otherwise, assertion failure will happen in many methods in the module. 
static NNS_G2D_INLINE void InitializeVRamLocation_( NNSG2dVRamLocation* pVramLocation )
{
    int i;
    for( i = 0; i < NNS_G2D_VRAM_TYPE_MAX; i++ )
    {
        pVramLocation->baseAddrOfVram[ i ] = NNS_G2D_VRAM_ADDR_NOT_INITIALIZED;
    }
} 

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SetVramLocation_( NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );

    if( IsImageReadyToUse_( pVramLocation, type ) )
    {
        OS_Warning("Invalid setting is detected.\n Make sure to initialize NNSG2dVRamLocation instance before using it.");
    }
        
    pVramLocation->baseAddrOfVram[ type ] = addr;
}
//------------------------------------------------------------------------------
static NNS_G2D_INLINE u32 GetVramLocation_( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return pVramLocation->baseAddrOfVram[ type ];
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsImageReadyToUse_( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return ( GetVramLocation_( pVramLocation, type ) != NNS_G2D_VRAM_ADDR_NOT_INITIALIZED ) ?
        TRUE : FALSE;
}





//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
static NNS_G2D_INLINE int          GetPow_( u16 num )
{
    switch( num )
    {
    case 1:return GX_TEXSIZE_S8;
    case 2:return GX_TEXSIZE_S16;
    case 4:return GX_TEXSIZE_S32;
    case 8:return GX_TEXSIZE_S64;
    case 16:return GX_TEXSIZE_S128;
    case 32:return GX_TEXSIZE_S256;
    default:
        OS_Warning("unexpected img size !");
        return GX_TEXSIZE_S8;
    }
}


//------------------------------------------------------------------------------
static NNS_G2D_INLINE void CopyCharDataToImageAttr_
( 
    const NNSG2dCharacterData*  pSrc, 
    NNSG2dImageAttr*            pDst 
)
{
    NNS_G2D_NULL_ASSERT( pSrc );
    NNS_G2D_NULL_ASSERT( pDst );
    //
    // TODO:初回のみ設定する
    // 初回以外ならば、同一であるかチェックをおこなう
    // 初回であることを判定できるように、何か規定値で初期化する
    //
    if( pSrc->mapingType == GX_OBJVRAMMODE_CHAR_2D )
    {
        pDst->sizeS         = (GXTexSizeS)(GetPow_( pSrc->W ));
        pDst->sizeT         = (GXTexSizeT)(GetPow_( pSrc->H ));
    }else{
        // 1D マッピングモード時には意味を持たない情報です。
        // 誤って使用されることがないように、
        // pSrc->W H には 不正なデータ(NNS_G2D_1D_MAPPING_CHAR_SIZE) をコンバータで挿入しています
        NNS_G2D_ASSERT( (pSrc->W == NNS_G2D_1D_MAPPING_CHAR_SIZE) && 
                    (pSrc->H == NNS_G2D_1D_MAPPING_CHAR_SIZE) );
        
        pDst->sizeS         = (GXTexSizeS)pSrc->W;
        pDst->sizeT         = (GXTexSizeT)pSrc->H;
    }
    
    pDst->fmt           = pSrc->pixelFmt;
    pDst->bExtendedPlt  = FALSE;                
    pDst->plttUse       = GX_TEXPLTTCOLOR0_TRNS;
    pDst->mappingType   = pSrc->mapingType;
}

//------------------------------------------------------------------------------
// キャラクタデータのVRAMへの実際のロード処理を実行します。
static NNS_G2D_INLINE void DoLoadingToVram_
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr,
    NNS_G2D_VRAM_TYPE           type
)
{
    const NNSG2dCharacterFmt charFmt = NNSi_G2dGetCharacterFmtType( pSrcData->characterFmt );
    
    //
    // VRAM転送マネージャが転送を行うので、自分でロードする必要はない
    //
    NNS_G2D_ASSERTMSG( !NNSi_G2dIsCharacterVramTransfered( pSrcData->characterFmt ),
        "You don't have to transfer a image data to VRAM. When you use the VRAM Transfer format image." );

    //
    // 指定されたVRAM種類に応じて実際のロード処理を実行します
    // ロードされるデータのフォーマットが適切でない場合はアサートに失敗します。
    //
    DC_FlushRange( pSrcData->pRawData, pSrcData->szByte );
    switch( type )
    {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        NNS_G2D_ASSERT( charFmt == NNS_G2D_CHARACTER_FMT_BMP );
        GX_BeginLoadTex();
            GX_LoadTex( (void*)pSrcData->pRawData, baseAddr, pSrcData->szByte );
        GX_EndLoadTex();    
        break;
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        NNS_G2D_ASSERT( charFmt == NNS_G2D_CHARACTER_FMT_CHAR );
        GX_LoadOBJ( (void*)pSrcData->pRawData, baseAddr, pSrcData->szByte );
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        NNS_G2D_ASSERT( charFmt == NNS_G2D_CHARACTER_FMT_CHAR );
        GXS_LoadOBJ( (void*)pSrcData->pRawData, baseAddr, pSrcData->szByte );
        break;
    default:
        NNS_G2D_ASSERT( FALSE );


    }
}

//------------------------------------------------------------------------------
// 1D マッピングモード種類が有効か判定します。
static BOOL IsValid1DMappingType_( NNS_G2D_VRAM_TYPE vramType, GXOBJVRamModeChar mappingType )
{
    switch( vramType )
    {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
    case NNS_G2D_VRAM_TYPE_2DMAIN:
       return (BOOL)( GX_OBJVRAMMODE_CHAR_1D_32K <= mappingType && 
                      mappingType <= GX_OBJVRAMMODE_CHAR_1D_256K );
       break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
       //
       // GX_OBJVRAMMODE_CHAR_1D_256K は使用できない点に注意
       //
       return (BOOL)( GX_OBJVRAMMODE_CHAR_1D_32K <= mappingType && 
                      mappingType <= GX_OBJVRAMMODE_CHAR_1D_128K );
       break;
    default:
       return FALSE;
    }
}

//------------------------------------------------------------------------------
// 入力ソースデータのサイズが適正か判定します
static BOOL IsValidDataSize_
( 
    const NNSG2dCharacterData*  pSrcData, 
    NNS_G2D_VRAM_TYPE           vramType 
)
{
    if( vramType == NNS_G2D_VRAM_TYPE_3DMAIN )
    {
       // 3D テクスチャデータに関しては特にサイズチェックは行わない
       return TRUE;
    }else{
       switch( pSrcData->mapingType )
       {
       case GX_OBJVRAMMODE_CHAR_2D:
           return (BOOL)( pSrcData->szByte <= 32 * 1024);
       case GX_OBJVRAMMODE_CHAR_1D_32K:
           return (BOOL)( pSrcData->szByte <= 32 * 1024);
       case GX_OBJVRAMMODE_CHAR_1D_64K:
           return (BOOL)( pSrcData->szByte <= 64 * 1024);
       case GX_OBJVRAMMODE_CHAR_1D_128K:
           return (BOOL)( pSrcData->szByte <= 128 * 1024);
       case GX_OBJVRAMMODE_CHAR_1D_256K:
           return (BOOL)( pSrcData->szByte <= 256 * 1024);
       default:
           return FALSE;
       }
    }
}

//------------------------------------------------------------------------------
// GX 関数を使用して、キャラクタマッピングモードに関するレジスタ設定を行います。
static NNS_G2D_INLINE void SetOBJVRamModeCharacterMapping_
( 
    NNS_G2D_VRAM_TYPE   vramType, 
    GXOBJVRamModeChar   vramMode 
)
{
    GX_OBJVRAMMODE_CHAR_ASSERT( vramMode );
    
    switch( vramType )
    {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        // Do Nothing.
        break;
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        GX_SetOBJVRamModeChar(vramMode);
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        GXS_SetOBJVRamModeChar(vramMode);
        break;   
    default:
        NNS_G2D_ASSERT( FALSE );
    }
}

//------------------------------------------------------------------------------
// 画像プロクシのパレメータ設定を行います。
static NNS_G2D_INLINE void SetupImageProxyPrams_
(
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy
)
{
    // パラメータ類のコピー
    // NNSG2dCharacterData から NNSG2dImageProxy へ
    CopyCharDataToImageAttr_( pSrcData, &pImgProxy->attr );
    // VRAM位置（アドレス）の設定
    NNS_G2dSetImageLocation( pImgProxy, type, baseAddr );
}


//------------------------------------------------------------------------------
// 実際のパレットデータのVRAMへの転送を実行します。
// type で指定したVRAM種類によって、転送に使用する関数は変化します。
//
static NNS_G2D_INLINE void DoLoadingPalette_
(
    const NNSG2dPaletteData*    pSrcData,
    u32                         srcOffset,
    u32                         addr,
    u32                         szByte,
    NNS_G2D_VRAM_TYPE           type
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    {
       const void*   pSrcRawData =  (const void*)( (const u8*)pSrcData->pRawData + srcOffset);
       
       DC_FlushRange( pSrcData->pRawData, pSrcData->szByte );
       
       switch( type )
       {
       case NNS_G2D_VRAM_TYPE_2DMAIN:
           // 2D
           if( pSrcData->bExtendedPlt )
           {
              GX_BeginLoadOBJExtPltt();
                  GX_LoadOBJExtPltt( pSrcRawData, addr, szByte );
              GX_EndLoadOBJExtPltt();                      
           }else{
              GX_LoadOBJPltt( pSrcRawData, addr, szByte );
           }
           break;
       case NNS_G2D_VRAM_TYPE_2DSUB:
           // 2D
           if( pSrcData->bExtendedPlt )
           {
              GXS_BeginLoadOBJExtPltt();
                  GXS_LoadOBJExtPltt( pSrcRawData, addr, szByte );
              GXS_EndLoadOBJExtPltt();
           }else{
              GXS_LoadOBJPltt( pSrcRawData, addr, szByte );
           }
           break;
       case NNS_G2D_VRAM_TYPE_3DMAIN:        
           // 3D
           GX_BeginLoadTexPltt();
              GX_LoadTexPltt( pSrcRawData, addr, szByte );
           GX_EndLoadTexPltt();
           break;
       default:
           NNS_G2D_ASSERT(FALSE);// TODO: msg
           break;
       }
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE u32 CalcSizePerOnePltt_( const NNSG2dPaletteData*    pSrcData )
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    
    if( pSrcData->fmt == GX_TEXFMT_PLTT16 )
    {
        return 16 * sizeof( u16 );
        
    }else{
        NNS_G2D_ASSERT( pSrcData->fmt == GX_TEXFMT_PLTT256 );// 2種類しか存在しないはず
        return 256 * sizeof( u16 );
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 GetCompressedPlttOriginalIndex_( const NNSG2dPaletteCompressInfo* pCmpInfo, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pCmpInfo );
    
    return *(((u16*)pCmpInfo->pPlttIdxTbl) + idx );
}

//------------------------------------------------------------------------------
// 圧縮形式で保存されたデータのVRAMへの実際のロード処理を実行します。
static NNS_G2D_INLINE void DoLoadingPaletteEx_
(
    const NNSG2dPaletteData*            pSrcData,
    const NNSG2dPaletteCompressInfo*    pCmpInfo,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_NULL_ASSERT( pCmpInfo );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    //
    // 分割して、部分的にロードします
    //
    {
        u16             i;
        u32             offsetAddr;
        const u32       szOnePltt   = CalcSizePerOnePltt_( pSrcData );
        const u16       numIdx      = pCmpInfo->numPalette;
    
        
        for( i = 0; i < numIdx; i++ )
        {
            offsetAddr  = GetCompressedPlttOriginalIndex_( pCmpInfo, i ) * szOnePltt;
            
            DoLoadingPalette_( pSrcData, szOnePltt * i, addr + offsetAddr, szOnePltt, type );
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitImageProxy

  Description:  NNSG2dImageProxyを初期化します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImageProxy実体
               
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitImageProxy( NNSG2dImageProxy* pImg )
{
    NNS_G2D_NULL_ASSERT( pImg );
    InitializeVRamLocation_( &pImg->vramLocation );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetImageLocation

  Description:  画像が格納されている実アドレスを設定します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImageProxy実体
                type:         [IN]  VRAMの種類
                addr:         [IN]  実アドレス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetImageLocation( NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type, u32 addr )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    SetVramLocation_( &pImg->vramLocation, type, addr );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetImageLocation

  Description:  画像が格納されている実アドレスを取得します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImageProxy実体
                type:         [IN]  VRAMの種類
                
  Returns:      実アドレス
  
 *---------------------------------------------------------------------------*/
u32 NNS_G2dGetImageLocation( const NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return GetVramLocation_( &pImg->vramLocation, type );
}
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsImageReadyToUse

  Description:  画像が利用可能か判定します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImageProxy実体
                type:         [IN]  VRAMの種類
                
  Returns:      利用可能か
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsImageReadyToUse( const NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return IsImageReadyToUse_( &pImg->vramLocation, type );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitImagePaletteProxy

  Description:  画像パレットを初期化します。
                
  Arguments:    pImg:         [OUT] NNSG2dImagePaletteProxy実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitImagePaletteProxy( NNSG2dImagePaletteProxy* pImg )
{
    NNS_G2D_NULL_ASSERT( pImg );
    InitializeVRamLocation_( &pImg->vramLocation );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetImagePaletteLocation

  Description:  画像パレットが格納されている実アドレスを取得します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImagePaletteProxy実体
                type:         [IN]  VRAMの種類
                addr:         [IN]  実アドレス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetImagePaletteLocation( NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type, u32 addr )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    SetVramLocation_( &pImg->vramLocation, type, addr );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetImagePaletteLocation

  Description:  画像パレットが格納されている実アドレスを取得します
                
                
  Arguments:    pImg:         [OUT] NNSG2dImagePaletteProxy実体
                type:         [IN]  VRAMの種類
                
  Returns:      実アドレス
  
 *---------------------------------------------------------------------------*/
u32 NNS_G2dGetImagePaletteLocation( const NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return GetVramLocation_( &pImg->vramLocation, type );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsImagePaletteReadyToUse

  Description:  画像パレットが利用可能か判定します
                
  Arguments:    pImg:         [OUT] NNSG2dImagePaletteProxy実体
                type:         [IN]  VRAMの種類
                
  Returns:      利用可能か
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsImagePaletteReadyToUse( const NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pImg );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return IsImageReadyToUse_( &pImg->vramLocation, type );
}







/*---------------------------------------------------------------------------*
  Name:         NNS_G2dLoadImage1DMapping

  Description:  画像データをハードウエアにロードし、画像プロクシにロード結果、
                画像フォーマットを設定します。
                1Dマッピングモードの画像素材専用の関数です。
                2D Graphics Engine 用VRAMが指定された場合は、
                内部で 2D Graphics Engine の キャラクタモードを1Dマッピングモードに設定します。

                
  Arguments:    pSrcData        [IN]  入力画像データソース 
                baseAddr        [IN]  読み込み先アドレス 
                type            [IN]  VRAM利用の種類(3D 2D-Main 2D-Sub ) 
                pImgProxy       [OUT] 読み込み結果を格納する画像プロクシ 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dLoadImage1DMapping
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_ASSERTMSG( IsValid1DMappingType_( type, pSrcData->mapingType ), 
                       "Invalid mapping-type.");
    NNS_G2D_ASSERTMSG( IsValidDataSize_( pSrcData, type ), 
                       "Invalid data size for the mapping-type.");
    
                       
                       
    
    SetOBJVRamModeCharacterMapping_( type, pSrcData->mapingType );
    
    DoLoadingToVram_( pSrcData, baseAddr, type );
    
    SetupImageProxyPrams_( pSrcData, baseAddr, type, pImgProxy );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dLoadImage2DMapping

  Description:  画像データをハードウエアにロードし、画像プロクシにロード結果、
                画像フォーマットを設定します。
                
                2Dマッピングモードの画像素材専用の関数です。
                2D Graphics Engine 用VRAMが指定された場合は、
                内部で 2D Graphics Engine の キャラクタモードを2Dマッピングモードに設定します。

                
  Arguments:    pSrcData        [IN]  入力画像データソース 
                baseAddr        [IN]  読み込み先アドレス 
                type            [IN]  VRAM利用の種類(3D 2D-Main 2D-Sub ) 
                pImgProxy       [OUT] 読み込み結果を格納する画像プロクシ 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dLoadImage2DMapping
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_NULL_ASSERT( pImgProxy );    
    NNS_G2D_ASSERT( pSrcData->mapingType == GX_OBJVRAMMODE_CHAR_2D );
    NNS_G2D_ASSERTMSG( IsValidDataSize_( pSrcData, type ), 
                       "Invalid data size for the mapping-type.");
    

    SetOBJVRamModeCharacterMapping_( type, pSrcData->mapingType );

    
    DoLoadingToVram_( pSrcData, baseAddr, type );
    
    SetupImageProxyPrams_( pSrcData, baseAddr, type, pImgProxy );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dLoadImageVramTransfer

  Description:  画像プロクシにロード結果、画像フォーマットを設定します。
                VRAM転送用の画像素材専用の関数です。
                ほかの NNS_G2dLoadImageXXX() 関数とはことなり実際には画像データをハードウエアにロードしません。
                実際のロードはゲームループ中で、VRAM転送モジュールが行うこととなります。
                2D Graphics Engine 用VRAMが指定された場合は、
                内部で 2D Graphics Engine の キャラクタモードを1Dマッピングモードに設定します。
                （NCGR,NCBRファイルではVRAM転送用の画像素材は必ず１Dマッピングモードで格納されています。） 
                
  Arguments:    pSrcData        [IN]  入力画像データソース 
                baseAddr        [IN]  読み込み先アドレス 
                type            [IN]  VRAM利用の種類(3D 2D-Main 2D-Sub ) 
                pImgProxy       [OUT] 読み込み結果を格納する画像プロクシ 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dLoadImageVramTransfer
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
)
{   
    NNS_G2D_MINMAX_ASSERT( pSrcData->mapingType, 
                       GX_OBJVRAMMODE_CHAR_1D_32K, 
                       GX_OBJVRAMMODE_CHAR_1D_256K );
                       
    NNS_G2D_ASSERTMSG( NNSi_G2dIsCharacterVramTransfered( pSrcData->characterFmt ),
        "NNS_G2dSetVramTransferImageProxy() must be used only for the vram transfer format image data." );
    
    DC_FlushRange( pSrcData->pRawData, pSrcData->szByte );
    
    SetOBJVRamModeCharacterMapping_( type, pSrcData->mapingType );
    
    CopyCharDataToImageAttr_( pSrcData, &pImgProxy->attr );
    
    NNS_G2dSetImageLocation( pImgProxy, type, baseAddr );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dLoadPalette

  Description:  画像パレットデータをハードウエアにロードし、
                画像パレットプロクシにロード結果、画像フォーマットを設定します。

                
                
  Arguments:    pSrcData        [IN]  入力画像データソース 
                addr            [IN]  読み込み先アドレス 
                type            [IN]  VRAM利用の種類(3D 2D-Main 2D-Sub ) 
                pPltProxt       [OUT] 読み込み結果を格納する画像パレットプロクシ 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dLoadPalette
(
    const NNSG2dPaletteData*    pSrcData,
    u32                         addr,
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImagePaletteProxy*    pPltProxy
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_VRAM_TYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    DoLoadingPalette_( pSrcData, 0, addr, pSrcData->szByte, type );                                      
    
    pPltProxy->fmt          = pSrcData->fmt;          
    pPltProxy->bExtendedPlt = pSrcData->bExtendedPlt;
    
    NNS_G2dSetImagePaletteLocation( pPltProxy, type, addr );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dLoadPaletteEx

  Description:  画像パレットデータをハードウエアにロードし、
                画像パレットプロクシにロード結果、画像フォーマットを設定します。

                
                
  Arguments:    pSrcData        [IN]  入力画像データソース 
                pCmpInfo        [IN]  圧縮情報
                addr            [IN]  読み込み先アドレス 
                type            [IN]  VRAM利用の種類(3D 2D-Main 2D-Sub ) 
                pPltProxy       [OUT] 読み込み結果を格納する画像パレットプロクシ 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dLoadPaletteEx
(
    const NNSG2dPaletteData*            pSrcData,
    const NNSG2dPaletteCompressInfo*    pCmpInfo,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type, 
    NNSG2dImagePaletteProxy*            pPltProxy
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_NULL_ASSERT( pCmpInfo );
    NNS_G2D_VRAM_TYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    //
    // 分割して、部分的にロードします
    //
    DoLoadingPaletteEx_( pSrcData, pCmpInfo, addr, type );
    
    // 
    // そのほかの設定については、通常のロードと変わりありません
    // 画像プロクシの設定が同一である点には特にご注意ください。
    //
    pPltProxy->fmt          = pSrcData->fmt;          
    pPltProxy->bExtendedPlt = pSrcData->bExtendedPlt;
    
    NNS_G2dSetImagePaletteLocation( pPltProxy, type, addr );
}


//------------------------------------------------------------------------------
// プレイベート関数 を 外部公開するのためのラッパー群
// ライブラリ internal 公開
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void NNSi_G2dInitializeVRamLocation( NNSG2dVRamLocation* pVramLocation )
{
    InitializeVRamLocation_( pVramLocation );
} 

//------------------------------------------------------------------------------
void NNSi_G2dSetVramLocation( NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );

    SetVramLocation_( pVramLocation, type, addr );
}
//------------------------------------------------------------------------------
u32 NNSi_G2dGetVramLocation( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return GetVramLocation_( pVramLocation, type );
}

//------------------------------------------------------------------------------
BOOL NNSi_G2dIsVramLocationReadyToUse( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_NULL_ASSERT( pVramLocation );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    return IsImageReadyToUse_( pVramLocation, type );
}

//------------------------------------------------------------------------------
// 画像データの実際のVRAMへのロード処理を行います。
void NNSi_G2dDoImageLoadingToVram
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr,
    NNS_G2D_VRAM_TYPE           type
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    DoLoadingToVram_( pSrcData, baseAddr, type );
}


//------------------------------------------------------------------------------
// 画像プロクシのパレメータ設定を行います。
void NNSi_G2dSetupImageProxyPrams
(
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_VRAM_TYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    
    SetupImageProxyPrams_( pSrcData, baseAddr, type, pImgProxy );
}

//------------------------------------------------------------------------------
// パレットデータの実際のVRAMへのロード処理を行います。
void NNSi_G2dDoLoadingPaletteToVram
(
    const NNSG2dPaletteData*    pSrcData,
    u32                         addr,
    NNS_G2D_VRAM_TYPE           type
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    
    DoLoadingPalette_( pSrcData, 0, addr, pSrcData->szByte, type );
}

//------------------------------------------------------------------------------
// パレットデータの実際のVRAMへのロード処理を行います。
// 圧縮形式で保存されたパレットデータに対して使用される関数です。
void NNSi_G2dDoLoadingPaletteToVramEx
(
    const NNSG2dPaletteData*            pSrcData,
    const NNSG2dPaletteCompressInfo*    pCmpInfo,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_NULL_ASSERT( pCmpInfo );
    NNS_G2D_VRAM_TYPE_VALID( type );
    
    
    DoLoadingPaletteEx_( pSrcData, pCmpInfo, addr, type );
}

//------------------------------------------------------------------------------
// パレットプロクシのパレメータ設定を行います。
void NNSi_G2dSetupPaletteProxyPrams
(
    const NNSG2dPaletteData*            pSrcData,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type, 
    NNSG2dImagePaletteProxy*            pPltProxy
)
{
    NNS_G2D_NULL_ASSERT( pSrcData );
    NNS_G2D_VRAM_TYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    pPltProxy->fmt          = pSrcData->fmt;          
    pPltProxy->bExtendedPlt = pSrcData->bExtendedPlt;
    
    NNS_G2dSetImagePaletteLocation( pPltProxy, type, addr );
}

