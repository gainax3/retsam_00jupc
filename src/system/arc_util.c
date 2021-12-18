//============================================================================================
/**
 * @file	arc_util.c
 * @bfief	アーカイブデータを便利に使うためのユーティリティ関数群
 * @author	taya
 * @date	05.08.30
 * @date	07.01.23	HANDLEバージョンを作成	tomoya takahashi
 */
//============================================================================================
#include "common.h"

#include "system/arc_tool.h"
#include "system/arc_util.h"


//==============================================================
// Prototype
//==============================================================
static u32 ArcUtil_BgCharSetCommon( void* p_data, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize );
static void ArcUtil_ScrnSetCommon( void* p_data, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize );
static u32 ArcUtil_ObjCharSetCommon( void* p_data, OBJTYPE objType, u32 offs, u32 transSize );
static void ArcUtil_PalSetEzCommon( void* p_data, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize );
static void ArcUtil_PalSysLoadCommon( void* p_data, NNS_G2D_VRAM_TYPE type, u32 offs, NNSG2dImagePaletteProxy* proxy );
static u32 ArcUtil_CharSysLoadCommon( void* p_data, CHAR_MAPPING_TYPE mapType, u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy );
static void ArcUtil_CharSysLoadSyncroMappingModeCommon( void* p_data, CHAR_MAPPING_TYPE mapType, u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy );
static const void* ArcUtil_TransTypeCharSysLoadCommon( void* p_data, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData );
static void* ArcUtil_CharDataGetCommon( void* p_data, NNSG2dCharacterData** charData );
static void* ArcUtil_ScrnDataGetCommon( void* p_data, NNSG2dScreenData** scrnData );
static void* ArcUtil_PalDataGetCommon( void* p_data, NNSG2dPaletteData** palData );
static void* ArcUtil_CellBankDataGetCommon( void* p_data, NNSG2dCellDataBank** cellBank );
static void* ArcUtil_AnimBankDataGetCommon( void* p_data, NNSG2dAnimBankData** anmBank );

//------------------------------------------------------------------
/**
 * BGｷｬﾗﾃﾞｰﾀの VRAM 転送
 *
 * @param   fileIdx			ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   bgl				BGLﾃﾞｰﾀ
 * @param   frm				転送先ﾌﾚｰﾑﾅﾝﾊﾞ
 * @param   offs			転送ｵﾌｾｯﾄ（ｷｬﾗ単位）
 * @param	transSize		転送するｻｲｽﾞ（ﾊﾞｲﾄ単位 ==0で全転送）
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   heapID			ﾃﾞｰﾀ読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 * @return  転送したデータサイズ（バイト）
 */
//------------------------------------------------------------------
u32 ArcUtil_BgCharSet(u32 fileIdx, u32 dataIdx, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_BgCharSetCommon( arcData, bgl, frm, offs, transSize );
}
//--------------------------------------------------------------------------------------------
/**
 * ｽｸﾘｰﾝﾃﾞｰﾀの VRAM 転送
 * ※ BGL側に ｽｸﾘｰﾝﾊﾞｯﾌｧ が用意されていれば、ｽｸﾘｰﾝﾊﾞｯﾌｧ への転送も行う
 *
 * @param   fileIdx			ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   bgl				BGLﾃﾞｰﾀ
 * @param   frm				転送先ﾌﾚｰﾑﾅﾝﾊﾞ
 * @param   offs			転送ｵﾌｾｯﾄ（ｷｬﾗ単位）
 * @param	transSize		転送するｻｲｽﾞ（ﾊﾞｲﾄ単位 ==0で全転送）
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   heapID			ﾃﾞｰﾀ読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//--------------------------------------------------------------------------------------------
void ArcUtil_ScrnSet(u32 fileIdx, u32 dataIdx, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	ArcUtil_ScrnSetCommon( arcData, bgl, frm, offs, transSize );
}
//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送
 *
 * @param   fileIdx		ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   palType		ﾊﾟﾚｯﾄ転送先ﾀｲﾌﾟ
 * @param   offs		ﾊﾟﾚｯﾄ転送先ｵﾌｾｯﾄ
 * @param   transSize	ﾊﾟﾚｯﾄ転送ｻｲｽﾞ（0 で全転送）
 * @param   heapID		ﾃﾞｰﾀ読み込みﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//------------------------------------------------------------------
void ArcUtil_PalSet( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 offs, u32 transSize, u32 heapID )
{
	ArcUtil_PalSetEx( fileIdx, dataIdx, palType, 0, offs, transSize, heapID );
}

//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送（転送元の読み込み開始ｵﾌｾｯﾄ指定版）
 *
 * @param   fileIdx		ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   palType		ﾊﾟﾚｯﾄ転送先ﾀｲﾌﾟ
 * @param   srcOfs		ﾊﾟﾚｯﾄ転送元読み込み開始ｵﾌｾｯﾄ
 * @param   dstOfs		ﾊﾟﾚｯﾄ転送先ｵﾌｾｯﾄ
 * @param   transSize	ﾊﾟﾚｯﾄ転送ｻｲｽﾞ（0 で全転送）
 * @param   heapID		ﾃﾞｰﾀ読み込みﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//------------------------------------------------------------------
void ArcUtil_PalSetEx( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize, u32 heapID )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, FALSE, heapID, ALLOC_BOTTOM );
	ArcUtil_PalSetEzCommon( arcData, palType, srcOfs, dstOfs, transSize );
}

//------------------------------------------------------------------
/**
 * OBJ ｷｬﾗﾃﾞｰﾀ の VRAM 転送
 *
 * @param   fileIdx				ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx				ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   objType				OBJﾀｲﾌﾟ(OBJTYPE_MAIN or OBJTYPE_SUB）
 * @param   offs				ｵﾌｾｯﾄ（ﾊﾞｲﾄ単位）
 * @param   transSize			転送ｻｲｽﾞ（ﾊﾞｲﾄ単位 : 0 で全転送）
 * @param   compressedFlag		圧縮されたﾃﾞｰﾀか？
 * @param   heapID				読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 * @return  転送したデータサイズ（バイト）
 */
//------------------------------------------------------------------
u32 ArcUtil_ObjCharSet( u32 fileIdx, u32 dataIdx, OBJTYPE objType, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	return ArcUtil_ObjCharSetCommon( arcData, objType, offs, transSize );
}

//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送＆ NITRO System ﾊﾟﾚｯﾄﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 * @param   fileIdx		[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx		[in] ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   type		[in] 転送先ﾀｲﾌﾟ
 * @param   offs		[in] 転送ｵﾌｾｯﾄ
 * @param   heapID		[in] ﾋｰﾌﾟID
 * @param   proxy		[out]作成するﾌﾟﾛｷｼｱﾄﾞﾚｽ
 *
 *	[ type ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void ArcUtil_PalSysLoad( u32 fileIdx, u32 dataIdx, NNS_G2D_VRAM_TYPE type, u32 offs, u32 heapID, NNSG2dImagePaletteProxy* proxy )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, FALSE, heapID, ALLOC_BOTTOM );
	ArcUtil_PalSysLoadCommon( arcData, type, offs, proxy );
}
//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 *
 * @param   fileIdx			[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx			[in] ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	[in] 圧縮されているか
 * @param   mapType			[in] ﾏｯﾋﾟﾝｸﾞﾀｲﾌﾟ
 * @param   transSize		[in] 転送サイズ。０なら全転送。
 * @param   vramType		[in] 転送先ﾀｲﾌﾟ
 * @param   offs			[in] 転送ｵﾌｾｯﾄ
 * @param   heapID			[in] ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			[out] 作成するﾌﾟﾛｷｼｱﾄﾞﾚｽ
 *
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
u32 ArcUtil_CharSysLoad( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	return ArcUtil_CharSysLoadCommon( arcData, mapType, transSize, vramType, offs, proxy );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 * ｷｬﾗﾃﾞｰﾀのﾏｯﾋﾟﾝｸﾞﾓｰﾄﾞ値を、現在のﾚｼﾞｽﾀ設定に合わせて書き換えます
 *
 * @param   fileIdx			[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx			[in] ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	[in] 圧縮されているか
 * @param   mapType			[in] ﾏｯﾋﾟﾝｸﾞﾀｲﾌﾟ
 * @param   transSize		[in] 転送サイズ。０なら全転送。
 * @param   vramType		[in] 転送先ﾀｲﾌﾟ
 * @param   offs			[in] 転送ｵﾌｾｯﾄ
 * @param   heapID			[in] ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			[out] 作成するﾌﾟﾛｷｼｱﾄﾞﾚｽ
 *
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void ArcUtil_CharSysLoadSyncroMappingMode( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	ArcUtil_CharSysLoadSyncroMappingModeCommon( arcData, mapType, transSize, vramType, offs, proxy );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成。VRAM転送型の画像素材用。
 *
 * ※ この関数を使っても、VRAMに画像は転送されません
 *    この関数でﾛｰﾄﾞしたﾃﾞｰﾀは解放されません。戻り値のconst void*を管理して、
 *    不要になったら解放処理を行ってください。
 *
 * @param   fileIdx			[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx			[in] ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	[in] 圧縮されているか
 * @param   vramType		[in] 転送先ﾀｲﾌﾟ
 * @param   offs			[in] 転送ｵﾌｾｯﾄ
 * @param   heapID			[in] ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			[out] 作成するﾌﾟﾛｷｼのｱﾄﾞﾚｽ
 * @param   charData		[out] ｷｬﾗﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 *
 * @retval  const void*		ﾛｰﾄﾞしたﾃﾞｰﾀのｱﾄﾞﾚｽ
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
const void* ArcUtil_TransTypeCharSysLoad( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, 
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_TransTypeCharSysLoadCommon( arcData, vramType, offs, proxy, charData );
}



//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   fileIdx				[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   dataIdx				[in] ﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag		[in] 圧縮されているか
 * @param   charData			[out] ｷｬﾗﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID				[in] ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//------------------------------------------------------------------
void* ArcUtil_CharDataGet( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dCharacterData** charData, u32 heapID )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_CharDataGetCommon( arcData, charData );
}
//--------------------------------------------------------------------------------------------
/**
 * ｽｸﾘｰﾝﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   fileIdx			[in] ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   arcIndex		[in] ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	[in] 圧縮されているﾃﾞｰﾀか？
 * @param   scrnData		[out] ｽｸﾘｰﾝﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			[in] ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_ScrnDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dScreenData** scrnData, u32 heapID)
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_ScrnDataGetCommon( arcData, scrnData );
}
//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀをﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   fileIdx			ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   palData			ﾊﾟﾚｯﾄﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//------------------------------------------------------------------
void* ArcUtil_PalDataGet( u32 fileIdx, u32 dataIdx, NNSG2dPaletteData** palData, u32 heapID )
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, FALSE, heapID, ALLOC_TOP );
	return ArcUtil_PalDataGetCommon( arcData, palData );
}
//--------------------------------------------------------------------------------------------
/**
 * ｾﾙﾊﾞﾝｸﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   fileIdx			ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   cellBank		ｾﾙﾊﾞﾝｸﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 * @param   pSize			ｾﾙﾊﾞﾝｸのﾊﾞｲﾄｻｲｽﾞを受け取る変数ﾎﾟｲﾝﾀ（要らなければNULLで）
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_CellBankDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dCellDataBank** cellBank, u32 heapID )
{
	void* arcData;
	arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_CellBankDataGetCommon( arcData, cellBank );
}
//--------------------------------------------------------------------------------------------
/**
 * ｱﾆﾒﾊﾞﾝｸﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   fileIdx			ｱｰｶｲﾌﾞﾌｧｲﾙｲﾝﾃﾞｯｸｽ
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   anmBank			ｱﾆﾒﾊﾞﾝｸﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_AnimBankDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dAnimBankData** anmBank, u32 heapID)
{
	void* arcData = ArcUtil_Load( fileIdx, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_AnimBankDataGetCommon( arcData, anmBank );
}











//------------------------------------------------------------------
/**
 * LZ圧縮後アーカイブされているデータを読み出し、解凍して返す
 *
 * @param   fileIdx		アーカイブファイルインデックス
 * @param   dataIdx		アーカイブデータインデックス
 * @param   heapID		読み出し・解凍に使うヒープＩＤ
 *
 * @retval  void*		解凍後のデータ保存先アドレス
 */
//------------------------------------------------------------------
void* ArcUtil_UnCompress(u32 fileIdx, u32 dataIdx, u32 heapID)
{
	return ArcUtil_Load(fileIdx, dataIdx, TRUE, heapID, ALLOC_TOP);
}

//------------------------------------------------------------------
/**
 * アーカイブデータの読み出し（メモリ確保する）
 *
 * @param   fileIdx			アーカイブファイルインデックス
 * @param   dataIdx			アーカイブデータインデックス
 * @param   compressedFlag	圧縮されているか？
 * @param   heapID			メモリ確保に使うヒープＩＤ
 * @param   allocType		ヒープのどの位置からメモリ確保するか
 *
 * @retval  void*			読み出し領域ポインタ
 */
//------------------------------------------------------------------
void* ArcUtil_Load(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, u32 heapID, ALLOC_TYPE allocType)
{
	void* arcData;

	if( compressedFlag || allocType == ALLOC_BOTTOM )
	{
		arcData = sys_AllocMemoryLo( heapID, ArchiveDataSizeGet(fileIdx, dataIdx) );
	}
	else
	{
		arcData = sys_AllocMemory( heapID, ArchiveDataSizeGet(fileIdx, dataIdx) );
	}

	if( arcData != NULL )
	{
		ArchiveDataLoad(arcData, fileIdx, dataIdx);
		if( compressedFlag )
		{
			void* data;

			if( allocType == ALLOC_TOP )
			{
				data = sys_AllocMemory( heapID, MI_GetUncompressedSize( arcData ) );
			}
			else
			{
				data = sys_AllocMemoryLo( heapID, MI_GetUncompressedSize( arcData ) );
			}

			if( data )
			{
				MI_UncompressLZ8( arcData, data );
				sys_FreeMemoryEz( arcData );
			}
			arcData = data;
		}
	}
	return arcData;
}
//------------------------------------------------------------------
/**
 * アーカイブデータの読み出し（読み出したサイズも取得できる）
 *
 * @param   fileIdx			アーカイブファイルインデックス
 * @param   dataIdx			アーカイブデータインデックス
 * @param   compressedFlag	圧縮されているか？
 * @param   heapID			メモリ確保に使うヒープＩＤ
 * @param   allocType		ヒープのどの位置からメモリ確保するか
 * @param   pSize			実データのバイトサイズ（圧縮されている場合は展開後）
 *
 * @retval  void*			読み出し領域ポインタ
 */
//------------------------------------------------------------------
void* ArcUtil_LoadEx(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, u32 heapID, ALLOC_TYPE allocType, u32* pSize)
{
	void* arcData;

	*pSize = ArchiveDataSizeGet(fileIdx, dataIdx);

	if( compressedFlag || allocType == ALLOC_BOTTOM )
	{
		arcData = sys_AllocMemoryLo( heapID, *pSize );
	}
	else
	{
		arcData = sys_AllocMemory( heapID, *pSize );
	}

	if( arcData != NULL )
	{
		ArchiveDataLoad(arcData, fileIdx, dataIdx);
		if( compressedFlag )
		{
			void* data;

			*pSize = MI_GetUncompressedSize( arcData );

			if( allocType == ALLOC_TOP )
			{
				data = sys_AllocMemory( heapID, *pSize );
			}
			else
			{
				data = sys_AllocMemoryLo( heapID, *pSize );
			}

			if( data )
			{
				MI_UncompressLZ8( arcData, data );
				sys_FreeMemoryEz( arcData );
			}
			arcData = data;
		}
	}
	return arcData;
}


//-----------------------------------------------------------------------------
/**
 *		ARCHANDLEバージョン
 */
//-----------------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * BGｷｬﾗﾃﾞｰﾀの VRAM 転送
 *
 * @param   handle			ハンドル
 * @param   arcIndex		アーカイブデータインデックス
 * @param   bgl				BGLﾃﾞｰﾀ
 * @param   frm				転送先ﾌﾚｰﾑﾅﾝﾊﾞ
 * @param   offs			転送ｵﾌｾｯﾄ（ｷｬﾗ単位）
 * @param	transSize		転送するｻｲｽﾞ（ﾊﾞｲﾄ単位 ==0で全転送）
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   heapID			ﾃﾞｰﾀ読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 * @return  転送したデータサイズ（バイト）
 */
//------------------------------------------------------------------
u32 ArcUtil_HDL_BgCharSet(ARCHANDLE* handle, u32 dataIdx, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_BgCharSetCommon( arcData, bgl, frm, offs, transSize );
}

//--------------------------------------------------------------------------------------------
/**
 * ｽｸﾘｰﾝﾃﾞｰﾀの VRAM 転送
 * ※ BGL側に ｽｸﾘｰﾝﾊﾞｯﾌｧ が用意されていれば、ｽｸﾘｰﾝﾊﾞｯﾌｧ への転送も行う
 *
 * @param   handle			ハンドル
 * @param   arcIndex		アーカイブデータインデックス
 * @param   bgl				BGLﾃﾞｰﾀ
 * @param   frm				転送先ﾌﾚｰﾑﾅﾝﾊﾞ
 * @param   offs			転送ｵﾌｾｯﾄ（ｷｬﾗ単位）
 * @param	transSize		転送するｻｲｽﾞ（ﾊﾞｲﾄ単位 ==0で全転送）
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   heapID			ﾃﾞｰﾀ読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//--------------------------------------------------------------------------------------------
void ArcUtil_HDL_ScrnSet(ARCHANDLE* handle, u32 dataIdx, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	ArcUtil_ScrnSetCommon( arcData, bgl, frm, offs, transSize );
}

//------------------------------------------------------------------
/**
 * OBJ ｷｬﾗﾃﾞｰﾀ の VRAM 転送
 *
 * @param   handle				ハンドル
 * @param   dataIdx				アーカイブデータインデックス
 * @param   objType				OBJﾀｲﾌﾟ
 * @param   offs				ｵﾌｾｯﾄ（ﾊﾞｲﾄ単位）
 * @param   transSize			転送ｻｲｽﾞ（ﾊﾞｲﾄ単位 : 0 で全転送）
 * @param   compressedFlag		圧縮されたﾃﾞｰﾀか？
 * @param   heapID				読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 * @return  転送したデータサイズ（バイト）
 */
//------------------------------------------------------------------
u32 ArcUtil_HDL_ObjCharSet( ARCHANDLE* handle, u32 dataIdx, OBJTYPE objType, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	return ArcUtil_ObjCharSetCommon( arcData, objType, offs, transSize );
}

//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送
 *
 * @param   handle		ハンドル
 * @param   dataIdx		アーカイブデータインデックス
 * @param   palType		ﾊﾟﾚｯﾄ転送先ﾀｲﾌﾟ
 * @param   offs		ﾊﾟﾚｯﾄ転送先ｵﾌｾｯﾄ
 * @param   transSize	ﾊﾟﾚｯﾄ転送ｻｲｽﾞ（0 で全転送）
 * @param   heapID		ﾃﾞｰﾀ読み込みﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//------------------------------------------------------------------
void ArcUtil_HDL_PalSet( ARCHANDLE* handle, u32 dataIdx, PALTYPE palType, u32 offs, u32 transSize, u32 heapID )
{
	ArcUtil_HDL_PalSetEx( handle, dataIdx, palType, 0, offs, transSize, heapID );
}


//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送（転送元の読み込み開始ｵﾌｾｯﾄ指定版）
 *
 * @param   handle		ハンドル
 * @param   dataIdx		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   palType		ﾊﾟﾚｯﾄ転送先ﾀｲﾌﾟ
 * @param   srcOfs		ﾊﾟﾚｯﾄ転送元読み込み開始ｵﾌｾｯﾄ
 * @param   dstOfs		ﾊﾟﾚｯﾄ転送先ｵﾌｾｯﾄ
 * @param   transSize	ﾊﾟﾚｯﾄ転送ｻｲｽﾞ（0 で全転送）
 * @param   heapID		ﾃﾞｰﾀ読み込みﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 */
//------------------------------------------------------------------
void ArcUtil_HDL_PalSetEx( ARCHANDLE* handle, u32 dataIdx, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize, u32 heapID )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, FALSE, heapID, ALLOC_BOTTOM );
	ArcUtil_PalSetEzCommon( arcData, palType, srcOfs, dstOfs, transSize );
}


//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀ の VRAM 転送＆ NITRO System ﾊﾟﾚｯﾄﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 * @param   handle		ハンドル
 * @param   dataIdx		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   type		転送先ﾀｲﾌﾟ
 * @param   offs		転送ｵﾌｾｯﾄ
 * @param   heapID		ﾋｰﾌﾟID
 * @param   proxy		作成するﾌﾟﾛｷｼのｱﾄﾞﾚｽ
 *
 *	[ type ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void ArcUtil_HDL_PalSysLoad( ARCHANDLE* handle, u32 dataIdx, NNS_G2D_VRAM_TYPE type, u32 offs, u32 heapID, NNSG2dImagePaletteProxy* proxy )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, FALSE, heapID, ALLOC_BOTTOM );
	ArcUtil_PalSysLoadCommon( arcData, type, offs, proxy );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 *
 * @param   handle			ハンドル
 * @param   dataIdx			ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているか
 * @param   mapType			ﾏｯﾋﾟﾝｸﾞﾀｲﾌﾟ
 * @param   transSize		転送ｻｲｽﾞ（0なら全転送）
 * @param   vramType		転送先ﾀｲﾌﾟ
 * @param   offs			転送ｵﾌｾｯﾄ
 * @param   heapID			ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			作成するﾌﾟﾛｷｼのｱﾄﾞﾚｽ
 *
 * @retval  転送されたﾃﾞｰﾀｻｲｽﾞ（ﾊﾞｲﾄ単位）
 *
 *	[ mapType ]
 *		MAP_TYPE_1D = 0,
 *		MAP_TYPE_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
u32 ArcUtil_HDL_CharSysLoad( ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType,
	u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	return ArcUtil_CharSysLoadCommon( arcData, mapType, transSize, vramType, offs, proxy );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成
 *（3D, OBJ 用にのみ対応。BG には使いません）
 *
 * ｷｬﾗﾃﾞｰﾀのﾏｯﾋﾟﾝｸﾞﾓｰﾄﾞ値を、現在のﾚｼﾞｽﾀ設定に合わせて書き換えます
 *
 * @param   handle			ハンドル
 * @param   dataIdx			ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているか
 * @param   mapType			ﾏｯﾋﾟﾝｸﾞﾀｲﾌﾟ
 * @param   transSize		転送サイズ。０なら全転送。
 * @param   vramType		転送先ﾀｲﾌﾟ
 * @param   offs			転送ｵﾌｾｯﾄ
 * @param   heapID			ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			作成するﾌﾟﾛｷｼｱﾄﾞﾚｽ
 *
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void ArcUtil_HDL_CharSysLoadSyncroMappingMode( ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_BOTTOM );
	ArcUtil_CharSysLoadSyncroMappingModeCommon( arcData, mapType, transSize, vramType, offs, proxy );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成。VRAM転送型の画像素材用。
 *
 * ※ この関数を使っても、VRAMに画像は転送されません
 *    この関数でﾛｰﾄﾞしたﾃﾞｰﾀは解放されません。戻り値のconst void*を管理して、
 *    不要になったら解放処理を行ってください。
 *
 * @param   handle			ハンドル
 * @param   dataIdx			ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているか
 * @param   vramType		転送先ﾀｲﾌﾟ
 * @param   offs			転送ｵﾌｾｯﾄ
 * @param   heapID			ﾃﾝﾎﾟﾗﾘに使うﾋｰﾌﾟID
 * @param   proxy			作成するﾌﾟﾛｷｼのｱﾄﾞﾚｽ
 * @param   charData		ｷｬﾗﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 *
 * @retval  const void*		ﾛｰﾄﾞしたﾃﾞｰﾀのｱﾄﾞﾚｽ
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
const void* ArcUtil_HDL_TransTypeCharSysLoad( ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, 
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_TransTypeCharSysLoadCommon( arcData, vramType, offs, proxy, charData );
}

//------------------------------------------------------------------
/**
 * ｷｬﾗﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   handle				ハンドル
 * @param   dataIdx				ﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag		圧縮されているか
 * @param   charData			ｷｬﾗﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID				ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//------------------------------------------------------------------
void* ArcUtil_HDL_CharDataGet( ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, NNSG2dCharacterData** charData, u32 heapID )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_CharDataGetCommon( arcData, charData );
}

//--------------------------------------------------------------------------------------------
/**
 * ｽｸﾘｰﾝﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   handle			ハンドル
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   scrnData		ｽｸﾘｰﾝﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾃﾞｰﾀ読み込み・解凍ﾃﾝﾎﾟﾗﾘとして使うﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_HDL_ScrnDataGet(ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, NNSG2dScreenData** scrnData, u32 heapID)
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_ScrnDataGetCommon( arcData, scrnData );
}

//------------------------------------------------------------------
/**
 * ﾊﾟﾚｯﾄﾃﾞｰﾀをﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   handle			ハンドル
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   palData			ﾊﾟﾚｯﾄﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//------------------------------------------------------------------
extern void* ArcUtil_HDL_PalDataGet( ARCHANDLE* handle, u32 dataIdx, NNSG2dPaletteData** palData, u32 heapID )
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, FALSE, heapID, ALLOC_TOP );
	return ArcUtil_PalDataGetCommon( arcData, palData );
}

//--------------------------------------------------------------------------------------------
/**
 * ｾﾙﾊﾞﾝｸﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   handle			ハンドル
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   cellBank		ｾﾙﾊﾞﾝｸﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_HDL_CellBankDataGet(ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, NNSG2dCellDataBank** cellBank, u32 heapID )
{
	void* arcData;
	arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_CellBankDataGetCommon( arcData, cellBank );
}

//--------------------------------------------------------------------------------------------
/**
 * ｱﾆﾒﾊﾞﾝｸﾃﾞｰﾀを ﾛｰﾄﾞして Unpack するだけです。解放は各自で。
 *
 * @param   handle			ハンドル
 * @param   arcIndex		ｱｰｶｲﾌﾞﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 * @param   compressedFlag	圧縮されているﾃﾞｰﾀか？
 * @param   anmBank			ｱﾆﾒﾊﾞﾝｸﾃﾞｰﾀｱﾄﾞﾚｽを保持するﾎﾟｲﾝﾀのｱﾄﾞﾚｽ
 * @param   heapID			ﾋｰﾌﾟID
 *
 * @retval  void*		ﾛｰﾄﾞしたﾃﾞｰﾀの先頭ﾎﾟｲﾝﾀ
 */
//--------------------------------------------------------------------------------------------
void* ArcUtil_HDL_AnimBankDataGet(ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, NNSG2dAnimBankData** anmBank, u32 heapID)
{
	void* arcData = ArcUtil_HDL_Load( handle, dataIdx, compressedFlag, heapID, ALLOC_TOP );
	return ArcUtil_AnimBankDataGetCommon( arcData, anmBank );
}


//------------------------------------------------------------------
/**
 * LZ圧縮後アーカイブされているデータを読み出し、解凍して返す
 *
 * @param   handle		ハンドル
 * @param   dataIdx		アーカイブデータインデックス
 * @param   heapID		読み出し・解凍に使うヒープＩＤ
 *
 * @retval  void*		解凍後のデータ保存先アドレス
 */
//------------------------------------------------------------------
void* ArcUtil_HDL_UnCompress(ARCHANDLE* handle, u32 dataIdx, u32 heapID)
{
	return ArcUtil_HDL_Load(handle, dataIdx, TRUE, heapID, ALLOC_TOP);
}


//------------------------------------------------------------------
/**
 * アーカイブデータの読み出し
 *
 * @param   handle			ハンドル
 * @param   dataIdx			アーカイブデータインデックス
 * @param   compressedFlag	圧縮されているか？
 * @param   heapID			メモリ確保に使うヒープＩＤ
 * @param   allocType		ヒープのどの位置からメモリ確保するか
 *
 * [allocType]
 *		ALLOC_TOP		ヒープ先頭から確保
 *		ALLOC_BOTTOM	ヒープ後方から確保
 *
 * @retval  void*			読み出し領域ポインタ
 */
//------------------------------------------------------------------
void* ArcUtil_HDL_Load(ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, u32 heapID, ALLOC_TYPE allocType)
{
	u32 siz_dmy;
	return ArcUtil_HDL_LoadEx( handle, dataIdx, compressedFlag, heapID, allocType, &siz_dmy );
}

//------------------------------------------------------------------
/**
 * アーカイブデータの読み出し＆データサイズ取得（圧縮されていたら解凍後のサイズを取得する）
 *
 * @param   handle			ハンドル
 * @param   dataIdx			アーカイブデータインデックス
 * @param   compressedFlag	圧縮されているか？
 * @param   heapID			メモリ確保に使うヒープＩＤ
 * @param   allocType		ヒープのどの位置からメモリ確保するか
 * @param   pSize			実データのバイトサイズを受け取る変数のポインタ
 *
 * [allocType]
 *		ALLOC_TOP		ヒープ先頭から確保
 *		ALLOC_BOTTOM	ヒープ後方から確保
 *
 * @retval  void*			読み出し領域ポインタ
 */
//------------------------------------------------------------------
void* ArcUtil_HDL_LoadEx(ARCHANDLE* handle, u32 dataIdx, BOOL compressedFlag, u32 heapID, ALLOC_TYPE allocType, u32* pSize)
{
	void* arcData;

	*pSize = ArchiveDataSizeGetByHandle(handle, dataIdx);

	if( compressedFlag || allocType == ALLOC_BOTTOM )
	{
		arcData = sys_AllocMemoryLo( heapID, *pSize );
	}
	else
	{
		arcData = sys_AllocMemory( heapID, *pSize );
	}

	if( arcData != NULL )
	{
		ArchiveDataLoadByHandle(handle, dataIdx, arcData);

		if( compressedFlag )
		{
			void* data;

			*pSize = MI_GetUncompressedSize( arcData );

			if( allocType == ALLOC_TOP )
			{
				data = sys_AllocMemory( heapID, *pSize );
			}
			else
			{
				data = sys_AllocMemoryLo( heapID, *pSize );
			}

			if( data )
			{
				MI_UncompressLZ8( arcData, data );
				sys_FreeMemoryEz( arcData );
			}
			arcData = data;
		}
	}
	return arcData;
}


//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	BGキャラデータのVRAM転送共通部分
 *
 *	@param	p_data			データ
 *	@param	bgl				BGLデータ
 *	@param	frm				転送先フレームナンバ
 *	@param	offs			転送オフセット（キャラ単位）
 *	@param	transSize		転送サイズ（バイト単位 ==0で全転送）
 *
 *	@return	転送したデータサイズ（バイト）
 */
//-----------------------------------------------------------------------------
static u32 ArcUtil_BgCharSetCommon( void* p_data, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize )
{
	if( p_data != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedBGCharacterData( p_data, &charData ) )
		{
			if( transSize == 0 )
			{
				transSize = charData->szByte;
			}
			GF_BGL_LoadCharacter(bgl, frm, charData->pRawData, transSize, offs);
		}

		sys_FreeMemoryEz( p_data );
	}
    return transSize;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータVRAM転送
 *	※ BGL側に ｽｸﾘｰﾝﾊﾞｯﾌｧ が用意されていれば、ｽｸﾘｰﾝﾊﾞｯﾌｧ への転送も行う
 *
 *	@param	p_data			データ
 *	@param	bgl				GBLデータ
 *	@param	frm				転送先フレーム
 *	@param	offs			転送オフセット（キャラ単位）
 *	@param	transSize		転送サイズ（バイト単位 ==0で全転送）
 */
//-----------------------------------------------------------------------------
static void ArcUtil_ScrnSetCommon( void* p_data, GF_BGL_INI* bgl, u32 frm, u32 offs, u32 transSize )
{
	if( p_data != NULL )
	{
		NNSG2dScreenData* scrnData;

		if( NNS_G2dGetUnpackedScreenData( p_data, &scrnData ) )
		{
			if( transSize == 0 )
			{
				transSize = scrnData->szByte;
			}

			if( GF_BGL_ScreenAdrsGet( bgl, frm ) != NULL )
			{
				GF_BGL_ScreenBufSet( bgl, frm, scrnData->rawData, transSize );
			}
			GF_BGL_LoadScreen( bgl, frm, scrnData->rawData, transSize, offs );
		}
		sys_FreeMemoryEz( p_data );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ ｷｬﾗﾃﾞｰﾀ の VRAM 転送
 *
 *	@param	p_data		データ
 *	@param	objType		OBJタイプ（OBJTYPE_MAIN or OBJTYPE_SUB）
 *	@param	offs		オフセット（バイト単位）
 *	@param	transSize	転送サイズ（バイト単位 ==0で全転送）
 *
 *	@return	転送したデータサイズ（バイト単位）
 */
//-----------------------------------------------------------------------------
static u32 ArcUtil_ObjCharSetCommon( void* p_data, OBJTYPE objType, u32 offs, u32 transSize )
{
	static void (* const load_func[])(const void*, u32, u32) = {
		GX_LoadOBJ,
		GXS_LoadOBJ,
	};

	if( p_data != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( p_data, &charData ) )
		{
			if( transSize == 0 )
			{
				transSize = charData->szByte;
			}

			DC_FlushRange( charData->pRawData, transSize );
			load_func[ objType ]( charData->pRawData, offs, transSize );
		}
		sys_FreeMemoryEz( p_data );
	}
	return transSize;
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットデータのVRAM転送
 *
 *	@param	p_data			データ
 *	@param	palType			パレット転送タイプ
 *	@param	srcOfs			パレット転送元読み込み開始オフセット
 *	@param	dstOfs			パレット転送先オフセット
 *	@param	transSize		転送サイズ（０で全転送）
 */
//-----------------------------------------------------------------------------
static void ArcUtil_PalSetEzCommon( void* p_data, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize )
{
	static void (* const load_func[])(const void*, u32, u32) = {
		GX_LoadBGPltt,
		GX_LoadOBJPltt,
		GX_LoadBGExtPltt,
		GX_LoadOBJExtPltt,
		GXS_LoadBGPltt,
		GXS_LoadOBJPltt,
		GXS_LoadBGExtPltt,
		GXS_LoadOBJExtPltt,
	};

	if( p_data != NULL )
	{
		NNSG2dPaletteData* palData;

		if( NNS_G2dGetUnpackedPaletteData( p_data, &palData ) )
		{
			(u8*)(palData->pRawData) += srcOfs;
			if( transSize == 0 )
			{
				transSize = palData->szByte - srcOfs;
			}
			DC_FlushRange( palData->pRawData, transSize );

			switch( palType ){
			case PALTYPE_MAIN_BG_EX:
				GX_BeginLoadBGExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GX_EndLoadBGExtPltt();
				break;

			case PALTYPE_SUB_BG_EX:
				GXS_BeginLoadBGExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GXS_EndLoadBGExtPltt();
				break;

			case PALTYPE_MAIN_OBJ_EX:
				GX_BeginLoadOBJExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GX_EndLoadOBJExtPltt();
				break;

			case PALTYPE_SUB_OBJ_EX:
				GXS_BeginLoadOBJExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GXS_EndLoadOBJExtPltt();
				break;

			default:
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				break;
			}
		}

		sys_FreeMemoryEz( p_data );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットデータのVRAM転送＆NITROSystemパレットプロクシを作成
 *	（3D,OBJ用にのみ対応　BGには使いません）
 *
 *	@param	p_data		データ
 *	@param	type		転送先タイプ
 *	@param	offs		転送オフセット
 *	@param	proxy		作成するプロクシ
 *	[ type ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 */
//-----------------------------------------------------------------------------
static void ArcUtil_PalSysLoadCommon( void* p_data, NNS_G2D_VRAM_TYPE type, u32 offs, NNSG2dImagePaletteProxy* proxy )
{
	if( p_data != NULL )
	{
		NNSG2dPaletteData*  palData;
		NNSG2dPaletteCompressInfo*  cmpData;
		BOOL  cmpFlag;

		cmpFlag = NNS_G2dGetUnpackedPaletteCompressInfo( p_data, &cmpData );

		if( NNS_G2dGetUnpackedPaletteData( p_data, &palData ) )
		{
			if( cmpFlag )
			{
				NNS_G2dLoadPaletteEx( palData, cmpData, offs, type, proxy );
			}
			else
			{
				NNS_G2dLoadPalette( palData, offs, type, proxy );
			}
		}

		sys_FreeMemoryEz( p_data );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラデータのVRAM転送＆NITRO Systemイメージプロキシを作成
 *	（3D,OBJ用にのみ対応　BGには使いません）
 *
 *	@param	p_data			データ
 *	@param	mapType			マッピングタイプ
 *	@param	transSize		転送サイズ（０＝全転送）
 *	@param	vramType		転送先タイプ
 *	@param	offs			転送先オフセット
 *	@param	proxy			作成するプロキシアドレス
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 *	@return	転送したデータサイズ
 */
//-----------------------------------------------------------------------------
static u32 ArcUtil_CharSysLoadCommon( void* p_data, CHAR_MAPPING_TYPE mapType, u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy )
{
	static void (* const load_func[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
		NNS_G2dLoadImage1DMapping,
		NNS_G2dLoadImage2DMapping,
	};
	u32  transed_size = 0;

	if( p_data != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( p_data, &charData ) )
		{
			if( transSize )
			{
				charData->szByte = transSize;
			}
			load_func[ mapType ]( charData, offs, vramType, proxy );
			transed_size = charData->szByte;
		}
		sys_FreeMemoryEz( p_data );
	}

	return transed_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラデータのVRAM転送＆NITRO Systemイメージプロキシを作成
 *	（3D,OBJ用にのみ対応　BGには使いません）
 * ｷｬﾗﾃﾞｰﾀのﾏｯﾋﾟﾝｸﾞﾓｰﾄﾞ値を、現在のﾚｼﾞｽﾀ設定に合わせて書き換えます
 *
 *	@param	p_data			データ
 *	@param	mapType			マッピングタイプ
 *	@param	transSize		転送サイズ（０＝全転送）
 *	@param	vramType		転送先タイプ
 *	@param	offs			転送先オフセット
 *	@param	proxy			作成するプロキシアドレス
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	※ VRAM転送型は別関数 ArcUtil_TranCharSysLoad を使う
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 */
//-----------------------------------------------------------------------------
static void ArcUtil_CharSysLoadSyncroMappingModeCommon( void* p_data, CHAR_MAPPING_TYPE mapType, u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy )
{
	static void (* const load_func[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
		NNS_G2dLoadImage1DMapping,
		NNS_G2dLoadImage2DMapping,
	};

	if( p_data != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( p_data, &charData ) )
		{
			if( transSize )
			{
				charData->szByte = transSize;
			}

			switch( vramType ){
			case NNS_G2D_VRAM_TYPE_2DMAIN:
				charData->mapingType = GX_GetOBJVRamModeChar();
				break;
			case NNS_G2D_VRAM_TYPE_2DSUB:
				charData->mapingType = GXS_GetOBJVRamModeChar();
				break;
			}
			load_func[ mapType ]( charData, offs, vramType, proxy );
		}
		sys_FreeMemoryEz( p_data );
	}
}	

//----------------------------------------------------------------------------
/**
 *	@brief	ｷｬﾗﾃﾞｰﾀ の VRAM 転送＆ NITRO System ｲﾒｰｼﾞﾌﾟﾛｷｼ を作成。VRAM転送型の画像素材用。
 *
 * ※ この関数を使っても、VRAMに画像は転送されません
 *    この関数でﾛｰﾄﾞしたﾃﾞｰﾀは解放されません。戻り値のconst void*を管理して、
 *    不要になったら解放処理を行ってください。
 *
 *	@param	p_data			データ
 *	@param	vramType		転送先タイプ
 *	@param	offs			転送オフセット
 *	@param	proxy			作成するプロキシ
 *	@param	charData		キャラクタデータアドレス格納先
 *
 *	@return const void*		ロードしたデータアドレス
 */
//-----------------------------------------------------------------------------
static const void* ArcUtil_TransTypeCharSysLoadCommon( void* p_data, NNS_G2D_VRAM_TYPE vramType, u32 offs, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedCharacterData( p_data, charData ) )
		{
			NNS_G2dLoadImageVramTransfer( *charData, offs, vramType, proxy );
		}
	}
	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタデータをアンパック
 *
 *	@param	p_data		データ
 *	@param	charData	キャラクタデータアンパック先
 *
 *	@return データワーク
 */
//-----------------------------------------------------------------------------
static void* ArcUtil_CharDataGetCommon( void* p_data, NNSG2dCharacterData** charData )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedBGCharacterData( p_data, charData ) == FALSE)
		{
			// 失敗したらNULL
			sys_FreeMemoryEz( p_data );
			return NULL;
		}
	}
	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータアンパック
 *
 *	@param	p_data		データ
 *	@param	scrnData	スクリーンデータ格納先
 *
 *	@return データワーク
 */
//-----------------------------------------------------------------------------
static void* ArcUtil_ScrnDataGetCommon( void* p_data, NNSG2dScreenData** scrnData )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedScreenData( p_data, scrnData ) == FALSE )
		{
			sys_FreeMemoryEz( p_data );
			return NULL;
		}
	}
	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットデータアンパック関数
 *
 *	@param	p_data		データ
 *	@param	palData		パレットデータ格納先
 *
 *	@return データワーク
 */
//-----------------------------------------------------------------------------
static void* ArcUtil_PalDataGetCommon( void* p_data, NNSG2dPaletteData** palData )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedPaletteData( p_data, palData ) == FALSE )
		{
			sys_FreeMemoryEz( p_data );
			return NULL;
		}
	}
	return p_data;
}	

//----------------------------------------------------------------------------
/**
 *	@brief	セルデータアンパック関数
 *
 *	@param	p_data		データ
 *	@param	cellBank	セルデータ格納先
 *
 *	@return	データワーク
 */
//-----------------------------------------------------------------------------
static void* ArcUtil_CellBankDataGetCommon( void* p_data, NNSG2dCellDataBank** cellBank )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedCellBank( p_data, cellBank ) == FALSE )
		{
			sys_FreeMemoryEz( p_data );
			return NULL;
		}
	}
	return p_data;
}	

//----------------------------------------------------------------------------
/**
 *	@brief	セルアニメデータアンパック
 *
 *	@param	p_data		データ
 *	@param	anmBank		セルアニメデータ格納先
 *
 *	@return	データワーク
 */
//-----------------------------------------------------------------------------
static void* ArcUtil_AnimBankDataGetCommon( void* p_data, NNSG2dAnimBankData** anmBank )
{
	if( p_data != NULL )
	{
		if( NNS_G2dGetUnpackedAnimBank( p_data, anmBank ) == FALSE )
		{
			sys_FreeMemoryEz( p_data );
			return NULL;
		}
	}
	return p_data;
}
