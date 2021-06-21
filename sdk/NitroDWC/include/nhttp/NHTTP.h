/*--------------------------------------------------------------------------
  Project:  NHTTP
  File:     NHTTP.h

  Copyright 2006 Nintendo.  All rights reserved.

  $Log: NHTTP.h,v $
  Revision 1.13  2007/02/21 04:23:18  takayama
  DlLibs_20070116 と同期。
  ただし、NHTTP_SetCAChain関数の引数名の変更(calength->cabuiltins)は引継ぎ。

  Revision 1.11  2006/06/23 06:54:22  aida
  replaced DWC_ND_COUNTBUFFER_LENGTH by DWC_ND_RECVBUF_MIN
  added CA

  Revision 1.10  2006/06/02 01:05:01  aida
  modified conditional operator

  Revision 1.9  2006/05/17 05:45:00  aida
  modified macro name

  Revision 1.8  2006/05/12 05:49:03  aida
  modified macro name

  Revision 1.7  2006/04/25 05:06:20  aida
  changed copyright description

  Revision 1.6  2006/04/17 06:53:54  aida
  changed k-option to -kkv

  Revision 1.5  2006/03/28 09:38:51  aida
  no message

  $NoKeywords: $
 *-------------------------------------------------------------------------*/
#ifndef NHTTP_NHTTP_H
#define NHTTP_NHTTP_H

#if ((defined WIN32) || (defined _WIN32))

#pragma once
#define BOOL int
#define TRUE 1
#define FALSE 0
#define SDK_ASSERT(XX)	_ASSERT(XX)
typedef unsigned long u32;
typedef struct _SOCCaInfo {
	char* caname;
	int casize;
	unsigned char* cabody;
	int caexpsize;
	unsigned char* caexp;
} SOCCaInfo, CPSCaInfo;
typedef struct _SOCSslConnection {
	char *server_name;				// server name to match
	int (*auth_callback)(int, struct _CPSSslConnection *, int);
	SOCCaInfo **ca_info;
	int ca_builtins;
} SOCSslConnection;
#include <stdlib.h>
#include <crtdbg.h>

#else

#ifndef _NITRO
#define _NITRO
#endif
#include <nitro.h>
#include <nitroWiFi.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////

// 通信スレッドのスタックの大きさ
#define NHTTP_COMMTHREAD_STACK	(8*1024)	// [byte]

// HTTP,HTTPSのTCPポート番号
#define NHTTP_HTTP_PORT			80
#define NHTTP_HTTPS_PORT		443

// HTTPリクエストヘッダに添付するバージョン文字列    GET / HTTP/1.1 とか
#define NHTTP_STR_HTTPVER		"1.1"	

// エンティティヘッダの初期受信バッファの大きさ (仕様で定義)
#define NHTTP_HDRRECVBUF_INILEN			1024	// [byte]

// エンティティヘッダの受信バッファを拡張する際のブロック長 (仕様で定義)
#define NHTTP_HDRRECVBUF_BLOCKSHIFT	9
#define NHTTP_HDRRECVBUF_BLOCKLEN		(1<<NHTTP_HDRRECVBUF_BLOCKSHIFT)	// [byte]
#define NHTTP_HDRRECVBUF_BLOCKMASK	(NHTTP_HDRRECVBUF_BLOCKLEN-1)


typedef enum {
	NHTTP_ERROR_NONE,          // 0
	NHTTP_ERROR_ALLOC,         // 1
	NHTTP_ERROR_TOOMANYREQ,    // 2
	NHTTP_ERROR_SOCKET,        // 3
	NHTTP_ERROR_DNS,           // 4
	NHTTP_ERROR_CONNECT,       // 5
	NHTTP_ERROR_BUFFULL,       // 6
	NHTTP_ERROR_HTTPPARSE,     // 7
	NHTTP_ERROR_CANCELED,      // 8
	NHTTP_ERROR_NITROSDK,      // 9
	NHTTP_ERROR_NITROWIFI,     // 10
	NHTTP_ERROR_UNKNOWN,       // 11
	NHTTP_ERROR_MAX
} NHTTPError;

typedef enum {
	NHTTP_REQMETHOD_GET,
	NHTTP_REQMETHOD_POST,
	NHTTP_REQMETHOD_HEAD,
	NHTTP_REQMETHOD_MAX
} NHTTPReqMethod;

typedef struct _NHTTPRes  NHTTPRes;
typedef struct _NHTTPReq  NHTTPReq;
typedef void* (*NHTTPAlloc)( u32 size, int align );
typedef void (*NHTTPFree)( void* ptr );
typedef void (*NHTTPCleanupCallback)( void );
typedef void (*NHTTPReqCallback)( NHTTPError error, NHTTPRes* res, void* param );


BOOL NHTTP_Startup( NHTTPAlloc alloc, NHTTPFree free, u32 threadprio );
void NHTTP_CleanupAsync( NHTTPCleanupCallback callback );
NHTTPError NHTTP_GetError(void);

NHTTPReq* NHTTP_CreateRequest( char* url, NHTTPReqMethod method, char* buf, u32 len, NHTTPReqCallback callback, void* param );
void NHTTP_DestroyRequest( NHTTPReq* req );
BOOL NHTTP_AddHeaderField( NHTTPReq* req, char* label, char* value );
BOOL NHTTP_AddPostDataAscii( NHTTPReq* req, char* label, char* value );
BOOL NHTTP_AddPostDataBinary( NHTTPReq* req, char* label, char* value, u32 length );
BOOL NHTTP_SetCAChain( NHTTPReq* req, SOCCaInfo** cainfo, int cabuiltins );

int NHTTP_SendRequestAsync( NHTTPReq* req );
BOOL NHTTP_CancelRequestAsync( int id );
BOOL NHTTP_GetProgress( u32* received, u32* contentlen );
void NHTTP_DestroyResponse( NHTTPRes* res );
int NHTTP_GetHeaderField( NHTTPRes* res, char* label, char** value );
int NHTTP_GetHeaderAll( NHTTPRes* res, char** value );
int NHTTP_GetBodyAll( NHTTPRes* res, char** value );


#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////

#endif