//---------------------------------------------------------------------------
//  Project:  irisSDK - Emulator Utility - tools
//  File:     setLogRege.js
//
//  Copyright 2003,2004 Nintendo.  All rights reserved.
//
//  These coded instructions, statements, and computer programs contain
//  proprietary information of Nintendo of America Inc. and/or Nintendo
//  Company Ltd., and are protected by Federal copyright law.  They may
//  not be disclosed to third parties or copied or duplicated in any form,
//  in whole or in part, without the prior written consent of Nintendo.
//
//  $Log: setLogRege.js,v $
//  Revision 1.3  2004/03/30 08:29:23  yasu
//  IRIS->NITRO
//
//  Revision 1.2  2004/02/06 04:09:40  kitani_toshikazu
//  Append Setting DbgFile out check box.
//
//  Revision 1.1  2004/02/06 02:57:27  kitani_toshikazu
//  Initial check in.
//
//
//  $NoKeywords: $
//---------------------------------------------------------------------------


//
// Change the IS-Emu's debug log file name, by rewriting its setting regeistry.
//


// 
// constants 
//
var REG_ISEMU_DGB_FILENAME = "HKEY_CURRENT_USER\\Software\\INTELLIGENT SYSTEMS\\IS-NITRO-EMULATOR\\Dbg Print Section\\File Name";
var REG_ISEMU_DGB_FILEFLAG = "HKEY_CURRENT_USER\\Software\\INTELLIGENT SYSTEMS\\IS-NITRO-EMULATOR\\Dbg Print Section\\File";

//
// 
//
objArgs = WScript.Arguments;
var logFile = objArgs(0);
WScript.Echo( "LogFilePath = " + logFile );


//
// write rege.
//
var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.RegWrite ( REG_ISEMU_DGB_FILEFLAG, 1, "REG_BINARY" );
WshShell.RegWrite ( REG_ISEMU_DGB_FILENAME, logFile );
