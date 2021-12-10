//==============================================================================
/**
 * @file	email_tool.c
 * @brief	Eメール：ツール類
 * @author	matsuda
 * @date	2007.10.22(月)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "libdpw/dpw_tr.h"
#include "gflib/strcode.h"
#include "system/procsys.h"
#include "savedata/config.h"
#include "gflib/strbuf.h"
#include "gflib/strbuf_family.h"

#include "application/email_main.h"

//==============================================================================
//	ASCIIコード変換テーブル
//==============================================================================
///STRCODE, ASCIIコード変換テーブル(Eメールで使用する文字のみ)
static const struct{
	u16 str_code;
	char ascii_code;
	u8 padding;
}AsciiConvertTbl[] = {
	// MatchComment: use h_n constants instead of n constants, also only keep 0-9
	{h_n0_,'0'},
	{h_n1_,'1'},
	{h_n2_,'2'},
	{h_n3_,'3'},
	{h_n4_,'4'},
	{h_n5_,'5'},
	{h_n6_,'6'},
	{h_n7_,'7'},
	{h_n8_,'8'},
	{h_n9_,'9'},
};


//--------------------------------------------------------------
/**
 * @brief   STRCODEをASCIIコードに変換
 *
 * @param   src			変換元メッセージ
 * @param   dest		変換後のASCIIコード代入先
 * @param   heap_id		ヒープID
 */
//--------------------------------------------------------------
void Email_Strcode_to_Ascii(STRBUF *src, char *dest, int heap_id)
{
    static const char prefix[] = "w";
    static const char suffix[] = "@wii.com";
    int spos;
	STRCODE *code;
	int pos = 0, i;

    for(spos = 0; prefix[spos]; pos++) {
        dest[pos] = prefix[spos++];
    }

	code = sys_AllocMemory(heap_id, sizeof(STRCODE) * EMAIL_ADDRESS_LEN_SIZE);
	
	STRBUF_GetStringCode(src, code, EMAIL_ADDRESS_LEN_SIZE);

	spos = 0;
	while(1){
		if(code[spos] == EOM_){
			dest[pos] = '\0';
			break;
		}
		for(i = 0; i < NELEMS(AsciiConvertTbl); i++){
			if(AsciiConvertTbl[i].str_code == code[spos]){
				dest[pos] = AsciiConvertTbl[i].ascii_code;
				break;
			}
		}
		GF_ASSERT(i < NELEMS(AsciiConvertTbl));	//ヒットしない文字が含まれている
		pos++;
		spos++;
	}

    for(spos = 0; suffix[spos]; pos++) {
        dest[pos] = suffix[spos++];
    }
    dest[pos] = '\0';
	OS_Printf( "mail address %s\n", dest );
	
	sys_FreeMemoryEz(code);
}
//--------------------------------------------------------------
/**
 * @brief   ASCIIコードをSTRCODEに変換
 *
 * @param   src			変換元メッセージ
 * @param   dest		変換後のSTRCODE代入先
 * @param   heap_id		ヒープID
 */
//--------------------------------------------------------------
void Email_Ascii_to_Strcode(char *src, STRBUF *dest, int heap_id)
{
	STRCODE *code;
	int pos = 0, i;
	
	code = sys_AllocMemory(heap_id, sizeof(STRCODE) * EMAIL_ADDRESS_LEN_SIZE);
	
	while(1){
		if(src[pos] == '\0'){
			code[pos] = EOM_;
			break;
		}
		for(i = 0; i < NELEMS(AsciiConvertTbl); i++){
			if(AsciiConvertTbl[i].ascii_code == src[pos]){
				code[pos] = AsciiConvertTbl[i].str_code;
				break;
			}
		}
		GF_ASSERT(i < NELEMS(AsciiConvertTbl));	//ヒットしない文字が含まれている
		pos++;
	}
	
	STRBUF_SetStringCode(dest, code);
	sys_FreeMemoryEz(code);
}

BOOL ov98_2249A80(STRBUF *strbuf, int heap_id)
{
	STRCODE* code;
	BOOL     ret = TRUE;
	int      i;

	code = sys_AllocMemory(heap_id, sizeof(STRCODE) * EMAIL_ADDRESS_LEN_SIZE);
	STRBUF_GetStringCode( strbuf, code, EMAIL_ADDRESS_LEN_SIZE );

	for(i = 0; code[i] != EOM_; i++){
		if(code[i] != h_n0_){
			ret = FALSE;
			break;
		}
	}
	sys_FreeMemoryEz( code );
	return ret;
}

void ov98_2249ACC(const char *address, u32 *dest, int heap_id)
{
	int      i,j;
	int      k=0;
	const char *p;

	for(i = 0; i < 4; i++){
		dest[i] = 0;
		for(j = 0; j < 4; j++){
			while(address[k] < '0' || address[k] > '9'){
				if(address[k] == '\0'){
					GF_ASSERT_MSG( address[k] != '\0', "Unexpected end of message" );
					break;
				}
				k++;
			}
			p = &address[k];
			dest[i] *= 10;
			dest[i] += *p - '0';
			k++;
		}
	}

	(void)heap_id;
}
