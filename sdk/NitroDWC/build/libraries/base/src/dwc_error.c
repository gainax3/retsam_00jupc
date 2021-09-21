#include <nitro.h>

#include <base/dwc_error.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
static DWCError stDwcLastError = DWC_ERROR_NONE;  // 最後のエラー
static int stDwcErrorCode = 0;  // 最後のエラーコード


//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  エラー取得関数
  引数　：errorCode エラーコード格納先ポインタ。必要なければNULLを渡せば良い。
  戻り値：エラー種別
  用途　：最後の通信エラー種別、エラーコードを取得する
 *---------------------------------------------------------------------------*/
DWCError DWC_GetLastError(int* errorCode)
{

    if (errorCode != NULL) *errorCode = stDwcErrorCode;
    
    return stDwcLastError;
}


// [arakit] main 051011
/*---------------------------------------------------------------------------*
  拡張版エラー取得関数
  引数　：errorCode エラーコード格納先ポインタ。必要なければNULLを渡せば良い。
          errorType エラー処理タイプ格納先ポインタ。必要なければNULLを渡せば良い。
  戻り値：エラー種別
  用途　：最後の通信エラー種別、エラーコード、エラー処理タイプを取得する
 *---------------------------------------------------------------------------*/
DWCError DWC_GetLastErrorEx(int* errorCode, DWCErrorType* errorType)
{

    if (errorCode != NULL) *errorCode = stDwcErrorCode;

    if (errorType != NULL){
        switch (stDwcLastError){
        case DWC_ERROR_AUTH_ANY:
        case DWC_ERROR_AUTH_OUT_OF_SERVICE:
        case DWC_ERROR_AUTH_STOP_SERVICE:
        case DWC_ERROR_AC_ANY:
        case DWC_ERROR_DISCONNECTED:
            *errorType = DWC_ETYPE_DISCONNECT;
            break;
        
        case DWC_ERROR_NETWORK:
            *errorType = DWC_ETYPE_SHUTDOWN_FM;
            break;
        
        case DWC_ERROR_GHTTP_ANY:
            *errorType = DWC_ETYPE_SHUTDOWN_GHTTP;
            break;
        
        case DWC_ERROR_FRIENDS_SHORTAGE:
        case DWC_ERROR_NOT_FRIEND_SERVER:
        case DWC_ERROR_MO_SC_CONNECT_BLOCK:
        case DWC_ERROR_SERVER_FULL:
            *errorType = DWC_ETYPE_LIGHT;
            break;
        
        case DWC_ERROR_DS_MEMORY_ANY:
        case DWC_ERROR_FATAL:
            *errorType = DWC_ETYPE_FATAL;
            break;
        
		case DWC_ERROR_ND_ANY:
			*errorType = DWC_ETYPE_SHUTDOWN_ND;
			break;
		
		case DWC_ERROR_ND_HTTP:
		case DWC_ERROR_SVL_HTTP:
        case DWC_ERROR_PROF_HTTP:
			*errorType = DWC_ETYPE_DISCONNECT;
			break;
		
		case DWC_ERROR_SVL_ANY:
		case DWC_ERROR_PROF_ANY:
			*errorType = DWC_ETYPE_SHOW_ERROR;
			break;
		
        case DWC_ERROR_NONE:
        default:
            *errorType = DWC_ETYPE_NO_ERROR;
            break;
        }
    }

    return stDwcLastError;
}
// [arakit] main 051011


/*---------------------------------------------------------------------------*
  エラークリア関数
  引数　：なし
  戻り値：なし
  用途　：通信エラーの記録をクリアする。Fatal Errorはクリアできない。
 *---------------------------------------------------------------------------*/
void DWC_ClearError(void)
{

    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERRORはクリア禁止
        stDwcLastError = DWC_ERROR_NONE;
        stDwcErrorCode = 0;
    }
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  エラーチェック関数
  引数　：なし
  戻り値：TRUE:エラーあり、FALSE:エラーなし
  用途　：通信エラーが発生したかどうかをチェックする
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsError(void)
{

    if (stDwcLastError != DWC_ERROR_NONE) return TRUE;
    else return FALSE;
}


/*---------------------------------------------------------------------------*
  エラーセット関数
  引数　：error エラー種別
          errorCode エラーの詳細を示すエラーコード
  戻り値：なし
  用途　：通信エラーの発生を記録する。Fatal Errorは上書きできない。
 *---------------------------------------------------------------------------*/
void DWCi_SetError(DWCError error, int errorCode)
{

    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERRORは上書き禁止
        stDwcLastError = error;
        stDwcErrorCode = errorCode;
    }

#ifndef SDK_FINALROM
    if ( error == DWC_ERROR_FATAL )
    {
        OS_TPrintf("FATALERROR_SET\n");
    }
#endif
}
