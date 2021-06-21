#ifndef DWC_GHTTP_H_
#define DWC_GHTTP_H_

#define GHTTP_EXTENDEDERROR 
// gamespy modules
#include "ghttp/ghttp.h"

#ifdef __cplusplus
extern "C" {
#endif

// 通信状態
typedef enum  
{
    DWCGHTTPFalse = -1,                              // Invalid processing
	DWCGHTTPHostLookup = GHTTPHostLookup,			 // 0, Resolving hostname to IP. 
	DWCGHTTPConnecting = GHTTPConnecting,			 // Waiting for socket connect to complete. 
	DWCGHTTPSecuringSession = GHTTPSecuringSession,	 // Setup secure channel.
	DWCGHTTPSendingRequest = GHTTPSendingRequest,	 // Sending the request. 
	DWCGHTTPPosting = GHTTPPosting,					 // Posting data (skipped if not posting). 
	DWCGHTTPWaiting = GHTTPWaiting,					 // Waiting for a response. 
	DWCGHTTPReceivingStatus = GHTTPReceivingStatus,	 // Receiving the response status. 
	DWCGHTTPReceivingHeaders = GHTTPReceivingHeaders,// Receiving the headers. 
	DWCGHTTPReceivingFile = GHTTPReceivingFile,		 // Receiving the file. 

    //命名規則ミスのための処置
    DWC_GHTTP_FALSE = -1,                               // Invalid processing
	DWC_GHTTP_HOST_LOOKUP = GHTTPHostLookup,			// 0, Resolving hostname to IP. 
	DWC_GHTTP_CONNECTING = GHTTPConnecting,			    // Waiting for socket connect to complete. 
	DWC_GHTTP_SECURING_SESSION = GHTTPSecuringSession,  // Setup secure channel.
	DWC_GHTTP_SENDING_REQUEST = GHTTPSendingRequest,	// Sending the request. 
	DWC_GHTTP_POSTING = GHTTPPosting,				    // Posting data (skipped if not posting). 
	DWC_GHTTP_WAITING = GHTTPWaiting,				    // Waiting for a response. 
	DWC_GHTTP_RECEIVING_STATUS = GHTTPReceivingStatus,  // Receiving the response status. 
	DWC_GHTTP_RECEIVING_HEADERS = GHTTPReceivingHeaders,// Receiving the headers. 
	DWC_GHTTP_RECEIVING_FILE = GHTTPReceivingFile	    // Receiving the file. 
} DWCGHTTPState;

// リクエストエラー値
typedef enum
{
    // Possible Error values returned from GHTTP functions.
	DWCGHTTPErrorStart = GHTTPErrorStart,                // -8, 
	DWCGHTTPFailedToOpenFile = GHTTPFailedToOpenFile,    // Failed to open file
	DWCGHTTPInvalidPost = GHTTPInvalidPost,              // Invalid post
	DWCGHTTPInsufficientMemory = GHTTPInsufficientMemory,// Insufficient memory
	DWCGHTTPInvalidFileName = GHTTPInvalidFileName,	     // Invalid filename
	DWCGHTTPInvalidBufferSize = GHTTPInvalidBufferSize,  // Invalid buffer size
	DWCGHTTPInvalidURL = GHTTPInvalidURL,                // Invalid URL
	DWCGHTTPUnspecifiedError = GHTTPUnspecifiedError,    // -1, Unspecified error

    //命名規則ミスのための処置
	DWC_GHTTP_ERROR_START = GHTTPErrorStart,                // -8,
    DWC_GHTTP_IN_ERROR = DWC_GHTTP_ERROR_START,             // In error condition
	DWC_GHTTP_FAILED_TO_OPEN_FILE = GHTTPFailedToOpenFile,   // Failed to open file
	DWC_GHTTP_INVALID_POST = GHTTPInvalidPost,              // Invalid post
	DWC_GHTTP_INSUFFICIENT_MEMORY = GHTTPInsufficientMemory,// Insufficient memory
	DWC_GHTTP_INVALID_FILE_NAME = GHTTPInvalidFileName,	    // Invalid filename
	DWC_GHTTP_INVALID_BUFFER_SIZE = GHTTPInvalidBufferSize, // Invalid buffer size
	DWC_GHTTP_INVALID_URL = GHTTPInvalidURL,                // Invalid URL
	DWC_GHTTP_UNSPECIFIED_ERROR = GHTTPUnspecifiedError     // -1, Unspecified error


} DWCGHTTPRequestError;

// 通信結果
typedef enum
{
    // The result of an HTTP request.
	DWCGHTTPSuccess = GHTTPSuccess,                  // 0, Successfully retrieved file.
	DWCGHTTPOutOfMemory = GHTTPOutOfMemory,          // A memory allocation failed.
	DWCGHTTPBufferOverflow = GHTTPBufferOverflow,    // The user-supplied buffer was too small to hold the file.
	DWCGHTTPParseURLFailed = GHTTPParseURLFailed,    // There was an error parsing the URL.
	DWCGHTTPHostLookupFailed = GHTTPHostLookupFailed,// Failed looking up the hostname.
	DWCGHTTPSocketFailed = GHTTPSocketFailed,        // Failed to create/initialize/read/write a socket.
	DWCGHTTPConnectFailed = GHTTPConnectFailed,      // Failed connecting to the http server.
	DWCGHTTPBadResponse = GHTTPBadResponse,          // Error understanding a response from the server.
	DWCGHTTPRequestRejected = GHTTPRequestRejected,  // The request has been rejected by the server.
	DWCGHTTPUnauthorized = GHTTPUnauthorized,        // Not authorized to get the file.
	DWCGHTTPForbidden = GHTTPForbidden,              // The server has refused to send the file.
	DWCGHTTPFileNotFound = GHTTPFileNotFound,        // Failed to find the file on the server.
	DWCGHTTPServerError = GHTTPServerError,          // The server has encountered an internal error.
	DWCGHTTPFileWriteFailed = GHTTPFileWriteFailed,  // An error occured writing to the local file (for ghttpSaveFile[Ex]).
	DWCGHTTPFileReadFailed = GHTTPFileReadFailed,    // There was an error reading from a local file (for posting files from disk).
	DWCGHTTPFileIncomplete = GHTTPFileIncomplete,    // Download started but was interrupted.  Only reported if file size is known.
	DWCGHTTPFileToBig = GHTTPFileToBig,              // The file is to big to be downloaded (size exceeds range of interal data types)
	DWCGHTTPEncryptionError = GHTTPEncryptionError,  // Error with encryption engine.
    DWCGHTTPNum,

    DWCGHTTPMemoryError = DWCGHTTPNum + 1,           // DWC memory allocation failed.

    //命名規則ミスのための処置
	DWC_GHTTP_SUCCESS = GHTTPSuccess,                    // 0, Successfully retrieved file.
	DWC_GHTTP_OUT_OF_MEMORY = GHTTPOutOfMemory,          // A memory allocation failed.
	DWC_GHTTP_BUFFER_OVERFLOW = GHTTPBufferOverflow,     // The user-supplied buffer was too small to hold the file.
	DWC_GHTTP_PARSE_URL_FAILED = GHTTPParseURLFailed,    // There was an error parsing the URL.
	DWC_GHTTP_HOST_LOOKUP_FAILED = GHTTPHostLookupFailed,// Failed looking up the hostname.
	DWC_GHTTP_SOCKET_FAILED = GHTTPSocketFailed,         // Failed to create/initialize/read/write a socket.
	DWC_GHTTP_CONNECT_FAILED = GHTTPConnectFailed,       // Failed connecting to the http server.
	DWC_GHTTP_BAD_RESPONSE = GHTTPBadResponse,           // Error understanding a response from the server.
	DWC_GHTTP_REQUEST_REJECTED = GHTTPRequestRejected,   // The request has been rejected by the server.
	DWC_GHTTP_UNAUTHORIZED = GHTTPUnauthorized,          // Not authorized to get the file.
	DWC_GHTTP_FORBIDDEN = GHTTPForbidden,                // The server has refused to send the file.
	DWC_GHTTP_FILE_NOT_FOUND = GHTTPFileNotFound,        // Failed to find the file on the server.
	DWC_GHTTP_SERVER_ERROR = GHTTPServerError,           // The server has encountered an internal error.
	DWC_GHTTP_FILE_WRITE_FAILED = GHTTPFileWriteFailed,  // An error occured writing to the local file (for ghttpSaveFile[Ex]).
	DWC_GHTTP_FILE_READ_FAILED = GHTTPFileReadFailed,    // There was an error reading from a local file (for posting files from disk).
	DWC_GHTTP_FILE_INCOMPLETE = GHTTPFileIncomplete,     // Download started but was interrupted.  Only reported if file size is known.
	DWC_GHTTP_FILE_TOO_BIG = GHTTPFileToBig,             // The file is to big to be downloaded (size exceeds range of interal data types)
	DWC_GHTTP_ENCRYPTION_ERROR = GHTTPEncryptionError,   // Error with encryption engine.
    DWC_GHTTP_NUM,

    DWC_GHTTP_MEMORY_ERROR = DWC_GHTTP_NUM + 1           // DWC memory allocation failed.


} DWCGHTTPResult;
 
// 関数型宣言
//---------------------------------------------------------
typedef void (*DWC_GHTTPCompletedCALLBACK)(const char* buf, int len, DWCGHTTPResult result, void* param);
typedef void (*DWC_GHTTPProgressCALLBACK)(DWCGHTTPState state, const char* buf, int len, int bytesReceived, int totalSize, void* param);
typedef GHTTPRequest DWCGHTTPRequest;
typedef GHTTPPost    DWCGHTTPPost;

//命名規則ミスのための処置
typedef void (*DWCGHTTPCompletedCallback)(const char* buf, int len, DWCGHTTPResult result, void* param);
typedef void (*DWCGHTTPProgressCallback)(DWCGHTTPState state, const char* buf, int len, int bytesReceived, int totalSize, void* param);

// 構造体宣言
//---------------------------------------------------------
typedef struct
{
    void* param;
    DWCGHTTPCompletedCallback completedCallback;
    DWCGHTTPProgressCallback  progressCallback;
    BOOL buffer_clear;
}DWCGHTTPParam;

/**
 * ライブラリの初期化と終了
 */
BOOL	DWC_InitGHTTP(  const char* gamename  );
BOOL	DWC_ShutdownGHTTP( void );

/**
 * Think
 */
BOOL	DWC_ProcessGHTTP( void );

/**
 * postオブジェクトの作成
 */
void    DWC_GHTTPNewPost( DWCGHTTPPost* post );
BOOL    DWC_GHTTPPostAddString( DWCGHTTPPost* post, const char* key, const char* value );
BOOL    DWC_GHTTPPostAddFileFromMemory( DWCGHTTPPost *post, const char *key, const char *buffer, int bufferlen, const char *filename, const char *contentType );

/**
 * データの送受信
 */
int  	DWC_PostGHTTPData( const char* url, DWCGHTTPPost* post, DWCGHTTPCompletedCallback completedCallback, void* param );
int     DWC_GetGHTTPData( const char* url, DWCGHTTPCompletedCallback completedCallback, void* param);
int 	DWC_GetGHTTPDataEx( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param );
int 	DWC_GetGHTTPDataEx2( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPPost *post, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param );
void    DWC_CancelGHTTPRequest(int req);

/**
 * 通信状態の確認
 */
DWCGHTTPState DWC_GetGHTTPState( int req );

#ifdef __cplusplus
}
#endif


#endif
