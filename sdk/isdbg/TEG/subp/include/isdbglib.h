////////////////////////////////////////////////////////////////////////
//
// isdbglib.h : サポートライブラリ
//
// Copyright (C) 2003-2004 INTELLIGENT SYSTEMS Co.,Ltd. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef     ISDEBUGGER_LIBRARY_H
#define     ISDEBUGGER_LIBRARY_H

#ifdef  __cplusplus
extern "C" {
#endif 


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// プリントデバッグ処理
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///
/// 関数説明
/// 
///
/// [文字列出力]
/// void ISDPrint(const char* pBuf);
///      pBuf:  表示用キューバッファに文字列を追加します。
///
/// [フォーマット付き文字列出力]
/// void ISDPrintf(const char *pBuf, ...)
///      pBuf:  表示用キューバッファにフォーマットした文字列を追加します。
///             使用方法は ANSIのprintf()関数と同様です。
///
/// [プログラム正当性チェックマクロ]
/// ISDASSERT( 評価式 );
///      評価式 には、C言語の評価式を記述します。評価式 の値が真(ZERO以
///      外)ならば次のプログラムを実行します。偽(ZERO)ならば、
///      ISDASSERT()が定義されているソースファイル名とそのラインナンバー、
///      評価式 を表示して、プログラムは停止します。
///
/// [プログラム正当性チェックマクロ]
/// ISDWARNING( 評価式 );
///      ISDASSERT()と同じ使用方法です。ISDASSERT()との違う点は、ASSERT
///      は、プログラムを停止し以降の処理は行わない（ハングアップ状態）
///      ことに対し、ISDWARNNING()は引き続きプログラムを実行する点です。
///
/////////////////////////////////////////////////////////////////////////
/// 使用上の注意
///
/// ・IS-CGB-EMULATOR ハードウェアの都合上、ホスト側にデータを送る間には
///   一時的に割込みを禁止、メモリウェイトが変更されます。
/// ・出荷時のROMなどのリリースバージョンを作成する際には、 NDEBUG を定義する
///   ことでプリントデバッグ関数・マクロは必ず取り除いてください。
///
/////////////////////////////////////////////////////////////////////////

#ifdef NDEBUG
    #define ISDPrint(pBuf)
    #define ISDPrintf(arg...)
    #define ISDAssert(file,line,info)
#else
    void ISDPrint(const char* pBuf);
    void ISDPrintf(const char *pBuf, ...);
    void ISDAssert(const char* pFile, int nLine, const char* pExpression, int nStopProgram);
#endif

/////////////////////////////////////////////////////////////////////////
///

#undef ISDASSERT
#ifdef NDEBUG
#define ISDASSERT(exp)
#else
#define ISDASSERT(exp)  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 1);
#endif

#undef ISDWARNING
#ifdef NDEBUG
#define ISDWARNING(exp)
#else
#define ISDWARNING(exp)  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 0);
#endif


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// NITROToolAPI - ツール製作用API
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/// ストリームAPI データ送(受)信サイズの最大バイト数
///#define      NITROSTM_SENDMAX            8192

/// MIDI-API データ受信サイズの最大バイト数
///#define      NITROMID_RECVMAX            2048

enum {
    NITRODEVID_NULL,
    NITRODEVID_CGBEMULATOR,             // IS-CGB-EMULATOR
    NITRODEVID_NITEMULATOR,             // NITRO-EMULATOR
    NITRODEVID_NITUIC,                  // IS-NITRO-UIC CARTRIDGE
    NITRODEVID_AGBMIDI,                 // IS-AGB-MIDI CARTRIDGE
    NITRODEVID_MAX
};

enum {
    NITRORESID_POLL       = 0,
    NITRORESID_TIMER      = 1,
    NITRORESID_VBLANK     = 2,
    NITRORESID_CARTRIDGE  = 3,
    NITRORESID_MAX
};

#define NITROMASK_RESOURCE_POLL         (1 << NITRORESID_POLL     )
#define NITROMASK_RESOURCE_TIMER        (1 << NITRORESID_TIMER    )
#define NITROMASK_RESOURCE_VBLANK       (1 << NITRORESID_VBLANK   )
#define NITROMASK_RESOURCE_CARTRIDGE    (1 << NITRORESID_CARTRIDGE)

typedef  struct tagNITRODEVCAPS {
    u32         m_nSizeStruct;         // CAPS構造体のサイズ
    u32         m_nDeviceID;           // デバイス識別ID
    u32         m_dwMaskResource;      // このデバイスを動作させるのに必要なりソース
} NITRODEVCAPS;
typedef void (*NITROSTREAMCALLBACKFUNC)( u32 dwUserData, u32 nChnStream, const void *pRecv, u32 dwRecvSize );

////////////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////////////
void         NITROToolAPIInit(void);

////////////////////////////////////////////////////////////////////////
// 接続されているデバイス能力の取得
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIGetMaxCaps(void);
const NITRODEVCAPS  *NITROToolAPIGetDeviceCaps( int nCaps );


////////////////////////////////////////////////////////////////////////
// デバイスのオープン、クローズ
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIOpen( const NITRODEVCAPS* pCaps );
int          NITROToolAPIClose( void );

////////////////////////////////////////////////////////////////////////
// ストリームでの送受信
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIStreamGetWritableLength( u32 *pnLength );
int          NITROToolAPIWriteStream( u32 nChnStream, const void *pSrc, u32 dwSize );
int          NITROToolAPISetReceiveStreamCallBackFunction( NITROSTREAMCALLBACKFUNC cbFunc, u32 dwUserData );

////////////////////////////////////////////////////////////////////////
// VBlank割り込みやタイマーなど、定期的な割り込みの中で継続して呼ぶ
////////////////////////////////////////////////////////////////////////
void         NITROToolAPIPollingIdle(void);             // EMULATOR用
void         NITROToolAPITimerInterrupt(void);          // タイマー割り込み
void         NITROToolAPIVBlankInterrupt(void);         // Ｖブランク割り込み
void         NITROToolAPICartridgeInterrupt(void);      // カードリッジ割り込み


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// NITROMIDIAPI - MIDIデータ読み取りAPI
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

typedef  struct tagNITROMIDICAPS {
    u32         m_nSizeStruct;          // CAPS構造体のサイズ
    u32         m_nDeviceID;            // デバイス識別ID (TOOLAPIと共通)
    u32         m_nMaxFIFO;             // 受信バッファサイズ
    u32         m_dwMaskResource;       // このデバイスを動作させるのに必要なりソース(TOOLAPIと共通)
} NITROMIDICAPS;

////////////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPIInit(void);

////////////////////////////////////////////////////////////////////////
// MIDIデバイス能力の取得
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIGetMaxCaps(void);
const NITROMIDICAPS  *NITROMIDIAPIGetDeviceCaps( int nCaps );

////////////////////////////////////////////////////////////////////////
// MIDIデバイスのオープン、クローズ
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIOpen( const NITROMIDICAPS* pCaps );
int         NITROMIDIAPIClose( void );

////////////////////////////////////////////////////////////////////////
// バッファのリセット
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPIReset(void);

////////////////////////////////////////////////////////////////////////
// バッファリング中のデータ長を取得します
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIGetReadableLength(void);

////////////////////////////////////////////////////////////////////////
// バッファリングされたデータを読みとります
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIRead( void *pBuf, u32 nBufSize );

////////////////////////////////////////////////////////////////////////
// カートリッジ割り込み内で呼んでください
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPICartridgeInterrupt(void);

#ifdef  __cplusplus
}
#endif 
#endif      /* ISDEBUGGER_LIBRARY_H */

