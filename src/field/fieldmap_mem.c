#include "common.h"
#include "fieldmap_mem.h"

enum
{
	ALLOCTYPE_NON,
	ALLOCTYPE_SOURCE,
	ALLOCTYPE_MEMORY,
};

typedef struct FLD_MAP_MEM_tag
{
	u16 GroundAllocType;
	u16 HeightAllocType;
	
	void *GroundData[4];
	void *HeightData[4];
/**
	void *SeaAnm;
	void *Tex;
*/
}FLD_MAP_MEM;

static u8 FieldGroundData[4][FIELD_GROUND_DATA_SIZE];
//static u8 FieldHeightData[4][FIELD_HEIGHT_DATA_SIZE];

static void ClearFldMapMem( const u8 inIdx,
							const int inGroundMemSize,
							const int inHeightMemSize,
							FMM_PTR mem);


//メモリ確保
FMM_PTR AllocFldMapMem(const BOOL inHeightDataValid)
{
	u8 i;
	int height_mem_size;
	FMM_PTR mem;
	
	//読み込みエリア確保
	mem = sys_AllocMemory( HEAPID_FIELD, sizeof(FLD_MAP_MEM) );
	memset( mem, 0, sizeof(FLD_MAP_MEM) );
	
	mem->GroundAllocType = ALLOCTYPE_SOURCE;
	if( inHeightDataValid ){ mem->HeightAllocType = ALLOCTYPE_MEMORY; }
	
	//メモリ確保
	for(i=0;i<4;i++){
		//地形データメモリ確保
		mem->GroundData[i] = (void*)&FieldGroundData[i][0];
		if (mem->GroundData[i] == NULL){
			OS_Printf("%d,地形データメモリ確保失敗\n",i);
			return NULL;
		}
		if (inHeightDataValid){
			//高さデータメモリ確保
			mem->HeightData[i] = sys_AllocMemory( HEAPID_FIELD, FIELD_HEIGHT_DATA_SIZE );
//			mem->HeightData[i] = (void*)&FieldHeightData[i][0];
			if (mem->HeightData[i] == NULL){
				OS_Printf("%d,高さデータメモリ確保失敗\n",i);
				return NULL;
			}
			height_mem_size = FIELD_HEIGHT_DATA_SIZE;
		}else{
			mem->HeightData[i] = NULL;
			height_mem_size = 0;
		}
		ClearFldMapMem(i,FIELD_GROUND_DATA_SIZE,height_mem_size,mem);
	}
	
	//海アニメデータメモリ確保
	//地形テクスチャメモリ確保
	
	return mem;
}

//メモリ解放
void FreeFldMapMem(FMM_PTR mem)
{
	int i;
	
	//メモリ解放
	for( i = 0; i < 4; i++ ){
		//地形データメモリ解放
		if( mem->GroundAllocType == ALLOCTYPE_MEMORY ){
			sys_FreeMemoryEz( (void *)mem->GroundData[i] );
		}
		mem->GroundData[i] = NULL;
		
		//高さデータはないとき（地下とか）もあるのでＮＵＬＬチェックする
		if (mem->HeightData[i] != NULL){
			//高さデータメモリ解放
			if( mem->HeightAllocType == ALLOCTYPE_MEMORY ){
				sys_FreeMemoryEz( (void *)mem->HeightData[i] );
			}
			mem->HeightData[i] = NULL;
		}
	}
	
	sys_FreeMemoryEz((void *)mem);
	mem = NULL;
}

#if 0	//dp old
void FreeFldMapMem(FMM_PTR mem)
{
	u8 i;
	//メモリ解放
	for(i=0;i<4;i++){
		//地形データメモリ解放
		//sys_FreeMemoryEz( (void *)mem->GroundData[i] );
		mem->GroundData[i] = NULL;
		//高さデータはないとき（地下とか）もあるのでＮＵＬＬチェックする
		if (mem->HeightData[i] != NULL){
			//高さデータメモリ解放
			sys_FreeMemoryEz( (void *)mem->HeightData[i] );
			mem->HeightData[i] = NULL;
		}
	}
	sys_FreeMemoryEz((void *)mem);
	mem = NULL;
}
#endif

//地形データメモリとポインタをバインド
void BindGroundMem(const u8 inIdx, FMM_CONST_PTR mem, void **outPtr)
{
	*outPtr = mem->GroundData[inIdx];
}

//高さデータメモリとポインタをバインド
void BindHeightMem(const u8 inIdx, FMM_CONST_PTR mem, void **outPtr)
{
	*outPtr = mem->HeightData[inIdx];
}

static void ClearFldMapMem(	const u8 inIdx,
							const int inGroundMemSize,
							const int inHeightMemSize,
							FMM_PTR mem)
{
	MI_CpuClear8(mem->GroundData[inIdx],inGroundMemSize);
	MI_CpuClear8(mem->HeightData[inIdx],inHeightMemSize);
}

//アロケートタイプ
#if 0	//穴抜けを考慮した一括タイプ　サイズでか過ぎて逆に無理
FMM_PTR AllocHeapFldMapMem( const BOOL inHeightDataValid )
{
	u8 *buf;
	u32 i,size;
	FMM_PTR mem;
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	u32 b_free,a_free,m_block;
	b_free = sys_GetHeapFreeSize(HEAPID_FIELD);
	m_block = sys_GetHeapFreeSizeBlock(HEAPID_FIELD);
	#endif
	
	size = sizeof(FLD_MAP_MEM) + (FIELD_GROUND_DATA_SIZE * 4);
	if( inHeightDataValid == TRUE ){
		size += FIELD_HEIGHT_DATA_SIZE * 4;
	}
	buf = sys_AllocMemory( HEAPID_FIELD, sizeof(size) );
	GF_ASSERT( buf != NULL );
	MI_CpuClear8( buf, size );
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
	OS_Printf("HeapMapMem AllocSize=%08xH,Free Block=%08xH,Before=%08xH,After=%08xH\n",
		size,m_block,b_free,a_free);
	#endif
	
	mem = (FMM_PTR)buf;
	size = sizeof( FLD_MAP_MEM );
	
	for( i = 0; i < 4; i++ ){
		mem->GroundData[i] = (void*)&buf[size];
		size += FIELD_GROUND_DATA_SIZE;
		if( inHeightDataValid == TRUE ){
			mem->HeightData[i] = (void*)&buf[size];
			size += FIELD_HEIGHT_DATA_SIZE;
		}
	}
	
	return( mem );
}

//アロケートタイプ　メモリ開放
void FreeHeapMapMem( FMM_PTR mem )
{
	sys_FreeMemoryEz( mem );
}
#endif

//アロケートタイプ
FMM_PTR AllocHeapFldMapMem( const BOOL inHeightDataValid )
{
	u32 i;
	FMM_PTR mem;
	#ifdef DEBUG_ONLY_FOR_kagaya
	u32 b_free,a_free;
	b_free = sys_GetHeapFreeSize(HEAPID_FIELD);
	#endif
	
	mem = sys_AllocMemory( HEAPID_FIELD, sizeof(FLD_MAP_MEM) );
	memset( mem, 0, sizeof(FLD_MAP_MEM) );
	mem->GroundAllocType = ALLOCTYPE_MEMORY;
	if( inHeightDataValid ){ mem->HeightAllocType = ALLOCTYPE_MEMORY; }
	
	for( i = 0; i < 4; i++ ){
		mem->GroundData[i] = sys_AllocMemory(
				HEAPID_FIELD, FIELD_GROUND_DATA_SIZE );
		MI_CpuClear8( mem->GroundData[i], FIELD_GROUND_DATA_SIZE );
		if( inHeightDataValid == TRUE ){
			mem->HeightData[i] = sys_AllocMemory(
				HEAPID_FIELD, FIELD_HEIGHT_DATA_SIZE );
			MI_CpuClear8( mem->HeightData[i], FIELD_HEIGHT_DATA_SIZE );
		}
	}
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
	OS_Printf("HeapMapMem Before=%08xH,After=%08xH\n",b_free,a_free);
	#endif
	
	return( mem );
}

//--------------------------------------------------------------
/**
 * 2つの地形メモリを交換
 * @param	mem0	交換するFMM_PTRその1
 * @param	mem1	交換するFMM_PTRその2
 * @retval
 */
//--------------------------------------------------------------
void ExchangeFldMapMemGround( FMM_PTR mem0, FMM_PTR mem1 )
{
	int i;
	FLD_MAP_MEM mem;
	
	mem = *mem0;
	mem0->GroundAllocType = mem1->GroundAllocType;
	mem1->GroundAllocType = mem.GroundAllocType;
	
	for( i = 0; i < 4; i++ ){
		mem0->GroundData[i] = mem1->GroundData[i];
		mem1->GroundData[i] = mem.GroundData[i];
	}
}

#if 0
//--------------------------------------------------------------
/**
 * 地形メモリのアロックタイプを取得
 * @param	mem
 * @retval	BOOL	TRUE=アロックメモリ型
 */
//--------------------------------------------------------------
BOOL CheckFldMapMemGroundAlloc( FMM_PTR mem )
{
	if( mem->GroundAllocType == ALLOCTYPE_MEMORY ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif
