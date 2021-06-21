/*---------------------------------------------------------------------------*
  Project:  AdminTool

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Threading;

namespace NWC
{
    // リクエスト用インタフェース
    interface IRequestContainer
    {
        void hton();
    }

    // レスポンス用インタフェース
    interface IResponseContainer
    {
        void ntoh();
    }

    // エンディアン変換などのユーティリティ
    class SessionUtil
    {
        // 現在実行中のシステムがリトルエンディアンの場合にバイト列を反転する
        public static byte[] ConvertEndian(byte[] bin)
        {
            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(bin);
                return bin;
            }
            else
            {
                return bin;
            }
        }

        public static short ntoh(short a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToInt16(b, 0);
        }
        public static ushort ntoh(ushort a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToUInt16(b, 0);
        }
        public static int ntoh(int a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToInt32(b, 0);
        }
        public static uint ntoh(uint a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToUInt32(b, 0);
        }
        public static long ntoh(long a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToInt64(b, 0);
        }
        public static ulong ntoh(ulong a)
        {
            byte[] b = BitConverter.GetBytes(a);
            ConvertEndian(b);
            return BitConverter.ToUInt64(b, 0);
        }
        public static short hton(short a)
        {
            return ntoh(a);
        }
        public static ushort hton(ushort a)
        {
            return ntoh(a);
        }
        public static int hton(int a)
        {
            return ntoh(a);
        }
        public static uint hton(uint a)
        {
            return ntoh(a);
        }
        public static long hton(long a)
        {
            return ntoh(a);
        }
        public static ulong hton(ulong a)
        {
            return ntoh(a);
        }
    }

    /// <summary>
    /// 暗号化DLLラッパー
    /// </summary>
    public class EncSession
    {

        enum DWC_END_SESSION_STATE
        {
            CANCELED,		//!< キャンセル
            ERROR,		    //!< エラー発生
            INITIAL,		//!< 初期状態
            INITIALIZED,	//!< 初期化後
            REQUEST,		//!< リクエスト発行
            GETTING_TOKEN,  //!< トークン取得中
            GOT_TOKEN,	    //!< トークン取得完了
            SENDING_DATA,	//!< データ送信中
            COMPLETED		//!< 完了
        }

        public enum DWC_ENC_SERVER
        {
            RELEASE,             //!< 製品用サーバーに接続します
            DEBUG,               //!< 開発用サーバーに接続します
            TEST                 //!< (使用しないでください)
        }

        private DWC_ENC_SERVER currentServer;
        private string key;
        private string proxyServer;
        public string ProxyServer
        {
            get { return proxyServer; }
            set
            {
                if (value != proxyServer)
                {
                    proxyServer = value;
                    ChangeServer(currentServer);
                }
                else
                {
                    proxyServer = value;
                }
            }
        }


        /// <summary>
        /// 内部でDWC_Init関数、DWCi_EncSessionInitialize関数が呼ばれます。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern void DWC_EncSessionInitialize( int server, string initdata );

        /// <summary>
        /// 内部でghttpSetProxy関数が呼ばれます。管理ツール専用API。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern bool DWC_EncSetProxy(string proxyServer);

        /// <summary>
        /// 内部でDWC_ClearError関数、DWCi_EncSessionShutdown関数、DWC_Shutdown関数が呼ばれます。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern void DWC_EncSessionShutdown();

        /// <summary>
        /// 内部でDWCi_EncSessionGetAsync関数が呼ばれます。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern int DWC_EncSessionGetAsync(string url, int pid, IntPtr data, uint size, int cb);

        /// <summary>
        /// 内部でDWCi_EncSessionProcess関数が呼ばれます。
        /// </summary>
        /// <returns></returns>
        [DllImport("dwcenc.dll")]
        protected static extern int DWC_EncSessionProcess();

        /// <summary>
        /// 内部でDWCi_EncSessionGetResponse関数が呼ばれます。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern IntPtr DWC_EncSessionGetResponse(ref uint size);

        /// <summary>
        /// 内部でDWCi_EncSessionCancel関数が呼ばれます。
        /// </summary>
        [DllImport("dwcenc.dll")]
        protected static extern void DWC_EncSessionCancel();

        protected static bool DWC_EncSessionInitializeWithProxy(int server, string initdata, string proxyServer)
        {
            DWC_EncSessionInitialize(server, initdata);
            if (proxyServer != null && proxyServer != "")
            {
                return EncSession.DWC_EncSetProxy(proxyServer);
            }
            return true;
        }

        /// <summary>
        /// GameSpyバックエンドとの通信を初期化する
        /// </summary>
        /// <param name="server">接続先サーバ</param>
        /// <param name="_key">ゲームごとに異なるシークレットキー</param>
        internal EncSession(DWC_ENC_SERVER server, string _key, string _proxyServer)
        {
            currentServer = server;
            key = _key;
            proxyServer = _proxyServer;
            EncSession.DWC_EncSessionInitializeWithProxy((int)server, key, proxyServer);
        }

        ~EncSession()
        {
            EncSession.DWC_EncSessionShutdown();
        }

        /// <summary>
        /// 接続先サーバを切り替える。前回と同じサーバを指定したときは接続し直す。
        /// </summary>
        /// <param name="server">接続先サーバ</param>
        /// <returns>プロキシサーバフォーマットが正しいか</returns>
        internal bool ChangeServer(DWC_ENC_SERVER server)
        {
            currentServer = server;
            EncSession.DWC_EncSessionShutdown();
            return EncSession.DWC_EncSessionInitializeWithProxy((int)server, key, proxyServer);
        }

        /// <summary>
        /// 接続先サーバ、ディレクトリを切り替える。前回と同じサーバを指定したときは接続し直す。
        /// </summary>
        /// <param name="server">接続先サーバ</param>
        /// <param name="_key">シークレットキー</param>
        /// <returns>プロキシサーバフォーマットが正しいか</returns>
        internal bool ChangeServer(DWC_ENC_SERVER server, string _key)
        {
            currentServer = server;
            key = _key;
            EncSession.DWC_EncSessionShutdown();
            return EncSession.DWC_EncSessionInitializeWithProxy((int)server, key, proxyServer);
        }

        protected bool WaitForResponse()
        {
            DWC_END_SESSION_STATE state;

            while(true)
            {
                state = (DWC_END_SESSION_STATE)EncSession.DWC_EncSessionProcess();

                switch (state)
                {
                    case DWC_END_SESSION_STATE.COMPLETED:		//!< 完了
                        return true;
                    case DWC_END_SESSION_STATE.CANCELED:		//!< キャンセル
                    case DWC_END_SESSION_STATE.ERROR:			//!< エラー発生
                        return false;
                    case DWC_END_SESSION_STATE.INITIAL:			//!< 初期状態
                    case DWC_END_SESSION_STATE.INITIALIZED:		//!< 初期化後
                    case DWC_END_SESSION_STATE.REQUEST:			//!< リクエスト発行
                    case DWC_END_SESSION_STATE.GETTING_TOKEN:	//!< トークン取得中
                    case DWC_END_SESSION_STATE.GOT_TOKEN:		//!< トークン取得完了
                    case DWC_END_SESSION_STATE.SENDING_DATA:	//!< データ送信中
                        break;
                }
                Thread.Sleep(10);
            }
        }

        /// <summary>
        /// 可変長のリクエストを送信し、レスポンスのポインタを返す
        /// </summary>
        /// <param name="url">接続先パス</param>
        /// <param name="id">ProfileID</param>
        /// <param name="inPtr">リクエストを格納したbyte列</param>
        /// <param name="outPtr">レスポンスを指したポインタ</param>
        /// <param name="outSize">レスポンスサイズ</param>
        internal void Session(string url, int id, byte[] bytes, out IntPtr outPtr, out uint outSize)
        {
            uint realSize = 0;
            int inSize = 0;
            IntPtr inPtr = new IntPtr();

            if (bytes == null || bytes.Length == 0)
            {
                //throw new Exception("入力データが空です。");
            }
            else
            {
                inSize = Marshal.SizeOf(bytes[0]) * bytes.Length;
                inPtr = Marshal.AllocHGlobal(inSize);
            }

            try
            {
                if (bytes.Length != 0)
                {
                    Marshal.Copy(bytes, 0, inPtr, bytes.Length);
                }

                lock (this)
                {
                    EncSession.DWC_EncSessionGetAsync(url, id, inPtr, (uint)inSize, 0);
                    if (!WaitForResponse())
                    {
                        throw new Exception("HTTPエラー");
                    }
                    outPtr = EncSession.DWC_EncSessionGetResponse(ref realSize);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(inPtr);
            }

            outSize = realSize;
            //MessageBox.Show("PTR: " + resultPtr + " size : " + realSize + " send size: " + Marshal.SizeOf(request));
        }

        /// <summary>
        /// 固定長のリクエストを送信し、レスポンスのポインタを返す
        /// </summary>
        /// <param name="url">接続先パス</param>
        /// <param name="id">ProfileID</param>
        /// <param name="request">リクエストデータ</param>
        /// <param name="outPtr">レスポンスを指したポインタ</param>
        /// <param name="outSize">レスポンスサイズ</param>
        internal void Session(string url, int id, object request, out IntPtr outPtr, out uint outSize)
        {
            uint realSize = 0;
            IntPtr ptr = Marshal.AllocCoTaskMem(Marshal.SizeOf(request));
            Marshal.StructureToPtr(request, ptr, false);

            try
            {
                lock (this)
                {
                    EncSession.DWC_EncSessionGetAsync(url, id, ptr, (uint)Marshal.SizeOf(request), 0);
                    if (!WaitForResponse())
                    {
                        throw new Exception("HTTPエラー");
                    }
                    outPtr = EncSession.DWC_EncSessionGetResponse(ref realSize);
                }
            }
            finally
            {
                Marshal.FreeCoTaskMem(ptr);
            }
            outSize = realSize;
            //MessageBox.Show("PTR: " + resultPtr + " size : " + realSize + " send size: " + Marshal.SizeOf(request));
        }

        /// <summary>
        /// 固定長のリクエストを送信し、固定長のレスポンスを返す
        /// </summary>
        /// <typeparam name="T_REQ">リクエストデータクラス</typeparam>
        /// <typeparam name="T_RES">レスポンスデータクラス</typeparam>
        /// <param name="url">接続先パス</param>
        /// <param name="pid">ProfileID</param>
        /// <param name="request">リクエストデータ</param>
        /// <param name="outPtr">レスポンスを指したポインタ</param>
        /// <param name="outSize">レスポンスサイズ</param>
        /// <returns>レスポンスデータ。レスポンスサイズが規定と異なった場合はdefault値が返る。</returns>
        internal T_RES GenericSession<T_REQ, T_RES>(string url, int pid, T_REQ request, out IntPtr outPtr, out uint outSize)
            where T_REQ : IRequestContainer
            where T_RES : IResponseContainer
        {
            T_RES response;

            //request.hton();

            Session(url, pid, request, out outPtr, out outSize);

            if (outSize != Marshal.SizeOf(typeof(T_RES)))
            {
                return default(T_RES);
                //throw new Exception("レスポンスサイズが不正です。\n" + outSize + " < " + Marshal.SizeOf(typeof(T_RES)));
            }

            response = (T_RES)Marshal.PtrToStructure(outPtr, typeof(T_RES));
            response.ntoh();

            return response;
        }
    }
}
