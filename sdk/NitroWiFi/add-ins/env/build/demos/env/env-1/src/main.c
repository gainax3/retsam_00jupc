/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - ENV - demos - env-1
  File:     main.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  2005/10/14 00:33:06  kitase_hirotake
  fix ENV_Init

  Revision 1.8  2005/08/31 09:44:48  yada
  applied for multi env array

  Revision 1.7  2005/08/23 07:03:52  yada
  add demo about ENV_SearchByPartialName()

  Revision 1.6  2005/08/23 01:07:37  yada
  fix for changing ENV_GetXXX spec

  Revision 1.5  2005/08/22 12:05:26  yada
  consider for const

  Revision 1.4  2005/08/22 11:35:58  yada
  temporary

  Revision 1.3  2005/08/18 12:06:16  yada
  add ENV_SearchByType

  Revision 1.2  2005/08/17 11:17:53  yada
  fix a little

  Revision 1.1  2005/08/17 06:31:07  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi/env.h>
//================================================================================

ENVResource myResources0[] =
{
	//---- end mark
	//ENV_RESOURCE_START,
	"xxx.member",	ENV_U32( 777 ),
	ENV_RESOURCE_END
};

ENVResource myResources[] =
{
	//---- end mark
	//ENV_RESOURCE_START,

	"test.U64_1",	ENV_U64( 30 ),
	"test.U64_2",	ENV_U64( -30 ),
	"test.U64_3",	ENV_U64( 0xf2345f67abcd00e0 ),

	"test.S64_1",	ENV_S64( 30 ),
	"test.S64_2",	ENV_S64( -30 ),
	"test.S64_3",	ENV_S64( 0xf2345f67abcd00e0 ),

	"test.s1",		ENV_STRING( "abcdef" ),
	"test.s2",		ENV_STRING( "\0\0\0" ),

	"test.b1",		ENV_BINARY( "abcdef" ),
	"test.b2",		ENV_BINARY( "\0\0\0" ),

	"TESTPREFIX.STR_1",		ENV_STRING( "www.nintendo.co.jp" ),
	"TESTPREFIX.BIN_1",		ENV_BINARY( "test@nintendo.co.jp\0" ),

	"TESTPREFIX.U32_1",		ENV_U32( 1 ),
	"TESTPREFIX.U32_2",		ENV_U32( 0x12345 ),
	"TESTPREFIX.U32_3",		ENV_U32( 0xfedcba98 ),
	"TESTPREFIX.U32_4",		ENV_U32( 0x80000000 ),

	"TESTPREFIX.S32_1",		ENV_S32( 12345 ),
	"TESTPREFIX.S32_2",		ENV_S32( -8000 ),
	"TESTPREFIX.S32_3",		ENV_S32( 3000000000 ),
	"TESTPREFIX.S32_4",		ENV_S32( -3000000000 ),

	"TESTPREFIX.U16_1",		ENV_U16( 1000 ),
	"TESTPREFIX.U16_2",		ENV_U16( -1234 ),
	"TESTPREFIX.U16_3",		ENV_U16( 65536 ),
	"TESTPREFIX.U16_4",		ENV_U16( 65537 ),

	"TESTPREFIX.S16_1",		ENV_S16( 1000 ),
	"TESTPREFIX.S16_2",		ENV_S16( -1234 ),
	"TESTPREFIX.S16_3",		ENV_S16( 65536 ),
	"TESTPREFIX.S16_4",		ENV_S16( 65537 ),

	"TESTPREFIX.BOOL_1",	ENV_BOOL( TRUE ),
	"TESTPREFIX.BOOL_2",	ENV_BOOL( FALSE ),
	"TESTPREFIX.BOOL_3",	ENV_BOOL( 1234 ),

	"class1.member1",		ENV_U32( 0 ),
	"class2.member1",		ENV_U32( 0 ),
	"class3.member1",		ENV_U32( 0 ),
	"class4.member11",		ENV_U32( 0 ),
	"class5.member1.1",		ENV_U32( 0 ),
	"class6.member2.member1",	ENV_U32( 0 ),

	//---- end mark
	ENV_RESOURCE_END
};

ENVResource* resourceArray[]=
{
   myResources0, myResources, NULL
};
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <string.h>
void NitroMain(void)
{
    OS_Init();
    OS_InitThread();

	//---- declaration of using ENV system
	ENV_Init();
	ENV_AppendResourceSet( myResources );

	//---- DEMO 1 : search resources whose class is 'test'
	OS_Printf( "==== search resources whose class is 'test'\n" );
	{
		ENVResourceIter iter;
		ENVResource*    res;

		ENV_InitIter( &iter );

		while( (res = ENV_SearchByClass( &iter, "test" ) ) )
		{
			OS_Printf( "found [%s]\n", res->name );
		}
	}

	//---- DEMO 2 : search resources whose member is 'member1'
	OS_Printf( "\n==== search resources whose class is 'member1'\n" );
	{
		ENVResourceIter iter;
		ENVResource*    res;

		ENV_InitIter( &iter );

		while( (res = ENV_SearchByMember( &iter, "member1" ) ) )
		{
			OS_Printf( "found [%s]\n", res->name );
		}
	}

	//---- DEMO 3 : search resources whose type is u32
	OS_Printf( "\n==== search resources whose type is u32\n" );
	{
		ENVResourceIter iter;
		ENVResource*    res;

		ENV_InitIter( &iter );

		while( (res = ENV_SearchByType( &iter, ENV_RESTYPE_U32 ) ) )
		{
			OS_Printf( "found [%s]\n", res->name );
		}
	}

	//---- DEMO 4 : search resources whose name contains 'member'
	OS_Printf( "\n==== search resources whose name contains 'member'\n" );
	{
		ENVResourceIter iter;
		ENVResource*    res;

		ENV_InitIter( &iter );

		while( (res = ENV_SearchByPartialName( &iter, "member" ) ) )
		{
			OS_Printf( "found [%s]\n", res->name );
		}
	}

	//---- DEMO 5 : show difference of string data and binary data.
	OS_Printf( "\n==== show difference of string data and binary data\n" );
	OS_Printf( "- sizeof(\"abcdef\")       = %d\n", sizeof( "abcdef" ) );
	OS_Printf( "- strlen(\"abcdef\")       = %d\n", strlen( "abcdef" ) );
	OS_Printf( "- ENV_GetSize(\"test.s1\") = %d\n", ENV_GetSize("test.s1") );
	OS_Printf( "- ENV_GetSize(\"test.b1\") = %d\n", ENV_GetSize("test.b1") );
	OS_Printf( "\n" );

	OS_Printf( "- sizeof(\"\\0\\0\\0\")       = %d\n", sizeof( "\0\0\0" ) );
	OS_Printf( "- strlen(\"\\0\\0\\0\")       = %d\n", strlen( "\0\0\0" ) );
	OS_Printf( "- ENV_GetSize(\"test.s2\") = %d\n", ENV_GetSize("test.s2") );
	OS_Printf( "- ENV_GetSize(\"test.b2\") = %d\n", ENV_GetSize("test.b2") );
	OS_Printf( "\n" );

	//---- DEMO 6 : get 64 bit value
	OS_Printf( "\n==== get 64 bit value\n" );
	{
		u64 retVal;
		(void)ENV_GetU64( "test.U64_1", &retVal );
		OS_Printf( "ENV_GetU64(\"test.U64_1\") = [%llx]\n", retVal );
		(void)ENV_GetU64( "test.U64_2", &retVal );
		OS_Printf( "ENV_GetU64(\"test.U64_2\") = [%llx]\n", retVal );
		(void)ENV_GetU64( "test.U64_3", &retVal );
		OS_Printf( "ENV_GetU64(\"test.U64_3\") = [%llx]\n", retVal );
		(void)ENV_GetU64( "test.U64_4", &retVal );
		OS_Printf( "ENV_GetU64(\"test.U64_4\") = [%llx]\n", retVal );
		OS_Printf( "\n" );
	}

	{
		s64 retVal;
		(void)ENV_GetS64( "test.S64_1", &retVal );
		OS_Printf( "ENV_GetS64(\"test.S64_1\") = [%lld]\n", retVal );
		(void)ENV_GetS64( "test.S64_2", &retVal );
		OS_Printf( "ENV_GetS64(\"test.S64_2\") = [%lld]\n", retVal );
		(void)ENV_GetS64( "test.S64_3", &retVal );
		OS_Printf( "ENV_GetS64(\"test.S64_3\") = [%lld]\n", retVal );
		(void)ENV_GetS64( "test.S64_4", &retVal );
		OS_Printf( "ENV_GetS64(\"test.S64_4\") = [%lld]\n", retVal );
		OS_Printf( "\n" );
	}

	//---- DEMO 7 : omit class name
	OS_Printf( "\n==== omit class name\n" );
	ENV_SetClass( "TESTPREFIX" );
	{
		char* retVal1;
		void* retVal2;
		(void)ENV_GetString( ".STR_1", &retVal1 );
		OS_Printf( "ENV_String(\".STR_1\") = [%s]\n", retVal1 );
		(void)ENV_GetBinary( ".BIN_1", &retVal2 );
		OS_Printf( "ENV_Binary(\".BIN_1\") = [%s]\n", retVal2 );
		OS_Printf( "\n" );
	}

	{
		u32 retVal;
		(void)ENV_GetU32( ".U32_1", &retVal );
		OS_Printf( "ENV_GetU32(\".U32_1\") = [%x]\n", retVal );
		(void)ENV_GetU32( ".U32_2", &retVal );
		OS_Printf( "ENV_GetU32(\".U32_2\") = [%x]\n", retVal );
		(void)ENV_GetU32( ".U32_3", &retVal );
		OS_Printf( "ENV_GetU32(\".U32_3\") = [%x]\n", retVal );
		(void)ENV_GetU32( ".U32_4", &retVal );
		OS_Printf( "ENV_GetU32(\".U32_4\") = [%x]\n", retVal );
		OS_Printf( "\n" );
	}

	{
		s32 retVal;
		(void)ENV_GetS32( ".S32_1", &retVal );
		OS_Printf( "ENV_GetU32(\".S32_1\") = [%d]\n", retVal );
		(void)ENV_GetS32( ".S32_2", &retVal );
		OS_Printf( "ENV_GetU32(\".S32_2\") = [%d]\n", retVal );
		(void)ENV_GetS32( ".S32_3", &retVal );
		OS_Printf( "ENV_GetU32(\".S32_3\") = [%d]\n", retVal );
		(void)ENV_GetS32( ".S32_4", &retVal );
		OS_Printf( "ENV_GetU32(\".S32_4\") = [%d]\n", retVal );
		OS_Printf( "\n" );
	}

	{
		u16 retVal;
		(void)ENV_GetU16( ".U16_1", &retVal );
		OS_Printf( "ENV_GetU16(\".U16_1\") = [%x]\n", retVal );
		(void)ENV_GetU16( ".U16_2", &retVal );
		OS_Printf( "ENV_GetU16(\".U16_2\") = [%x]\n", retVal );
		(void)ENV_GetU16( ".U16_3", &retVal );
		OS_Printf( "ENV_GetU16(\".U16_3\") = [%x]\n", retVal );
		(void)ENV_GetU16( ".U16_4", &retVal );
		OS_Printf( "ENV_GetU16(\".U16_4\") = [%x]\n", retVal );
		OS_Printf( "\n" );
	}

	{
		s16 retVal;
		(void)ENV_GetS16( ".S16_1", &retVal );
		OS_Printf( "ENV_GetU16(\".S16_1\") = [%d]\n", retVal );
		(void)ENV_GetS16( ".S16_2", &retVal );
		OS_Printf( "ENV_GetU16(\".S16_2\") = [%d]\n", retVal );
		(void)ENV_GetS16( ".S16_3", &retVal );
		OS_Printf( "ENV_GetU16(\".S16_3\") = [%d]\n", retVal );
		(void)ENV_GetS16( ".S16_4", &retVal );
		OS_Printf( "ENV_GetU16(\".S16_4\") = [%d]\n", retVal );
		OS_Printf( "\n" );
	}

	{
		BOOL retVal;
		(void)ENV_GetBOOL( ".BOOL_1", &retVal );
		OS_Printf( "ENV_GetBOOL(\".BOOL_1\") = [%x]\n", retVal );
		(void)ENV_GetBOOL( ".BOOL_2", &retVal );
		OS_Printf( "ENV_GetBOOL(\".BOOL_2\") = [%x]\n", retVal );
		(void)ENV_GetBOOL( ".BOOL_3", &retVal );
		OS_Printf( "ENV_GetBOOL(\".BOOL_3\") = [%x]\n", retVal );
	}


	//---- DEMO 8 : read value as different type from described type" );
	OS_Printf( "\n==== read value as different type from described type\n" );
	{
		BOOL retBOOL;
		s32 retS32;
		u32 retU32;
		u16 retU16;
		(void)ENV_GetBOOL( ".S32_1", &retBOOL );
		OS_Printf( "read s32(12345) as BOOL : %d\n", retBOOL );
		(void)ENV_GetS32( ".S16_2", &retS32 );
		OS_Printf( "read s16(-1234) as s32  : %d\n", retS32 );
		(void)ENV_GetS32( ".S16_4", &retS32 );
		OS_Printf( "read s16(65537) as s32  : %d\n", retS32 );
		(void)ENV_GetU32( ".S16_2", &retU32 );
		OS_Printf( "read s16(-1234) as u32  : 0x%x\n", retU32 );
		(void)ENV_GetU16( "test.U64_3", &retU16 );
		OS_Printf( "read u64(0xf234567abcd00e0) as u16 : 0x%x\n", retU16 );
		OS_Printf( "\n" );
	}

    OS_Printf( "==== Finish sample.\n" );
    OS_Terminate();
}

/*====== End of main.c ======*/
