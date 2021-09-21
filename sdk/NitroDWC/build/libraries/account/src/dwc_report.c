#include <nitro.h>
#ifndef NITRODWC_NOGS
#include <nonport.h>
#include <gsiDebug.h>
#endif

#include <base/dwc_report.h>


#ifndef SDK_FINALROM
/** --------------------------------------------------------------------
  内部変数
  ----------------------------------------------------------------------*/
static u32          sReportLevel;

#ifdef GSI_COMMON_DEBUG
/** --------------------------------------------------------------------
  GameSpyのデバッグ表示コールバック関数
  ----------------------------------------------------------------------*/
static void
DWCs_GsDebugCallback( GSIDebugCategory theCat, GSIDebugType theType,
                      GSIDebugLevel theLevel, const char * theTokenStr,
                      va_list theParamList )
{
#pragma unused(theLevel)

    DWC_Printf( DWC_REPORTFLAG_GAMESPY,
#if 1  // 時間も表示
                "%d [%s][%s] ",
                (u32)OS_TicksToMilliSeconds(OS_GetTick()),
#else  // カテゴリとタイプのみ表示
                "[%s][%s] ",
#endif
                gGSIDebugCatStrings[theCat], 
                gGSIDebugTypeStrings[theType] );

    OS_TVPrintf( theTokenStr, theParamList );
}
#endif

/** --------------------------------------------------------------------
  dwcのデバッグレポートレベルを調整する関数
  ----------------------------------------------------------------------*/
void
DWC_SetReportLevel( u32 level )
{
    sReportLevel = level;

#ifdef GSI_COMMON_DEBUG
    // GameSpyのデバッグ表示も行う場合は、GameSpy用のデバッグ出力設定を行う
    if (level & DWC_REPORTFLAG_GAMESPY){
        gsSetDebugCallback(DWCs_GsDebugCallback);
        //gsSetDebugLevel(GSIDebugCat_NatNeg, GSIDebugType_All, GSIDebugLevel_Verbose);
        //gsSetDebugLevel(GSIDebugCat_QR2, GSIDebugType_All, GSIDebugLevel_Verbose);
        //gsSetDebugLevel(GSIDebugCat_SB, GSIDebugType_All, GSIDebugLevel_Verbose);
    }
#endif
}

/** --------------------------------------------------------------------
  dwcのデバッグ用レポート関数
  ----------------------------------------------------------------------*/
void
DWC_Printf( u32 level, const char* fmt, ... )
{
    if ( level & sReportLevel )
    {
        va_list  vlist;

        switch ( level )
        {
        case DWC_REPORTFLAG_INFO:        OS_PutString( "DWC_INFO:     " ); break;
        case DWC_REPORTFLAG_ERROR:       OS_PutString( "++DWC_ERROR:  " ); break;
        case DWC_REPORTFLAG_DEBUG:       OS_PutString( "DWC_DEBUG:    " ); break;
        case DWC_REPORTFLAG_WARNING:     OS_PutString( "DWC_WARN:     " ); break;
        case DWC_REPORTFLAG_ACHECK:      OS_PutString( "DWC_ACHECK:   " ); break;
        case DWC_REPORTFLAG_LOGIN:       OS_PutString( "DWC_LOGIN:    " ); break;
        case DWC_REPORTFLAG_MATCH_NN:    OS_PutString( "DWC_MATCH_NN: " ); break;
        case DWC_REPORTFLAG_MATCH_GT2:   OS_PutString( "DWC_MATCH_GT2:" ); break;
        case DWC_REPORTFLAG_TRANSPORT:   OS_PutString( "DWC_TRANSPORT:" ); break;
        case DWC_REPORTFLAG_QR2_REQ:     OS_PutString( "DWC_QR2_REQ:  " ); break;
        case DWC_REPORTFLAG_SB_UPDATE:   OS_PutString( "DWC_SB_UPDATE:" ); break;
        case DWC_REPORTFLAG_SEND_INFO:   OS_PutString( "DWC_SEND:     " ); break;
        case DWC_REPORTFLAG_RECV_INFO:   OS_PutString( "DWC_RECV:     " ); break;
        case DWC_REPORTFLAG_UPDATE_SV:   OS_PutString( "DWC_UPDATE_SV:" ); break;
        case DWC_REPORTFLAG_CONNECTINET: OS_PutString( "DWC_CONNECTINET:" ); break;
        case DWC_REPORTFLAG_AUTH:        OS_PutString( "DWC_AUTH:"      ); break;
        case DWC_REPORTFLAG_AC:          OS_PutString( "DWC_AC:"        ); break;
        case DWC_REPORTFLAG_BM:          OS_PutString( "DWC_BM:"        ); break;
        case DWC_REPORTFLAG_UTIL:        OS_PutString( "DWC_UTIL:"      ); break;
        case DWC_REPORTFLAG_GAMESPY:     OS_PutString( "DWC_GAMESPY:  " ); break;
        default:                         OS_PutString( "DWC_UNKNOWN:  " ); break;
        }

        va_start( vlist, fmt );
        OS_TVPrintf( fmt, vlist );
        va_end( vlist );
    }
}


#endif /* SDK_FINALROM */
