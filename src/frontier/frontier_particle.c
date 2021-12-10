//==============================================================================
/**
 * @file	frontier_particle.c
 * @brief	フロンティア 2Dマップ用パーティクル
 * @author	matsuda
 * @date	2007.06.06(水)
 */
//==============================================================================
#include "common.h"
#include "system/particle.h"
#include "system/arc_tool.h"

#include "frontier_particle.h"
#include "fs_usescript.h"


//==============================================================================
//	定数定義
//==============================================================================
///戦闘パーティクルのカメラニア設定
#define BP_NEAR			(FX32_ONE)
///戦闘パーティクルのカメラファー設定
#define BP_FAR			(FX32_ONE * 900)


//==============================================================================
//	構造体定義
//==============================================================================
///フロンティアパーティクルシステム構造体
typedef struct _FRP_WORK{
	PTC_PTR ptc[SPAWORK_MAX];
	u16 heap_id;
}FRP_WORK;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static u32 sAllocTex(u32 size, BOOL is4x4comp);
static u32 sAllocTexPalette(u32 size, BOOL is4pltt);
static void Local_ParticleExit(PTC_PTR ptc);



//--------------------------------------------------------------
/**
 * @brief   パーティクルシステム初期化
 * @param   heap_id		ヒープID
 */
//--------------------------------------------------------------
FRP_PTR FRParticle_Init(int heap_id)
{
	FRP_WORK *frp;
	
	frp = sys_AllocMemory(heap_id, sizeof(FRP_WORK));
	MI_CpuClear8(frp, sizeof(FRP_WORK));
	
	frp->heap_id = heap_id;
	
	//パーティクルシステム初期化
	Particle_SystemWorkInit();
	
	return frp;
}

//--------------------------------------------------------------
/**
 * @brief   パーティクルシステム終了
 * @param   frp		フロンティアパーティクルシステムワークへのポインタ
 */
//--------------------------------------------------------------
void FRParticle_Exit(FRP_PTR frp)
{
	int i;
	void *heap;
	
	for(i = 0; i < SPAWORK_MAX; i++){
		if(frp->ptc[i] != NULL){
			Local_ParticleExit(frp->ptc[i]);
		}
	}
	
	sys_FreeMemoryEz(frp);
}


//--------------------------------------------------------------
/**
 * @brief   パーティクルシステムを作成し、リソースの読み込み＆登録
 *
 * @param   frp			フロンティアパーティクルシステムワークへのポインタ
 * @param   work_id		SPAWORK_???
 * @param   spa_no		SPAインデックス
 *
 * @retval  生成されたパーティクルシステムのポインタ
 */
//--------------------------------------------------------------
PTC_PTR FRParticle_SystemCreate(FRP_PTR frp, int work_id, int spa_no, int camera_type)
{
	PTC_PTR ptc;
	void *heap;
	GF_CAMERA_PTR camera_ptr;
	void *resource;
	
	GF_ASSERT(frp->ptc[work_id] == NULL);
	
	//パーティクルシステム作成
	heap = sys_AllocMemory(frp->heap_id, PARTICLE_LIB_HEAP_SIZE);
	ptc = Particle_SystemCreate(sAllocTex, sAllocTexPalette, heap, 
		PARTICLE_LIB_HEAP_SIZE, TRUE, frp->heap_id);
	camera_ptr = Particle_GetCameraPtr(ptc);
	GFC_SetCameraClip(BP_NEAR, BP_FAR, camera_ptr);
	Particle_CameraTypeSet( ptc ,camera_type );

	//リソース読み込み＆登録
	resource = Particle_ArcResourceLoad(ARC_FRONTIER_PARTICLE, spa_no, frp->heap_id);
	Particle_ResourceSet(ptc, resource, PTC_AUTOTEX_LNK | PTC_AUTOPLTT_LNK, TRUE);
	
	//管理領域に登録
	frp->ptc[work_id] = ptc;
	
	return ptc;
}

//--------------------------------------------------------------
/**
 * @brief   パーティクルシステムを終了し、管理領域からも削除する
 *
 * @param   frp		フロンティアパーティクルシステムワークへのポインタ
 * @param   work_id	SPAWORK_???
 * @param   ptc		パーティクルシステムワークへのポインタ
 */
//--------------------------------------------------------------
void FRParticle_SystemExit(FRP_PTR frp, int work_id)
{
#if 1
	GF_ASSERT(frp->ptc[work_id] != NULL);
	
	Local_ParticleExit(frp->ptc[work_id]);
	frp->ptc[work_id] = NULL;
#else
	int i;
	
	Local_ParticleExit(ptc);
	
	for(i = 0; i < SPAWORK_MAX; i++){
		if(frp->ptc[i] == ptc){
			frp->ptc[i] = NULL;
			return;
		}
	}
	GF_ASSERT(0);	//管理領域に登録されていないPTCが渡されている
#endif
}

//--------------------------------------------------------------
/**
 * @brief   フロンティア用パーティクルシステム・メイン関数(計算・描画処理などを実行)
 *
 * @param   ptc		パーティクルシステムワークへのポインタ
 * @retval  FALSE:1つも動いているパーティクルがない
 */
//--------------------------------------------------------------
int FRParticle_Main(void)
{
	int draw_num;
	
	GF_G3X_Reset();


	if(Particle_GetActionNum() == 0){
		return FALSE;
	}
	

	draw_num = Particle_DrawAll();	//パーティクル描画

	if(draw_num > 0){
		//パーティクルの描画が終了したので、再びソフトウェアスプライト用カメラに設定
		GF_G3X_Reset();
	}

	Particle_CalcAll();	//パーティクル計算

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   PTCポインタを取得する
 *
 * @param   frp			フロンティアパーティクルシステムワークへのポインタ
 * @param   work_id		SPAWORK_???
 *
 * @retval  PTCポインタ
 */
//--------------------------------------------------------------
PTC_PTR FRParticle_PTCPtrGet(FRP_PTR frp, int work_id)
{
	GF_ASSERT(frp->ptc[work_id] != NULL);
	return frp->ptc[work_id];
}

//--------------------------------------------------------------
/**
 * @brief   生成されているパーティクルエミッタ全ての終了待ち
 *
 * @param   frp		フロンティアパーティクルシステムワークへのポインタ
 *
 * @retval  TRUE:全て終了している。　FALSE:1つ以上のエミッタが動作している
 */
//--------------------------------------------------------------
BOOL FRParticle_EmitNumGet(FRP_PTR frp)
{
	int i;
	
	for(i = 0; i < SPAWORK_MAX; i++){
		if(frp->ptc[i] != NULL){
			if(Particle_GetEmitterNum(frp->ptc[i]) > 0){
				return FALSE;
			}
		}
	}
	return TRUE;	//動いているエミッタはひとつもない
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   パーティクルシステムを終了させる
 * @param   ptc		パーティクルシステムワークへのポインタ
 */
//--------------------------------------------------------------
static void Local_ParticleExit(PTC_PTR ptc)
{
	void *heap;

	heap = Particle_HeapPtrGet(ptc);
	Particle_SystemExit(ptc);
	sys_FreeMemoryEz(heap);
}

//--------------------------------------------------------------
/**
 * @brief   テクスチャVRAMアドレスを返すためのコールバック関数
 *
 * @param   size		テクスチャサイズ
 * @param   is4x4comp	4x4圧縮テクスチャであるかどうかのフラグ(TRUE=圧縮テクスチャ)
 *
 * @retval  読み込みを開始するVRAMのアドレス
 */
//--------------------------------------------------------------
static u32 sAllocTex(u32 size, BOOL is4x4comp)
{
	u32 address;
	NNSGfdTexKey key;
	
	key = NNS_GfdAllocTexVram(size, is4x4comp, 0);
	GF_ASSERT(key != NNS_GFD_ALLOC_ERROR_TEXKEY);
	Particle_LnkTexKeySet(key);		//リンクドリストを使用しているのでキー情報をセット
	
	address = NNS_GfdGetTexKeyAddr(key);
	OS_TPrintf("vram=%d\n", address);
	return address;
}

//--------------------------------------------------------------
/**
 * @brief	テクスチャパレットVRAMアドレスを返すためのコールバック関数
 *
 * @param	size		テクスチャサイズ
 * @param	is4pltt		4色パレットであるかどうかのフラグ
 *
 * @retval	読み込みを開始するVRAMのアドレス
 *
 * direct形式のテクスチャの場合、SPL_LoadTexPlttByCallbackFunctionは
 * コールバック関数を呼び出しません。
 */
//--------------------------------------------------------------
static u32 sAllocTexPalette(u32 size, BOOL is4pltt)
{
	NNSGfdPlttKey key;
	u32 address;
	
	key = NNS_GfdAllocPlttVram(size, is4pltt, NNS_GFD_ALLOC_FROM_LOW);
	if(key == NNS_GFD_ALLOC_ERROR_PLTTKEY){
		GF_ASSERT(0 && "パーティクルでパレットの確保が出来ません！\n");
	}
	Particle_PlttLnkTexKeySet(key);	//リンクドリストを使用しているのでキー情報をセット
	address = NNS_GfdGetPlttKeyAddr(key);
	OS_TPrintf("vram=%d\n", address);
	return address;
}
