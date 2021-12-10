//=============================================================================
/**
 * @file	field_anime.c
 * @bfief	フィールドアニメ処理（主に転送アニメ)
 * @author	mori GAME FREAK inc.
 *
 *
 */
//=============================================================================

#include "common.h"
#include "system/arc_tool.h"
#include "calctool.h"
#include "str_tool.h"
#include "system/lib_pack.h"
#include "field_anime.h"

#include "fldtanime.naix"

//==============================================================================
// 定義
//==============================================================================
//---- kagaya debug
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_ANIME_NAME_PRINT
#endif
//----

#define FIELD_ANIME_MAX			( 16 )	 // 地形用転送アニメの同時動作上限
#define TEXTURE_NAME_LENGTH		( 16 )	 // テクスチャ指定名の最大長
#define TEXTURE_ANIME_MAX		( 18 )	 // 同時に登録できるアニメーションの数

#define TEX_ANIME_END_CODE		( 0xff ) // アニメデータの終端定義(AnmTbl用)

// アニメデータ定義
typedef struct{							// アニメ用データ（ROMから読み込みます)
	char name[TEXTURE_NAME_LENGTH];		// テクスチャファイル名
	u8	 AnmTbl[TEXTURE_ANIME_MAX][2];	// アニメ用テーブル[0]:パターン番号  [1]:wait
}FIELD_ANIME;


// 転送アニメ管理ワーク
typedef struct {
	u8  			*texadr;			// 既に転送されているテクスチャアドレス
	int 			texsize;			// 転送するテクスチャサイズ
	NNSG3dResTex 	*animetex;			// 転送するテクスチャリソースのポインタ
	void			*texfile;			// テクスチャファイルの実体
	FIELD_ANIME		*animetbl;			// アニメーション定義テーブルのポインタ
	u16				point,wait;			// 現在の表示ポインタ,ウェイト
}FIELD_ANIME_WORK;

// 読み込んだアニメデータをcastするための構造体定義
typedef struct{
	u32 num;
	FIELD_ANIME anime_sheet[0];			// numの指定で後ろにデータが続く
}FIELD_ANIME_SHEET;


// フィールド転送アニメ用コントロールワーク
struct _FIELD_ANIME_CONTROL_WORK{
	void 			 	*anime_bindata;	// ROMから読み込むアニメ指定データ
	FIELD_ANIME_SHEET   *fa_sheet;
	FIELD_ANIME_WORK faw[FIELD_ANIME_MAX];	// アニメ用ワーク
};

typedef struct _FIELD_ANIME_CONTROL_WORK FIELD_ANIME_CONTROL_WORK;

//==============================================================================
// ワーク
//==============================================================================


//==============================================================================
// 関数
//==============================================================================

static int  AnimeSet(FIELD_ANIME_PTR faw, NNSG3dResTex *tex, int AnimeNo, ARCHANDLE *handle);


//==============================================================================
/**
 * 初期化
 *
 * @param   none		
 *
 * @retval  FIELD_ANIME_PTR		アニメ管理ポインタ	
 */
//==============================================================================
FIELD_ANIME_PTR InitFieldAnime(void)
{
	int i;
	FIELD_ANIME_PTR facw=NULL;
	
	facw = sys_AllocMemory(HEAPID_FIELD,sizeof(FIELD_ANIME_CONTROL_WORK));


	GF_ASSERT(facw);

	for(i=0;i<FIELD_ANIME_MAX;i++){
		facw->faw[i].point    = 0;
		facw->faw[i].wait     = 0;
		facw->faw[i].texadr   = NULL;
		facw->faw[i].animetex = NULL;
		facw->faw[i].animetbl = NULL;
		facw->faw[i].texfile  = NULL;
	}
	
	return facw;
}


//114 892
// 34%


//------------------------------------------------------------------
/**
 * フィールド転送アニメ登録
 *
 * @param   tex			既にVRAMに転送済みのテクスチャ管理ポインタ
 * @param   Tindex		上のテクスチャの指定index
 * @param   AnimeNo		転送を行いたいテクスチャアニメ番号（これがファイル名になる)
 *
 * @retval  int 		登録できたら0-10 できなかったら-1
 */
//------------------------------------------------------------------
int FieldAnimeSets(FIELD_ANIME_PTR facw, NNSG3dResTex *tex)
{
	int i,result=0;
	ARCHANDLE *handle;

//	OS_Printf("field trans_ anime登録開始");

	// ファイルハンドルオープン
	handle = ArchiveDataHandleOpen( ARC_FIELD_TRANS_ANIME, HEAPID_FIELD );

	// アニメテーブル読み込み
	facw->anime_bindata = ArchiveDataLoadAllocByHandle( handle, NARC_fldtanime_tanime_bin, HEAPID_FIELD );
	facw->fa_sheet      = (FIELD_ANIME_SHEET*)facw->anime_bindata;
	
//	OS_Printf("---------------------sheet data anime num = %d\n",facw->fa_sheet->num);
//	for(i=0;i<facw->fa_sheet->num;i++){
//		OS_Printf("------Anime No.%d: %s\n", i,facw->fa_sheet->anime_sheet[i].name);
//	}
	

	
	//ファイルの数だけテクスチャ検索＆登録作業を行う
	for (i=0;i<facw->fa_sheet->num;i++){
		if(AnimeSet(facw, tex, i, handle)>=0){
			result++;
		}
	}

	//ファイルハンドルクローズ
	ArchiveDataHandleClose( handle );

//	OS_Printf("field anime num = %d\n",result);
	
	return result;
}


//------------------------------------------------------------------
/**
 * フィールド転送アニメ登録
 *
 * @param   tex			既にVRAMに転送済みのテクスチャ管理ポインタ
 * @param   Tindex		上のテクスチャの指定index
 * @param   AnimeNo		転送を行いたいテクスチャアニメ番号（これがファイル名になる)
 *
 * @retval  int 		登録できたら0-10 できなかったら-1
 */
//------------------------------------------------------------------
static int AnimeSet(FIELD_ANIME_PTR facw, NNSG3dResTex *tex, int AnimeNo, ARCHANDLE *handle)
{
	int  no,i;
	char filename[40];
	
	//確保されてないのに呼ばれてるかもしれないので
	if(facw==NULL){								
		return -1;
	}

	//使用していないワークを検索
	for(no=0;no<FIELD_ANIME_MAX;no++){				
		if(facw->faw[no].texfile==NULL){
			break;
		}
	}
	//登録MAX もしくは地形テクスチャがNULLだったら登録中止
	if(no==FIELD_ANIME_MAX || tex==NULL){			
		return -1;
	}

	//転送済み地形テクスチャから転送対象となる場所を取得する
	facw->faw[no].texadr   = GetTexStartVRAMAdrByName(tex,facw->fa_sheet->anime_sheet[AnimeNo].name);	
	if (facw->faw[no].texadr == NULL){
		//テクスチャがなかったら登録中止
		return -1;			
	}
	
	#ifdef DEBUG_ANIME_NAME_PRINT
	{
		OS_Printf( "FieldTransAnime [%s] Start\n",
			facw->fa_sheet->anime_sheet[AnimeNo].name );
	}
	#endif
	
	facw->faw[no].texsize  = GetTexByteSizeByName(tex,facw->fa_sheet->anime_sheet[AnimeNo].name);

	// アニメーションデータ読み込み
	sprintf(filename,"data/fld_anime%d.bin",AnimeNo);				
	facw->faw[no].animetbl = &(facw->fa_sheet->anime_sheet[AnimeNo]);	
	
//	OS_Printf("field_anime animetbl=%08x\n",facw->faw[no].animetbl);
	
	//転送アニメテクスチャ読み込み
	facw->faw[no].texfile = ArchiveDataLoadAllocByHandle( handle, AnimeNo+1, HEAPID_FIELD );

	facw->faw[no].animetex  = NNS_G3dGetTex((NNSG3dResFileHeader*) facw->faw[no].texfile );

	return no;
}



//==============================================================================
/**
 * フィールド用テクスチャ転送アニメ常駐関数
 *
 * @param   faw		アニメ管理ポインタ		
 *
 * @retval  none		
 */
//==============================================================================
void FieldAnimeMain(FIELD_ANIME_PTR facw)
{
	int i;
	
	if(facw==NULL){								//確保されてないのに呼ばれてるかもしれないので
		return;
	}
	
	for(i=0;i<FIELD_ANIME_MAX;i++){				//使用していないワークを検索
		FIELD_ANIME_WORK *faw = &facw->faw[i];

		if(faw->texfile==NULL){
			continue;							//何もセットされていない場合はスルー
		}

		//>>add
		if (faw->animetbl == NULL){
			continue;
		}
		//<<
		
		if(faw->animetbl->AnmTbl[faw->point][1] <= faw->wait){	//アニメ更新タイミングの時
			faw->wait = 0;						//ウェイトクリア
			faw->point++;						//ポイント＋＋
			if(faw->animetbl->AnmTbl[faw->point][0]==TEX_ANIME_END_CODE){	//アニメ定義が端まできている場合はループ
				faw->point = 0;
			}
			
			#ifdef DEBUG_ANIME_NAME_PRINT
			{
				OS_Printf( "FieldTransAnime [%s] PatternChange %d\n",
					faw->animetbl->name, faw->point );
			}
			#endif
			
			AddVramTransferManager(
					NNS_GFD_DST_3D_TEX_VRAM,		//転送セット
					(u32)faw->texadr,
					GetTexStartAdr(faw->animetex,faw->animetbl->AnmTbl[faw->point][0]),
					faw->texsize);	
//			OS_Printf("i=%d, point=%d pat=%04x\n",i,faw[i].point,faw[i].animetbl->AnmTbl[faw[i].point][0]);
		}else{
			faw->wait++;			//ウェイト更新
		}
	}
}



//==============================================================================
/**
 * 個別にテクスチャ転送アニメを解放
 *
 * @param   faw		アニメ管理ポインタ
 * @param   no		faw[]の添字
 *
 * @retval  none	
 */
//==============================================================================
void FieldAnimeRelease(FIELD_ANIME_PTR facw, int no )
{
	// テクスチャファイルの解放はキャッシュに積まれているので行わない
	

	//確保されてなかったら終了
	if(facw==NULL){
		return;
	}

	// テクスチャファイル解放
	if(facw->faw[no].texfile){
		sys_FreeMemoryEz(facw->faw[no].texfile);
	}

	// アニメーション定義テーブル指定を解除
	if(facw->faw[no].animetbl!=NULL){
		facw->faw[no].animetbl = NULL;
	}
	facw->faw[no].point = 0;	//アニメ用ワークの初期化
	facw->faw[no].wait  = 0;
}

//==============================================================================
/**
 * テクスチャアニメ転送システム終了
 *
 * @param   faw		アニメ管理ポインタ
 *
 * @retval  none
 */
//==============================================================================
void FieldAnimeAllRelease(FIELD_ANIME_PTR facw)
{
	int i;
	
	//確保されてないのに呼ばれてるかもしれないので
	if(facw==NULL){								
		return;
	}	
	// アニメ登録テーブルの解放
	for(i=0;i<FIELD_ANIME_MAX;i++){
		FieldAnimeRelease( facw, i );
	}
	
	// アニメーションデータ解放
	sys_FreeMemoryEz(facw->anime_bindata);
	
}

//==============================================================================
/**
 * フィールド転送アニメシステム解放
 *
 * @param   facw		アニメ管理ポインタ
 *
 * @retval  none		
 */
//==============================================================================
void ReleaseFieldAnime(FIELD_ANIME_PTR facw)
{
	if(facw!=NULL){
		sys_FreeMemoryEz(facw);
		facw = NULL;
	}
}



