/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/ppw_profile.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file
 *
 * @brief ポケモンロビーライブラリ 内部公開ヘッダ
 */

#ifndef PPW_PROFILE_H_
#define PPW_PROFILE_H_

#include "dwci_lobbyData.h"
class PPW_Lobby;

// TODO: 自分の情報と他人の情報を別クラスにする
class PPW_LobbyInnerProfile : public DWCi_Base
{
public:
    // データ型定義
    // システム定義プロフィール
    struct SystemProfile
    {
        // 自動的に送受信するシステム管理情報
        // 参加しているサブチャンネル
        PPW_LOBBY_CHANNEL_KIND subChannelKind;
        
        // ロビーに入った時刻
        s64 enterTime;
        
        // メインチャンネルに入る準備が完了したか
        BOOL ready;
        
        SystemProfile()
            : subChannelKind(PPW_LOBBY_CHANNEL_KIND_INVALID)
            , enterTime(PPW_LOBBY_INVALID_TIME)
            , ready(FALSE)
        {
            enterTime = DWCi_Np_GetCurrentSecondByUTC();
        }
        
        // 比較演算子
        bool operator==(const SystemProfile& rhs) const
        {
            return memcmp(this, &rhs, sizeof(*this)) == 0;
        }
        
        PPW_LobbySystemProfile ToPublicSystemProfile() const
        {
            PPW_LobbySystemProfile profile;
            profile.subChannelKind = subChannelKind;
            profile.enterTime = enterTime;
            return profile;
        }
    };
    
    // ユーザ定義データ
    struct UserProfile
    {
        typedef std::vector<u8, DWCi_Allocator<u8> > DataType;
        DataType data;
        
        UserProfile()
            : data()
        {}
        
        // バイト列からの生成
        UserProfile(const u8* _userProfile, u32 _userProfileSize)
        {
            data.assign(_userProfile, _userProfile + _userProfileSize);
        }
        
        // 比較演算子
        bool operator==(const UserProfile& rhs) const
        {
            return data.size() == rhs.data.size() && (data.empty() || memcmp(DWCi_GetVectorBuffer(data), DWCi_GetVectorBuffer(rhs.data), data.size()) == 0);
        }
    };
    // プロフィールの状態
    enum STATE
    {
        STATE_PREPARING,    // 準備中
        STATE_DATARECVED,   // データ取得済み
        STATE_READY,        // データ取得済みかつ準備完了受信済み
        STATE_ACTIVATED     // 接続完了コールバックコール済み
    };
private:
    // 自動的に送受信するシステム定義プロフィール
    DWCi_ChangeDetectable<SystemProfile> systemProfile;
    
    // 自動的に送受信するユーザ定義プロフィール
    DWCi_ChangeDetectable<UserProfile> userProfile;
    
    
    // 内部処理用データ
    
    // プロフィール情報登録状況
    STATE state;
    
    // ユニークID
    s32 pid;
    
    // 初期化されたかチェック用
    BOOL systemProfileInitialized;
    BOOL userProfileInitialized;
    BOOL activated;
public:
    PPW_LobbyInnerProfile()
        : pid(PPW_LOBBY_INVALID_USER_ID){}
    PPW_LobbyInnerProfile(s32 _pid)
        : systemProfile()
        , userProfile()
        , state(STATE_PREPARING)
        , pid(_pid)
        , systemProfileInitialized(FALSE)
        , userProfileInitialized(FALSE)
        , activated(FALSE)
    {
    }
    
    virtual ~PPW_LobbyInnerProfile(){};
    
    // 比較演算子
    bool operator<(const PPW_LobbyInnerProfile& rhs) const
    {
        return pid < rhs.pid;
    }
    
    // find_if用関数オブジェクト
    struct Equal : public std::unary_function<PPW_LobbyInnerProfile*, bool>
    {
        const s32 pid;
        Equal(s32 _pid)
            : pid(_pid)
        {}
        
        bool operator() (const PPW_LobbyInnerProfile* rhs) const
        {
            return pid == rhs->pid;
        }
    };

    /** 
     * @brief アプリケーション及びシステムが情報をセットします。
     * 
     * @param[in] _userProfile ユーザ定義プロフィールデータ。
     * @param[in] _userProfileSize ユーザ定義プロフィールの個数。
     * @param[in] force 強制的に送信するか。
     */
    void SetUserProfile(const u8* _userProfile, u32 _userProfileSize, BOOL force=FALSE)
    {
        UserProfile newUserProfile(_userProfile, _userProfileSize);
        userProfile.Change(newUserProfile, force);
        userProfileInitialized = TRUE;
    }
    
    /** 
     * @brief アプリケーションが情報を取得します。
     * 
     * @param[in] _userProfile ユーザ定義プロフィールデータ。
     * @param[in] _userProfileSize ユーザ定義プロフィールの個数。
     * 
     * @retval TRUE。
     * @retval FALSE。
     */
    BOOL GetUserProfile(u8* _userProfile, u32* _userProfileSize) const
    {
        UserProfile::DataType data = userProfile.Get().data;
        u32 orgSize = *_userProfileSize;
        
        // 正しいサイズを伝える
        *_userProfileSize = data.size();
        
        if(*_userProfileSize == 0)
        {
            return TRUE;
        }
        
        if(orgSize < data.size() || _userProfile == NULL)
        {
            return FALSE;
        }
        DWCi_Np_CpuCopy8(DWCi_GetVectorBuffer(data), _userProfile, data.size());
        return TRUE;
    }
    
    /** 
     * @brief ユーザ定義プロフィールを変更確認せずに取得します。
     * 
     * @retval ユーザ定義プロフィール。
     */
    const UserProfile& GetUserProfile() const
    {
        return userProfile.Get();
    }
    
    /** 
     * @brief  ユーザデータを変更確認、取得します。
     * 
     * @retval ユーザ定義プロフィール。
     */
    const UserProfile& GetUserProfileConfirm() const
    {
        return userProfile.Confirm();
    }
    
    /** 
     * @brief  ユーザデータが更新されたかを調べます。
     * 
     * @retval TRUE 更新されている。
     * @retval FALSE 更新されていない。
     */
    BOOL IsUserProfileUpdated() const
    {
        return userProfile.IsChanged();
    }
    
    /** 
     * @brief  システム定義プロフィールを全てセットします。
     * 
     * @param _systemProfile システム定義プロフィール
     */
    void SetSystemProfile(const SystemProfile& _systemProfile)
    {
        systemProfile = _systemProfile;
        systemProfileInitialized = TRUE;
    }
    
    /** 
     * @brief システム定義プロフィールを変更確認せずに取得します。
     * 
     * @retval システム定義プロフィール。
     */
    const SystemProfile& GetSystemProfile() const
    {
        return systemProfile.Get();
    }
    
    /** 
     * @brief システム定義プロフィールを変更確認、取得します。
     * 
     * @retval システム定義プロフィール。
     */
    const SystemProfile& GetSystemProfileConfirm() const
    {
        return systemProfile.Confirm();
    }
    
    /** 
     * @brief システムデータが更新されたかを調べます。
     * 
     * @retval TRUE 更新されている。
     * @retval FALSE 更新されていない。
     */
    BOOL IsSystemProfileUpdated() const
    {
        return systemProfile.IsChanged();
    }
    
    /** 
     * @brief ユーザ接続コールバックを呼んだことを記録します。
     */
    void Activate()
    {
        activated = TRUE;
    }
    
    /** 
     * @brief ユーザ接続コールバックをコール済みかを調べます。
     * 
     * @retval TRUE 呼んだ。
     * @retval FALSE まだ。
     */
    BOOL IsActivated() const
    {
        return activated;
    }
    
    /** 
     * @brief 状態を取得します。
     * 
     * @retval ステート。
     */
    STATE GetState() const
    {
        return state;
    }
    
    /** 
     * @brief pidを取得します。
     * 
     * @retval pid。
     */
    s32 GetPid() const
    {
        return pid;
    }
    
    /** 
     * @brief ユーザデータとシステムデータをすでに受信済みか。
     * 
     * @retval TRUE 受信済み。
     * @retval FALSE まだ。
     */
    bool IsDataRecved() const
    {
        return state >= STATE_DATARECVED;
    }
    
    void Process();
};

class PPW_LobbyProfileManager : public DWCi_Base
{
    typedef std::set<PPW_LobbyInnerProfile*, DWCi_Base::ptr_less<PPW_LobbyInnerProfile>, DWCi_Allocator<PPW_LobbyInnerProfile*> > PPW_ProfileContainer;
    PPW_ProfileContainer profiles;
    PPW_LobbyInnerProfile myProfile;
public:
    PPW_LobbyProfileManager()
        : profiles()
        , myProfile()
    {}
    virtual ~PPW_LobbyProfileManager()
    {
        ClearProfile();
    }
    
    /** 
     * @brief 指定したユーザがActivateされているか調べます。
     * 
     * @retval TRUE アクティベート済み。
     * @retval FALSE まだ。
     */
    BOOL IsActivatedProfile(s32 id)
    {
        PPW_LobbyInnerProfile* profile = FindProfile(id);
        return profile && profile->IsActivated();
    }
    
    /** 
     * @brief 自分のプロフィールを取得する。
     * 
     * @retval 自分のプロフィール。
     */
    PPW_LobbyInnerProfile& GetMyProfile()
    {
        return myProfile;
    }
    
    BOOL                    AddProfile(s32 id);
    BOOL                    RemoveProfile(s32 id);
    PPW_LobbyInnerProfile*  FindProfile(s32 id);
    void                    ClearProfile();
    
    BOOL                                    IsFull() const;
    std::vector<s32, DWCi_Allocator<s32> >  EnumId() const;
    void    InitializeMyProfile(s32 id, const u8* userProfile, u32 userProfileSize);
    BOOL    CheckAllProfilesRecved() const;
    void    Process();
};

#endif
