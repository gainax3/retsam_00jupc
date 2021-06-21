/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - account
  File:     dwc_account.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.23 $
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ACCOUNT_H_
#define DWC_ACCOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** --------------------------------------------------------------------
  ユーザデータ格納用の構造体
  ----------------------------------------------------------------------*/
// フラグチェック用の構造体
typedef struct DWCstAccFlag
{
    u32         flags;
    u32         reserved;
    u32         reserved1;
} DWCAccFlag;

// ログインID格納用の構造体
typedef struct DWCstAccLoginId
{
    u32         id_data;
    u32         userid_lo32;
    u32         playerid;
} DWCAccLoginId;

// 友達登録鍵用の構造体
typedef struct DWCstAccFriendKey
{
    u32         id_data;
    u32         friendkey_lo32;
    u32         friendkey_hi32;
} DWCAccFriendKey;

// GS Profile ID用の構造体
typedef struct DWCstAccGsProfileId
{
    u32         id_data;
    int         id;
    u32         reserved;
} DWCAccGsProfileId;

// 友達情報格納用の共用体 12Bytes
typedef union DWCstAccFriendData
{
    DWCAccFlag          flags;
    DWCAccLoginId       login_id;
    DWCAccFriendKey     friend_key;
    DWCAccGsProfileId   gs_profile_id;
} DWCAccFriendData;

// 自分のデータ格納用の構造体 64Bytes
typedef struct DWCstAccUserData
{
    int                 size;               // 4
    DWCAccLoginId       pseudo;             // 12
    DWCAccLoginId       authentic;          // 12
    int                 gs_profile_id;      // 4
    int                 flag;               // 4
    u32                 gamecode;           // 4

    int                 reserved[5];        // 20
    u32                 crc32;              // 4
} DWCAccUserData;

// DWCUserDataの定義
typedef DWCAccUserData DWCUserData;
typedef DWCAccFriendData DWCFriendData;

/** --------------------------------------------------------------------
  定数定義
  ----------------------------------------------------------------------*/
// FriendData用の定義
#define DWC_ACC_USERDATA_BUFSIZE            64

#define DWC_ACC_USERNAME_STRING_LENGTH      20
#define DWC_ACC_USERNAME_GSBRCD_OFFSET      9
#define DWC_ACC_USERNAME_STRING_BUFSIZE     (DWC_ACC_USERNAME_STRING_LENGTH + 1)
#define DWC_ACC_FRIENDKEY_STRING_LENGTH     12
#define DWC_ACC_FRIENDKEY_STRING_BUFSIZE    (DWC_ACC_FRIENDKEY_STRING_LENGTH + 1)

#define DWC_ACC_MASKBITS(bits)          (((1u<<bits)-1))

#define DWC_ACC_USERID_BITS             43
#define DWC_ACC_PLAYERID_BITS           32

#define DWC_ACC_USERID_HI32_SHIFT       0
#define DWC_ACC_USERID_HI32_BITS        11
#define DWC_ACC_USERID_HI32_MASK        DWC_ACC_MASKBITS(DWC_ACC_USERID_HI32_BITS)
#define DWC_ACC_FLAGS_SHIFT             11
#define DWC_ACC_FLAGS_BITS              21
#define DWC_ACC_FLAGS_MASK              DWC_ACC_MASKBITS(DWC_ACC_FLAGS_BITS)

#define DWC_ACC_FRIENDDATA_NODATA           0
#define DWC_ACC_FRIENDDATA_LOGIN_ID         1
#define DWC_ACC_FRIENDDATA_FRIEND_KEY       2
#define DWC_ACC_FRIENDDATA_GS_PROFILE_ID    3
#define DWC_ACC_FRIENDDATA_MASK             3

#define DWC_ACC_FRIENDDATA_ISBUDDY          (1u << 2)
#define DWC_ACC_FRIENDDATA_ISBUDDY_MASK     (1u << 2)

#define DWC_ACC_USERDATA_DIRTY                    (1u)
#define DWC_ACC_USERDATA_DIRTY_MASK               (1u)

/** --------------------------------------------------------------------
  公開定義
  ----------------------------------------------------------------------*/
#define DWC_FRIENDDATA_NODATA        DWC_ACC_FRIENDDATA_NODATA
#define DWC_FRIENDDATA_LOGIN_ID      DWC_ACC_FRIENDDATA_LOGIN_ID
#define DWC_FRIENDDATA_FRIEND_KEY    DWC_ACC_FRIENDDATA_FRIEND_KEY
#define DWC_FRIENDDATA_GS_PROFILE_ID DWC_ACC_FRIENDDATA_GS_PROFILE_ID
#define DWC_FRIENDKEY_STRING_BUFSIZE DWC_ACC_FRIENDKEY_STRING_BUFSIZE

/** --------------------------------------------------------------------
  関数の外部定義
  ----------------------------------------------------------------------*/
// ユーザデータの作成／確認用。
void    DWC_CreateUserData            ( DWCAccUserData* userdata, u32 gamecode );
BOOL    DWC_CheckUserData             ( const DWCAccUserData* userdata );
BOOL    DWC_CheckHasProfile           ( const DWCAccUserData* userdata );
BOOL    DWC_CheckValidConsole         ( const DWCAccUserData* userdata );
BOOL    DWC_CheckDirtyFlag            ( const DWCAccUserData* userdata );
void    DWC_ClearDirtyFlag            ( DWCAccUserData* userdata );

// 友達関係
u64     DWC_GetFriendKey              ( const DWCAccFriendData* frienddata );
u64     DWC_CreateFriendKey           ( const DWCAccUserData*   userdata );
void    DWC_CreateExchangeToken       ( const DWCAccUserData*   userdata, DWCAccFriendData* token );
void    DWC_CreateFriendKeyToken      ( DWCAccFriendData* token,      u64 friend_key    );
void    DWC_SetGsProfileId            ( DWCAccFriendData* frienddata, int gs_profile_id );
BOOL    DWC_CheckFriendKey            ( const DWCAccUserData*   userdata, u64 friend_key );
BOOL    DWC_IsEqualFriendData         ( const DWCAccFriendData* frienddata1, const DWCAccFriendData* frienddata2 );
BOOL    DWC_IsValidFriendData         ( const DWCAccFriendData* frienddata );
BOOL    DWC_IsBuddyFriendData         ( const DWCAccFriendData* frienddata );
int     DWC_GetFriendDataType         ( const DWCAccFriendData* frienddata );
void    DWC_ClearBuddyFlagFriendData  ( DWCAccFriendData*       frienddata );

// 友達検索用
int     DWC_GetGsProfileId            ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata );
void    DWC_LoginIdToUserName         ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata, char* username );

// デバッグ用
void    DWC_ReportFriendData          ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata );
void    DWC_ReportUserData            ( const DWCAccUserData* userdata );

// 友達登録鍵関係
u64     DWC_StringToFriendKey         ( const char* string );
void    DWC_FriendKeyToString         ( char* string, u64 friendKey );

// 内部関数。
void    DWCi_Acc_CreateUserData       ( DWCAccUserData* userdata, u32 gamecode );
void    DWCi_Acc_CreateTempLoginId    ( DWCAccLoginId* loginid );

BOOL    DWC_Acc_CheckFriendKey        ( u64 friendkey, u32 gamecode );
u64     DWC_Acc_CreateFriendKey       ( int gs_profile_id, u32 gamecode );
int     DWC_Acc_FriendKeyToGsProfileId( u64 friendkey, u32 gamecode );

u64     DWCi_Acc_GetUserId            ( const DWCAccLoginId*     data );
u32     DWCi_Acc_GetPlayerId          ( const DWCAccLoginId*     data );
u64     DWCi_Acc_GetFriendKey         ( const DWCAccFriendKey*   data );
int     DWCi_Acc_GetGsProfileId       ( const DWCAccGsProfileId* data );
void    DWCi_Acc_SetUserId            ( DWCAccLoginId*     data, u64 userid );
void    DWCi_Acc_SetPlayerId          ( DWCAccLoginId*     data, u32 playerid );
void    DWCi_Acc_SetFriendKey         ( DWCAccFriendKey*   data, u64 friend_key );
void    DWCi_Acc_SetGsProfileId       ( DWCAccGsProfileId* data, int gs_profile_id );

void    DWCi_Acc_LoginIdToUserName    ( const DWCAccLoginId* loginid, u32 gamecode, char* username );
void    DWCi_Acc_SetLoginIdToUserData ( DWCAccUserData* userdata, const DWCAccLoginId* loginid, int gs_profile_id );
BOOL    DWCi_Acc_CheckConsoleUserId   ( const DWCAccLoginId* loginid );

BOOL    DWCi_Acc_IsDirty              ( const DWCAccUserData* userdata );
void    DWCi_Acc_ClearDirty           ( DWCAccUserData* userdata );
BOOL    DWCi_Acc_IsValidLoginId       ( const DWCAccLoginId* loginid );
BOOL    DWCi_Acc_IsAuthentic          ( const DWCAccUserData* userdata );
BOOL    DWCi_Acc_IsValidFriendData    ( const DWCAccFriendData* frienddata );

void    DWCi_SetBuddyFriendData       ( DWCAccFriendData* frienddata );

void
DWCi_Acc_TestFlagFunc( void );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DWC_ACCOUNT_H_
