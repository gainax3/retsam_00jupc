//==============================================================================
/**
 * @file	quaternion.h
 * @brief	クォータニオン
 * @author	goto
 * @date	2007.10.09(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

// -----------------------------------------
//
//	クォータニオン
//
// -----------------------------------------
typedef union {
	
	struct {		
		f32	wr;		///< 実数
		f32	xi;		///< 虚数
		f32	yi;		///< 虚数
		f32	zi;		///< 虚数
	};
	f32 q[ 4 ];
	
} QUATERNION;


// -----------------------------------------
//
//	クォータニオンマトリックス ( 4x4 )
//
// -----------------------------------------
typedef union {
	
	struct {
		f32 _00, _01, _02, _03;
        f32 _10, _11, _12, _13;
        f32 _20, _21, _22, _23;
        f32 _30, _31, _32, _33;
	};
	f32 m[ 4 ][ 4 ];
	f32 a[ 16 ];
	
} QUATERNION_MTX44;




// クォータニオンの初期化
extern void Quaternion_Identity( QUATERNION* q );

// クォータニオンのコピー
extern void Quaternion_Copy( QUATERNION* q1, QUATERNION* q2 );

// クォータニオンの積
extern void Quaternion_Mul( QUATERNION* q, const QUATERNION* q1, const QUATERNION* q2 );

// クォータニオンの加算
extern void Quaternion_Add( QUATERNION* q, const QUATERNION* q1, const QUATERNION* q2 );

// クォータニオンの減算
extern void Quaternion_Sub( QUATERNION* q, const QUATERNION* q1, const QUATERNION* q2 );

// クォータニオンと実数の除算
extern void Quaternion_DivReal( QUATERNION *ans, const QUATERNION *qt, f32 s );

// クォータニオンのノルム || a || ^ 2
extern f32  Quaternion_GetNormSqrt( const QUATERNION* q );

// クォータニオンのノルム || a ||
extern f32  Quaternion_GetNorm( const QUATERNION* q );

// クォータニオンをマトリックスに設定
extern void Quaternion_SetMtx44( QUATERNION_MTX44* qmtx, const QUATERNION* q );

// クォータニオンマトリックスをfx32型4x4マトリックスに変換
extern void Quaternion_SetMtx44_to_MtxFx44( const QUATERNION_MTX44* qmtx, MtxFx44* mtx );

// クォータニオンマトリックスをfx32型4x3マトリックスに変換
extern void Quaternion_SetMtx44_to_MtxFx43( const QUATERNION_MTX44* qmtx, MtxFx43* mtx );

// クォータニオンの保持している回転量
extern u16  Quaternion_GetRotation( const QUATERNION* q );

#endif
