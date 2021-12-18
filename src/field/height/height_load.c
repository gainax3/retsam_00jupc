#include "common.h"
#include "height.h"
#include "system/arc_tool.h"

///#define GRID_ENTRY_HEADER_SIZE	(4)

#define DIV_HEIGHT_DATA_SIZE	(/*0x4000*/0xe000)

#define HEIGHT_HEADER_SIZE	(/*40*/20)

///#define INVALIED_HEIGHT_MEM_ALOOC

typedef struct {
	int VtxNum;
	int NrmNum;
	int DNum;
	int PolygonNum;
///	int GridNum;
///	int TotalPolyIDListSize;
///	int TotalLineListSize;
	int LineNum;
	int TotalPolyIDListNum;
}READ_INFO;

typedef struct {
	FSFile  file;
	int DataOffset;
	READ_INFO	ReadInfo;
	BOOL    OpenIsOK;
	u8 Seq;
	u8 *HeightMem;
	MHI_PTR	HeightData;
	BOOL	StopFlg;
	int *EndFlg;
	ARCHANDLE *ArcHandle;
	int DataSize;
	int *Counter;
}HEIGHT_TASK_DATA;


//////////////////////////////////////////////////////////////////////////////
// 前方宣言
//////////////////////////////////////////////////////////////////////////////
static void SetDataMem(const READ_INFO *inInfo, MHI_PTR	outMapHeightInfo, void **mem);

///static void ReadGridSpDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo);
static void ReadVtxDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
static void ReadNrmDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
static void ReadDDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
static void ReadPolygonDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
///static void ReadGridDataTblArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
static void ReadLineDataTblArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);
static void ReadEntryDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo);


#ifdef PM_DEBUG
extern u32 sys_GetMemoryBlockSize( const void* memBlock );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

//ヘッダーデータ4+16バイトを読む
static void ReadHeaderDataArc(ARCHANDLE *ioHandle, READ_INFO *outInfo)
{	
	u16 temp[4];

	//格納先のクリーン
	MI_CpuClear32(outInfo,sizeof(READ_INFO));

	//IDデータ4バイトを読み飛ばす
	ArchiveDataLoadByHandleContinue( ioHandle, 4, temp );
	//頂点数
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->VtxNum );
	//法線数
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->NrmNum );
	//D値数
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->DNum );
	//四角ポリゴン数
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->PolygonNum );
	//ライン総数
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->LineNum/*TotalLineListSize*/ );	
	//ポリゴンインデックス実データ
	ArchiveDataLoadByHandleContinue( ioHandle, 2, &outInfo->TotalPolyIDListNum/*TotalPolyIDListSize*/ );

OS_Printf("head:%d,%d,%d,%d,%d,%d\n",outInfo->VtxNum,outInfo->NrmNum,outInfo->DNum,
		outInfo->PolygonNum,outInfo->LineNum,outInfo->TotalPolyIDListNum);	
}

static void SetDataMem(const READ_INFO *inInfo, MHI_PTR	outMapHeightInfo, void **mem)
{
	void *p;
	int offset;
	u8 mod;
	offset = 0;
#ifdef HEIGHT_DATA_MEMORRY_DEBUG
	OS_Printf("Before Get Height Data:Graphic_Heap = %x\n",sys_GetHeapFreeSize(HEAPID_FIELD));
#endif

	//頂点データ
	p = &((u8*)(*mem))[0];
	outMapHeightInfo->VertexArray = p;//(void *)&data[0];
	offset += (sizeof(XZ_VERTEX)*inInfo->VtxNum);
	OS_Printf("ofs=%d\n",offset);
/**	
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
	//法線データ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->NormalArray = p;//(void *)&data[offset];
	offset += (sizeof(VecFx32)*inInfo->NrmNum);
/**
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
	//Dデータ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->DArray = p;//(void *)&data[offset];
	offset += (sizeof(fx32)*inInfo->DNum);
/**
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
	//ポリゴンデータ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->PolygonData = p;//(void *)&data[offset];
	offset += (sizeof(POLYGON_DATA)*inInfo->PolygonNum);
/*
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
/**	
	//グリッド分割データ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->SplitGridData = p;//(void *)&data[offset];
	offset += (sizeof(SPLIT_GRID_DATA));
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
	//グリッドテーブルデータ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->GridDataTbl = p;//(void *)&data[offset];
	offset += (GRID_ENTRY_HEADER_SIZE*inInfo->GridNum);
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
	//ラインテーブルデータ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->LineDataTbl = p;//(void *)&data[offset];
	offset += /*(inInfo->TotalLineListSize)*/sizeof(LINE_DATA)*inInfo->LineNum;
/**	
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	
	//ライン内ポリゴンIDエントリデータ
	p = &((u8*)(*mem))[offset];
	outMapHeightInfo->PolyIDList = p;//(void *)&data[offset];
	offset += sizeof(u16)*inInfo->TotalPolyIDListNum/*(inInfo->TotalPolyIDListSize)*/;
/**
	mod = offset%4;
	if (mod != 0){
		offset += mod;
	}
*/	

#ifdef DEBUG_ONLY_FOR_saitou
	OS_Printf("local_print height_data_toal:%x\n",
			(sizeof(XZ_VERTEX)*inInfo->VtxNum)+(sizeof(VecFx32)*inInfo->NrmNum)+ \
			(sizeof(fx32)*inInfo->DNum)+ \
			(sizeof(POLYGON_DATA)*inInfo->PolygonNum)+/*sizeof(SPLIT_GRID_DATA)+*/ \
			(sizeof(LINE_DATA)*inInfo->LineNum)+ \
			/*(GRID_ENTRY_HEADER_SIZE*inInfo->GridNum)+inInfo->TotalLineListSize+ \*/
			(sizeof(u16)*inInfo->TotalPolyIDListNum)/*inInfo->TotalPolyIDListSize*/);
#endif
	
	///GF_ASSERT((offset<=0x8000)&&"高さデータメモリオーバー");
	GF_ASSERT(offset<=0x9000&&"高さデータが大きすぎです");
}

///ARC
#if 0
//グリッド分割データをリード
static void ReadGridSpDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, sizeof(SPLIT_GRID_DATA),
									outMap3DInfo->SplitGridData );
}
#endif
//ヘッダから取得した、頂点数のデータを読む
static void ReadVtxDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, sizeof(XZ_VERTEX)*inInfo->VtxNum,
									outMap3DInfo->VertexArray );
}

static void ReadNrmDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, sizeof(VecFx32)*inInfo->NrmNum,
									outMap3DInfo->NormalArray );
}

static void ReadDDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, sizeof(fx32)*inInfo->DNum,
									outMap3DInfo->DArray );
}

static void ReadPolygonDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, sizeof(POLYGON_DATA)*inInfo->PolygonNum,
									outMap3DInfo->PolygonData );
}
/**
static void ReadGridDataTblArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, GRID_ENTRY_HEADER_SIZE*inInfo->GridNum,
									outMap3DInfo->GridDataTbl );
}
*/
static void ReadLineDataTblArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, /*inInfo->TotalLineListSize*/sizeof(LINE_DATA)*inInfo->LineNum,
									outMap3DInfo->LineDataTbl );
}

static void ReadEntryDataArc(ARCHANDLE *ioHandle, MHI_PTR outMap3DInfo, const READ_INFO *inInfo)
{
	ArchiveDataLoadByHandleContinue(ioHandle, /*inInfo->TotalPolyIDListSize*/sizeof(u16)*inInfo->TotalPolyIDListNum,
									outMap3DInfo->PolyIDList );
}

#define HEIGHT_END_SEQ	(2)

static void HeightDataArcTask(TCB_PTR _tcb, void * work)
{
	BOOL rc;
	HEIGHT_TASK_DATA	*height_task_data;
	height_task_data = (HEIGHT_TASK_DATA	*)work;
	if (height_task_data->StopFlg == TRUE){
		height_task_data->Seq = HEIGHT_END_SEQ;
	}

	switch(height_task_data->Seq){
	case 0:
		if (*height_task_data->Counter){
			rc = FALSE;
			break;
		}

		ReadHeaderDataArc(height_task_data->ArcHandle, &height_task_data->ReadInfo);
		//ライン数保存
		height_task_data->HeightData->LineNum = height_task_data->ReadInfo.LineNum;
		
		SetDataMem(&height_task_data->ReadInfo, height_task_data->HeightData, (void**)&height_task_data->HeightMem);

		rc = TRUE;
		break;
	case 1:
///		ReadGridSpDataArc(height_task_data->ArcHandle,height_task_data->HeightData);
		ReadVtxDataArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		ReadNrmDataArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		ReadDDataArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		ReadPolygonDataArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
///		ReadGridDataTblArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		ReadLineDataTblArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		ReadEntryDataArc(height_task_data->ArcHandle,height_task_data->HeightData, &height_task_data->ReadInfo);
		rc = TRUE;
		break;
		
	case HEIGHT_END_SEQ:
		*height_task_data->EndFlg = 0;
		sys_FreeMemoryEz((void *)work);
		TCB_Delete(_tcb);
		return;
	}
	if (rc == TRUE){
		height_task_data->Seq++;
		if(height_task_data->Seq == HEIGHT_END_SEQ){
			height_task_data->HeightData->DataValid = TRUE;		//データ有効
		}
	}
}
#if 0
#ifdef	INVALIED_HEIGHT_MEM_ALOOC

static void FreeMHIMember(MHI_PTR outMap3DInfo)
{
	//頂点データ
	if (outMap3DInfo->VertexArray != NULL){
		sys_FreeMemoryEz(outMap3DInfo->VertexArray);
		outMap3DInfo->VertexArray = NULL;
	}
	//法線データ
	if (outMap3DInfo->NormalArray != NULL){
		sys_FreeMemoryEz(outMap3DInfo->NormalArray);
		outMap3DInfo->NormalArray = NULL;
	}
	//ポリゴンデータ
	if (outMap3DInfo->PolygonData != NULL){
		sys_FreeMemoryEz(outMap3DInfo->PolygonData);
		outMap3DInfo->PolygonData = NULL;
	}
	//グリッド分割データ
	if (outMap3DInfo->SplitGridData != NULL){
		sys_FreeMemoryEz(outMap3DInfo->SplitGridData);
		outMap3DInfo->SplitGridData = NULL;	
	}
	//グリッドテーブルデータ
	if (outMap3DInfo->GridDataTbl != NULL){
		sys_FreeMemoryEz(outMap3DInfo->GridDataTbl);
		outMap3DInfo->GridDataTbl = NULL;
	}
	//ラインテーブルデータ
	if (outMap3DInfo->LineDataTbl != NULL){
		sys_FreeMemoryEz(outMap3DInfo->LineDataTbl);
		outMap3DInfo->LineDataTbl = NULL;
	}
	//グリッド実データ
	if (outMap3DInfo->PolyIDList != NULL){
		sys_FreeMemoryEz(outMap3DInfo->PolyIDList);
		outMap3DInfo->PolyIDList = NULL;
	}
}
#endif //INVALIED_HEIGHT_MEM_ALOOC
#endif
////////////////////////////////////////////////////////////////////////////////////////
MHI_PTR	AllocMapHeightInfo(void)
{
	MHI_PTR ptr;
	ptr = sys_AllocMemory(HEAPID_FIELD,sizeof(MAP_HEIGHT_INFO));
	ptr->VertexArray = NULL;
	ptr->NormalArray = NULL;
	ptr->PolygonData = NULL;
///	ptr->SplitGridData = NULL;	
///	ptr->GridDataTbl = NULL;
	ptr->LineDataTbl = NULL;
	ptr->PolyIDList = NULL;
	ptr->PolyIDList = NULL;
	ptr->DataValid = FALSE;
	ptr->LineNum = 0;
	return ptr;
}

void SetupHeightDataArc(ARCHANDLE *ioHandle,
						const int inDataSize,
						MHI_PTR outMapHeightInfo, u8 * inHeightMem)
{
    if(1) {
        char    buf[256];
		READ_INFO *read_info;
		//一時メモリ確保
		read_info = sys_AllocMemoryLo(HEAPID_FIELD,sizeof(READ_INFO));

		//アーカイブデータ取得
		ReadHeaderDataArc(ioHandle, read_info);
		//ライン数保存
		outMapHeightInfo->LineNum = read_info->LineNum;
#ifdef PM_DEBUG
		{
			//サイズ＝確保メモリ-管理ヘッダ
			u32 size = sys_GetMemoryBlockSize((void*)inHeightMem)-(0x10);
			if ((inDataSize!=0)&&(inDataSize<=HEIGHT_HEADER_SIZE)){
				GF_ASSERT( 0 && "SetupHeightData ERROR:ファイルが不正");
			}
			//ファイルサイズから固定ヘッダ（高さ読み込みヘッダ）を引いた分が確保したメモリを超えるかをチェック
			if ((inDataSize!=0)&&(inDataSize-HEIGHT_HEADER_SIZE>size)){
				OS_Printf("fileSize=%x,allocSize=%x",inDataSize,size);
				GF_ASSERT( 0 && "SetupHeightData ERROR:メモリが不足してます");
			}

		}
#endif
		SetDataMem(read_info, outMapHeightInfo, (void **)&inHeightMem);
		
///		ReadGridSpDataArc(ioHandle,outMapHeightInfo);
		ReadVtxDataArc(ioHandle,outMapHeightInfo, read_info);
		ReadNrmDataArc(ioHandle,outMapHeightInfo, read_info);
		ReadDDataArc(ioHandle,outMapHeightInfo, read_info);
		ReadPolygonDataArc(ioHandle,outMapHeightInfo, read_info);
///		ReadGridDataTblArc(ioHandle,outMapHeightInfo, read_info);
		ReadLineDataTblArc(ioHandle,outMapHeightInfo, read_info);
		ReadEntryDataArc(ioHandle,outMapHeightInfo, read_info);

		//デバッグ：中身の表示
		{
			int i;
			for(i=0;i<read_info->VtxNum;i++){
				OS_Printf("VERT:%x,%x\n",outMapHeightInfo->VertexArray[i].X, outMapHeightInfo->VertexArray[i].Z);
			}
			for(i=0;i<read_info->NrmNum;i++){
				OS_Printf("NORM:%x,%x,%x\n",outMapHeightInfo->NormalArray[i].x, outMapHeightInfo->NormalArray[i].y, outMapHeightInfo->NormalArray[i].z);
			}
			for(i=0;i<read_info->DNum;i++){
				OS_Printf("D:%x\n",outMapHeightInfo->DArray[i]);
			}
			for(i=0;i<read_info->PolygonNum;i++){
				OS_Printf("Poly:%d,%d,%d,%d\n",outMapHeightInfo->PolygonData[i].vtx_idx0, outMapHeightInfo->PolygonData[i].vtx_idx1,
						outMapHeightInfo->PolygonData[i].nrm_idx, outMapHeightInfo->PolygonData[i].d_idx);
			}
			for(i=0;i<read_info->LineNum;i++){
				OS_Printf("Line:%x,%d,%d\n",outMapHeightInfo->LineDataTbl[i].LineZ,
						outMapHeightInfo->LineDataTbl[i].EntryNum,
						outMapHeightInfo->LineDataTbl[i].ParamIndex);
			}
			for(i=0;i<read_info->TotalPolyIDListNum;i++){
				OS_Printf("PolyList:%d\n",outMapHeightInfo->PolyIDList[i]);
			}

		}
		

		//一時メモリ解放
		sys_FreeMemoryEz(read_info);

		outMapHeightInfo->DataValid = TRUE;		//データ有効
	}
}

void FreeMapHeightInfo(MHI_PTR outMap3DInfo)
{
	if (outMap3DInfo == NULL)
	{
		return;
	}
#ifdef	INVALIED_HEIGHT_MEM_ALOOC
	FreeMHIMember(outMap3DInfo);
#endif //INVALIED_HEIGHT_MEM_ALOOC
	sys_FreeMemoryEz(outMap3DInfo);
	outMap3DInfo = NULL;
}

void SetInvalidHeightData(MHI_PTR outMap3DInfo)
{
	if (outMap3DInfo == NULL){
		return;
	}
	outMap3DInfo->DataValid = FALSE;		//データ無効
#ifdef	INVALIED_HEIGHT_MEM_ALOOC
	FreeMHIMember(outMap3DInfo);
#else
	//頂点データ
	outMap3DInfo->VertexArray = NULL;
	//法線データ
	outMap3DInfo->NormalArray = NULL;
	//ポリゴンデータ
	outMap3DInfo->PolygonData = NULL;
	//グリッド分割データ
///	outMap3DInfo->SplitGridData = NULL;	
	//グリッドテーブルデータ
///	outMap3DInfo->GridDataTbl = NULL;
	//ラインテーブルデータ
	outMap3DInfo->LineDataTbl = NULL;
	//グリッド実データ
	outMap3DInfo->PolyIDList = NULL;
#endif //INVALIED_HEIGHT_MEM_ALOOC
}

TCB_PTR SetupHeightDataArcTask(	ARCHANDLE *inHandle,
								const int inDataSize,
								MHI_PTR ioMap3DInfo,
								int *inEndFlg,
								u8 **inHeightMem,
								int *inCounter)
{
	TCB_PTR _tcb;
	HEIGHT_TASK_DATA *height_task_data;
	height_task_data = sys_AllocMemoryLo(HEAPID_FIELD,sizeof(HEIGHT_TASK_DATA));
	if (height_task_data == NULL){
		OS_Printf("No_mem\n");
	}
	height_task_data->Seq = 0;
	height_task_data->ArcHandle = inHandle;
	height_task_data->DataSize = inDataSize;
	height_task_data->HeightData = ioMap3DInfo;
	height_task_data->EndFlg = inEndFlg;
	height_task_data->StopFlg = FALSE;
	height_task_data->OpenIsOK = FALSE;
	height_task_data->DataOffset = 0;
	height_task_data->HeightMem = *inHeightMem;
	height_task_data->Counter = inCounter;
	
	_tcb = TCB_Add(HeightDataArcTask,(void*)height_task_data,1);
	return _tcb;
}

void StopHeightLoadTaskRequest(TCB_PTR tcb)
{
	HEIGHT_TASK_DATA *height_task_data;
	height_task_data = (HEIGHT_TASK_DATA*)TCB_GetWork(tcb);
	height_task_data->StopFlg = TRUE;
}



void InitHeightData(MHI_PTR outData)
{
//	outData->LoadOK = FALSE;
	outData->DataValid = FALSE;
}

BOOL GetInvalidHeightDataFlg(MHI_CONST_PTR inMap3DInfo)
{
	return inMap3DInfo->DataValid;
}
