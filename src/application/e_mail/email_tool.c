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

static const char sOv98_2249D1C[] = "w";
static const char sOv98_2249D20[] = "@wii.com";

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
#ifdef NONEQUIVALENT
void Email_Strcode_to_Ascii(STRBUF *src, char *dest, int heap_id)
{
	STRCODE *code;
	int pos = 0, i;
	
	code = sys_AllocMemory(heap_id, sizeof(STRCODE) * EMAIL_ADDRESS_LEN_SIZE);
	
	STRBUF_GetStringCode(src, code, EMAIL_ADDRESS_LEN_SIZE);
	while(1){
		if(code[pos] == EOM_){
			dest[pos] = '\0';
			break;
		}
		for(i = 0; i < NELEMS(AsciiConvertTbl); i++){
			if(AsciiConvertTbl[i].str_code == code[pos]){
				dest[pos] = AsciiConvertTbl[i].ascii_code;
				break;
			}
		}
		GF_ASSERT(i < NELEMS(AsciiConvertTbl));	//ヒットしない文字が含まれている
		pos++;
	}
	
	OS_Printf( "mail address %s\n", dest );
	
	sys_FreeMemoryEz(code);
}
#else
asm void Email_Strcode_to_Ascii(STRBUF *src, char *dest, int heap_id)
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #8
	add r7, r2, #0
	mov r4, #0
	add r5, r1, #0
	ldr r2, =sOv98_2249D1C // _02249A70
	str r0, [sp]
	add r1, r4, #0
	add r0, r2, #0
	add r3, r4, #0
_022499DC:
	add r6, r1, #0
	ldrsb r6, [r0, r6]
	add r2, r2, #1
	add r1, r1, #1
	strb r6, [r5, r4]
	ldrsb r6, [r2, r3]
	add r4, r4, #1
	cmp r6, #0
	bne _022499DC
	add r0, r7, #0
	mov r1, #0xc8
	bl sys_AllocMemory
	str r0, [sp, #4]
	ldr r0, [sp]
	ldr r1, [sp, #4]
	mov r2, #0x64
	bl STRBUF_GetStringCode
	ldr r7, [sp, #4]
	mov r6, #0
_02249A06:
	ldrh r1, [r7]
	ldr r0, =0x0000FFFF // _02249A74
	cmp r1, r0
	bne _02249A14
	mov r2, #0
	strb r2, [r5, r4]
	b _02249A4A
_02249A14:
	ldr r2, [sp, #4]
	lsl r3, r6, #1
	ldrh r3, [r2, r3]
	ldr r1, =AsciiConvertTbl // _02249A78
	mov r0, #0
_02249A1E:
	ldrh r2, [r1]
	cmp r2, r3
	bne _02249A32
	ldr r1, =AsciiConvertTbl // _02249A78
	lsl r2, r0, #2
	add r2, r1, r2
	mov r1, #2
	ldrsb r1, [r2, r1]
	strb r1, [r5, r4]
	b _02249A3A
_02249A32:
	add r0, r0, #1
	add r1, r1, #4
	cmp r0, #0xa
	blo _02249A1E
_02249A3A:
	cmp r0, #0xa
	blo _02249A42
	bl GF_AssertFailedWarningCall
_02249A42:
	add r4, r4, #1
	add r7, r7, #2
	add r6, r6, #1
	b _02249A06
_02249A4A:
	ldr r1, =sOv98_2249D20 // _02249A7C
	add r3, r2, #0
	add r0, r1, #0
_02249A50:
	add r6, r2, #0
	ldrsb r6, [r0, r6]
	add r1, r1, #1
	add r2, r2, #1
	strb r6, [r5, r4]
	ldrsb r6, [r1, r3]
	add r4, r4, #1
	cmp r6, #0
	bne _02249A50
	mov r0, #0
	strb r0, [r5, r4]
	ldr r0, [sp, #4]
	bl sys_FreeMemoryEz
	add sp, #8
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _02249A70: .4byte sOv98_2249D1C
// _02249A74: .4byte 0x0000FFFF
// _02249A78: .4byte AsciiConvertTbl
// _02249A7C: .4byte sOv98_2249D20
}
#endif

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

// NONMATCHING
asm void ov98_2249A80(void)
{
	push {r4, r5, r6, lr}
	add r6, r0, #0
	add r0, r1, #0
	mov r1, #0xc8
	mov r5, #1
	bl sys_AllocMemory
	add r4, r0, #0
	add r0, r6, #0
	add r1, r4, #0
	mov r2, #0x64
	bl STRBUF_GetStringCode
	ldrh r1, [r4]
	ldr r0, =0x0000FFFF // _02249AC4
	cmp r1, r0
	beq _02249AB8
	ldr r1, =0x00000121 // _02249AC8
	add r3, r4, #0
_02249AA6:
	ldrh r2, [r3]
	cmp r2, r1
	beq _02249AB0
	mov r5, #0
	b _02249AB8
_02249AB0:
	add r3, r3, #2
	ldrh r2, [r3]
	cmp r2, r0
	bne _02249AA6
_02249AB8:
	add r0, r4, #0
	bl sys_FreeMemoryEz
	add r0, r5, #0
	pop {r4, r5, r6, pc}
	nop
// _02249AC4: .4byte 0x0000FFFF
// _02249AC8: .4byte 0x00000121
}

// NONMATCHING
asm void ov98_2249ACC(void)
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #8
	mov r4, #0
	add r5, r0, #0
	add r7, r1, #0
	str r4, [sp, #4]
_02249AD8:
	mov r0, #0
	str r0, [r7, #0]
	str r0, [sp]
	b _02249B14
_02249AE0:
	cmp r0, #0
	bne _02249AEC
	bne _02249AFA
	bl GF_AssertFailedWarningCall
	b _02249AFA
_02249AEC:
	add r4, r4, #1
_02249AEE:
	ldrsb r0, [r5, r4]
	add r6, r5, r4
	cmp r0, #0x30
	blt _02249AE0
	cmp r0, #0x39
	bgt _02249AE0
_02249AFA:
	ldr r1, [r7, #0]
	mov r0, #0xa
	mul r0, r1
	str r0, [r7, #0]
	mov r1, #0
	ldrsb r1, [r6, r1]
	add r4, r4, #1
	sub r1, #0x30
	add r0, r0, r1
	str r0, [r7, #0]
	ldr r0, [sp]
	add r0, r0, #1
	str r0, [sp]
_02249B14:
	ldr r0, [sp]
	cmp r0, #4
	blt _02249AEE
	ldr r0, [sp, #4]
	add r7, r7, #4
	add r0, r0, #1
	str r0, [sp, #4]
	cmp r0, #4
	blt _02249AD8
	add sp, #8
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
}
