//===================================================================
/**
 * @file	agbdownload.h
 * @bfief	AGBカセットからのふしぎデータダウンロード
 * @author	Satoshi Mitsuhara
 * @date	06.06.09
 *
 * $Id: agbdownload.h,v 1.1 2006/06/08 17:59:07 mitsuhara Exp $
 */
//===================================================================
#ifndef __AGBDOWNLOAD_H__
#define __AGBDOWNLOAD_H__


// データサイズが置かれているアドレス
#define AGBMISSIONDATASIZE	0x08100000
// ビーコン情報が配置されているアドレス
#define AGBBEACONDATAPTR	0x08100010
// ふしぎデータが配置されているアドレス
#define AGBMISSIONDATAPTR	0x08100100
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/15
// 電子署名認証に対応
#define AGB_SIGNEDDATA_PTR		((void *)AGBMISSIONDATASIZE)	// 認証対象領域の先頭
#define AGB_SIGNEDDATA_SIZE		(0x4A8)							// 認証対象領域のサイズ
#define AGB_SIGNATURE_PTR		((void *)0x08020000)			// 電子署名が配置されているアドレス
#define AGB_SIGNATURE_SIZE		(128)
// ----------------------------------------------------------------------------

#define AGB_MAKER_CODE		0x3130		// 任天堂

extern int GetAgbCartridgeDataSize(void);
extern BOOL GetAgbCartridgeBeaconData(void *dist, int size);
extern BOOL ReadAgbCartridgeData(void *dist, int size);

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/15
// Crypto ライブラリ内でメモリを確保するヒープを指定するための関数
extern void SetAgbCartridgeHeapID(u32 heapID);
// ----------------------------------------------------------------------------


#endif	// __AGBDOWNLOAD_H__
/*  */

