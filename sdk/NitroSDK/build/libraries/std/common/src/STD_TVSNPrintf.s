	.extern string_put_char
	.extern string_fill_char
	.extern string_put_string
	.extern _ull_div

	.macro arm_func_start name
	.balign 4, 0
	.global \name
	.arm
	.endm

	.text

	arm_func_start STD_TVSNPrintf
STD_TVSNPrintf: // 0x020D2C5C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sb, r2
	str r1, [sp, #0xc]
	str r0, [sp, #0x14]
	str r0, [sp, #0x10]
	ldrsb r0, [sb]
	str r1, [sp]
	mov fp, r3
	cmp r0, #0
	beq _020D3488
_020D2C88:
	ldrsb r1, [sb]
	and r0, r1, #0xff
	eor r0, r0, #0x20
	sub r0, r0, #0xa1
	cmp r0, #0x3c
	bhs _020D2CC4
	add r0, sp, #0xc
	bl string_put_char
	ldrsb r1, [sb, #1]!
	cmp r1, #0
	beq _020D347C
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020D347C
_020D2CC4:
	cmp r1, #0x25
	beq _020D2CDC
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020D347C
_020D2CDC:
	mov r5, #0
	mov sl, r5
	mov r2, sb
	sub r6, r5, #1
	mov r0, #0xa
	mov r3, #0x57
_020D2CF4:
	ldrsb r4, [sb, #1]!
	cmp r4, #0x20
	bgt _020D2D08
	beq _020D2D44
	b _020D2D5C
_020D2D08:
	cmp r4, #0x30
	bgt _020D2D5C
	cmp r4, #0x2b
	blt _020D2D5C
	beq _020D2D30
	cmp r4, #0x2d
	beq _020D2D4C
	cmp r4, #0x30
	beq _020D2D54
	b _020D2D5C
_020D2D30:
	ldrsb r1, [sb, #-1]
	cmp r1, #0x20
	bne _020D2D5C
	orr r5, r5, #2
	b _020D2CF4
_020D2D44:
	orr r5, r5, #1
	b _020D2CF4
_020D2D4C:
	orr r5, r5, #8
	b _020D2CF4
_020D2D54:
	orr r5, r5, #0x10
	b _020D2CF4
_020D2D5C:
	cmp r4, #0x2a
	bne _020D2D80
	add fp, fp, #4
	ldr sl, [fp, #-4]
	add sb, sb, #1
	cmp sl, #0
	rsblt sl, sl, #0
	orrlt r5, r5, #8
	b _020D2DA8
_020D2D80:
	mov r1, #0xa
	b _020D2D94
_020D2D88:
	ldrsb r4, [sb], #1
	mla r4, sl, r1, r4
	sub sl, r4, #0x30
_020D2D94:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020D2DA8
	cmp r4, #0x39
	ble _020D2D88
_020D2DA8:
	ldrsb r1, [sb]
	cmp r1, #0x2e
	bne _020D2E04
	ldrsb r1, [sb, #1]!
	mov r6, #0
	cmp r1, #0x2a
	bne _020D2DDC
	add fp, fp, #4
	ldr r6, [fp, #-4]
	add sb, sb, #1
	cmp r6, #0
	mvnlt r6, #0
	b _020D2E04
_020D2DDC:
	mov r1, #0xa
	b _020D2DF0
_020D2DE4:
	ldrsb r4, [sb], #1
	mla r4, r6, r1, r4
	sub r6, r4, #0x30
_020D2DF0:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020D2E04
	cmp r4, #0x39
	ble _020D2DE4
_020D2E04:
	ldrsb r1, [sb]
	cmp r1, #0x68
	beq _020D2E1C
	cmp r1, #0x6c
	beq _020D2E34
	b _020D2E48
_020D2E1C:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x68
	orrne r5, r5, #0x40
	addeq sb, sb, #1
	orreq r5, r5, #0x100
	b _020D2E48
_020D2E34:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x6c
	orrne r5, r5, #0x20
	addeq sb, sb, #1
	orreq r5, r5, #0x80
_020D2E48:
	ldrsb r1, [sb]
	cmp r1, #0x69
	bgt _020D2E8C
	cmp r1, #0x63
	blt _020D2E70
	beq _020D2EFC
	cmp r1, #0x64
	cmpne r1, #0x69
	beq _020D30B4
	b _020D3098
_020D2E70:
	cmp r1, #0x25
	bgt _020D2E80
	beq _020D307C
	b _020D3098
_020D2E80:
	cmp r1, #0x58
	beq _020D2EE8
	b _020D3098
_020D2E8C:
	cmp r1, #0x6e
	bgt _020D2E9C
	beq _020D3024
	b _020D3098
_020D2E9C:
	sub r1, r1, #0x6f
	cmp r1, #9
	addls pc, pc, r1, lsl #2
	b _020D3098
_020D2EAC: // jump table
	b _020D2ED4 // case 0
	b _020D2EF0 // case 1
	b _020D3098 // case 2
	b _020D3098 // case 3
	b _020D2F70 // case 4
	b _020D3098 // case 5
	b _020D2EE0 // case 6
	b _020D3098 // case 7
	b _020D3098 // case 8
	b _020D30AC // case 9
_020D2ED4:
	orr r5, r5, #0x1000
	mov r0, #8
	b _020D30B4
_020D2EE0:
	orr r5, r5, #0x1000
	b _020D30B4
_020D2EE8:
	mov r3, #0x37
	b _020D30AC
_020D2EF0:
	orr r5, r5, #4
	mov r6, #8
	b _020D30AC
_020D2EFC:
	cmp r6, #0
	bge _020D3098
	add fp, fp, #4
	tst r5, #8
	ldr r4, [fp, #-4]
	beq _020D2F38
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
	add r0, sp, #0xc
	sub r2, sl, #1
	mov r1, #0x20
	bl string_fill_char
	b _020D2F68
_020D2F38:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	sub r2, sl, #1
	bl string_fill_char
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
_020D2F68:
	add sb, sb, #1
	b _020D347C
_020D2F70:
	add fp, fp, #4
	cmp r6, #0
	ldr r7, [fp, #-4]
	mov r4, #0
	bge _020D2FA8
	ldrsb r0, [r7]
	cmp r0, #0
	beq _020D2FBC
_020D2F90:
	add r4, r4, #1
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020D2F90
	b _020D2FBC
_020D2FA4:
	add r4, r4, #1
_020D2FA8:
	cmp r4, r6
	bge _020D2FBC
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020D2FA4
_020D2FBC:
	tst r5, #8
	sub sl, sl, r4
	beq _020D2FEC
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
	b _020D301C
_020D2FEC:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r2, sl
	mov r1, r1, asr #0x18
	bl string_fill_char
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
_020D301C:
	add sb, sb, #1
	b _020D347C
_020D3024:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	tst r5, #0x100
	sub r2, r1, r0
	bne _020D3074
	tst r5, #0x40
	beq _020D3050
	add fp, fp, #4
	ldr r0, [fp, #-4]
	strh r2, [r0]
	b _020D3074
_020D3050:
	add fp, fp, #4
	tst r5, #0x80
	ldreq r0, [fp, #-4]
	streq r2, [r0]
	beq _020D3074
	ldr r0, [fp, #-4]
	mov r1, r2, asr #0x1f
	str r2, [r0]
	str r1, [r0, #4]
_020D3074:
	add sb, sb, #1
	b _020D347C
_020D307C:
	add r0, r2, #1
	cmp r0, sb
	bne _020D3098
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020D347C
_020D3098:
	mov r1, r2
	add r0, sp, #0xc
	sub r2, sb, r2
	bl string_put_string
	b _020D347C
_020D30AC:
	orr r5, r5, #0x1000
	mov r0, #0x10
_020D30B4:
	tst r5, #8
	bicne r5, r5, #0x10
	cmp r6, #0
	bicge r5, r5, #0x10
	movlt r6, #1
	mov r4, #0
	tst r5, #0x1000
	beq _020D3174
	tst r5, #0x100
	beq _020D30EC
	add fp, fp, #4
	ldrb r7, [fp, #-4]
	mov r1, #0
	b _020D3124
_020D30EC:
	tst r5, #0x40
	beq _020D3104
	add fp, fp, #4
	ldrh r7, [fp, #-4]
	mov r1, #0
	b _020D3124
_020D3104:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, #0
	beq _020D3124
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020D3124:
	bic r5, r5, #3
	tst r5, #4
	beq _020D3234
	cmp r0, #0x10
	bne _020D315C
	cmp r1, #0
	cmpeq r7, #0
	beq _020D3234
	add r4, r3, #0x21
	mov r2, #0x30
	strb r4, [sp, #8]
	strb r2, [sp, #9]
	mov r4, #2
	b _020D3234
_020D315C:
	cmp r0, #8
	bne _020D3234
	mov r2, #0x30
	strb r2, [sp, #8]
	mov r4, #1
	b _020D3234
_020D3174:
	tst r5, #0x100
	beq _020D318C
	add fp, fp, #4
	ldrsb r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020D31C4
_020D318C:
	tst r5, #0x40
	beq _020D31A4
	add fp, fp, #4
	ldrsh r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020D31C4
_020D31A4:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, r7, asr #0x1f
	beq _020D31C4
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020D31C4:
	mov ip, #0
	and r2, ip, #0
	and r8, r1, #0x80000000
	cmp r2, #0
	cmpeq r8, #0
	beq _020D31FC
	mvn r4, r7
	mov r2, #0x2d
	mvn r1, r1
	strb r2, [sp, #8]
	adds r7, r4, #1
	adc r1, r1, ip
	mov r4, #1
	b _020D3234
_020D31FC:
	cmp r1, ip
	cmpeq r7, ip
	cmpeq r6, #0
	beq _020D3234
	tst r5, #2
	beq _020D3224
	mov r2, #0x2b
	strb r2, [sp, #8]
	mov r4, #1
	b _020D3234
_020D3224:
	tst r5, #1
	movne r2, #0x20
	strneb r2, [sp, #8]
	movne r4, #1
_020D3234:
	cmp r0, #8
	mov r8, #0
	beq _020D3254
	cmp r0, #0xa
	beq _020D32A0
	cmp r0, #0x10
	beq _020D3338
	b _020D3384
_020D3254:
	cmp r1, r8
	cmpeq r7, r8
	beq _020D3384
	add r2, sp, #0x18
	mov r0, r8
	mov lr, #7
	mov ip, r8
_020D3270:
	and r3, r7, lr
	add r3, r3, #0x30
	strb r3, [r2, r8]
	mov r3, r1, lsr #3
	cmp r3, r0
	mov r7, r7, lsr #3
	orr r7, r7, r1, lsl #29
	mov r1, r3
	cmpeq r7, ip
	add r8, r8, #1
	bne _020D3270
	b _020D3384
_020D32A0:
	mov r0, r8
	cmp r0, r8
	cmpeq r1, r8
	bne _020D32EC
	cmp r7, #0
	beq _020D3384
	ldr ip, _020D34D4 // =0xCCCCCCCD
	add r3, sp, #0x18
	mov r2, #0xa
_020D32C4:
	umull r1, r0, r7, ip
	movs r0, r0, lsr #3
	mul r1, r0, r2
	sub r1, r7, r1
	mov r7, r0
	add r0, r1, #0x30
	strb r0, [r3, r8]
	add r8, r8, #1
	bne _020D32C4
	b _020D3384
_020D32EC:
	cmp r1, r8
	cmpeq r7, r8
	beq _020D3384
_020D32F8:
	mov r0, r7
	mov r2, #0xa
	mov r3, #0
	bl _ull_div
	mov r2, #0xa
	umull r3, r2, r0, r2
	subs r2, r7, r3
	add r3, r2, #0x30
	add r2, sp, #0x18
	strb r3, [r2, r8]
	cmp r1, #0
	cmpeq r0, #0
	mov r7, r0
	add r8, r8, #1
	bne _020D32F8
	b _020D3384
_020D3338:
	cmp r1, r8
	cmpeq r7, r8
	beq _020D3384
	add ip, sp, #0x18
	mov lr, #0xf
_020D334C:
	and r2, r7, lr
	mov r7, r7, lsr #4
	mov r0, r1, lsr #4
	orr r7, r7, r1, lsl #28
	cmp r2, #0xa
	mov r1, r0
	addlt r0, r2, #0x30
	addge r0, r2, r3
	strb r0, [ip, r8]
	mov r0, #0
	cmp r1, r0
	cmpeq r7, r0
	add r8, r8, #1
	bne _020D334C
_020D3384:
	cmp r4, #0
	ble _020D33AC
	ldrsb r0, [sp, #8]
	cmp r0, #0x30
	bne _020D33AC
	add r0, sp, #0x18
	mov r1, #0x30
	strb r1, [r0, r8]
	add r8, r8, #1
	mov r4, #0
_020D33AC:
	tst r5, #0x10
	sub r6, r6, r8
	beq _020D33C8
	sub r0, sl, r8
	sub r0, r0, r4
	cmp r6, r0
	movlt r6, r0
_020D33C8:
	cmp r6, #0
	subgt sl, sl, r6
	add r0, r4, r8
	sub sl, sl, r0
	ands r0, r5, #8
	str r0, [sp, #4]
	bne _020D33F4
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020D33F4:
	cmp r4, #0
	ble _020D3420
	add r0, sp, #8
	add r5, r0, r4
	add r7, sp, #0xc
_020D3408:
	ldrsb r1, [r5, #-1]!
	mov r0, r7
	sub r4, r4, #1
	bl string_put_char
	cmp r4, #0
	bgt _020D3408
_020D3420:
	add r0, sp, #0xc
	mov r2, r6
	mov r1, #0x30
	bl string_fill_char
	cmp r8, #0
	ble _020D345C
	add r0, sp, #0x18
	add r5, r0, r8
	add r4, sp, #0xc
_020D3444:
	ldrsb r1, [r5, #-1]!
	mov r0, r4
	sub r8, r8, #1
	bl string_put_char
	cmp r8, #0
	bgt _020D3444
_020D345C:
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020D3478
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020D3478:
	add sb, sb, #1
_020D347C:
	ldrsb r0, [sb]
	cmp r0, #0
	bne _020D2C88
_020D3488:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _020D34A4
	ldr r0, [sp, #0x10]
	mov r1, #0
	strb r1, [r0]
	b _020D34C0
_020D34A4:
	ldr r0, [sp]
	cmp r0, #0
	beq _020D34C0
	ldr r1, [sp, #0x14]
	mov r2, #0
	add r0, r1, r0
	strb r2, [r0, #-1]
_020D34C0:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	sub r0, r1, r0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020D34D4: .word 0xCCCCCCCD
