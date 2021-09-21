//============================================================================================
/**
 * @file	mat_shp.c
 * @bfief	マット・シェイプデータ関連ファイル
 * @author	Nozomu Saitou
 *
 */
//============================================================================================
#include "mat_shp.h"

//1mat1shape対応データ　バイナリフォーマット



typedef struct MAT_SHP_HEADER_tag
{
	u16 Num;	//オブジェクトがもつMAT_SHPの要素数
	u16	Idx;	//MAT_SHP配列の参照IndexNo
}MAT_SHP_HEADER;

typedef struct MAT_SHP_DATA_tag
{
	MAT_SHP_HEADER	*Header;
	MAT_SHP			*MatShp;
}MAT_SHP_DATA;

//==============================================================================
/**
 * マット・シェイプデータのメモリ確保
 *
 * @retval  MSD_PTR		マット・シェイプデータポインタ	
 */
//==============================================================================
MSD_PTR AllocMatShpData(void)
{
	MSD_PTR data;
	data = sys_AllocMemory( HEAPID_FIELD, sizeof(MAT_SHP_DATA) );
	GF_ASSERT( data != NULL && "ERROR:mat_shp no enough memory" );
	return data;
}

//==============================================================================
/**
 * マット・シェイプデータのロード
 *
 * @param   inName			ファイルパス
 * @param	outData			マット・シェイプデータポインタ
 *
 * @retval  none	
 */
//==============================================================================
void LoadMatShape(const char * inName, MSD_PTR outData)
{
	FSFile file;
	u16 obj_num;
	u16 mat_shp_num;
	int header_size;
	int mat_shp_size;

	FS_InitFile(&file);
	if (FS_OpenFile(&file, inName)){
		int ret;
		ret = FS_ReadFile(&file, &obj_num, 2);
		GF_ASSERT( ret >= 0 && "ERROR:mat_shp can not read" );
		
		ret = FS_ReadFile(&file, &mat_shp_num, 2);
		GF_ASSERT( ret >= 0 && "ERROR:mat_shp can not read" );
		
		//メモリ確保
		header_size = sizeof(MAT_SHP_HEADER)*obj_num;
		mat_shp_size = sizeof(MAT_SHP)*mat_shp_num;

		outData->Header = sys_AllocMemory( HEAPID_FIELD, header_size );
		GF_ASSERT( outData->Header != NULL&& "ERROR:mat_shp no enough memory" );
		outData->MatShp = sys_AllocMemory( HEAPID_FIELD, mat_shp_size );
		GF_ASSERT( outData->MatShp != NULL&& "ERROR:mat_shp no enough memory" );

		ret = FS_ReadFile(&file, outData->Header, header_size);
		GF_ASSERT( ret >= 0 && "ERROR:mat_shp can not read" );
		ret = FS_ReadFile(&file, outData->MatShp, mat_shp_size);
		GF_ASSERT( ret >= 0 && "ERROR:mat_shp can not read" );

		(void)FS_CloseFile(&file);
	}else{
		OS_Printf("FS_OpenFile(\"%s\") ... ERROR!\n",inName);
		GF_ASSERT(0);
	}
}

//==============================================================================
/**
 * マット・シェイプデータの解放
 *
 * @param	outData			マット・シェイプデータポインタ
 *
 * @retval  none	
 */
//==============================================================================
void FreeMatShpData(MSD_PTR outData)
{
	sys_FreeMemoryEz(outData->MatShp);
	sys_FreeMemoryEz(outData->Header);
	sys_FreeMemoryEz(outData);
}

//==============================================================================
/**
 * マット・シェイプ数の取得
 *
 * @param   inObjIdx		3ＤOBJインデックス
 * @param	inData			マット・シェイプデータポインタ
 * @param	outNum			マット・シェイプ数
 *
 * @retval  none	
 */
//==============================================================================
void GetMatShpNum(const int inObjIdx,
						MSD_CONST_PTR inData,
						u16 *outNum)
{
	*outNum = inData->Header[inObjIdx].Num;
}

//==============================================================================
/**
 * マット・シェイプ数とインデックスを取得
 *
 * @param   inObjIdx		3ＤOBJインデックス
 * @param	inData			マット・シェイプデータポインタ
 * @param	outNum			マット・シェイプ数
 * @param	outIdx			インデックス
 *
 * @retval  none	
 */
//==============================================================================
void GetMatShpNumAndIdx(const int inObjIdx,
						MSD_CONST_PTR inData,
						u16 *outNum,
						u16 *outIdx)
{
	*outNum = inData->Header[inObjIdx].Num;
	*outIdx = inData->Header[inObjIdx].Idx;
}

//==============================================================================
/**
 * インデックスをキーにして、マット・シェイプを取得
 *
 * @param   inIdx			インデックス
 * @param	inData			マット・シェイプデータポインタ
 *
 * @retval  MAT_SHP			マット・シェイプ構造体ポインタ
 */
//==============================================================================
MAT_SHP const *GetMatShp(const u16 inIdx, MSD_CONST_PTR inData)
{
	return &inData->MatShp[inIdx];
}

