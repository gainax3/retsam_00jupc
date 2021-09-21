/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_lobbyUtil.h

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
 * @brief Wi-Fi ロビーライブラリ ユーティリティヘッダ
 */

#ifndef DWCi_LOBBY_UTIL_H_
#define DWCi_LOBBY_UTIL_H_

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <string.h>
#include "dwci_lobbyNonport.h"
#include "dwci_lobbyBase.h"

#define DWC_SAFE_FREE(ptr) if(ptr){ DWC_Free((DWCAllocType)0, ptr, 0); ptr=NULL; }
#define DWC_SAFE_DELETE(ptr) if(ptr){ delete ptr; ptr = NULL; }

// ライブラリ内部で使用するアロケータ
template <class T> class DWCi_Allocator;

// ライブラリ内部で使用する文字列
typedef std::basic_string<char, std::char_traits<char>, DWCi_Allocator<char> > DWCi_String;

// 静的アサート
// 型引数がfalseになるとコンパイルエラーになる
template<bool>
class DWCi_StaticAssert;
template<>
class DWCi_StaticAssert<true>{};
#define DWCi_STATIC_ASSERT(exp) DWCi_StaticAssert<(exp)>()


// vectorのデータバッファを取得する。vector::data()
template <class T, class AllocatorT>
T* DWCi_GetVectorBuffer(const std::vector<T, AllocatorT>& vec)
{
    if(vec.empty())
    {
        return NULL;
    }
    return const_cast<T*>(&vec[0]);
}

// DWCi_Stringを大文字小文字を無視して比較する
template <class T>
s32 DWCi_Stricmp(T lhs, T rhs)
{
    // 関数型の定義。(char (*)(char))など同じ。
    typedef typename T::value_type (*ValueTypeToLower)(typename T::value_type);
    
    // 小文字に変換してから比較
    // std::tolowerはシグネチャをキャストしないとwarningが出る。
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), (ValueTypeToLower)std::tolower);
    std::transform(rhs.begin(), rhs.end(), rhs.begin(), (ValueTypeToLower)std::tolower);
    return lhs.compare(rhs);
}

// DWCi_Stringを大文字小文字を無視し文字数を指定して比較する
template <class T>
s32 DWCi_Strnicmp(T lhs, T rhs, std::size_t n)
{
    // 関数型の定義。(char (*)(char))など同じ。
    typedef typename T::value_type (*ValueTypeToLower)(typename T::value_type);
    
    // 小文字に変換してから比較
    std::transform(lhs.begin(), lhs.end(), lhs.begin(), (ValueTypeToLower)std::tolower);
    std::transform(rhs.begin(), rhs.end(), rhs.begin(), (ValueTypeToLower)std::tolower);
    return lhs.compare(0, n, rhs);
}

inline s32 DWCi_StrnicmpChar(const char* lhs, const char* rhs, std::size_t n)
{
    return DWCi_Strnicmp(DWCi_String(lhs), DWCi_String(rhs), n);
}

// DWCi_Stringなどのchar文字列用のsprintf
// Tはstd::string互換の型
template <typename T>
T DWCi_SPrintf(const char* format, ...)
{
    va_list           arguments;
    std::vector<char, typename T::allocator_type > buffer;

    DWC_ASSERTMSG(format, "format is NULL");
    va_start(arguments, format);

    int length = DWCi_Np_VSNPrintf(NULL, 0, format, arguments) + 1;
    DWC_ASSERTMSG(length > 0, format);
    buffer.resize((std::size_t)length);
    int result = DWCi_Np_VSNPrintf(DWCi_GetVectorBuffer(buffer), (std::size_t)length, format, arguments);
    DWC_ASSERTMSG(result > 0, format);
    buffer[(std::size_t)length-1]    = '\0';    // 保険

    va_end(arguments);
    return T(DWCi_GetVectorBuffer(buffer));
}

// DWCi_Stringなどのchar文字列用のsnprintf
// sizeにはsnprintf同様バッファのサイズを与える。size-1番目には必ず'\0'が書き込まれる。
template <class T>
T DWCi_SNPrintf(std::size_t size, const char* format, ...)
{
    va_list           arguments;
    std::vector<char, typename T::allocator_type> buffer(size);

    DWC_ASSERTMSG(format, "format is NULL");
    va_start(arguments, format);
    DWCi_Np_VSNPrintf(DWCi_GetVectorBuffer(buffer), size, format, arguments);
    va_end(arguments);
    return T(DWCi_GetVectorBuffer(buffer));
}

/**
 * split関数
 *
 * @param inStr 分割したい文字列
 * @param delims デリミタ(文字の配列)
 * @param limit 分割する最大数。先頭からlimit個分割する。1なら分割されずそのままのstringが入る。0以下または省略すると全て分割する。
 * 
 * @retval 分割された文字列
*/
template <class T>
std::vector<T, DWCi_Allocator<T> > DWCi_SplitByChars(const T& inStr, const T& delims, std::size_t limit=0)
{
    std::vector<T, DWCi_Allocator<T> > result;
    std::size_t cutAt;
    std::size_t splitCount = 1;
    T str(inStr);
    
    while( splitCount != limit && (cutAt = str.find_first_of(delims)) != str.npos )
    {
        if(cutAt > 0)
        {
            result.push_back(str.substr(0, cutAt));
            splitCount++;
        }
        str = str.substr(cutAt + 1);
    }
    if(str.length() > 0)
    {
        result.push_back(str);
    }
    return result;
}

/**
 * split関数
 *
 * @param inStr 分割したい文字列
 * @param delim デリミタ(文字列)
 * @param limit 分割する最大数。先頭からlimit個分割する。1なら分割されずそのままのstringが入る。0以下または省略すると全て分割する。
 * 
 * @retval 分割された文字列
*/
template <class T>
std::vector<T, DWCi_Allocator<T> > DWCi_SplitByStr(const T& inStr, const T& delim, std::size_t limit=0)
{
    std::vector<T, DWCi_Allocator<T> > result;
    std::size_t cutAt;
    std::size_t splitCount = 1;
    T str(inStr);
    
    while( splitCount != limit && (cutAt = str.find(delim)) != str.npos )
    {
        if(cutAt > 0)
        {
            result.push_back(str.substr(0, cutAt));
            splitCount++;
        }
        str = str.substr(cutAt + delim.length());
    }
    if(str.length() > 0)
    {
        result.push_back(str);
    }
    return result;
}

// Base64デコード
template <class charT, class AllocatorT>  // charTには1byteの型を指定してください。
BOOL DWCi_Base64Decode(const char* value, std::vector<charT, AllocatorT >& buf)
{
    DWCi_STATIC_ASSERT(sizeof(charT) == 1);
    
    u32 size = strlen(value);
    buf.resize(size);
    if(size == 0)
    {
        // サイズ0の文字列が与えられたときはサイズ0のvectorを返す
        return TRUE;
    }
    int retSize = DWC_Base64Decode(value, strlen(value), (char*)DWCi_GetVectorBuffer(buf), buf.size());
    if(retSize == -1)
    {
        return FALSE;
    }
    buf.resize((std::size_t)retSize);
    return TRUE;
}

// 変更を検知するクラス
// 値が変更されたのを検知し、Confirm()が呼ばれたときに確認したものとする
// Tは値が変更されたかをチェックするために==演算子をオーバーロードする必要がある。
template <class T>
class DWCi_ChangeDetectable : public DWCi_Base
{
    T value;
    mutable BOOL changed;
    
public:
    typedef T value_type;
    
    DWCi_ChangeDetectable()
        : value()
        , changed(TRUE)
    {}
    
    // value_typeからのコンストラクタ
    DWCi_ChangeDetectable(const T& _value)
        : value(_value)
        , changed(TRUE)
    {}
    
    // コピーコンストラクタ
    DWCi_ChangeDetectable(const DWCi_ChangeDetectable& src)
    {
        *this = src;
    }
    
    // 代入
    DWCi_ChangeDetectable& operator=(const DWCi_ChangeDetectable& rhs)
    {
        if(!(value == rhs.value))
        {
            value = rhs.value;
            changed = TRUE;
        }
        return *this;
    }
    
    // value_typeからの代入
    DWCi_ChangeDetectable& operator=(const T& _value)
    {
        return *this = DWCi_ChangeDetectable<T>(_value);
    }
    
    // 代入と同じだがforceがTRUEのときは強制的に変更する
    const DWCi_ChangeDetectable& Change(const DWCi_ChangeDetectable& src, BOOL force)
    {
        if(force)
        {
            changed = TRUE;
        }
        return *this = src;
    }
    
    // value_typeからのChange
    const DWCi_ChangeDetectable& Change(const T& _value, BOOL force)
    {
        return this->Change(DWCi_ChangeDetectable<T>(_value), force);
    }
    
    // value_typeの演算子をエミュレート
    bool operator==(const T& _value) const
    {
        return value == _value;
    }
    bool operator<(const T& _value) const
    {
        return value < _value;
    }
    
    // 変更されたか調べる
    BOOL IsChanged() const
    {
        return changed;
    }
    
    // 変更を確認して取得する
    const T& Confirm() const
    {
        changed = FALSE;
        return value;
    }
    
    // 値を取得する
    const T& Get() const
    {
        return value;
    }
};

#endif
