#ifndef _HEIGHT_H_
#define _HEIGHT_H_
#include <nitro.h>

#include "common.h"
#include "../height_def.h"

//#include "grid_def.h"

//XZ頂点データ
typedef struct XZ_VERTEX_tag
{
	fx32 X;
	fx32 Z;
}XZ_VERTEX;
/**
//ポリゴンデータ部
typedef struct SPLIT_GRID_DATA_tag{
	u16		NumX;		//グリッドＸ数
	u16		NumZ;		//グリッドＺ数

	fx32	StartX;		//始点位置Ｘ
	fx32	StartZ;		//始点位置Ｚ
	fx32	EndX;		//終点位置Ｘ
	fx32	EndZ;		//終点位置Ｚ
	fx32	SizeX;		//グリッドＸサイズ
	fx32	SizeZ;		//グリッドＺサイズ
}SPLIT_GRID_DATA;
*/
typedef struct POLYGON_DATA_tag{
	u16 vtx_idx0;
	u16 vtx_idx1;
///	u16 vtx_idx2;	//3角ポリゴンを形成する、頂点データ配列へのインデックスNo
	u16	nrm_idx;	//法線データ配列(正規化済み)へのインデックスNo
	
///	fx32	paramD;		//平面の方程式から導いた、補正値D
	u16 d_idx;
}POLYGON_DATA;

typedef struct HEIGHT_ARRAY_tag
{
	fx32 Height;
	int Prev;
	int Next;
}HEIGHT_ARRAY;

typedef struct LINE_DATA_tag
{
	fx32 LineZ;
	u16 EntryNum;
	u16 ParamIndex;
}LINE_DATA;

typedef struct MAP_HEIGHT_INFO_tag{
///	SPLIT_GRID_DATA	*SplitGridData;
	POLYGON_DATA	*PolygonData;
	fx32			*DArray;
///	u16				*GridDataTbl;
	/*u16*/LINE_DATA				*LineDataTbl;
	u16				*PolyIDList;
	XZ_VERTEX		*VertexArray;
	VecFx32			*NormalArray;
	BOOL			DataValid;		//データ有効有無
	int				LineNum;
}MAP_HEIGHT_INFO;

#endif	//_HEIGHT_H_
