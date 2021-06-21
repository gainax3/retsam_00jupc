/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - account
  File:     dwc_account.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.22 $

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <bm/util_wifiidtool.h>
#include <base/dwc_account.h>

#define DWCi_CHECK_FLAG(flag,mask)      ((flag&mask)==mask)

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetMaskBits

  Description:  u32のデータの一部のビット列を取得します。

  Arguments:    data  : 取り出したいデータの格納されたu32データ
                shift : ビット列の右にシフトする数
                mask  : シフトした後のビット列に適用するビットマスク

  Returns:      抜き出したビット列
 *---------------------------------------------------------------------------*/
static inline u32
DWCi_Acc_GetMaskBits( u32 data,
                      u32 shift,
                      u32 mask )
{
    return ((data >> shift) & mask);
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetMaskBits

  Description:  u32より短いビット列をu32のデータの一部に格納します。

  Arguments:    ptr   : 格納したいu32データのポインタ
                data  : 格納したいビット列
                shift : ビット列の左にシフトする数
                mask  : シフトする前のビット列に適用するビットマスク

  Returns:      ビット列がマスクに収まっていれば、結果を書き込んでTRUE
                収まっていなければ、何もせずにFALSE
 *---------------------------------------------------------------------------*/
static BOOL
DWCi_Acc_SetMaskBits( u32* ptr,
                      u32  data,
                      u32  shift,
                      u32  mask )
{
    // dataがmask内に収まっているかどうかをチェック。
    if ( (data & ~mask) != 0 )
    {
        return FALSE;
    }

    *ptr = (*ptr & ~(mask << shift)) | (data << shift);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetUserId

  Description:  DWCAccLoginId構造体からUserIDを取得します。

  Arguments:    userdata : ユーザデータの格納されたDWCAccLoginId構造体へのポインタ

  Returns:      UserID 64bits
 *---------------------------------------------------------------------------*/
u64
DWCi_Acc_GetUserId( const DWCAccLoginId* userdata )
{
    u32 hi32;

    hi32 = DWCi_Acc_GetMaskBits( userdata->id_data, DWC_ACC_USERID_HI32_SHIFT, DWC_ACC_USERID_HI32_MASK );

    return (u64)hi32 << 32 | userdata->userid_lo32;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetPlayerId

  Description:  DWCAccLoginId構造体からPlayerIDを取得します。

  Arguments:    userdata : ユーザデータの格納されたDWCAccLoginId構造体へのポインタ

  Returns:      PlayerID 32bits
 *---------------------------------------------------------------------------*/
u32
DWCi_Acc_GetPlayerId( const DWCAccLoginId* userdata )
{
    return userdata->playerid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetFriendKey

  Description:  DWCAccFriendKey構造体から友達登録鍵を取得します。

  Arguments:    data : ユーザデータの格納されたDWCAccFriendKey構造体へのポインタ

  Returns:      友達登録鍵 64bits
 *---------------------------------------------------------------------------*/
u64
DWCi_Acc_GetFriendKey( const DWCAccFriendKey* data )
{
    return (u64)data->friendkey_hi32 << 32 | data->friendkey_lo32;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetGsProfileId

  Description:  DWCAccFriendKey構造体からgsProfileIdを取得します。

  Arguments:    data : ユーザデータの格納されたDWCAccGsProfileId構造体へのポインタ

  Returns:      友達登録鍵 64bits
 *---------------------------------------------------------------------------*/
int
DWCi_Acc_GetGsProfileId( const DWCAccGsProfileId* data )
{
    return data->id;
}


/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetUserId

  Description:  DWCAccLoginId構造体にUserIDを格納します。

  Arguments:    userdata : ユーザデータを格納するDWCAccLoginId構造体へのポインタ
                userid   : UserID 64bits

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetUserId( DWCAccLoginId*  userdata,
                    u64             userid      )
{
    if ( !DWCi_Acc_SetMaskBits( &userdata->id_data, (u32)(userid>>32), DWC_ACC_USERID_HI32_SHIFT, DWC_ACC_USERID_HI32_MASK ) )
    {
        // useridが範囲外の値。
    }
    userdata->userid_lo32 = (u32)userid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetPlayerId

  Description:  DWCAccLoginId構造体にPlayerIDを格納します。

  Arguments:    userdata : ユーザデータを格納するDWCAccLoginId構造体へのポインタ
                playerid : PlayerID 64bits

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetPlayerId( DWCAccLoginId*    userdata,
                      u32               playerid        )
{
    userdata->playerid = playerid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetPlayerId

  Description:  DWCAccLoginId構造体にPlayerIDを格納します。

  Arguments:    userdata : ユーザデータを格納するDWCAccLoginId構造体へのポインタ
                playerid : PlayerID 64bits

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetFriendKey( DWCAccFriendKey* data,
                       u64              friendkey )
{
    data->friendkey_lo32 = (u32)friendkey;
    data->friendkey_hi32 = (u32)(friendkey >> 32);
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetGsProfileId

  Description:  DWCAccLoginId構造体にGsProfileIdを格納します。

  Arguments:    userdata  : ユーザデータを格納するDWCAccLoginId構造体へのポインタ
                friendkey : PlayerID 64bits

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetGsProfileId( DWCAccGsProfileId* data, int gs_profile_id )
{
    data->id = gs_profile_id;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetFlags

  Description:  DWCAccFlag構造体のフラグを取得します。

  Arguments:    userdata : ユーザデータを格納するDWCAccLoginId構造体へのポインタ

  Returns:      フラグ（有効12bits）
 *---------------------------------------------------------------------------*/
static u32
DWCi_Acc_GetFlags( const DWCAccFlag* userdata )
{
    return DWCi_Acc_GetMaskBits( userdata->flags, DWC_ACC_FLAGS_SHIFT, DWC_ACC_FLAGS_MASK );
}

// データタイプ
static u32
DWCi_Acc_GetFlag_DataType( const DWCAccFlag* userdata )
{
    return DWCi_Acc_GetFlags( userdata ) & DWC_ACC_FRIENDDATA_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsBuddyFriendData

  Description:  DWCAccFriendData構造体にGSプロファイルIDが格納されているとき、
                友達情報が既にサーバ上で友達関係になっているかどうかを確認する。

  Arguments:    frienddata : フレンドデータを格納するDWCAccFriendData構造体へのポインタ

  Returns:      TRUE  : 既に友達関係になっている
                FALSE : まだなっていない。
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsBuddyFriendData( const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 type;

        type = DWCi_Acc_GetFlags( &frienddata->flags );

        return ( DWCi_CHECK_FLAG( type, DWC_ACC_FRIENDDATA_ISBUDDY ) ) ? TRUE : FALSE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         DWC_

  Description:  DWCAccFriendData構造体にGSプロファイルIDが格納されているとき、
                友達情報が既にサーバ上で友達関係になっているかどうかを確認する。

  Arguments:    frienddata : フレンドデータを格納するDWCAccFriendData構造体へのポインタ

  Returns:      TRUE  : 既に友達関係になっている
                FALSE : まだなっていない。
 *---------------------------------------------------------------------------*/
int
DWC_GetFriendDataType( const DWCAccFriendData* frienddata )
{
    return (int)DWCi_Acc_GetFlag_DataType( &frienddata->flags );
}


/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetFlags

  Description:  DWCAccLoginId構造体にフラグを格納します。

  Arguments:    userdata : ユーザデータを格納するDWCAccLoginId構造体へのポインタ
                flags    : フラグ（有効12bits）

  Returns:      なし
 *---------------------------------------------------------------------------*/
static void
DWCi_Acc_SetFlags( DWCAccFlag* userdata,
                   u32         flags )
{
    DWCi_Acc_SetMaskBits( &userdata->flags, flags, DWC_ACC_FLAGS_SHIFT, DWC_ACC_FLAGS_MASK );
}

// データタイプ
static void
DWCi_Acc_SetFlag_DataType( DWCAccFlag* userdata,
                           int         type )
{
    u32 flags;

    flags = DWCi_Acc_GetFlags( userdata );

    flags = ( flags & ~DWC_ACC_FRIENDDATA_MASK ) | type;

    DWCi_Acc_SetFlags( userdata, flags );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_SetBuddyFriendData

  Description:  DWCAccFriendData構造体にGSプロファイルIDが格納されているとき、
                友達情報が既にサーバ上で友達関係になっていたら、セットする。

  Arguments:    frienddata : フレンドデータを格納するDWCAccFriendData構造体へのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_SetBuddyFriendData( DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 flags;

        flags = DWCi_Acc_GetFlags( &frienddata->flags );

        flags = ( flags &  ~DWC_ACC_FRIENDDATA_ISBUDDY_MASK ) | DWC_ACC_FRIENDDATA_ISBUDDY;

        DWCi_Acc_SetFlags( &frienddata->flags, flags );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_ClearBuddyFlagFriendData

  Description:  DWCAccFriendData構造体にGSプロファイルIDが格納されているときで、
                友達情報にセットされた友達成立フラグをクリアする。

  Arguments:    frienddata : フレンドデータを格納するDWCAccFriendData構造体へのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_ClearBuddyFlagFriendData( DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 flags;

        flags = DWCi_Acc_GetFlags( &frienddata->flags );

        flags &=  ~DWC_ACC_FRIENDDATA_ISBUDDY_MASK;

        DWCi_Acc_SetFlags( &frienddata->flags, flags );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CreateFriendKey

  Description:  GS ProfileIdから、友達登録鍵を生成します。

  Arguments:    userdata : 友達登録鍵の元となるGS Profile IDの格納された構造体へのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
u64
DWC_Acc_CreateFriendKey( int gs_profile_id,
                         u32 gamecode )
{
    MATHCRC8Table table;
    u32 code[2];
    int crc;

    code[0] = (u32)gs_profile_id;
    code[1] = gamecode;

    MATH_CRC8InitTable( &table );
    crc = MATH_CalcCRC8( &table, code, sizeof( code ) ) & 0x7f; // 7bitsのみ有効

    return ( ((u64)crc << 32) | (u32)gs_profile_id );
}


/*---------------------------------------------------------------------------*
  Name:         DWC_CheckFriendKey

  Description:  友達登録鍵が正しいものかどうかを確認します。

  Arguments:    userdata  : ユーザデータ
                friendkey : 友達登録鍵

  Returns:      TRUE  : 正しい
                FALSE : 不正なコード
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckFriendKey( const DWCAccUserData* userdata, u64 friendkey )
{
    return DWC_Acc_CheckFriendKey( friendkey, userdata->gamecode );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CheckFriendKey

  Description:  友達登録鍵が正しいものかどうかを確認します。

  Arguments:    friendkey : 友達登録鍵
                gamecode  : 不正なコードのキーとなる値

  Returns:      TRUE  : 正しい
                FALSE : 不正なコード
 *---------------------------------------------------------------------------*/
BOOL
DWC_Acc_CheckFriendKey( u64 friendkey,
                        u32 gamecode )
{
    MATHCRC8Table table;
    u32 code[2];
    int crc;

    // 負のGSプロファイルIDは不正
    if ( friendkey & 0x80000000 ) return FALSE;

    code[0] = (u32)friendkey;
    code[1] = gamecode;

    MATH_CRC8InitTable( &table );
    crc = MATH_CalcCRC8( &table, code, sizeof( code ) ) & 0x7f; // 7bitsのみ有効

    if ( crc != (friendkey >> 32) )
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_FriendKeyToGsProfileId

  Description:  友達登録鍵からGS ProfileIdを取り出します。

  Arguments:    friendkey : 友達登録鍵
                gamecode  : 不正なコードのキーとなる値

  Returns:      0のときは、取得失敗（不正な友達登録鍵）
                それ以外のときは、GS ProfileId
 *---------------------------------------------------------------------------*/
int
DWC_Acc_FriendKeyToGsProfileId( u64 friendkey,
                                u32 gamecode )
{
    if ( !DWC_Acc_CheckFriendKey( friendkey, gamecode ) )
    {
        return 0;
    }

    return (int)( friendkey & 0xffffffff );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_FriendKeyToString

  Description:  友達登録鍵を数字で表現された文字列に変換します。

  Arguments:    string      文字列の出力先
                friendkey   入力する友達登録鍵（数値表現）

  Returns:      なし
 *---------------------------------------------------------------------------*/
void DWC_FriendKeyToString( char* string, u64 friendKey )
{
    OS_SNPrintf( string, DWC_ACC_FRIENDKEY_STRING_BUFSIZE, "%012llu", friendKey );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_StringToFriendKey

  Description:  数字で表現された文字列を友達登録鍵（数値表現）に変換します。

  Arguments:    string      入力する数字で表現された文字列

  Returns:      友達登録鍵（数値表現）
 *---------------------------------------------------------------------------*/
u64 DWC_StringToFriendKey( const char* string )
{
    int i;
    u64 result = 0;
    u64 dec = 1;

    for ( i = 0 ; i < DWC_ACC_FRIENDKEY_STRING_LENGTH ; ++i )
    {
        char tmp = *(string + DWC_ACC_FRIENDKEY_STRING_LENGTH - 1 - i);
        if(tmp < '0' || tmp > '9')
            return 0;
        
        result +=  (tmp - '0') * dec;

        dec *= 10;
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_U64ToString32

  Description:  32進文字列に変換します。5bitsを1文字で表現します。

  Arguments:    num    : 変換する数値
                bitnum : 変換するビット長
                result : 結果を格納するためのバッファ。(bitnum+4)/5 + 1バイト
                         分の領域が必要です。

  Returns:      なし
 *---------------------------------------------------------------------------*/
static void
DWCi_Acc_U64ToString32( u64   num,
                        int   bitnum,
                        char* result )
{
    int i;
    const int loopnum = (bitnum + 4) / 5;
    const char* const sStringTable = "0123456789abcdefghijklmnopqrstuv";
    //                                          0123456789012345678901

    for ( i = 0 ; i < loopnum ; ++i )
    {
        *(result + loopnum - 1 - i) = *(sStringTable + (num & 0x1f));

        num >>= 5;
    }

    *( result + loopnum ) = 0; // NULL終端を入れる。
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_LoginIdToUserName

  Description:  DWCAccLoginId構造体のログインIDを文字列表現に変換します。
                ユーザID9桁＋ゲームコード＋プレイヤーID7桁になります。
                NULL終端を含めて21バイトの出力領域が必要です。

  Arguments:    loginid  : ユーザデータの格納されたDWCAccLoginId構造体へのポインタ
                gamecode : ゲームコード
                username : 結果を格納するためのバッファ。21バイト必要です。

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_LoginIdToUserName( const DWCAccLoginId* loginid,
                            u32                  gamecode,
                            char*                username )
{
    char userid[DWC_ACC_USERNAME_STRING_BUFSIZE];
    char playerid[DWC_ACC_USERNAME_STRING_BUFSIZE];

    DWCi_Acc_U64ToString32( DWCi_Acc_GetUserId( loginid ), DWC_ACC_USERID_BITS, userid );
    DWCi_Acc_U64ToString32( DWCi_Acc_GetPlayerId( loginid ), DWC_ACC_PLAYERID_BITS, playerid );

    OS_SNPrintf( username, DWC_ACC_USERNAME_STRING_BUFSIZE,
                 "%s%c%c%c%c%s",
                 userid,
                 (u8)(gamecode >> 24), (u8)(gamecode >> 16), (u8)(gamecode >> 8), (u8)gamecode,
                 playerid );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CreateUserData

  Description:  ユーザデータを作成します。

  Arguments:    userdata  : ユーザデータを格納するためのバッファ
                gamecode  : アプリケーションに任天堂が割り当てるゲームコード

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_CreateUserData( DWCAccUserData* userdata, u32 gamecode )
{
    MATHCRC32Table      table;

    MI_CpuClear8( userdata, DWC_ACC_USERDATA_BUFSIZE );

    // ユーザ情報の設定
    userdata->size = DWC_ACC_USERDATA_BUFSIZE;
    userdata->gs_profile_id = 0;
    userdata->gamecode = gamecode;

    // テンポラリのLoginIdを生成する。
    DWCi_Acc_CreateTempLoginId( &userdata->pseudo );

    // Userdataを作成するときは、認証されたIDは生成できないので、入れない。
    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)&userdata->authentic, DWC_ACC_FRIENDDATA_NODATA   );

    // CRCの計算
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    // dirtyフラグを立てる。
    userdata->flag |= DWC_ACC_USERDATA_DIRTY;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_CreateTempLoginId

  Description:  テンポラリログインIDを作成します。

  Arguments:    loginid  : 確認するログインIDが格納されたポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_CreateTempLoginId( DWCAccLoginId* loginid )
{
    DWCAuthWiFiId       authid;
    MATHRandContext32   randcontext;
	u32					buffer[8];
	u32					i;
	
	// [nakata] ランダムコンテキストの初期化をTickベースからLowEntropyDataベースに変更
	OS_GetLowEntropyData(buffer);
	for(i=1; i<8; i++) {
		buffer[i] = buffer[i]^buffer[i-1];
	}
    MATH_InitRand32( &randcontext, buffer[7] );

    // 本体のユーザIDを入れる。
    DWC_Auth_GetId( &authid );
    if ( authid.flg )
    {
        DWCi_Acc_SetUserId( loginid, authid.uId );
    }
    else
    {
        DWCi_Acc_SetUserId( loginid, authid.notAttestedId );
    }
#if 0
    DWCi_Acc_SetPlayerId( loginid, 1 ); // デバッグ用に固定値にする。
#else
    DWCi_Acc_SetPlayerId( loginid, MATH_Rand32( &randcontext, 0 ) );
#endif
    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)loginid,    DWC_ACC_FRIENDDATA_LOGIN_ID );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_CheckConsoleUserId

  Description:  ログインIDのユーザIDとコンソールのユーザIDを比較する

  Arguments:    loginid  : 確認するログインIDが格納されたポインタ

  Returns:      TRUE  : 同じ
                FALSE : 違う
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_CheckConsoleUserId( const DWCAccLoginId* loginid )
{
    DWCAuthWiFiId       authid;
    
    DWC_Auth_GetId( &authid );
    if ( authid.flg )
    {
        return (DWCi_Acc_GetUserId( loginid ) == authid.uId) ? TRUE : FALSE;
    }
    else
    {
        return (DWCi_Acc_GetUserId( loginid ) == authid.notAttestedId) ? TRUE : FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_IsValidLoginId

  Description:  ログインIDが正しいものかどうかを確認します。

  Arguments:    loginid  : 確認するログインIDが格納されたポインタ

  Returns:      TRUE  : loginIDが格納されている
                FALSE : 格納されているものはログインIDではない
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_IsValidLoginId( const DWCAccLoginId* loginid )
{
    return ( DWCi_Acc_GetFlag_DataType( (DWCAccFlag*)loginid ) == DWC_ACC_FRIENDDATA_LOGIN_ID ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_IsAuthentic

  Description:  ユーザデータが認証済みかどうかを確認します。

  Arguments:    userdata  : 確認するユーザデータが格納されたポインタ

  Returns:      TRUE  : 既に認証済み。
                FALSE : まだ認証を受けていない。
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_IsAuthentic( const DWCAccUserData* userdata )
{
    return DWCi_Acc_IsValidLoginId( &userdata->authentic );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsValidFriendData / DWCi_Acc_IsValidFriendData

  Description:  FriendDataに有効なデータがセットされているかどうかを調べます。

  Arguments:    frienddata : FriendData構造体へのポインタ

  Returns:      TRUE  : 有効なデータ
                FALSE : 無効なデータ（データなし）
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsValidFriendData( const DWCAccFriendData* frienddata )
{
    u32 type = DWCi_Acc_GetFlag_DataType( &frienddata->flags );

    if ( type ==  DWC_ACC_FRIENDDATA_NODATA )
    {
        return FALSE;
    }

    return TRUE;
}

// 過去の互換性のため。
BOOL
DWCi_Acc_IsValidFriendData( const DWCAccFriendData* frienddata )
{
    return DWC_IsValidFriendData( frienddata );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateUserData

  Description:  ユーザデータを生成します。このユーザデータをバックアップメモリに
                保存してください。

  Arguments:    userdata  : 生成されたユーザデータを格納するポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_CreateUserData( DWCAccUserData* userdata, u32 gamecode )
{
    DWCi_Acc_CreateUserData( userdata, gamecode );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckUserData

  Description:  ユーザデータを検査します。

  Arguments:    userdata  : 検査するユーザデータが格納されたポインタ

  Returns:      TRUE  : 正常
                FALSE : 破壊されている
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckUserData( const DWCAccUserData* userdata )
{
    u32                 crc32;
    MATHCRC32Table      table;

    MATH_CRC32InitTable( &table );
    crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    return ( crc32 == userdata->crc32 ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckHasProfile

  Description:  ユーザデータが既にプロファイルIDを取得しているかを確認する。

  Arguments:    userdata  : 検査するユーザデータが格納されたポインタ

  Returns:      TRUE  : 既にProfileIdを取得している
                FALSE : ProfileIdはまだ取得されていない
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckHasProfile( const DWCAccUserData* userdata )
{
    return ( DWCi_Acc_IsValidLoginId( &userdata->authentic ) && userdata->gs_profile_id > 0 ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckValidConsole

  Description:  ユーザデータと本体の整合性を確認します。

  Arguments:    userdata  : 検査するユーザデータが格納されたポインタ

  Returns:      TRUE  : 本体に格納されているユーザIDとパスワードは使用可能 
                FALSE : 使用不可
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckValidConsole( const DWCAccUserData* userdata )
{
    DWCAuthWiFiId       authid;

    if ( DWCi_Acc_GetFlag_DataType( (DWCAccFlag*)&userdata->authentic ) == DWC_ACC_FRIENDDATA_NODATA )
    {
        // 認証されたログインIDがなければ、TRUE
        return TRUE;
    }

    DWC_Auth_GetId( &authid );

    if ( !authid.flg )
    {
        // 認証されたログインIDがあるにもかかわらず、本体のユーザIDが認証されていなければ、FALSE
        return FALSE;
    }

    return ( DWCi_Acc_GetUserId( &userdata->authentic ) == authid.uId ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetUserIdToUserData

  Description:  ユーザデータにUserID/PlayerIDなどの情報をセットします。

  Arguments:    userdata  : ユーザデータを格納するためのバッファ
                userid    : ユーザID
                playerid  : プレーヤーID

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetLoginIdToUserData( DWCAccUserData*      userdata,
                               const DWCAccLoginId* loginid,
                               int                  gs_profile_id )
{
    MATHCRC32Table      table;

    // loginIDの設定
    userdata->authentic = *loginid;

    // gs_profile_idの設定
    userdata->gs_profile_id = gs_profile_id;

    // CRCの計算
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    // dirtyフラグを立てる。
    userdata->flag |= DWC_ACC_USERDATA_DIRTY;
}


/*---------------------------------------------------------------------------*
  Name:         DWC_CheckDirtyFlag / DWCi_Acc_IsDirty

  Description:  ユーザデータの変更フラグチェックします。

  Arguments:    userdata  : ユーザデータを格納するためのバッファ

  Returns:      TRUE  : 変更されている。
                FALSE : 変更されていない。
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckDirtyFlag( const DWCAccUserData* userdata )
{
    return DWCi_Acc_IsDirty( userdata );
}

BOOL
DWCi_Acc_IsDirty( const DWCAccUserData* userdata )
{
    return DWCi_CHECK_FLAG( userdata->flag, DWC_ACC_USERDATA_DIRTY ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_ClearDirty

  Description:  ユーザデータの変更フラグをクリアします。

  Arguments:    userdata  : ユーザデータを格納するためのバッファ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_ClearDirtyFlag( DWCAccUserData* userdata )
{
    DWCi_Acc_ClearDirty( userdata );
}

void
DWCi_Acc_ClearDirty( DWCAccUserData* userdata )
{
    MATHCRC32Table      table;

    userdata->flag &= ~DWC_ACC_USERDATA_DIRTY;

    // CRCの計算
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_TestFlagFunc

  Description:  テスト用の内部関数。

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_TestFlagFunc( void )
{
    DWCAccLoginId id;
    u32 flags;

    MI_CpuClear8( &id, sizeof( DWCAccLoginId ) );

    flags = DWCi_Acc_GetFlags( (DWCAccFlag*)&id );
    OS_TPrintf("%08x: %08x\n",flags, DWCi_Acc_GetFlag_DataType((DWCAccFlag*)&id ) );
    if ( DWCi_Acc_IsValidLoginId( &id ) )
    {
        OS_TPrintf("valid\n");
    }
    else
    {
        OS_TPrintf("invalid\n");
    }

    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)&id , DWC_ACC_FRIENDDATA_LOGIN_ID );

    flags = DWCi_Acc_GetFlags( (DWCAccFlag*)&id );
    OS_TPrintf("%08x: %08x\n",flags, DWCi_Acc_GetFlag_DataType((DWCAccFlag*)&id ) );
    if ( DWCi_Acc_IsValidLoginId( &id ) )
    {
        OS_TPrintf("valid\n");
    }
    else
    {
        OS_TPrintf("invalid\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetFriendKey

  Description:  FriendDataから友達登録鍵を取得します。

  Arguments:    frienddata : FriendData構造体へのポインタ

  Returns:      0以外 : 友達登録鍵
                0     : 入力が正しくない
 *---------------------------------------------------------------------------*/
u64
DWC_GetFriendKey( const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        return DWCi_Acc_GetFriendKey( &frienddata->friend_key );
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetGsProfileId

  Description:  FriendDataからgsProfileIdを取得します。

  Arguments:    userdata   : 自分のユーザデータの格納されているポインタ
                frienddata : FriendData構造体へのポインタ

  Returns:      0以外 : gsProfileId
                0     : 入力が正しくない
                -1    : 仮ID -> LoginID文字列データを取得して、検索してください。
 *---------------------------------------------------------------------------*/
int
DWC_GetGsProfileId( const DWCAccUserData* userdata,
                    const DWCAccFriendData* frienddata )
{
    u32 type = DWCi_Acc_GetFlag_DataType( &frienddata->flags );

    switch ( type )
    {
    case DWC_ACC_FRIENDDATA_FRIEND_KEY:
        {
            u64 friend_key = DWCi_Acc_GetFriendKey( &frienddata->friend_key );

            if ( DWC_Acc_CheckFriendKey( friend_key, userdata->gamecode ) )
            {
                return DWC_Acc_FriendKeyToGsProfileId( friend_key, userdata->gamecode );
            }

            return 0;
        }
        // NOT REACH HERE
        break;

    case DWC_ACC_FRIENDDATA_GS_PROFILE_ID:
        return DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id );
        // NOT REACH HERE
        break;
    case DWC_ACC_FRIENDDATA_LOGIN_ID:
        return -1;
        // NOT REACH HERE
        break;
    default:
        // NOT REACH HERE
        break;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateFriendKey

  Description:  ユーザデータから、相手に渡す友達登録鍵を生成します。
                友達登録鍵はゲームコードに依存します。

  Arguments:    userdata : 自分のユーザデータの格納されているポインタ

  Returns:      0以外 : 友達登録鍵
                0     : 入力が正しくない（たとえば、gsProfileId未取得など）
 *---------------------------------------------------------------------------*/
u64
DWC_CreateFriendKey( const DWCAccUserData* userdata )
{
    u64 ret = 0;

    if ( userdata->gs_profile_id != 0 )
    {
        ret = DWC_Acc_CreateFriendKey( userdata->gs_profile_id, userdata->gamecode );
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateFriendKeyToken

  Description:  友達リストに格納するためのFriendDataを友達登録鍵から作成します。

  Arguments:    token      : FriendDataを格納するためのポインタ
                friend_key : 友達登録鍵

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_CreateFriendKeyToken( DWCAccFriendData* token,
                          u64 friend_key )
{
    MI_CpuClear8( token, sizeof( DWCAccFriendData ) );
    DWCi_Acc_SetFriendKey( &token->friend_key, friend_key );
    DWCi_Acc_SetFlag_DataType( &token->flags, DWC_ACC_FRIENDDATA_FRIEND_KEY );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateExchangeToken

  Description:  友達リストに格納するためのFriendDataをUserDataから作成します。

  Arguments:    userdata : ユーザデータが格納されているポインタ
                token      : FriendDataを格納するためのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_CreateExchangeToken( const DWCAccUserData* userdata,
                         DWCAccFriendData* token )
{
    MI_CpuClear8( token, sizeof( DWCAccFriendData ) );

    if ( DWCi_Acc_IsAuthentic( userdata ) )
    {
        DWCi_Acc_SetGsProfileId( &token->gs_profile_id, userdata->gs_profile_id );
        DWCi_Acc_SetFlag_DataType( &token->flags, DWC_ACC_FRIENDDATA_GS_PROFILE_ID );
    }
    else
    {
        // いろいろなフラグを入れて、確率を上げようとしたが、仮IDを生成する本体と
        // ネットにつなぐ本IDを取得する本体の縛りを入れないようにしたため、ほとんどの
        // 情報は無駄になってしまう。
        token->login_id = userdata->pseudo;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetGsProfileId

  Description:  友達リストにgsProfileIdを格納します。

  Arguments:    frienddata    : FriendData構造体へのポインタ
                gs_profile_id : gsProfileId

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_SetGsProfileId( DWCAccFriendData* frienddata,
                    int gs_profile_id )
{
    MI_CpuClear8( frienddata, sizeof( DWCAccFriendData ) );

    DWCi_Acc_SetGsProfileId( &frienddata->gs_profile_id, gs_profile_id );
    DWCi_Acc_SetFlag_DataType( &frienddata->flags, DWC_ACC_FRIENDDATA_GS_PROFILE_ID );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_LoginIdToUserName

  Description:  友達を検索するためのユーザネームを取得します。

  Arguments:    userdata : ユーザデータが格納されているポインタ
                token    : FriendDataが格納されているポインタ
                username : 結果を出力するバッファ(DWC_ACC_USERNAME_STRING_BUFSIZE)

  Returns:      なし
 *---------------------------------------------------------------------------*/
void
DWC_LoginIdToUserName( const DWCAccUserData* userdata,
                       const DWCAccFriendData* frienddata,
                       char* username )
{
    DWCi_Acc_LoginIdToUserName( &frienddata->login_id, userdata->gamecode, username );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsEqualFriendData

  Description:  友達情報DWCFriendDataが同じかどうかを確認します。

  Arguments:    frienddata1 : 比較する友達情報へのポインタ
                frienddata2 : 比較する友達情報へのポインタ

  Returns:      TRUE  : 同じ
                FALSE : 違う
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsEqualFriendData( const DWCAccFriendData* frienddata1,
                       const DWCAccFriendData* frienddata2 )
{
    u32 type1, type2;
    type1 = DWCi_Acc_GetFlag_DataType( &frienddata1->flags );
    type2 = DWCi_Acc_GetFlag_DataType( &frienddata2->flags );

    if ( type1 != type2 )
    {
        return FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        return ( DWCi_Acc_GetGsProfileId( &frienddata1->gs_profile_id ) == DWCi_Acc_GetGsProfileId( &frienddata2->gs_profile_id ) ) ?
            TRUE : FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_LOGIN_ID )
    {
        return ( ( DWCi_Acc_GetUserId  ( &frienddata1->login_id ) == DWCi_Acc_GetUserId  ( &frienddata2->login_id ) ) &&
                 ( DWCi_Acc_GetPlayerId( &frienddata1->login_id ) == DWCi_Acc_GetPlayerId( &frienddata2->login_id ) )    ) ?
            TRUE : FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        return ( DWCi_Acc_GetFriendKey( &frienddata1->friend_key ) == DWCi_Acc_GetFriendKey( &frienddata2->friend_key ) ) ?
            TRUE : FALSE;
    }

    return FALSE;
}

void
DWC_ReportFriendData( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        if ( DWC_IsBuddyFriendData( frienddata ) )
        {
            OS_TPrintf(" GS_ID : %d (ok)\n", DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id ));
        }
        else
        {
            OS_TPrintf(" GS_ID : %d\n", DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id ));
        }
    }
    else if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        char name[DWC_ACC_FRIENDKEY_STRING_BUFSIZE];
        DWC_FriendKeyToString( name, DWCi_Acc_GetFriendKey( &frienddata->friend_key ));
        OS_TPrintf(" F_KEY : %s\n", name );
    }
    else if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_LOGIN_ID )
    {
        char name[DWC_ACC_USERNAME_STRING_BUFSIZE];
        DWCi_Acc_LoginIdToUserName( &frienddata->login_id, userdata->gamecode, name );
        OS_TPrintf(" LN_ID : %s\n", name );
    }
    else
    {
        OS_TPrintf(" NO_DATA \n");
    }
    
}

void
DWC_ReportUserData( const DWCAccUserData* userdata )
{
    OS_TPrintf("*******************************\n");
    OS_TPrintf(" [pseudo login id]\n");
    DWC_ReportFriendData( userdata, (DWCAccFriendData*)&userdata->pseudo );
    OS_TPrintf("+++++++++++++++++++++++++++++++\n");
    OS_TPrintf(" [authentic login id]\n");
    DWC_ReportFriendData( userdata, (DWCAccFriendData*)&userdata->authentic );
    OS_TPrintf("+++++++++++++++++++++++++++++++\n");
    OS_TPrintf(" GS_ID : %d\n", userdata->gs_profile_id );
    OS_TPrintf("*******************************\n");
}
