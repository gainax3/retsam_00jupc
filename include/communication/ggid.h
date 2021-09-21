#ifndef __GGID_H__
#define __GGID_H__

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/10
// src/communication/wh.h, src/fushigi/beacondownload.h のGGID定義を分離
// # 名前が _ で始まるままなのは気になるけれど……
// ----------------------------------------------------------------------------

// 使用する GGID
#define _DP_GGID       (0x333)   // メインのGGID

#define _MYSTERY_GGID  (0x333)

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/10/31
// 海外版ポケモンレンジャーと通信できるようにGGIDを変更（5言語共通）
// localize_spec_mark(LANG_ALL) imatake 2006/12/14
// LANG_JAPAN が LANG_JAPANESE になっていたのを修正……
#if (PM_LANG == LANG_JAPAN)
#define	_RANGER_GGID   (0x00000178)
#else
#define	_RANGER_GGID   (0x00400131)
#endif
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/10/31
// 海外版PBRと通信できるようにGGIDを変更
// 日英以外は未定だが、それ以外でも英語版のGGIDをとりあえず返す
// localize_spec_mark(LANG_ALL) imatake 2006/12/14
// LANG_JAPAN が LANG_JAPANESE になっていたのを修正……
#if (PM_LANG == LANG_JAPAN)
#define	_WII_GGID      (0x346)   //
#else
#define	_WII_GGID      (0x00400286)
#endif
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/10
// ふしぎなおくりものビーコンのGGIDを各国語対応
#if (PM_LANG == LANG_JAPAN)
#define _BCON_DOWNLOAD_GGID (0x345)
#elif (PM_LANG == LANG_ENGLISH)
#define _BCON_DOWNLOAD_GGID (0x00400318)
#elif (PM_LANG == LANG_FRANCE)
#define _BCON_DOWNLOAD_GGID (0x008000CD)
#elif (PM_LANG == LANG_ITALY)
#define _BCON_DOWNLOAD_GGID (0x008000CF)
#elif (PM_LANG == LANG_GERMANY)
#define _BCON_DOWNLOAD_GGID (0x008000CE)
#elif (PM_LANG == LANG_SPAIN)
#define _BCON_DOWNLOAD_GGID (0x008000D0)
#endif
// ----------------------------------------------------------------------------

#endif
