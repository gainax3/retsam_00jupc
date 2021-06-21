/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_base.h

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
 * @brief Wi-Fi ロビーライブラリ アロケータヘッダ
 */

#ifndef DWCi_LOBBY_BASE_H_
#define DWCi_LOBBY_BASE_H_

#pragma exceptions off
#ifndef NITRO_DEBUG
    #pragma RTTI off
#endif

//#include <new>
namespace std
{
    typedef void (*new_handler)();
}

#include <dwc.h>
#include <functional>

#ifdef max  // std::numeric<T>::maxが使えなくなるのでundef
    #undef max
#endif
#ifdef min  // std::numeric<T>::minが使えなくなるのでundef
    #undef min
#endif
#include <limits>



// new, delete, allocator 定義クラス。このライブラリで使う(動的に生成する)構造体、クラスは全てこのクラスを継承する
// コンテナに入れるときは必ずアロケータに DWCi_Allocator<T> を指定する。ただしTはコンテナに入れるクラス。
// 例1: std::vector<DWCi_Channel, DWCi_Allocator<DWCi_Channel> > channel;
// 例2: std::set<DWCi_Channel, std::less<DWCi_Channel>, DWCi_Allocator<DWCi_Channel> > testset;
class DWCi_Base
{
protected:
    DWCi_Base(){}
    // このクラスのポインタを使用して派生クラスのdeleteを行わないので非virtualのprotectedにする
    ~DWCi_Base(){}
public:
    static void* operator new(std::size_t size) throw();
    
    // 配置new
    static void* operator new (std::size_t size, void* p) throw()
    {
        (void)size;
        return p;
    }
    
    static void* operator new[](size_t size) throw()
    {
        return operator new(size);
    }
    
    static void operator delete(void *rawMemory)
    {
        if(rawMemory == NULL)
        {
            return;
        }
        //DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_Base::operator delete: deleted at %p\n", rawMemory);
        DWC_Free((DWCAllocType)0, rawMemory, 0);
    }
    
    static void operator delete(void *rawMemory, void* p)
    {
        (void)rawMemory;
        (void)p;
        return;
    }
    
    static void operator delete[](void *rawMemory)
    {
        operator delete(rawMemory);
    }
    
    // 独自にnew_handlerを定義する
private:
    static std::new_handler s_currentNewHandler;
public:
    static std::new_handler set_new_handler(std::new_handler handler) throw()
    {
        std::new_handler oldNewHandler = s_currentNewHandler;
        s_currentNewHandler = handler;
        return oldNewHandler;
    }
    
public:
    // ポインタの指している先を比較する関数オブジェクト
    template<class T>
    struct ptr_less : std::binary_function<T*, T*, bool>
    {
        bool operator()(const T* x, const T* y) const
        {
            return *x < *y;
        }
    };
};

// カスタムアロケータ
// DWCi_Baseのnew, deleteを使用する
template <class T>
class DWCi_Allocator
{
public:
    typedef T        value_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    // 型Uへのrebind
    template <class U>
    struct rebind
    {
        typedef DWCi_Allocator<U> other;
    };

    // 値のアドレスをとる
    pointer address (reference value) const
    {
        return &value;
    }
    const_pointer address (const_reference value) const
    {
        return &value;
    }

    // コンストラクタとデストラクタ
    // アロケータはステートを持たないので何もしない
    DWCi_Allocator() throw()
    {
    }
    DWCi_Allocator(const DWCi_Allocator&) throw()
    {
    }
    template <class U>
    DWCi_Allocator (const DWCi_Allocator<U>&) throw()
    {
    }
    ~DWCi_Allocator() throw()
    {
    }

    // アロケートできる最大数を返す
    size_type max_size () const throw()
    {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // アロケートするが初期化はしない
    pointer allocate (size_type num, const void* = 0)
    {
        // DWCi_Baseのnewを使う
        pointer ret = (pointer)(DWCi_Base::operator new(num*sizeof(T)));
        return ret;
    }

    // 配置newで初期化する
    void construct (pointer p, const T& value)
    {
        // DWCi_Baseを継承していないTに対してはnewヘッダの配置newが使われる
        new((void*)p) T(value);
    }

    // デストラクタで破棄する
    void destroy (pointer p)
    {
        p->~T();
    }

    // 破棄されたオブジェクトの領域を解放する
    void deallocate (pointer p, size_type num)
    {
        (void)num;
        DWCi_Base::operator delete((void*)p);
    }
};

// このアロケータから生成される型は全て等しいものとする
template <class T1, class T2>
bool operator== (const DWCi_Allocator<T1>&,
                 const DWCi_Allocator<T2>&) throw()
{
    return true;
}
template <class T1, class T2>
bool operator!= (const DWCi_Allocator<T1>&,
                 const DWCi_Allocator<T2>&) throw()
{
    return false;
}

#endif
