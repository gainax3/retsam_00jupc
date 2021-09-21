//---------------------------------------------------------------------------
//  Project:  NitroSDK - Emulator Utility - tools
//  File:     runEmu.js
//
//  Copyright 2003,2004 Nintendo.  All rights reserved.
//
//  These coded instructions, statements, and computer programs contain
//  proprietary information of Nintendo of America Inc. and/or Nintendo
//  Company Ltd., and are protected by Federal copyright law.  They may
//  not be disclosed to third parties or copied or duplicated in any form,
//  in whole or in part, without the prior written consent of Nintendo.
//
//  $Log: runEmu.js,v $
//  Revision 1.3  2004/02/09 11:07:24  kitani_toshikazu
//  Work around IS-IRIS-EMU => IS=NITRO-EMU.
//
//  Revision 1.2  2004/02/05 12:21:14  yasu
//  change SDK prefix iris -> nitro
//
//  Revision 1.1  2004/02/04 11:03:51  kitani_toshikazu
//  Initial check in.
//
//
//  $NoKeywords: $
//---------------------------------------------------------------------------

//
// Auto test execution 
// 

//--------------------------------------
// constants difinitions 
var NITRO_EMU_EXE = "/X86/bin/ISNITROEMULATOR.exe";
var IDOL_TIME    = 10000;                   // ms


//--------------------------------------
// get exec bin file name 
objArgs = WScript.Arguments;
var fnameBin = objArgs(0);
WScript.Echo("input bin file = " + fnameBin );

var logfilePath = objArgs(1);
WScript.Echo("log out file = " + logfilePath );


//--------------------------------------
// run emu   
var WShell      = WScript.CreateObject("WScript.Shell");
var WshSysEnv   = WShell.Environment("SYSTEM");
var App         = WshSysEnv("IS_NITRO_DIR") + NITRO_EMU_EXE;
WScript.Echo( App );
var oExec = WShell.Exec( App + " " + fnameBin + " /logfile " + logfilePath );

//--------------------------------------
// wait for a while   
WScript.Sleep( IDOL_TIME );

oExec.Terminate();

