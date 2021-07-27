	.include "macros.inc"
	.include "global.inc"
	.section .text
	.balign 4, 0

	arm_func_start ov66_2234FC4
ov66_2234FC4: // 0x02234FC4
	ldr r0, _02234FD0 // =0x0225B6C4
	ldr r0, [r0]
	bx lr
	.align 2, 0
_02234FD0: .word 0x0225B6C4
	arm_func_end ov66_2234FC4

	arm_func_start PPW_LobbyInitializeAsync
PPW_LobbyInitializeAsync: // 0x02234FD4
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r8, [sp, #0x28]
	mov r7, r0
	mov r0, r8
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl DWC_CheckUserData
	cmp r0, #0
	beq _02235010
	mov r0, r8
	bl DWC_CheckHasProfile
	cmp r0, #0
	bne _0223501C
_02235010:
	add sp, sp, #0x10
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0223501C:
	ldr r1, [r8, #0x1c]
	ldr r0, [sp, #0x2c]
	str r1, [sp]
	ldr ip, [sp, #0x30]
	str r0, [sp, #4]
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	str ip, [sp, #8]
	mov r4, #1
	str r4, [sp, #0xc]
	bl ov66_2235058
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end PPW_LobbyInitializeAsync

	arm_func_start ov66_2235058
ov66_2235058: // 0x02235058
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x30
	ldr r4, _02235344 // =0x0225B6C4
	mov r8, r0
	ldr r0, [r4]
	mov r7, r1
	cmp r0, #0
	mov r6, r2
	mov r5, r3
	addne sp, sp, #0x30
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r3, _02235348 // =ov66_2239F84
	ldr r2, _0223534C // =ov66_223A1C8
	mov r1, #5
	ldr r0, _02235350 // =0x0000047C
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	str r1, [sp, #0x2c]
	bl FUN_02246290
	movs r4, r0
	beq _022351EC
	mov r0, #0
	str r0, [r4]
	str r0, [r4, #4]
	add lr, r4, #8
	mov ip, #4
_022350C4:
	ldmia r5!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs ip, ip, #1
	bne _022350C4
	ldr r1, [r5]
	ldr r0, [sp, #0x48]
	str r1, [lr]
	str r0, [r4, #0x4c]
	ldr r0, [sp, #0x54]
	str r6, [r4, #0x50]
	str r0, [r4, #0x54]
	mov r0, #0
	add r2, r4, #0x58
	mov r1, r0
_022350FC:
	str r1, [r2, r0, lsl #2]
	add r0, r0, #1
	cmp r0, #3
	blo _022350FC
	str r1, [r4, #0x64]
	str r1, [r4, #0x68]
	str r1, [r4, #0x6c]
	str r1, [r4, #0x70]
	mov r0, #5
	str r0, [r4, #0xb8]
	add r0, r4, #0xbc
	mov r3, #0
_0223512C:
	str r3, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _0223512C
	mov r0, #5
	str r0, [r4, #0xd0]
	add r0, r4, #0xd4
	mov r2, #0
_0223514C:
	str r2, [r0, r3, lsl #2]
	add r3, r3, #1
	cmp r3, #3
	blo _0223514C
	ldr r0, _02235354 // =0x02258EA0
	add r1, r4, #0xf0
	str r0, [r4, #0xe8]
	str r2, [r4, #0xec]
	str r2, [r4, #0xf0]
	str r1, [r4, #0xf4]
	ldr r0, _02235358 // =0x02258EB0
	add r1, sp, #8
	str r0, [r4, #0xf8]
	mov r0, #5
	str r0, [r4, #0xfc]
	str r2, [r4, #0x100]
	str r2, [r4, #0x104]
	add r0, sp, #0x14
	str r2, [r4, #0x108]
	bl DWC_GetDateTime
	add r0, sp, #0x14
	add r1, sp, #8
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x100]
	str r1, [r4, #0x104]
	mov r0, #1
	mov r2, #0
	str r0, [r4, #0x10c]
	str r2, [r4, #0x110]
	str r2, [r4, #0x114]
	str r2, [r4, #0x118]
	str r0, [r4, #0x11c]
	sub r0, r2, #1
	str r0, [r4, #0x124]
	str r2, [r4, #0x134]
	str r2, [r4, #0x138]
	add r1, r4, #0x138
	add r0, r4, #0x144
	str r1, [r4, #0x140]
	bl ov66_223CA1C
_022351EC:
	ldr r0, _02235344 // =0x0225B6C4
	cmp r4, #0
	str r4, [r0]
	addeq sp, sp, #0x30
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r4, [sp, #0x48]
	add r3, sp, #0x24
	mov r0, r8
	mov r1, r7
	mov r2, r6
	str r4, [sp]
	bl ov66_2244A18
	mov r4, r0
	ldr r1, _02235344 // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #4]
	cmp r4, #1
	ldr r1, [r1]
	beq _02235248
	cmp r4, #2
	moveq r0, #3
	b _0223524C
_02235248:
	mov r0, #2
_0223524C:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r4, #0
	beq _02235308
	ldr r0, _02235344 // =0x0225B6C4
	ldr r5, [r0]
	cmp r5, #0
	beq _022352FC
	beq _022352F0
	add r0, r5, #0x194
	bl ov66_223B620
	add r0, r5, #0x17c
	bl ov66_223A9FC
	add r0, r5, #0x170
	bl ov66_223B670
	add r0, r5, #0x134
	bl FUN_0223ABC4
	add r0, r5, #0x134
	bl ov66_223AD4C
	ldr r1, _02235354 // =0x02258EA0
	add r0, r5, #0xe8
	str r1, [r5, #0xe8]
	bl ov66_2241E54
	add r0, r5, #0x110
	bl ov66_223B8E8
	add r0, r5, #0xec
	bl ov66_223AA9C
	add r0, r5, #0xd4
	bl ov66_223BADC
	add r0, r5, #0xbc
	bl ov66_223BADC
	add r0, r5, #0x58
	bl ov66_223BADC
	cmp r5, #0
	beq _022352F0
	mov r0, #0
	mov r1, r5
	mov r2, r0
	bl DWC_Free
_022352F0:
	ldr r0, _02235344 // =0x0225B6C4
	mov r1, #0
	str r1, [r0]
_022352FC:
	add sp, sp, #0x30
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02235308:
	ldr r0, _02235344 // =0x0225B6C4
	ldr r1, [sp, #0x48]
	ldr r0, [r0]
	ldr r2, [sp, #0x4c]
	ldr r3, [sp, #0x50]
	add r0, r0, #0xe8
	bl ov66_2242008
	bl ov66_22374B4
	ldr r0, _02235344 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r0]
	mov r0, #0
	str r2, [r1]
	add sp, sp, #0x30
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02235344: .word 0x0225B6C4
_02235348: .word ov66_2239F84
_0223534C: .word ov66_223A1C8
_02235350: .word 0x0000047C
_02235354: .word 0x02258EA0
_02235358: .word 0x02258EB0
	arm_func_end ov66_2235058

	arm_func_start ov66_223535C
ov66_223535C: // 0x0223535C
	stmfd sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x18
	bl ov66_223B8E8
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223535C

	arm_func_start ov66_2235374
ov66_2235374: // 0x02235374
	stmfd sp!, {r4, lr}
	ldr r1, _022353A0 // =0x02258EA0
	mov r4, r0
	str r1, [r4]
	bl ov66_2241E54
	add r0, r4, #0x28
	bl ov66_223B8E8
	add r0, r4, #4
	bl ov66_223AA9C
	mov r0, r4
	ldmia sp!, {r4, pc}
	.align 2, 0
_022353A0: .word 0x02258EA0
	arm_func_end ov66_2235374

	arm_func_start PPW_LobbyShutdownAsync
PPW_LobbyShutdownAsync: // 0x022353A4
	stmfd sp!, {r3, lr}
	ldr r0, _02235424 // =0x0225B6C4
	ldr r1, [r0]
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, [r1]
	cmp r0, #9
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	add r0, r1, #0x134
	bl FUN_0223ABC4
	ldr r0, _02235424 // =0x0225B6C4
	mov r1, #9
	ldr r0, [r0]
	str r1, [r0]
	bl FUN_02244E98
	ldr r1, _02235424 // =0x0225B6C4
	mov r2, #0
	strb r2, [sp]
	cmp r0, #1
	ldr r3, [r1]
	beq _0223540C
	cmp r0, #2
	moveq r2, #3
	b _02235410
_0223540C:
	mov r2, #2
_02235410:
	cmp r2, #0
	strne r2, [r3, #4]
	movne r1, #0xa
	strne r1, [r3]
	ldmia sp!, {r3, pc}
	.align 2, 0
_02235424: .word 0x0225B6C4
	arm_func_end PPW_LobbyShutdownAsync

	arm_func_start PPW_LobbyProcess
PPW_LobbyProcess: // 0x02235428
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x6c
	ldr r0, _02235B34 // =0x0225B6D4
	ldr r7, [r0]
	mov r0, #0
	cmp r7, #0
	beq _022356C4
	str r0, [sp, #0x10]
	bl FUN_022584EC
	cmp r0, #0
	beq _02235648
	cmp r0, #1
	beq _02235534
	cmp r0, #8
	bne _0223568C
	add r0, sp, #0x10
	bl FUN_02258664
	mov r5, r0
	ldr r1, [sp, #0x10]
	mov r0, #0
	bl DWC_Alloc
	movs r4, r0
	beq _02235494
	ldr r2, [sp, #0x10]
	mov r0, r5
	mov r1, r4
	bl MI_CpuCopy8
_02235494:
	cmp r4, #0
	beq _022354DC
	ldr r6, [r7, #0x38]
	ldr r5, [sp, #0x10]
	bl FUN_022585F4
	ldr r2, _02235B38 // =0x0225B6D8
	ldr r0, _02235B34 // =0x0225B6D4
	ldr r8, [r2]
	ldr r3, [r0]
	mov r1, r4
	str r3, [r2]
	str r8, [r0]
	ldr r7, [r7, #0x34]
	mov r2, r5
	mov r3, r6
	mov r0, #1
	blx r7
	b _02235514
_022354DC:
	ldr r5, [r7, #0x38]
	bl FUN_022585F4
	ldr r3, _02235B38 // =0x0225B6D8
	mov r0, #0
	ldr r2, _02235B34 // =0x0225B6D4
	ldr r8, [r3]
	ldr r6, [r2]
	mov r1, r0
	str r6, [r3]
	str r8, [r2]
	ldr r6, [r7, #0x34]
	mov r2, r0
	mov r3, r5
	blx r6
_02235514:
	cmp r4, #0
	beq _0223552C
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_0223552C:
	mov r0, #1
	b _02235690
_02235534:
	bl DWC_ClearError
	ldr r0, [r7, #4]
	subs r0, r0, #1
	str r0, [r7, #4]
	beq _02235608
	bl FUN_022585F4
	ldr r0, [r7, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r7, #0xd
	ldrne r1, [r7, #0x14]
	ldr r0, [r7, #8]
	bl FUN_02258338
	ldr r0, [r7, #0x3c]
	ldr r4, [r7, #0x2c]
	cmp r0, #0
	ldr r0, [r7, #0x18]
	beq _022355B4
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r7, #0x19
	ldrne r5, [r7, #0x20]
	add r0, r7, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r7, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _022355E8
_022355B4:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r7, #0x19
	ldrne r5, [r7, #0x20]
	add r0, r7, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r7, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_022355E8:
	cmp r0, #0
	moveq r0, #1
	beq _022355FC
	bl FUN_022585F4
	mov r0, #0
_022355FC:
	cmp r0, #0
	movne r0, #0
	bne _02235690
_02235608:
	ldr r4, [r7, #0x38]
	bl FUN_022585F4
	ldr r3, _02235B38 // =0x0225B6D8
	mov r0, #0
	ldr r2, _02235B34 // =0x0225B6D4
	ldr r6, [r3]
	ldr r5, [r2]
	mov r1, r0
	str r5, [r3]
	str r6, [r2]
	ldr r5, [r7, #0x34]
	mov r2, r0
	mov r3, r4
	blx r5
	mov r0, #1
	b _02235690
_02235648:
	bl DWC_ClearError
	ldr r5, [r7, #0x38]
	bl FUN_022585F4
	mov r0, #0
	ldr r4, _02235B38 // =0x0225B6D8
	ldr r2, _02235B34 // =0x0225B6D4
	ldr r6, [r4]
	mov r3, r5
	ldr r5, [r2]
	mov r1, r0
	str r5, [r4]
	str r6, [r2]
	ldr r4, [r7, #0x34]
	mov r2, r0
	blx r4
	mov r0, #1
	b _02235690
_0223568C:
	mov r0, #0
_02235690:
	cmp r0, #0
	ldrne r0, _02235B38 // =0x0225B6D8
	ldrne r0, [r0]
	cmpne r0, #0
	beq _022356C0
	beq _022356B4
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022356B4:
	ldr r0, _02235B38 // =0x0225B6D8
	mov r1, #0
	str r1, [r0]
_022356C0:
	mov r0, #1
_022356C4:
	cmp r0, #0
	beq _022356E8
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r0, [r0]
	cmp r0, #0
	bne _02235700
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022356E8:
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x6c
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02235700:
	cmp r0, #0
	beq _02235724
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #0xa
	addeq sp, sp, #0x6c
	moveq r0, #0xa
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02235724:
	bl ov66_224477C
	cmp r0, #5
	addls pc, pc, r0, lsl #2
	b _0223582C
_02235734: // jump table
	b _0223574C // case 0
	b _0223582C // case 1
	b _0223582C // case 2
	b _022357F4 // case 3
	b _0223582C // case 4
	b _02235810 // case 5
_0223574C:
	bl PPW_LobbyShutdownAsync
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r4, [r0]
	cmp r4, #0
	beq _022357E8
	beq _022357DC
	add r0, r4, #0x194
	bl ov66_223B620
	add r0, r4, #0x17c
	bl ov66_223A9FC
	add r0, r4, #0x170
	bl ov66_223B670
	add r0, r4, #0x134
	bl FUN_0223ABC4
	add r0, r4, #0x134
	bl ov66_223AD4C
	ldr r1, _02235B40 // =0x02258EA0
	add r0, r4, #0xe8
	str r1, [r4, #0xe8]
	bl ov66_2241E54
	add r0, r4, #0x110
	bl ov66_223B8E8
	add r0, r4, #0xec
	bl ov66_223AA9C
	add r0, r4, #0xd4
	bl ov66_223BADC
	add r0, r4, #0xbc
	bl ov66_223BADC
	add r0, r4, #0x58
	bl ov66_223BADC
	cmp r4, #0
	beq _022357DC
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_022357DC:
	ldr r0, _02235B3C // =0x0225B6C4
	mov r1, #0
	str r1, [r0]
_022357E8:
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022357F4:
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1]
	cmp r0, #1
	moveq r0, #2
	streq r0, [r1]
	b _0223582C
_02235810:
	bl ov66_2244960
	ldr r1, _02235B3C // =0x0225B6C4
	cmp r0, #0
	ldr r1, [r1]
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
_0223582C:
	ldr r2, _02235B3C // =0x0225B6C4
	add r0, sp, #0x28
	add r1, sp, #0x38
	ldr r5, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x28
	add r1, sp, #0x38
	bl RTC_ConvertDateTimeToSecond
	ldr r2, [r5, #0x140]
	add r7, r5, #0x138
	mov r6, r0
	mov sb, r1
	str r2, [sp, #0xc]
	cmp r2, r7
	beq _022358FC
	mov fp, #1
	mov sl, #0
	add r4, sp, #0xc
_02235874:
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	subs r1, r6, r1
	ldr r8, [r2, #0x18]
	sbc r0, sb, r0
	ldr r3, [r2, #0x1c]
	subs r1, r8, r1
	sbcs r1, r3, r0
	bge _022358B8
	str r6, [r2, #0x10]
	str sb, [r2, #0x14]
	ldr r0, [r2, #0x20]
	ldr r1, [r2, #0x24]
	blx r1
	cmp r0, #0
	moveq r0, sl
	beq _022358BC
_022358B8:
	mov r0, fp
_022358BC:
	cmp r0, #0
	bne _022358E8
	ldr r8, [sp, #0xc]
	mov r0, r4
	bl ov66_223AD6C
	add r0, sp, #8
	mov r2, r8
	add r1, r5, #0x134
	str r8, [sp, #4]
	bl FUN_0223AB54
	b _022358F0
_022358E8:
	mov r0, r4
	bl ov66_223AD6C
_022358F0:
	ldr r2, [sp, #0xc]
	cmp r2, r7
	bne _02235874
_022358FC:
	add r0, r5, #0xe8
	bl ov66_2242460
	add r0, r5, #0x144
	bl ov66_223EE30
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1]
	cmp r0, #0xa
	addls pc, pc, r0, lsl #2
	b _02235B20
_02235924: // jump table
	b _02235950 // case 0
	b _02235B20 // case 1
	b _0223595C // case 2
	b _02235968 // case 3
	b _02235988 // case 4
	b _02235B20 // case 5
	b _022359E8 // case 6
	b _02235A50 // case 7
	b _02235B20 // case 8
	b _02235B20 // case 9
	b _02235B20 // case 10
_02235950:
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223595C:
	mov r0, #3
	str r0, [r1]
	b _02235B20
_02235968:
	ldr r0, [r1, #0x144]
	cmp r0, #2
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	movne r0, #4
	strne r0, [r1]
	b _02235B20
_02235988:
	ldr r1, _02235B44 // =0x02258FB0
	add r0, sp, #0x50
	bl ov66_223BA6C
	add r0, sp, #0x44
	add r2, sp, #0x50
	mov r1, #0
	bl ov66_22378A8
	ldr r0, [sp, #0x44]
	ldr r1, _02235B48 // =ov66_223A250
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x45
	ldrne r0, [sp, #0x4c]
	mov r2, #0
	bl FUN_0224591C
	add r0, sp, #0x44
	bl ov66_223BADC
	add r0, sp, #0x50
	bl ov66_223BADC
	ldr r0, _02235B3C // =0x0225B6C4
	mov r1, #5
	ldr r0, [r0]
	str r1, [r0]
	b _02235B20
_022359E8:
	bl ov66_2237BB4
	cmp r0, #0
	beq _02235A3C
	ldr r1, _02235B3C // =0x0225B6C4
	cmp r0, #1
	ldr r1, [r1]
	beq _02235A10
	cmp r0, #2
	beq _02235A18
	b _02235A20
_02235A10:
	mov r0, #2
	b _02235A24
_02235A18:
	mov r0, #3
	b _02235A24
_02235A20:
	mov r0, #0
_02235A24:
	cmp r0, #0
	beq _02235B20
	str r0, [r1, #4]
	mov r0, #0xa
	str r0, [r1]
	b _02235B20
_02235A3C:
	ldr r0, _02235B3C // =0x0225B6C4
	mov r1, #7
	ldr r0, [r0]
	str r1, [r0]
	b _02235B20
_02235A50:
	bl ov66_223859C
	cmp r0, #0
	beq _02235B20
	ldr r0, _02235B3C // =0x0225B6C4
	add r7, sp, #0x60
	ldr r4, [r0]
	mov r0, #0
	str r0, [r4, #0x10c]
	ldr r0, [r4, #0xfc]
	mov r6, #1
	str r0, [sp, #0x5c]
	add r0, r4, #0x100
	ldmia r0, {r2, r3}
	stmia r7, {r2, r3}
	ldr r1, [sp, #0x5c]
	str r6, [sp, #0x68]
	add r0, sp, #0x18
	str r1, [sp, #0x14]
	add r5, sp, #0x14
	ldmia r7, {r2, r3}
	stmia r0, {r2, r3}
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	str r6, [sp, #0x20]
	str r6, [sp, #0x24]
	bl memcmp
	cmp r0, #0
	moveq r0, r6
	movne r0, #0
	cmp r0, #0
	bne _02235AF8
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #0xfc]
	add r0, r4, #0x100
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x108]
	str r0, [r4, #0x10c]
_02235AF8:
	mov r0, #1
	ldr r1, _02235B3C // =0x0225B6C4
	str r0, [r4, #0x128]
	ldr r2, [r1]
	mov r3, #8
	str r3, [r2]
	ldr r2, [r1]
	mov r1, #0
	ldr r2, [r2, #8]
	blx r2
_02235B20:
	ldr r0, _02235B3C // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	add sp, sp, #0x6c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_02235B34: .word 0x0225B6D4
_02235B38: .word 0x0225B6D8
_02235B3C: .word 0x0225B6C4
_02235B40: .word 0x02258EA0
_02235B44: .word 0x02258FB0
_02235B48: .word ov66_223A250
	arm_func_end PPW_LobbyProcess

	arm_func_start ov66_2235B4C
ov66_2235B4C: // 0x02235B4C
	stmfd sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x28
	bl ov66_223B8E8
	add r0, r4, #0x18
	bl ov66_223BADC
	add r0, r4, #0xc
	bl ov66_223BADC
	cmp r4, #0
	beq _02235B84
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02235B84:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_2235B4C

	arm_func_start ov66_2235B8C
ov66_2235B8C: // 0x02235B8C
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov66_2235B8C

	arm_func_start PPW_LobbyJoinSubChannelAsync
PPW_LobbyJoinSubChannelAsync: // 0x02235BA0
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x54
	ldr r1, _02235D9C // =0x0225B6C4
	mov r4, r0
	ldr r1, [r1]
	cmp r1, #0
	addeq sp, sp, #0x54
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _02235BDC
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x54
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_02235BDC:
	cmp r1, #0
	beq _02235C00
	ldr r0, _02235D9C // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x54
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02235C00:
	cmp r1, #0
	beq _02235C28
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	addne sp, sp, #0x54
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02235C28:
	ldr r5, _02235DA0 // =ov66_2239B54
	ldr lr, _02235DA4 // =ov66_2239D60
	ldr ip, _02235DA8 // =ov66_2239FA4
	ldr r3, _02235DAC // =ov66_223A1E4
	ldr r2, _02235DB0 // =ov66_223A958
	add r0, sp, #0x24
	str r5, [sp, #0x3c]
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r3, [sp, #0x48]
	str r2, [sp, #0x4c]
	str r4, [sp, #0x50]
	bl ov66_2235DC4
	add r0, sp, #0x30
	add r2, sp, #0x24
	mov r1, r4
	bl ov66_22378A8
	add r0, sp, #0x24
	bl ov66_223BADC
	cmp r4, #4
	addls pc, pc, r4, lsl #2
	b _02235CAC
_02235C80: // jump table
	b _02235CAC // case 0
	b _02235C94 // case 1
	b _02235C94 // case 2
	b _02235C9C // case 3
	b _02235CA4 // case 4
_02235C94:
	ldr r2, _02235DB4 // =0x02258C4C
	b _02235CB0
_02235C9C:
	ldr r2, _02235DB8 // =0x02258C0C
	b _02235CB0
_02235CA4:
	ldr r2, _02235DBC // =0x02258C2C
	b _02235CB0
_02235CAC:
	mov r2, #0
_02235CB0:
	ldr r0, [sp, #0x30]
	ldr r1, _02235DC0 // =ov66_22399A8
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	add r3, sp, #0x3c
	stmia sp, {r1, r4}
	ldr r2, [r2, #0x1c]
	mov r1, #0
	bl FUN_02244F44
	mov r5, r0
	ldr r1, _02235D9C // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r5, #1
	ldr r1, [r1]
	beq _02235D04
	cmp r5, #2
	moveq r0, #3
	b _02235D08
_02235D04:
	mov r0, #2
_02235D08:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r5, #0
	beq _02235D34
	add r0, sp, #0x30
	bl ov66_223BADC
	add sp, sp, #0x54
	mov r0, r5
	ldmia sp!, {r4, r5, pc}
_02235D34:
	add r0, sp, #0x10
	str r4, [sp, #0xc]
	add r1, sp, #0x30
	add r4, sp, #0xc
	bl ov66_223B938
	mov r1, #0
	ldr r0, _02235D9C // =0x0225B6C4
	ldr r2, [sp, #0xc]
	ldr r5, [r0]
	str r1, [r4, #0x10]
	str r1, [r4, #0x14]
	add r1, sp, #0x10
	str r2, [r5, #0xd0]
	add r0, r5, #0xd4
	bl ov66_223BC5C
	ldr r1, [sp, #0x1c]
	add r0, sp, #0x10
	str r1, [r5, #0xe0]
	ldr r1, [sp, #0x20]
	str r1, [r5, #0xe4]
	bl ov66_223BADC
	add r0, sp, #0x30
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x54
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_02235D9C: .word 0x0225B6C4
_02235DA0: .word ov66_2239B54
_02235DA4: .word ov66_2239D60
_02235DA8: .word ov66_2239FA4
_02235DAC: .word ov66_223A1E4
_02235DB0: .word ov66_223A958
_02235DB4: .word 0x02258C4C
_02235DB8: .word 0x02258C0C
_02235DBC: .word 0x02258C2C
_02235DC0: .word ov66_22399A8
	arm_func_end PPW_LobbyJoinSubChannelAsync

	arm_func_start ov66_2235DC4
ov66_2235DC4: // 0x02235DC4
	ldr ip, _02235DD0 // =ov66_223B938
	add r1, r1, #0x58
	bx ip
	.align 2, 0
_02235DD0: .word ov66_223B938
	arm_func_end ov66_2235DC4

	arm_func_start PPW_LobbyLeaveSubChannel
PPW_LobbyLeaveSubChannel: // 0x02235DD4
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x40
	ldr r0, _02235FC4 // =0x0225B6C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x40
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, pc}
	beq _02235E0C
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x40
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, pc}
_02235E0C:
	cmp r1, #0
	beq _02235E30
	ldr r0, _02235FC4 // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x40
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, pc}
_02235E30:
	cmp r1, #0
	beq _02235E50
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _02235E5C
_02235E50:
	add sp, sp, #0x40
	mov r0, #4
	ldmia sp!, {r3, r4, r5, pc}
_02235E5C:
	add r0, sp, #0x24
	add r1, r1, #0xd0
	bl ov66_2235FC8
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x25
	ldrne r0, [sp, #0x2c]
	bl FUN_022451C8
	ldr r1, _02235FC4 // =0x0225B6C4
	mov r2, #0
	strb r2, [sp]
	cmp r0, #1
	ldr r1, [r1]
	beq _02235EA4
	cmp r0, #2
	moveq r2, #3
	b _02235EA8
_02235EA4:
	mov r2, #2
_02235EA8:
	cmp r2, #0
	strne r2, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0x24
	bl ov66_223BADC
	ldr r0, _02235FC4 // =0x0225B6C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0x18
	mov r0, r1
_02235ED4:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02235ED4
	mov r2, #5
	add r1, sp, #0x18
	add r0, r4, #0xd4
	str r2, [r4, #0xd0]
	bl ov66_223BC5C
	mov r1, #0
	str r1, [r4, #0xe0]
	add r0, sp, #0x18
	str r1, [r4, #0xe4]
	bl ov66_223BADC
	ldr r0, _02235FC4 // =0x0225B6C4
	mov r1, #0
	ldr r4, [r0]
	add r5, sp, #4
	str r1, [r4, #0x10c]
	ldr r0, [r4, #0xfc]
	add r1, sp, #0x34
	str r0, [sp, #0x30]
	add r0, r4, #0x100
	ldmia r0, {r2, r3}
	stmia r1, {r2, r3}
	ldr ip, [r4, #0x108]
	mov r0, #5
	str ip, [sp, #0x3c]
	str r0, [sp, #0x30]
	str r0, [sp, #4]
	ldmia r1, {r2, r3}
	add r1, sp, #8
	stmia r1, {r2, r3}
	mov r0, #1
	str r0, [sp, #0x14]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	str ip, [sp, #0x10]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02235FB0
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #0xfc]
	add r0, r4, #0x100
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x108]
	str r0, [r4, #0x10c]
_02235FB0:
	mov r0, #1
	str r0, [r4, #0x128]
	mov r0, #0
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02235FC4: .word 0x0225B6C4
	arm_func_end PPW_LobbyLeaveSubChannel

	arm_func_start ov66_2235FC8
ov66_2235FC8: // 0x02235FC8
	ldr ip, _02235FD4 // =ov66_223B938
	add r1, r1, #4
	bx ip
	.align 2, 0
_02235FD4: .word ov66_223B938
	arm_func_end ov66_2235FC8

	arm_func_start PPW_LobbyGetTimeInfo
PPW_LobbyGetTimeInfo: // 0x02235FD8
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x1c
	ldr r1, _02236080 // =0x0225B6C4
	mov r5, r0
	ldr r4, [r1]
	cmp r4, #0
	addeq sp, sp, #0x1c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _02236014
	ldr r0, [r4]
	cmp r0, #0xa
	addeq sp, sp, #0x1c
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_02236014:
	cmp r4, #0
	beq _02236038
	ldr r0, _02236080 // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x1c
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02236038:
	add r0, sp, #0xc
	add r1, sp, #0
	bl DWC_GetDateTime
	add r0, sp, #0xc
	add r1, sp, #0
	bl RTC_ConvertDateTimeToSecond
	stmia r5, {r0, r1}
	ldr r2, [r4, #0x14c]
	ldr r1, [r4, #0x150]
	mov r0, #0
	str r2, [r5, #8]
	str r1, [r5, #0xc]
	ldr r2, [r4, #0x154]
	ldr r1, [r4, #0x158]
	str r2, [r5, #0x10]
	str r1, [r5, #0x14]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_02236080: .word 0x0225B6C4
	arm_func_end PPW_LobbyGetTimeInfo

	arm_func_start PPW_LobbyGetSchedule
PPW_LobbyGetSchedule: // 0x02236084
	stmfd sp!, {r3, lr}
	ldr r2, _022360F0 // =0x0225B6C4
	ldr r3, [r2]
	mov r2, r1
	cmp r3, #0
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _022360B4
	ldr r1, [r3]
	cmp r1, #0xa
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_022360B4:
	cmp r3, #0
	beq _022360D4
	ldr r1, _022360F0 // =0x0225B6C4
	ldr r1, [r1]
	ldr r1, [r1]
	cmp r1, #8
	movne r0, #4
	ldmneia sp!, {r3, pc}
_022360D4:
	mov r1, r0
	add r0, r3, #0x144
	bl ov66_223CA80
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r3, pc}
	.align 2, 0
_022360F0: .word 0x0225B6C4
	arm_func_end PPW_LobbyGetSchedule

	arm_func_start PPW_LobbyGetSubChannelState
PPW_LobbyGetSubChannelState: // 0x022360F4
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r0, _022361B4 // =0x0225B6C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0xc
	moveq r0, #3
	ldmeqia sp!, {r3, r4, pc}
	beq _0223612C
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0xc
	moveq r0, #3
	ldmeqia sp!, {r3, r4, pc}
_0223612C:
	cmp r1, #0
	beq _02236150
	ldr r0, _022361B4 // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0xc
	movne r0, #3
	ldmneia sp!, {r3, r4, pc}
_02236150:
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #0
	add r1, r1, #0xd0
	bl ov66_2235FC8
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	bl ov66_2244988
	mov r4, r0
	add r0, sp, #0
	bl ov66_223BADC
	cmp r4, #0
	movne r0, #2
	moveq r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_022361B4: .word 0x0225B6C4
	arm_func_end PPW_LobbyGetSubChannelState

	arm_func_start PPW_LobbySetChannelData
PPW_LobbySetChannelData: // 0x022361B8
	stmfd sp!, {r3, lr}
	ldr ip, _02236214 // =0x0225B6C4
	ldr lr, [ip]
	cmp lr, #0
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _022361E4
	ldr ip, [lr]
	cmp ip, #0xa
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_022361E4:
	cmp lr, #0
	beq _02236204
	ldr ip, _02236214 // =0x0225B6C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	movne r0, #4
	ldmneia sp!, {r3, pc}
_02236204:
	ldr ip, [sp, #8]
	str ip, [sp]
	bl ov66_2237348
	ldmia sp!, {r3, pc}
	.align 2, 0
_02236214: .word 0x0225B6C4
	arm_func_end PPW_LobbySetChannelData

	arm_func_start PPW_LobbyGetChannelDataAsync
PPW_LobbyGetChannelDataAsync: // 0x02236218
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	ldr r4, _0223633C // =0x0225B6C4
	mov r6, r0
	ldr ip, [r4]
	mov r5, r2
	cmp ip, #0
	mov r4, r3
	addeq sp, sp, #0x14
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	beq _0223625C
	ldr r0, [ip]
	cmp r0, #0xa
	addeq sp, sp, #0x14
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, pc}
_0223625C:
	cmp ip, #0
	beq _02236280
	ldr r0, _0223633C // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x14
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, pc}
_02236280:
	mov r2, r1
	add r0, sp, #8
	add r1, ip, #0xb8
	bl ov66_2236348
	ldr r1, _02236340 // =0x02258FB4
	add r0, sp, #8
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022362C4
	add r0, sp, #8
	bl ov66_223BADC
	add sp, sp, #0x14
	mov r0, #5
	ldmia sp!, {r3, r4, r5, r6, pc}
_022362C4:
	ldr r0, [sp, #8]
	ldr r3, _02236344 // =ov66_223A21C
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #9
	ldrne r0, [sp, #0x10]
	mov r1, r6
	mov r2, r5
	str r4, [sp]
	bl FUN_022456B0
	mov r4, r0
	ldr r1, _0223633C // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #4]
	cmp r4, #1
	ldr r1, [r1]
	beq _02236314
	cmp r4, #2
	moveq r0, #3
	b _02236318
_02236314:
	mov r0, #2
_02236318:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #8
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_0223633C: .word 0x0225B6C4
_02236340: .word 0x02258FB4
_02236344: .word ov66_223A21C
	arm_func_end PPW_LobbyGetChannelDataAsync

	arm_func_start ov66_2236348
ov66_2236348: // 0x02236348
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	cmp r2, #0
	bne _02236374
	bl ov66_2235FC8
	add r3, sp, #0
	ldmia r4, {r0, r1, r2}
	add sp, sp, #0xc
	stmia r3, {r0, r1, r2}
	ldmia sp!, {r3, r4, pc}
_02236374:
	add r1, r1, #0x18
	bl ov66_2235FC8
	add r3, sp, #0
	ldmia r4, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_2236348

	arm_func_start PPW_LobbySendChannelBinaryMessage
PPW_LobbySendChannelBinaryMessage: // 0x02236390
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, _02236410 // =0x0225B6C4
	ldr lr, [ip]
	cmp lr, #0
	addeq sp, sp, #0xc
	moveq r0, #4
	ldmeqia sp!, {pc}
	beq _022363C8
	ldr ip, [lr]
	cmp ip, #0xa
	addeq sp, sp, #0xc
	moveq r0, #7
	ldmeqia sp!, {pc}
_022363C8:
	cmp lr, #0
	beq _022363EC
	ldr ip, _02236410 // =0x0225B6C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	addne sp, sp, #0xc
	movne r0, #4
	ldmneia sp!, {pc}
_022363EC:
	stmia sp, {r1, r2}
	mov r1, r0
	mov r2, #1
	str r3, [sp, #8]
	mov r3, r2
	mvn r0, #0
	bl ov66_2237170
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_02236410: .word 0x0225B6C4
	arm_func_end PPW_LobbySendChannelBinaryMessage

	arm_func_start PPW_LobbySendPlayerBinaryMessage
PPW_LobbySendPlayerBinaryMessage: // 0x02236414
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, _02236490 // =0x0225B6C4
	ldr lr, [ip]
	cmp lr, #0
	addeq sp, sp, #0xc
	moveq r0, #4
	ldmeqia sp!, {pc}
	beq _0223644C
	ldr ip, [lr]
	cmp ip, #0xa
	addeq sp, sp, #0xc
	moveq r0, #7
	ldmeqia sp!, {pc}
_0223644C:
	cmp lr, #0
	beq _02236470
	ldr ip, _02236490 // =0x0225B6C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	addne sp, sp, #0xc
	movne r0, #4
	ldmneia sp!, {pc}
_02236470:
	stmia sp, {r1, r2}
	mov r2, #1
	str r3, [sp, #8]
	mov r3, r2
	mov r1, #5
	bl ov66_2237170
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_02236490: .word 0x0225B6C4
	arm_func_end PPW_LobbySendPlayerBinaryMessage

	arm_func_start PPW_LobbyGetMyUserId
PPW_LobbyGetMyUserId: // 0x02236494
	ldr r0, _022364AC // =0x0225B6C4
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #0x4c]
	mvneq r0, #0
	bx lr
	.align 2, 0
_022364AC: .word 0x0225B6C4
	arm_func_end PPW_LobbyGetMyUserId

	arm_func_start ov66_22364B0
ov66_22364B0: // 0x022364B0
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov66_22364B0

	arm_func_start PPW_LobbyUpdateMyProfile
PPW_LobbyUpdateMyProfile: // 0x022364C4
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x3c
	ldr r3, _02236670 // =0x0225B6C4
	mov r6, r2
	ldr r5, [r3]
	cmp r5, #0
	addeq sp, sp, #0x3c
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _02236500
	ldr r2, [r5]
	cmp r2, #0xa
	addeq sp, sp, #0x3c
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02236500:
	cmp r5, #0
	beq _02236524
	ldr r2, _02236670 // =0x0225B6C4
	ldr r2, [r2]
	ldr r2, [r2]
	cmp r2, #8
	addne sp, sp, #0x3c
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02236524:
	mov r2, #0
	str r2, [sp, #0x20]
	add r4, r0, r1
	str r2, [sp, #0x24]
	str r2, [sp, #0x28]
	strb r2, [sp, #9]
	strb r2, [sp, #0xa]
	strb r2, [sp, #8]
	strb r2, [sp, #4]
	sub r2, sp, #4
	ldr r1, [sp, #0x1c]
	str r4, [r2]
	str r1, [r2, #4]
	add ip, sp, #0x20
	str r0, [sp, #0x10]
	ldr r3, [r2]
	add r1, sp, #0x10
	mov r0, ip
	str r4, [sp, #0x18]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x20]
	ldr ip, [sp, #0x24]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	str r2, [sp, #0x34]
	sub r2, sp, #4
	strb r0, [r2]
	add r4, sp, #0x2c
	ldr r3, [r2]
	mov r0, r4
	add r2, r1, ip
	bl ov66_223C2F4
	mov r0, #1
	cmp r6, #0
	str r0, [r4, #0xc]
	strne r0, [r5, #0x11c]
	mov r6, r4
	ldr r8, [r5, #0x114]
	ldr r0, [r6, #4]
	mov r7, #0
	cmp r8, r0
	bne _02236610
	cmp r8, #0
	beq _0223660C
	add r0, r5, #0x110
	bl ov66_2235B8C
	mov r4, r0
	mov r0, r6
	bl ov66_2235B8C
	mov r1, r0
	mov r0, r4
	mov r2, r8
	bl memcmp
	cmp r0, #0
	bne _02236610
_0223660C:
	mov r7, #1
_02236610:
	cmp r7, #0
	bne _0223664C
	add r0, r5, #0x110
	cmp r0, r6
	beq _02236644
	mov r3, #0
	ldmia r6, {r1, r4}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4
	bl ov66_2236674
_02236644:
	mov r0, #1
	str r0, [r5, #0x11c]
_0223664C:
	add r0, sp, #0x2c
	bl ov66_223B8E8
	mov r1, #1
	add r0, sp, #0x20
	str r1, [r5, #0x12c]
	bl ov66_223B8E8
	mov r0, #0
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02236670: .word 0x0225B6C4
	arm_func_end PPW_LobbyUpdateMyProfile

	arm_func_start ov66_2236674
ov66_2236674: // 0x02236674
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	ldr r1, [r7, #8]
	sub r4, r5, r6
	cmp r4, r1
	bls _022366B4
	mov r2, #0
	sub r1, r4, r1
	strb r2, [sp]
	bl ov66_223B7A4
	mov r2, r0
	mov r0, r7
	mov r1, r4
	bl ov66_223B734
_022366B4:
	ldr r0, [r7]
	mov r1, r6
	sub r2, r5, r6
	bl memmove
	str r4, [r7, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2236674

	arm_func_start ov66_22366CC
ov66_22366CC: // 0x022366CC
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r5, r0
	ldr r3, [sp, #0x24]
	ldr r2, [sp, #0x1c]
	ldr r1, [r5, #8]
	sub r4, r3, r2
	cmp r4, r1
	bls _02236714
	mov r2, #0
	sub r1, r4, r1
	strb r2, [sp, #2]
	bl ov66_223B7A4
	mov r2, r0
	mov r0, r5
	mov r1, r4
	bl ov66_223B734
_02236714:
	ldr r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	ldr r3, [sp, #0x24]
	str r1, [sp, #4]
	str r0, [sp, #8]
	cmp r1, r3
	ldr ip, [r5]
	beq _0223675C
	mov r2, #0
_02236738:
	ldr r0, [sp, #4]
	strb r2, [sp]
	ldrb r1, [r0], #1
	str r0, [sp, #4]
	cmp r0, r3
	strb r1, [sp, #1]
	ldrsb r1, [sp, #1]
	strb r1, [ip], #1
	bne _02236738
_0223675C:
	str r4, [r5, #4]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_22366CC

	arm_func_start PPW_LobbyStartRecruit
PPW_LobbyStartRecruit: // 0x02236770
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x94
	ldr r1, _022369EC // =0x0225B6C4
	mov r5, r0
	ldr r1, [r1]
	cmp r1, #0
	addeq sp, sp, #0x94
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _022367AC
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x94
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_022367AC:
	cmp r1, #0
	beq _022367D0
	ldr r0, _022369EC // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x94
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_022367D0:
	mov r1, #0
	add r0, sp, #0x88
	mov r2, r1
_022367DC:
	str r2, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022367DC
	ldr r0, _022369EC // =0x0225B6C4
	ldr r4, [r0]
	ldr r0, [r4, #0x68]
	cmp r0, #0
	bne _022368EC
	mov r0, #1
	str r0, [r4, #0x68]
	ldr r0, [r5]
	add ip, r5, #0x10
	str r0, [r4, #0x74]
	ldr r0, [r5, #4]
	add r3, r4, #0x84
	str r0, [r4, #0x78]
	ldr r0, [r5, #8]
	mov r2, #0x10
	str r0, [r4, #0x7c]
	ldr r0, [r5, #0xc]
	str r0, [r4, #0x80]
_02236834:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02236834
	add ip, r5, #0x30
	add r3, r4, #0xa4
	mov r2, #0xa
_02236860:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02236860
	bl current_time
	ldr r1, _022369F0 // =0x10624DD3
	mov r3, #0x3e8
	umull r1, r2, r0, r1
	mov r2, r2, lsr #6
	umull r1, r2, r3, r2
	sub r2, r0, r1
	str r2, [sp]
	ldr r3, [r4, #0x4c]
	ldr r2, _022369F4 // =0x02258E88
	add r0, sp, #0x7c
	mov r1, #0x14
	bl ov66_2236A38
	add r0, sp, #0x88
	add r1, sp, #0x7c
	bl ov66_223BC5C
	add r0, sp, #0x7c
	bl ov66_223BADC
	ldr r0, [sp, #0x88]
	mov r2, #0x13
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x89
	ldrne r1, [sp, #0x90]
	add r0, r4, #0xa4
	bl strncpy
	mov r2, #1
_022368EC:
	cmp r2, #0
	bne _02236908
	add r0, sp, #0x88
	bl ov66_223BADC
	add sp, sp, #0x94
	mov r0, #4
	ldmia sp!, {r4, r5, pc}
_02236908:
	ldr r0, [sp, #0x88]
	mov r2, #0x13
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x89
	ldrne r1, [sp, #0x90]
	add r0, r5, #0x30
	bl strncpy
	ldr r2, _022369EC // =0x0225B6C4
	add r0, sp, #0x6c
	add r1, sp, #0x60
	ldr r4, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x6c
	add r1, sp, #0x60
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #5
	str r0, [sp, #0x10]
	sbc r0, r1, #0
	str r0, [sp, #0x14]
	ldr r0, _022369F8 // =ov66_2238728
	mov r1, #5
	str r1, [sp, #0x18]
	str r0, [sp, #0x24]
	str r2, [sp, #0x1c]
	str r2, [sp, #0x20]
	add r0, sp, #0x28
	mov r1, #3
	add r2, sp, #0x10
	bl ov66_22369FC
	mov r0, #0
	add r2, sp, #0x2c
	add lr, sp, #0x48
	add r1, r4, #0x134
	strb r0, [sp, #5]
	strb r0, [sp, #4]
	ldr r0, [sp, #0x28]
	add ip, r2, #8
	str r0, [sp, #0x44]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia lr, {r2, r3}
	add r5, lr, #8
	ldmia ip, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [lr, #0x14]
	add r0, sp, #8
	add r2, sp, #0x44
	str r4, [lr, #0x10]
	bl FUN_0223AABC
	add r0, sp, #0x88
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x94
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_022369EC: .word 0x0225B6C4
_022369F0: .word 0x10624DD3
_022369F4: .word 0x02258E88
_022369F8: .word ov66_2238728
	arm_func_end PPW_LobbyStartRecruit

	arm_func_start ov66_22369FC
ov66_22369FC: // 0x022369FC
	stmfd sp!, {r3, lr}
	mov lr, r2
	str r1, [r0]
	add ip, r0, #4
	ldmia lr, {r2, r3}
	stmia ip, {r2, r3}
	add r1, lr, #8
	add r0, ip, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r0, [lr, #0x10]
	str r0, [ip, #0x10]
	ldr r0, [lr, #0x14]
	str r0, [ip, #0x14]
	ldmia sp!, {r3, pc}
	arm_func_end ov66_22369FC

	arm_func_start ov66_2236A38
ov66_2236A38: // 0x02236A38
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r2, #0
	mov r5, r0
	add r0, sp, #0
	mov r4, r1
	str r2, [sp]
	str r2, [sp, #4]
	str r2, [sp, #8]
	bl ov66_223B6C0
	add r0, sp, #0
	bl ov66_2236AB0
	add r1, sp, #0x20
	bic r3, r1, #3
	ldr r2, [sp, #0x20]
	mov r1, r4
	add r3, r3, #4
	bl OS_VSNPrintf
	add r0, sp, #0
	bl ov66_2236AB0
	mov r1, r0
	mov r0, r5
	bl ov66_223BA6C
	add r0, sp, #0
	bl ov66_223B8E8
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_2236A38

	arm_func_start ov66_2236AB0
ov66_2236AB0: // 0x02236AB0
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov66_2236AB0

	arm_func_start PPW_LobbyUpdateRecruitInfo
PPW_LobbyUpdateRecruitInfo: // 0x02236AC4
	stmfd sp!, {r4, lr}
	ldr r1, _02236BB4 // =0x0225B6C4
	ldr r2, [r1]
	cmp r2, #0
	moveq r0, #4
	ldmeqia sp!, {r4, pc}
	beq _02236AF0
	ldr r1, [r2]
	cmp r1, #0xa
	moveq r0, #7
	ldmeqia sp!, {r4, pc}
_02236AF0:
	cmp r2, #0
	beq _02236B10
	ldr r1, _02236BB4 // =0x0225B6C4
	ldr r1, [r1]
	ldr r1, [r1]
	cmp r1, #8
	movne r0, #4
	ldmneia sp!, {r4, pc}
_02236B10:
	ldr r1, _02236BB4 // =0x0225B6C4
	ldr r4, [r1]
	ldr r1, [r4, #0x68]
	cmp r1, #0
	moveq r0, #0
	beq _02236BA4
	ldr r1, [r0]
	add lr, r0, #0x10
	str r1, [r4, #0x74]
	ldr r1, [r0, #4]
	add ip, r4, #0x84
	str r1, [r4, #0x78]
	ldr r1, [r0, #8]
	mov r3, #0x10
	str r1, [r4, #0x7c]
	ldr r1, [r0, #0xc]
	str r1, [r4, #0x80]
_02236B54:
	ldrb r2, [lr]
	ldrb r1, [lr, #1]
	add lr, lr, #2
	subs r3, r3, #1
	strb r2, [ip]
	strb r1, [ip, #1]
	add ip, ip, #2
	bne _02236B54
	add ip, r0, #0x30
	add r3, r4, #0xa4
	mov r2, #0xa
_02236B80:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02236B80
	mov r0, #1
_02236BA4:
	cmp r0, #0
	moveq r0, #4
	movne r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_02236BB4: .word 0x0225B6C4
	arm_func_end PPW_LobbyUpdateRecruitInfo

	arm_func_start PPW_LobbyStopRecruit
PPW_LobbyStopRecruit: // 0x02236BB8
	stmfd sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _02236C9C // =0x0225B6C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x10
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _02236BF0
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x10
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_02236BF0:
	cmp r1, #0
	beq _02236C14
	ldr r0, _02236C9C // =0x0225B6C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x10
	movne r0, #4
	ldmneia sp!, {r3, pc}
_02236C14:
	ldr r0, _02236C9C // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0x68]
	cmp r0, #0
	mov r0, #0
	strne r0, [r1, #0x68]
	movne r0, #1
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	ldr r0, _02236C9C // =0x0225B6C4
	mov r1, #0
	ldr r2, [r0]
	mov r0, #2
	str r0, [sp]
	add r0, r2, #0x74
	str r0, [sp, #4]
	mov r0, #0x44
	str r0, [sp, #8]
	ldr r0, [r2, #0x4c]
	mov r3, r1
	mov r2, #1
	bl ov66_2237170
	ldr r0, _02236C9C // =0x0225B6C4
	mov r2, #3
	ldr r0, [r0]
	add r1, sp, #0xc
	add r0, r0, #0x134
	str r2, [sp, #0xc]
	bl ov66_2236CA0
	mov r0, #0
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.align 2, 0
_02236C9C: .word 0x0225B6C4
	arm_func_end PPW_LobbyStopRecruit

	arm_func_start ov66_2236CA0
ov66_2236CA0: // 0x02236CA0
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	mov r2, r1
	add r0, sp, #8
	mov r1, r4
	bl ov66_2236CF0
	ldr r2, [sp, #8]
	add r0, r4, #4
	cmp r2, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #4
	mov r1, r4
	str r2, [sp]
	bl FUN_0223AB54
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_2236CA0

	arm_func_start ov66_2236CF0
ov66_2236CF0: // 0x02236CF0
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	mov r4, r2
	mov r6, r0
	ldr r2, [r5, #4]
	mov r0, r5
	mov r1, r4
	add r3, r5, #4
	bl ov66_2236D44
	add r1, r5, #4
	cmp r0, r1
	beq _02236D30
	ldr r2, [r4]
	ldr r1, [r0, #0xc]
	cmp r2, r1
	bhs _02236D3C
_02236D30:
	add r0, r5, #4
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
_02236D3C:
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_2236CF0

	arm_func_start ov66_2236D44
ov66_2236D44: // 0x02236D44
	cmp r2, #0
	beq _02236D6C
	ldr r1, [r1]
_02236D50:
	ldr r0, [r2, #0xc]
	cmp r0, r1
	movhs r3, r2
	ldrhs r2, [r2]
	ldrlo r2, [r2, #4]
	cmp r2, #0
	bne _02236D50
_02236D6C:
	mov r0, r3
	bx lr
	arm_func_end ov66_2236D44

	arm_func_start PPW_LobbySubmitQuestionnaire
PPW_LobbySubmitQuestionnaire: // 0x02236D74
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x334
	ldr r1, _022370E8 // =0x0225B6C4
	mov r5, r0
	ldr r2, [r1]
	cmp r2, #0
	addeq sp, sp, #0x334
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _02236DB0
	ldr r0, [r2]
	cmp r0, #0xa
	addeq sp, sp, #0x334
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02236DB0:
	cmp r5, #0
	blt _02236DC0
	cmp r5, #2
	ble _02236DCC
_02236DC0:
	add sp, sp, #0x334
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02236DCC:
	add r1, sp, #0x5c
	add r0, r2, #0x144
	bl ov66_223EDE4
	cmp r0, #0
	beq _02236DF4
	ldr r0, _022370E8 // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0x6c]
	cmp r0, #0
	beq _02236E00
_02236DF4:
	add sp, sp, #0x334
	mov r0, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02236E00:
	add r0, r1, #0x110
	bl ov66_2235B8C
	mov r4, r0
	mov r0, #0
	add r3, sp, #0x30
	mov r1, r0
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	ldr r2, _022370E8 // =0x0225B6C4
	str r0, [r3]
	ldr r1, [r2]
	add r0, r1, #0x110
	bl ov66_2235B8C
	ldr r1, _022370E8 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x20]
	str r1, [sp, #0x1c]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x22]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x24
	strb r1, [sp, #0x23]
	bl OS_GetMacAddress
	ldrh r8, [sp, #0x20]
	ldrb r7, [sp, #0x22]
	ldrb r6, [sp, #0x23]
	ldrb r2, [sp, #0x25]
	ldrb r3, [sp, #0x24]
	ldrb r1, [sp, #0x26]
	ldrb r0, [sp, #0x27]
	ldrb ip, [sp, #0x28]
	strh r8, [sp, #0x34]
	ldrb r8, [sp, #0x29]
	strb r7, [sp, #0x36]
	ldrh r7, [sp, #0x2a]
	strb r6, [sp, #0x37]
	ldr lr, [sp, #0x1c]
	ldr r6, [sp, #0x2c]
	strb r2, [sp, #0x39]
	ldr r2, [sp, #0x60]
	strb r3, [sp, #0x38]
	ldr r3, [sp, #0x5c]
	strb r1, [sp, #0x3a]
	strb r0, [sp, #0x3b]
	str lr, [sp, #0x30]
	strb ip, [sp, #0x3c]
	strb r8, [sp, #0x3d]
	strh r7, [sp, #0x3e]
	str r6, [sp, #0x40]
	str r2, [sp, #0x48]
	ldr r1, _022370E8 // =0x0225B6C4
	str r5, [sp, #0x4c]
	str r3, [sp, #0x44]
	ldr r3, [r4, #4]
	ldr r0, _022370EC // =0x02258E58
	str r3, [sp, #0x50]
	ldr r2, [r1]
	ldrb r1, [r4, #0x38]
	ldr r5, [r0, #4]
	str r1, [sp, #0x54]
	ldrh r1, [r4, #0x3c]
	strh r1, [sp, #0x58]
	ldrb r1, [r4, #0x3e]
	strh r1, [sp, #0x5a]
	ldr r4, [r0, #0xc]
	ldr r0, _022370F0 // =0x0225B6D4
	ldr r1, [r2, #0x50]
	ldr r0, [r0]
	cmp r1, #0
	moveq r8, #0
	movne r8, #1
	ldr r7, [r2, #0x4c]
	cmp r0, #0
	bne _022370CC
	mov r0, #0x40
	bl FUN_02246290
	movs r6, r0
	beq _02236FE0
	ldr r1, _022370F4 // =0x02258EC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov66_223BA6C
	mov r1, r5
	add r0, r6, #0x18
	bl ov66_223BA6C
	add r2, sp, #0x30
	add r4, r2, #0x2c
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _022370F8 // =ov66_22393C0
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	str r1, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
_02236FE0:
	ldr r0, _022370F0 // =0x0225B6D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl FUN_02258338
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02237050
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _02237084
_02237050:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_02237084:
	cmp r0, #0
	moveq r0, #1
	beq _02237098
	bl FUN_022585F4
	mov r0, #0
_02237098:
	cmp r0, #0
	bne _022370CC
	ldr r0, _022370F0 // =0x0225B6D4
	ldr r0, [r0]
	cmp r0, #0
	beq _022370CC
	beq _022370C0
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022370C0:
	ldr r0, _022370F0 // =0x0225B6D4
	mov r1, #0
	str r1, [r0]
_022370CC:
	ldr r0, _022370E8 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r0]
	mov r0, #0
	str r2, [r1, #0x6c]
	add sp, sp, #0x334
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_022370E8: .word 0x0225B6C4
_022370EC: .word 0x02258E58
_022370F0: .word 0x0225B6D4
_022370F4: .word 0x02258EC0
_022370F8: .word ov66_22393C0
	arm_func_end PPW_LobbySubmitQuestionnaire

	arm_func_start PPW_LobbyGetLastError
PPW_LobbyGetLastError: // 0x022370FC
	ldr r0, _02237114 // =0x0225B6C4
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	moveq r0, #0
	bx lr
	.align 2, 0
_02237114: .word 0x0225B6C4
	arm_func_end PPW_LobbyGetLastError

	arm_func_start PPW_LobbyToErrorCode
PPW_LobbyToErrorCode: // 0x02237118
	stmfd sp!, {r3, lr}
	cmp r0, #4
	addge r0, r0, #0x17c
	addge r0, r0, #0x2800
	ldmgeia sp!, {r3, pc}
	bl ov66_224497C
	ldmia sp!, {r3, pc}
	arm_func_end PPW_LobbyToErrorCode

	arm_func_start ov66_2237134
ov66_2237134: // 0x02237134
	cmp r0, #0
	beq _02237150
	cmp r0, #1
	beq _02237158
	cmp r0, #2
	beq _02237160
	b _02237168
_02237150:
	mov r0, #0
	bx lr
_02237158:
	mov r0, #2
	bx lr
_02237160:
	mov r0, #3
	bx lr
_02237168:
	mov r0, #1
	bx lr
	arm_func_end ov66_2237134

	arm_func_start ov66_2237170
ov66_2237170: // 0x02237170
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x24
	mov r5, r0
	mvn r0, #0
	mov r4, r1
	cmp r5, r0
	cmpeq r4, #5
	addeq sp, sp, #0x24
	moveq r0, #3
	ldmeqia sp!, {r4, r5, pc}
	mov ip, #0
	add r1, sp, #0x18
	mov r0, ip
_022371A4:
	str r0, [r1, ip, lsl #2]
	add ip, ip, #1
	cmp ip, #3
	blo _022371A4
	ldr lr, [sp, #0x38]
	mov r0, r2
	mov r1, r3
	ldr r2, [sp, #0x30]
	ldr r3, [sp, #0x34]
	add ip, sp, #0x18
	str lr, [sp]
	str ip, [sp, #4]
	bl ov66_22416DC
	cmp r0, #0
	bne _022371F4
	add r0, sp, #0x18
	bl ov66_223BADC
	add sp, sp, #0x24
	mov r0, #3
	ldmia sp!, {r4, r5, pc}
_022371F4:
	cmp r4, #5
	bne _02237268
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x19
	ldrne r1, [sp, #0x20]
	mov r0, r5
	bl FUN_022453BC
	mov r4, r0
	ldr r1, _02237340 // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #9]
	cmp r4, #1
	ldr r1, [r1]
	beq _02237240
	cmp r4, #2
	moveq r0, #3
	b _02237244
_02237240:
	mov r0, #2
_02237244:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0x18
	bl ov66_223BADC
	add sp, sp, #0x24
	mov r0, r4
	ldmia sp!, {r4, r5, pc}
_02237268:
	ldr r1, _02237340 // =0x0225B6C4
	add r0, sp, #0xc
	ldr r1, [r1]
	mov r2, r4
	add r1, r1, #0xb8
	bl ov66_2236348
	ldr r1, _02237344 // =0x02258FB4
	add r0, sp, #0xc
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022372BC
	add r0, sp, #0xc
	bl ov66_223BADC
	add r0, sp, #0x18
	bl ov66_223BADC
	add sp, sp, #0x24
	mov r0, #5
	ldmia sp!, {r4, r5, pc}
_022372BC:
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xc]
	addeq r1, sp, #0x19
	mov r0, r0, lsl #0x1f
	ldrne r1, [sp, #0x20]
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0xd
	ldrne r0, [sp, #0x14]
	bl FUN_022452D0
	mov r4, r0
	ldr r1, _02237340 // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r4, #1
	ldr r1, [r1]
	beq _02237310
	cmp r4, #2
	moveq r0, #3
	b _02237314
_02237310:
	mov r0, #2
_02237314:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0xc
	bl ov66_223BADC
	add r0, sp, #0x18
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_02237340: .word 0x0225B6C4
_02237344: .word 0x02258FB4
	arm_func_end ov66_2237170

	arm_func_start ov66_2237348
ov66_2237348: // 0x02237348
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	ldr r4, _022374A8 // =0x0225B6C4
	mov r6, r0
	ldr r4, [r4]
	mov r5, r2
	mov r2, r1
	add r1, r4, #0xb8
	add r0, sp, #0x10
	mov r4, r3
	bl ov66_2236348
	ldr r1, _022374AC // =0x02258FB4
	add r0, sp, #0x10
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022373A8
	add r0, sp, #0x10
	bl ov66_223BADC
	add sp, sp, #0x1c
	mov r0, #5
	ldmia sp!, {r3, r4, r5, r6, pc}
_022373A8:
	ldr r1, [sp, #0x30]
	cmp r1, #0x12c
	bls _022373C8
	add r0, sp, #0x10
	bl ov66_223BADC
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, pc}
_022373C8:
	ldr r0, _022374B0 // =0xAAAAAAAB
	add r1, r1, #2
	umull r0, r2, r1, r0
	mov r3, #0
	mov r2, r2, lsr #1
	mov r1, r2, lsl #2
	add r0, sp, #4
	add r1, r1, #1
	str r3, [sp, #4]
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	bl ov66_223B6C0
	add r0, sp, #4
	bl ov66_2236AB0
	mov r2, r0
	ldr r1, [sp, #0x30]
	ldr r3, [sp, #8]
	mov r0, r4
	bl DWC_Base64Encode
	ldr r1, [sp, #4]
	mov r2, #0
	strb r2, [r1, r0]
	ldr r0, [sp, #0x10]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r4, sp, #0x11
	ldrne r4, [sp, #0x18]
	add r0, sp, #4
	bl ov66_2236AB0
	mov r3, r0
	mov r0, r4
	mov r1, r6
	mov r2, r5
	bl FUN_022454C8
	mov r4, r0
	ldr r1, _022374A8 // =0x0225B6C4
	mov r0, #0
	strb r0, [sp]
	cmp r4, #1
	ldr r1, [r1]
	beq _02237478
	cmp r4, #2
	moveq r0, #3
	b _0223747C
_02237478:
	mov r0, #2
_0223747C:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #4
	bl ov66_223B8E8
	add r0, sp, #0x10
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_022374A8: .word 0x0225B6C4
_022374AC: .word 0x02258FB4
_022374B0: .word 0xAAAAAAAB
	arm_func_end ov66_2237348

	arm_func_start ov66_22374B4
ov66_22374B4: // 0x022374B4
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc4
	ldr r0, _02237894 // =0x0225B6C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov66_2235B8C
	ldr r1, _02237894 // =0x0225B6C4
	mov r4, r0
	ldr r0, [r1]
	add r0, r0, #0x110
	bl ov66_2235B8C
	ldr r1, _02237894 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x20]
	str r1, [sp, #0x1c]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x22]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x24
	strb r1, [sp, #0x23]
	bl OS_GetMacAddress
	ldr r0, [r4]
	add r3, sp, #0x38
	str r0, [sp, #0x30]
	ldr r0, [r4, #4]
	add r5, r4, #8
	str r0, [sp, #0x34]
	mov r2, #4
_0223752C:
	ldrh r1, [r5]
	ldrh r0, [r5, #2]
	add r5, r5, #4
	subs r2, r2, #1
	strh r1, [r3]
	strh r0, [r3, #2]
	add r3, r3, #4
	bne _0223752C
	ldr r1, [r4, #0x18]
	add r0, sp, #0x74
	str r1, [sp, #0x48]
	ldr r2, [r4, #0x1c]
	add r1, r4, #0x44
	str r2, [sp, #0x4c]
	ldrh r3, [r4, #0x20]
	ldrh r2, [r4, #0x22]
	add r6, sp, #0x7c
	add r7, r4, #0x4c
	strh r3, [sp, #0x50]
	strh r2, [sp, #0x52]
	ldrh r3, [r4, #0x24]
	ldrh r2, [r4, #0x26]
	mov r5, #6
	strh r3, [sp, #0x54]
	strh r2, [sp, #0x56]
	ldrh r3, [r4, #0x28]
	ldrh r2, [r4, #0x2a]
	strh r3, [sp, #0x58]
	strh r2, [sp, #0x5a]
	ldrb r3, [r4, #0x2c]
	ldrb r2, [r4, #0x2d]
	strb r3, [sp, #0x5c]
	strb r2, [sp, #0x5d]
	ldrb r3, [r4, #0x2e]
	ldrb r2, [r4, #0x2f]
	strb r3, [sp, #0x5e]
	strb r2, [sp, #0x5f]
	ldrb r3, [r4, #0x30]
	ldrb r2, [r4, #0x31]
	strb r3, [sp, #0x60]
	strb r2, [sp, #0x61]
	ldrb r3, [r4, #0x32]
	ldrb r2, [r4, #0x33]
	strb r3, [sp, #0x62]
	strb r2, [sp, #0x63]
	ldrb r3, [r4, #0x34]
	ldrb r2, [r4, #0x35]
	strb r3, [sp, #0x64]
	strb r2, [sp, #0x65]
	ldrb r3, [r4, #0x36]
	ldrb r2, [r4, #0x37]
	strb r3, [sp, #0x66]
	strb r2, [sp, #0x67]
	ldrb r2, [r4, #0x38]
	strb r2, [sp, #0x68]
	ldrb r2, [r4, #0x39]
	strb r2, [sp, #0x69]
	ldrh r2, [r4, #0x3a]
	strh r2, [sp, #0x6a]
	ldrh r2, [r4, #0x3c]
	strh r2, [sp, #0x6c]
	ldrb r2, [r4, #0x3e]
	strb r2, [sp, #0x6e]
	ldrb r2, [r4, #0x3f]
	strb r2, [sp, #0x6f]
	ldrb r2, [r4, #0x40]
	strb r2, [sp, #0x70]
	ldrb r2, [r4, #0x41]
	strb r2, [sp, #0x71]
	ldrb r2, [r4, #0x42]
	strb r2, [sp, #0x72]
	ldrb r2, [r4, #0x43]
	strb r2, [sp, #0x73]
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
_02237658:
	ldrb r1, [r7]
	ldrb r0, [r7, #1]
	add r7, r7, #2
	subs r5, r5, #1
	strb r1, [r6]
	strb r0, [r6, #1]
	add r6, r6, #2
	bne _02237658
	add r6, r4, #0x58
	add r5, sp, #0x88
	ldmia r6!, {r0, r1, r2, r3}
	stmia r5!, {r0, r1, r2, r3}
	ldmia r6!, {r0, r1, r2, r3}
	stmia r5!, {r0, r1, r2, r3}
	ldmia r6, {r0, r1, r2, r3}
	stmia r5, {r0, r1, r2, r3}
	ldr r1, _02237894 // =0x0225B6C4
	ldr r0, _02237898 // =0x02258E58
	ldrh r6, [r4, #0x88]
	ldrh r3, [r4, #0x8a]
	ldr r2, [r1]
	ldr r5, [r0, #0x18]
	strh r6, [sp, #0xb8]
	strh r3, [sp, #0xba]
	ldr r3, [r4, #0x8c]
	ldr r1, [r4, #0x90]
	str r3, [sp, #0xbc]
	str r1, [sp, #0xc0]
	ldr r4, [r0, #0xc]
	ldr r0, _0223789C // =0x0225B6D4
	ldr r1, [r2, #0x50]
	ldr r0, [r0]
	cmp r1, #0
	moveq r8, #0
	movne r8, #1
	cmp r0, #0
	ldr r7, [r2, #0x4c]
	addne sp, sp, #0xc4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl FUN_02246290
	movs r6, r0
	beq _02237798
	ldr r1, _022378A0 // =0x02258EC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov66_223BA6C
	mov r1, r5
	add r0, r6, #0x18
	bl ov66_223BA6C
	add r2, sp, #0x1c
	add r4, r2, #0xa8
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _022378A4 // =ov66_223899C
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	str r1, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
_02237798:
	ldr r0, _0223789C // =0x0225B6D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl FUN_02258338
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02237808
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _0223783C
_02237808:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_0223783C:
	cmp r0, #0
	moveq r0, #1
	beq _02237850
	bl FUN_022585F4
	mov r0, #0
_02237850:
	cmp r0, #0
	addne sp, sp, #0xc4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _0223789C // =0x0225B6D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0xc4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _02237880
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02237880:
	ldr r0, _0223789C // =0x0225B6D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0xc4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02237894: .word 0x0225B6C4
_02237898: .word 0x02258E58
_0223789C: .word 0x0225B6D4
_022378A0: .word 0x02258EC0
_022378A4: .word ov66_223899C
	arm_func_end ov66_22374B4

	arm_func_start ov66_22378A8
ov66_22378A8: // 0x022378A8
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	ldr r3, _02237900 // =0x02258BF4
	mov r5, r0
	ldr r1, [r3, r1, lsl #2]
	add r0, sp, #0xc
	mov r4, r2
	bl ov66_223BA6C
	ldr r2, _02237904 // =0x02258FB8
	add r0, sp, #0
	add r1, sp, #0xc
	bl ov66_22379D0
	add r1, sp, #0
	mov r0, r5
	mov r2, r4
	bl ov66_2237908
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02237900: .word 0x02258BF4
_02237904: .word 0x02258FB8
	arm_func_end ov66_22378A8

	arm_func_start ov66_2237908
ov66_2237908: // 0x02237908
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r3, #0
	mov r7, r0
	mov r0, r3
_0223791C:
	str r0, [r7, r3, lsl #2]
	add r3, r3, #1
	cmp r3, #3
	blo _0223791C
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmneib r1, {r4, r5}
	bne _02237950
	ldrb r0, [r1]
	add r5, r1, #1
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_02237950:
	ldr r0, [r2]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r6, [r2, #8]
	ldrne r8, [r2, #4]
	bne _02237978
	ldrb r0, [r2]
	add r6, r2, #1
	mov r0, r0, lsl #0x18
	mov r8, r0, lsr #0x19
_02237978:
	mov r0, r7
	add r1, r4, r8
	bl ov66_223BB10
	add r0, r5, r4
	mov r1, #0
	ldrb ip, [sp, #9]
	str r0, [sp]
	mov r0, r7
	mov r2, r1
	mov r3, r5
	strb ip, [sp, #4]
	bl ov66_223C018
	add r1, r6, r8
	ldrb r5, [sp, #8]
	str r1, [sp]
	mov r1, r4
	mov r3, r6
	mov r2, #0
	strb r5, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov66_2237908

	arm_func_start ov66_22379D0
ov66_22379D0: // 0x022379D0
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r4, r1
	mov r1, #0
	mov r8, r0
	mov r7, r2
	mov r0, r1
_022379EC:
	str r0, [r8, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022379EC
	mov r0, r7
	bl strlen
	ldr r1, [r4]
	mov r6, r0
	mov r0, r1, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r5, [r4, #8]
	ldrne r4, [r4, #4]
	bne _02237A30
	ldrb r0, [r4]
	add r5, r4, #1
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_02237A30:
	mov r0, r8
	add r1, r4, r6
	bl ov66_223BB10
	add r0, r5, r4
	mov r1, #0
	ldrb ip, [sp, #9]
	str r0, [sp]
	mov r0, r8
	mov r2, r1
	mov r3, r5
	strb ip, [sp, #4]
	bl ov66_223C018
	add r1, r7, r6
	ldrb r5, [sp, #8]
	str r1, [sp]
	mov r1, r4
	mov r3, r7
	mov r2, #0
	strb r5, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov66_22379D0

	arm_func_start ov66_2237A88
ov66_2237A88: // 0x02237A88
	stmfd sp!, {r3, r4, r5, lr}
	ldr r1, _02237AD0 // =0x0225B6C4
	mov r5, r0
	ldr r0, [r1]
	ldr r4, [r0, #0x4c]
	bl current_time
	mov ip, r0
	ldr r0, _02237AD4 // =0x10624DD3
	mov r1, #0x3e8
	umull r0, r3, ip, r0
	mov r3, r3, lsr #6
	umull r3, r0, r1, r3
	ldr r1, _02237AD8 // =0x02258FBC
	mov r0, r5
	mov r2, r4
	sub r3, ip, r3
	bl ov66_2237ADC
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02237AD0: .word 0x0225B6C4
_02237AD4: .word 0x10624DD3
_02237AD8: .word 0x02258FBC
	arm_func_end ov66_2237A88

	arm_func_start ov66_2237ADC
ov66_2237ADC: // 0x02237ADC
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	add r1, sp, #0x24
	mov r6, r0
	mov r0, #0
	bic r4, r1, #3
	ldr r2, [sp, #0x24]
	mov r1, r0
	str r0, [sp, #8]
	add r3, r4, #4
	str r0, [sp, #4]
	str r0, [sp, #0xc]
	bl OS_VSNPrintf
	ldr r3, [sp, #8]
	add r5, r0, #1
	cmp r5, r3
	bls _02237B48
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	add r0, sp, #4
	sub r1, r5, r3
	bl ov66_223B870
	b _02237B60
_02237B48:
	bhs _02237B60
	sub r0, r3, r5
	sub r0, r3, r0
	mov r1, #0
	strb r1, [sp, #1]
	str r0, [sp, #8]
_02237B60:
	add r0, sp, #4
	bl ov66_2236AB0
	ldr r2, [sp, #0x24]
	mov r1, r5
	add r3, r4, #4
	bl OS_VSNPrintf
	ldr r1, [sp, #4]
	add r0, sp, #4
	add r1, r1, r5
	mov r2, #0
	strb r2, [r1, #-1]
	bl ov66_2236AB0
	mov r1, r0
	mov r0, r6
	bl ov66_223BA6C
	add r0, sp, #4
	bl ov66_223B8E8
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_2237ADC

	arm_func_start ov66_2237BB4
ov66_2237BB4: // 0x02237BB4
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x54
	ldr r0, _02237D08 // =0x0225B6C4
	ldr r5, _02237D0C // =ov66_2239B54
	ldr lr, _02237D10 // =ov66_2239D60
	ldr ip, _02237D14 // =ov66_2239FA4
	ldr r4, _02237D18 // =ov66_223A1E4
	ldr r3, _02237D1C // =ov66_223A958
	ldr r1, [r0]
	mov r2, #0
	add r0, sp, #0x24
	str r5, [sp, #0x3c]
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r4, [sp, #0x48]
	str r3, [sp, #0x4c]
	str r2, [sp, #0x50]
	bl ov66_2235DC4
	add r0, sp, #0x30
	add r2, sp, #0x24
	mov r1, #0
	bl ov66_22378A8
	add r0, sp, #0x24
	bl ov66_223BADC
	ldr r0, [sp, #0x30]
	ldr r1, _02237D20 // =ov66_2239408
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	add r3, sp, #0x3c
	str r1, [sp]
	mov r1, #0
	mov r2, #0x14
	str r1, [sp, #4]
	bl FUN_02244F44
	mov r4, r0
	ldr r1, _02237D08 // =0x0225B6C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r4, #1
	ldr r1, [r1]
	beq _02237C6C
	cmp r4, #2
	moveq r0, #3
	b _02237C70
_02237C6C:
	mov r0, #2
_02237C70:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r4, #0
	beq _02237C9C
	add r0, sp, #0x30
	bl ov66_223BADC
	add sp, sp, #0x54
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, pc}
_02237C9C:
	mov r2, #0
	add r0, sp, #0x10
	add r1, sp, #0x30
	str r2, [sp, #0xc]
	add r5, sp, #0xc
	bl ov66_223B938
	mov r1, #0
	ldr r0, _02237D08 // =0x0225B6C4
	ldr r2, [sp, #0xc]
	ldr r6, [r0]
	str r1, [r5, #0x10]
	str r1, [r5, #0x14]
	add r1, sp, #0x10
	str r2, [r6, #0xb8]
	add r0, r6, #0xbc
	bl ov66_223BC5C
	ldr r1, [sp, #0x1c]
	add r0, sp, #0x10
	str r1, [r6, #0xc8]
	ldr r1, [sp, #0x20]
	str r1, [r6, #0xcc]
	bl ov66_223BADC
	add r0, sp, #0x30
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x54
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_02237D08: .word 0x0225B6C4
_02237D0C: .word ov66_2239B54
_02237D10: .word ov66_2239D60
_02237D14: .word ov66_2239FA4
_02237D18: .word ov66_223A1E4
_02237D1C: .word ov66_223A958
_02237D20: .word ov66_2239408
	arm_func_end ov66_2237BB4

	arm_func_start ov66_2237D24
ov66_2237D24: // 0x02237D24
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x40
	mov r8, r0
	mov r7, r1
	add r0, sp, #0x28
	mov r1, r2
	mov r6, r3
	bl ov66_223BA6C
	ldr r1, _02237EF4 // =0x0225B6C4
	add r0, sp, #0x10
	ldr r5, [r1]
	add sb, sp, #0x28
	add r1, r5, #0xb8
	bl ov66_2235FC8
	add r0, sp, #0x10
	mov r1, sb
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x10
	bl ov66_223BADC
	cmp r4, #0
	ldrne r5, [r5, #0xb8]
	bne _02237DB0
	add r0, sp, #0x1c
	add r1, r5, #0xd0
	bl ov66_2235FC8
	add r0, sp, #0x1c
	mov r1, sb
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x1c
	bl ov66_223BADC
	cmp r4, #0
	ldrne r5, [r5, #0xd0]
	moveq r5, #5
_02237DB0:
	add r0, sp, #0x28
	bl ov66_223BADC
	cmp r5, #5
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [sp, #0x64]
	mov r2, #0
	add r1, sp, #0x34
	str r2, [sp, #0x34]
	str r2, [sp, #0x38]
	str r2, [sp, #0x3c]
	bl ov66_2237EF8
	cmp r0, #0
	add r0, sp, #0x34
	bne _02237DF8
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02237DF8:
	ldr sb, [sp, #0x38]
	cmp sb, #0
	bne _02237E10
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02237E10:
	bl ov66_2235B8C
	ldr r1, [sp, #0x60]
	ldr r4, [sp, #0x68]
	str r1, [sp]
	stmib sp, {r0, sb}
	mov r0, r8
	mov r1, r7
	mov r2, r5
	mov r3, r6
	str r4, [sp, #0xc]
	bl ov66_2238070
	cmp r0, #0
	beq _02237E54
	add r0, sp, #0x34
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02237E54:
	mvn r0, #0
	cmp r6, r0
	beq _02237EA0
	ldr r0, _02237EF4 // =0x0225B6C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _02237EA0
	add r0, sp, #0x34
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02237EA0:
	ldr r1, _02237EF4 // =0x0225B6C4
	add r0, sp, #0x34
	ldr r4, [r1]
	bl ov66_2235B8C
	ldr r2, [sp, #0x60]
	ldr r1, [sp, #0x68]
	str r2, [sp]
	str r0, [sp, #4]
	ldr r2, [sp, #0x38]
	mov r0, r8
	str r2, [sp, #8]
	str r1, [sp, #0xc]
	ldr r4, [r4, #0x1c]
	mov r1, r7
	mov r2, r5
	mov r3, r6
	blx r4
	add r0, sp, #0x34
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_02237EF4: .word 0x0225B6C4
	arm_func_end ov66_2237D24

	arm_func_start ov66_2237EF8
ov66_2237EF8: // 0x02237EF8
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r2, #0
	mov r5, r1
	strb r2, [sp, #4]
	mov r6, r0
	bl strlen
	ldr r3, [r5, #4]
	mov r4, r0
	cmp r4, r3
	bls _02237F48
	mov r1, #0
	strb r1, [sp, #2]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r5
	sub r1, r4, r3
	bl ov66_223B870
	b _02237F64
_02237F48:
	bhs _02237F64
	mov r0, #0
	strb r0, [sp, #3]
	ldr r1, [r5, #4]
	sub r0, r3, r4
	sub r0, r1, r0
	str r0, [r5, #4]
_02237F64:
	cmp r4, #0
	addeq sp, sp, #8
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r6
	bl strlen
	mov r4, r0
	mov r0, r5
	bl ov66_2235B8C
	mov r2, r0
	ldr r3, [r5, #4]
	mov r0, r6
	mov r1, r4
	bl DWC_Base64Decode
	mov r4, r0
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r3, [r5, #4]
	cmp r4, r3
	bls _02237FE4
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r5
	sub r1, r4, r3
	bl ov66_223B870
	b _02238000
_02237FE4:
	bhs _02238000
	mov r0, #0
	strb r0, [sp, #1]
	ldr r1, [r5, #4]
	sub r0, r3, r4
	sub r0, r1, r0
	str r0, [r5, #4]
_02238000:
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_2237EF8

	arm_func_start ov66_223800C
ov66_223800C: // 0x0223800C
	stmfd sp!, {r4, lr}
	ldr r2, [r1]
	mov r4, #0
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r3, [r1, #4]
	bne _02238034
	ldrb r2, [r1]
	mov r2, r2, lsl #0x18
	mov r3, r2, lsr #0x19
_02238034:
	ldr r2, [r0]
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r2, [r0, #4]
	bne _02238054
	ldrb r2, [r0]
	mov r2, r2, lsl #0x18
	mov r2, r2, lsr #0x19
_02238054:
	cmp r2, r3
	bne _02238068
	bl ov66_223BE90
	cmp r0, #0
	moveq r4, #1
_02238068:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223800C

	arm_func_start ov66_2238070
ov66_2238070: // 0x02238070
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x70
	ldr r4, [sp, #0x98]
	cmp r2, #0
	ldr sb, [sp, #0x9c]
	ldr r8, [sp, #0xa0]
	mov sl, r3
	addne sp, sp, #0x70
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, _0223844C // =0x02258FC4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _02238248
	ldr r0, _02238450 // =0x0225B6C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp sl, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, sl
	add r0, r2, #0xe8
	bl ov66_2241DCC
	movs r4, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #0
	str r0, [sp, #0x48]
	add r2, sb, r8
	str r0, [sp, #0x4c]
	str r0, [sp, #0x50]
	strb r0, [sp, #9]
	strb r0, [sp, #0xa]
	strb r0, [sp, #8]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x24]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r0, sp, #0x48
	add r1, sp, #0x18
	str sb, [sp, #0x18]
	str r2, [sp, #0x20]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x48]
	ldr r6, [sp, #0x4c]
	str r2, [sp, #0x54]
	str r2, [sp, #0x58]
	str r2, [sp, #0x5c]
	sub r2, sp, #4
	strb r0, [r2]
	add r5, sp, #0x54
	ldr r3, [r2]
	mov r0, r5
	add r2, r1, r6
	bl ov66_223C2F4
	mov r0, #1
	str r0, [r5, #0xc]
	ldr r7, [r4, #0x1c]
	ldr r0, [sp, #0x58]
	mov r6, #0
	cmp r7, r0
	bne _022381C0
	cmp r7, #0
	beq _022381BC
	add r0, r4, #0x18
	bl ov66_2235B8C
	mov fp, r0
	mov r0, r5
	bl ov66_2235B8C
	mov r2, r7
	mov r1, r0
	mov r0, fp
	bl memcmp
	cmp r0, #0
	bne _022381C0
_022381BC:
	mov r6, #1
_022381C0:
	cmp r6, #0
	bne _022381FC
	add r0, r4, #0x18
	cmp r0, r5
	beq _022381F4
	mov r3, #0
	ldmia r5, {r1, r5}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r5
	bl ov66_2236674
_022381F4:
	mov r0, #1
	str r0, [r4, #0x24]
_022381FC:
	add r0, sp, #0x54
	bl ov66_223B8E8
	mov r1, #1
	add r0, sp, #0x48
	str r1, [r4, #0x34]
	bl ov66_223B8E8
	ldr r0, [r4, #0x38]
	cmp r0, #0
	beq _0223823C
	ldr r1, _02238450 // =0x0225B6C4
	mov r0, sl
	ldr r2, [r1]
	mov r1, sb
	ldr r3, [r2, #0x24]
	mov r2, r8
	blx r3
_0223823C:
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02238248:
	ldr r1, _02238454 // =0x02258FD4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _0223838C
	ldr r0, _02238450 // =0x0225B6C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp sl, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r8, #0x10
	addne sp, sp, #0x70
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, sl
	add r0, r2, #0xe8
	bl ov66_2241DCC
	movs r4, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sb]
	add r0, sp, #0x38
	str r1, [sp, #0x34]
	add r1, sb, #4
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r5, sp, #0x34
	ldr r6, [sb, #0xc]
	mov r3, #1
	mov r1, r5
	add r0, r4, #4
	mov r2, #0x10
	str r6, [sp, #0x40]
	str r3, [sp, #0x44]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _0223831C
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #4]
	add r0, r4, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
_0223831C:
	mov r0, #1
	str r0, [r4, #0x30]
	ldr r0, [r4, #0x38]
	cmp r0, #0
	ldrne r0, [r4, #0x14]
	cmpne r0, #0
	beq _02238380
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	ldr r3, _02238450 // =0x0225B6C4
	add r0, sp, #0x28
	add r4, sp, #0x64
	str r5, [sp, #0x30]
	str r2, [sp, #0x28]
	str r1, [sp, #0x2c]
	ldmia r0, {r0, r1, r2}
	stmia r4, {r0, r1, r2}
	ldr r1, [r3]
	mov r0, sl
	ldr r2, [r1, #0x20]
	mov r1, r4
	blx r2
_02238380:
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223838C:
	ldr r1, _02238458 // =0x02258FE4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _022383E8
	cmp r8, #8
	addne sp, sp, #0x70
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _02238450 // =0x0225B6C4
	ldr r4, [sb]
	ldr r3, [sb, #4]
	ldr r1, [r0]
	mov r0, #1
	str r0, [r1, #0x148]
	add r2, sp, #0x10
	str r4, [sp, #0x10]
	str r3, [sp, #0x14]
	add r1, r1, #0x14c
	ldmia r2, {r2, r3}
	add sp, sp, #0x70
	stmia r1, {r2, r3}
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022383E8:
	ldr r1, _0223845C // =0x02258FF4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _02238440
	ldr r0, _02238450 // =0x0225B6C4
	mov r1, sb
	ldr r0, [r0]
	mov r2, r8
	add r0, r0, #0x144
	bl ov66_223CB84
	cmp r0, #0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _02238450 // =0x0225B6C4
	mov r2, #4
	ldr r1, [r0]
	add sp, sp, #0x70
	str r2, [r1, #0x144]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02238440:
	mov r0, #0
	add sp, sp, #0x70
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0223844C: .word 0x02258FC4
_02238450: .word 0x0225B6C4
_02238454: .word 0x02258FD4
_02238458: .word 0x02258FE4
_0223845C: .word 0x02258FF4
	arm_func_end ov66_2238070

	arm_func_start ov66_2238460
ov66_2238460: // 0x02238460
	stmfd sp!, {r4, r5, r6, lr}
	mov r6, r0
	cmp r2, #0
	mov r5, r1
	mov r4, r3
	movne r0, #1
	ldmneia sp!, {r4, r5, r6, pc}
	ldr r0, [r6, #0x10]
	cmp r0, #0
	beq _0223856C
	cmp r0, #1
	beq _0223849C
	cmp r0, #2
	beq _02238504
	b _02238590
_0223849C:
	ldr r0, _02238598 // =0x0225B6C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp r5, r0
	beq _02238590
	add r0, r2, #0xe8
	bl ov66_2241DCC
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _02238590
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #0x44
	bne _02238590
	ldr r1, _02238598 // =0x0225B6C4
	mov r0, r5
	ldr r2, [r1]
	mov r1, r4
	ldr r2, [r2, #0x28]
	blx r2
	b _02238590
_02238504:
	ldr r0, _02238598 // =0x0225B6C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp r5, r0
	beq _02238590
	add r0, r2, #0xe8
	bl ov66_2241DCC
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _02238590
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #0x44
	bne _02238590
	ldr r1, _02238598 // =0x0225B6C4
	mov r0, r5
	ldr r2, [r1]
	mov r1, r4
	ldr r2, [r2, #0x2c]
	blx r2
	b _02238590
_0223856C:
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #4
	bne _02238590
	ldr r0, _02238598 // =0x0225B6C4
	ldr r0, [r0]
	add r0, r0, #0x144
	bl ov66_223F0D0
_02238590:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02238598: .word 0x0225B6C4
	arm_func_end ov66_2238460

	arm_func_start ov66_223859C
ov66_223859C: // 0x0223859C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r1, _02238674 // =0x0225B6C4
	add r0, sp, #0
	ldr r1, [r1]
	add r1, r1, #0xb8
	bl ov66_2235FC8
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	bl ov66_2244988
	cmp r0, #0
	moveq r4, #1
	movne r4, #0
	add r0, sp, #0
	bl ov66_223BADC
	cmp r4, #0
	addne sp, sp, #0xc
	movne r0, #0
	ldmneia sp!, {r3, r4, pc}
	ldr r0, _02238674 // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0xc8]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0xcc]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0x148]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0x144]
	cmp r0, #4
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, r1, #0xe8
	bl ov66_22423B0
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_02238674: .word 0x0225B6C4
	arm_func_end ov66_223859C

	arm_func_start ov66_2238678
ov66_2238678: // 0x02238678
	stmfd sp!, {r3, r4, r5, lr}
	ldr r0, _022386D0 // =0x0225B6C4
	ldr r4, [r0]
	ldr r1, [r4, #0x11c]
	cmp r1, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r4, #0x11c]
	ldr r1, [r0]
	add r0, r4, #0x110
	ldr r5, [r1, #0x4c]
	bl ov66_2235B8C
	ldr ip, [r4, #0x114]
	mov r3, r0
	ldr r2, _022386D4 // =0x02258FC4
	mov r0, r5
	mov r1, #0
	str ip, [sp]
	bl ov66_2237348
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_022386D0: .word 0x0225B6C4
_022386D4: .word 0x02258FC4
	arm_func_end ov66_2238678

	arm_func_start ov66_22386D8
ov66_22386D8: // 0x022386D8
	stmfd sp!, {r3, lr}
	ldr r0, _02238720 // =0x0225B6C4
	ldr r3, [r0]
	ldr r1, [r3, #0x10c]
	cmp r1, #0
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	mov r1, #0
	str r1, [r3, #0x10c]
	mov r2, #0x10
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02238724 // =0x02258FD4
	ldr r0, [r0, #0x4c]
	add r3, r3, #0xfc
	bl ov66_2237348
	mov r0, #1
	ldmia sp!, {r3, pc}
	.align 2, 0
_02238720: .word 0x0225B6C4
_02238724: .word 0x02258FD4
	arm_func_end ov66_22386D8

	arm_func_start ov66_2238728
ov66_2238728: // 0x02238728
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _02238780 // =0x0225B6C4
	ldr r3, [r0]
	ldr r0, [r3, #0x68]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {pc}
	mov r2, #1
	str r2, [sp]
	add r0, r3, #0x74
	str r0, [sp, #4]
	mov r0, #0x44
	str r0, [sp, #8]
	mov r1, #0
	ldr r0, [r3, #0x4c]
	mov r3, r1
	bl ov66_2237170
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_02238780: .word 0x0225B6C4
	arm_func_end ov66_2238728

	arm_func_start ov66_2238784
ov66_2238784: // 0x02238784
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r1, _022387D4 // =0x0225B6C4
	add r0, sp, #0
	ldr r1, [r1]
	add r1, r1, #0xb8
	bl ov66_2235FC8
	ldr r0, [sp]
	ldr r1, _022387D8 // =ov66_223A6F4
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	mov r2, #0
	bl FUN_02245D20
	add r0, sp, #0
	bl ov66_223BADC
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_022387D4: .word 0x0225B6C4
_022387D8: .word ov66_223A6F4
	arm_func_end ov66_2238784

	arm_func_start ov66_22387DC
ov66_22387DC: // 0x022387DC
	stmfd sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _0223884C // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0x148]
	cmp r0, #0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, pc}
	add r0, sp, #4
	add r1, r1, #0xb8
	bl ov66_2235FC8
	ldr r0, [sp, #4]
	mov ip, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r2, _02238850 // =0x02258FE4
	ldr r3, _02238854 // =ov66_223A750
	sub r1, ip, #1
	str ip, [sp]
	bl FUN_022456B0
	add r0, sp, #4
	bl ov66_223BADC
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.align 2, 0
_0223884C: .word 0x0225B6C4
_02238850: .word 0x02258FE4
_02238854: .word ov66_223A750
	arm_func_end ov66_22387DC

	arm_func_start ov66_2238858
ov66_2238858: // 0x02238858
	stmfd sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _022388D4 // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0x144]
	cmp r0, #4
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, pc}
	add r0, sp, #4
	add r1, r1, #0xb8
	bl ov66_2235FC8
	ldr r0, [sp, #4]
	mov ip, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r2, _022388D8 // =0x02258FF4
	ldr r3, _022388DC // =ov66_223A860
	sub r1, ip, #1
	str ip, [sp]
	bl FUN_022456B0
	add r0, sp, #4
	bl ov66_223BADC
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.align 2, 0
_022388D4: .word 0x0225B6C4
_022388D8: .word 0x02258FF4
_022388DC: .word ov66_223A860
	arm_func_end ov66_2238858

	arm_func_start ov66_22388E0
ov66_22388E0: // 0x022388E0
	cmp r0, #0xa
	addls pc, pc, r0, lsl #2
	b _02238930
_022388EC: // jump table
	b _02238918 // case 0
	b _02238930 // case 1
	b _02238930 // case 2
	b _02238930 // case 3
	b _02238930 // case 4
	b _02238930 // case 5
	b _02238920 // case 6
	b _02238928 // case 7
	b _02238928 // case 8
	b _02238928 // case 9
	b _02238928 // case 10
_02238918:
	mov r0, #0
	bx lr
_02238920:
	mov r0, #1
	bx lr
_02238928:
	mov r0, #2
	bx lr
_02238930:
	mov r0, #2
	bx lr
	arm_func_end ov66_22388E0

	arm_func_start ov66_2238938
ov66_2238938: // 0x02238938
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r3
	mov r3, #0
	cmp r0, #0
	ldr r4, [sp, #0x18]
	str r3, [r5]
	mov r7, r1
	mov r6, r2
	str r3, [r4]
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #4
	movlo r0, #2
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r0, [r7]
	bl ov66_22388E0
	cmp r0, #2
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r1, r7, #4
	str r1, [r5]
	sub r1, r6, #4
	str r1, [r4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2238938

	arm_func_start ov66_223899C
ov66_223899C: // 0x0223899C
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x38
	ldr r3, _02238C68 // =0x0225B6C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r5, #0
	add r4, sp, #0x1c
	add r3, sp, #0x20
	str r5, [sp, #0x20]
	str r5, [sp, #0x1c]
	str r4, [sp]
	bl ov66_2238938
	movs r4, r0
	bne _022389F0
	ldr r0, [sp, #0x1c]
	cmp r0, #0x94
	movne r4, #2
_022389F0:
	cmp r4, #0
	bne _02238A08
	ldr r0, [sp, #0x20]
	ldr r1, [sp, #0x1c]
	mov r2, #0
	bl PPW_LobbyUpdateMyProfile
_02238A08:
	ldr r0, _02238C68 // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	ldr r3, [r0, #0x34]
	mov r0, r4
	blx r3
	cmp r4, #0
	beq _02238A48
	ldr r0, _02238C68 // =0x0225B6C4
	mov r1, #6
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02238A48:
	ldr r0, _02238C68 // =0x0225B6C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov66_2235B8C
	ldr r1, _02238C68 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x28]
	str r1, [sp, #0x24]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x2a]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x2c
	strb r1, [sp, #0x2b]
	bl OS_GetMacAddress
	ldr r0, _02238C6C // =0x02258E58
	ldr r1, _02238C68 // =0x0225B6C4
	ldr r5, [r0, #0x10]
	ldr r1, [r1]
	ldr r4, [r0, #0xc]
	ldr r0, [r1, #0x50]
	ldr r7, [r1, #0x4c]
	cmp r0, #0
	ldr r0, _02238C70 // =0x0225B6D4
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl FUN_02246290
	movs r6, r0
	beq _02238B6C
	ldr r1, _02238C74 // =0x02258EC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov66_223BA6C
	mov r1, r5
	add r0, r6, #0x18
	bl ov66_223BA6C
	add r2, sp, #0x24
	add r4, r2, #0x14
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _02238C78 // =ov66_2238C7C
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	mov r0, #1
	str r0, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
_02238B6C:
	ldr r0, _02238C70 // =0x0225B6D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl FUN_02258338
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02238BDC
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _02238C10
_02238BDC:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_02238C10:
	cmp r0, #0
	moveq r0, #1
	beq _02238C24
	bl FUN_022585F4
	mov r0, #0
_02238C24:
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _02238C70 // =0x0225B6D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _02238C54
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02238C54:
	ldr r0, _02238C70 // =0x0225B6D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02238C68: .word 0x0225B6C4
_02238C6C: .word 0x02258E58
_02238C70: .word 0x0225B6D4
_02238C74: .word 0x02258EC0
_02238C78: .word ov66_2238C7C
	arm_func_end ov66_223899C

	arm_func_start ov66_2238C7C
ov66_2238C7C: // 0x02238C7C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x50
	ldr r3, _02238FFC // =0x0225B6C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0x50
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r5, #0
	add r4, sp, #0x1c
	add r3, sp, #0x20
	str r5, [sp, #0x20]
	str r5, [sp, #0x1c]
	str r4, [sp]
	bl ov66_2238938
	movs r4, r0
	bne _02238CE4
	ldr r0, _02238FFC // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov66_223EB80
	cmp r0, #0
	moveq r4, #2
_02238CE4:
	ldr r0, _02238FFC // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r2, [r0]
	mov r0, r4
	ldr r2, [r2, #0x38]
	blx r2
	cmp r4, #0
	beq _02238D20
	ldr r0, _02238FFC // =0x0225B6C4
	mov r1, #4
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02238D20:
	cmp r0, #0
	beq _02238D40
	ldr r0, _02238FFC // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov66_223CB84
_02238D40:
	ldr r0, _02238FFC // =0x0225B6C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov66_2235B8C
	ldr r1, _02238FFC // =0x0225B6C4
	add r2, sp, #0x38
	mov r3, #0
	ldr r1, [r1]
	mov r4, r0
	add r0, r1, #0x110
	str r3, [r2]
	str r3, [r2, #4]
	str r3, [r2, #8]
	str r3, [r2, #0xc]
	str r3, [r2, #0x10]
	str r3, [r2, #0x14]
	bl ov66_2235B8C
	ldr r1, _02238FFC // =0x0225B6C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x28]
	str r1, [sp, #0x24]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x2a]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x2c
	strb r1, [sp, #0x2b]
	bl OS_GetMacAddress
	ldrh r1, [sp, #0x28]
	ldrb r0, [sp, #0x2a]
	ldrb sb, [sp, #0x2b]
	ldrb lr, [sp, #0x2c]
	ldrb ip, [sp, #0x2d]
	ldrb r8, [sp, #0x2e]
	ldrb r7, [sp, #0x2f]
	ldrb r6, [sp, #0x30]
	ldrb r5, [sp, #0x31]
	ldrh r3, [sp, #0x32]
	ldr sl, [sp, #0x24]
	ldr r2, [sp, #0x34]
	strh r1, [sp, #0x3c]
	strb r0, [sp, #0x3e]
	str sl, [sp, #0x38]
	strb sb, [sp, #0x3f]
	strb lr, [sp, #0x40]
	strb ip, [sp, #0x41]
	strb r8, [sp, #0x42]
	strb r7, [sp, #0x43]
	strb r6, [sp, #0x44]
	strb r5, [sp, #0x45]
	ldr r0, _02239000 // =0x02258E58
	str r2, [sp, #0x48]
	ldr r1, _02238FFC // =0x0225B6C4
	strh r3, [sp, #0x46]
	ldr r3, [r4, #4]
	ldr r2, [r1]
	str r3, [sp, #0x4c]
	ldr r1, [r2, #0x50]
	ldr r5, [r0, #0x14]
	ldr r4, [r0, #0xc]
	ldr r0, _02239004 // =0x0225B6D4
	cmp r1, #0
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	ldr r7, [r2, #0x4c]
	addne sp, sp, #0x50
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r0, #0x40
	bl FUN_02246290
	movs r6, r0
	beq _02238F00
	ldr r1, _02239008 // =0x02258EC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov66_223BA6C
	mov r1, r5
	add r0, r6, #0x18
	bl ov66_223BA6C
	add r2, sp, #0x38
	add r4, r2, #0x18
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _0223900C // =ov66_2239010
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	mov r0, #1
	str r0, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
_02238F00:
	ldr r0, _02239004 // =0x0225B6D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl FUN_02258338
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02238F70
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _02238FA4
_02238F70:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_02238FA4:
	cmp r0, #0
	moveq r0, #1
	beq _02238FB8
	bl FUN_022585F4
	mov r0, #0
_02238FB8:
	cmp r0, #0
	addne sp, sp, #0x50
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r0, _02239004 // =0x0225B6D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x50
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	beq _02238FE8
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02238FE8:
	ldr r0, _02239004 // =0x0225B6D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02238FFC: .word 0x0225B6C4
_02239000: .word 0x02258E58
_02239004: .word 0x0225B6D4
_02239008: .word 0x02258EC0
_0223900C: .word ov66_2239010
	arm_func_end ov66_2238C7C

	arm_func_start ov66_2239010
ov66_2239010: // 0x02239010
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x38
	ldr r3, _022392F0 // =0x0225B6C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r5, #0
	add r4, sp, #0x1c
	add r3, sp, #0x20
	str r5, [sp, #0x20]
	str r5, [sp, #0x1c]
	str r4, [sp]
	bl ov66_2238938
	ldr r2, _022392F0 // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r3, [r2]
	mov r4, r0
	ldr r2, [sp, #0x1c]
	add r0, r3, #0x144
	bl ov66_223ED8C
	cmp r0, #0
	moveq r4, #2
	cmp r4, #0
	beq _02239098
	ldr r0, _022392F0 // =0x0225B6C4
	mov r1, #5
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02239098:
	ldr r0, _022392F0 // =0x0225B6C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov66_223EBAC
	ldr r0, _022392F0 // =0x0225B6C4
	ldr r2, [sp, #0x1c]
	ldr r0, [r0]
	ldr r1, [sp, #0x20]
	ldr r3, [r0, #0x3c]
	mov r0, r4
	mov r2, r2, lsr #3
	blx r3
	ldr r0, _022392F0 // =0x0225B6C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov66_2235B8C
	ldr r1, _022392F0 // =0x0225B6C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x28]
	str r1, [sp, #0x24]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x2a]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x2c
	strb r1, [sp, #0x2b]
	bl OS_GetMacAddress
	ldr r0, _022392F4 // =0x02258E58
	ldr r1, _022392F0 // =0x0225B6C4
	ldr r5, [r0, #8]
	ldr r1, [r1]
	ldr r4, [r0, #0xc]
	ldr r0, [r1, #0x50]
	ldr r7, [r1, #0x4c]
	cmp r0, #0
	ldr r0, _022392F8 // =0x0225B6D4
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl FUN_02246290
	movs r6, r0
	beq _022391F4
	ldr r1, _022392FC // =0x02258EC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov66_223BA6C
	mov r1, r5
	add r0, r6, #0x18
	bl ov66_223BA6C
	add r2, sp, #0x24
	add r4, r2, #0x14
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _02239300 // =ov66_2239304
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	mov r0, #1
	str r0, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
_022391F4:
	ldr r0, _022392F8 // =0x0225B6D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl FUN_02258338
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02239264
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov66_22588F4
	b _02239298
_02239264:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl FUN_022588D0
_02239298:
	cmp r0, #0
	moveq r0, #1
	beq _022392AC
	bl FUN_022585F4
	mov r0, #0
_022392AC:
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _022392F8 // =0x0225B6D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _022392DC
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022392DC:
	ldr r0, _022392F8 // =0x0225B6D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_022392F0: .word 0x0225B6C4
_022392F4: .word 0x02258E58
_022392F8: .word 0x0225B6D4
_022392FC: .word 0x02258EC0
_02239300: .word ov66_2239304
	arm_func_end ov66_2239010

	arm_func_start ov66_2239304
ov66_2239304: // 0x02239304
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r3, _022393BC // =0x0225B6C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, pc}
	mov r3, #0
	add ip, sp, #4
	str r3, [sp, #8]
	str r3, [sp, #4]
	add r3, sp, #8
	str ip, [sp]
	bl ov66_2238938
	ldr r1, [sp, #4]
	mov r4, r0
	cmp r1, #0x2d8
	movne r4, #2
	cmp r4, #0
	beq _02239378
	ldr r0, _022393BC // =0x0225B6C4
	mov r1, #7
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
_02239378:
	ldr r0, _022393BC // =0x0225B6C4
	ldr r1, [sp, #8]
	ldr r0, [r0]
	add r0, r0, #0x144
	bl ov66_223EE0C
	ldr r0, _022393BC // =0x0225B6C4
	ldr r1, [sp, #8]
	ldr r2, [r0]
	mov r0, r4
	ldr r2, [r2, #0x40]
	blx r2
	ldr r0, _022393BC // =0x0225B6C4
	mov r1, #2
	ldr r0, [r0]
	str r1, [r0, #0x144]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_022393BC: .word 0x0225B6C4
	arm_func_end ov66_2239304

	arm_func_start ov66_22393C0
ov66_22393C0: // 0x022393C0
	stmfd sp!, {r3, lr}
	ldr r1, _02239404 // =0x0225B6C4
	ldr r2, [r1]
	cmp r2, #0
	ldrne r1, [r2]
	cmpne r1, #9
	ldmeqia sp!, {r3, pc}
	mov r3, #0
	ldr r1, _02239404 // =0x0225B6C4
	str r3, [r2, #0x6c]
	ldr r1, [r1]
	cmp r0, #0
	moveq r3, #2
	ldr r1, [r1, #0x44]
	mov r0, r3
	blx r1
	ldmia sp!, {r3, pc}
	.align 2, 0
_02239404: .word 0x0225B6C4
	arm_func_end ov66_22393C0

	arm_func_start ov66_2239408
ov66_2239408: // 0x02239408
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2b4
	mov r6, r3
	cmp r6, #5
	mov r7, r2
	addeq sp, sp, #0x2b4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	cmp r0, #0
	beq _022398E4
	ldr r0, _0223998C // =0x0225B6C4
	mov r4, #0
	ldr r3, [r0]
	mov r2, #5
	add r0, sp, #0x288
	add r1, sp, #0x27c
	str r4, [r3, #0x70]
	str r2, [sp, #0x298]
	str r4, [sp, #0x29c]
	str r4, [sp, #0x2a0]
	str r4, [sp, #0x2a4]
	bl DWC_GetDateTime
	add r0, sp, #0x288
	add r1, sp, #0x27c
	bl RTC_ConvertDateTimeToSecond
	ldr r4, [sp, #0x298]
	str r0, [sp, #0x29c]
	str r1, [sp, #0x2a0]
	ldr r2, _0223998C // =0x0225B6C4
	str r4, [sp, #0x268]
	add r3, sp, #0x29c
	ldr r4, [r2]
	add ip, sp, #0x26c
	ldmia r3, {r2, r3}
	stmia ip, {r2, r3}
	ldr r1, [sp, #0x2a4]
	mov r0, #1
	str r0, [sp, #0x278]
	add r5, sp, #0x268
	str r1, [sp, #0x274]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _022394F0
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #0xfc]
	add r0, r4, #0x100
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x108]
	str r0, [r4, #0x10c]
_022394F0:
	ldr r1, _02239990 // =ov66_223A5A4
	mov r3, #1
	mov r0, r7
	mov r2, r6
	str r3, [r4, #0x128]
	bl FUN_02245A80
	ldr r2, _0223998C // =0x0225B6C4
	add r0, sp, #0x258
	add r1, sp, #0x24c
	ldr r4, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x258
	add r1, sp, #0x24c
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #3
	str r0, [sp, #0x1fc]
	sbc r0, r1, #0
	str r0, [sp, #0x200]
	ldr r0, _02239994 // =ov66_22386D8
	mov r1, #3
	str r1, [sp, #0x204]
	str r0, [sp, #0x210]
	str r2, [sp, #0x208]
	str r2, [sp, #0x20c]
	add r0, sp, #0x214
	mov r1, #1
	add r2, sp, #0x1fc
	bl ov66_22369FC
	mov r0, #0
	add r2, sp, #0x218
	add r7, sp, #0x234
	add r1, r4, #0x134
	strb r0, [sp, #9]
	strb r0, [sp, #8]
	ldr r0, [sp, #0x214]
	add r6, r2, #8
	str r0, [sp, #0x230]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0x2c
	add r2, sp, #0x230
	str r4, [r7, #0x10]
	bl FUN_0223AABC
	ldr r1, _0223998C // =0x0225B6C4
	add r0, sp, #0x1ec
	ldr r4, [r1]
	add r1, sp, #0x1e0
	bl DWC_GetDateTime
	add r0, sp, #0x1ec
	add r1, sp, #0x1e0
	bl RTC_ConvertDateTimeToSecond
	ldr r6, _02239998 // =ov66_2238678
	mov r7, #0
	subs r5, r0, #4
	str r0, [sp, #0x190]
	sbc r3, r1, #0
	mov ip, #4
	add r0, sp, #0x1a8
	add r2, sp, #0x190
	mov r1, #2
	str ip, [sp, #0x198]
	str r7, [sp, #0x19c]
	str r7, [sp, #0x1a0]
	str r6, [sp, #0x1a4]
	str r5, [sp, #0x190]
	str r3, [sp, #0x194]
	bl ov66_22369FC
	ldr r0, [sp, #0x1a8]
	add r6, sp, #0x1ac
	add r5, sp, #0x1c8
	str r0, [sp, #0x1c4]
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	add r2, r6, #8
	add r0, r5, #8
	ldmia r2, {r2, r3}
	stmia r0, {r2, r3}
	ldr r2, [r6, #0x10]
	ldr r3, [r6, #0x14]
	str r2, [r5, #0x10]
	add r1, r4, #0x134
	add r0, sp, #0x24
	add r2, sp, #0x1c4
	strb r7, [sp, #7]
	strb r7, [sp, #6]
	str r3, [r5, #0x14]
	bl FUN_0223AABC
	ldr r1, _0223998C // =0x0225B6C4
	add r0, sp, #0x180
	ldr r4, [r1]
	add r1, sp, #0x174
	bl DWC_GetDateTime
	add r0, sp, #0x180
	add r1, sp, #0x174
	bl RTC_ConvertDateTimeToSecond
	mov r2, #4
	str r2, [sp, #0x12c]
	mov r2, r7
	str r0, [sp, #0x124]
	subs r0, r0, #4
	str r0, [sp, #0x124]
	str r2, [sp, #0x130]
	str r2, [sp, #0x134]
	ldr r2, _0223999C // =ov66_22387DC
	str r1, [sp, #0x128]
	sbc r0, r1, #0
	str r2, [sp, #0x138]
	str r0, [sp, #0x128]
	add r0, sp, #0x13c
	mov r1, #5
	add r2, sp, #0x124
	bl ov66_22369FC
	ldr r0, [sp, #0x13c]
	add r7, sp, #0x140
	add r6, sp, #0x15c
	str r0, [sp, #0x158]
	ldmia r7, {r2, r3}
	mov ip, #0
	stmia r6, {r2, r3}
	add r1, r7, #8
	ldmia r1, {r2, r3}
	add r0, r6, #8
	stmia r0, {r2, r3}
	ldr r5, [r7, #0x10]
	ldr r3, [r7, #0x14]
	add r0, sp, #0x1c
	add r2, sp, #0x158
	add r1, r4, #0x134
	strb ip, [sp, #5]
	strb ip, [sp, #4]
	str r5, [r6, #0x10]
	str r3, [r6, #0x14]
	bl FUN_0223AABC
	ldr r2, _0223998C // =0x0225B6C4
	add r0, sp, #0x114
	add r1, sp, #0x108
	ldr r4, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x114
	add r1, sp, #0x108
	bl RTC_ConvertDateTimeToSecond
	mov r2, #3
	str r2, [sp, #0xc0]
	mov r2, #0
	str r0, [sp, #0xb8]
	subs r0, r0, #3
	str r0, [sp, #0xb8]
	str r2, [sp, #0xc4]
	str r2, [sp, #0xc8]
	ldr r2, _022399A0 // =ov66_2238784
	str r1, [sp, #0xbc]
	sbc r0, r1, #0
	str r2, [sp, #0xcc]
	str r0, [sp, #0xbc]
	add r0, sp, #0xd0
	mov r1, #4
	add r2, sp, #0xb8
	bl ov66_22369FC
	mov r0, #0
	add r2, sp, #0xd4
	add r7, sp, #0xf0
	strb r0, [sp, #3]
	strb r0, [sp, #2]
	ldr r0, [sp, #0xd0]
	add r6, r2, #8
	str r0, [sp, #0xec]
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r1, [r7, #0x10]
	str r0, [r7, #0x14]
	add r0, sp, #0x14
	add r2, sp, #0xec
	add r1, r4, #0x134
	bl FUN_0223AABC
	ldr r2, _0223998C // =0x0225B6C4
	add r0, sp, #0xa8
	add r1, sp, #0x9c
	ldr r4, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0xa8
	add r1, sp, #0x9c
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #4
	str r0, [sp, #0x4c]
	sbc r0, r1, #0
	str r0, [sp, #0x50]
	ldr r0, _022399A4 // =ov66_2238858
	mov r1, #4
	str r1, [sp, #0x54]
	str r0, [sp, #0x60]
	str r2, [sp, #0x58]
	str r2, [sp, #0x5c]
	add r0, sp, #0x64
	mov r1, #6
	add r2, sp, #0x4c
	bl ov66_22369FC
	mov r0, #0
	add r2, sp, #0x68
	add r7, sp, #0x84
	add r1, r4, #0x134
	strb r0, [sp, #1]
	strb r0, [sp]
	ldr r0, [sp, #0x64]
	add r6, r2, #8
	str r0, [sp, #0x80]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0xc
	add r2, sp, #0x80
	str r4, [r7, #0x10]
	bl FUN_0223AABC
	ldr r0, _0223998C // =0x0225B6C4
	add r1, sp, #0x40
	ldr r4, [r0]
	add r3, sp, #0x2a8
	ldr r0, [r4, #0x100]
	ldr r2, [r4, #0xfc]
	str r0, [sp, #0x40]
	ldr r0, [r4, #0x104]
	str r2, [sp, #0x48]
	str r0, [sp, #0x44]
	ldmia r1, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r5, [r4, #0x124]
	add r0, r4, #0x110
	bl ov66_2235B8C
	ldr r3, [r4, #0x114]
	mov r2, r0
	ldr r4, [r4, #0xc]
	add r1, sp, #0x2a8
	mov r0, r5
	blx r4
	add sp, sp, #0x2b4
	ldmia sp!, {r4, r5, r6, r7, pc}
_022398E4:
	ldr r0, _0223998C // =0x0225B6C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0x34
	mov r0, r1
_022398F8:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022398F8
	mov r2, #5
	add r1, sp, #0x34
	add r0, r4, #0xbc
	str r2, [r4, #0xb8]
	bl ov66_223BC5C
	mov r1, #0
	str r1, [r4, #0xc8]
	add r0, sp, #0x34
	str r1, [r4, #0xcc]
	bl ov66_223BADC
	ldr r0, _0223998C // =0x0225B6C4
	ldr r1, [r0]
	ldr r0, [r1, #0x70]
	cmp r0, #3
	movge r0, #0
	addlt r0, r0, #1
	strlt r0, [r1, #0x70]
	movlt r0, #1
	cmp r0, #0
	beq _02239970
	ldr r0, _0223998C // =0x0225B6C4
	mov r1, #4
	ldr r0, [r0]
	add sp, sp, #0x2b4
	str r1, [r0]
	ldmia sp!, {r4, r5, r6, r7, pc}
_02239970:
	ldr r0, _0223998C // =0x0225B6C4
	mov r1, #3
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x2b4
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_0223998C: .word 0x0225B6C4
_02239990: .word ov66_223A5A4
_02239994: .word ov66_22386D8
_02239998: .word ov66_2238678
_0223999C: .word ov66_22387DC
_022399A0: .word ov66_2238784
_022399A4: .word ov66_2238858
	arm_func_end ov66_2239408

	arm_func_start ov66_22399A8
ov66_22399A8: // 0x022399A8
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	mov r6, r3
	cmp r6, #5
	mov r7, r0
	addeq sp, sp, #0x30
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r7, #0
	beq _02239AD8
	cmp r6, #4
	addls pc, pc, r6, lsl #2
	b _02239A04
_022399D8: // jump table
	b _02239A04 // case 0
	b _022399EC // case 1
	b _022399EC // case 2
	b _022399F4 // case 3
	b _022399FC // case 4
_022399EC:
	ldr r1, _02239B44 // =0x02258C4C
	b _02239A08
_022399F4:
	ldr r1, _02239B48 // =0x02258C0C
	b _02239A08
_022399FC:
	ldr r1, _02239B4C // =0x02258C2C
	b _02239A08
_02239A04:
	mov r1, #0
_02239A08:
	cmp r1, #0
	bne _02239A24
	ldr r0, _02239B50 // =0x0225B6C4
	mov r3, #1
	ldr r4, [r0]
	mov r0, #0xa
	stmia r4, {r0, r3}
_02239A24:
	mov r0, r2
	bl FUN_02245C54
	ldr r0, _02239B50 // =0x0225B6C4
	mov r1, #0
	ldr r4, [r0]
	add ip, sp, #0x24
	str r1, [r4, #0x10c]
	ldr r1, [r4, #0xfc]
	add r0, r4, #0x100
	str r1, [sp, #0x20]
	ldmia r0, {r2, r3}
	stmia ip, {r2, r3}
	ldr r1, [r4, #0x108]
	add r0, sp, #0x10
	str r1, [sp, #0x2c]
	str r6, [sp, #0x20]
	str r6, [sp, #0xc]
	ldmia ip, {r2, r3}
	stmia r0, {r2, r3}
	mov r0, #1
	str r0, [sp, #0x1c]
	add r5, sp, #0xc
	str r1, [sp, #0x18]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02239ACC
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #0xfc]
	add r0, r4, #0x100
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x108]
	str r0, [r4, #0x10c]
_02239ACC:
	mov r0, #1
	str r0, [r4, #0x128]
	b _02239B24
_02239AD8:
	ldr r0, _02239B50 // =0x0225B6C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0
	mov r0, r1
_02239AEC:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02239AEC
	mov r2, #5
	add r1, sp, #0
	add r0, r4, #0xd4
	str r2, [r4, #0xd0]
	bl ov66_223BC5C
	mov r1, #0
	str r1, [r4, #0xe0]
	add r0, sp, #0
	str r1, [r4, #0xe4]
	bl ov66_223BADC
_02239B24:
	ldr r1, _02239B50 // =0x0225B6C4
	mov r0, r7
	ldr r2, [r1]
	mov r1, r6
	ldr r2, [r2, #8]
	blx r2
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02239B44: .word 0x02258C4C
_02239B48: .word 0x02258C0C
_02239B4C: .word 0x02258C2C
_02239B50: .word 0x0225B6C4
	arm_func_end ov66_22399A8

	arm_func_start ov66_2239B54
ov66_2239B54: // 0x02239B54
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x60
	mov r2, r0
	mov r7, r1
	add r0, sp, #0x38
	mov r1, r2
	bl ov66_223BA6C
	ldr r1, _02239D5C // =0x0225B6C4
	add r0, sp, #0x20
	ldr r5, [r1]
	add r6, sp, #0x38
	add r1, r5, #0xb8
	bl ov66_2235FC8
	add r0, sp, #0x20
	mov r1, r6
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x20
	bl ov66_223BADC
	cmp r4, #0
	ldrne r5, [r5, #0xb8]
	bne _02239BDC
	add r0, sp, #0x2c
	add r1, r5, #0xd0
	bl ov66_2235FC8
	add r0, sp, #0x2c
	mov r1, r6
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x2c
	bl ov66_223BADC
	cmp r4, #0
	ldrne r5, [r5, #0xd0]
	moveq r5, #5
_02239BDC:
	add r0, sp, #0x38
	bl ov66_223BADC
	cmp r5, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r5, #0
	bne _02239C14
	ldr r0, _02239D5C // =0x0225B6C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241C44
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02239C14:
	ldr r0, _02239D5C // =0x0225B6C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	movs r4, r0
	bne _02239C5C
	ldr r0, _02239D5C // =0x0225B6C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241C44
	ldr r0, _02239D5C // =0x0225B6C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	mov r4, r0
_02239C5C:
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r0, [r4, #4]
	add r1, sp, #0x54
	str r0, [sp, #0x50]
	add r0, r4, #8
	ldmia r0, {r2, r3}
	stmia r1, {r2, r3}
	ldr ip, [r4, #0x10]
	add r0, sp, #0x10
	str ip, [sp, #0x5c]
	str r5, [sp, #0x50]
	str r5, [sp, #0xc]
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r6, sp, #0xc
	mov r3, #1
	mov r1, r6
	add r0, r4, #4
	mov r2, #0x10
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02239CF4
	ldr r0, [r6]
	add r1, r6, #4
	str r0, [r4, #4]
	add r0, r4, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r6, #0xc]
	mov r0, #1
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
_02239CF4:
	mov r0, #1
	str r0, [r4, #0x30]
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	add r0, sp, #0
	add r3, sp, #0x44
	str r5, [sp, #8]
	str r2, [sp]
	str r1, [sp, #4]
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, [r4, #0x38]
	cmp r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _02239D5C // =0x0225B6C4
	mov r0, r7
	ldr r2, [r1]
	mov r1, r3
	ldr r2, [r2, #0x20]
	blx r2
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02239D5C: .word 0x0225B6C4
	arm_func_end ov66_2239B54

	arm_func_start ov66_2239D60
ov66_2239D60: // 0x02239D60
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x60
	mov r2, r0
	mov r6, r1
	add r0, sp, #0x38
	mov r1, r2
	bl ov66_223BA6C
	ldr r1, _02239F80 // =0x0225B6C4
	add r0, sp, #0x20
	ldr r5, [r1]
	add r7, sp, #0x38
	add r1, r5, #0xb8
	bl ov66_2235FC8
	add r0, sp, #0x20
	mov r1, r7
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x20
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r5, #0xb8]
	bne _02239DE8
	add r0, sp, #0x2c
	add r1, r5, #0xd0
	bl ov66_2235FC8
	add r0, sp, #0x2c
	mov r1, r7
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x2c
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r5, #0xd0]
	moveq r4, #5
_02239DE8:
	add r0, sp, #0x38
	bl ov66_223BADC
	cmp r4, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r4, #0
	bne _02239E5C
	ldr r0, _02239F80 // =0x0225B6C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _02239F80 // =0x0225B6C4
	mov r0, r6
	ldr r1, [r1]
	ldr r1, [r1, #0x10]
	blx r1
	ldr r0, _02239F80 // =0x0225B6C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241D40
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02239E5C:
	ldr r0, _02239F80 // =0x0225B6C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	movs r4, r0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r0, [r4, #4]
	add lr, sp, #0x54
	str r0, [sp, #0x50]
	add r0, r4, #8
	ldmia r0, {r2, r3}
	stmia lr, {r2, r3}
	ldr ip, [r4, #0x10]
	mov r1, #5
	add r0, sp, #0x10
	str ip, [sp, #0x5c]
	str r1, [sp, #0x50]
	str r1, [sp, #0xc]
	add r5, sp, #0xc
	ldmia lr, {r2, r3}
	stmia r0, {r2, r3}
	mov r3, #1
	mov r1, r5
	add r0, r4, #4
	mov r2, #0x10
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02239F18
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #4]
	add r0, r4, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
_02239F18:
	mov r0, #1
	str r0, [r4, #0x30]
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	add r0, sp, #0
	add r3, sp, #0x44
	str r5, [sp, #8]
	str r2, [sp]
	str r1, [sp, #4]
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, [r4, #0x38]
	cmp r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _02239F80 // =0x0225B6C4
	mov r0, r6
	ldr r2, [r1]
	mov r1, r3
	ldr r2, [r2, #0x20]
	blx r2
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02239F80: .word 0x0225B6C4
	arm_func_end ov66_2239D60

	arm_func_start ov66_2239F84
ov66_2239F84: // 0x02239F84
	stmfd sp!, {r3, lr}
	mov ip, r1
	mov r3, r2
	mov r1, r0
	mov r2, ip
	mov r0, #0
	bl ov66_2239FA4
	ldmia sp!, {r3, pc}
	arm_func_end ov66_2239F84

	arm_func_start ov66_2239FA4
ov66_2239FA4: // 0x02239FA4
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x58
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, #5
	beq _0223A040
	add r0, sp, #0x28
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _0223A1C4 // =0x0225B6C4
	add r0, sp, #4
	ldr r8, [r1]
	add sb, sp, #0x28
	add r1, r8, #0xb8
	bl ov66_2235FC8
	add r0, sp, #4
	mov r1, sb
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #4
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r8, #0xb8]
	bne _0223A038
	add r0, sp, #0x10
	add r1, r8, #0xd0
	bl ov66_2235FC8
	add r0, sp, #0x10
	mov r1, sb
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x10
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r8, #0xd0]
	moveq r4, #5
_0223A038:
	add r0, sp, #0x28
	bl ov66_223BADC
_0223A040:
	cmp r7, #0
	beq _0223A054
	cmp r4, #5
	addeq sp, sp, #0x58
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223A054:
	mov r7, #0
	add r2, sp, #0x4c
	mov r3, #6
	add r0, sp, #0x1c
	mov r1, r5
	str r7, [sp, #0x34]
	str r3, [sp, #0x38]
	str r7, [sp, #0x3c]
	str r7, [sp, #0x40]
	str r7, [sp, #0x44]
	str r7, [sp, #0x48]
	str r7, [r2]
	str r7, [r2, #4]
	str r7, [r2, #8]
	bl ov66_223BA6C
	add r0, sp, #0x1c
	add r1, sp, #0x34
	bl ov66_2241184
	cmp r0, #0
	moveq r5, #1
	movne r5, r7
	add r0, sp, #0x1c
	bl ov66_223BADC
	cmp r5, #0
	beq _0223A0C8
	add r0, sp, #0x4c
	bl ov66_223B8E8
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223A0C8:
	ldr r0, [sp, #0x40]
	cmp r0, #0
	beq _0223A190
	cmp r0, #1
	bne _0223A1B4
	ldr r0, _0223A1C4 // =0x0225B6C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov66_2241DCC
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _0223A11C
	add r0, sp, #0x4c
	bl ov66_223B8E8
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223A11C:
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	beq _0223A164
	cmp r0, #1
	bne _0223A1B4
	ldr r1, _0223A1C4 // =0x0225B6C4
	add r0, sp, #0x4c
	ldr r5, [r1]
	bl ov66_2235B8C
	ldr r1, [sp, #0x50]
	mov r3, r0
	str r1, [sp]
	ldr r2, [sp, #0x44]
	ldr r5, [r5, #0x18]
	mov r0, r6
	mov r1, r4
	blx r5
	b _0223A1B4
_0223A164:
	ldr r1, _0223A1C4 // =0x0225B6C4
	add r0, sp, #0x4c
	ldr r5, [r1]
	bl ov66_2235B8C
	mov r3, r0
	ldr r2, [sp, #0x44]
	ldr r5, [r5, #0x14]
	mov r0, r6
	mov r1, r4
	blx r5
	b _0223A1B4
_0223A190:
	add r0, sp, #0x4c
	bl ov66_2235B8C
	ldr r5, [sp, #0x50]
	mov r3, r0
	add r0, sp, #0x34
	mov r1, r6
	mov r2, r4
	str r5, [sp]
	bl ov66_2238460
_0223A1B4:
	add r0, sp, #0x4c
	bl ov66_223B8E8
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_0223A1C4: .word 0x0225B6C4
	arm_func_end ov66_2239FA4

	arm_func_start ov66_223A1C8
ov66_223A1C8: // 0x0223A1C8
	stmfd sp!, {r3, lr}
	ldr r1, _0223A1E0 // =0x0225B6C4
	ldr r1, [r1]
	ldr r1, [r1, #0x48]
	blx r1
	ldmia sp!, {r3, pc}
	.align 2, 0
_0223A1E0: .word 0x0225B6C4
	arm_func_end ov66_223A1C8

	arm_func_start ov66_223A1E4
ov66_223A1E4: // 0x0223A1E4
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	stmia sp, {r2, r3}
	mov lr, r1
	mov r0, #1
	mov ip, #0
	mov r1, r0
	mov r2, r4
	mov r3, lr
	str ip, [sp, #8]
	bl ov66_2237D24
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223A1E4

	arm_func_start ov66_223A21C
ov66_223A21C: // 0x0223A21C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr lr, [sp, #0x18]
	mov r4, r2
	stmia sp, {r3, lr}
	ldr ip, [sp, #0x1c]
	mov r2, r1
	mov r3, r4
	mov r1, #0
	str ip, [sp, #8]
	bl ov66_2237D24
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223A21C

	arm_func_start ov66_223A250
ov66_223A250: // 0x0223A250
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	ldr r8, [sp, #0x60]
	cmp r0, #0
	mov sl, r1
	mov sb, r2
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	mov r7, #0
	ble _0223A360
	ldr fp, _0223A3A8 // =0x02258FB8
	add r6, sp, #0x24
	add r5, sp, #0x18
	add r4, sp, #0x30
_0223A28C:
	ldr r0, [r8, r7, lsl #2]
	cmp r0, #0x14
	bge _0223A354
	ldr r1, [sb, r7, lsl #2]
	mov r0, r6
	bl ov66_223BA6C
	mov r0, r5
	mov r1, fp
	bl ov66_223BA6C
	mov r0, r4
	mov r1, r6
	mov r2, r5
	mov r3, #0
	bl ov66_223A3B0
	mov r0, r5
	bl ov66_223BADC
	mov r0, r6
	bl ov66_223BADC
	ldr r0, [sp, #0x34]
	cmp r0, #2
	bhs _0223A2EC
	mov r0, r4
	bl ov66_223B578
	b _0223A354
_0223A2EC:
	ldr r1, [sp, #0x30]
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0xd
	ldrne r1, [r1, #0x14]
	add r0, sp, #0xc
	bl ov66_223BA6C
	ldr r0, _0223A3AC // =0x0225B6C4
	add r1, sp, #0xc
	ldr r0, [r0]
	add r0, r0, #0x58
	bl ov66_223BC5C
	add r0, sp, #0xc
	bl ov66_223BADC
	ldr r1, _0223A3AC // =0x0225B6C4
	mov r3, #0
	ldr r2, [r1]
	add r0, sp, #0x30
	str r3, [r2, #0x64]
	ldr r1, [r1]
	mov r2, #6
	str r2, [r1]
	bl ov66_223B578
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223A354:
	add r7, r7, #1
	cmp r7, sl
	blt _0223A28C
_0223A360:
	add r0, sp, #0
	bl ov66_2237A88
	ldr r0, _0223A3AC // =0x0225B6C4
	add r1, sp, #0
	ldr r0, [r0]
	add r0, r0, #0x58
	bl ov66_223BC5C
	add r0, sp, #0
	bl ov66_223BADC
	ldr r0, _0223A3AC // =0x0225B6C4
	mov r3, #1
	ldr r2, [r0]
	mov r1, #6
	str r3, [r2, #0x64]
	ldr r0, [r0]
	str r1, [r0]
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0223A3A8: .word 0x02258FB8
_0223A3AC: .word 0x0225B6C4
	arm_func_end ov66_223A250

	arm_func_start ov66_223A3B0
ov66_223A3B0: // 0x0223A3B0
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x2c
	mov sl, r0
	mov r4, #0
	str r4, [sl]
	str r4, [sl, #4]
	add r0, sp, #0x20
	mov sb, r2
	str r3, [sp]
	str r4, [sl, #8]
	mov r7, #1
	bl ov66_223B938
	sub r5, sp, #4
	add fp, sp, #0x20
	add r6, sp, #0x14
	mvn r4, #0
	b _0223A4D0
_0223A3F4:
	cmp r8, #0
	beq _0223A488
	mov r0, r6
	mov r1, fp
	mov r2, #0
	mov r3, r8
	bl ov66_223A594
	mov r1, #0
	add r0, sp, #7
	strb r1, [r0]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223A458
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223A448
	mov r1, r6
	bl ov66_223B938
_0223A448:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223A47C
_0223A458:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #6]
	mov r2, r2
	strb r2, [r5]
	ldr r2, [r5]
	mov r0, sl
	mov r1, r6
	bl ov66_223B548
_0223A47C:
	mov r0, r6
	bl ov66_223BADC
	add r7, r7, #1
_0223A488:
	ldr r0, [sb]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sb, #4]
	bne _0223A4A8
	ldrb r0, [sb]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223A4A8:
	add r2, r8, r0
	add r0, sp, #8
	mov r1, fp
	mov r3, r4
	bl ov66_223A594
	mov r0, fp
	add r1, sp, #8
	bl ov66_223BC5C
	add r0, sp, #8
	bl ov66_223BADC
_0223A4D0:
	ldr r0, [sp]
	cmp r7, r0
	beq _0223A4F8
	mov r0, fp
	mov r1, sb
	mov r2, #0
	bl ov66_223BDC4
	mov r8, r0
	cmp r8, r4
	bne _0223A3F4
_0223A4F8:
	ldr r0, [sp, #0x20]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sp, #0x24]
	bne _0223A518
	ldrb r0, [sp, #0x20]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223A518:
	cmp r0, #0
	beq _0223A584
	mov r2, #0
	strb r2, [sp, #5]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223A564
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223A554
	add r1, sp, #0x20
	bl ov66_223B938
_0223A554:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223A584
_0223A564:
	strb r2, [sp, #4]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	add r1, sp, #0x20
	mov r0, sl
	bl ov66_223B548
_0223A584:
	add r0, sp, #0x20
	bl ov66_223BADC
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_223A3B0

	arm_func_start ov66_223A594
ov66_223A594: // 0x0223A594
	stmfd sp!, {r3, lr}
	str r1, [sp]
	bl ov66_223B9B4
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223A594

	arm_func_start ov66_223A5A4
ov66_223A5A4: // 0x0223A5A4
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	cmp r0, #0
	mov sl, r1
	mov sb, r2
	mov r8, r3
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sp, #0x1c
	bl ov66_223BA6C
	ldr r1, _0223A6E4 // =0x0225B6C4
	add r0, sp, #4
	ldr r5, [r1]
	add r6, sp, #0x1c
	add r1, r5, #0xb8
	bl ov66_2235FC8
	add r0, sp, #4
	mov r1, r6
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #4
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r5, #0xb8]
	bne _0223A638
	add r0, sp, #0x10
	add r1, r5, #0xd0
	bl ov66_2235FC8
	add r0, sp, #0x10
	mov r1, r6
	bl ov66_223800C
	mov r4, r0
	add r0, sp, #0x10
	bl ov66_223BADC
	cmp r4, #0
	ldrne r4, [r5, #0xd0]
	moveq r4, #5
_0223A638:
	add r0, sp, #0x1c
	bl ov66_223BADC
	cmp r4, #5
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r4, #0
	addne sp, sp, #0x28
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sb, #0
	mov r7, #0
	ble _0223A6CC
	ldr r5, _0223A6E8 // =ov66_223A21C
	ldr fp, _0223A6E4 // =0x0225B6C4
	mov r6, r7
	mov r4, r7
_0223A674:
	ldr r2, [fp]
	ldr r1, [r8, r7, lsl #2]
	ldr r0, [r2, #0x4c]
	cmp r1, r0
	beq _0223A6C0
	add r0, r2, #0xe8
	bl ov66_2241C44
	str r6, [sp]
	ldr r1, [r8, r7, lsl #2]
	ldr r2, _0223A6EC // =0x02258FC4
	mov r0, sl
	mov r3, r5
	bl FUN_022456B0
	str r4, [sp]
	ldr r1, [r8, r7, lsl #2]
	ldr r2, _0223A6F0 // =0x02258FD4
	mov r0, sl
	mov r3, r5
	bl FUN_022456B0
_0223A6C0:
	add r7, r7, #1
	cmp r7, sb
	blt _0223A674
_0223A6CC:
	ldr r0, _0223A6E4 // =0x0225B6C4
	mov r1, #1
	ldr r0, [r0]
	str r1, [r0, #0xc8]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0223A6E4: .word 0x0225B6C4
_0223A6E8: .word ov66_223A21C
_0223A6EC: .word 0x02258FC4
_0223A6F0: .word 0x02258FD4
	arm_func_end ov66_223A5A4

	arm_func_start ov66_223A6F4
ov66_223A6F4: // 0x0223A6F4
	stmfd sp!, {r3, lr}
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, [r2, #0x1c]
	cmp r0, #0x14
	bne _0223A738
	ldr r0, _0223A748 // =0x0225B6C4
	mov r3, #1
	ldr r2, [r0]
	add r1, sp, #0
	str r3, [r2, #0xcc]
	ldr r0, [r0]
	mov r2, #4
	add r0, r0, #0x134
	str r2, [sp]
	bl ov66_2236CA0
	ldmia sp!, {r3, pc}
_0223A738:
	mov r0, r1
	ldr r1, _0223A74C // =0x02258C6C
	bl FUN_02245C54
	ldmia sp!, {r3, pc}
	.align 2, 0
_0223A748: .word 0x0225B6C4
_0223A74C: .word 0x02258C6C
	arm_func_end ov66_223A6F4

	arm_func_start ov66_223A750
ov66_223A750: // 0x0223A750
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x40
	cmp r0, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r4, pc}
	ldr r0, [sp, #0x48]
	mov r2, #0
	add r1, sp, #0x34
	str r2, [sp, #0x34]
	str r2, [sp, #0x38]
	str r2, [sp, #0x3c]
	bl ov66_2237EF8
	cmp r0, #0
	beq _0223A794
	ldr r0, [sp, #0x38]
	cmp r0, #8
	beq _0223A7F8
_0223A794:
	ldr r2, _0223A858 // =0x0225B6C4
	add r0, sp, #0x18
	add r1, sp, #0x28
	ldr r4, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x18
	add r1, sp, #0x28
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x14c]
	str r1, [r4, #0x150]
	add r2, r4, #0x14c
	add r0, sp, #0x10
	ldmia r2, {r2, r3}
	stmia r0, {r2, r3}
	mov r1, #8
	mov r3, r0
	ldr r2, _0223A85C // =0x02258FE4
	sub r0, r1, #9
	str r1, [sp]
	mov r1, #0
	bl ov66_2237348
	add r0, sp, #0x34
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r4, pc}
_0223A7F8:
	add r0, sp, #0x34
	bl ov66_2235B8C
	ldr r1, _0223A858 // =0x0225B6C4
	ldr lr, [r0]
	ldr ip, [r0, #4]
	ldr r2, [r1]
	mov r0, #1
	str r0, [r2, #0x148]
	add r3, sp, #8
	str lr, [sp, #8]
	str ip, [sp, #0xc]
	add r0, r2, #0x14c
	ldmia r3, {r2, r3}
	stmia r0, {r2, r3}
	ldr r0, [r1]
	mov r2, #5
	add r1, sp, #4
	add r0, r0, #0x134
	str r2, [sp, #4]
	bl ov66_2236CA0
	add r0, sp, #0x34
	bl ov66_223B8E8
	add sp, sp, #0x40
	ldmia sp!, {r4, pc}
	.align 2, 0
_0223A858: .word 0x0225B6C4
_0223A85C: .word 0x02258FE4
	arm_func_end ov66_223A750

	arm_func_start ov66_223A860
ov66_223A860: // 0x0223A860
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x20
	cmp r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r2, #0
	ldr r0, [sp, #0x30]
	add r1, sp, #0x14
	str r2, [sp, #0x18]
	str r2, [sp, #0x14]
	str r2, [sp, #0x1c]
	bl ov66_2237EF8
	cmp r0, #0
	ldrne r5, [sp, #0x18]
	cmpne r5, #0
	beq _0223A8C8
	ldr r1, _0223A950 // =0x0225B6C4
	add r0, sp, #0x14
	ldr r4, [r1]
	bl ov66_2235B8C
	mov r1, r0
	mov r2, r5
	add r0, r4, #0x144
	bl ov66_223CB84
	cmp r0, #0
	bne _0223A918
_0223A8C8:
	ldr r1, _0223A950 // =0x0225B6C4
	add r0, sp, #8
	ldr r1, [r1]
	add r1, r1, #0x144
	bl ov66_223CB2C
	add r0, sp, #8
	bl ov66_2235B8C
	ldr ip, [sp, #0xc]
	mov r3, r0
	ldr r2, _0223A954 // =0x02258FF4
	mvn r0, #0
	mov r1, #0
	str ip, [sp]
	bl ov66_2237348
	add r0, sp, #8
	bl ov66_223B8E8
	add r0, sp, #0x14
	bl ov66_223B8E8
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, pc}
_0223A918:
	ldr r0, _0223A950 // =0x0225B6C4
	mov r3, #4
	ldr r2, [r0]
	add r1, sp, #4
	str r3, [r2, #0x144]
	ldr r0, [r0]
	mov r2, #6
	add r0, r0, #0x134
	str r2, [sp, #4]
	bl ov66_2236CA0
	add r0, sp, #0x14
	bl ov66_223B8E8
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0223A950: .word 0x0225B6C4
_0223A954: .word 0x02258FF4
	arm_func_end ov66_223A860

	arm_func_start ov66_223A958
ov66_223A958: // 0x0223A958
	bx lr
	arm_func_end ov66_223A958

	arm_func_start ov66_223A95C
ov66_223A95C: // 0x0223A95C
	stmfd sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x28
	bl ov66_223B8E8
	add r0, r4, #0x18
	bl ov66_223BADC
	add r0, r4, #0xc
	bl ov66_223BADC
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223A95C

	arm_func_start ov66_223A984
ov66_223A984: // 0x0223A984
	stmfd sp!, {r4, lr}
	ldr r1, _0223A9C8 // =0x02258EA0
	mov r4, r0
	str r1, [r4]
	bl ov66_2241E54
	add r0, r4, #0x28
	bl ov66_223B8E8
	add r0, r4, #4
	bl ov66_223AA9C
	cmp r4, #0
	beq _0223A9C0
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_0223A9C0:
	mov r0, r4
	ldmia sp!, {r4, pc}
	.align 2, 0
_0223A9C8: .word 0x02258EA0
	arm_func_end ov66_223A984

	arm_func_start ov66_223A9CC
ov66_223A9CC: // 0x0223A9CC
	stmfd sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x18
	bl ov66_223B8E8
	cmp r4, #0
	beq _0223A9F4
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_0223A9F4:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223A9CC

	arm_func_start ov66_223A9FC
ov66_223A9FC: // 0x0223A9FC
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r2, #0
	mov r4, r0
	strb r2, [sp]
	str r2, [r4, #4]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _0223AA8C
	ldr r1, [r4, #0x14]
	cmp r1, #0
	beq _0223AA3C
	ldr r2, [r4, #0xc]
	ldr r0, [r4, #0x10]
	add r0, r2, r0
	bl _u32_div_f
	mov r2, r1
_0223AA3C:
	ldr r8, [r4, #8]
	ldr r0, [r4, #0xc]
	ldr r1, [r4, #0x14]
	add r6, r8, r2, lsl #2
	add r7, r8, r0, lsl #2
	cmp r7, r6
	add sb, r8, r1, lsl #2
	beq _0223AA8C
	mov r5, #0
_0223AA60:
	ldr r1, [r7]
	cmp r1, #0
	beq _0223AA78
	mov r0, r5
	mov r2, r5
	bl DWC_Free
_0223AA78:
	add r7, r7, #4
	cmp r7, sb
	moveq r7, r8
	cmp r7, r6
	bne _0223AA60
_0223AA8C:
	add r0, r4, #8
	bl ov66_223C41C
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov66_223A9FC

	arm_func_start ov66_223AA9C
ov66_223AA9C: // 0x0223AA9C
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _0223AAB4
	bl ov66_223C46C
_0223AAB4:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223AA9C

	arm_func_start FUN_0223AABC
FUN_0223AABC: // 0x0223AABC
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r5, r2
	mov r6, r1
	mov r4, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r6
	mov r1, r5
	str ip, [sp]
	bl FUN_0223C364
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _0223AB0C
	ldr r2, [r3, #0xc]
	ldr r0, [r5]
	cmp r2, r0
	bhs _0223AB38
_0223AB0C:
	str r5, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r6
	bl FUN_0223ABF4
	ldr r1, _0223AB50 // =0x02258E58
	str r0, [r4]
	ldrb r0, [r1, #2]
	add sp, sp, #0xc
	strb r0, [r4, #4]
	arm_func_end FUN_0223AABC

	arm_func_start ov66_223AB34
ov66_223AB34: // 0x0223AB34
	ldmia sp!, {r3, r4, r5, r6, pc}
_0223AB38:
	ldr r0, _0223AB50 // =0x02258E58
	str r3, [r4]
	ldrb r0, [r0]
	strb r0, [r4, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_0223AB50: .word 0x02258E58
	arm_func_end ov66_223AB34

	arm_func_start FUN_0223AB54
FUN_0223AB54: // 0x0223AB54
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #0xc]
	mov r6, r0
	cmp r4, r1
	bne _0223AB84
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #0xc]
_0223AB84:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	cmp r4, #0
	beq _0223ABAC
	mov r0, #0
	mov r1, r4
	arm_func_end FUN_0223AB54

	arm_func_start ov66_223ABA4
ov66_223ABA4: // 0x0223ABA4
	mov r2, r0
	bl DWC_Free
_0223ABAC:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223ABA4

	arm_func_start FUN_0223ABC4
FUN_0223ABC4: // 0x0223ABC4
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	arm_func_end FUN_0223ABC4

	arm_func_start ov66_223ABD4
ov66_223ABD4: // 0x0223ABD4
	ldmeqia sp!, {r4, pc}
	bl FUN_0223AD00
	mov r0, #0
	str r0, [r4]
	str r0, [r4, #4]
	add r0, r4, #4
	str r0, [r4, #0xc]
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223ABD4

	arm_func_start FUN_0223ABF4
FUN_0223ABF4: // 0x0223ABF4
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	mov sb, r0
	ldr r4, [sb]
	mvn r0, #0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	cmp r4, r0
	ldr r5, [sp, #0x30]
	bne _0223AC2C
	ldr r0, _0223ACFC // =0x02259004
	bl printf
	bl abort
_0223AC2C:
	mov r1, #0
	mov r0, #0x28
	strb r1, [sp]
	bl FUN_02246290
	mov r4, r0
	add r0, sb, #4
	str r4, [sp, #4]
	str r4, [sp, #8]
	str r0, [sp, #0xc]
	str r4, [sp, #0x10]
	adds r2, r4, #0xc
	beq _0223AC94
	ldr r1, [r5]
	add r0, r2, #4
	str r1, [r2]
	add r1, r5, #4
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r2, r5, #0xc
	add r1, r0, #8
	ldmia r2, {r2, r3}
	stmia r1, {r2, r3}
	ldr r1, [r5, #0x14]
	str r1, [r0, #0x10]
	ldr r1, [r5, #0x18]
	str r1, [r0, #0x14]
_0223AC94:
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r8, [r0]
	cmp r7, #0
	strne r4, [r8]
	streq r4, [r8, #4]
	ldr r1, [sb]
	mov r0, r4
	add r1, r1, #1
	str r1, [sb]
	ldr r1, [sb, #4]
	bl ov66_223ADD8
	cmp r6, #0
	ldr r1, [sp, #0x10]
	strne r4, [sb, #0xc]
	cmp r1, #0
	arm_func_end FUN_0223ABF4

	arm_func_start ov66_223ACE0
ov66_223ACE0: // 0x0223ACE0
	beq _0223ACF0
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_0223ACF0:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_0223ACFC: .word 0x02259004
	arm_func_end ov66_223ACE0

	arm_func_start FUN_0223AD00
FUN_0223AD00: // 0x0223AD00
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _0223AD1C
	bl FUN_0223AD00
_0223AD1C:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _0223AD30
	mov r0, r5
	arm_func_end FUN_0223AD00

	arm_func_start ov66_223AD2C
ov66_223AD2C: // 0x0223AD2C
	bl FUN_0223AD00
_0223AD30:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223AD2C

	arm_func_start ov66_223AD4C
ov66_223AD4C: // 0x0223AD4C
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _0223AD64
	bl FUN_0223AD00
_0223AD64:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223AD4C

	arm_func_start ov66_223AD6C
ov66_223AD6C: // 0x0223AD6C
	ldr r2, [r0]
	ldr r1, [r2, #4]
	cmp r1, #0
	beq _0223ADA0
	ldr r2, [r1]
	cmp r2, #0
	beq _0223AD98
_0223AD88:
	mov r1, r2
	ldr r2, [r2]
	cmp r2, #0
	bne _0223AD88
_0223AD98:
	str r1, [r0]
	bx lr
_0223ADA0:
	ldr r1, [r2, #8]
	bic r3, r1, #1
	ldr r1, [r3]
	cmp r2, r1
	beq _0223ADD0
_0223ADB4:
	mov r2, r3
	str r3, [r0]
	ldr r1, [r2, #8]
	bic r3, r1, #1
	ldr r1, [r3]
	cmp r2, r1
	bne _0223ADB4
_0223ADD0:
	str r3, [r0]
	bx lr
	arm_func_end ov66_223AD6C

	arm_func_start ov66_223ADD8
ov66_223ADD8: // 0x0223ADD8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	ldr r0, [sl, #8]
	mov r8, #0
	mov sb, #1
	orr r0, r0, #1
	str r0, [sl, #8]
	add r7, sp, #0x2c
	mov fp, r8
	mov r4, sb
	mov r5, r8
	mov r6, sb
	b _0223AF9C
_0223AE10:
	ldr r1, [r0, #8]
	bic r2, r1, #1
	ldr r1, [r2]
	cmp r0, r1
	bne _0223AEE4
	ldr r1, [r2, #4]
	cmp r1, #0
	beq _0223AE80
	ldr r2, [r1, #8]
	tst r2, #1
	movne r2, sb
	moveq r2, r8
	cmp r2, #1
	bne _0223AE80
	ldr r2, [r0, #8]
	bic r2, r2, #1
	str r2, [r0, #8]
	ldr r0, [r1, #8]
	bic r0, r0, #1
	str r0, [r1, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic sl, r0, #1
	ldr r0, [sl, #8]
	orr r0, r0, #1
	str r0, [sl, #8]
	b _0223AF9C
_0223AE80:
	ldr r1, [r0, #4]
	cmp sl, r1
	bne _0223AE98
	mov r1, r7
	mov sl, r0
	bl ov66_223B450
_0223AE98:
	ldr r0, [sl, #8]
	mov r1, r7
	bic r2, r0, #1
	ldr r0, [r2, #8]
	bic r0, r0, #1
	str r0, [r2, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic r2, r0, #1
	ldr r0, [r2, #8]
	orr r0, r0, #1
	str r0, [r2, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic r0, r0, #1
	bl ov66_223B4CC
	b _0223AF9C
_0223AEE4:
	cmp r1, #0
	beq _0223AF3C
	ldr r2, [r1, #8]
	tst r2, #1
	movne r2, r6
	moveq r2, r5
	cmp r2, #1
	bne _0223AF3C
	ldr r2, [r0, #8]
	bic r2, r2, #1
	str r2, [r0, #8]
	ldr r0, [r1, #8]
	bic r0, r0, #1
	str r0, [r1, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic sl, r0, #1
	ldr r0, [sl, #8]
	orr r0, r0, #1
	str r0, [sl, #8]
	b _0223AF9C
_0223AF3C:
	ldr r1, [r0]
	cmp sl, r1
	bne _0223AF54
	mov r1, r7
	mov sl, r0
	bl ov66_223B4CC
_0223AF54:
	ldr r0, [sl, #8]
	mov r1, r7
	bic r2, r0, #1
	ldr r0, [r2, #8]
	bic r0, r0, #1
	str r0, [r2, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic r2, r0, #1
	ldr r0, [r2, #8]
	orr r0, r0, #1
	str r0, [r2, #8]
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r0, [r0, #8]
	bic r0, r0, #1
	bl ov66_223B450
_0223AF9C:
	ldr r0, [sp, #0x2c]
	cmp sl, r0
	beq _0223AFC8
	ldr r0, [sl, #8]
	bic r0, r0, #1
	ldr r1, [r0, #8]
	tst r1, #1
	movne r1, r4
	moveq r1, fp
	cmp r1, #1
	beq _0223AE10
_0223AFC8:
	ldr r1, [sp, #0x2c]
	ldr r0, [r1, #8]
	bic r0, r0, #1
	str r0, [r1, #8]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223ADD8

	arm_func_start ov66_223AFE4
ov66_223AFE4: // 0x0223AFE4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r4, r0
	add r0, sp, #0
	str r4, [sp]
	bl ov66_223AD6C
	ldr r1, [r4]
	mov r0, r4
	cmp r1, #0
	ldrne r1, [r4, #4]
	cmpne r1, #0
	ldrne r0, [sp]
	ldr r1, [r0]
	ldr r2, [r0, #8]
	cmp r1, #0
	ldreq r1, [r0, #4]
	bic sb, r2, #1
	cmp r1, #0
	beq _0223B040
	ldr r2, [r1, #8]
	and r2, r2, #1
	orr r2, sb, r2
	str r2, [r1, #8]
_0223B040:
	ldr r2, [r0, #8]
	bic r3, r2, #1
	ldr r2, [r3]
	cmp r0, r2
	streq r1, [r3]
	strne r1, [r3, #4]
	moveq sl, #1
	ldr r2, [r0, #8]
	movne sl, #0
	tst r2, #1
	movne r2, #1
	moveq r2, #0
	cmp r2, #0
	moveq r2, #1
	movne r2, #0
	cmp r0, r4
	beq _0223B12C
	ldr r5, [r4, #8]
	ldr r3, [r0, #8]
	bic r5, r5, #1
	and r3, r3, #1
	orr r3, r5, r3
	bic r5, r3, #1
	str r3, [r0, #8]
	ldr r3, [r5]
	cmp r4, r3
	streq r0, [r5]
	strne r0, [r5, #4]
	ldr r5, [r4]
	cmp r5, #0
	str r5, [r0]
	moveq sb, r0
	beq _0223B0D4
	ldr r3, [r5, #8]
	and r3, r3, #1
	orr r3, r0, r3
	str r3, [r5, #8]
_0223B0D4:
	ldr r5, [r4, #4]
	cmp r5, #0
	str r5, [r0, #4]
	moveq sb, r0
	beq _0223B0F8
	ldr r3, [r5, #8]
	and r3, r3, #1
	orr r3, r0, r3
	str r3, [r5, #8]
_0223B0F8:
	ldr r3, [r4, #8]
	tst r3, #1
	movne r3, #1
	moveq r3, #0
	cmp r3, #0
	ldr r3, [r0, #8]
	orrne r3, r3, #1
	biceq r3, r3, #1
	str r3, [r0, #8]
	ldr r3, [sp, #0x2c]
	cmp r3, r4
	streq r0, [sp, #0x2c]
	b _0223B13C
_0223B12C:
	cmp r1, #0
	ldreq r0, [sp, #0x2c]
	cmpeq r4, r0
	moveq r2, #0
_0223B13C:
	cmp r2, #0
	beq _0223B440
	mov r5, #1
	mov r4, #0
	mov r7, r4
	mov r8, r5
	add r6, sp, #0x2c
	mov fp, r5
	b _0223B404
_0223B160:
	cmp sl, #0
	beq _0223B2B8
	ldr r0, [sb, #4]
	ldr r1, [r0, #8]
	tst r1, #1
	movne r2, r8
	moveq r2, r7
	cmp r2, #1
	bne _0223B1A8
	bic r1, r1, #1
	str r1, [r0, #8]
	ldr r1, [sb, #8]
	mov r0, sb
	orr r1, r1, #1
	str r1, [sb, #8]
	mov r1, r6
	bl ov66_223B450
	ldr r0, [sb, #4]
_0223B1A8:
	ldr r2, [r0]
	cmp r2, #0
	beq _0223B1CC
	ldr r1, [r2, #8]
	tst r1, #1
	movne r1, r5
	moveq r1, r4
	cmp r1, #0
	bne _0223B21C
_0223B1CC:
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _0223B1F0
	ldr r1, [r1, #8]
	tst r1, #1
	movne r1, fp
	moveq r1, #0
	cmp r1, #0
	bne _0223B21C
_0223B1F0:
	ldr r2, [r0, #8]
	mov r1, sb
	orr r2, r2, #1
	str r2, [r0, #8]
	ldr r0, [sb, #8]
	bic sb, r0, #1
	ldr r0, [sb]
	cmp r1, r0
	moveq sl, #1
	movne sl, #0
	b _0223B404
_0223B21C:
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _0223B240
	ldr r1, [r1, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _0223B264
_0223B240:
	ldr r3, [r2, #8]
	mov r1, r6
	bic r3, r3, #1
	str r3, [r2, #8]
	ldr r2, [r0, #8]
	orr r2, r2, #1
	str r2, [r0, #8]
	bl ov66_223B4CC
	ldr r0, [sb, #4]
_0223B264:
	ldr r1, [sb, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	ldr r1, [r0, #8]
	orrne r1, r1, #1
	biceq r1, r1, #1
	str r1, [r0, #8]
	ldr r2, [sb, #8]
	mov r1, r6
	bic r2, r2, #1
	str r2, [sb, #8]
	ldr r3, [r0, #4]
	mov r0, sb
	ldr r2, [r3, #8]
	bic r2, r2, #1
	str r2, [r3, #8]
	bl ov66_223B450
	ldr r1, [sp, #0x2c]
	b _0223B404
_0223B2B8:
	ldr r0, [sb]
	ldr r1, [r0, #8]
	tst r1, #1
	movne r2, #1
	moveq r2, #0
	cmp r2, #1
	bne _0223B2F8
	bic r1, r1, #1
	str r1, [r0, #8]
	ldr r1, [sb, #8]
	mov r0, sb
	orr r1, r1, #1
	str r1, [sb, #8]
	mov r1, r6
	bl ov66_223B4CC
	ldr r0, [sb]
_0223B2F8:
	ldr r2, [r0]
	cmp r2, #0
	beq _0223B31C
	ldr r1, [r2, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _0223B36C
_0223B31C:
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _0223B340
	ldr r1, [r1, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _0223B36C
_0223B340:
	ldr r2, [r0, #8]
	mov r1, sb
	orr r2, r2, #1
	str r2, [r0, #8]
	ldr r0, [sb, #8]
	bic sb, r0, #1
	ldr r0, [sb]
	cmp r1, r0
	moveq sl, #1
	movne sl, #0
	b _0223B404
_0223B36C:
	cmp r2, #0
	beq _0223B38C
	ldr r1, [r2, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _0223B3B4
_0223B38C:
	ldr r3, [r0, #4]
	mov r1, r6
	ldr r2, [r3, #8]
	bic r2, r2, #1
	str r2, [r3, #8]
	ldr r2, [r0, #8]
	orr r2, r2, #1
	str r2, [r0, #8]
	bl ov66_223B450
	ldr r0, [sb]
_0223B3B4:
	ldr r1, [sb, #8]
	tst r1, #1
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	ldr r1, [r0, #8]
	orrne r1, r1, #1
	biceq r1, r1, #1
	str r1, [r0, #8]
	ldr r2, [sb, #8]
	mov r1, r6
	bic r2, r2, #1
	str r2, [sb, #8]
	ldr r3, [r0]
	mov r0, sb
	ldr r2, [r3, #8]
	bic r2, r2, #1
	str r2, [r3, #8]
	bl ov66_223B4CC
	ldr r1, [sp, #0x2c]
_0223B404:
	ldr r0, [sp, #0x2c]
	cmp r1, r0
	beq _0223B430
	cmp r1, #0
	beq _0223B160
	ldr r0, [r1, #8]
	tst r0, #1
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _0223B160
_0223B430:
	cmp r1, #0
	ldrne r0, [r1, #8]
	bicne r0, r0, #1
	strne r0, [r1, #8]
_0223B440:
	ldr r0, [sp]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223AFE4

	arm_func_start ov66_223B450
ov66_223B450: // 0x0223B450
	ldr r3, [r1]
	ldr r2, [r0, #4]
	cmp r3, r0
	streq r2, [r1]
	ldr r1, [r2]
	str r1, [r0, #4]
	ldr r3, [r2]
	cmp r3, #0
	beq _0223B484
	ldr r1, [r3, #8]
	and r1, r1, #1
	orr r1, r0, r1
	str r1, [r3, #8]
_0223B484:
	ldr r3, [r0, #8]
	ldr r1, [r2, #8]
	bic r3, r3, #1
	and r1, r1, #1
	orr r1, r3, r1
	str r1, [r2, #8]
	ldr r1, [r0, #8]
	bic r3, r1, #1
	ldr r1, [r3]
	cmp r0, r1
	streq r2, [r3]
	strne r2, [r3, #4]
	str r0, [r2]
	ldr r1, [r0, #8]
	and r1, r1, #1
	orr r1, r2, r1
	str r1, [r0, #8]
	bx lr
	arm_func_end ov66_223B450

	arm_func_start ov66_223B4CC
ov66_223B4CC: // 0x0223B4CC
	ldr r3, [r1]
	ldr r2, [r0]
	cmp r3, r0
	streq r2, [r1]
	ldr r1, [r2, #4]
	str r1, [r0]
	ldr r3, [r2, #4]
	cmp r3, #0
	beq _0223B500
	ldr r1, [r3, #8]
	and r1, r1, #1
	orr r1, r0, r1
	str r1, [r3, #8]
_0223B500:
	ldr r3, [r0, #8]
	ldr r1, [r2, #8]
	bic r3, r3, #1
	and r1, r1, #1
	orr r1, r3, r1
	str r1, [r2, #8]
	ldr r1, [r0, #8]
	bic r3, r1, #1
	ldr r1, [r3]
	cmp r0, r1
	streq r2, [r3]
	strne r2, [r3, #4]
	str r0, [r2, #4]
	ldr r1, [r0, #8]
	and r1, r1, #1
	orr r1, r2, r1
	str r1, [r0, #8]
	bx lr
	arm_func_end ov66_223B4CC

	arm_func_start ov66_223B548
ov66_223B548: // 0x0223B548
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r2, #0
	mov r5, r0
	mov r1, #1
	strb r2, [sp]
	bl ov66_223C4B8
	mov r0, r5
	mov r2, r4
	mov r1, #1
	bl ov66_223C588
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223B548

	arm_func_start ov66_223B578
ov66_223B578: // 0x0223B578
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B5C4
	ldr r1, [r4, #4]
	mov r3, #0
	strb r3, [sp]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov66_223C6F8
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B5C4
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_0223B5C4:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223B578

	arm_func_start ov66_223B5D0
ov66_223B5D0: // 0x0223B5D0
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223B614
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B614
	mov r2, r0
	bl DWC_Free
_0223B614:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223B5D0

	arm_func_start ov66_223B620
ov66_223B620: // 0x0223B620
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223B664
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B664
	mov r2, r0
	bl DWC_Free
_0223B664:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223B620

	arm_func_start ov66_223B670
ov66_223B670: // 0x0223B670
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223B6B4
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B6B4
	mov r2, r0
	bl DWC_Free
_0223B6B4:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223B670

	arm_func_start ov66_223B6C0
ov66_223B6C0: // 0x0223B6C0
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r1
	mov r4, r0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r3, #0
	strb r3, [sp, #1]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov66_223B714
	mov r1, #0
	strb r1, [sp]
	ldr r3, [r4]
	ldr r0, [r4, #4]
	mov r2, r5
	add r0, r3, r0
	bl memset
	ldr r0, [r4, #4]
	add r0, r0, r5
	str r0, [r4, #4]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223B6C0

	arm_func_start ov66_223B714
ov66_223B714: // 0x0223B714
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223B714

	arm_func_start ov66_223B734
ov66_223B734: // 0x0223B734
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0223B77C
	beq _0223B770
	mov r2, r0
	bl DWC_Free
_0223B770:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0223B77C:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov66_223B714
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223B734

	arm_func_start ov66_223B7A4
ov66_223B7A4: // 0x0223B7A4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223B7CC
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223B7CC:
	ldr r0, _0223B868 // =0x55555555
	cmp r4, r0
	bhs _0223B818
	add r1, r4, #1
	ldr r0, _0223B86C // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223B818:
	cmp r4, r0, lsl #1
	bhs _0223B854
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223B854:
	mvn r0, #0
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223B868: .word 0x55555555
_0223B86C: .word 0xCCCCCCCD
	arm_func_end ov66_223B7A4

	arm_func_start ov66_223B870
ov66_223B870: // 0x0223B870
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #8]
	mov r4, r1
	cmp r4, r3
	ldrls r1, [r5, #4]
	subls r0, r3, r4
	cmpls r1, r0
	bls _0223B8B4
	ldr r0, [r5, #4]
	mov r2, #0
	add r1, r0, r4
	mov r0, r5
	sub r1, r1, r3
	strb r2, [sp]
	bl ov66_223B7A4
	b _0223B8D8
_0223B8B4:
	ldr r0, [r5]
	mov r2, r4
	add r0, r0, r1
	mov r1, #0
	bl memset
	ldr r0, [r5, #4]
	add r0, r0, r4
	str r0, [r5, #4]
	ldmia sp!, {r3, r4, r5, pc}
_0223B8D8:
	mov r0, r5
	mov r1, r4
	bl ov66_223C734
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223B870

	arm_func_start ov66_223B8E8
ov66_223B8E8: // 0x0223B8E8
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223B92C
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223B92C
	mov r2, r0
	bl DWC_Free
_0223B92C:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223B8E8

	arm_func_start ov66_223B938
ov66_223B938: // 0x0223B938
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r5, r1
	ldr r1, [r5]
	mov r4, r0
	mov r0, r1, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmeqia r5, {r0, r1, r2}
	stmeqia r4, {r0, r1, r2}
	beq _0223B9A8
	mov r1, #0
	mov r0, r1
_0223B968:
	str r0, [r4, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _0223B968
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223BB10
	ldmib r5, {r0, r3}
	mov r1, #0
	add r0, r3, r0
	str r0, [sp]
	ldrb ip, [sp, #8]
	mov r0, r4
	mov r2, r1
	strb ip, [sp, #4]
	bl ov66_223C018
_0223B9A8:
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, pc}
	arm_func_end ov66_223B938

	arm_func_start ov66_223B9B4
ov66_223B9B4: // 0x0223B9B4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov r5, r1
	mov r1, #0
	mov r6, r0
	mov r4, r2
	mov r0, r1
_0223B9D4:
	str r0, [r6, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _0223B9D4
	ldr r0, [r5]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r1, [r5, #4]
	bne _0223BA04
	ldrb r0, [r5]
	mov r0, r0, lsl #0x18
	mov r1, r0, lsr #0x19
_0223BA04:
	cmp r4, r1
	bhi _0223BA2C
	ldr r0, [sp, #0x2c]
	sub r1, r1, r4
	cmp r1, r0
	addlo r0, sp, #8
	addhs r0, sp, #0x2c
	str r1, [sp, #8]
	ldr r1, [r0]
	b _0223BA30
_0223BA2C:
	mov r1, #0
_0223BA30:
	mov r0, r6
	bl ov66_223BB10
	str r4, [sp]
	ldr r4, [sp, #0x2c]
	mov r1, #0
	mov r0, r6
	mov r2, r1
	mov r3, r5
	str r4, [sp, #4]
	bl ov66_223BD18
	mov r0, r6
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223B9B4

	arm_func_start ov66_223BA6C
ov66_223BA6C: // 0x0223BA6C
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov r5, r1
	mov r1, #0
	mov r6, r0
	mov r0, r1
_0223BA84:
	str r0, [r6, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _0223BA84
	mov r0, r5
	bl strlen
	mov r4, r0
	mov r0, r6
	mov r1, r4
	bl ov66_223BB10
	add r0, r5, r4
	mov r1, #0
	ldrb r3, [sp, #8]
	str r0, [sp]
	mov r0, r6
	mov r2, r1
	strb r3, [sp, #4]
	mov r3, r5
	bl ov66_223C018
	mov r0, r6
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_223BA6C

	arm_func_start ov66_223BADC
ov66_223BADC: // 0x0223BADC
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r1, [r4, #8]
	cmpne r1, #0
	beq _0223BB08
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_0223BB08:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223BADC

	arm_func_start ov66_223BB10
ov66_223BB10: // 0x0223BB10
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, r1
	mvn r1, #0x80000001
	mov sb, r0
	cmp r8, r1
	bls _0223BB34
	ldr r0, _0223BC58 // =0x02259020
	bl printf
	bl abort
_0223BB34:
	ldr r0, [sb]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r7, #1
	moveq r7, #0
	cmp r7, #0
	ldmneia sb, {r0, r4}
	movne r1, r0, lsr #1
	bne _0223BB68
	ldrb r0, [sb]
	mov r1, #0xb
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_0223BB68:
	cmp r8, r4
	movlo r8, r4
	add r0, r8, #1
	cmp r0, #0xb
	movls r8, #0xb
	addhi r0, r8, #0x10
	bichi r8, r0, #0xf
	cmp r8, r1
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	cmp r8, #0xb
	bne _0223BBA4
	add r5, sb, #1
	ldr sl, [sb, #8]
	mov r6, #0
	b _0223BBD8
_0223BBA4:
	cmp r8, r1
	mov r0, r8
	bls _0223BBBC
	bl FUN_02246290
	mov r5, r0
	b _0223BBC8
_0223BBBC:
	bl FUN_02246290
	movs r5, r0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223BBC8:
	cmp r7, #0
	ldrne sl, [sb, #8]
	mov r6, #1
	addeq sl, sb, #1
_0223BBD8:
	mov r0, r5
	mov r1, sl
	mov r2, r4
	bl memmove
	mov r0, #0
	cmp r7, #0
	strb r0, [r5, r4]
	cmpne sl, #0
	beq _0223BC08
	mov r1, sl
	mov r2, r0
	bl DWC_Free
_0223BC08:
	ldr r1, [sb]
	and r0, r6, #1
	bic r1, r1, #1
	orr r0, r1, r0
	str r0, [sb]
	cmp r6, #0
	bne _0223BC40
	ldrb r1, [sb]
	and r0, r4, #0xff
	mov r0, r0, lsl #0x19
	bic r1, r1, #0xfe
	orr r0, r1, r0, lsr #24
	strb r0, [sb]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223BC40:
	stmib sb, {r4, r5}
	ldr r0, [sb]
	and r0, r0, #1
	orr r0, r0, r8, lsl #1
	str r0, [sb]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_0223BC58: .word 0x02259020
	arm_func_end ov66_223BB10

	arm_func_start ov66_223BC5C
ov66_223BC5C: // 0x0223BC5C
	stmfd sp!, {r3, lr}
	mov ip, r0
	ldr r0, [ip]
	mov r3, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _0223BCA4
	ldr r0, [r3]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	bne _0223BCA4
	ldmia r3, {r0, r1, r2}
	stmia ip, {r0, r1, r2}
	mov r0, ip
	ldmia sp!, {r3, pc}
_0223BCA4:
	cmp r1, #0
	ldreqb r0, [ip]
	ldrne r2, [ip, #4]
	mov r1, #0
	moveq r0, r0, lsl #0x18
	moveq r2, r0, lsr #0x19
	mov r0, ip
	bl ov66_223BCC8
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223BC5C

	arm_func_start ov66_223BCC8
ov66_223BCC8: // 0x0223BCC8
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, [r3]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldrne lr, [r3, #8]
	ldrne r3, [r3, #4]
	bne _0223BCF8
	ldrb ip, [r3]
	add lr, r3, #1
	mov r3, ip, lsl #0x18
	mov r3, r3, lsr #0x19
_0223BCF8:
	ldrb ip, [sp, #8]
	add r3, lr, r3
	str r3, [sp]
	mov r3, lr
	strb ip, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0xc
	ldmia sp!, {pc}
	arm_func_end ov66_223BCC8

	arm_func_start ov66_223BD18
ov66_223BD18: // 0x0223BD18
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r4, [r3]
	mov r8, r0
	mov r0, r4, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r7, r1
	mov r6, r2
	ldrne r4, [r3, #8]
	ldrne r5, [r3, #4]
	bne _0223BD54
	ldrb r0, [r3]
	add r4, r3, #1
	mov r0, r0, lsl #0x18
	mov r5, r0, lsr #0x19
_0223BD54:
	ldr r0, [sp, #0x28]
	cmp r0, r5
	bls _0223BD6C
	ldr r0, _0223BDC0 // =0x02259044
	bl printf
	bl abort
_0223BD6C:
	ldr r2, [sp, #0x28]
	ldr r0, [sp, #0x2c]
	sub r1, r5, r2
	cmp r1, r0
	addlo r0, sp, #0xc
	str r1, [sp, #0xc]
	add r4, r4, r2
	addhs r0, sp, #0x2c
	ldr r0, [r0]
	ldrb r5, [sp, #8]
	str r0, [sp, #0x2c]
	add r0, r4, r0
	str r0, [sp]
	mov r0, r8
	mov r1, r7
	mov r2, r6
	mov r3, r4
	strb r5, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0223BDC0: .word 0x02259044
	arm_func_end ov66_223BD18

	arm_func_start ov66_223BDC4
ov66_223BDC4: // 0x0223BDC4
	stmfd sp!, {r3, lr}
	ldr r3, [r1]
	mov r3, r3, lsl #0x1f
	movs r3, r3, lsr #0x1f
	ldmneib r1, {r3, ip}
	bne _0223BDEC
	ldrb r3, [r1]
	add ip, r1, #1
	mov r1, r3, lsl #0x18
	mov r3, r1, lsr #0x19
_0223BDEC:
	mov r1, ip
	bl ov66_223BDF8
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223BDC4

	arm_func_start ov66_223BDF8
ov66_223BDF8: // 0x0223BDF8
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	ldr ip, [r0]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldrne lr, [r0, #8]
	ldrne r0, [r0, #4]
	bne _0223BE24
	ldrb ip, [r0]
	add lr, r0, #1
	mov r0, ip, lsl #0x18
	mov r0, r0, lsr #0x19
_0223BE24:
	cmp r2, r0
	bhi _0223BE88
	sub ip, r0, r2
	cmp ip, r3
	add r4, r1, r3
	add r5, lr, r2
	blo _0223BE88
_0223BE40:
	mov r6, r5
	mov r7, r1
	cmp r1, r4
	bhs _0223BE70
_0223BE50:
	ldrsb r2, [r7]
	ldrsb r0, [r6]
	cmp r2, r0
	bne _0223BE78
	add r7, r7, #1
	cmp r7, r4
	add r6, r6, #1
	blo _0223BE50
_0223BE70:
	sub r0, r5, lr
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223BE78:
	sub ip, ip, #1
	cmp ip, r3
	add r5, r5, #1
	bhs _0223BE40
_0223BE88:
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_223BDF8

	arm_func_start ov66_223BE90
ov66_223BE90: // 0x0223BE90
	stmfd sp!, {r3, lr}
	ldr r2, [r1]
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r3, [r1, #8]
	ldrne ip, [r1, #4]
	bne _0223BEBC
	ldrb r2, [r1]
	add r3, r1, #1
	mov r1, r2, lsl #0x18
	mov ip, r1, lsr #0x19
_0223BEBC:
	ldr r1, [r0]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r0, #4]
	bne _0223BEDC
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_0223BEDC:
	mov r1, #0
	str ip, [sp]
	bl ov66_223BF38
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223BE90

	arm_func_start ov66_223BEEC
ov66_223BEEC: // 0x0223BEEC
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl strlen
	ldr r1, [r5]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r5, #4]
	bne _0223BF20
	ldrb r1, [r5]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_0223BF20:
	str r0, [sp]
	mov r0, r5
	mov r3, r4
	mov r1, #0
	bl ov66_223BF38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223BEEC

	arm_func_start ov66_223BF38
ov66_223BF38: // 0x0223BF38
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r2, [r0]
	mov r6, r1
	mov r1, r2, lsl #0x1f
	movs r1, r1, lsr #0x1f
	mov r5, r3
	ldrne r4, [r0, #8]
	ldrne r7, [r0, #4]
	bne _0223BF74
	ldrb r1, [r0]
	add r4, r0, #1
	mov r0, r1, lsl #0x18
	mov r7, r0, lsr #0x19
_0223BF74:
	cmp r6, r7
	bls _0223BF88
	ldr r0, _0223C014 // =0x02259060
	bl printf
	bl abort
_0223BF88:
	ldr r0, [sp, #0x28]
	sub r1, r7, r6
	cmp r0, r1
	addlo r0, sp, #0x28
	addhs r0, sp, #0
	str r1, [sp]
	ldr r1, [r0]
	ldr r0, [sp, #0x30]
	str r1, [sp, #4]
	cmp r0, r1
	addlo r0, sp, #0x30
	addhs r0, sp, #4
	ldr r2, [r0]
	mov r1, r5
	add r0, r4, r6
	bl memcmp
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, lr}
	addne sp, sp, #0x10
	bxne lr
	ldr r1, [sp, #0x30]
	ldr r0, [sp, #4]
	cmp r0, r1
	addlo sp, sp, #8
	mvnlo r0, #0
	ldmloia sp!, {r3, r4, r5, r6, r7, lr}
	addlo sp, sp, #0x10
	bxlo lr
	movne r0, #1
	moveq r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223C014: .word 0x02259060
	arm_func_end ov66_223BF38

	arm_func_start ov66_223C018
ov66_223C018: // 0x0223C018
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	mov r6, r0
	ldr r0, [r6]
	mov r5, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r0, #1
	strne r0, [sp]
	moveq r0, #0
	streq r0, [sp]
	ldr r0, [sp]
	mov fp, r3
	ldr r4, [sp, #0x60]
	cmp r0, #0
	bne _0223C078
	ldrb r1, [r6]
	mov r0, #0xb
	str r0, [sp, #0x14]
	mov r0, r1, lsl #0x18
	add r7, r6, #1
	mov r8, r0, lsr #0x19
	b _0223C088
_0223C078:
	ldmia r6, {r0, r8}
	mov r0, r0, lsr #1
	ldr r7, [r6, #8]
	str r0, [sp, #0x14]
_0223C088:
	cmp r5, r8
	bls _0223C09C
	ldr r0, _0223C2E8 // =0x02259044
	bl printf
	bl abort
_0223C09C:
	ldr r0, [sp, #0x58]
	sub r1, r8, r5
	cmp r1, r0
	addlo r0, sp, #0x18
	str r1, [sp, #0x18]
	addhs r0, sp, #0x58
	ldr r0, [r0]
	sub sb, r4, fp
	mvn r1, #0x80000001
	cmp sb, r1
	str r0, [sp, #0x10]
	subls sl, r8, r0
	subls r0, r1, sb
	cmpls sl, r0
	bls _0223C0E4
	ldr r0, _0223C2EC // =0x0225907C
	bl printf
	bl abort
_0223C0E4:
	ldr r0, [sp, #0x10]
	add sl, sb, sl
	add r1, r5, r0
	ldr r0, [sp, #0x14]
	cmp sl, r0
	sub r0, r8, r1
	str r0, [sp, #0xc]
	blo _0223C1F8
	ldr r0, [sp, #0x14]
	add r1, sl, #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	bhs _0223C130
_0223C11C:
	mov r0, r8, lsl #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	blo _0223C11C
_0223C130:
	mov r0, r8
	bl FUN_02246290
	str r0, [sp, #8]
	cmp r5, #0
	beq _0223C150
	mov r1, r7
	mov r2, r5
	bl memcpy
_0223C150:
	ldr r0, [sp, #8]
	sub r2, r4, fp
	add r0, r0, r5
	mov r1, fp
	str r0, [sp, #4]
	bl memmove
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223C190
	mov r2, r0
	ldr r0, [sp, #4]
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	add r0, r0, sb
	add r1, r3, r1
	bl memcpy
_0223C190:
	ldr r1, _0223C2F0 // =0x02258E58
	ldr r0, [sp]
	ldrsb r1, [r1, #3]
	cmp r0, #0
	ldr r0, [sp, #8]
	strb r1, [r0, sl]
	beq _0223C1C8
	cmp r7, #0
	beq _0223C1D8
	mov r0, #0
	mov r1, r7
	mov r2, r0
	bl DWC_Free
	b _0223C1D8
_0223C1C8:
	ldr r0, [r6]
	bic r0, r0, #1
	orr r0, r0, #1
	str r0, [r6]
_0223C1D8:
	ldr r0, [sp, #8]
	str r0, [r6, #8]
	str sl, [r6, #4]
	ldr r0, [r6]
	and r0, r0, #1
	orr r0, r0, r8, lsl #1
	str r0, [r6]
	b _0223C2D4
_0223C1F8:
	mov r1, #0
	add r0, sp, #0x1c
	mov r3, r1
_0223C204:
	str r3, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _0223C204
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223C264
	add r0, r7, r5
	add r0, r0, sb
	cmp r0, r4
	bhs _0223C264
	add r0, r7, r8
	cmp r4, r0
	bhi _0223C264
	add r0, sp, #0x1c
	mov r1, fp
	mov r2, r4
	bl ov66_223C3D0
	ldr r0, [sp, #0x1c]
	add r1, sp, #0x1c
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq fp, r1, #1
	ldrne fp, [r1, #8]
_0223C264:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223C288
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	mov r2, r0
	add r0, r3, sb
	add r1, r3, r1
	bl memmove
_0223C288:
	mov r1, fp
	mov r2, sb
	add r0, r7, r5
	bl memmove
	ldr r0, [sp]
	ldr r1, _0223C2F0 // =0x02258E58
	cmp r0, #0
	ldrsb r0, [r1, #1]
	strb r0, [r7, sl]
	strne sl, [r6, #4]
	bne _0223C2CC
	ldrb r1, [r6]
	and r0, sl, #0xff
	mov r0, r0, lsl #0x19
	bic r1, r1, #0xfe
	orr r0, r1, r0, lsr #24
	strb r0, [r6]
_0223C2CC:
	add r0, sp, #0x1c
	bl ov66_223BADC
_0223C2D4:
	mov r0, r6
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223C2E8: .word 0x02259044
_0223C2EC: .word 0x0225907C
_0223C2F0: .word 0x02258E58
	arm_func_end ov66_223C018

	arm_func_start ov66_223C2F4
ov66_223C2F4: // 0x0223C2F4
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r1
	mov r4, r2
	subs r1, r4, r5
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r3, #0
	strb r3, [sp, #1]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov66_223B714
	mov r0, #0
	strb r0, [sp]
	ldr r2, [r6]
	ldr r0, [r6, #4]
	mov r1, r5
	add r0, r2, r0
	sub r2, r4, r5
	bl memmove
	ldr r1, [r6, #4]
	sub r0, r4, r5
	add r0, r1, r0
	str r0, [r6, #4]
	add sp, sp, #4
	arm_func_end ov66_223C2F4

	arm_func_start ov66_223C360
ov66_223C360: // 0x0223C360
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_223C360

	arm_func_start FUN_0223C364
FUN_0223C364: // 0x0223C364
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_0223C398:
	ldr r5, [r1]
	ldr r4, [r6, #0xc]
	mov r0, r6
	cmp r5, r4
	ldrlo r6, [r6]
	strlob lr, [r3]
	blo _0223C3C4
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_0223C3C4:
	cmp r6, #0
	bne _0223C398
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_0223C364

	arm_func_start ov66_223C3D0
ov66_223C3D0: // 0x0223C3D0
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, [r0]
	mov r3, r1
	mov r1, ip, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r1, [r0, #4]
	bne _0223C3FC
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r1, r1, lsr #0x19
_0223C3FC:
	ldrb ip, [sp, #8]
	str r2, [sp]
	mov r2, r1
	mov r1, #0
	strb ip, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0xc
	ldmia sp!, {pc}
	arm_func_end ov66_223C3D0

	arm_func_start ov66_223C41C
ov66_223C41C: // 0x0223C41C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223C460
	ldr r2, [r4, #8]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #8]
	sub r1, r1, r2
	str r1, [r4, #8]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223C460
	mov r2, r0
	bl DWC_Free
_0223C460:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223C41C

	arm_func_start ov66_223C46C
ov66_223C46C: // 0x0223C46C
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _0223C488
	bl ov66_223C46C
_0223C488:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _0223C49C
	mov r0, r5
	bl ov66_223C46C
_0223C49C:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223C46C

	arm_func_start ov66_223C4B8
ov66_223C4B8: // 0x0223C4B8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	ldr r0, _0223C57C // =0x15555555
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223C4E0
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223C4E0:
	ldr r0, _0223C580 // =0x071C71C7
	cmp r4, r0
	bhs _0223C52C
	add r1, r4, #1
	ldr r0, _0223C584 // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223C52C:
	cmp r4, r0, lsl #1
	bhs _0223C568
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223C568:
	ldr r0, _0223C57C // =0x15555555
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223C57C: .word 0x15555555
_0223C580: .word 0x071C71C7
_0223C584: .word 0xCCCCCCCD
	arm_func_end ov66_223C4B8

	arm_func_start ov66_223C588
ov66_223C588: // 0x0223C588
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	mov r4, #0
	mov r7, r0
	add r3, r7, #8
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	str r4, [sp, #0x10]
	str r4, [sp, #0x18]
	str r3, [sp, #0x14]
	mov r6, r1
	ldr r3, [r7, #4]
	ldr r1, [r7, #8]
	add r3, r3, r6
	sub r1, r3, r1
	mov r5, r2
	bl ov66_223C4B8
	mov r3, r4
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov66_223C950
	ldr r4, [r7, #4]
	mov r3, #0
	ldr r1, [sp, #8]
	mov r0, #0xc
	mla r2, r4, r0, r1
	ldr r1, [sp, #0xc]
	str r4, [sp, #0x18]
	mla r4, r1, r0, r2
	strb r3, [sp, #2]
	cmp r6, #0
	beq _0223C648
_0223C61C:
	cmp r4, #0
	beq _0223C630
	mov r0, r4
	mov r1, r5
	bl ov66_223B938
_0223C630:
	ldr r0, [sp, #0xc]
	subs r6, r6, #1
	add r0, r0, #1
	str r0, [sp, #0xc]
	add r4, r4, #0xc
	bne _0223C61C
_0223C648:
	ldr r6, [r7]
	ldr r1, [r7, #4]
	mov r0, #0xc
	mla r5, r1, r0, r6
	mov r3, #0
	ldr r2, [sp, #8]
	ldr r1, [sp, #0x18]
	strb r3, [sp, #1]
	mla r4, r1, r0, r2
	cmp r5, r6
	bls _0223C6AC
_0223C674:
	sub r5, r5, #0xc
	subs r4, r4, #0xc
	beq _0223C68C
	mov r0, r4
	mov r1, r5
	bl ov66_223B938
_0223C68C:
	ldr r1, [sp, #0x18]
	ldr r0, [sp, #0xc]
	sub r1, r1, #1
	add r0, r0, #1
	str r1, [sp, #0x18]
	str r0, [sp, #0xc]
	cmp r5, r6
	bhi _0223C674
_0223C6AC:
	mov r0, #0
	strb r0, [sp]
	add r3, sp, #0x10
	ldr r2, [r7, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r7, #8]
	str r2, [r3]
	ldr r2, [r7]
	ldr r1, [sp, #8]
	str r1, [r7]
	str r2, [sp, #8]
	ldr r2, [r7, #4]
	ldr r1, [sp, #0xc]
	str r1, [r7, #4]
	str r2, [sp, #0xc]
	bl ov66_223C870
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov66_223C588

	arm_func_start ov66_223C6F8
ov66_223C6F8: // 0x0223C6F8
	stmfd sp!, {r3, r4, r5, lr}
	ldr ip, [r0]
	ldr r3, [r0, #4]
	mov r2, #0xc
	mla r4, r3, r2, ip
	movs r5, r1
	sub r1, r3, r5
	str r1, [r0, #4]
	ldmeqia sp!, {r3, r4, r5, pc}
_0223C71C:
	sub r4, r4, #0xc
	mov r0, r4
	bl ov66_223BADC
	subs r5, r5, #1
	bne _0223C71C
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223C6F8

	arm_func_start ov66_223C734
ov66_223C734: // 0x0223C734
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	mov r3, #0
	mov r6, r0
	add r2, r6, #8
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	str r3, [sp, #0x10]
	str r3, [sp, #0x18]
	str r2, [sp, #0x14]
	mov r4, r1
	ldr r2, [r6, #4]
	ldr r1, [r6, #8]
	add r2, r2, r4
	sub r1, r2, r1
	bl ov66_223B7A4
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov66_223C8E0
	ldr r5, [r6, #4]
	ldr r2, [sp, #8]
	mov r1, #0
	add r3, r2, r5
	ldr r0, [sp, #0xc]
	mov r2, r4
	add r0, r3, r0
	str r5, [sp, #0x18]
	strb r1, [sp, #2]
	bl memset
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #0x18]
	add r1, r1, r4
	str r1, [sp, #0xc]
	ldr r5, [r6]
	ldr r2, [r6, #4]
	mov r1, #0
	strb r1, [sp, #1]
	add r1, r5, r2
	sub r4, r1, r5
	sub r2, r0, r4
	ldr r0, [sp, #8]
	str r2, [sp, #0x18]
	add r0, r0, r2
	mov r1, r5
	mov r2, r4
	bl memcpy
	mov r0, r5
	mov r1, #0
	mov r2, r4
	bl memset
	ldr r1, [sp, #0xc]
	mov r0, #0
	add r1, r1, r4
	str r1, [sp, #0xc]
	strb r0, [sp]
	str r0, [r6, #4]
	add r3, sp, #0x10
	ldr r2, [r6, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r6, #8]
	str r2, [r3]
	ldr r2, [r6]
	ldr r1, [sp, #8]
	str r1, [r6]
	str r2, [sp, #8]
	ldr r2, [r6, #4]
	ldr r1, [sp, #0xc]
	str r1, [r6, #4]
	str r2, [sp, #0xc]
	bl ov66_223C8C4
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_223C734

	arm_func_start ov66_223C870
ov66_223C870: // 0x0223C870
	stmfd sp!, {r4, r5, r6, lr}
	mov r4, r0
	ldr r2, [r4]
	ldr r1, [r4, #0x10]
	mov r0, #0xc
	mla r5, r1, r0, r2
	ldr r1, [r4, #4]
	mla r6, r1, r0, r5
	cmp r6, r5
	bls _0223C8AC
_0223C898:
	sub r6, r6, #0xc
	mov r0, r6
	bl ov66_223BADC
	cmp r6, r5
	bhi _0223C898
_0223C8AC:
	mov r1, #0
	mov r0, r4
	str r1, [r4, #4]
	bl ov66_223C988
	mov r0, r4
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_223C870

	arm_func_start ov66_223C8C4
ov66_223C8C4: // 0x0223C8C4
	stmfd sp!, {r4, lr}
	mov r4, r0
	mov r1, #0
	str r1, [r4, #4]
	bl ov66_223C900
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223C8C4

	arm_func_start ov66_223C8E0
ov66_223C8E0: // 0x0223C8E0
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223C8E0

	arm_func_start ov66_223C900
ov66_223C900: // 0x0223C900
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223C944
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223C944
	mov r2, r0
	bl DWC_Free
_0223C944:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223C900

	arm_func_start ov66_223C950
ov66_223C950: // 0x0223C950
	stmfd sp!, {r3, r4, r5, lr}
	ldr r2, _0223C984 // =0x15555555
	mov r4, r1
	mov r5, r0
	cmp r4, r2
	bls _0223C96C
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223C96C:
	mov r0, #0xc
	mul r0, r4, r0
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0223C984: .word 0x15555555
	arm_func_end ov66_223C950

	arm_func_start ov66_223C988
ov66_223C988: // 0x0223C988
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #0
	beq _0223C9D4
	ldr r1, [r4, #4]
	mov r3, #0
	strb r3, [sp]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov66_223C9E0
	ldr r1, [r4]
	cmp r1, #0
	beq _0223C9D4
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_0223C9D4:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223C988

	arm_func_start ov66_223C9E0
ov66_223C9E0: // 0x0223C9E0
	stmfd sp!, {r3, r4, r5, lr}
	ldr ip, [r0]
	ldr r3, [r0, #4]
	mov r2, #0xc
	mla r4, r3, r2, ip
	movs r5, r1
	sub r1, r3, r5
	str r1, [r0, #4]
	ldmeqia sp!, {r3, r4, r5, pc}
_0223CA04:
	sub r4, r4, #0xc
	mov r0, r4
	bl ov66_223BADC
	subs r5, r5, #1
	bne _0223CA04
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223C9E0

	arm_func_start ov66_223CA1C
ov66_223CA1C: // 0x0223CA1C
	mov r1, #0
	str r1, [r0]
	str r1, [r0, #4]
	str r1, [r0, #8]
	str r1, [r0, #0xc]
	str r1, [r0, #0x10]
	str r1, [r0, #0x14]
	str r1, [r0, #0x18]
	str r1, [r0, #0x1c]
	str r1, [r0, #0x20]
	strb r1, [r0, #0x28]
	strb r1, [r0, #0x29]
	str r1, [r0, #0x2c]
	str r1, [r0, #0x30]
	str r1, [r0, #0x34]
	str r1, [r0, #0x38]
	str r1, [r0, #0x3c]
	str r1, [r0, #0x40]
	str r1, [r0, #0x44]
	str r1, [r0, #0x48]
	str r1, [r0, #0x4c]
	str r1, [r0, #0x50]
	str r1, [r0, #0x54]
	str r1, [r0, #0x58]
	bx lr
	arm_func_end ov66_223CA1C

	arm_func_start ov66_223CA80
ov66_223CA80: // 0x0223CA80
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #0x30]
	ldr r0, [r2]
	sub r3, r3, #1
	mov r3, r3, lsl #3
	add r3, r3, #0x18
	mov r4, r1
	str r3, [r2]
	cmp r0, r3
	blo _0223CAB4
	cmp r4, #0
	bne _0223CABC
_0223CAB4:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_0223CABC:
	ldr r0, [r5, #0x1c]
	str r0, [r4]
	ldr r0, [r5, #0x20]
	str r0, [r4, #4]
	ldr r0, [r5, #0x24]
	str r0, [r4, #8]
	ldrb r0, [r5, #0x28]
	strb r0, [r4, #0xc]
	ldrb r0, [r5, #0x29]
	strb r0, [r4, #0xd]
	ldr r0, [r5, #0x30]
	strh r0, [r4, #0xe]
	ldrh r0, [r4, #0xe]
	cmp r0, #0
	beq _0223CB10
	add r0, r5, #0x2c
	bl ov66_223CB18
	ldr r2, [r5, #0x30]
	add r1, r4, #0x10
	mov r2, r2, lsl #3
	bl MI_CpuCopy8
_0223CB10:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223CA80

	arm_func_start ov66_223CB18
ov66_223CB18: // 0x0223CB18
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov66_223CB18

	arm_func_start ov66_223CB2C
ov66_223CB2C: // 0x0223CB2C
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r1, #0
	mov r5, r0
	add r2, sp, #0
	mov r0, r4
	str r1, [sp]
	bl ov66_223CA80
	ldr r1, [sp]
	mov r2, #0
	str r2, [r5]
	str r2, [r5, #4]
	mov r0, r5
	str r2, [r5, #8]
	bl ov66_223B6C0
	mov r0, r5
	bl ov66_2235B8C
	mov r1, r0
	add r2, sp, #0
	mov r0, r4
	bl ov66_223CA80
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223CB2C

	arm_func_start ov66_223CB84
ov66_223CB84: // 0x0223CB84
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	mov r5, r0
	mov r4, r1
	bl ov66_223EB80
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrb r0, [r4, #0xc]
	add r1, r4, #0x10
	mov r3, #0
	strb r0, [r5, #0x28]
	ldrb r0, [r4, #0xd]
	sub r2, sp, #4
	strb r0, [r5, #0x29]
	ldr ip, [r4]
	add r0, r5, #0x2c
	str ip, [r5, #0x1c]
	ldr ip, [r4, #4]
	str ip, [r5, #0x20]
	ldrh r4, [r4, #0xe]
	strb r3, [sp, #1]
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4, lsl #3
	bl ov66_223E9EC
	ldr r0, [r5, #0x2c]
	ldr r1, [r5, #0x30]
	ldrb r2, [sp, #2]
	sub r3, sp, #4
	add r1, r0, r1, lsl #3
	strb r2, [r3]
	ldr r2, [r3]
	str r1, [sp, #0x10]
	str r0, [sp, #0x14]
	bl ov66_223D0F4
	ldr r2, [r5, #0x2c]
	ldr r1, [r5, #0x30]
	ldrb r0, [sp]
	add r4, r2, r1, lsl #3
	str r4, [sp, #4]
	ldr r1, [r5, #0x2c]
	sub r2, sp, #4
	strb r0, [r2]
	ldr r3, [r2]
	add r2, sp, #4
	add r0, r5, #0x38
	str r4, [sp, #8]
	str r1, [sp, #0xc]
	bl ov66_223CC5C
	mov r0, #1
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223CB84

	arm_func_start ov66_223CC5C
ov66_223CC5C: // 0x0223CC5C
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x3c
	mov r5, r2
	ldr r3, [r5]
	ldr r2, [sp, #0x54]
	mov r6, r0
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r1, r0, asr #3
	str r1, [sp, #0x10]
	ldr r1, [r6, #4]
	str r2, [sp, #8]
	cmp r1, r0, asr #3
	addlo r0, r6, #4
	addhs r0, sp, #0x10
	ldr r4, [r0]
	ldr r1, [sp, #0x54]
	str r3, [sp, #4]
	add r3, r1, r4, lsl #3
	add r0, sp, #0x14
	mov r1, r6
	mov r2, #0
	str r3, [sp, #0xc]
	bl ov66_223F158
	add r1, sp, #0x54
	add r0, sp, #0xc
	ldr r2, [r0]
	ldr r1, [r1]
	add r0, sp, #0x28
	add r3, sp, #0x14
	bl ov66_223CF98
	ldr r0, [sp, #0x10]
	subs r0, r0, r4
	str r0, [sp, #0x10]
	beq _0223CD20
	ldrb r0, [sp, #1]
	sub r2, sp, #4
	add r1, sp, #0xc
	strb r0, [r2]
	ldr r3, [r2]
	mov r0, r6
	mov r2, r5
	bl ov66_223CD68
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_0223CD20:
	ldr r3, [r6, #4]
	cmp r4, r3
	addhs sp, sp, #0x3c
	ldmhsia sp!, {r3, r4, r5, r6, lr}
	addhs sp, sp, #0x10
	bxhs lr
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r6
	sub r1, r3, r4
	bl ov66_223F66C
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223CC5C

	arm_func_start ov66_223CD68
ov66_223CD68: // 0x0223CD68
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x38
	mov r5, r1
	mov r4, r2
	ldr r1, [r5]
	ldr ip, [r4]
	mov r6, r0
	cmp r1, ip
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r3, [r5]
	ldr r1, [r6, #0x10]
	sub r2, ip, r3
	mov r0, r2, asr #2
	add r0, r2, r0, lsr #29
	str r3, [sp, #0xc]
	cmp r1, #0
	mov r2, r0, asr #3
	subne r0, r1, #1
	moveq r3, #0
	movne r3, r0, lsl #7
	ldr r1, [r6]
	ldr r0, [r6, #4]
	str ip, [sp, #8]
	add r0, r1, r0
	sub r1, r3, r0
	cmp r2, r1
	bls _0223CDE4
	mov r0, r6
	sub r1, r2, r1
	bl ov66_223F1E4
_0223CDE4:
	ldr r2, [r6, #4]
	add r0, sp, #0x10
	mov r1, r6
	bl ov66_223F158
	add r0, r6, #8
	str r0, [sp]
	add r0, r6, #4
	str r0, [sp, #4]
	ldr r1, [r5]
	ldr r2, [r4]
	add r0, sp, #0x24
	add r3, sp, #0x10
	bl ov66_223CE20
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_223CD68

	arm_func_start ov66_223CE20
ov66_223CE20: // 0x0223CE20
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	ldr r4, [sp, #0x2c]
	ldr r2, [sp, #0x30]
	ldr r1, [sp, #0x3c]
	cmp r4, r2
	beq _0223CF60
	sub r4, r2, r4
	mov r2, r4, asr #2
	add r2, r4, r2, lsr #29
	mov r2, r2, asr #3
	str r2, [sp, #0x10]
_0223CE54:
	ldr r4, [r3, #0x10]
	cmp r4, #0
	moveq r2, r4
	ldrne r2, [r3]
	ldrne r2, [r2]
	addne r2, r2, #0x400
	sub r4, r2, r4
	mov r2, r4, asr #2
	add r2, r4, r2, lsr #29
	ldr r4, [sp, #0x10]
	mov ip, r2, asr #3
	cmp r4, r2, asr #3
	addgt r2, sp, #8
	str ip, [sp, #8]
	addle r2, sp, #0x10
	ldr r4, [sp, #0x2c]
	ldr r5, [r2]
	str r4, [sp, #4]
	add lr, r4, r5, lsl #3
	str lr, [sp, #0xc]
	str lr, [sp]
	cmp r4, lr
	ldr r6, [r3, #0x10]
	beq _0223CEEC
_0223CEB4:
	cmp r6, #0
	beq _0223CECC
	ldr ip, [r4]
	ldr r2, [r4, #4]
	str ip, [r6]
	str r2, [r6, #4]
_0223CECC:
	ldr r2, [r1]
	add r4, r4, #8
	add r2, r2, #1
	str r2, [r1]
	cmp r4, lr
	add r6, r6, #8
	bne _0223CEB4
	str r4, [sp, #4]
_0223CEEC:
	ldr ip, [sp, #0x10]
	sub r2, r5, #1
	sub ip, ip, r5
	str ip, [sp, #0x10]
	ldr ip, [r3, #0x10]
	add r2, ip, r2, lsl #3
	str r2, [r3, #0x10]
	add ip, r2, #8
	ldr r2, [r3]
	str ip, [r3, #0x10]
	ldr r2, [r2]
	add r2, r2, #0x400
	cmp ip, r2
	bne _0223CF4C
	ldr r2, [r3]
	add ip, r2, #4
	str ip, [r3]
	ldr r2, [r3, #0xc]
	cmp ip, r2
	ldreq r2, [r3, #4]
	streq r2, [r3]
	ldr r2, [r3]
	ldr r2, [r2]
	str r2, [r3, #0x10]
_0223CF4C:
	ldr r2, [sp, #0x10]
	cmp r2, #0
	ldrne r2, [sp, #0xc]
	strne r2, [sp, #0x2c]
	bne _0223CE54
_0223CF60:
	ldr r1, [r3]
	str r1, [r0]
	ldr r1, [r3, #4]
	str r1, [r0, #4]
	ldr r1, [r3, #8]
	str r1, [r0, #8]
	ldr r1, [r3, #0xc]
	str r1, [r0, #0xc]
	ldr r1, [r3, #0x10]
	str r1, [r0, #0x10]
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223CE20

	arm_func_start ov66_223CF98
ov66_223CF98: // 0x0223CF98
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x14
	ldr r2, [sp, #0x24]
	ldr r1, [sp, #0x28]
	cmp r2, r1
	beq _0223D0BC
	sub r2, r1, r2
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov r1, r1, asr #3
	str r1, [sp, #0x10]
_0223CFC8:
	ldr r2, [r3, #0x10]
	cmp r2, #0
	moveq r1, r2
	ldrne r1, [r3]
	ldrne r1, [r1]
	addne r1, r1, #0x400
	sub r2, r1, r2
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	ldr r2, [sp, #0x10]
	mov ip, r1, asr #3
	cmp r2, r1, asr #3
	addgt r1, sp, #8
	str ip, [sp, #8]
	addle r1, sp, #0x10
	ldr r2, [sp, #0x24]
	ldr ip, [r1]
	str r2, [sp, #4]
	add r4, r2, ip, lsl #3
	str r4, [sp, #0xc]
	str r4, [sp]
	cmp r2, r4
	ldr lr, [r3, #0x10]
	bhs _0223D048
_0223D028:
	ldr r1, [r2]
	str r1, [lr]
	ldr r1, [r2, #4]
	add r2, r2, #8
	str r1, [lr, #4]
	cmp r2, r4
	add lr, lr, #8
	blo _0223D028
_0223D048:
	ldr r2, [sp, #0x10]
	sub r1, ip, #1
	sub r2, r2, ip
	str r2, [sp, #0x10]
	ldr r2, [r3, #0x10]
	add r1, r2, r1, lsl #3
	str r1, [r3, #0x10]
	add r2, r1, #8
	ldr r1, [r3]
	str r2, [r3, #0x10]
	ldr r1, [r1]
	add r1, r1, #0x400
	cmp r2, r1
	bne _0223D0A8
	ldr r1, [r3]
	add r2, r1, #4
	str r2, [r3]
	ldr r1, [r3, #0xc]
	cmp r2, r1
	ldreq r1, [r3, #4]
	streq r1, [r3]
	ldr r1, [r3]
	ldr r1, [r1]
	str r1, [r3, #0x10]
_0223D0A8:
	ldr r1, [sp, #0x10]
	cmp r1, #0
	ldrne r1, [sp, #0xc]
	strne r1, [sp, #0x24]
	bne _0223CFC8
_0223D0BC:
	ldr r1, [r3]
	str r1, [r0]
	ldr r1, [r3, #4]
	str r1, [r0, #4]
	ldr r1, [r3, #8]
	str r1, [r0, #8]
	ldr r1, [r3, #0xc]
	str r1, [r0, #0xc]
	ldr r1, [r3, #0x10]
	str r1, [r0, #0x10]
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223CF98

	arm_func_start ov66_223D0F4
ov66_223D0F4: // 0x0223D0F4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x30
	ldr r1, [sp, #0x3c]
	ldr r0, [sp, #0x38]
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r1, r0, asr #3
	cmp r1, #0x14
	movle r1, #0
	add r0, sp, #0xc
	bl ov66_223E988
	ldr r4, [sp, #0xc]
	ldr r3, [sp, #0x10]
	add ip, sp, #0x24
	mov r2, #0
	add lr, r4, r3, lsl #3
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	str r4, [sp, #0x2c]
	add r0, sp, #0x38
	add r1, sp, #0x3c
	str lr, [sp]
	add r3, sp, #0x40
	str ip, [sp, #4]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x1c
	str r4, [sp, #0x1c]
	str ip, [sp, #0x20]
	str r3, [sp, #8]
	str lr, [sp, #0x14]
	str ip, [sp, #0x18]
	ldmia r2, {r2, r3}
	bl ov66_223D19C
	ldr r0, [sp, #0x2c]
	bl _ZdaPv
	add sp, sp, #0x30
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223D0F4

	arm_func_start ov66_223D19C
ov66_223D19C: // 0x0223D19C
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x4c
	ldr r0, [sp, #0x64]
	ldr r6, [sp, #0x60]
	ldr r7, [sp, #0x78]
	sub r1, r0, r6
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r5, r0, asr #3
	cmp r5, #1
	addle sp, sp, #0x4c
	ldmleia sp!, {r4, r5, r6, r7, lr}
	addle sp, sp, #0x10
	bxle lr
	cmp r5, #2
	bne _0223D240
	ldr r0, [sp, #0x64]
	ldr r1, [sp, #0x60]
	sub r4, r0, #8
	ldr r3, [r4]
	ldr r0, [r1]
	str r4, [sp, #0x64]
	cmp r3, r0
	addge sp, sp, #0x4c
	ldmgeia sp!, {r4, r5, r6, r7, lr}
	addge sp, sp, #0x10
	bxge lr
	ldr r2, [r1]
	ldr r1, [r1, #4]
	str r2, [sp, #0x1c]
	str r3, [r6]
	ldr r0, [r4, #4]
	str r1, [sp, #0x20]
	str r0, [r6, #4]
	str r2, [r4]
	add sp, sp, #0x4c
	str r1, [r4, #4]
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223D240:
	cmp r5, #0x14
	bgt _0223D270
	add r0, sp, #0x60
	add r1, sp, #0x64
	ldr r0, [r0]
	ldr r1, [r1]
	mov r2, r7
	bl ov66_223E8C8
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223D270:
	ldr r2, [sp, #0x70]
	ldr r1, [sp, #0x68]
	add r0, r5, r5, lsr #31
	sub r2, r2, r1
	mov r1, r2, asr #2
	add r2, r2, r1, lsr #29
	sub r4, r5, r0, asr #1
	ldr r1, [sp, #0x60]
	cmp r5, r2, asr #3
	add r1, r1, r4, lsl #3
	str r1, [sp, #0x48]
	add r0, sp, #0x60
	add r2, sp, #0x68
	bgt _0223D354
	ldr r0, [r0]
	str r7, [sp]
	ldmia r2, {r2, r3}
	bl ov66_223DFD4
	ldr r2, [sp, #0x68]
	add r0, sp, #0x48
	add r3, r2, r4, lsl #3
	add r1, sp, #0x64
	ldr r6, [sp, #0x6c]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x40
	str r6, [sp, #0x44]
	str r3, [sp, #0x40]
	str r7, [sp]
	ldmia r2, {r2, r3}
	bl ov66_223DFD4
	ldr r0, [sp, #0x68]
	add r1, sp, #0x68
	add r2, r0, r4, lsl #3
	add r3, r0, r5, lsl #3
	stmib sp, {r2, r6}
	str r3, [sp, #0xc]
	ldr r0, [sp, #0x60]
	str r6, [sp, #0x10]
	str r0, [sp, #0x14]
	str r7, [sp, #0x18]
	sub r0, sp, #4
	stmia r0, {r2, r6}
	str r3, [sp, #0x24]
	ldr r3, [r0]
	add r0, sp, #0x3c
	str r6, [sp, #0x28]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x34]
	str r6, [sp, #0x30]
	str r6, [sp, #0x38]
	ldmia r1, {r1, r2}
	bl ov66_223DE50
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223D354:
	ldr r4, [sp, #0x70]
	ldr r3, [sp, #0x74]
	str r4, [sp]
	stmib sp, {r3, r7}
	ldr r0, [r0]
	ldmia r2, {r2, r3}
	bl ov66_223D19C
	ldr r4, [sp, #0x70]
	add r0, sp, #0x48
	add r1, sp, #0x64
	ldr r3, [sp, #0x74]
	str r4, [sp]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x68
	stmib sp, {r3, r7}
	ldmia r2, {r2, r3}
	bl ov66_223D19C
	add r0, sp, #0x60
	add r1, sp, #0x48
	add r2, sp, #0x64
	ldr ip, [sp, #0x48]
	ldr r3, [sp, #0x60]
	ldr r0, [r0]
	sub r5, ip, r3
	mov r4, r5, asr #2
	add r4, r5, r4, lsr #29
	mov r4, r4, asr #3
	str r4, [sp]
	ldr r4, [sp, #0x64]
	ldr r5, [sp, #0x68]
	sub r6, r4, ip
	mov r4, r6, asr #2
	add r4, r6, r4, lsr #29
	mov r4, r4, asr #3
	str r4, [sp, #4]
	str r5, [sp, #8]
	ldr r4, [sp, #0x6c]
	ldr r5, [sp, #0x70]
	str r4, [sp, #0xc]
	ldr r1, [r1]
	ldr r2, [r2]
	ldr r4, [sp, #0x74]
	str r5, [sp, #0x10]
	mov r3, r7
	str r4, [sp, #0x14]
	bl ov66_223D420
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223D19C

	arm_func_start ov66_223D420
ov66_223D420: // 0x0223D420
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x88
	ldr r8, [sp, #0xb0]
	ldr r7, [sp, #0xb4]
	cmp r8, #0
	cmpne r7, #0
	mov r4, r3
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	addeq sp, sp, #0x10
	bxeq lr
	add r0, r8, r7
	cmp r0, #2
	bne _0223D4BC
	ldr r6, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	ldr r5, [r6]
	ldr r0, [r1]
	cmp r5, r0
	addge sp, sp, #0x88
	ldmgeia sp!, {r4, r5, r6, r7, r8, lr}
	addge sp, sp, #0x10
	bxge lr
	ldr r4, [r1]
	ldr r3, [r1, #4]
	ldr r2, [sp, #0xa0]
	ldr r0, [sp, #0xa4]
	str r5, [r2]
	ldr r1, [r6, #4]
	str r4, [sp, #0x54]
	str r1, [r2, #4]
	str r4, [r0]
	str r3, [sp, #0x58]
	str r3, [r0, #4]
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_0223D4BC:
	ldr r1, [sp, #0xc0]
	ldr r0, [sp, #0xb8]
	cmp r8, r7
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	cmple r8, r0, asr #3
	bgt _0223D604
	ldr r1, [sp, #0xa4]
	ldr r0, [sp, #0xa0]
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	str r0, [sp, #0x50]
	cmp r0, r1
	movne r0, #1
	moveq r0, #0
	str r3, [sp, #0x44]
	str r2, [sp, #0x48]
	str r1, [sp, #0x4c]
	cmp r0, #0
	beq _0223D5A8
	cmp r0, #0
	add r1, sp, #0x3c
	beq _0223D5A8
	ldr r5, [sp, #0x50]
	ldr r3, [sp, #0x4c]
_0223D524:
	ldr r0, [sp, #0x44]
	ldr r6, [sp, #0x48]
	str r0, [sp, #0x3c]
	str r6, [sp, #0x40]
	ldr r2, [r6, #8]
	ldr r6, [r6, #4]
	sub r7, r0, r2
	mov r2, r7, asr #2
	add r2, r7, r2, lsr #29
	cmp r6, r2, asr #3
	ble _0223D564
	ldr r2, [r5]
	str r2, [r0]
	ldr r2, [r5, #4]
	str r2, [r0, #4]
	b _0223D58C
_0223D564:
	cmp r0, #0
	beq _0223D57C
	ldr r6, [r5]
	ldr r2, [r5, #4]
	str r6, [r0]
	str r2, [r0, #4]
_0223D57C:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_0223D58C:
	ldr r0, [sp, #0x44]
	add r5, r5, #8
	add r0, r0, #8
	str r0, [sp, #0x44]
	cmp r5, r3
	bne _0223D524
	str r5, [sp, #0x50]
_0223D5A8:
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa8]
	str r0, [sp, #4]
	str r1, [sp, #8]
	ldr r0, [sp, #0xa0]
	ldr r5, [sp, #0x44]
	str r0, [sp, #0xc]
	ldr r2, [sp, #0x48]
	str r4, [sp, #0x10]
	sub r0, sp, #4
	str r5, [r0]
	str r2, [r0, #4]
	ldr r3, [r0]
	add r1, sp, #0xb8
	str r2, [sp, #0x78]
	add r0, sp, #0x7c
	str r5, [sp, #0x74]
	ldmia r1, {r1, r2}
	bl ov66_223DCF8
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_0223D604:
	cmp r7, r0, asr #3
	bgt _0223D738
	ldr r1, [sp, #0xa4]
	ldr r0, [sp, #0xa8]
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	cmp r1, r0
	movne r0, #1
	moveq r0, #0
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	str r1, [sp, #0x38]
	cmp r0, #0
	beq _0223D6D4
	cmp r0, #0
	add r1, sp, #0x28
	beq _0223D6D4
	ldr r5, [sp, #0x38]
	ldr r3, [sp, #0xa8]
_0223D650:
	ldr r0, [sp, #0x30]
	ldr r6, [sp, #0x34]
	str r0, [sp, #0x28]
	str r6, [sp, #0x2c]
	ldr r2, [r6, #8]
	ldr r6, [r6, #4]
	sub r7, r0, r2
	mov r2, r7, asr #2
	add r2, r7, r2, lsr #29
	cmp r6, r2, asr #3
	ble _0223D690
	ldr r2, [r5]
	str r2, [r0]
	ldr r2, [r5, #4]
	str r2, [r0, #4]
	b _0223D6B8
_0223D690:
	cmp r0, #0
	beq _0223D6A8
	ldr r6, [r5]
	ldr r2, [r5, #4]
	str r6, [r0]
	str r2, [r0, #4]
_0223D6A8:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_0223D6B8:
	ldr r0, [sp, #0x30]
	add r5, r5, #8
	add r0, r0, #8
	str r0, [sp, #0x30]
	cmp r5, r3
	bne _0223D650
	str r5, [sp, #0x38]
_0223D6D4:
	ldr r6, [sp, #0x30]
	ldr r5, [sp, #0x34]
	str r6, [sp, #4]
	ldr r0, [sp, #0xa8]
	str r5, [sp, #8]
	str r0, [sp, #0xc]
	str r4, [sp, #0x10]
	add r1, sp, #0xa0
	ldr r3, [sp, #0xb8]
	sub r4, sp, #4
	ldr r2, [sp, #0xbc]
	str r3, [r4]
	str r2, [r4, #4]
	add r0, sp, #0xa4
	ldr r2, [r0]
	ldr r3, [r4]
	ldr r1, [r1]
	add r0, sp, #0x70
	str r6, [sp, #0x68]
	str r5, [sp, #0x6c]
	bl ov66_223DAC4
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_0223D738:
	cmp r8, r7
	bgt _0223D798
	add r1, sp, #0xa0
	add r2, sp, #0xa4
	add r0, r7, r7, lsr #31
	mov r6, r0, asr #1
	ldr r3, [sp, #0xa4]
	ldr r1, [r1]
	add r3, r3, r6, lsl #3
	ldr r2, [r2]
	add r0, sp, #0x64
	str r4, [sp]
	str r3, [sp, #0x80]
	bl ov66_223DA44
	ldr r3, [sp, #0x64]
	ldr r2, [sp, #0xa0]
	str r3, [sp, #0x84]
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	mov r5, r0, asr #3
	b _0223D7EC
_0223D798:
	add r1, sp, #0xa4
	add r2, sp, #0xa8
	add r0, r8, r8, lsr #31
	mov r5, r0, asr #1
	ldr r3, [sp, #0xa0]
	ldr r1, [r1]
	add r3, r3, r5, lsl #3
	ldr r2, [r2]
	add r0, sp, #0x60
	str r4, [sp]
	str r3, [sp, #0x84]
	bl ov66_223D9C4
	ldr r3, [sp, #0x60]
	ldr r2, [sp, #0xa4]
	str r3, [sp, #0x80]
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	str r3, [sp, #0x18]
	str r2, [sp, #0x1c]
	mov r6, r0, asr #3
_0223D7EC:
	add r1, sp, #0x84
	add r2, sp, #0xa4
	add r0, sp, #0x80
	ldr r3, [r0]
	ldr r1, [r1]
	ldr r2, [r2]
	add r0, sp, #0x5c
	bl ov66_223D8B0
	ldr r2, [sp, #0x5c]
	add r0, sp, #0xa0
	add r1, sp, #0x84
	ldr r3, [sp, #0xb8]
	stmia sp, {r5, r6}
	str r3, [sp, #8]
	ldr ip, [sp, #0xbc]
	ldr r3, [sp, #0xc0]
	str ip, [sp, #0xc]
	str r3, [sp, #0x10]
	ldr ip, [sp, #0xc4]
	ldr r0, [r0]
	ldr r1, [r1]
	mov r3, r4
	str ip, [sp, #0x14]
	str r2, [sp, #0xa4]
	bl ov66_223D420
	sub r2, r8, r5
	str r2, [sp]
	sub r5, r7, r6
	str r5, [sp, #4]
	ldr r3, [sp, #0xb8]
	add r0, sp, #0xa4
	add r1, sp, #0x80
	add r2, sp, #0xa8
	str r3, [sp, #8]
	ldr r5, [sp, #0xbc]
	ldr r3, [sp, #0xc0]
	str r5, [sp, #0xc]
	str r3, [sp, #0x10]
	ldr r5, [sp, #0xc4]
	ldr r0, [r0]
	ldr r1, [r1]
	ldr r2, [r2]
	mov r3, r4
	str r5, [sp, #0x14]
	bl ov66_223D420
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223D420

	arm_func_start ov66_223D8B0
ov66_223D8B0: // 0x0223D8B0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	ldr r4, [sp, #0x24]
	ldr lr, [sp, #0x28]
	cmp r4, lr
	ldreq r1, [sp, #0x2c]
	addeq sp, sp, #0x10
	streq r1, [r0]
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr ip, [sp, #0x2c]
	cmp lr, ip
	ldreq r1, [sp, #0x24]
	addeq sp, sp, #0x10
	streq r1, [r0]
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r5, [sp, #0x28]
_0223D904:
	ldr r3, [r4]
	ldr r2, [r4, #4]
	ldr r1, [r5]
	str r3, [sp, #8]
	str r1, [r4]
	ldr r1, [r5, #4]
	str r2, [sp, #0xc]
	str r1, [r4, #4]
	str r3, [r5]
	str r2, [r5, #4]
	add r5, r5, #8
	cmp r5, ip
	add r4, r4, #8
	beq _0223D948
	cmp r4, lr
	moveq lr, r5
	b _0223D904
_0223D948:
	mov r5, r4
	cmp r4, lr
	beq _0223D9B0
	ldr r1, [sp, #0x2c]
	mov r6, lr
_0223D95C:
	ldr ip, [r4]
	ldr r3, [r4, #4]
	ldr r2, [r6]
	str ip, [sp]
	str r2, [r4]
	ldr r2, [r6, #4]
	str r3, [sp, #4]
	str r2, [r4, #4]
	str ip, [r6]
	str r3, [r6, #4]
	add r6, r6, #8
	cmp r6, r1
	add r4, r4, #8
	bne _0223D9A4
	cmp r4, lr
	beq _0223D9B0
	mov r6, lr
	b _0223D95C
_0223D9A4:
	cmp r4, lr
	moveq lr, r6
	b _0223D95C
_0223D9B0:
	str r5, [r0]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223D8B0

	arm_func_start ov66_223D9C4
ov66_223D9C4: // 0x0223D9C4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr ip, [sp, #0x14]
	ldr r1, [sp, #0x18]
	str ip, [sp]
	sub r2, r1, ip
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov lr, r1, asr #3
	cmp lr, #0
	ble _0223DA2C
	ldr r4, [r3]
_0223D9F8:
	add r1, lr, lr, lsr #31
	mov r3, r1, asr #1
	ldr r1, [ip, r3, lsl #3]
	add r2, ip, r3, lsl #3
	cmp r1, r4
	movge lr, r3
	bge _0223DA24
	add ip, r2, #8
	add r1, r3, #1
	str ip, [sp, #0x14]
	sub lr, lr, r1
_0223DA24:
	cmp lr, #0
	bgt _0223D9F8
_0223DA2C:
	ldr r1, [sp, #0x14]
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223D9C4

	arm_func_start ov66_223DA44
ov66_223DA44: // 0x0223DA44
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr ip, [sp, #0x14]
	ldr r1, [sp, #0x18]
	str ip, [sp]
	sub r2, r1, ip
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov lr, r1, asr #3
	cmp lr, #0
	ble _0223DAAC
	ldr r4, [r3]
_0223DA78:
	add r1, lr, lr, lsr #31
	mov r3, r1, asr #1
	ldr r1, [ip, r3, lsl #3]
	add r2, ip, r3, lsl #3
	cmp r4, r1
	movlt lr, r3
	blt _0223DAA4
	add ip, r2, #8
	add r1, r3, #1
	str ip, [sp, #0x14]
	sub lr, lr, r1
_0223DAA4:
	cmp lr, #0
	bgt _0223DA78
_0223DAAC:
	ldr r1, [sp, #0x14]
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223DA44

	arm_func_start ov66_223DAC4
ov66_223DAC4: // 0x0223DAC4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x40
	ldr r3, [sp, #0x54]
	ldr r1, [sp, #0x58]
	cmp r3, r1
	bne _0223DB50
	ldr r5, [sp, #0x6c]
	ldr r4, [sp, #0x64]
	ldr r3, [sp, #0x5c]
	ldr r2, [sp, #0x68]
	ldr r1, [sp, #0x60]
	str r5, [sp, #0x2c]
	str r4, [sp, #0x30]
	str r2, [sp, #0x34]
	str r3, [sp, #0x38]
	str r1, [sp, #0x3c]
	cmp r4, r3
	beq _0223DB38
_0223DB10:
	ldr r1, [sp, #0x30]
	sub r2, r1, #8
	ldr r1, [r2]
	str r2, [sp, #0x30]
	str r1, [r5, #-8]!
	ldr r1, [r2, #4]
	cmp r2, r3
	str r1, [r5, #4]
	bne _0223DB10
	str r5, [sp, #0x2c]
_0223DB38:
	ldr r1, [sp, #0x2c]
	add sp, sp, #0x40
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DB50:
	ldr r2, [sp, #0x5c]
	ldr r1, [sp, #0x64]
	cmp r2, r1
	bne _0223DBAC
	ldr r3, [sp, #0x6c]
	ldr r2, [sp, #0x58]
	ldr r4, [sp, #0x54]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	str r4, [sp, #0x28]
	cmp r2, r4
	bls _0223DB98
_0223DB80:
	ldr r1, [r2, #-8]!
	cmp r2, r4
	str r1, [r3, #-8]!
	ldr r1, [r2, #4]
	str r1, [r3, #4]
	bhi _0223DB80
_0223DB98:
	add sp, sp, #0x40
	str r3, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DBAC:
	ldr r1, [sp, #0x58]
	ldr r4, [sp, #0x64]
	sub r1, r1, #8
	sub r4, r4, #8
	ldr ip, [sp, #0x6c]
	str r1, [sp, #0x58]
	str r4, [sp, #0x64]
_0223DBC8:
	ldr r5, [sp, #0x64]
	ldr lr, [r1]
	ldr r4, [r5]
	cmp r4, lr
	bge _0223DC74
	str lr, [ip, #-8]!
	ldr lr, [r1, #4]
	str ip, [sp, #0x6c]
	str lr, [ip, #4]
	cmp r3, r1
	bne _0223DC68
	ldr r1, [sp, #0x64]
	ldr r2, [sp, #0x68]
	add lr, r1, #8
	ldr r3, [sp, #0x5c]
	ldr r1, [sp, #0x60]
	str ip, [sp, #0xc]
	str lr, [sp, #0x64]
	str lr, [sp, #0x10]
	str r2, [sp, #0x14]
	str r3, [sp, #0x18]
	str r1, [sp, #0x1c]
	cmp lr, r3
	beq _0223DC50
_0223DC28:
	ldr r1, [sp, #0x10]
	sub r2, r1, #8
	ldr r1, [r2]
	str r2, [sp, #0x10]
	str r1, [ip, #-8]!
	ldr r1, [r2, #4]
	cmp r2, r3
	str r1, [ip, #4]
	bne _0223DC28
	str ip, [sp, #0xc]
_0223DC50:
	ldr r1, [sp, #0xc]
	add sp, sp, #0x40
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DC68:
	sub r1, r1, #8
	str r1, [sp, #0x58]
	b _0223DBC8
_0223DC74:
	str r4, [ip, #-8]!
	ldr lr, [r5, #4]
	str ip, [sp, #0x6c]
	str lr, [ip, #4]
	cmp r2, r5
	bne _0223DCD8
	ldr r3, [sp, #0x54]
	add r2, r1, #8
	str ip, [sp]
	str r2, [sp, #0x58]
	str r2, [sp, #4]
	str r3, [sp, #8]
	cmp r2, r3
	bls _0223DCC4
_0223DCAC:
	ldr r1, [r2, #-8]!
	cmp r2, r3
	str r1, [ip, #-8]!
	ldr r1, [r2, #4]
	str r1, [ip, #4]
	bhi _0223DCAC
_0223DCC4:
	str ip, [r0]
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DCD8:
	ldr lr, [sp, #0x64]
	sub lr, lr, #8
	str lr, [sp, #0x64]
	b _0223DBC8
	arm_func_end ov66_223DAC4
_0223DCE8:
	.byte 0x40, 0xD0, 0x8D, 0xE2, 0x38, 0x40, 0xBD, 0xE8
	.byte 0x10, 0xD0, 0x8D, 0xE2, 0x1E, 0xFF, 0x2F, 0xE1

	arm_func_start ov66_223DCF8
ov66_223DCF8: // 0x0223DCF8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x20
	ldr r4, [sp, #0x44]
	ldr r1, [sp, #0x48]
	ldr ip, [sp, #0x4c]
	ldr r2, [sp, #0x3c]
	b _0223DD64
_0223DD18:
	ldr lr, [sp, #0x34]
	ldr r5, [r4]
	ldr r3, [lr]
	cmp r5, r3
	bge _0223DD44
	str r5, [ip]
	ldr r3, [r4, #4]
	add r4, r4, #8
	str r3, [ip, #4]
	str r4, [sp, #0x44]
	b _0223DD5C
_0223DD44:
	str r3, [ip]
	ldr r3, [sp, #0x34]
	ldr lr, [lr, #4]
	add r3, r3, #8
	str lr, [ip, #4]
	str r3, [sp, #0x34]
_0223DD5C:
	add ip, ip, #8
	str ip, [sp, #0x4c]
_0223DD64:
	ldr r3, [sp, #0x34]
	cmp r3, r2
	cmpne r4, r1
	bne _0223DD18
	ldr r2, [sp, #0x34]
	ldr r1, [sp, #0x3c]
	cmp r2, r1
	bne _0223DDD8
	ldr r3, [sp, #0x4c]
	ldr r4, [sp, #0x48]
	ldr r2, [sp, #0x44]
	str r3, [sp, #0x14]
	str r4, [sp, #0x18]
	str r2, [sp, #0x1c]
	cmp r2, r4
	bhs _0223DDC4
_0223DDA4:
	ldr r1, [r2]
	str r1, [r3]
	ldr r1, [r2, #4]
	add r2, r2, #8
	str r1, [r3, #4]
	cmp r2, r4
	add r3, r3, #8
	blo _0223DDA4
_0223DDC4:
	add sp, sp, #0x20
	str r3, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DDD8:
	ldr lr, [sp, #0x4c]
	ldr ip, [sp, #0x3c]
	ldr r2, [sp, #0x34]
	ldr r3, [sp, #0x40]
	ldr r1, [sp, #0x38]
	str lr, [sp]
	str ip, [sp, #4]
	str r3, [sp, #8]
	str r2, [sp, #0xc]
	str r1, [sp, #0x10]
	cmp r2, ip
	beq _0223DE38
_0223DE08:
	ldr r3, [sp, #0xc]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0xc]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _0223DE08
	str lr, [sp]
_0223DE38:
	ldr r1, [sp]
	str r1, [r0]
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223DCF8

	arm_func_start ov66_223DE50
ov66_223DE50: // 0x0223DE50
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x28
	ldr r1, [sp, #0x54]
	ldr ip, [sp, #0x5c]
	ldr r3, [sp, #0x44]
	b _0223DEC0
_0223DE6C:
	ldr r5, [sp, #0x4c]
	ldr lr, [sp, #0x3c]
	ldr r4, [r5]
	ldr r2, [lr]
	cmp r4, r2
	bge _0223DEA0
	str r4, [ip]
	ldr r2, [sp, #0x4c]
	ldr lr, [r5, #4]
	add r2, r2, #8
	str lr, [ip, #4]
	str r2, [sp, #0x4c]
	b _0223DEB8
_0223DEA0:
	str r2, [ip]
	ldr r2, [sp, #0x3c]
	ldr lr, [lr, #4]
	add r2, r2, #8
	str lr, [ip, #4]
	str r2, [sp, #0x3c]
_0223DEB8:
	add ip, ip, #8
	str ip, [sp, #0x5c]
_0223DEC0:
	ldr r2, [sp, #0x3c]
	cmp r2, r3
	ldrne r2, [sp, #0x4c]
	cmpne r2, r1
	bne _0223DE6C
	ldr r2, [sp, #0x3c]
	ldr r1, [sp, #0x44]
	cmp r2, r1
	bne _0223DF5C
	ldr lr, [sp, #0x5c]
	ldr ip, [sp, #0x54]
	ldr r2, [sp, #0x4c]
	ldr r3, [sp, #0x58]
	ldr r1, [sp, #0x50]
	str lr, [sp, #0x14]
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	str r2, [sp, #0x20]
	str r1, [sp, #0x24]
	cmp r2, ip
	beq _0223DF44
_0223DF14:
	ldr r3, [sp, #0x20]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0x20]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _0223DF14
	str lr, [sp, #0x14]
_0223DF44:
	ldr r1, [sp, #0x14]
	add sp, sp, #0x28
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223DF5C:
	ldr lr, [sp, #0x5c]
	ldr ip, [sp, #0x44]
	ldr r2, [sp, #0x3c]
	ldr r3, [sp, #0x48]
	ldr r1, [sp, #0x40]
	str lr, [sp]
	str ip, [sp, #4]
	str r3, [sp, #8]
	str r2, [sp, #0xc]
	str r1, [sp, #0x10]
	cmp r2, ip
	beq _0223DFBC
_0223DF8C:
	ldr r3, [sp, #0xc]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0xc]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _0223DF8C
	str lr, [sp]
_0223DFBC:
	ldr r1, [sp]
	str r1, [r0]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223DE50

	arm_func_start ov66_223DFD4
ov66_223DFD4: // 0x0223DFD4
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x80
	ldr r1, [sp, #0x9c]
	ldr r0, [sp, #0x98]
	ldr r7, [sp, #0xa8]
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r5, r0, asr #3
	cmp r5, #0
	addle sp, sp, #0x80
	ldmleia sp!, {r3, r4, r5, r6, r7, lr}
	addle sp, sp, #0x10
	bxle lr
	cmp r5, #1
	bne _0223E0A8
	ldr r3, [sp, #0xa0]
	ldr r1, [sp, #0xa4]
	str r3, [sp, #0x74]
	str r1, [sp, #0x78]
	ldr r0, [r1, #8]
	ldr r1, [r1, #4]
	sub r2, r3, r0
	mov r0, r2, asr #2
	add r0, r2, r0, lsr #29
	cmp r1, r0, asr #3
	add r2, sp, #0x74
	ble _0223E06C
	ldr r1, [sp, #0x98]
	add sp, sp, #0x80
	ldr r0, [r1]
	str r0, [r3]
	ldr r0, [r1, #4]
	str r0, [r3, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223E06C:
	cmp r3, #0
	beq _0223E088
	ldr r0, [sp, #0x98]
	ldr r1, [r0]
	ldr r0, [r0, #4]
	str r1, [r3]
	str r0, [r3, #4]
_0223E088:
	ldr r1, [r2, #4]
	add sp, sp, #0x80
	ldr r0, [r1, #4]
	add r0, r0, #1
	str r0, [r1, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223E0A8:
	cmp r5, #2
	bne _0223E2A4
	ldr r3, [sp, #0x9c]
	ldr r2, [sp, #0x98]
	ldr r1, [r3, #-8]
	ldr r0, [r2]
	cmp r1, r0
	bge _0223E1B8
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	str r0, [sp, #0x34]
	cmp r2, r3
	movne r0, #1
	moveq r0, #0
	str r1, [sp, #0x30]
	str r3, [sp, #0x38]
	str r2, [sp, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	cmp r0, #0
	add r2, sp, #0x28
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r4, [sp, #0x3c]
	ldr r1, [sp, #0x38]
_0223E120:
	ldr r0, [sp, #0x30]
	ldr r5, [sp, #0x34]
	str r0, [sp, #0x28]
	str r5, [sp, #0x2c]
	ldr r3, [r5, #8]
	sub r1, r1, #8
	sub r6, r0, r3
	mov r3, r6, asr #2
	ldr r5, [r5, #4]
	add r3, r6, r3, lsr #29
	str r1, [sp, #0x38]
	cmp r5, r3, asr #3
	ble _0223E168
	ldr r3, [r1]
	str r3, [r0]
	ldr r3, [r1, #4]
	str r3, [r0, #4]
	b _0223E190
_0223E168:
	cmp r0, #0
	beq _0223E180
	ldr r5, [r1]
	ldr r3, [r1, #4]
	str r5, [r0]
	str r3, [r0, #4]
_0223E180:
	ldr r3, [r2, #4]
	ldr r0, [r3, #4]
	add r0, r0, #1
	str r0, [r3, #4]
_0223E190:
	ldr r3, [sp, #0x30]
	ldr r0, [sp, #0x38]
	add r3, r3, #8
	str r3, [sp, #0x30]
	cmp r4, r0
	bne _0223E120
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223E1B8:
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	str r0, [sp, #0x1c]
	cmp r2, r3
	movne r0, #1
	moveq r0, #0
	str r1, [sp, #0x18]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	cmp r0, #0
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	cmp r0, #0
	add r1, sp, #0x10
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r4, [sp, #0x24]
	ldr r3, [sp, #0x20]
_0223E210:
	ldr r0, [sp, #0x18]
	ldr r5, [sp, #0x1c]
	str r0, [sp, #0x10]
	str r5, [sp, #0x14]
	ldr r2, [r5, #8]
	ldr r5, [r5, #4]
	sub r6, r0, r2
	mov r2, r6, asr #2
	add r2, r6, r2, lsr #29
	cmp r5, r2, asr #3
	ble _0223E250
	ldr r2, [r4]
	str r2, [r0]
	ldr r2, [r4, #4]
	str r2, [r0, #4]
	b _0223E278
_0223E250:
	cmp r0, #0
	beq _0223E268
	ldr r5, [r4]
	ldr r2, [r4, #4]
	str r5, [r0]
	str r2, [r0, #4]
_0223E268:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_0223E278:
	ldr r0, [sp, #0x18]
	add r4, r4, #8
	add r0, r0, #8
	str r0, [sp, #0x18]
	cmp r4, r3
	bne _0223E210
	str r4, [sp, #0x24]
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223E2A4:
	cmp r5, #0x14
	bgt _0223E2F0
	ldr r3, [sp, #0xa0]
	str r7, [sp, #4]
	sub r4, sp, #4
	add r1, sp, #0x98
	ldr r2, [sp, #0xa4]
	str r3, [r4]
	str r2, [r4, #4]
	add r0, sp, #0x9c
	ldr r2, [r0]
	ldr r3, [r4]
	ldr r1, [r1]
	add r0, sp, #0x6c
	bl ov66_223E6EC
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223E2F0:
	add r0, r5, r5, lsr #31
	sub r4, r5, r0, asr #1
	ldr r1, [sp, #0xa0]
	ldr r6, [sp, #0xa4]
	add ip, r1, r4, lsl #3
	ldr r0, [sp, #0x98]
	str ip, [sp]
	add r1, r0, r4, lsl #3
	add r3, sp, #0x98
	str r6, [sp, #4]
	ldr r0, [r3]
	add r2, sp, #0xa0
	str r1, [sp, #0x7c]
	str ip, [sp, #0x64]
	str r6, [sp, #0x68]
	str r7, [sp, #8]
	ldmia r2, {r2, r3}
	bl ov66_223D19C
	ldr r2, [sp, #0xa0]
	add r0, sp, #0x7c
	add ip, r2, r5, lsl #3
	add r3, r2, r4, lsl #3
	add r1, sp, #0x9c
	str ip, [sp]
	str r6, [sp, #4]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x5c
	str r6, [sp, #0x60]
	str r3, [sp, #0x5c]
	str ip, [sp, #0x54]
	str r6, [sp, #0x58]
	str r7, [sp, #8]
	ldmia r2, {r2, r3}
	bl ov66_223D19C
	ldr r1, [sp, #0x98]
	ldr r3, [sp, #0xa0]
	add r2, r1, r4, lsl #3
	add r4, r1, r5, lsl #3
	str r4, [sp]
	str r3, [sp, #4]
	ldr r1, [sp, #0xa4]
	add r0, sp, #0x98
	str r1, [sp, #8]
	ldr r1, [r0]
	add r0, sp, #0x4c
	mov r3, r2
	str r4, [sp, #0x40]
	str r2, [sp, #0x44]
	str r2, [sp, #0x48]
	str r7, [sp, #0xc]
	bl ov66_223E3D0
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223DFD4

	arm_func_start ov66_223E3D0
ov66_223E3D0: // 0x0223E3D0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x40
	ldr r3, [sp, #0x64]
	ldr r5, [sp, #0x68]
	ldr r1, [sp, #0x5c]
	ldr r6, [sp, #0x60]
	add ip, sp, #0x38
	add lr, sp, #0x30
	b _0223E4F8
_0223E3F8:
	ldr r4, [r3]
	ldr r2, [r1]
	cmp r4, r2
	bge _0223E47C
	ldr r2, [sp, #0x6c]
	ldr r7, [sp, #0x70]
	str r2, [sp, #0x38]
	str r7, [sp, #0x3c]
	ldr r4, [r7, #8]
	ldr r7, [r7, #4]
	sub r8, r2, r4
	mov r4, r8, asr #2
	add r4, r8, r4, lsr #29
	cmp r7, r4, asr #3
	ble _0223E448
	ldr r4, [r3]
	str r4, [r2]
	ldr r4, [r3, #4]
	str r4, [r2, #4]
	b _0223E470
_0223E448:
	cmp r2, #0
	beq _0223E460
	ldr r7, [r3]
	ldr r4, [r3, #4]
	str r7, [r2]
	str r4, [r2, #4]
_0223E460:
	ldr r4, [ip, #4]
	ldr r2, [r4, #4]
	add r2, r2, #1
	str r2, [r4, #4]
_0223E470:
	add r3, r3, #8
	str r3, [sp, #0x64]
	b _0223E4EC
_0223E47C:
	ldr r2, [sp, #0x6c]
	ldr r7, [sp, #0x70]
	str r2, [sp, #0x30]
	str r7, [sp, #0x34]
	ldr r4, [r7, #8]
	ldr r7, [r7, #4]
	sub r8, r2, r4
	mov r4, r8, asr #2
	add r4, r8, r4, lsr #29
	cmp r7, r4, asr #3
	ble _0223E4BC
	ldr r4, [r1]
	str r4, [r2]
	ldr r4, [r1, #4]
	str r4, [r2, #4]
	b _0223E4E4
_0223E4BC:
	cmp r2, #0
	beq _0223E4D4
	ldr r7, [r1]
	ldr r4, [r1, #4]
	str r7, [r2]
	str r4, [r2, #4]
_0223E4D4:
	ldr r4, [lr, #4]
	ldr r2, [r4, #4]
	add r2, r2, #1
	str r2, [r4, #4]
_0223E4E4:
	add r1, r1, #8
	str r1, [sp, #0x5c]
_0223E4EC:
	ldr r2, [sp, #0x6c]
	add r2, r2, #8
	str r2, [sp, #0x6c]
_0223E4F8:
	cmp r1, r6
	cmpne r3, r5
	bne _0223E3F8
	ldr r2, [sp, #0x5c]
	ldr r1, [sp, #0x60]
	cmp r2, r1
	bne _0223E600
	ldr r2, [sp, #0x68]
	ldr r1, [sp, #0x64]
	ldr r4, [sp, #0x6c]
	ldr r3, [sp, #0x70]
	str r1, [sp, #0x2c]
	cmp r1, r2
	movne r1, #1
	moveq r1, #0
	str r4, [sp, #0x20]
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	cmp r1, #0
	beq _0223E5E0
	cmp r1, #0
	add r2, sp, #0x18
	beq _0223E5E0
	ldr r5, [sp, #0x2c]
	ldr r4, [sp, #0x28]
_0223E55C:
	ldr r1, [sp, #0x20]
	ldr ip, [sp, #0x24]
	str r1, [sp, #0x18]
	str ip, [sp, #0x1c]
	ldr r3, [ip, #8]
	ldr ip, [ip, #4]
	sub lr, r1, r3
	mov r3, lr, asr #2
	add r3, lr, r3, lsr #29
	cmp ip, r3, asr #3
	ble _0223E59C
	ldr r3, [r5]
	str r3, [r1]
	ldr r3, [r5, #4]
	str r3, [r1, #4]
	b _0223E5C4
_0223E59C:
	cmp r1, #0
	beq _0223E5B4
	ldr ip, [r5]
	ldr r3, [r5, #4]
	str ip, [r1]
	str r3, [r1, #4]
_0223E5B4:
	ldr r3, [r2, #4]
	ldr r1, [r3, #4]
	add r1, r1, #1
	str r1, [r3, #4]
_0223E5C4:
	ldr r1, [sp, #0x20]
	add r5, r5, #8
	add r1, r1, #8
	str r1, [sp, #0x20]
	cmp r5, r4
	bne _0223E55C
	str r5, [sp, #0x2c]
_0223E5E0:
	ldr r2, [sp, #0x20]
	ldr r1, [sp, #0x24]
	str r2, [r0]
	str r1, [r0, #4]
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_0223E600:
	ldr r2, [sp, #0x60]
	ldr r1, [sp, #0x5c]
	ldr r4, [sp, #0x6c]
	ldr r3, [sp, #0x70]
	str r1, [sp, #0x14]
	cmp r1, r2
	movne r1, #1
	moveq r1, #0
	str r4, [sp, #8]
	str r3, [sp, #0xc]
	str r2, [sp, #0x10]
	cmp r1, #0
	beq _0223E6CC
	cmp r1, #0
	add r2, sp, #0
	beq _0223E6CC
	ldr ip, [sp, #0x14]
	ldr r4, [sp, #0x10]
_0223E648:
	ldr r1, [sp, #8]
	ldr r5, [sp, #0xc]
	str r1, [sp]
	str r5, [sp, #4]
	ldr r3, [r5, #8]
	ldr lr, [r5, #4]
	sub r5, r1, r3
	mov r3, r5, asr #2
	add r3, r5, r3, lsr #29
	cmp lr, r3, asr #3
	ble _0223E688
	ldr r3, [ip]
	str r3, [r1]
	ldr r3, [ip, #4]
	str r3, [r1, #4]
	b _0223E6B0
_0223E688:
	cmp r1, #0
	beq _0223E6A0
	ldr lr, [ip]
	ldr r3, [ip, #4]
	str lr, [r1]
	str r3, [r1, #4]
_0223E6A0:
	ldr r3, [r2, #4]
	ldr r1, [r3, #4]
	add r1, r1, #1
	str r1, [r3, #4]
_0223E6B0:
	ldr r1, [sp, #8]
	add ip, ip, #8
	add r1, r1, #8
	str r1, [sp, #8]
	cmp ip, r4
	bne _0223E648
	str ip, [sp, #0x14]
_0223E6CC:
	ldr r2, [sp, #8]
	ldr r1, [sp, #0xc]
	str r2, [r0]
	str r1, [r0, #4]
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223E3D0

	arm_func_start ov66_223E6EC
ov66_223E6EC: // 0x0223E6EC
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x18
	ldr r3, [sp, #0x3c]
	ldr r1, [sp, #0x40]
	ldr r4, [sp, #0x48]
	cmp r3, r1
	ldr ip, [sp, #0x44]
	beq _0223E8B0
	ldr r1, [sp, #0x44]
	str r4, [sp, #0x14]
	str r1, [sp, #0x10]
	ldr r2, [r4, #8]
	ldr r5, [r4, #4]
	sub r6, r1, r2
	mov r2, r6, asr #2
	add r2, r6, r2, lsr #29
	cmp r5, r2, asr #3
	add r6, sp, #0x10
	ble _0223E754
	ldr r5, [sp, #0x3c]
	ldr r2, [r5]
	str r2, [r1]
	ldr r2, [r5, #4]
	str r2, [r1, #4]
	b _0223E780
_0223E754:
	cmp r1, #0
	beq _0223E770
	ldr r2, [sp, #0x3c]
	ldr r5, [r2]
	ldr r2, [r2, #4]
	str r5, [r1]
	str r2, [r1, #4]
_0223E770:
	ldr r2, [r6, #4]
	ldr r1, [r2, #4]
	add r1, r1, #1
	str r1, [r2, #4]
_0223E780:
	ldr r1, [sp, #0x40]
	add r3, r3, #8
	cmp r3, r1
	movne r1, #1
	moveq r1, #0
	add ip, ip, #8
	cmp r1, #0
	beq _0223E8B0
	cmp r1, #0
	add r2, sp, #0
	beq _0223E8B0
_0223E7AC:
	mov lr, ip
	mov r5, ip
	ldr r8, [sp, #0x44]
	add r1, sp, #8
	b _0223E824
_0223E7C0:
	str lr, [sp, #8]
	str r4, [sp, #0xc]
	ldr r6, [r4, #8]
	ldr r7, [r4, #4]
	sub sb, lr, r6
	mov r6, sb, asr #2
	add r6, sb, r6, lsr #29
	cmp r7, r6, asr #3
	ble _0223E7F8
	ldr r6, [r5]
	str r6, [lr]
	ldr r6, [r5, #4]
	str r6, [lr, #4]
	b _0223E820
_0223E7F8:
	cmp lr, #0
	beq _0223E810
	ldr r7, [r5]
	ldr r6, [r5, #4]
	str r7, [lr]
	str r6, [lr, #4]
_0223E810:
	ldr r7, [r1, #4]
	ldr r6, [r7, #4]
	add r6, r6, #1
	str r6, [r7, #4]
_0223E820:
	sub lr, lr, #8
_0223E824:
	cmp r5, r8
	beq _0223E83C
	ldr r7, [r3]
	ldr r6, [r5, #-8]!
	cmp r7, r6
	blt _0223E7C0
_0223E83C:
	str lr, [sp]
	str r4, [sp, #4]
	ldr r1, [r4, #8]
	ldr r5, [r4, #4]
	sub r6, lr, r1
	mov r1, r6, asr #2
	add r1, r6, r1, lsr #29
	cmp r5, r1, asr #3
	ble _0223E874
	ldr r1, [r3]
	str r1, [lr]
	ldr r1, [r3, #4]
	str r1, [lr, #4]
	b _0223E89C
_0223E874:
	cmp lr, #0
	beq _0223E88C
	ldr r5, [r3]
	ldr r1, [r3, #4]
	str r5, [lr]
	str r1, [lr, #4]
_0223E88C:
	ldr r5, [r2, #4]
	ldr r1, [r5, #4]
	add r1, r1, #1
	str r1, [r5, #4]
_0223E89C:
	ldr r1, [sp, #0x40]
	add r3, r3, #8
	cmp r3, r1
	add ip, ip, #8
	bne _0223E7AC
_0223E8B0:
	str ip, [r0]
	str r4, [r0, #4]
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223E6EC

	arm_func_start ov66_223E8C8
ov66_223E8C8: // 0x0223E8C8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	ldr ip, [sp, #0x18]
	ldr r3, [sp, #0x1c]
	cmp ip, r3
	ldrne r0, [sp, #0x18]
	addne lr, r0, #8
	cmpne lr, r3
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
_0223E8FC:
	sub r5, lr, #8
	ldr r1, [lr]
	ldr r0, [r5]
	cmp r1, r0
	bge _0223E96C
	ldr r2, [lr]
	ldr r1, [lr, #4]
	mov r4, r5
	str r0, [lr]
	ldr r0, [r5, #4]
	str r2, [sp]
	str r1, [sp, #4]
	str r0, [lr, #4]
	b _0223E948
_0223E934:
	ldr r0, [r5]
	str r0, [r4]
	ldr r0, [r5, #4]
	str r0, [r4, #4]
	sub r4, r4, #8
_0223E948:
	cmp r5, ip
	beq _0223E95C
	ldr r0, [r5, #-8]!
	cmp r2, r0
	blt _0223E934
_0223E95C:
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [r4]
	str r0, [r4, #4]
_0223E96C:
	add lr, lr, #8
	cmp lr, r3
	bne _0223E8FC
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_223E8C8

	arm_func_start ov66_223E988
ov66_223E988: // 0x0223E988
	stmfd sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r2, #0
	ldr r0, _0223E9E4 // =0x02259098
	str r2, [r6]
	ldr r0, [r0, #4]
	mov r5, r1
	str r0, [r6, #4]
	cmp r5, #0
	ldmleia sp!, {r4, r5, r6, pc}
	ldr r4, _0223E9E8 // =0x021D0D60
_0223E9B4:
	mov r1, r4
	mov r0, r5, lsl #3
	bl _ZnamRKSt9nothrow_t
	str r0, [r6]
	cmp r0, #0
	strne r5, [r6, #4]
	ldmneia sp!, {r4, r5, r6, pc}
	add r0, r5, r5, lsr #31
	mov r5, r0, asr #1
	cmp r5, #0
	bgt _0223E9B4
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_0223E9E4: .word 0x02259098
_0223E9E8: .word 0x021D0D60
	arm_func_end ov66_223E988

	arm_func_start ov66_223E9EC
ov66_223E9EC: // 0x0223E9EC
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r1
	mov r4, r2
	sub r2, r4, r5
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov ip, r1, asr #3
	mov r6, r0
	str ip, [sp, #4]
	ldr r3, [r6, #8]
	cmp r3, r1, asr #3
	bhs _0223EA38
	mov r2, #0
	sub r1, ip, r3
	strb r2, [sp, #3]
	bl ov66_223FDD8
	mov r2, r0
	b _0223EB18
_0223EA38:
	ldr r1, [r6, #4]
	mov r0, r5
	cmp r1, ip
	addlo r1, r6, #4
	addhs r1, sp, #4
	ldr r1, [r1]
	ldr r2, [r6]
	add r0, r0, r1, lsl #3
	cmp r5, r0
	bhs _0223EA80
_0223EA60:
	ldr r1, [r5]
	str r1, [r2]
	ldr r1, [r5, #4]
	add r5, r5, #8
	str r1, [r2, #4]
	cmp r5, r0
	add r2, r2, #8
	blo _0223EA60
_0223EA80:
	ldr r3, [r6, #4]
	ldr r2, [sp, #4]
	cmp r2, r3
	bhs _0223EAB0
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r6, #4]
	sub r0, r3, r2
	sub r0, r1, r0
	add sp, sp, #8
	str r0, [r6, #4]
	ldmia sp!, {r4, r5, r6, pc}
_0223EAB0:
	cmp r3, r2
	addhs sp, sp, #8
	ldmhsia sp!, {r4, r5, r6, pc}
	mov r1, #0
	strb r1, [sp, #1]
	ldr r2, [r6]
	ldr r1, [r6, #4]
	cmp r0, r4
	addeq sp, sp, #8
	add r3, r2, r1, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223EADC:
	cmp r3, #0
	beq _0223EAF4
	ldr r2, [r0]
	ldr r1, [r0, #4]
	str r2, [r3]
	str r1, [r3, #4]
_0223EAF4:
	ldr r1, [r6, #4]
	add r0, r0, #8
	add r1, r1, #1
	str r1, [r6, #4]
	cmp r0, r4
	add r3, r3, #8
	bne _0223EADC
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
_0223EB18:
	ldr r1, [sp, #4]
	mov r0, r6
	bl ov66_223FD68
	mov r0, #0
	strb r0, [sp]
	cmp r5, r4
	ldr r1, [r6]
	ldr r0, [r6, #4]
	addeq sp, sp, #8
	add r2, r1, r0, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223EB44:
	cmp r2, #0
	beq _0223EB5C
	ldr r1, [r5]
	ldr r0, [r5, #4]
	str r1, [r2]
	str r0, [r2, #4]
_0223EB5C:
	ldr r0, [r6, #4]
	add r5, r5, #8
	add r0, r0, #1
	str r0, [r6, #4]
	cmp r5, r4
	add r2, r2, #8
	bne _0223EB44
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_223E9EC

	arm_func_start ov66_223EB80
ov66_223EB80: // 0x0223EB80
	cmp r2, #0x10
	movlo r0, #0
	bxlo lr
	ldrh r0, [r1, #0xe]
	sub r0, r0, #1
	mov r0, r0, lsl #3
	add r0, r0, #0x18
	cmp r2, r0
	movhs r0, #1
	movlo r0, #0
	bx lr
	arm_func_end ov66_223EB80

	arm_func_start ov66_223EBAC
ov66_223EBAC: // 0x0223EBAC
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl ov66_223ED8C
	mov r1, #0
	bic r2, r4, #7
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r3, [r0]
	mov r1, r5
	add r0, r6, #0x50
	add r2, r5, r2
	bl ov66_223EBF8
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_223EBAC

	arm_func_start ov66_223EBF8
ov66_223EBF8: // 0x0223EBF8
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r1
	mov r4, r2
	sub r2, r4, r5
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov ip, r1, asr #3
	mov r6, r0
	str ip, [sp, #4]
	ldr r3, [r6, #8]
	cmp r3, r1, asr #3
	bhs _0223EC44
	mov r2, #0
	sub r1, ip, r3
	strb r2, [sp, #3]
	bl ov66_223FC9C
	mov r2, r0
	b _0223ED24
_0223EC44:
	ldr r1, [r6, #4]
	mov r0, r5
	cmp r1, ip
	addlo r1, r6, #4
	addhs r1, sp, #4
	ldr r1, [r1]
	ldr r2, [r6]
	add r0, r0, r1, lsl #3
	cmp r5, r0
	bhs _0223EC8C
_0223EC6C:
	ldr r1, [r5]
	str r1, [r2]
	ldr r1, [r5, #4]
	add r5, r5, #8
	str r1, [r2, #4]
	cmp r5, r0
	add r2, r2, #8
	blo _0223EC6C
_0223EC8C:
	ldr r3, [r6, #4]
	ldr r2, [sp, #4]
	cmp r2, r3
	bhs _0223ECBC
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r6, #4]
	sub r0, r3, r2
	sub r0, r1, r0
	add sp, sp, #8
	str r0, [r6, #4]
	ldmia sp!, {r4, r5, r6, pc}
_0223ECBC:
	cmp r3, r2
	addhs sp, sp, #8
	ldmhsia sp!, {r4, r5, r6, pc}
	mov r1, #0
	strb r1, [sp, #1]
	ldr r2, [r6]
	ldr r1, [r6, #4]
	cmp r0, r4
	addeq sp, sp, #8
	add r3, r2, r1, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223ECE8:
	cmp r3, #0
	beq _0223ED00
	ldr r2, [r0]
	ldr r1, [r0, #4]
	str r2, [r3]
	str r1, [r3, #4]
_0223ED00:
	ldr r1, [r6, #4]
	add r0, r0, #8
	add r1, r1, #1
	str r1, [r6, #4]
	cmp r0, r4
	add r3, r3, #8
	bne _0223ECE8
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
_0223ED24:
	ldr r1, [sp, #4]
	mov r0, r6
	bl ov66_223FC2C
	mov r0, #0
	strb r0, [sp]
	cmp r5, r4
	ldr r1, [r6]
	ldr r0, [r6, #4]
	addeq sp, sp, #8
	add r2, r1, r0, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223ED50:
	cmp r2, #0
	beq _0223ED68
	ldr r1, [r5]
	ldr r0, [r5, #4]
	str r1, [r2]
	str r0, [r2, #4]
_0223ED68:
	ldr r0, [r6, #4]
	add r5, r5, #8
	add r0, r0, #1
	str r0, [r6, #4]
	cmp r5, r4
	add r2, r2, #8
	bne _0223ED50
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_223EBF8

	arm_func_start ov66_223ED8C
ov66_223ED8C: // 0x0223ED8C
	stmdb sp!, {r3}
	sub sp, sp, #4
	tst r2, #7
	addne sp, sp, #4
	movne r0, #0
	ldmneia sp!, {r3}
	bxne lr
	cmp r2, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3}
	bxne lr
	ldr r2, [r0, #0x54]
	mov r1, #0
	strb r1, [sp]
	ldr r1, [r0, #0x54]
	sub r1, r1, r2
	str r1, [r0, #0x54]
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3}
	bx lr
	arm_func_end ov66_223ED8C

	arm_func_start ov66_223EDE4
ov66_223EDE4: // 0x0223EDE4
	stmfd sp!, {r3, lr}
	ldr r2, [r0, #0x334]
	cmp r2, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	add r0, r0, #0x5c
	mov r2, #0x2d8
	bl MI_CpuCopy8
	mov r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223EDE4

	arm_func_start ov66_223EE0C
ov66_223EE0C: // 0x0223EE0C
	stmfd sp!, {r4, lr}
	mov r4, r0
	mov r0, r1
	add r1, r4, #0x5c
	mov r2, #0x2d8
	bl MI_CpuCopy8
	mov r0, #1
	str r0, [r4, #0x334]
	ldmia sp!, {r4, pc}
	arm_func_end ov66_223EE0C

	arm_func_start ov66_223EE30
ov66_223EE30: // 0x0223EE30
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #5
	addls pc, pc, r1, lsl #2
	b _0223F000
_0223EE4C: // jump table
	b _0223F000 // case 0
	b _0223F000 // case 1
	b _0223F000 // case 2
	b _0223F000 // case 3
	b _0223EE64 // case 4
	b _0223EEE8 // case 5
_0223EE64:
	bl ov66_223F00C
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl ov66_2234FC4
	mov r1, r0
	add r0, sp, #0x30
	add r1, r1, #0xb8
	bl ov66_2235FC8
	ldr r0, [sp, #0x30]
	ldr r1, _0223F008 // =0x02258C8C
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	bl FUN_02245C54
	add r0, sp, #0x30
	bl ov66_223BADC
	mov r1, #0
	str r1, [sp, #0x10]
	mov r5, #4
	add r0, sp, #0x10
	str r1, [sp]
	str r0, [sp, #4]
	mov r3, r1
	sub r0, r5, #5
	mov r2, #1
	str r5, [sp, #8]
	bl ov66_2237170
	mov r0, r4
	bl ov66_223F0D0
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223EEE8:
	ldr r0, [r4, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, [r4, #0x38]
	ldr r0, [r4, #0x44]
	ldr r1, [r4, #0x4c]
	add r0, r0, r5, lsr #7
	bl _u32_div_f
	ldr r2, [r4, #0x40]
	add r0, sp, #0x14
	ldr r6, [r2, r1, lsl #2]
	mov r5, r5, lsl #0x19
	add r1, sp, #0x24
	add r7, r6, r5, lsr #22
	bl DWC_GetDateTime
	add r0, sp, #0x14
	add r1, sp, #0x24
	bl RTC_ConvertDateTimeToSecond
	mov r8, r0
	ldr r2, [r4, #0x10]
	ldr r3, [r6, r5, lsr #22]
	ldr r0, [r4, #0x14]
	mov sl, r1
	subs r2, r8, r2
	sbc r1, sl, r0
	subs r0, r2, r3
	sbcs r0, r1, r3, asr #31
	addlt sp, sp, #0x3c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	sub r5, sp, #4
	add r6, sp, #0xc
	mov fp, #0
_0223EF6C:
	bl ov66_2234FC4
	mov r1, r0
	ldr r0, [r7, #4]
	ldr r1, [r1, #0x30]
	blx r1
	strb fp, [r6]
	and r2, fp, #0xff
	strb r2, [r5]
	ldr r2, [r5]
	add r0, r4, #0x38
	mov r1, #1
	bl ov66_223F724
	ldr r0, [r4, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr sb, [r4, #0x38]
	ldr r0, [r4, #0x44]
	ldr r1, [r4, #0x4c]
	add r0, r0, sb, lsr #7
	bl _u32_div_f
	ldr r0, [r4, #0x40]
	mov r2, sb, lsl #0x19
	ldr r1, [r0, r1, lsl #2]
	ldr r0, [r1, r2, lsr #22]
	add r1, r1, r2, lsr #22
	str r0, [r7]
	ldr r0, [r1, #4]
	str r0, [r7, #4]
	ldr r1, [r4, #0x10]
	ldr r3, [r7]
	ldr r0, [r4, #0x14]
	subs r2, r8, r1
	sbc r1, sl, r0
	subs r0, r2, r3
	sbcs r0, r1, r3, asr #31
	bge _0223EF6C
_0223F000:
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0223F008: .word 0x02258C8C
	arm_func_end ov66_223EE30

	arm_func_start ov66_223F00C
ov66_223F00C: // 0x0223F00C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #5
	beq _0223F034
	bl ov66_2234FC4
	ldr r0, [r0]
	cmp r0, #8
	beq _0223F040
_0223F034:
	add sp, sp, #0x1c
	mov r0, #0
	ldmia sp!, {r3, r4, pc}
_0223F040:
	ldr r2, [r4, #0x10]
	ldr r1, [r4, #0x14]
	mov r0, #0
	cmp r1, r0
	cmpeq r2, r0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r0, [r4, #0x18]
	cmp r0, #0
	addne sp, sp, #0x1c
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	bl ov66_2234FC4
	add r0, r0, #0xe8
	bl ov66_2241ED0
	cmp r0, #0
	addne sp, sp, #0x1c
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	add r0, sp, #0
	add r1, sp, #0x10
	bl DWC_GetDateTime
	add r0, sp, #0
	add r1, sp, #0x10
	bl RTC_ConvertDateTimeToSecond
	ldr ip, [r4, #8]
	ldr r2, [r4, #0x1c]
	ldr r3, [r4, #0xc]
	adds r4, ip, r2
	adc r2, r3, #0
	subs r0, r4, r0
	sbcs r0, r2, r1
	movlt r0, #1
	movge r0, #0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223F00C

	arm_func_start ov66_223F0D0
ov66_223F0D0: // 0x0223F0D0
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #5
	beq _0223F144
	bl ov66_2234FC4
	ldr r0, [r0]
	cmp r0, #8
	bne _0223F144
	add r0, sp, #0
	add r1, sp, #0x10
	bl DWC_GetDateTime
	add r0, sp, #0
	add r1, sp, #0x10
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x10]
	str r1, [r4, #0x14]
	mov r0, #5
	str r0, [r4]
	mov r0, #0
	str r0, [r4, #0x18]
	bl ov66_2234FC4
	ldr r1, [r0, #0x30]
	mov r0, #0
	blx r1
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r3, r4, pc}
_0223F144:
	mov r0, #1
	str r0, [r4, #0x18]
	mov r0, #0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_223F0D0

	arm_func_start ov66_223F158
ov66_223F158: // 0x0223F158
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	mov r5, r1
	ldr r1, [r5]
	mov r4, r2
	mov r6, r0
	add r4, r4, r1
	ldr r3, [r5, #8]
	ldr r1, [r5, #0xc]
	ldr r0, [r5, #0x14]
	add r2, r3, r1, lsl #2
	add r1, r3, r0, lsl #2
	str r1, [sp, #0xc]
	add r0, sp, #0
	str r2, [sp]
	str r3, [sp, #4]
	str r2, [sp, #8]
	mov r1, r4, lsr #7
	bl ov66_2240C80
	ldr r0, [r5, #0x10]
	ldr r1, [sp]
	cmp r0, #0
	moveq r5, #0
	ldr r0, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #0xc]
	ldrne r5, [r1]
	str r1, [r6]
	stmib r6, {r0, r2}
	mov r0, r4, lsl #0x19
	str r3, [r6, #0xc]
	add r0, r5, r0, lsr #22
	str r0, [r6, #0x10]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_223F158

	arm_func_start ov66_223F1E4
ov66_223F1E4: // 0x0223F1E4
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xbc
	mov r5, r0
	tst r1, #0x7f
	movne r0, #1
	moveq r0, #0
	mov r8, #0
	add r4, r0, r1, lsr #7
	mov r7, r8
	add r6, sp, #0x28
	b _0223F280
_0223F210:
	ldr r1, [r5, #8]
	ldr r0, [r5, #0xc]
	ldr sb, [r1, r0, lsl #2]
	strb r8, [sp, #0x13]
	ldr r0, [r5, #0xc]
	add r1, r0, #1
	str r1, [r5, #0xc]
	ldr r0, [r5, #0x14]
	cmp r1, r0
	streq r8, [r5, #0xc]
	ldr r0, [r5, #0x10]
	sub r1, r0, #1
	str r1, [r5, #0x10]
	ldr r0, [r5, #0xc]
	add r0, r0, r1
	ldr r1, [r5, #0x14]
	sub r0, r0, #1
	bl _u32_div_f
	ldr r2, [r5, #8]
	add r0, r5, #8
	str sb, [r2, r1, lsl #2]
	mov r1, r6
	str r7, [sp, #0x28]
	bl ov66_223F944
	ldr r0, [r5]
	sub r4, r4, #1
	sub r0, r0, #0x80
	str r0, [r5]
_0223F280:
	cmp r4, #0
	beq _0223F294
	ldr r0, [r5]
	cmp r0, #0x80
	bhs _0223F210
_0223F294:
	cmp r4, #0
	addeq sp, sp, #0xbc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, _0223F660 // =0x0225B6C8
	ldr r2, [r1]
	tst r2, #1
	bne _0223F2C4
	ldr r0, _0223F664 // =0x022590A8
	orr r2, r2, #1
	mvn r3, #0xff
	str r3, [r0]
	str r2, [r1]
_0223F2C4:
	ldr r1, _0223F664 // =0x022590A8
	mov r6, r4, lsl #7
	ldr r0, [r1]
	cmp r0, r4, lsl #7
	blo _0223F320
	ldr r0, _0223F660 // =0x0225B6C8
	ldr r2, [r0]
	tst r2, #1
	bne _0223F2F8
	orr r2, r2, #1
	mvn r3, #0xff
	str r3, [r1]
	str r2, [r0]
_0223F2F8:
	ldr r1, [r5, #0x10]
	ldr r0, _0223F664 // =0x022590A8
	cmp r1, #0
	ldr r2, [r0]
	moveq r1, #0
	subne r0, r1, #1
	movne r1, r0, lsl #7
	sub r0, r2, r6
	cmp r1, r0
	bls _0223F328
_0223F320:
	ldr r0, _0223F668 // =0x022590AC
	bl ov66_223F7D8
_0223F328:
	ldr r0, [r5, #0x14]
	cmp r0, #0
	moveq r1, #0
	subne r1, r0, #1
	ldr r0, [r5, #0x10]
	add r0, r4, r0
	cmp r0, r1
	bhi _0223F3AC
	cmp r4, #0
	addeq sp, sp, #0xbc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	mov r7, #0x400
	mov sb, #0
	add r8, sp, #0x24
_0223F360:
	mov r0, r7
	bl FUN_02246290
	ldr r3, [r5, #0xc]
	ldr r2, [r5, #0x10]
	ldr r1, [r5, #0x14]
	add r2, r3, r2
	mov r6, r0
	sub r0, r2, #1
	bl _u32_div_f
	ldr r2, [r5, #8]
	add r0, r5, #8
	str r6, [r2, r1, lsl #2]
	mov r1, r8
	str sb, [sp, #0x24]
	bl ov66_223F944
	subs r4, r4, #1
	bne _0223F360
	add sp, sp, #0xbc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_0223F3AC:
	movs r1, #0
	add r6, sp, #0xac
	str r1, [sp, #0xa4]
	str r1, [sp, #0xa8]
	str r1, [sp, #0x74]
	str r1, [sp, #0x78]
	str r1, [sp, #0x7c]
	str r1, [sp, #0x80]
	str r1, [r6]
	str r1, [r6, #4]
	str r1, [r6, #8]
	str r1, [r6, #0xc]
	add r8, sp, #0x74
	beq _0223F3EC
	add r0, r1, r1
	bl _u32_div_f
_0223F3EC:
	ldr r7, [r8]
	ldr r2, [r8, #4]
	ldr r0, [r8, #0xc]
	add r3, r7, r2, lsl #2
	add r2, r7, r0, lsl #2
	add r8, r7, r1, lsl #2
	ldrb r1, [sp, #0x12]
	add r0, sp, #0x84
	str r8, [sp, #0x84]
	str r7, [sp, #0x88]
	str r7, [sp, #0x98]
	str r3, [sp, #0x8c]
	str r2, [sp, #0x90]
	str r0, [sp, #4]
	add r0, sp, #0x94
	strb r1, [sp, #8]
	sub r7, sp, #0xc
	str r3, [sp, #0x94]
	str r3, [sp, #0x9c]
	str r2, [sp, #0xa0]
	ldmia r0, {r0, r1, r2, r3}
	stmia r7, {r0, r1, r2, r3}
	mov r0, r6
	ldmia r7, {r1, r2, r3}
	bl ov66_2240D98
	add r0, sp, #0x74
	bl ov66_223C41C
	ldr r0, [r5, #0x10]
	add r8, sp, #0xac
	add r0, r4, r0
	add r0, r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [r5, #0x14]
	cmp r0, #0
	moveq r2, #0
	subne r2, r0, #1
	ldr r0, [sp, #0x1c]
	mov r1, r2, lsl #1
	cmp r0, r2, lsl #1
	addhi r0, sp, #0x1c
	addls r0, sp, #0x20
	str r1, [sp, #0x20]
	ldr r1, [r0]
	mov r0, r8
	bl ov66_223F7E8
	mov r2, #0
	add r1, sp, #0x18
	mov r0, r8
	str r2, [sp, #0x18]
	bl ov66_223F944
	cmp r4, #0
	beq _0223F50C
	mov r7, #0x400
	mov sl, #0
	add sb, sp, #0x14
_0223F4C8:
	mov r0, r7
	bl FUN_02246290
	ldr r3, [r8, #4]
	ldr r2, [r8, #8]
	ldr r1, [r8, #0xc]
	add r2, r3, r2
	mov r6, r0
	sub r0, r2, #1
	bl _u32_div_f
	ldr r2, [r8]
	mov r0, r8
	str r6, [r2, r1, lsl #2]
	mov r1, sb
	str sl, [sp, #0x14]
	bl ov66_223F944
	subs r4, r4, #1
	bne _0223F4C8
_0223F50C:
	ldr r0, [r5, #0x10]
	cmp r0, #0
	beq _0223F5FC
	ldr r1, [r5, #0x14]
	cmp r1, #0
	moveq r1, #0
	beq _0223F538
	ldr r2, [r5, #0xc]
	ldr r0, [r5, #0x10]
	add r0, r2, r0
	bl _u32_div_f
_0223F538:
	ldr r4, [r5, #8]
	ldr r2, [r5, #0xc]
	ldr r0, [r5, #0x14]
	add r3, r4, r2, lsl #2
	add r2, r4, r0, lsl #2
	add r1, r4, r1, lsl #2
	str r1, [sp, #0x64]
	add r0, sp, #0x64
	str r4, [sp, #0x68]
	str r3, [sp, #0x6c]
	str r2, [sp, #0x70]
	mvn r1, #0
	bl ov66_2240C80
	ldr r0, [r5, #8]
	ldr r2, [r5, #0xc]
	ldr r1, [r5, #0x14]
	add sb, r0, r2, lsl #2
	mov r6, #0
	ldr r7, [r8]
	ldr r2, [r8, #0xc]
	ldr r3, [r8, #4]
	add r2, r7, r2, lsl #2
	add r4, r7, r3, lsl #2
	ldr r3, [sp, #0x64]
	str r2, [sp, #0x38]
	ldr r2, [sp, #0x68]
	str r3, [sp, #0x50]
	ldr r3, [sp, #0x6c]
	str r2, [sp, #0x54]
	ldr r2, [sp, #0x70]
	str r7, [sp, #0x30]
	add r7, r0, r1, lsl #2
	str r3, [sp, #0x58]
	str r2, [sp, #0x5c]
	str r0, [sp, #0x40]
	add r1, sp, #0x2c
	add r2, sp, #0x3c
	add r3, sp, #0x50
	str r7, [sp, #0x48]
	strb r6, [sp, #0xe]
	strb r6, [sp, #0xf]
	strb r6, [sp, #0xd]
	mov r0, r8
	str sb, [sp, #0x3c]
	str sb, [sp, #0x44]
	str r4, [sp, #0x2c]
	str r4, [sp, #0x34]
	strb r6, [sp]
	bl ov66_223FEA4
_0223F5FC:
	ldr r2, [r5, #8]
	ldr r1, [r8]
	add r0, sp, #0xa4
	str r1, [r5, #8]
	str r2, [r8]
	ldr r3, [r5, #0xc]
	ldr r1, [r8, #4]
	mov r2, #0
	str r1, [r5, #0xc]
	str r3, [r8, #4]
	ldr r3, [r5, #0x10]
	ldr r1, [r8, #8]
	str r1, [r5, #0x10]
	str r3, [r8, #8]
	ldr r3, [r5, #0x14]
	ldr r1, [r8, #0xc]
	str r1, [r5, #0x14]
	ldr r1, [r8, #8]
	str r3, [r8, #0xc]
	sub r1, r1, r1
	strb r2, [sp, #0xc]
	str r1, [r8, #8]
	bl ov66_224091C
	add sp, sp, #0xbc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_0223F660: .word 0x0225B6C8
_0223F664: .word 0x022590A8
_0223F668: .word 0x022590AC
	arm_func_end ov66_223F1E4

	arm_func_start ov66_223F66C
ov66_223F66C: // 0x0223F66C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #4
	cmp r1, #0
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r0, [r6, #4]
	sub r0, r0, r1
	str r0, [r6, #4]
	ldr r0, [r6, #0x10]
	cmp r0, #0
	moveq r2, #0
	subne r0, r0, #1
	movne r2, r0, lsl #7
	ldmia r6, {r0, r1}
	add r0, r0, r1
	sub r5, r2, r0
	cmp r5, #0x100
	addlo sp, sp, #4
	ldmloia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	mov r8, #0
	mov r7, r8
	mov r4, r8
_0223F6C8:
	strb r4, [sp]
	ldr r0, [r6, #0x10]
	sub r1, r0, #1
	str r1, [r6, #0x10]
	ldr r0, [r6, #0xc]
	add r0, r0, r1
	ldr r1, [r6, #0x14]
	sub r0, r0, #1
	bl _u32_div_f
	ldr sb, [r6, #8]
	mov sl, r1
	ldr r1, [sb, sl, lsl #2]
	cmp r1, #0
	beq _0223F70C
	mov r0, r8
	mov r2, r8
	bl DWC_Free
_0223F70C:
	sub r5, r5, #0x80
	str r7, [sb, sl, lsl #2]
	cmp r5, #0x100
	bhs _0223F6C8
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end ov66_223F66C

	arm_func_start ov66_223F724
ov66_223F724: // 0x0223F724
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	cmp r1, #0
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r0, [r6, #4]
	sub r0, r0, r1
	str r0, [r6, #4]
	ldr r0, [r6]
	add r0, r0, r1
	str r0, [r6]
	cmp r0, #0x100
	blo _0223F7C0
	mov r5, #0
	mov r4, r5
_0223F764:
	ldr r1, [r6, #8]
	ldr r0, [r6, #0xc]
	ldr r1, [r1, r0, lsl #2]
	cmp r1, #0
	beq _0223F784
	mov r0, r5
	mov r2, r5
	bl DWC_Free
_0223F784:
	strb r4, [sp]
	ldr r0, [r6, #0xc]
	add r1, r0, #1
	str r1, [r6, #0xc]
	ldr r0, [r6, #0x14]
	cmp r1, r0
	streq r4, [r6, #0xc]
	ldr r0, [r6, #0x10]
	sub r0, r0, #1
	str r0, [r6, #0x10]
	ldr r0, [r6]
	sub r0, r0, #0x80
	str r0, [r6]
	cmp r0, #0x100
	bhs _0223F764
_0223F7C0:
	ldr r0, [r6, #4]
	cmp r0, #0
	moveq r0, #0x40
	streq r0, [r6]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_223F724

	arm_func_start ov66_223F7D8
ov66_223F7D8: // 0x0223F7D8
	stmfd sp!, {r3, lr}
	bl printf
	bl abort
	ldmia sp!, {r3, pc}
	arm_func_end ov66_223F7D8

	arm_func_start ov66_223F7E8
ov66_223F7E8: // 0x0223F7E8
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	mov r4, r0
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	subne r0, r0, #1
	cmp r1, r0
	addls sp, sp, #0x38
	ldmlsia sp!, {r3, r4, r5, pc}
	mov r2, #0
	add r0, r4, #0xc
	str r0, [sp, #0x34]
	str r2, [sp, #0x24]
	str r2, [sp, #0x28]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	strb r2, [sp, #2]
	sub r0, sp, #4
	strb r2, [r0]
	ldr r2, [r0]
	add r0, sp, #0x24
	bl ov66_2240A0C
	ldr r1, [r4, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223F864
	ldr r2, [r4, #4]
	ldr r0, [r4, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223F864:
	ldr r0, [r4]
	mov ip, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #4]
	ldr r2, [r4]
	sub r3, sp, #4
	str r2, [sp, #8]
	ldr r0, [r4, #4]
	add r0, r2, r0, lsl #2
	str r0, [sp, #0xc]
	ldr r1, [r4, #0xc]
	add r0, sp, #0x24
	add r1, r2, r1, lsl #2
	str r1, [sp, #0x10]
	ldr r5, [r4]
	ldr r2, [r4, #4]
	add r1, sp, #0x14
	add r2, r5, r2, lsl #2
	str r2, [sp, #0x14]
	ldr r5, [r4]
	add r2, sp, #4
	str r5, [sp, #0x18]
	ldr lr, [r4, #4]
	add lr, r5, lr, lsl #2
	str lr, [sp, #0x1c]
	ldr lr, [r4, #0xc]
	add lr, r5, lr, lsl #2
	str lr, [sp, #0x20]
	strb ip, [sp, #1]
	strb ip, [r3]
	ldr r3, [r3]
	bl ov66_2240B80
	mov r0, #0
	strb r0, [sp]
	str r0, [r4, #8]
	ldr r1, [r4]
	ldr r0, [sp, #0x24]
	add r3, sp, #0x30
	str r0, [r4]
	str r1, [sp, #0x24]
	ldr r2, [r4, #4]
	ldr r1, [sp, #0x28]
	add r0, sp, #0x24
	str r1, [r4, #4]
	str r2, [sp, #0x28]
	ldr r2, [r4, #8]
	ldr r1, [sp, #0x2c]
	str r1, [r4, #8]
	str r2, [sp, #0x2c]
	ldr r2, [r4, #0xc]
	ldr r1, [r3]
	str r1, [r4, #0xc]
	str r2, [r3]
	bl ov66_22409BC
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223F7E8

	arm_func_start ov66_223F944
ov66_223F944: // 0x0223F944
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	mov r5, r0
	ldr r3, [r5, #8]
	mov r4, r1
	ldr r1, [r5, #0xc]
	add r2, r3, #1
	cmp r2, r1
	bhs _0223F998
	ldr r0, [r5, #4]
	add r0, r0, r3
	bl _u32_div_f
	ldr r0, [r5]
	add sp, sp, #0x38
	adds r1, r0, r1, lsl #2
	ldrne r0, [r4]
	strne r0, [r1]
	ldr r0, [r5, #8]
	add r0, r0, #1
	str r0, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
_0223F998:
	mov r3, #0
	add r2, r5, #0xc
	mov r1, #1
	str r3, [sp, #0x24]
	str r3, [sp, #0x28]
	str r3, [sp, #0x2c]
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	bl ov66_223FB04
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #2]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #0x24
	strb r3, [sp, #3]
	bl ov66_2240A0C
	ldr r1, [r5, #8]
	ldr r0, [sp, #0x24]
	str r1, [sp, #0x28]
	adds r1, r0, r1, lsl #2
	ldrne r0, [r4]
	strne r0, [r1]
	ldr r0, [sp, #0x2c]
	add r0, r0, #1
	str r0, [sp, #0x2c]
	ldr r1, [r5, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223FA24
	ldr r2, [r5, #4]
	ldr r0, [r5, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223FA24:
	ldr r0, [r5]
	mov r4, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #4]
	ldr r2, [r5]
	sub r3, sp, #4
	str r2, [sp, #8]
	ldr r0, [r5, #4]
	add r0, r2, r0, lsl #2
	str r0, [sp, #0xc]
	ldr r1, [r5, #0xc]
	add r0, sp, #0x24
	add r1, r2, r1, lsl #2
	str r1, [sp, #0x10]
	ldr ip, [r5]
	ldr r2, [r5, #4]
	add r1, sp, #0x14
	add r2, ip, r2, lsl #2
	str r2, [sp, #0x14]
	ldr lr, [r5]
	add r2, sp, #4
	str lr, [sp, #0x18]
	ldr ip, [r5, #4]
	add ip, lr, ip, lsl #2
	str ip, [sp, #0x1c]
	ldr ip, [r5, #0xc]
	add ip, lr, ip, lsl #2
	str ip, [sp, #0x20]
	strb r4, [sp, #1]
	strb r4, [r3]
	ldr r3, [r3]
	bl ov66_2240A74
	mov r0, r4
	strb r0, [sp]
	str r0, [r5, #8]
	ldr r1, [r5]
	ldr r0, [sp, #0x24]
	add r3, sp, #0x30
	str r0, [r5]
	str r1, [sp, #0x24]
	ldr r2, [r5, #4]
	ldr r1, [sp, #0x28]
	add r0, sp, #0x24
	str r1, [r5, #4]
	str r2, [sp, #0x28]
	ldr r2, [r5, #8]
	ldr r1, [sp, #0x2c]
	str r1, [r5, #8]
	str r2, [sp, #0x2c]
	ldr r2, [r5, #0xc]
	ldr r1, [r3]
	str r1, [r5, #0xc]
	str r2, [r3]
	bl ov66_22409BC
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223F944

	arm_func_start ov66_223FB04
ov66_223FB04: // 0x0223FB04
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	ldr r2, _0223FC04 // =0x0225B6D0
	ldr r3, [r2]
	tst r3, #1
	bne _0223FB34
	ldr r1, _0223FC08 // =0x022590A4
	orr r3, r3, #1
	mvn r4, #0xc0000001
	str r4, [r1]
	str r3, [r2]
_0223FB34:
	ldr r1, [r0, #0xc]
	ldr r0, _0223FC08 // =0x022590A4
	cmp r1, #0
	moveq r5, #0
	subne r5, r1, #1
	ldr r4, [r0]
	ldr r1, [sp, #0x1c]
	sub r0, r4, r5
	cmp r1, r0
	bls _0223FB60
	bl ov66_223FC14
_0223FB60:
	ldr r0, _0223FC0C // =0xAAAAAAAB
	umull r0, r1, r4, r0
	mov r1, r1, lsr #1
	cmp r5, r1
	bhs _0223FBB4
	add r1, r5, #1
	ldr r0, _0223FC10 // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x1c]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x1c
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r5, r0
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223FBB4:
	cmp r5, r1, lsl #1
	bhs _0223FBF0
	ldr r0, [sp, #0x1c]
	add r1, r5, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x1c
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r5, r0
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223FBF0:
	mov r0, r4
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223FC04: .word 0x0225B6D0
_0223FC08: .word 0x022590A4
_0223FC0C: .word 0xAAAAAAAB
_0223FC10: .word 0xCCCCCCCD
	arm_func_end ov66_223FB04

	arm_func_start ov66_223FC14
ov66_223FC14: // 0x0223FC14
	stmfd sp!, {r3, lr}
	ldr r0, _0223FC28 // =0x022590C4
	bl printf
	bl abort
	ldmia sp!, {r3, pc}
	.align 2, 0
_0223FC28: .word 0x022590C4
	arm_func_end ov66_223FC14

	arm_func_start ov66_223FC2C
ov66_223FC2C: // 0x0223FC2C
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0223FC74
	beq _0223FC68
	mov r2, r0
	bl DWC_Free
_0223FC68:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0223FC74:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov66_2240D38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223FC2C

	arm_func_start ov66_223FC9C
ov66_223FC9C: // 0x0223FC9C
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0xe0000000
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223FCC4
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223FCC4:
	ldr r0, _0223FD60 // =0x0AAAAAAA
	cmp r4, r0
	bhs _0223FD10
	add r1, r4, #1
	ldr r0, _0223FD64 // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223FD10:
	cmp r4, r0, lsl #1
	bhs _0223FD4C
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223FD4C:
	mvn r0, #0xe0000000
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223FD60: .word 0x0AAAAAAA
_0223FD64: .word 0xCCCCCCCD
	arm_func_end ov66_223FC9C

	arm_func_start ov66_223FD68
ov66_223FD68: // 0x0223FD68
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0223FDB0
	beq _0223FDA4
	mov r2, r0
	bl DWC_Free
_0223FDA4:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0223FDB0:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov66_2240D68
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_223FD68

	arm_func_start ov66_223FDD8
ov66_223FDD8: // 0x0223FDD8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0xe0000000
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223FE00
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223FE00:
	ldr r0, _0223FE9C // =0x0AAAAAAA
	cmp r4, r0
	bhs _0223FE4C
	add r1, r4, #1
	ldr r0, _0223FEA0 // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223FE4C:
	cmp r4, r0, lsl #1
	bhs _0223FE88
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223FE88:
	mvn r0, #0xe0000000
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_0223FE9C: .word 0x0AAAAAAA
_0223FEA0: .word 0xCCCCCCCD
	arm_func_end ov66_223FDD8

	arm_func_start ov66_223FEA4
ov66_223FEA4: // 0x0223FEA4
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1a4
	mov sb, r2
	mov r8, r3
	ldrb r6, [r8, #0x10]
	ldrb r2, [sb, #0x10]
	str r1, [sp]
	mov sl, r0
	ldr ip, [r8, #4]
	ldr fp, [r8, #8]
	ldr r7, [r8, #0xc]
	ldr r5, [sb]
	ldr r4, [sb, #4]
	ldr r3, [sb, #8]
	ldr lr, [sb, #0xc]
	ldr r1, [r8]
	add r0, sp, #0x5c
	str r1, [sp, #0x5c]
	add r1, sp, #0x48
	strb r6, [sp, #0x168]
	strb r2, [sp, #0x17c]
	str ip, [sp, #0x60]
	str fp, [sp, #0x64]
	str r7, [sp, #0x68]
	strb r6, [sp, #0x6c]
	str r5, [sp, #0x48]
	str r4, [sp, #0x4c]
	str r3, [sp, #0x50]
	str lr, [sp, #0x54]
	strb r2, [sp, #0x58]
	bl ov66_2240CC4
	movs r4, r0
	addeq sp, sp, #0x1a4
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp]
	ldr r3, [sl]
	ldr r1, [sl, #4]
	ldr r2, [r0]
	add r7, r3, r1, lsl #2
	sub r1, r2, r7
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	cmp r2, r7
	mov r5, r0, asr #2
	ldrlo r0, [sl, #0xc]
	addlo r5, r5, r0
	ldr r0, [sl, #0xc]
	cmp r0, #0
	moveq r0, #0
	subne r0, r0, #1
	cmp r4, r0
	ldrls r1, [sl, #8]
	subls r0, r0, r4
	cmpls r1, r0
	bhi _0224026C
	str r7, [sp, #0x194]
	ldr r6, [sl]
	str r6, [sp, #0x198]
	ldr r0, [sl, #4]
	add r0, r6, r0, lsl #2
	str r0, [sp, #4]
	str r0, [sp, #0x19c]
	ldr r0, [sl, #0xc]
	add fp, r6, r0, lsl #2
	str fp, [sp, #0x1a0]
	ldr r1, [sl, #8]
	sub r0, r1, r5
	str r0, [sp, #8]
	cmp r5, r0
	bhs _022400A4
	add r0, sp, #0x194
	rsb r1, r4, #0
	bl ov66_2240C80
	ldr r1, [sp, #0x194]
	ldr r0, [sl]
	cmp r5, #0
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r0, r4
	str r0, [sl, #8]
	beq _022401E0
	ldr r2, [sp, #0x1a0]
	ldr sl, [sp, #0x194]
	ldr r4, [sp, #0x198]
	ldr r3, [sp, #0x19c]
	str r2, [sp, #0x124]
	ldr r2, [sp, #4]
	add r0, sp, #0x38
	mov r1, r5
	str sl, [sp, #0x118]
	str r4, [sp, #0x11c]
	str r3, [sp, #0x120]
	str r7, [sp, #0x38]
	str r6, [sp, #0x3c]
	str r2, [sp, #0x40]
	str fp, [sp, #0x44]
	bl ov66_2240C80
	ldr r2, [r0]
	add r1, sp, #0x138
	str r2, [sp, #0x128]
	ldr r3, [r0, #4]
	add r2, sp, #0x128
	str r3, [sp, #0x12c]
	ldr r4, [r0, #8]
	add r3, sp, #0x118
	str r4, [sp, #0x130]
	ldr r4, [r0, #0xc]
	add r0, sp, #0x148
	str r4, [sp, #0x134]
	ldr r4, [sp, #4]
	str r7, [sp, #0x138]
	str r6, [sp, #0x13c]
	str r4, [sp, #0x140]
	str fp, [sp, #0x144]
	bl ov66_22407A8
	ldr r3, [sp, #0x148]
	ldr r2, [sp, #0x14c]
	ldr r1, [sp, #0x150]
	ldr r0, [sp, #0x154]
	str r3, [sp, #0x194]
	str r2, [sp, #0x198]
	str r1, [sp, #0x19c]
	str r0, [sp, #0x1a0]
	b _022401E0
_022400A4:
	add r0, r1, r4
	str r0, [sl, #8]
	ldr r1, [sl, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _022400CC
	ldr r2, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r2, r0
	bl _u32_div_f
_022400CC:
	ldr r5, [sl]
	ldr r2, [sl, #4]
	ldr r0, [sl, #0xc]
	add r3, r5, r2, lsl #2
	add r2, r5, r0, lsl #2
	ldr r0, [sp, #8]
	add r1, r5, r1, lsl #2
	str r1, [sp, #0x194]
	str r5, [sp, #0x198]
	str r3, [sp, #0x19c]
	str r2, [sp, #0x1a0]
	cmp r0, #0
	beq _022401D4
	add r0, sp, #0x18
	str r1, [sp, #0xd8]
	str r1, [sp, #0x18]
	str r5, [sp, #0xdc]
	str r3, [sp, #0xe0]
	str r2, [sp, #0xe4]
	str r5, [sp, #0x1c]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	rsb r1, r4, #0
	bl ov66_2240C80
	ldr r2, [r0]
	ldr r1, [sp, #8]
	str r2, [sp, #0xe8]
	ldr r2, [r0, #4]
	add r1, r4, r1
	str r2, [sp, #0xec]
	ldr r2, [r0, #8]
	ldr r6, [sp, #0x194]
	str r2, [sp, #0xf0]
	ldr r7, [r0, #0xc]
	ldr r5, [sp, #0x198]
	ldr r3, [sp, #0x19c]
	ldr r2, [sp, #0x1a0]
	add r0, sp, #0x28
	str r7, [sp, #0xf4]
	str r6, [sp, #0x28]
	str r5, [sp, #0x2c]
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	rsb r1, r1, #0
	bl ov66_2240C80
	ldr r2, [r0]
	add r1, sp, #0xf8
	str r2, [sp, #0xf8]
	ldr r3, [r0, #4]
	add r2, sp, #0xe8
	str r3, [sp, #0xfc]
	ldr r5, [r0, #8]
	add r3, sp, #0xd8
	str r5, [sp, #0x100]
	ldr r5, [r0, #0xc]
	add r0, sp, #0x108
	str r5, [sp, #0x104]
	bl ov66_2240614
	ldr r0, [sp, #0x108]
	ldr r2, [sp, #0x10c]
	ldr r1, [sp, #0x110]
	str r0, [sp, #0x194]
	ldr r0, [sp, #0x114]
	str r2, [sp, #0x198]
	str r1, [sp, #0x19c]
	str r0, [sp, #0x1a0]
_022401D4:
	add r0, sp, #0x194
	rsb r1, r4, #0
	bl ov66_2240C80
_022401E0:
	ldr r3, [sp, #0x194]
	ldr r2, [sp, #0x198]
	ldr r0, [sp, #0x1a0]
	ldr r1, [sp, #0x19c]
	str r0, [sp, #0x9c]
	str r3, [sp, #0x90]
	str r2, [sp, #0x94]
	str r1, [sp, #0x98]
	ldr r1, [r8]
	add r0, sp, #0xc8
	str r1, [sp, #0xa0]
	ldr r2, [r8, #4]
	add r1, sp, #0xb4
	str r2, [sp, #0xa4]
	ldr r3, [r8, #8]
	add r2, sp, #0xa0
	str r3, [sp, #0xa8]
	ldr r4, [r8, #0xc]
	add r3, sp, #0x90
	str r4, [sp, #0xac]
	ldrb r4, [r8, #0x10]
	strb r4, [sp, #0xb0]
	ldr r4, [sb]
	str r4, [sp, #0xb4]
	ldr r4, [sb, #4]
	str r4, [sp, #0xb8]
	ldr r4, [sb, #8]
	str r4, [sp, #0xbc]
	ldr r4, [sb, #0xc]
	str r4, [sp, #0xc0]
	ldrb r4, [sb, #0x10]
	strb r4, [sp, #0xc4]
	bl ov66_2240444
	add sp, sp, #0x1a4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224026C:
	mov r3, #0
	add r2, sl, #0xc
	mov r0, sl
	mov r1, r4
	str r3, [sp, #0x180]
	str r3, [sp, #0x184]
	str r3, [sp, #0x188]
	str r3, [sp, #0x18c]
	str r2, [sp, #0x190]
	bl ov66_223FB04
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #0x10]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #0x180
	strb r3, [sp, #0x11]
	bl ov66_2240A0C
	ldr r0, [sp, #0x180]
	str r5, [sp, #0x184]
	cmp r4, #0
	add r5, r0, r5, lsl #2
	beq _02240320
	mov r3, #0
_022402D0:
	ldr r0, [sb]
	cmp r5, #0
	strb r3, [sp, #0xf]
	ldr r2, [r0]
	sub r4, r4, #1
	strne r2, [r5]
	ldr r0, [sp, #0x188]
	add r5, r5, #4
	add r0, r0, #1
	str r0, [sp, #0x188]
	ldr r0, [sb]
	add r1, r0, #4
	str r1, [sb]
	ldr r0, [sb, #0xc]
	cmp r1, r0
	ldreq r0, [sb, #4]
	streq r0, [sb]
	cmp r4, #0
	bne _022402D0
	str r2, [sp, #0x14]
_02240320:
	ldr r1, [sl, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _02240340
	ldr r2, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r2, r0
	bl _u32_div_f
_02240340:
	ldr r0, [sl]
	mov r2, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #0x80]
	ldr r4, [sl]
	sub r1, sp, #4
	str r4, [sp, #0x84]
	ldr r0, [sl, #4]
	add r0, r4, r0, lsl #2
	str r0, [sp, #0x88]
	ldr r3, [sl, #0xc]
	add r0, sp, #0x180
	add r3, r4, r3, lsl #2
	str r3, [sp, #0x8c]
	strb r2, [sp, #0xe]
	strb r2, [r1]
	ldr r3, [r1]
	ldr r1, [sp]
	add r2, sp, #0x80
	bl ov66_2240B80
	ldr r1, [sl]
	ldr r0, [sl, #4]
	mov r2, #0
	add r0, r1, r0, lsl #2
	str r0, [sp, #0x70]
	ldr r4, [sl]
	sub r1, sp, #4
	str r4, [sp, #0x74]
	ldr r0, [sl, #4]
	add r0, r4, r0, lsl #2
	str r0, [sp, #0x78]
	ldr r3, [sl, #0xc]
	add r0, sp, #0x180
	add r3, r4, r3, lsl #2
	str r3, [sp, #0x7c]
	strb r2, [sp, #0xd]
	strb r2, [r1]
	ldr r3, [r1]
	ldr r2, [sp]
	add r1, sp, #0x70
	bl ov66_2240A74
	mov r0, #0
	strb r0, [sp, #0xc]
	str r0, [sl, #8]
	ldr r1, [sl]
	ldr r0, [sp, #0x180]
	add r3, sp, #0x18c
	str r0, [sl]
	str r1, [sp, #0x180]
	ldr r2, [sl, #4]
	ldr r1, [sp, #0x184]
	add r0, sp, #0x180
	str r1, [sl, #4]
	str r2, [sp, #0x184]
	ldr r2, [sl, #8]
	ldr r1, [sp, #0x188]
	str r1, [sl, #8]
	str r2, [sp, #0x188]
	ldr r2, [sl, #0xc]
	ldr r1, [r3]
	str r1, [sl, #0xc]
	str r2, [r3]
	bl ov66_22409BC
	add sp, sp, #0x1a4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_223FEA4

	arm_func_start ov66_2240444
ov66_2240444: // 0x02240444
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x60
	mov r5, r1
	ldr r8, [r5]
	ldr r7, [r2]
	mov r6, r0
	mov r4, r3
	cmp r8, r7
	beq _022405EC
	mov r0, r2
	bl ov66_2240CC4
	str r0, [sp, #0xc]
_02240474:
	ldr r2, [r4, #0xc]
	ldr r0, [r4]
	ldr r1, [sp, #0xc]
	sub r2, r2, r0
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	addle r0, sp, #0xc
	str r2, [sp, #8]
	addgt r0, sp, #8
	ldr r7, [r0]
	ldr r1, [r5]
	add r0, sp, #0x10
	str r1, [sp, #0x10]
	ldr r2, [r5, #4]
	mov r1, r7
	str r2, [sp, #0x14]
	ldr r2, [r5, #8]
	str r2, [sp, #0x18]
	ldr r2, [r5, #0xc]
	str r2, [sp, #0x1c]
	ldrb r2, [r5, #0x10]
	strb r2, [sp, #0x20]
	bl ov66_2240C80
	ldrb r2, [sp, #0x20]
	ldr r0, [sp, #0x10]
	ldr ip, [r4]
	ldr sb, [sp, #0x14]
	ldr r8, [sp, #0x18]
	ldr r3, [sp, #0x1c]
	str r0, [sp, #0x24]
	str sb, [sp, #0x28]
	str r8, [sp, #0x2c]
	str r3, [sp, #0x30]
	strb r2, [sp, #0x34]
	ldr lr, [r5]
	str r3, [sp, #0x58]
	str lr, [sp, #0x38]
	ldr r1, [r5, #4]
	str r0, [sp, #0x4c]
	str r1, [sp, #0x3c]
	ldr r1, [r5, #8]
	str sb, [sp, #0x50]
	str r1, [sp, #0x40]
	ldr r1, [r5, #0xc]
	str r8, [sp, #0x54]
	str r1, [sp, #0x44]
	ldrb r1, [r5, #0x10]
	strb r2, [sp, #0x5c]
	strb r2, [sp]
	strb r1, [sp, #0x48]
	cmp lr, r0
	add r3, sp, #0x24
	add r2, sp, #0x38
	beq _02240598
	mov r1, #0
_02240558:
	strb r1, [sp, #1]
	ldr r0, [lr]
	str r0, [ip]
	ldr lr, [r2]
	ldr r8, [r2, #0xc]
	add lr, lr, #4
	cmp lr, r8
	ldreq r8, [r2, #4]
	str lr, [r2]
	streq r8, [r2]
	ldr lr, [r2]
	ldr r8, [r3]
	add ip, ip, #4
	cmp lr, r8
	bne _02240558
	str r0, [sp, #4]
_02240598:
	ldr r1, [sp, #0xc]
	sub r0, r7, #1
	sub r1, r1, r7
	str r1, [sp, #0xc]
	ldr r1, [r4]
	add r0, r1, r0, lsl #2
	add r1, r0, #4
	str r1, [r4]
	ldr r0, [r4, #0xc]
	cmp r1, r0
	ldreq r0, [r4, #4]
	streq r0, [r4]
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _022405EC
	add r0, sp, #0x4c
	ldmia r0, {r0, r1, r2, r3}
	stmia r5, {r0, r1, r2, r3}
	ldrb r7, [sp]
	strb r7, [r5, #0x10]
	b _02240474
_022405EC:
	ldr r0, [r4]
	str r0, [r6]
	ldr r0, [r4, #4]
	str r0, [r6, #4]
	ldr r0, [r4, #8]
	str r0, [r6, #8]
	ldr r0, [r4, #0xc]
	str r0, [r6, #0xc]
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov66_2240444

	arm_func_start ov66_2240614
ov66_2240614: // 0x02240614
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sb, r1
	mov r8, r2
	ldr r2, [r8]
	ldr r1, [sb]
	mov sl, r0
	mov r7, r3
	cmp r1, r2
	beq _02240780
	add r4, sp, #8
	mov fp, #0
	mov r5, #1
_02240648:
	ldr r0, [r8, #4]
	cmp r2, r0
	ldreq r0, [r8, #0xc]
	streq r0, [r8]
	ldr r0, [r8]
	sub r0, r0, #4
	str r0, [r8]
	ldr r1, [r7]
	ldr r0, [r7, #4]
	cmp r1, r0
	ldreq r0, [r7, #0xc]
	streq r0, [r7]
	ldr r0, [r7]
	sub r0, r0, #4
	str r0, [r7]
	ldr r1, [r7, #8]
	cmp r0, r1
	ldrlo r1, [r7, #4]
	sub r1, r0, r1
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sp, #8]
	ldr r1, [sb]
	ldr r0, [sb, #8]
	ldr r6, [r8, #8]
	cmp r1, r0
	movhs r2, r5
	ldr r0, [r8]
	movlo r2, fp
	cmp r0, r6
	movhs r3, #1
	movlo r3, #0
	cmp r2, r3
	bne _022406FC
	sub r1, r0, r1
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp, #4]
	addge r0, sp, #4
	b _02240728
_022406FC:
	cmp r0, r6
	ldrlo r6, [r8, #4]
	sub r1, r0, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp]
	addge r0, sp, #0
_02240728:
	ldr r6, [r0]
	ldr r1, [r8]
	add r0, r6, #1
	sub r1, r1, r6, lsl #2
	add r0, r1, r0, lsl #2
	str r1, [r8]
	sub r2, r0, r1
	ldr r3, [r7]
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	add r3, r3, #4
	mov r2, r0, asr #2
	sub r0, r3, r2, lsl #2
	mov r2, r2, lsl #2
	bl memmove
	ldr r0, [r7]
	sub r0, r0, r6, lsl #2
	str r0, [r7]
	ldr r2, [r8]
	ldr r0, [sb]
	cmp r0, r2
	bne _02240648
_02240780:
	ldr r0, [r7]
	str r0, [sl]
	ldr r0, [r7, #4]
	str r0, [sl, #4]
	ldr r0, [r7, #8]
	str r0, [sl, #8]
	ldr r0, [r7, #0xc]
	str r0, [sl, #0xc]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_2240614

	arm_func_start ov66_22407A8
ov66_22407A8: // 0x022407A8
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sb, r1
	mov r8, r2
	ldr r2, [sb]
	ldr r1, [r8]
	mov sl, r0
	mov r7, r3
	cmp r2, r1
	beq _022408F4
	add r4, sp, #8
	mov fp, #0
	mov r5, #1
_022407DC:
	ldr r1, [r7, #0xc]
	ldr r0, [r7]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sp, #8]
	ldr r2, [sb]
	ldr r0, [sb, #8]
	ldr r1, [r8]
	cmp r2, r0
	movhs r3, r5
	ldr r0, [r8, #8]
	movlo r3, fp
	cmp r1, r0
	movhs r0, #1
	movlo r0, #0
	cmp r3, r0
	bne _02240850
	sub r1, r1, r2
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp, #4]
	addge r0, sp, #4
	b _02240878
_02240850:
	ldr r0, [sb, #0xc]
	ldr r1, [sp, #8]
	sub r2, r0, r2
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp]
	addge r0, sp, #0
_02240878:
	ldr r6, [r0]
	ldr r1, [sb]
	ldr r0, [r7]
	add r2, r1, r6, lsl #2
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r2, r2, asr #2
	mov r2, r2, lsl #2
	bl memmove
	ldr r1, [sb]
	sub r0, r6, #1
	add r1, r1, r0, lsl #2
	add r2, r1, #4
	str r2, [sb]
	ldr r1, [sb, #0xc]
	cmp r2, r1
	ldreq r1, [sb, #4]
	streq r1, [sb]
	ldr r1, [r7]
	add r0, r1, r0, lsl #2
	add r1, r0, #4
	str r1, [r7]
	ldr r0, [r7, #0xc]
	cmp r1, r0
	ldreq r0, [r7, #4]
	streq r0, [r7]
	ldr r1, [sb]
	ldr r0, [r8]
	cmp r1, r0
	bne _022407DC
_022408F4:
	ldr r0, [r7]
	str r0, [sl]
	ldr r0, [r7, #4]
	str r0, [sl, #4]
	ldr r0, [r7, #8]
	str r0, [sl, #8]
	ldr r0, [r7, #0xc]
	str r0, [sl, #0xc]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_22407A8

	arm_func_start ov66_224091C
ov66_224091C: // 0x0224091C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r2, #0
	mov r4, r0
	strb r2, [sp]
	str r2, [r4, #4]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _022409AC
	ldr r1, [r4, #0x14]
	cmp r1, #0
	beq _0224095C
	ldr r2, [r4, #0xc]
	ldr r0, [r4, #0x10]
	add r0, r2, r0
	bl _u32_div_f
	mov r2, r1
_0224095C:
	ldr r8, [r4, #8]
	ldr r0, [r4, #0xc]
	ldr r1, [r4, #0x14]
	add r6, r8, r2, lsl #2
	add r7, r8, r0, lsl #2
	cmp r7, r6
	add sb, r8, r1, lsl #2
	beq _022409AC
	mov r5, #0
_02240980:
	ldr r1, [r7]
	cmp r1, #0
	beq _02240998
	mov r0, r5
	mov r2, r5
	bl DWC_Free
_02240998:
	add r7, r7, #4
	cmp r7, sb
	moveq r7, r8
	cmp r7, r6
	bne _02240980
_022409AC:
	add r0, r4, #8
	bl ov66_223C41C
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov66_224091C

	arm_func_start ov66_22409BC
ov66_22409BC: // 0x022409BC
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _02240A00
	ldr r2, [r4, #8]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #8]
	sub r1, r1, r2
	str r1, [r4, #8]
	ldr r1, [r4]
	cmp r1, #0
	beq _02240A00
	mov r2, r0
	bl DWC_Free
_02240A00:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_22409BC

	arm_func_start ov66_2240A0C
ov66_2240A0C: // 0x02240A0C
	stmfd sp!, {r3, r4, r5, lr}
	ldr r2, _02240A6C // =0x0225B6CC
	mov r5, r0
	ldr r3, [r2]
	mov r4, r1
	tst r3, #1
	bne _02240A3C
	ldr r0, _02240A70 // =0x022590A0
	orr r1, r3, #1
	mvn r3, #0xc0000001
	str r3, [r0]
	str r1, [r2]
_02240A3C:
	ldr r0, _02240A70 // =0x022590A0
	ldr r0, [r0]
	cmp r4, r0
	bls _02240A50
	bl ov66_223FC14
_02240A50:
	add r0, r4, #1
	mov r0, r0, lsl #2
	bl FUN_02246290
	str r0, [r5]
	add r0, r4, #1
	str r0, [r5, #0xc]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02240A6C: .word 0x0225B6CC
_02240A70: .word 0x022590A0
	arm_func_end ov66_2240A0C

	arm_func_start ov66_2240A74
ov66_2240A74: // 0x02240A74
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	ldr r3, [r7]
	ldr r0, [r7, #4]
	ldr r2, [r5]
	ldr r1, [r6]
	add r0, r3, r0, lsl #2
	cmp r1, r2
	bhi _02240AEC
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r3, r2, asr #2
	mov r2, r3, lsl #2
	sub r0, r0, r3, lsl #2
	bl memmove
	ldr r2, [r5]
	ldr r0, [r6]
	ldr r1, [r7, #8]
	sub r2, r2, r0
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	add r1, r1, r0, asr #2
	str r1, [r7, #8]
	ldr r1, [r7, #4]
	sub r0, r1, r0, asr #2
	str r0, [r7, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02240AEC:
	ldr r1, [r5, #4]
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r2, r2, asr #2
	sub r4, r0, r2, lsl #2
	mov r0, r4
	mov r2, r2, lsl #2
	bl memmove
	ldr r1, [r6]
	ldr r0, [r6, #0xc]
	sub r2, r0, r1
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r0, r0, asr #2
	mov r2, r0, lsl #2
	sub r0, r4, r0, lsl #2
	bl memmove
	ldr r1, [r6, #0xc]
	ldr r0, [r6]
	ldr r4, [r5]
	ldr r3, [r5, #4]
	sub r2, r1, r0
	sub r3, r4, r3
	mov r1, r2, asr #1
	mov r0, r3, asr #1
	add r1, r2, r1, lsr #30
	add r0, r3, r0, lsr #30
	mov r2, r1, asr #2
	ldr r1, [r7, #4]
	add r2, r2, r0, asr #2
	sub r0, r1, r2
	str r0, [r7, #4]
	ldr r0, [r7, #8]
	add r0, r0, r2
	str r0, [r7, #8]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2240A74

	arm_func_start ov66_2240B80
ov66_2240B80: // 0x02240B80
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	ldr r3, [r7, #4]
	ldr r0, [r7, #8]
	mov r6, r1
	ldr r1, [r7, #0xc]
	add r0, r3, r0
	mov r5, r2
	bl _u32_div_f
	ldr r2, [r7]
	ldr r0, [r5]
	add r4, r2, r1, lsl #2
	ldr r1, [r6]
	cmp r1, r0
	bhi _02240BFC
	sub r2, r0, r1
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r2, r0, asr #2
	mov r0, r4
	mov r2, r2, lsl #2
	bl memmove
	ldr r1, [r5]
	ldr r0, [r6]
	ldr r2, [r7, #8]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	add r0, r2, r0, asr #2
	str r0, [r7, #8]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02240BFC:
	ldr r2, [r6, #0xc]
	mov r0, r4
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r8, r2, asr #2
	mov r2, r8, lsl #2
	bl memmove
	ldr r1, [r5, #4]
	ldr r2, [r5]
	add r0, r4, r8, lsl #2
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r2, r2, asr #2
	mov r2, r2, lsl #2
	bl memmove
	ldr r1, [r5]
	ldr r0, [r5, #4]
	ldr r4, [r6, #0xc]
	ldr r3, [r6]
	sub r2, r1, r0
	sub r3, r4, r3
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	mov r0, r3, asr #1
	add r0, r3, r0, lsr #30
	mov r1, r1, asr #2
	ldr r2, [r7, #8]
	add r0, r1, r0, asr #2
	add r0, r2, r0
	str r0, [r7, #8]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end ov66_2240B80

	arm_func_start ov66_2240C80
ov66_2240C80: // 0x02240C80
	stmfd sp!, {r4, lr}
	ldr r3, [r0, #0xc]
	ldmia r0, {r2, r4}
	sub ip, r2, r4
	sub lr, r3, r4
	mov r3, ip, asr #1
	mov r2, lr, asr #1
	add r3, ip, r3, lsr #30
	add r2, lr, r2, lsr #30
	adds r1, r1, r3, asr #2
	addmi r1, r1, r2, asr #2
	bmi _02240CB8
	cmp r1, r2, asr #2
	subge r1, r1, r2, asr #2
_02240CB8:
	add r1, r4, r1, lsl #2
	str r1, [r0]
	ldmia sp!, {r4, pc}
	arm_func_end ov66_2240C80

	arm_func_start ov66_2240CC4
ov66_2240CC4: // 0x02240CC4
	stmfd sp!, {r3, lr}
	ldr ip, [r1]
	ldr r3, [r0]
	ldr lr, [r0, #8]
	sub r2, r3, ip
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	cmp r3, lr
	mov r2, r1, asr #2
	blo _02240D10
	cmp ip, lr
	bhs _02240D30
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	sub r2, r2, r0, asr #2
	b _02240D30
_02240D10:
	cmp ip, lr
	blo _02240D30
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	add r2, r2, r0, asr #2
_02240D30:
	mov r0, r2
	ldmia sp!, {r3, pc}
	arm_func_end ov66_2240CC4

	arm_func_start ov66_2240D38
ov66_2240D38: // 0x02240D38
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mvn r1, #0xe0000000
	mov r5, r0
	cmp r4, r1
	bls _02240D54
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02240D54:
	mov r0, r4, lsl #3
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_2240D38

	arm_func_start ov66_2240D68
ov66_2240D68: // 0x02240D68
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mvn r1, #0xe0000000
	mov r5, r0
	cmp r4, r1
	bls _02240D84
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02240D84:
	mov r0, r4, lsl #3
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_2240D68

	arm_func_start ov66_2240D98
ov66_2240D98: // 0x02240D98
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x24
	ldr r1, [sp, #0x44]
	mov r4, r0
	ldmia r1, {r0, r1, r2, r3}
	add r5, sp, #0x14
	add lr, sp, #0x34
	stmia r5, {r0, r1, r2, r3}
	add ip, sp, #4
	ldmia lr, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	mov r0, r5
	mov r1, ip
	bl ov66_2241110
	movs r1, r0
	addeq sp, sp, #0x24
	ldmeqia sp!, {r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
	mov r2, #0
	strb r2, [sp, #1]
	sub r0, sp, #4
	strb r2, [r0]
	ldr r2, [r0]
	mov r0, r4
	bl ov66_2241064
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r3, [r0]
	ldr r2, [sp, #0x44]
	add r1, sp, #0x34
	mov r0, r4
	bl ov66_2240E38
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_2240D98

	arm_func_start ov66_2240E38
ov66_2240E38: // 0x02240E38
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x5c
	mov r7, r0
	ldr r4, [r7]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x5c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r2, [r7, #4]
	ldr r0, [r7, #8]
	ldr r1, [r7, #0xc]
	add r0, r2, r0
	bl _u32_div_f
	add r4, r4, r1, lsl #2
	mov r0, r5
	mov r1, r6
	bl ov66_2241110
	str r0, [sp, #0x18]
	ldr r2, [r7]
	ldr r1, [r7, #0xc]
	add r1, r2, r1, lsl #2
	sub r2, r1, r4
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	mov r2, r1, asr #2
	cmp r0, r1, asr #2
	addlt r1, sp, #0x18
	str r2, [sp, #0x14]
	addge r1, sp, #0x14
	ldr r1, [r1]
	str r1, [sp, #0x14]
	ldr r1, [r7, #8]
	add r0, r1, r0
	str r0, [r7, #8]
	ldr r0, [r6]
	ldr r1, [sp, #0x14]
	str r0, [sp, #0x2c]
	ldr r2, [r6, #4]
	add r0, sp, #0x2c
	str r2, [sp, #0x30]
	ldr r2, [r6, #8]
	str r2, [sp, #0x34]
	ldr r2, [r6, #0xc]
	str r2, [sp, #0x38]
	bl ov66_22410CC
	ldr r8, [r0]
	ldr r3, [r0, #4]
	ldr r2, [r0, #8]
	ldr r1, [r0, #0xc]
	str r8, [sp, #0x4c]
	add r0, sp, #0x4c
	mov r8, sp
	str r3, [sp, #0x50]
	str r2, [sp, #0x54]
	str r1, [sp, #0x58]
	ldmia r0, {r0, r1, r2, r3}
	stmia r8, {r0, r1, r2, r3}
	str r4, [sp, #0x10]
	ldmia r6, {r0, r1, r2, r3}
	bl ov66_2240FB0
	ldr r1, [sp, #0x14]
	ldr r0, [sp, #0x18]
	subs r0, r0, r1
	str r0, [sp, #0x18]
	addeq sp, sp, #0x5c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r2, [r6]
	add r0, sp, #0x1c
	str r2, [sp, #0x1c]
	ldr r2, [r6, #4]
	str r2, [sp, #0x20]
	ldr r2, [r6, #8]
	str r2, [sp, #0x24]
	ldr r2, [r6, #0xc]
	str r2, [sp, #0x28]
	bl ov66_22410CC
	ldmia r0, {r8, lr}
	ldr ip, [r0, #8]
	ldr r6, [r0, #0xc]
	mov r4, sp
	ldmia r5, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r4, [r7]
	str r8, [sp, #0x3c]
	add r0, sp, #0x3c
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r6, [sp, #0x48]
	ldmia r0, {r0, r1, r2, r3}
	str r4, [sp, #0x10]
	bl ov66_2240FB0
	add sp, sp, #0x5c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov66_2240E38

	arm_func_start ov66_2240FB0
ov66_2240FB0: // 0x02240FB0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r0, [sp, #0x2c]
	ldr r6, [sp, #0x28]
	ldr r5, [sp, #0x38]
	ldr sl, [sp, #0x48]
	cmp r6, r5
	str r0, [sp]
	ldr r8, [sp, #0x34]
	ldr r7, [sp, #0x30]
	ldr fp, [sp, #0x40]
	beq _02241054
_02240FE0:
	cmp r6, r7
	movhs r0, #1
	movlo r0, #0
	cmp r5, fp
	movhs r1, #1
	movlo r1, #0
	cmp r0, r1
	subeq r1, r5, r6
	subne r1, r8, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov sb, r0, asr #2
	add r0, r6, sb, lsl #2
	sub r1, r0, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r4, r0, asr #2
	mov r2, r4, lsl #2
	mov r0, sl
	mov r1, r6
	bl memmove
	sub r0, sb, #1
	add r0, r6, r0, lsl #2
	add r6, r0, #4
	cmp r6, r8
	ldreq r6, [sp]
	add sl, sl, r4, lsl #2
	cmp r6, r5
	bne _02240FE0
_02241054:
	mov r0, sl
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_2240FB0

	arm_func_start ov66_2241064
ov66_2241064: // 0x02241064
	stmfd sp!, {r3, r4, r5, lr}
	ldr r2, _022410C4 // =0x0225B6D0
	mov r5, r0
	ldr r3, [r2]
	mov r4, r1
	tst r3, #1
	bne _02241094
	ldr r0, _022410C8 // =0x022590A4
	orr r1, r3, #1
	mvn r3, #0xc0000001
	str r3, [r0]
	str r1, [r2]
_02241094:
	ldr r0, _022410C8 // =0x022590A4
	ldr r0, [r0]
	cmp r4, r0
	bls _022410A8
	bl ov66_223FC14
_022410A8:
	add r0, r4, #1
	mov r0, r0, lsl #2
	bl FUN_02246290
	str r0, [r5]
	add r0, r4, #1
	str r0, [r5, #0xc]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_022410C4: .word 0x0225B6D0
_022410C8: .word 0x022590A4
	arm_func_end ov66_2241064

	arm_func_start ov66_22410CC
ov66_22410CC: // 0x022410CC
	stmfd sp!, {r4, lr}
	ldr r3, [r0, #0xc]
	ldmia r0, {r2, r4}
	sub ip, r2, r4
	sub lr, r3, r4
	mov r3, ip, asr #1
	mov r2, lr, asr #1
	add r3, ip, r3, lsr #30
	add r2, lr, r2, lsr #30
	adds r1, r1, r3, asr #2
	addmi r1, r1, r2, asr #2
	bmi _02241104
	cmp r1, r2, asr #2
	subge r1, r1, r2, asr #2
_02241104:
	add r1, r4, r1, lsl #2
	str r1, [r0]
	ldmia sp!, {r4, pc}
	arm_func_end ov66_22410CC

	arm_func_start ov66_2241110
ov66_2241110: // 0x02241110
	stmfd sp!, {r3, lr}
	ldr ip, [r1]
	ldr r3, [r0]
	ldr lr, [r0, #8]
	sub r2, r3, ip
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	cmp r3, lr
	mov r2, r1, asr #2
	blo _0224115C
	cmp ip, lr
	bhs _0224117C
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	sub r2, r2, r0, asr #2
	b _0224117C
_0224115C:
	cmp ip, lr
	blo _0224117C
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	add r2, r2, r0, asr #2
_0224117C:
	mov r0, r2
	ldmia sp!, {r3, pc}
	arm_func_end ov66_2241110

	arm_func_start ov66_2241184
ov66_2241184: // 0x02241184
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x84
	mov r4, r1
	mov r5, r0
	ldr r1, _02241534 // =0x022590DC
	add r0, sp, #0x60
	bl ov66_223BA6C
	add r0, sp, #0x78
	add r2, sp, #0x60
	mov r1, r5
	mov r3, #2
	bl ov66_223A3B0
	add r0, sp, #0x60
	bl ov66_223BADC
	ldr r0, [sp, #0x7c]
	cmp r0, #2
	beq _022411DC
	add r0, sp, #0x78
	bl ov66_223B578
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_022411DC:
	ldr r1, [sp, #0x78]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [r1, #4]
	bne _02241200
	ldrb r0, [r1]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02241200:
	cmp r0, #0x28
	bls _0224121C
	add r0, sp, #0x78
	bl ov66_223B578
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0224121C:
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [r1, #0x10]
	bne _0224123C
	ldrb r0, [r1, #0xc]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0224123C:
	cmp r0, #0x190
	bls _02241258
	add r0, sp, #0x78
	bl ov66_223B578
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_02241258:
	ldr r1, _02241538 // =0x022590E0
	add r0, sp, #0x54
	bl ov66_223BA6C
	ldr r1, [sp, #0x78]
	add r0, sp, #0x6c
	add r2, sp, #0x54
	mov r3, #0
	bl ov66_223A3B0
	add r0, sp, #0x54
	bl ov66_223BADC
	ldr r0, [sp, #0x70]
	cmp r0, #6
	bhs _022412A8
	add r0, sp, #0x6c
	bl ov66_223B578
	add r0, sp, #0x78
	bl ov66_223B578
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_022412A8:
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #1
	ldrne r0, [r1, #8]
	add r1, sp, #0x20
	bl strtoul
	str r0, [r4]
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0xd
	ldrne r0, [r1, #0x14]
	add r1, sp, #0x20
	bl strtoul
	str r0, [r4, #4]
	ldr r1, [sp, #0x6c]
	ldr r5, _0224153C // =0x022590E4
	ldr r0, [r1, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0x19
	ldrne r1, [r1, #0x20]
	add r0, sp, #0x3c
	bl ov66_223BA6C
	add r0, sp, #0x48
	mov r1, r5
	bl ov66_223BA6C
	add r0, sp, #0x3c
	add r1, sp, #0x48
	mov r2, #1
	bl ov66_224154C
	mov r5, r0
	add r0, sp, #0x48
	bl ov66_223BADC
	add r0, sp, #0x3c
	bl ov66_223BADC
	cmp r5, #0
	moveq r0, #1
	movne r0, #0
	str r0, [r4, #8]
	ldr r1, [sp, #0x6c]
	ldr r5, _02241540 // =0x022590E8
	ldr r0, [r1, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0x25
	ldrne r1, [r1, #0x2c]
	add r0, sp, #0x24
	bl ov66_223BA6C
	add r0, sp, #0x30
	mov r1, r5
	bl ov66_223BA6C
	add r0, sp, #0x24
	add r1, sp, #0x30
	mov r2, #1
	bl ov66_224154C
	mov r5, r0
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	cmp r5, #0
	moveq r0, #0
	movne r0, #1
	str r0, [r4, #0xc]
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0x31
	ldrne r0, [r1, #0x38]
	add r1, sp, #0x20
	bl strtol
	str r0, [r4, #0x10]
	add r0, sp, #0x6c
	bl ov66_223B578
	ldr r0, [r4, #8]
	cmp r0, #1
	bne _02241438
	ldr r1, [sp, #0x78]
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0xd
	ldrne r0, [r1, #0x14]
	add r1, r4, #0x18
	bl ov66_2237EF8
	cmp r0, #0
	bne _02241520
	add r0, sp, #0x78
	bl ov66_223B578
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_02241438:
	ldr r0, [sp, #0x78]
	ldr r1, [r0, #0xc]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r0, #0x10]
	bne _0224145C
	ldrb r1, [r0, #0xc]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_0224145C:
	ldr r1, [r0, #0xc]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldr r1, [r0, #0xc]
	addeq r3, r0, #0xd
	mov r1, r1, lsl #0x1f
	ldrne r3, [r0, #0x14]
	movs r1, r1, lsr #0x1f
	addeq r5, r0, #0xd
	ldrne r5, [r0, #0x14]
	mov r0, #0
	add r2, r3, r2
	strb r0, [sp, #0xb]
	strb r0, [sp, #0xc]
	strb r0, [sp, #0xa]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x1c]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0x10
	str r5, [sp, #0x10]
	str r2, [sp, #0x18]
	add r0, r4, #0x18
	ldmia r1, {r1, r2}
	bl ov66_22366CC
	mov r2, #0
	strb r2, [sp, #8]
	ldr r1, [r4, #0x1c]
	ldr r0, [r4, #0x20]
	cmp r1, r0
	bhs _02241500
	add r3, r1, #1
	str r3, [r4, #0x1c]
	ldr r0, _02241544 // =0x022590D8
	ldr r1, [r4, #0x18]
	ldrsb r2, [r0, #1]
	add r0, r1, r3
	strb r2, [r0, #-1]
	b _02241520
_02241500:
	strb r2, [sp, #9]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	ldr r1, _02241548 // =0x022590D9
	add r0, r4, #0x18
	bl ov66_2241944
_02241520:
	add r0, sp, #0x78
	bl ov66_223B578
	mov r0, #1
	add sp, sp, #0x84
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_02241534: .word 0x022590DC
_02241538: .word 0x022590E0
_0224153C: .word 0x022590E4
_02241540: .word 0x022590E8
_02241544: .word 0x022590D8
_02241548: .word 0x022590D9
	arm_func_end ov66_2241184

	arm_func_start ov66_224154C
ov66_224154C: // 0x0224154C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x30
	mov r6, r0
	ldr r0, [r6]
	mov r5, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r4, r2
	ldmneib r6, {r0, r1}
	bne _02241584
	ldrb r0, [r6]
	add r1, r6, #1
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02241584:
	add r0, r1, r0
	str r0, [sp, #0x28]
	ldr r0, [r6]
	add r1, sp, #0x28
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	moveq r2, #1
	movne r2, #0
	ldr r0, [r1]
	cmp r2, #0
	addne r3, r6, #1
	ldreq r3, [r6, #8]
	str r0, [sp, #0x10]
	cmp r2, #0
	addne sb, r6, #1
	ldreq sb, [r6, #8]
	ldr r8, [sp, #0x10]
	str r3, [sp, #0x24]
	str r3, [sp, #0xc]
	str sb, [sp, #0x2c]
	str sb, [sp, #0x14]
	cmp sb, r8
	beq _02241600
	ldr r7, [sp, #0xc]
_022415E4:
	ldrsb r0, [sb], #1
	bl ov66_22416BC
	cmp sb, r8
	strb r0, [r7], #1
	bne _022415E4
	str r7, [sp, #0xc]
	str sb, [sp, #0x14]
_02241600:
	ldr r0, [r5]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmneib r5, {r0, r1}
	bne _02241624
	ldrb r0, [r5]
	add r1, r5, #1
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02241624:
	add r0, r1, r0
	str r0, [sp, #0x1c]
	ldr r0, [r5]
	add r1, sp, #0x1c
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	moveq r2, #1
	movne r2, #0
	ldr r0, [r1]
	cmp r2, #0
	addne r3, r5, #1
	ldreq r3, [r5, #8]
	str r0, [sp, #4]
	cmp r2, #0
	addne sb, r5, #1
	ldreq sb, [r5, #8]
	ldr r8, [sp, #4]
	str r3, [sp, #0x18]
	str r3, [sp]
	str sb, [sp, #0x20]
	str sb, [sp, #8]
	cmp sb, r8
	beq _022416A0
	ldr r7, [sp]
_02241684:
	ldrsb r0, [sb], #1
	bl ov66_22416BC
	cmp sb, r8
	strb r0, [r7], #1
	bne _02241684
	str r7, [sp]
	str sb, [sp, #8]
_022416A0:
	mov r0, r6
	mov r2, r4
	mov r3, r5
	mov r1, #0
	bl ov66_2241A5C
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov66_224154C

	arm_func_start ov66_22416BC
ov66_22416BC: // 0x022416BC
	cmp r0, #0
	bxlt lr
	cmp r0, #0x80
	bxge lr
	ldr r1, _022416D8 // =0x020FE764
	ldrb r0, [r1, r0]
	bx lr
	.align 2, 0
_022416D8: .word 0x020FE764
	arm_func_end ov66_22416BC

	arm_func_start ov66_22416DC
ov66_22416DC: // 0x022416DC
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x1c4
	movs sb, r0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	ldr r5, [sp, #0x1e0]
	ldr r4, [sp, #0x1e4]
	bne _02241710
	cmp r5, #0x190
	addhi sp, sp, #0x1c4
	movhi r0, #0
	ldmhiia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02241710:
	cmp sb, #1
	bne _02241728
	cmp r5, #0x12c
	addhi sp, sp, #0x1c4
	movhi r0, #0
	ldmhiia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02241728:
	ldr r1, _0224191C // =0x022590EC
	mov r0, r4
	bl ov66_22419DC
	ldr r2, _02241920 // =0x022590F0
	add r0, sp, #0x24
	mov r1, #3
	mov r3, #0
	bl ov66_2236A38
	mov r0, r4
	add r1, sp, #0x24
	bl ov66_2241974
	add r0, sp, #0x24
	bl ov66_223BADC
	ldr r1, _02241924 // =0x022590E0
	mov r0, r4
	bl ov66_22419A8
	ldr r2, _02241920 // =0x022590F0
	add r0, sp, #0x18
	mov r1, #2
	mov r3, #6
	bl ov66_2236A38
	mov r0, r4
	add r1, sp, #0x18
	bl ov66_2241974
	add r0, sp, #0x18
	bl ov66_223BADC
	ldr r1, _02241924 // =0x022590E0
	mov r0, r4
	bl ov66_22419A8
	cmp sb, #0
	beq _022417B0
	cmp sb, #1
	beq _022417C0
	b _022417D0
_022417B0:
	ldr r1, _02241928 // =0x022590E8
	mov r0, r4
	bl ov66_22419A8
	b _022417DC
_022417C0:
	ldr r1, _0224192C // =0x022590E4
	mov r0, r4
	bl ov66_22419A8
	b _022417DC
_022417D0:
	add sp, sp, #0x1c4
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_022417DC:
	ldr r1, _02241924 // =0x022590E0
	mov r0, r4
	bl ov66_22419A8
	cmp r8, #0
	beq _02241808
	cmp r8, #1
	bne _02241818
	ldr r1, _02241930 // =0x022590F4
	mov r0, r4
	bl ov66_22419A8
	b _02241824
_02241808:
	ldr r1, _02241928 // =0x022590E8
	mov r0, r4
	bl ov66_22419A8
	b _02241824
_02241818:
	add sp, sp, #0x1c4
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02241824:
	ldr r1, _02241924 // =0x022590E0
	mov r0, r4
	bl ov66_22419A8
	ldr r2, _02241920 // =0x022590F0
	add r0, sp, #0xc
	mov r3, r7
	mov r1, #8
	bl ov66_2236A38
	add r1, sp, #0xc
	mov r0, r4
	bl ov66_2241974
	add r0, sp, #0xc
	bl ov66_223BADC
	ldr r1, _02241924 // =0x022590E0
	mov r0, r4
	bl ov66_22419A8
	ldr r1, _02241934 // =0x022590F8
	mov r0, r4
	bl ov66_22419A8
	ldr r1, _02241938 // =0x022590DC
	mov r0, r4
	bl ov66_22419A8
	cmp sb, #0
	beq _02241890
	cmp sb, #1
	beq _022418BC
	b _02241904
_02241890:
	ldr r2, _0224193C // =0x022590FC
	add r0, sp, #0
	mov r1, r5
	mov r3, r6
	bl ov66_2236A38
	add r1, sp, #0
	mov r0, r4
	bl ov66_2241974
	add r0, sp, #0
	bl ov66_223BADC
	b _02241910
_022418BC:
	ldr r2, _02241940 // =0x00000191
	add r0, sp, #0x30
	mov r1, #0
	bl MI_CpuFill8
	ldr r3, _02241940 // =0x00000191
	add r2, sp, #0x30
	mov r0, r6
	mov r1, r5
	bl DWC_Base64Encode
	mvn r1, #0
	cmp r0, r1
	addeq sp, sp, #0x1c4
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	add r1, sp, #0x30
	mov r0, r4
	bl ov66_22419A8
	b _02241910
_02241904:
	add sp, sp, #0x1c4
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02241910:
	mov r0, #1
	add sp, sp, #0x1c4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_0224191C: .word 0x022590EC
_02241920: .word 0x022590F0
_02241924: .word 0x022590E0
_02241928: .word 0x022590E8
_0224192C: .word 0x022590E4
_02241930: .word 0x022590F4
_02241934: .word 0x022590F8
_02241938: .word 0x022590DC
_0224193C: .word 0x022590FC
_02241940: .word 0x00000191
	arm_func_end ov66_22416DC

	arm_func_start ov66_2241944
ov66_2241944: // 0x02241944
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r2, #0
	mov r5, r0
	mov r1, #1
	strb r2, [sp]
	bl ov66_223B7A4
	mov r0, r5
	mov r2, r4
	mov r1, #1
	bl ov66_2241A94
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_2241944

	arm_func_start ov66_2241974
ov66_2241974: // 0x02241974
	ldr r2, [r0]
	mov r3, r1
	mov r1, r2, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r1, [r0, #4]
	bne _02241998
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r1, r1, lsr #0x19
_02241998:
	ldr ip, _022419A4 // =ov66_223BCC8
	mov r2, #0
	bx ip
	.align 2, 0
_022419A4: .word ov66_223BCC8
	arm_func_end ov66_2241974

	arm_func_start ov66_22419A8
ov66_22419A8: // 0x022419A8
	ldr r2, [r0]
	mov r3, r1
	mov r1, r2, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r1, [r0, #4]
	bne _022419CC
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r1, r1, lsr #0x19
_022419CC:
	ldr ip, _022419D8 // =ov66_2241A10
	mov r2, #0
	bx ip
	.align 2, 0
_022419D8: .word ov66_2241A10
	arm_func_end ov66_22419A8

	arm_func_start ov66_22419DC
ov66_22419DC: // 0x022419DC
	ldr r2, [r0]
	mov r3, r1
	mov r1, r2, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r0, #4]
	bne _02241A00
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_02241A00:
	ldr ip, _02241A0C // =ov66_2241A10
	mov r1, #0
	bx ip
	.align 2, 0
_02241A0C: .word ov66_2241A10
	arm_func_end ov66_22419DC

	arm_func_start ov66_2241A10
ov66_2241A10: // 0x02241A10
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0xc
	mov r4, r3
	mov r7, r0
	mov r0, r4
	mov r6, r1
	mov r5, r2
	bl strlen
	add r0, r4, r0
	ldrb ip, [sp, #8]
	str r0, [sp]
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	strb ip, [sp, #4]
	bl ov66_223C018
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov66_2241A10

	arm_func_start ov66_2241A5C
ov66_2241A5C: // 0x02241A5C
	stmfd sp!, {r3, lr}
	ldr ip, [r3]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldmneib r3, {ip, lr}
	bne _02241A84
	ldrb ip, [r3]
	add lr, r3, #1
	mov r3, ip, lsl #0x18
	mov ip, r3, lsr #0x19
_02241A84:
	mov r3, lr
	str ip, [sp]
	bl ov66_223BF38
	ldmia sp!, {r3, pc}
	arm_func_end ov66_2241A5C

	arm_func_start ov66_2241A94
ov66_2241A94: // 0x02241A94
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	mov r4, #0
	mov r6, r0
	add r3, r6, #8
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	str r4, [sp, #0x10]
	str r4, [sp, #0x18]
	str r3, [sp, #0x14]
	mov r5, r1
	ldr r3, [r6, #4]
	ldr r1, [r6, #8]
	add r3, r3, r5
	sub r1, r3, r1
	mov r4, r2
	bl ov66_223B7A4
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov66_223C8E0
	ldr r2, [r6, #4]
	mov r0, #0
	ldr r1, [sp, #8]
	strb r0, [sp, #2]
	str r2, [sp, #0x18]
	add r3, r1, r2
	ldr r0, [sp, #0xc]
	ldrb r1, [r4]
	mov r2, r5
	add r0, r3, r0
	bl memset
	ldr r0, [sp, #0xc]
	mov r1, #0
	add r0, r0, r5
	str r0, [sp, #0xc]
	ldr r5, [r6]
	ldr r2, [r6, #4]
	ldr r0, [sp, #0x18]
	strb r1, [sp, #1]
	add r1, r5, r2
	sub r4, r1, r5
	sub r2, r0, r4
	ldr r0, [sp, #8]
	str r2, [sp, #0x18]
	add r0, r0, r2
	mov r1, r5
	mov r2, r4
	bl memcpy
	mov r0, r5
	mov r1, #0
	mov r2, r4
	bl memset
	ldr r1, [sp, #0xc]
	mov r0, #0
	add r1, r1, r4
	str r1, [sp, #0xc]
	strb r0, [sp]
	str r0, [r6, #4]
	add r3, sp, #0x10
	ldr r2, [r6, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r6, #8]
	str r2, [r3]
	ldr r2, [r6]
	ldr r1, [sp, #8]
	str r1, [r6]
	str r2, [sp, #8]
	ldr r2, [r6, #4]
	ldr r1, [sp, #0xc]
	str r1, [r6, #4]
	str r2, [sp, #0xc]
	bl ov66_223C8C4
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov66_2241A94

	arm_func_start ov66_2241BD8
ov66_2241BD8: // 0x02241BD8
	ldr r1, [r0, #0x28]
	cmp r1, #3
	addls pc, pc, r1, lsl #2
	bx lr
_02241BE8: // jump table
	b _02241BF8 // case 0
	b _02241C1C // case 1
	b _02241C30 // case 2
	bx lr // case 3
_02241BF8:
	ldr r1, [r0, #0x30]
	cmp r1, #0
	ldrne r1, [r0, #0x34]
	cmpne r1, #0
	movne r1, #1
	moveq r1, #0
	str r1, [r0, #0x28]
	cmp r1, #0
	bxeq lr
_02241C1C:
	ldr r1, [r0, #0x10]
	cmp r1, #0
	movne r1, #2
	strne r1, [r0, #0x28]
	bx lr
_02241C30:
	ldr r1, [r0, #0x38]
	cmp r1, #0
	movne r1, #3
	strne r1, [r0, #0x28]
	bx lr
	arm_func_end ov66_2241BD8

	arm_func_start ov66_2241C44
ov66_2241C44: // 0x02241C44
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x34
	mov r6, r0
	mov r0, #0x3c
	mov r5, r1
	bl FUN_02246290
	movs r4, r0
	beq _02241CD4
	ldr r1, _02241D3C // =0x02258EB0
	mov r0, #5
	str r1, [r4]
	str r0, [r4, #4]
	mov r2, #0
	str r2, [r4, #8]
	str r2, [r4, #0xc]
	add r0, sp, #0x24
	add r1, sp, #0x18
	str r2, [r4, #0x10]
	bl DWC_GetDateTime
	add r0, sp, #0x24
	add r1, sp, #0x18
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #8]
	str r1, [r4, #0xc]
	mov r1, #1
	str r1, [r4, #0x14]
	mov r0, #0
	str r0, [r4, #0x18]
	str r0, [r4, #0x1c]
	str r0, [r4, #0x20]
	str r1, [r4, #0x24]
	str r0, [r4, #0x28]
	str r5, [r4, #0x2c]
	str r0, [r4, #0x30]
	str r0, [r4, #0x34]
	str r0, [r4, #0x38]
_02241CD4:
	add r0, sp, #4
	add r2, sp, #0x14
	add r1, r6, #4
	str r4, [sp, #0x14]
	bl ov66_2242550
	ldrb r0, [sp, #8]
	mov r2, #0
	ldr r1, [sp, #4]
	strb r2, [sp, #1]
	str r1, [sp, #0xc]
	strb r2, [sp]
	strb r0, [sp, #0x10]
	tst r0, #0xff
	bne _02241D30
	ldr r0, [sp, #0x14]
	cmp r0, #0
	beq _02241D24
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02241D24:
	add sp, sp, #0x34
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, pc}
_02241D30:
	mov r0, #1
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_02241D3C: .word 0x02258EB0
	arm_func_end ov66_2241C44

	arm_func_start ov66_2241D40
ov66_2241D40: // 0x02241D40
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r4, r0
	ldr r0, [r4, #0xc]
	add r6, r4, #8
	mov r7, r1
	str r0, [sp, #4]
	cmp r0, r6
	beq _02241DC0
	add r5, sp, #4
_02241D68:
	ldr r0, [r0, #0xc]
	ldr r1, [r0, #0x2c]
	cmp r7, r1
	bne _02241DAC
	cmp r0, #0
	beq _02241D8C
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02241D8C:
	add r1, sp, #4
	ldr r2, [r1]
	add r0, sp, #0
	add r1, r4, #4
	bl ov66_22425F0
	add sp, sp, #8
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02241DAC:
	mov r0, r5
	bl ov66_223AD6C
	ldr r0, [sp, #4]
	cmp r0, r6
	bne _02241D68
_02241DC0:
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2241D40

	arm_func_start ov66_2241DCC
ov66_2241DCC: // 0x02241DCC
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x18
	mov r5, r0
	ldr r0, [r5, #0x3c]
	mov r4, r1
	cmp r4, r0
	addeq sp, sp, #0x18
	addeq r0, r5, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r5, #0xc]
	add r7, r5, #8
	str r4, [sp, #0xc]
	str r4, [sp]
	str r7, [sp, #0x10]
	str r7, [sp, #4]
	str r0, [sp, #0x14]
	str r0, [sp, #8]
	add r6, sp, #8
	b _02241E20
_02241E18:
	mov r0, r6
	bl ov66_223AD6C
_02241E20:
	ldr r1, [sp, #8]
	cmp r1, r7
	ldrne r0, [r1, #0xc]
	ldrne r0, [r0, #0x2c]
	cmpne r4, r0
	bne _02241E18
	add r0, r5, #8
	cmp r1, r0
	ldr r0, [sp, #8]
	moveq r0, #0
	ldrne r0, [r0, #0xc]
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2241DCC

	arm_func_start ov66_2241E54
ov66_2241E54: // 0x02241E54
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r5, r0
	ldr r0, [r5, #0xc]
	add r4, r5, #8
	str r0, [sp, #8]
	cmp r0, r4
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r8, sp, #8
	add r7, sp, #0
	add r6, sp, #4
_02241E84:
	ldr r0, [r0, #0xc]
	cmp r0, #0
	beq _02241E9C
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02241E9C:
	ldr r1, [sp, #8]
	mov r0, r8
	str r1, [sp, #4]
	bl ov66_223AD6C
	ldr r2, [r6]
	mov r0, r7
	add r1, r5, #4
	bl ov66_22425F0
	ldr r0, [sp, #8]
	cmp r0, r4
	bne _02241E84
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov66_2241E54

	arm_func_start ov66_2241ED0
ov66_2241ED0: // 0x02241ED0
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r1, r0
	ldr r0, [r1, #4]
	mov r2, #0
	add r0, r0, #1
	mov r4, r2
	cmp r0, #0x14
	bne _02241F10
	add r0, sp, #0
	bl ov66_2241F2C
	ldr r0, [sp, #4]
	mov r2, #1
	add r0, r0, #1
	cmp r0, #0x14
	moveq r4, r2
_02241F10:
	cmp r2, #0
	beq _02241F20
	add r0, sp, #0
	bl ov66_223B5D0
_02241F20:
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_2241ED0

	arm_func_start ov66_2241F2C
ov66_2241F2C: // 0x02241F2C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0xc
	mov r2, #0
	mov r6, r0
	str r2, [r6]
	str r2, [r6, #4]
	mov r4, r1
	str r2, [r6, #8]
	ldr r1, [r4, #4]
	cmp r1, #0
	bls _02241F5C
	bl ov66_2242738
_02241F5C:
	ldr r0, [r4, #0xc]
	add r5, r4, #8
	str r0, [sp, #8]
	cmp r0, r5
	addeq sp, sp, #0xc
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	sub r8, sp, #4
	mov r4, #0
	add sb, sp, #4
	add r7, sp, #8
_02241F84:
	ldr r1, [r0, #0xc]
	ldr r0, [r1, #0x38]
	cmp r0, #0
	beq _02241FEC
	ldr r0, [r1, #0x2c]
	str r0, [sp, #4]
	strb r4, [sp]
	ldr r1, [r6, #4]
	ldr r0, [r6, #8]
	cmp r1, r0
	bhs _02241FD0
	mov r0, r1
	add r2, r0, #1
	str r2, [r6, #4]
	ldr r0, [r6]
	ldr r1, [sp, #4]
	add r0, r0, r2, lsl #2
	str r1, [r0, #-4]
	b _02241FEC
_02241FD0:
	strb r4, [sp, #1]
	and r0, r4, #0xff
	strb r0, [r8]
	ldr r2, [r8]
	mov r0, r6
	mov r1, sb
	bl ov66_2242818
_02241FEC:
	mov r0, r7
	bl ov66_223AD6C
	ldr r0, [sp, #8]
	cmp r0, r5
	bne _02241F84
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov66_2241F2C

	arm_func_start ov66_2242008
ov66_2242008: // 0x02242008
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xd4
	ldr ip, _02242398 // =0x02258EB0
	mov r6, r0
	mov sb, r1
	add r8, sp, #0x9c
	mov r7, #0
	mov sl, #5
	add r0, sp, #0x78
	add r1, sp, #0x6c
	mov r5, r2
	mov r4, r3
	str ip, [sp, #0x98]
	str sl, [r8]
	str r7, [r8, #4]
	str r7, [r8, #8]
	str r7, [r8, #0xc]
	add r7, sp, #0x98
	bl DWC_GetDateTime
	add r0, sp, #0x78
	add r1, sp, #0x6c
	bl RTC_ConvertDateTimeToSecond
	str sb, [r7, #0x2c]
	stmib r8, {r0, r1}
	mov r0, #1
	str r0, [r8, #0x10]
	str r0, [r7, #0x24]
	mov r0, #0
	str r0, [r7, #0x18]
	str r0, [r7, #0x1c]
	str r0, [r7, #0x20]
	str r0, [r7, #0x28]
	str r0, [r7, #0x30]
	str r0, [r7, #0x34]
	str r0, [r7, #0x38]
	mov r1, r8
	add r0, r6, #0x14
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _022420E0
	ldr r0, [r7, #4]
	add r1, r7, #8
	str r0, [r6, #0x14]
	add r0, r6, #0x18
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r7, #0x10]
	mov r0, #1
	str r1, [r6, #0x20]
	str r0, [r6, #0x24]
_022420E0:
	ldr sb, [r6, #0x2c]
	ldr r0, [r7, #0x1c]
	mov r8, #0
	cmp sb, r0
	bne _0224212C
	cmp sb, #0
	beq _02242128
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov sl, r0
	add r0, r7, #0x18
	bl ov66_2235B8C
	mov r1, r0
	mov r0, sl
	mov r2, sb
	bl memcmp
	cmp r0, #0
	bne _0224212C
_02242128:
	mov r8, #1
_0224212C:
	cmp r8, #0
	bne _02242170
	add r0, r6, #0x28
	add r1, r7, #0x18
	cmp r0, r1
	beq _02242168
	mov r3, #0
	ldr r1, [r7, #0x18]
	ldr r8, [r7, #0x1c]
	strb r3, [sp, #0xd]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r8
	bl ov66_2236674
_02242168:
	mov r0, #1
	str r0, [r6, #0x34]
_02242170:
	ldr r1, [r7, #0x28]
	add r0, sp, #0xb0
	str r1, [r6, #0x38]
	ldr r1, [r7, #0x2c]
	str r1, [r6, #0x3c]
	ldr r1, [r7, #0x30]
	str r1, [r6, #0x40]
	ldr r1, [r7, #0x34]
	str r1, [r6, #0x44]
	ldr r1, [r7, #0x38]
	str r1, [r6, #0x48]
	bl ov66_223B8E8
	mov r2, #0
	mov r3, #5
	add r0, sp, #0x5c
	add r1, sp, #0x50
	str r3, [sp, #0x88]
	str r2, [sp, #0x8c]
	str r2, [sp, #0x90]
	str r2, [sp, #0x94]
	bl DWC_GetDateTime
	add r0, sp, #0x5c
	add r1, sp, #0x50
	bl RTC_ConvertDateTimeToSecond
	str r1, [sp, #0x90]
	ldr r1, [sp, #0x88]
	str r0, [sp, #0x8c]
	add r0, sp, #0x8c
	str r1, [sp, #0x3c]
	ldmia r0, {r2, r3}
	add r1, sp, #0x40
	stmia r1, {r2, r3}
	ldr r0, [sp, #0x94]
	add r7, sp, #0x3c
	str r0, [sp, #0x48]
	mov r0, #1
	str r0, [sp, #0x4c]
	add r0, r6, #0x14
	mov r1, r7
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02242250
	ldr r0, [r7]
	add r1, r7, #4
	str r0, [r6, #0x14]
	add r0, r6, #0x18
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r7, #0xc]
	mov r0, #1
	str r1, [r6, #0x20]
	str r0, [r6, #0x24]
_02242250:
	mov r1, #1
	str r1, [r6, #0x40]
	mov r0, #0
	add r2, r5, r4
	str r0, [sp, #0x20]
	str r0, [sp, #0x24]
	str r0, [sp, #0x28]
	strb r0, [sp, #9]
	strb r0, [sp, #0xa]
	strb r0, [sp, #8]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x1c]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r0, sp, #0x20
	add r1, sp, #0x10
	str r5, [sp, #0x10]
	str r2, [sp, #0x18]
	ldmia r1, {r1, r2}
	bl ov66_22366CC
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x20]
	ldr r4, [sp, #0x24]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	str r2, [sp, #0x34]
	sub r2, sp, #4
	strb r0, [r2]
	add r5, sp, #0x2c
	ldr r3, [r2]
	mov r0, r5
	add r2, r1, r4
	bl ov66_223C2F4
	mov r0, #1
	str r0, [r5, #0xc]
	ldr r8, [r6, #0x2c]
	ldr r0, [sp, #0x30]
	mov r7, #0
	cmp r8, r0
	bne _02242334
	cmp r8, #0
	beq _02242330
	add r0, r6, #0x28
	bl ov66_2235B8C
	mov r4, r0
	mov r0, r5
	bl ov66_2235B8C
	mov r1, r0
	mov r0, r4
	mov r2, r8
	bl memcmp
	cmp r0, #0
	bne _02242334
_02242330:
	mov r7, #1
_02242334:
	cmp r7, #0
	bne _02242370
	add r0, r6, #0x28
	cmp r0, r5
	beq _02242368
	mov r3, #0
	ldmia r5, {r1, r4}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4
	bl ov66_2236674
_02242368:
	mov r0, #1
	str r0, [r6, #0x34]
_02242370:
	add r0, sp, #0x2c
	bl ov66_223B8E8
	mov r1, #1
	add r0, sp, #0x20
	str r1, [r6, #0x44]
	bl ov66_223B8E8
	mov r0, #1
	str r0, [r6, #0x48]
	add sp, sp, #0xd4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02242398: .word 0x02258EB0
	arm_func_end ov66_2242008

	arm_func_start ov66_224239C
ov66_224239C: // 0x0224239C
	ldr r0, [r0, #0x28]
	cmp r0, #1
	movge r0, #1
	movlt r0, #0
	bx lr
	arm_func_end ov66_224239C

	arm_func_start ov66_22423B0
ov66_22423B0: // 0x022423B0
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x20
	ldr r1, _0224245C // =0x02259100
	mov r7, r0
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r2, [sp, #4]
	str r1, [sp]
	ldr r0, [r7, #0xc]
	add r5, r7, #8
	str r1, [sp, #0x10]
	str r2, [sp, #0x14]
	str r5, [sp, #0x18]
	str r5, [sp, #8]
	str r0, [sp, #0x1c]
	str r0, [sp, #0xc]
	add r6, sp, #0
	add r4, sp, #0xc
	b _02242404
_022423FC:
	mov r0, r4
	bl ov66_223AD6C
_02242404:
	ldr r0, [sp, #0xc]
	cmp r0, r5
	beq _02242440
	ldr r1, [sp, #4]
	ldr r0, [r0, #0xc]
	tst r1, #1
	add r0, r0, r1, asr #1
	ldreq r1, [r6]
	beq _02242434
	ldr r2, [r0]
	ldr r1, [r6]
	ldr r1, [r2, r1]
_02242434:
	blx r1
	cmp r0, #0
	bne _022423FC
_02242440:
	ldr r1, [sp, #0xc]
	add r0, r7, #8
	cmp r1, r0
	moveq r0, #1
	movne r0, #0
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_0224245C: .word 0x02259100
	arm_func_end ov66_22423B0

	arm_func_start ov66_2242460
ov66_2242460: // 0x02242460
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1c
	ldr r1, [r0, #0xc]
	add sb, r0, #8
	str r1, [sp]
	cmp r1, sb
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r6, #0
	add fp, sp, #4
	add sl, sp, #0x10
_0224248C:
	ldr r0, [r1, #0xc]
	bl ov66_2241BD8
	ldr r0, [sp]
	ldr r3, [r0, #0xc]
	ldr r0, [r3, #0x28]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _02242534
_022424AC: // jump table
	b _02242534 // case 0
	b _02242534 // case 1
	b _022424BC // case 2
	b _02242534 // case 3
_022424BC:
	str r6, [r3, #0x14]
	ldr r0, [sp]
	ldr r5, [r0, #0xc]
	str r6, [r5, #0x24]
	ldr r2, [r3, #4]
	ldr r1, [r3, #8]
	ldr r0, [r3, #0xc]
	str r2, [sp, #0xc]
	str r1, [sp, #4]
	str r0, [sp, #8]
	ldmia fp, {r0, r1, r2}
	stmia sl, {r0, r1, r2}
	ldr r0, [sp]
	ldr r7, [r5, #0x1c]
	ldr r0, [r0, #0xc]
	ldr r8, [r0, #0x2c]
	bl ov66_2234FC4
	mov r4, r0
	add r0, r5, #0x18
	bl ov66_2235B8C
	mov r2, r0
	ldr r4, [r4, #0xc]
	mov r3, r7
	mov r0, r8
	mov r1, sl
	blx r4
	ldr r0, [sp]
	ldr r1, [r0, #0xc]
	mov r0, #1
	str r0, [r1, #0x38]
_02242534:
	add r0, sp, #0
	bl ov66_223AD6C
	ldr r1, [sp]
	cmp r1, sb
	bne _0224248C
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_2242460

	arm_func_start ov66_2242550
ov66_2242550: // 0x02242550
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r4, r2
	mov r5, r1
	mov r6, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r5
	mov r1, r4
	str ip, [sp]
	bl ov66_2242848
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _022425A8
	ldr r2, [r3, #0xc]
	ldr r0, [r4]
	ldr r2, [r2, #0x2c]
	ldr r0, [r0, #0x2c]
	cmp r2, r0
	bge _022425D4
_022425A8:
	str r4, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r5
	bl ov66_2242660
	ldr r1, _022425EC // =0x02259100
	str r0, [r6]
	ldrb r0, [r1]
	add sp, sp, #0xc
	strb r0, [r6, #4]
	ldmia sp!, {r3, r4, r5, r6, pc}
_022425D4:
	ldr r0, _022425EC // =0x02259100
	str r3, [r6]
	ldrb r0, [r0, #1]
	strb r0, [r6, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_022425EC: .word 0x02259100
	arm_func_end ov66_2242550

	arm_func_start ov66_22425F0
ov66_22425F0: // 0x022425F0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #8]
	mov r6, r0
	cmp r4, r1
	bne _02242620
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #8]
_02242620:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	cmp r4, #0
	beq _02242648
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02242648:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov66_22425F0

	arm_func_start ov66_2242660
ov66_2242660: // 0x02242660
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	mov r8, r0
	ldr r4, [r8]
	mvn r0, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	cmp r4, r0
	bne _02242694
	ldr r0, _02242734 // =0x0225910C
	bl printf
	bl abort
_02242694:
	mov r1, #0
	mov r0, #0x10
	strb r1, [sp]
	bl FUN_02246290
	mov r4, r0
	add r0, r8, #4
	str r0, [sp, #0xc]
	adds r1, r4, #0xc
	ldrne r0, [sp, #0x30]
	str r4, [sp, #0x10]
	ldrne r0, [r0]
	str r4, [sp, #4]
	strne r0, [r1]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r7, [r0]
	cmp r6, #0
	strne r4, [r7]
	streq r4, [r7, #4]
	ldr r1, [r8]
	mov r0, r4
	add r1, r1, #1
	str r1, [r8]
	ldr r1, [r8, #4]
	str r4, [sp, #8]
	bl ov66_223ADD8
	cmp r5, #0
	ldr r1, [sp, #0x10]
	strne r4, [r8, #8]
	cmp r1, #0
	beq _02242728
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_02242728:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02242734: .word 0x0225910C
	arm_func_end ov66_2242660

	arm_func_start ov66_2242738
ov66_2242738: // 0x02242738
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	mov r2, #0
	mov r5, r0
	str r2, [sp, #4]
	str r2, [sp, #8]
	str r2, [sp, #0xc]
	strb r2, [sp, #2]
	sub r0, sp, #4
	strb r2, [r0]
	ldr r2, [r0]
	add r0, sp, #4
	bl ov66_224290C
	ldr r4, [r5]
	ldr r0, [r5, #4]
	mov ip, #0
	add r0, r4, r0, lsl #2
	sub r1, r0, r4
	mov r0, r1, asr #1
	add r6, r1, r0, lsr #30
	mov r0, r6, asr #2
	mov r7, r0, lsl #2
	ldr r3, [sp, #4]
	ldr r0, [sp, #8]
	mov r1, r4
	mov r2, r7
	add r0, r3, r0, lsl #2
	strb ip, [sp, #1]
	bl memcpy
	mov r0, r4
	mov r2, r7
	mov r1, #0
	bl memset
	ldr r1, [sp, #8]
	mov r0, #0
	add r1, r1, r6, asr #2
	str r1, [sp, #8]
	strb r0, [sp]
	str r0, [r5, #4]
	add r3, sp, #0xc
	ldr r2, [r5, #8]
	ldr r1, [r3]
	add r0, sp, #4
	str r1, [r5, #8]
	str r2, [r3]
	ldr r2, [r5]
	ldr r1, [sp, #4]
	str r1, [r5]
	str r2, [sp, #4]
	ldr r2, [r5, #4]
	ldr r1, [sp, #8]
	str r1, [r5, #4]
	str r2, [sp, #8]
	bl ov66_22428BC
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2242738

	arm_func_start ov66_2242818
ov66_2242818: // 0x02242818
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r2, #0
	mov r5, r0
	mov r1, #1
	strb r2, [sp]
	bl ov66_224293C
	mov r0, r5
	mov r2, r4
	mov r1, #1
	bl ov66_2242A08
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_2242818

	arm_func_start ov66_2242848
ov66_2242848: // 0x02242848
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_0224287C:
	ldr r4, [r1]
	ldr r0, [r6, #0xc]
	ldr r5, [r4, #0x2c]
	ldr r4, [r0, #0x2c]
	mov r0, r6
	cmp r5, r4
	ldrlt r6, [r6]
	strltb lr, [r3]
	blt _022428B0
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_022428B0:
	cmp r6, #0
	bne _0224287C
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2242848

	arm_func_start ov66_22428BC
ov66_22428BC: // 0x022428BC
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _02242900
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _02242900
	mov r2, r0
	bl DWC_Free
_02242900:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_22428BC

	arm_func_start ov66_224290C
ov66_224290C: // 0x0224290C
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mvn r1, #0xc0000000
	mov r5, r0
	cmp r4, r1
	bls _02242928
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02242928:
	mov r0, r4, lsl #2
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_224290C

	arm_func_start ov66_224293C
ov66_224293C: // 0x0224293C
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0xc0000000
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _02242964
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02242964:
	ldr r0, _02242A00 // =0x15555555
	cmp r4, r0
	bhs _022429B0
	add r1, r4, #1
	ldr r0, _02242A04 // =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_022429B0:
	cmp r4, r0, lsl #1
	bhs _022429EC
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_022429EC:
	mvn r0, #0xc0000000
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_02242A00: .word 0x15555555
_02242A04: .word 0xCCCCCCCD
	arm_func_end ov66_224293C

	arm_func_start ov66_2242A08
ov66_2242A08: // 0x02242A08
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	mov r4, #0
	mov r6, r0
	add r3, r6, #8
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	str r4, [sp, #0x10]
	str r4, [sp, #0x18]
	str r3, [sp, #0x14]
	mov r5, r1
	ldr r3, [r6, #4]
	ldr r1, [r6, #8]
	add r3, r3, r5
	sub r1, r3, r1
	mov r4, r2
	bl ov66_224293C
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov66_2242B88
	ldr r3, [r6, #4]
	ldr r1, [sp, #8]
	mov r2, #0
	ldr r0, [sp, #0xc]
	add r1, r1, r3, lsl #2
	mov ip, r5
	str r3, [sp, #0x18]
	strb r2, [sp, #2]
	cmp r5, #0
	add r1, r1, r0, lsl #2
	beq _02242AAC
_02242A9C:
	ldr r0, [r4]
	subs ip, ip, #1
	str r0, [r1], #4
	bne _02242A9C
_02242AAC:
	ldr r0, [sp, #0xc]
	ldr r1, [sp, #0x18]
	add r0, r0, r5
	str r0, [sp, #0xc]
	ldr r4, [r6]
	ldr r0, [r6, #4]
	mov ip, #0
	add r0, r4, r0, lsl #2
	sub r2, r0, r4
	mov r0, r2, asr #1
	add r5, r2, r0, lsr #30
	sub r3, r1, r5, asr #2
	mov r2, r5, asr #2
	ldr r0, [sp, #8]
	mov r7, r2, lsl #2
	mov r1, r4
	mov r2, r7
	add r0, r0, r3, lsl #2
	strb ip, [sp, #1]
	str r3, [sp, #0x18]
	bl memcpy
	mov r0, r4
	mov r2, r7
	mov r1, #0
	bl memset
	ldr r1, [sp, #0xc]
	mov r0, #0
	add r1, r1, r5, asr #2
	str r1, [sp, #0xc]
	strb r0, [sp]
	str r0, [r6, #4]
	add r3, sp, #0x10
	ldr r2, [r6, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r6, #8]
	str r2, [r3]
	ldr r2, [r6]
	ldr r1, [sp, #8]
	str r1, [r6]
	str r2, [sp, #8]
	ldr r2, [r6, #4]
	ldr r1, [sp, #0xc]
	str r1, [r6, #4]
	str r2, [sp, #0xc]
	bl ov66_2242B6C
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov66_2242A08

	arm_func_start ov66_2242B6C
ov66_2242B6C: // 0x02242B6C
	stmfd sp!, {r4, lr}
	mov r4, r0
	mov r1, #0
	str r1, [r4, #4]
	bl ov66_2242BB8
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov66_2242B6C

	arm_func_start ov66_2242B88
ov66_2242B88: // 0x02242B88
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mvn r1, #0xc0000000
	mov r5, r0
	cmp r4, r1
	bls _02242BA4
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02242BA4:
	mov r0, r4, lsl #2
	bl FUN_02246290
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov66_2242B88

	arm_func_start ov66_2242BB8
ov66_2242BB8: // 0x02242BB8
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _02242BFC
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _02242BFC
	mov r2, r0
	bl DWC_Free
_02242BFC:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov66_2242BB8

	arm_func_start ov66_2242C08
ov66_2242C08: // 0x02242C08
	ldr r0, _02242C20 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.align 2, 0
_02242C20: .word 0x0225B6DC
	arm_func_end ov66_2242C08

	arm_func_start ov66_2242C24
ov66_2242C24: // 0x02242C24
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr r0, _02242CDC // =0x0225B6DC
	ldr r3, [r0]
	cmp r3, #0
	beq _02242C4C
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_02242C4C:
	mov r2, #0
	add r1, sp, #0
	add r0, r3, #0x9c
	str r2, [sp]
	bl ov66_2236CA0
	ldr r0, _02242CDC // =0x0225B6DC
	ldr r4, [r0]
	ldr r1, [r4, #4]
	cmp r1, #4
	bne _02242CB8
	cmp r4, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	beq _02242CA4
	mov r0, r4
	bl FUN_02246754
	cmp r4, #0
	beq _02242CA4
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02242CA4:
	ldr r0, _02242CDC // =0x0225B6DC
	mov r1, #0
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_02242CB8:
	mov r1, #3
	str r1, [r4, #0x58]
	mov r1, #5
	str r1, [r4, #4]
	ldr r0, [r0]
	mov r1, #1
	str r1, [r0, #0x60]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_02242CDC: .word 0x0225B6DC
	arm_func_end ov66_2242C24

	arm_func_start ov66_2242CE0
ov66_2242CE0: // 0x02242CE0
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x78
	ldr r0, _02242EF8 // =0x0225B6DC
	mov r4, r1
	ldr r0, [r0]
	mov r5, r2
	mov r7, r3
	cmp r0, #0
	beq _02242D14
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x78
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_02242D14:
	ldr r6, _02242EFC // =0x022591C0
	add r0, sp, #0x18
	mov r1, r4
	bl ov66_223BA6C
	add r0, sp, #0x24
	mov r1, r6
	bl ov66_223BA6C
	add r0, sp, #0x18
	add r1, sp, #0x24
	mov r2, #6
	bl ov66_224154C
	mov r6, r0
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0x18
	bl ov66_223BADC
	cmp r6, #0
	bne _02242E80
	ldr r4, _02242F00 // =0x022591C8
	add r0, sp, #0
	mov r1, r5
	bl ov66_223BA6C
	add r0, sp, #0xc
	mov r1, r4
	bl ov66_223BA6C
	add r0, sp, #0
	add r1, sp, #0xc
	mov r2, #0xc
	bl ov66_224154C
	mov r4, r0
	add r0, sp, #0xc
	bl ov66_223BADC
	add r0, sp, #0
	bl ov66_223BADC
	cmp r4, #0
	addne sp, sp, #0x78
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x60
	mov r1, r5
	bl ov66_223BA6C
	ldr r1, _02242F04 // =0x022591D8
	add r0, sp, #0x54
	bl ov66_223BA6C
	add r0, sp, #0x6c
	add r1, sp, #0x60
	add r2, sp, #0x54
	mov r3, #0
	bl ov66_2242F08
	add r0, sp, #0x54
	bl ov66_223BADC
	add r0, sp, #0x60
	bl ov66_223BADC
	ldr r0, [sp, #0x70]
	cmp r0, #0
	bne _02242E00
	add r0, sp, #0x6c
	bl ov66_223B578
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02242E00:
	ldr r1, [sp, #0x70]
	mov r0, #0xc
	sub r1, r1, #1
	mul r2, r1, r0
	ldr r1, [sp, #0x6c]
	ldr r0, [r1, r2]
	add r1, r1, r2
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #1
	ldrne r0, [r1, #8]
	bl atoi
	movs r4, r0
	bne _02242E48
	add r0, sp, #0x6c
	bl ov66_223B578
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02242E48:
	ldr r0, _02242EF8 // =0x0225B6DC
	add r3, sp, #0x48
	ldr r0, [r0]
	add r0, r0, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r1, [sp, #0x90]
	ldr r2, [sp, #0x4c]
	mov r0, r4
	blx r2
	add r0, sp, #0x6c
	bl ov66_223B578
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02242E80:
	cmp r7, #3
	addne sp, sp, #0x78
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x3c
	mov r1, r4
	bl ov66_223BA6C
	ldr r0, _02242EF8 // =0x0225B6DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	bl FUN_022474F4
	mov r4, r0
	add r0, sp, #0x3c
	bl ov66_223BADC
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x78
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _02242EF8 // =0x0225B6DC
	add r3, sp, #0x30
	ldr r0, [r0]
	add r0, r0, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r2, [sp, #0x90]
	ldr r3, [sp, #0x30]
	mov r0, r4
	mov r1, r5
	blx r3
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02242EF8: .word 0x0225B6DC
_02242EFC: .word 0x022591C0
_02242F00: .word 0x022591C8
_02242F04: .word 0x022591D8
	arm_func_end ov66_2242CE0

	arm_func_start ov66_2242F08
ov66_2242F08: // 0x02242F08
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sl, r0
	mov r4, #0
	str r4, [sl]
	str r4, [sl, #4]
	add r0, sp, #0x24
	str r2, [sp]
	str r3, [sp, #4]
	str r4, [sl, #8]
	mov r8, #1
	bl ov66_223B938
	sub r5, sp, #4
	add r6, sp, #0x24
	add r7, sp, #0x18
	add fp, sp, #0xc
	mvn r4, #0
	b _0224300C
_02242F50:
	cmp sb, #0
	beq _02242FE4
	mov r0, r7
	mov r1, r6
	mov r2, #0
	mov r3, sb
	bl ov66_223A594
	mov r1, #0
	add r0, sp, #0xb
	strb r1, [r0]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _02242FB4
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _02242FA4
	mov r1, r7
	bl ov66_223B938
_02242FA4:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _02242FD8
_02242FB4:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #0xa]
	mov r2, r2
	strb r2, [r5]
	ldr r2, [r5]
	mov r0, sl
	mov r1, r7
	bl ov66_223B548
_02242FD8:
	mov r0, r7
	bl ov66_223BADC
	add r8, r8, #1
_02242FE4:
	add r2, sb, #1
	mov r0, fp
	mov r1, r6
	mov r3, r4
	bl ov66_223A594
	mov r0, r6
	mov r1, fp
	bl ov66_223BC5C
	mov r0, fp
	bl ov66_223BADC
_0224300C:
	ldr r0, [sp, #4]
	cmp r8, r0
	beq _02243034
	ldr r1, [sp]
	mov r0, r6
	mov r2, #0
	bl FUN_02246150
	mov sb, r0
	cmp sb, r4
	bne _02242F50
_02243034:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sp, #0x28]
	bne _02243054
	ldrb r0, [sp, #0x24]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02243054:
	cmp r0, #0
	beq _022430C0
	mov r2, #0
	strb r2, [sp, #9]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _022430A0
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _02243090
	add r1, sp, #0x24
	bl ov66_223B938
_02243090:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _022430C0
_022430A0:
	strb r2, [sp, #8]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	add r1, sp, #0x24
	mov r0, sl
	bl ov66_223B548
_022430C0:
	add r0, sp, #0x24
	bl ov66_223BADC
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov66_2242F08

	arm_func_start ov66_22430D0
ov66_22430D0: // 0x022430D0
	ldr r0, _022430E8 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.align 2, 0
_022430E8: .word 0x0225B6DC
	arm_func_end ov66_22430D0

	arm_func_start ov66_22430EC
ov66_22430EC: // 0x022430EC
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	ldr r0, _022431DC // =0x0225B6DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r3
	cmp r0, #0
	beq _0224311C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x30
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0224311C:
	ldr r0, [sp, #0x48]
	cmp r0, #3
	addne sp, sp, #0x30
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x24
	mov r1, r2
	bl ov66_223BA6C
	ldr r0, _022431DC // =0x0225B6DC
	add r1, sp, #0x24
	ldr r0, [r0]
	bl FUN_022474F4
	mov r4, r0
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0x18
	mov r1, r7
	bl ov66_223BA6C
	ldr r0, _022431DC // =0x0225B6DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl FUN_02246B38
	mov r5, r0
	add r0, sp, #0x18
	bl ov66_223BADC
	mov r1, r7
	add r0, sp, #0xc
	bl ov66_223BA6C
	ldr r1, _022431DC // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467D4
	ldr r0, [sp]
	ldr r3, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r5, [r5, #8]
	mov r1, r4
	mov r2, r6
	blx r5
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022431DC: .word 0x0225B6DC
	arm_func_end ov66_22430EC

	arm_func_start ov66_22431E0
ov66_22431E0: // 0x022431E0
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _02243230 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	beq _02243208
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {pc}
_02243208:
	add r0, sp, #0
	bl ov66_223BA6C
	ldr r0, _02243230 // =0x0225B6DC
	add r1, sp, #0
	ldr r0, [r0]
	bl FUN_02247258
	add r0, sp, #0
	bl ov66_223BADC
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_02243230: .word 0x0225B6DC
	arm_func_end ov66_22431E0

	arm_func_start ov66_2243234
ov66_2243234: // 0x02243234
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0xa8
	ldr r0, _022434CC // =0x0225B6DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	cmp r0, #0
	beq _02243264
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xa8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_02243264:
	add r0, sp, #0x90
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x90
	ldr r0, [r0]
	bl FUN_02247704
	mov r6, r0
	add r0, sp, #0x90
	bl ov66_223BADC
	cmp r6, #0
	beq _022432B4
	ldr r0, _022434CC // =0x0225B6DC
	mov r1, r5
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_02254E58
	cmp r0, r6
	addgt sp, sp, #0xa8
	ldmgtia sp!, {r3, r4, r5, r6, r7, pc}
_022432B4:
	add r0, sp, #0x84
	mov r1, r4
	bl ov66_223BA6C
	add r0, sp, #0x84
	bl FUN_02246928
	ldr r1, _022434CC // =0x0225B6DC
	mov r2, r0
	ldr r1, [r1]
	add r0, sp, #0x9c
	bl FUN_02247578
	add r0, sp, #0x84
	bl ov66_223BADC
	add r0, sp, #0x78
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x9c
	ldr r0, [r0]
	add r2, sp, #0x78
	bl FUN_0224717C
	mov r6, r0
	add r0, sp, #0x78
	bl ov66_223BADC
	cmp r6, #0
	beq _022433B4
	add r0, sp, #0x6c
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x6c
	ldr r0, [r0]
	bl FUN_02246B38
	mov r6, r0
	add r0, sp, #0x6c
	bl ov66_223BADC
	add r0, sp, #0x60
	mov r1, r5
	bl ov66_223BA6C
	ldr r1, _022434CC // =0x0225B6DC
	add r0, sp, #0x54
	ldr r1, [r1]
	add r2, sp, #0x60
	bl FUN_022467D4
	add r0, sp, #0x48
	mov r1, r4
	bl ov66_223BA6C
	ldr r1, [sp, #0x54]
	add r0, sp, #0x48
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	addeq r7, sp, #0x55
	ldrne r7, [sp, #0x5c]
	bl FUN_02246928
	mov r1, r0
	ldr r2, [r6, #0x14]
	ldr r3, [r6, #4]
	mov r0, r7
	blx r3
	add r0, sp, #0x48
	bl ov66_223BADC
	add r0, sp, #0x54
	bl ov66_223BADC
	add r0, sp, #0x60
	bl ov66_223BADC
_022433B4:
	add r0, sp, #0x3c
	mov r1, r4
	bl ov66_223BA6C
	add r0, sp, #0x30
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r2, sp, #0x30
	bl FUN_02246C2C
	mov r4, r0
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x3c
	bl ov66_223BADC
	mvn r0, #0
	cmp r4, r0
	bne _02243410
	add r0, sp, #0x9c
	bl ov66_223BADC
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02243410:
	add r0, sp, #0x24
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x24
	ldr r0, [r0]
	bl FUN_02247690
	mov r6, r0
	add r0, sp, #0x24
	bl ov66_223BADC
	cmp r6, #0
	beq _022434BC
	add r0, sp, #0x18
	mov r1, r5
	bl ov66_223BA6C
	ldr r0, _022434CC // =0x0225B6DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl FUN_02246B38
	mov r6, r0
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0xc
	mov r1, r5
	bl ov66_223BA6C
	ldr r1, _022434CC // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467D4
	ldr r0, [sp]
	ldr r2, [r6, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r3, [r6]
	mov r1, r4
	blx r3
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
_022434BC:
	add r0, sp, #0x9c
	bl ov66_223BADC
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022434CC: .word 0x0225B6DC
	arm_func_end ov66_2243234

	arm_func_start ov66_22434D0
ov66_22434D0: // 0x022434D0
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x48
	ldr r0, _022435F8 // =0x0225B6DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r2
	cmp r0, #0
	beq _02243500
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_02243500:
	add r0, sp, #0x3c
	mov r1, r6
	bl ov66_223BA6C
	ldr r0, _022435F8 // =0x0225B6DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	bl FUN_022474F4
	mov r4, r0
	add r0, sp, #0x3c
	bl ov66_223BADC
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x30
	mov r1, r7
	bl ov66_223BA6C
	ldr r0, _022435F8 // =0x0225B6DC
	add r1, sp, #0x30
	ldr r0, [r0]
	bl FUN_02246B38
	mov r5, r0
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x24
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _022435F8 // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl FUN_022467D4
	ldr r0, [sp, #0x18]
	ldr r2, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	ldr r3, [r5, #4]
	mov r1, r4
	blx r3
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0xc
	mov r1, r6
	bl ov66_223BA6C
	add r0, sp, #0
	mov r1, r7
	bl ov66_223BA6C
	ldr r0, _022435F8 // =0x0225B6DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r2, sp, #0
	bl FUN_0224717C
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022435F8: .word 0x0225B6DC
	arm_func_end ov66_22434D0

	arm_func_start ov66_22435FC
ov66_22435FC: // 0x022435FC
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r0, _0224366C // =0x0225B6DC
	mov r4, r3
	ldr r0, [r0]
	cmp r0, #0
	beq _02243628
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x18
	ldmeqia sp!, {r4, pc}
_02243628:
	add r0, sp, #0xc
	mov r1, r2
	bl ov66_223BA6C
	add r0, sp, #0
	mov r1, r4
	bl ov66_223BA6C
	ldr r0, _0224366C // =0x0225B6DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r2, sp, #0
	bl FUN_022475DC
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224366C: .word 0x0225B6DC
	arm_func_end ov66_22435FC

	arm_func_start ov66_2243670
ov66_2243670: // 0x02243670
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _0224372C // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	beq _02243698
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {pc}
_02243698:
	mov r2, #0
	add r1, sp, #0
	mov r0, r2
_022436A4:
	str r0, [r1, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #3
	blo _022436A4
	cmp r3, #3
	addls pc, pc, r3, lsl #2
	b _02243710
_022436C0: // jump table
	b _022436D0 // case 0
	b _022436E0 // case 1
	b _022436F0 // case 2
	b _02243700 // case 3
_022436D0:
	ldr r1, _02243730 // =0x022591DC
	add r0, sp, #0
	bl ov66_22419DC
	b _0224371C
_022436E0:
	ldr r1, _02243734 // =0x022591E4
	add r0, sp, #0
	bl ov66_22419DC
	b _0224371C
_022436F0:
	ldr r1, _02243738 // =0x022591EC
	add r0, sp, #0
	bl ov66_22419DC
	b _0224371C
_02243700:
	ldr r1, _0224373C // =0x022591F0
	add r0, sp, #0
	bl ov66_22419DC
	b _0224371C
_02243710:
	ldr r1, _02243740 // =0x022591FC
	add r0, sp, #0
	bl ov66_22419DC
_0224371C:
	add r0, sp, #0
	bl ov66_223BADC
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_0224372C: .word 0x0225B6DC
_02243730: .word 0x022591DC
_02243734: .word 0x022591E4
_02243738: .word 0x022591EC
_0224373C: .word 0x022591F0
_02243740: .word 0x022591FC
	arm_func_end ov66_2243670

	arm_func_start ov66_2243744
ov66_2243744: // 0x02243744
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x24
	ldr r0, _022437F8 // =0x0225B6DC
	mov r6, r1
	ldr r0, [r0]
	mov r4, r2
	cmp r0, #0
	beq _02243774
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x24
	ldmeqia sp!, {r3, r4, r5, r6, pc}
_02243774:
	add r0, sp, #0x18
	mov r1, r6
	bl ov66_223BA6C
	ldr r0, _022437F8 // =0x0225B6DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl FUN_02246B38
	mov r5, r0
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0xc
	mov r1, r6
	bl ov66_223BA6C
	ldr r1, _022437F8 // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467D4
	ldr r0, [sp]
	ldr r2, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r3, [r5, #0x10]
	mov r1, r4
	blx r3
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_022437F8: .word 0x0225B6DC
	arm_func_end ov66_2243744

	arm_func_start ov66_22437FC
ov66_22437FC: // 0x022437FC
	ldr r0, _02243814 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.align 2, 0
_02243814: .word 0x0225B6DC
	arm_func_end ov66_22437FC

	arm_func_start ov66_2243818
ov66_2243818: // 0x02243818
	ldr r0, _02243830 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.align 2, 0
_02243830: .word 0x0225B6DC
	arm_func_end ov66_2243818

	arm_func_start ov66_2243834
ov66_2243834: // 0x02243834
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr r0, _022438C8 // =0x0225B6DC
	mov r4, r1
	ldr r3, [r0]
	cmp r3, #0
	beq _02243860
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_02243860:
	mov r2, #0
	add r1, sp, #0
	add r0, r3, #0x9c
	str r2, [sp]
	bl ov66_2236CA0
	cmp r4, #0
	bne _022438A8
	ldr r0, _022438C8 // =0x0225B6DC
	mov r2, #3
	ldr r3, [r0]
	mov r1, #5
	str r2, [r3, #0x58]
	str r1, [r3, #4]
	ldr r0, [r0]
	mov r1, #1
	str r1, [r0, #0x60]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_022438A8:
	ldr r0, _022438C8 // =0x0225B6DC
	ldr r1, [r0]
	ldr r0, [r1, #4]
	cmp r0, #5
	movne r0, #2
	strne r0, [r1, #4]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_022438C8: .word 0x0225B6DC
	arm_func_end ov66_2243834

	arm_func_start ov66_22438CC
ov66_22438CC: // 0x022438CC
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r0, _02243930 // =0x0225B6DC
	mov r4, r2
	ldr r1, [r0]
	cmp r1, #0
	beq _022438F8
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, pc}
_022438F8:
	add r0, sp, #0
	bl ov66_2243934
	ldr r0, [sp]
	mov r2, #0x7f
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #1
	ldrne r1, [sp, #8]
	mov r0, r4
	bl strncpy
	add r0, sp, #0
	bl ov66_223BADC
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_02243930: .word 0x0225B6DC
	arm_func_end ov66_22438CC

	arm_func_start ov66_2243934
ov66_2243934: // 0x02243934
	ldr ip, _02243940 // =ov66_223B938
	add r1, r1, #0x18
	bx ip
	.align 2, 0
_02243940: .word ov66_223B938
	arm_func_end ov66_2243934

	arm_func_start ov66_2243944
ov66_2243944: // 0x02243944
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x60
	ldr r2, _02243B5C // =0x0225B6DC
	mov r4, r0
	ldr r0, [r2]
	cmp r0, #0
	beq _02243970
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
_02243970:
	cmp r1, #1
	bne _022439E4
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x54
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl FUN_022468DC
	ldr r0, _02243B5C // =0x0225B6DC
	add r1, sp, #0x54
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov66_223BC5C
	add r0, sp, #0x54
	bl ov66_223BADC
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x48
	ldr r1, [r1]
	bl ov66_2243B64
	ldr r0, [sp, #0x48]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x49
	ldrne r1, [sp, #0x50]
	mov r0, r4
	bl FUN_02254258
	add r0, sp, #0x48
	bl ov66_223BADC
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_022439E4:
	cmp r1, #2
	bne _02243A58
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x3c
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl FUN_022468DC
	ldr r0, _02243B5C // =0x0225B6DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov66_223BC5C
	add r0, sp, #0x3c
	bl ov66_223BADC
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x30
	ldr r1, [r1]
	bl ov66_2243B64
	ldr r0, [sp, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x31
	ldrne r1, [sp, #0x38]
	mov r0, r4
	bl FUN_02254258
	add r0, sp, #0x30
	bl ov66_223BADC
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_02243A58:
	sub r0, r1, #3
	cmp r0, #1
	bhi _02243AD8
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x24
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl FUN_022468DC
	ldr r0, _02243B5C // =0x0225B6DC
	add r1, sp, #0x24
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov66_223BC5C
	add r0, sp, #0x24
	bl ov66_223BADC
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	bl ov66_2243B64
	ldr r0, [sp, #0x18]
	ldr r3, _02243B60 // =0x02259204
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r2, sp, #0x19
	ldrne r2, [sp, #0x20]
	mov r0, r4
	mov r1, #2
	bl FUN_02254320
	add r0, sp, #0x18
	bl ov66_223BADC
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_02243AD8:
	cmp r1, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
	cmp r1, #6
	addne sp, sp, #0x60
	ldmneia sp!, {r4, pc}
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0xc
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl FUN_022468DC
	ldr r0, _02243B5C // =0x0225B6DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov66_223BC5C
	add r0, sp, #0xc
	bl ov66_223BADC
	ldr r1, _02243B5C // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	bl ov66_2243B64
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #1
	ldrne r1, [sp, #8]
	mov r0, r4
	bl FUN_02254258
	add r0, sp, #0
	bl ov66_223BADC
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
	.align 2, 0
_02243B5C: .word 0x0225B6DC
_02243B60: .word 0x02259204
	arm_func_end ov66_2243944

	arm_func_start ov66_2243B64
ov66_2243B64: // 0x02243B64
	ldr ip, _02243B70 // =ov66_223B938
	add r1, r1, #0xc
	bx ip
	.align 2, 0
_02243B70: .word ov66_223B938
	arm_func_end ov66_2243B64

	arm_func_start ov66_2243B74
ov66_2243B74: // 0x02243B74
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x28
	ldr r0, _02243C6C // =0x0225B6DC
	mov r6, r1
	ldr r1, [r0]
	mov r7, r2
	cmp r1, #0
	beq _02243BA4
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02243BA4:
	add r0, sp, #0x1c
	mov r2, r7
	bl FUN_0224680C
	ldr r0, _02243C6C // =0x0225B6DC
	ldr r5, [sp, #0x50]
	ldr r4, [r0]
	ldr ip, _02243C70 // =0x02259138
	add r3, sp, #0x10
	mov r8, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	str ip, [sp, #0x10]
	str r5, [sp, #0x14]
	str r8, [sp, #0x18]
	str r3, [sp, #8]
	bl ov66_2243C94
	ldr r1, [sp, #0xc]
	add r0, r4, #0x94
	cmp r1, r0
	moveq sb, r8
	ldrne sb, [r1, #0xc]
	cmp r6, #1
	moveq r6, #1
	add r0, sp, #0x1c
	ldr r4, [sb, #0xc]
	movne r6, #0
	bl ov66_2243C7C
	mov r8, r0
	mov r0, sb
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	mov r1, r7
	ldr r7, [sp, #0x4c]
	ldr r3, [sp, #0x48]
	str r7, [sp]
	str r0, [sp, #4]
	mov r2, r8
	mov r0, r6
	blx r4
	ldr r0, _02243C6C // =0x0225B6DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	add r0, sp, #0x1c
	bl ov66_223B5D0
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_02243C6C: .word 0x0225B6DC
_02243C70: .word 0x02259138
	arm_func_end ov66_2243B74

	arm_func_start ov66_2243C74
ov66_2243C74: // 0x02243C74
	ldr r0, [r0, #8]
	bx lr
	arm_func_end ov66_2243C74

	arm_func_start ov66_2243C7C
ov66_2243C7C: // 0x02243C7C
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov66_2243C7C

	arm_func_start ov66_2243C90
ov66_2243C90: // 0x02243C90
	bx lr
	arm_func_end ov66_2243C90

	arm_func_start ov66_2243C94
ov66_2243C94: // 0x02243C94
	stmfd sp!, {r4, r5, r6, lr}
	mov r4, r1
	mov r6, r2
	mov r5, r0
	ldr r2, [r4, #4]
	mov r0, r4
	mov r1, r6
	add r3, r4, #4
	bl ov66_2243CF0
	add r1, r4, #4
	cmp r0, r1
	beq _02243CDC
	ldr r2, [r6]
	ldr r1, [r0, #0xc]
	ldr r2, [r2, #4]
	ldr r1, [r1, #4]
	cmp r2, r1
	bhs _02243CE8
_02243CDC:
	add r0, r4, #4
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, pc}
_02243CE8:
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov66_2243C94

	arm_func_start ov66_2243CF0
ov66_2243CF0: // 0x02243CF0
	cmp r2, #0
	beq _02243D20
	ldr r0, [r1]
	ldr r1, [r0, #4]
_02243D00:
	ldr r0, [r2, #0xc]
	ldr r0, [r0, #4]
	cmp r0, r1
	movhs r3, r2
	ldrhs r2, [r2]
	ldrlo r2, [r2, #4]
	cmp r2, #0
	bne _02243D00
_02243D20:
	mov r0, r3
	bx lr
	arm_func_end ov66_2243CF0

	arm_func_start ov66_2243D28
ov66_2243D28: // 0x02243D28
	cmp r1, #0
	bx lr
	arm_func_end ov66_2243D28

	arm_func_start ov66_2243D30
ov66_2243D30: // 0x02243D30
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x58
	ldr r0, _02243EE8 // =0x0225B6DC
	mov r4, r1
	ldr r0, [r0]
	mov r6, r2
	mov r5, r3
	cmp r0, #0
	beq _02243D64
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x58
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02243D64:
	mov r0, #0
	str r0, [sp, #0x4c]
	str r0, [sp, #0x50]
	str r0, [sp, #0x54]
	cmp r5, #0
	beq _02243DEC
	add r0, sp, #0x40
	mov r1, r6
	bl ov66_223BA6C
	add r1, sp, #0x40
	ldr r0, _02243EE8 // =0x0225B6DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x78]
	add r0, sp, #0x34
	mov r2, r5
	bl FUN_02246DF0
	add r0, sp, #0x4c
	add r1, sp, #0x34
	cmp r0, r1
	beq _02243DDC
	mov r3, #0
	ldr r1, [sp, #0x34]
	ldr r5, [sp, #0x38]
	strb r3, [sp, #4]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r5, lsl #2
	bl ov66_2243EF0
_02243DDC:
	add r0, sp, #0x34
	bl ov66_223B5D0
	add r0, sp, #0x40
	bl ov66_223BADC
_02243DEC:
	ldr r0, _02243EE8 // =0x0225B6DC
	ldr r8, [sp, #0x80]
	ldr r5, [r0]
	ldr sb, _02243EEC // =0x02259138
	add r3, sp, #0x10
	mov r7, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r5, #0x90
	str sb, [sp, #0x10]
	str r8, [sp, #0x14]
	str r7, [sp, #0x18]
	str r3, [sp, #8]
	bl ov66_2243C94
	ldr r1, [sp, #0xc]
	add r0, r5, #0x94
	cmp r1, r0
	moveq r5, r7
	ldrne r5, [r1, #0xc]
	add r0, sp, #0x28
	mov r1, r6
	bl ov66_223BA6C
	ldr r1, _02243EE8 // =0x0225B6DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl FUN_022467D4
	ldr r0, [sp, #0x1c]
	ldr r6, [sp, #0x50]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r4, #1
	moveq sl, #1
	add r0, sp, #0x4c
	ldr r7, [r5, #0xc]
	movne sl, #0
	bl ov66_22364B0
	mov r4, r0
	mov r0, r5
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	str r0, [sp]
	mov r1, sb
	mov r2, r6
	mov r3, r4
	mov r0, sl
	blx r7
	add r0, sp, #0x1c
	bl ov66_223BADC
	add r0, sp, #0x28
	bl ov66_223BADC
	ldr r0, _02243EE8 // =0x0225B6DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	add r0, sp, #0x4c
	bl ov66_223B5D0
	add sp, sp, #0x58
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02243EE8: .word 0x0225B6DC
_02243EEC: .word 0x02259138
	arm_func_end ov66_2243D30

	arm_func_start ov66_2243EF0
ov66_2243EF0: // 0x02243EF0
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r1
	mov r5, r2
	mov r7, r0
	sub r2, r5, r6
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	ldr r3, [r7, #8]
	mov r4, r1, asr #2
	cmp r3, r1, asr #2
	bhs _02243F3C
	mov r2, #0
	sub r1, r4, r3
	strb r2, [sp]
	bl ov66_224293C
	mov r2, r0
	mov r0, r7
	mov r1, r4
	bl FUN_022460E0
_02243F3C:
	sub r1, r5, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r2, r0, asr #2
	ldr r0, [r7]
	mov r1, r6
	mov r2, r2, lsl #2
	bl memmove
	str r4, [r7, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov66_2243EF0

	arm_func_start ov66_2243F64
ov66_2243F64: // 0x02243F64
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x6c
	ldr r0, _02244134 // =0x0225B6DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	mov r6, r3
	cmp r0, #0
	beq _02243F98
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02243F98:
	cmp r5, #1
	bne _02243FDC
	add r0, sp, #0x60
	mov r1, r4
	bl ov66_223BA6C
	add r1, sp, #0x60
	ldr r0, _02244134 // =0x0225B6DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x90]
	add r0, sp, #0x54
	mov r2, r6
	bl FUN_02246DF0
	add r0, sp, #0x54
	bl ov66_223B5D0
	add r0, sp, #0x60
	bl ov66_223BADC
_02243FDC:
	add r0, sp, #0x48
	mov r1, r4
	bl ov66_223BA6C
	ldr r0, _02244134 // =0x0225B6DC
	add r1, sp, #0x48
	ldr r0, [r0]
	bl FUN_022476C8
	add r0, sp, #0x48
	bl ov66_223BADC
	ldr r0, _02244134 // =0x0225B6DC
	ldr r8, [sp, #0x98]
	ldr r6, [r0]
	ldr r0, _02244138 // =0x02259138
	mov r1, #0
	str r0, [sp, #0xc]
	add r0, sp, #0xc
	str r1, [sp, #0x14]
	str r0, [sp, #4]
	add r0, sp, #8
	add r1, r6, #0x90
	add r2, sp, #4
	str r8, [sp, #0x10]
	bl ov66_2243C94
	ldr r1, [sp, #8]
	add r0, r6, #0x94
	cmp r1, r0
	moveq r6, #0
	ldrne r6, [r1, #0xc]
	add r0, sp, #0x3c
	mov r1, r4
	bl ov66_223BA6C
	ldr r1, _02244134 // =0x0225B6DC
	add r0, sp, #0x30
	ldr r1, [r1]
	add r2, sp, #0x3c
	bl FUN_022467D4
	ldr r0, [sp, #0x30]
	ldr r7, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x31
	ldrne sb, [sp, #0x38]
	cmp r5, #1
	moveq sl, #1
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	mov r3, r0
	mov r0, sl
	mov r2, sb
	mov r1, #0
	blx r7
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x3c
	bl ov66_223BADC
	ldr r0, _02244134 // =0x0225B6DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	cmp r5, #0
	addne sp, sp, #0x6c
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	add r0, sp, #0x24
	mov r1, r4
	bl ov66_223BA6C
	ldr r1, _02244134 // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl FUN_022467D4
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	bl FUN_022451C8
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02244134: .word 0x0225B6DC
_02244138: .word 0x02259138
	arm_func_end ov66_2243F64

	arm_func_start ov66_224413C
ov66_224413C: // 0x0224413C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x6c
	ldr r4, _02244390 // =0x0225B6DC
	mov r8, r0
	ldr r0, [r4]
	mov r5, r1
	mov r7, r2
	mov r6, r3
	cmp r0, #0
	beq _02244174
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02244174:
	cmp r5, #1
	bne _02244208
	add r0, sp, #0x60
	mov r1, r6
	bl ov66_223BA6C
	ldr r0, _02244390 // =0x0225B6DC
	add r1, sp, #0x60
	ldr r0, [r0]
	bl FUN_02247704
	mov r4, r0
	add r0, sp, #0x60
	bl ov66_223BADC
	cmp r4, #0
	beq _02244208
	ldr r0, _02244390 // =0x0225B6DC
	mov r1, r6
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_02254E58
	mvn r1, #0
	cmp r4, r1
	moveq r5, #0
	moveq r7, #7
	beq _02244208
	cmp r0, r4
	bgt _022441E4
	cmp r0, r1
	bne _022441F0
_022441E4:
	mov r5, #0
	mov r7, #2
	b _02244208
_022441F0:
	ldr r0, _02244390 // =0x0225B6DC
	mov r1, r6
	ldr r0, [r0]
	mov r2, r4
	ldr r0, [r0]
	bl FUN_02254D68
_02244208:
	cmp r5, #1
	bne _02244274
	ldr r1, _02244390 // =0x0225B6DC
	add r0, sp, #0x54
	ldr r1, [r1]
	bl ov66_2243B64
	add r0, sp, #0x48
	mov r1, r6
	bl ov66_223BA6C
	ldr r0, _02244390 // =0x0225B6DC
	add r1, sp, #0x54
	ldr r0, [r0]
	add r2, sp, #0x48
	bl FUN_02246C2C
	add r0, sp, #0x48
	bl ov66_223BADC
	add r0, sp, #0x54
	bl ov66_223BADC
	mov r2, #0
	str r2, [sp]
	ldr r2, _02244394 // =ov66_2243F64
	ldr r3, [sp, #0x88]
	mov r0, r8
	mov r1, r6
	bl FUN_02255104
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02244274:
	add r0, sp, #0x3c
	mov r1, r6
	bl ov66_223BA6C
	ldr r1, _02244390 // =0x0225B6DC
	add r0, sp, #0x30
	ldr r1, [r1]
	add r2, sp, #0x3c
	bl FUN_022467D4
	ldr r0, [sp, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	bl FUN_022451C8
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x3c
	bl ov66_223BADC
	ldr r0, _02244390 // =0x0225B6DC
	ldr r5, [sp, #0x88]
	ldr r4, [r0]
	ldr r0, _02244398 // =0x02259138
	mov r1, #0
	str r0, [sp, #0xc]
	add r0, sp, #0xc
	str r1, [sp, #0x14]
	str r0, [sp, #4]
	add r0, sp, #8
	add r1, r4, #0x90
	add r2, sp, #4
	str r5, [sp, #0x10]
	bl ov66_2243C94
	ldr r1, [sp, #8]
	add r0, r4, #0x94
	cmp r1, r0
	moveq r4, #0
	ldrne r4, [r1, #0xc]
	add r0, sp, #0x24
	mov r1, r6
	bl ov66_223BA6C
	ldr r1, _02244390 // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl FUN_022467D4
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r4
	ldr r1, [r0]
	addeq r6, sp, #0x19
	ldrne r6, [sp, #0x20]
	ldr r1, [r1, #0xc]
	ldr r4, [r4, #0xc]
	blx r1
	mov r3, r0
	mov r1, r7
	mov r2, r6
	mov r0, #0
	blx r4
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	ldr r0, _02244390 // =0x0225B6DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02244390: .word 0x0225B6DC
_02244394: .word ov66_2243F64
_02244398: .word 0x02259138
	arm_func_end ov66_224413C

	arm_func_start ov66_224439C
ov66_224439C: // 0x0224439C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x40
	ldr r0, _02244520 // =0x0225B6DC
	mov r7, r1
	ldr r0, [r0]
	mov sb, r2
	cmp r0, #0
	beq _022443CC
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022443CC:
	cmp sb, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r3, #0
	mvneq fp, #0
	beq _0224440C
	add r0, sp, #0x34
	mov r1, r3
	bl ov66_223BA6C
	ldr r0, _02244520 // =0x0225B6DC
	add r1, sp, #0x34
	ldr r0, [r0]
	bl FUN_022474F4
	mov fp, r0
	add r0, sp, #0x34
	bl ov66_223BADC
_0224440C:
	ldr r0, _02244520 // =0x0225B6DC
	ldr r8, [sp, #0x74]
	ldr r4, [r0]
	ldr r6, _02244524 // =0x02259138
	add r3, sp, #0x10
	mov r5, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	str r6, [sp, #0x10]
	str r8, [sp, #0x14]
	str r5, [sp, #0x18]
	str r3, [sp, #8]
	bl ov66_2243C94
	add r0, r4, #0x94
	ldr r1, [sp, #0xc]
	mov r4, #0
	cmp r1, r0
	moveq r6, r5
	ldrne r6, [r1, #0xc]
	mov r5, r4
	cmp r7, #1
	bne _02244484
	ldr r0, [sp, #0x68]
	cmp r0, #0
	ble _02244484
	ldr r1, [sp, #0x6c]
	ldr r0, [sp, #0x70]
	ldr r4, [r1]
	ldr r5, [r0]
_02244484:
	add r0, sp, #0x28
	mov r1, sb
	bl ov66_223BA6C
	ldr r1, _02244520 // =0x0225B6DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl FUN_022467D4
	ldr r0, [sp, #0x1c]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r7, #1
	moveq sl, #1
	ldr r7, [r6, #0xc]
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	str r5, [sp]
	str r0, [sp, #4]
	mov r0, sl
	mov r1, sb
	mov r2, fp
	mov r3, r4
	blx r7
	add r0, sp, #0x1c
	bl ov66_223BADC
	add r0, sp, #0x28
	bl ov66_223BADC
	ldr r0, _02244520 // =0x0225B6DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_02244520: .word 0x0225B6DC
_02244524: .word 0x02259138
	arm_func_end ov66_224439C

	arm_func_start ov66_2244528
ov66_2244528: // 0x02244528
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x34
	ldr r0, _0224461C // =0x0225B6DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r3
	cmp r0, #0
	beq _02244558
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x34
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02244558:
	cmp r2, #0
	mvneq r4, #0
	beq _0224458C
	add r0, sp, #0x28
	mov r1, r2
	bl ov66_223BA6C
	ldr r0, _0224461C // =0x0225B6DC
	add r1, sp, #0x28
	ldr r0, [r0]
	bl FUN_022474F4
	mov r4, r0
	add r0, sp, #0x28
	bl ov66_223BADC
_0224458C:
	add r0, sp, #0x1c
	mov r1, r7
	bl ov66_223BA6C
	ldr r0, _0224461C // =0x0225B6DC
	add r1, sp, #0x1c
	ldr r0, [r0]
	bl FUN_02246B38
	mov r5, r0
	add r0, sp, #0x1c
	bl ov66_223BADC
	add r0, sp, #0x10
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _0224461C // =0x0225B6DC
	add r0, sp, #4
	ldr r1, [r1]
	add r2, sp, #0x10
	bl FUN_022467D4
	ldr r0, [sp, #4]
	ldr r1, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r3, [sp, #0x48]
	str r1, [sp]
	ldr r5, [r5, #0xc]
	mov r1, r4
	mov r2, r6
	blx r5
	add r0, sp, #4
	bl ov66_223BADC
	add r0, sp, #0x10
	bl ov66_223BADC
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_0224461C: .word 0x0225B6DC
	arm_func_end ov66_2244528

	arm_func_start ov66_2244620
ov66_2244620: // 0x02244620
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x2c
	ldr r4, _02244750 // =0x0225B6DC
	mov r8, r1
	ldr r1, [r4]
	mov r7, r2
	mov r6, r3
	cmp r1, #0
	beq _02244654
	ldr r1, [r1, #4]
	cmp r1, #5
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02244654:
	cmp r8, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r3, [sp, #0x48]
	mov r1, r7
	mov r2, r6
	bl ov66_22437FC
	ldr r0, _02244750 // =0x0225B6DC
	ldr r5, [sp, #0x48]
	ldr r4, [r0]
	ldr lr, _02244754 // =0x02259138
	add r3, sp, #8
	mov ip, #0
	add r0, sp, #4
	add r2, sp, #0
	add r1, r4, #0x90
	str lr, [sp, #8]
	str r5, [sp, #0xc]
	str ip, [sp, #0x10]
	str r3, [sp]
	bl ov66_2243C94
	ldr r1, [sp, #4]
	add r0, r4, #0x94
	cmp r1, r0
	moveq r4, #0
	ldrne r4, [r1, #0xc]
	add r0, sp, #0x20
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _02244750 // =0x0225B6DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl FUN_022467D4
	ldr r0, [sp, #0x14]
	ldr r7, [r4, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r4
	ldr r1, [r0]
	addeq sb, sp, #0x15
	ldrne sb, [sp, #0x1c]
	cmp r8, #1
	moveq r8, #1
	ldr r1, [r1, #0xc]
	movne r8, #0
	blx r1
	mov r3, r0
	mov r0, r8
	mov r1, sb
	mov r2, r6
	blx r7
	add r0, sp, #0x14
	bl ov66_223BADC
	add r0, sp, #0x20
	bl ov66_223BADC
	ldr r0, _02244750 // =0x0225B6DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl FUN_0224640C
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_02244750: .word 0x0225B6DC
_02244754: .word 0x02259138
	arm_func_end ov66_2244620

	arm_func_start ov66_2244758
ov66_2244758: // 0x02244758
	ldr r0, _02244778 // =0x0225B6DC
	mov r1, #3
	ldr r2, [r0]
	mov r0, #5
	str r1, [r2, #0x58]
	str r0, [r2, #4]
	mov r0, #0
	bx lr
	.align 2, 0
_02244778: .word 0x0225B6DC
	arm_func_end ov66_2244758

	arm_func_start ov66_224477C
ov66_224477C: // 0x0224477C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	ldr r0, _0224495C // =0x0225B6DC
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x28
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	beq _022447B4
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022447B4:
	ldr r0, [r1]
	cmp r0, #0
	beq _022447C4
	bl FUN_0225448C
_022447C4:
	ldr r2, _0224495C // =0x0225B6DC
	add r0, sp, #0xc
	add r1, sp, #0x1c
	ldr r5, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0xc
	add r1, sp, #0x1c
	bl RTC_ConvertDateTimeToSecond
	ldr r2, [r5, #0xa8]
	add r7, r5, #0xa0
	mov r6, r0
	mov sb, r1
	str r2, [sp, #8]
	cmp r2, r7
	beq _02244894
	mov fp, #1
	mov sl, #0
	add r4, sp, #8
_0224480C:
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	subs r1, r6, r1
	ldr r8, [r2, #0x18]
	sbc r0, sb, r0
	ldr r3, [r2, #0x1c]
	subs r1, r8, r1
	sbcs r1, r3, r0
	bge _02244850
	str r6, [r2, #0x10]
	str sb, [r2, #0x14]
	ldr r0, [r2, #0x20]
	ldr r1, [r2, #0x24]
	blx r1
	cmp r0, #0
	moveq r0, sl
	beq _02244854
_02244850:
	mov r0, fp
_02244854:
	cmp r0, #0
	bne _02244880
	ldr r8, [sp, #8]
	mov r0, r4
	bl ov66_223AD6C
	add r0, sp, #4
	mov r2, r8
	add r1, r5, #0x9c
	str r8, [sp]
	bl FUN_0223AB54
	b _02244888
_02244880:
	mov r0, r4
	bl ov66_223AD6C
_02244888:
	ldr r2, [sp, #8]
	cmp r2, r7
	bne _0224480C
_02244894:
	ldr r0, _0224495C // =0x0225B6DC
	ldr r3, [r0]
	cmp r3, #0
	beq _022448B8
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022448B8:
	ldr r1, [r3, #0x54]
	ldr r0, _0224495C // =0x0225B6DC
	cmp r1, #3
	ldr r2, [r0]
	blo _022448F8
	mov r1, #0
	str r1, [r3, #0x54]
	ldr r2, [r0]
	mov r1, #3
	str r1, [r2, #0x58]
	mov r1, #5
	str r1, [r2, #4]
	ldr r0, [r0]
	add sp, sp, #0x28
	ldr r0, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022448F8:
	ldr r0, [r2, #4]
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	b _02244948
_02244908: // jump table
	b _02244948 // case 0
	b _02244948 // case 1
	b _0224491C // case 2
	b _02244948 // case 3
	b _02244948 // case 4
_0224491C:
	ldr r1, [r2, #0x5c]
	add r0, r1, #1
	str r0, [r2, #0x5c]
	cmp r1, #0x3c
	bne _02244948
	mov r0, #0
	str r0, [r2, #0x5c]
	ldr r0, [r2, #4]
	cmp r0, #5
	movne r0, #3
	strne r0, [r2, #4]
_02244948:
	ldr r0, _0224495C // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0224495C: .word 0x0225B6DC
	arm_func_end ov66_224477C

	arm_func_start ov66_2244960
ov66_2244960: // 0x02244960
	ldr r0, _02244978 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #0x58]
	moveq r0, #0
	bx lr
	.align 2, 0
_02244978: .word 0x0225B6DC
	arm_func_end ov66_2244960

	arm_func_start ov66_224497C
ov66_224497C: // 0x0224497C
	add r0, r0, #0x410
	add r0, r0, #0xa000
	bx lr
	arm_func_end ov66_224497C

	arm_func_start ov66_2244988
ov66_2244988: // 0x02244988
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r2, _02244A14 // =0x0225B6DC
	mov r1, r0
	ldr r0, [r2]
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	add r0, sp, #0xc
	bl ov66_223BA6C
	ldr r1, _02244A14 // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467C4
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _02244A14 // =0x0225B6DC
	addeq r1, sp, #1
	ldr r0, [r0]
	ldrne r1, [sp, #8]
	ldr r0, [r0]
	bl FUN_02254F20
	cmp r0, #1
	moveq r4, #1
	add r0, sp, #0
	movne r4, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.align 2, 0
_02244A14: .word 0x0225B6DC
	arm_func_end ov66_2244988

	arm_func_start ov66_2244A18
ov66_2244A18: // 0x02244A18
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x13c
	ldr r4, _02244E30 // =0x0225B6DC
	mov sl, r0
	ldr r0, [r4]
	mov sb, r1
	cmp r0, #0
	mov r8, r2
	mov r7, r3
	addne sp, sp, #0x13c
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	bl FUN_022468DC
	ldr r1, _02244E34 // =0x02259208
	add r0, sp, #0x124
	bl ov66_223BA6C
	ldr r1, _02244E38 // =0x02259210
	add r0, sp, #0x118
	bl ov66_223BA6C
	mov r4, #0
	mov r0, #0xac
	mov r5, r4
	bl FUN_02246290
	movs r6, r0
	beq _02244AE0
	add r0, sp, #0xf8
	mov r1, sb
	bl ov66_223BA6C
	add r0, sp, #0xec
	mov r1, sl
	mov r4, #1
	bl ov66_223BA6C
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	str r1, [sp]
	str r0, [sp, #4]
	add r1, sp, #0x124
	str r1, [sp, #8]
	add r0, sp, #0x118
	str r0, [sp, #0xc]
	add r1, sp, #0xec
	add r2, sp, #0xf8
	mov r3, r8
	mov r0, r6
	str r7, [sp, #0x10]
	mov r5, r4
	bl FUN_02246588
	mov r6, r0
_02244AE0:
	ldr r0, _02244E30 // =0x0225B6DC
	cmp r5, #0
	str r6, [r0]
	beq _02244AF8
	add r0, sp, #0xec
	bl ov66_223BADC
_02244AF8:
	cmp r4, #0
	beq _02244B08
	add r0, sp, #0xf8
	bl ov66_223BADC
_02244B08:
	ldr r0, _02244E30 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	bne _02244B3C
	add r0, sp, #0x118
	bl ov66_223BADC
	add r0, sp, #0x124
	bl ov66_223BADC
	add r0, sp, #0x130
	bl ov66_223BADC
	add sp, sp, #0x13c
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02244B3C:
	bl FUN_02246368
	cmp r0, #0
	moveq r4, #1
	beq _02244DC8
	add r0, sp, #0x104
	mov r1, #0
	mov r2, #0x14
	bl MI_CpuFill8
	ldr r4, _02244E3C // =ov66_2242C08
	ldr r3, _02244E40 // =ov66_2242C24
	ldr r1, _02244E44 // =ov66_22430D0
	ldr r2, _02244E48 // =ov66_2242CE0
	ldr r0, _02244E30 // =0x0225B6DC
	str r1, [sp, #0x110]
	ldr r1, [r0]
	str r4, [sp, #0x104]
	str r3, [sp, #0x108]
	str r2, [sp, #0x10c]
	ldr r2, [r7, #8]
	add r0, sp, #0xe0
	str r2, [sp, #0x114]
	bl FUN_02244E68
	ldr r2, _02244E4C // =0x02259218
	add r0, sp, #0xd4
	add r1, sp, #0xe0
	bl ov66_22379D0
	ldr r2, _02244E50 // =0x0225921C
	add r0, sp, #0xc8
	add r1, sp, #0xd4
	bl ov66_22379D0
	ldr r1, _02244E30 // =0x0225B6DC
	add r0, sp, #0xbc
	ldr r1, [r1]
	bl ov66_2243B64
	ldr r1, _02244E30 // =0x0225B6DC
	add r0, sp, #0xb0
	ldr r1, [r1]
	bl FUN_02244E78
	ldr r1, _02244E30 // =0x0225B6DC
	add r0, sp, #0xa4
	ldr r1, [r1]
	bl FUN_02244E68
	ldr r1, _02244E30 // =0x0225B6DC
	add r0, sp, #0x98
	ldr r1, [r1]
	bl FUN_02244E88
	ldr r0, [sp, #0x98]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xa4]
	addeq r1, sp, #0x99
	mov r0, r0, lsl #0x1f
	ldrne r1, [sp, #0xa0]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xb0]
	addeq r4, sp, #0xa5
	mov r0, r0, lsl #0x1f
	ldrne r4, [sp, #0xac]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xbc]
	addeq r3, sp, #0xb1
	mov r0, r0, lsl #0x1f
	ldrne r3, [sp, #0xb8]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xc8]
	addeq r2, sp, #0xbd
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0xc4]
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0xc9
	ldrne r0, [sp, #0xd0]
	str r4, [sp]
	str r1, [sp, #4]
	add r4, sp, #0x104
	str r4, [sp, #8]
	ldr r1, _02244E54 // =ov66_2243944
	ldr r4, _02244E58 // =ov66_22438CC
	str r1, [sp, #0xc]
	ldr r1, _02244E5C // =ov66_2243834
	str r4, [sp, #0x10]
	str r1, [sp, #0x14]
	ldr r4, [r7, #8]
	ldr r1, _02244E60 // =0x00001A0B
	str r4, [sp, #0x18]
	mov r4, #0
	str r4, [sp, #0x1c]
	bl FUN_022541CC
	mov r4, r0
	add r0, sp, #0x98
	bl ov66_223BADC
	add r0, sp, #0xa4
	bl ov66_223BADC
	add r0, sp, #0xb0
	bl ov66_223BADC
	add r0, sp, #0xbc
	bl ov66_223BADC
	add r0, sp, #0xc8
	bl ov66_223BADC
	add r0, sp, #0xd4
	bl ov66_223BADC
	add r0, sp, #0xe0
	bl ov66_223BADC
	cmp r4, #0
	moveq r4, #2
	beq _02244DC8
	ldr r0, _02244E30 // =0x0225B6DC
	ldr r2, _02244E30 // =0x0225B6DC
	ldr r1, [r0]
	str r4, [r1]
	ldr r1, [r0]
	ldr r0, [r1, #4]
	cmp r0, #5
	movne r0, #1
	strne r0, [r1, #4]
	add r0, sp, #0x88
	add r1, sp, #0x7c
	ldr r5, [r2]
	bl DWC_GetDateTime
	add r0, sp, #0x88
	add r1, sp, #0x7c
	bl RTC_ConvertDateTimeToSecond
	ldr r3, _02244E64 // =ov66_2244758
	str r0, [sp, #0x2c]
	str r1, [sp, #0x30]
	mov r4, #0x1e
	mov r1, #0
	add r0, sp, #0x44
	add r2, sp, #0x2c
	str r4, [sp, #0x34]
	str r1, [sp, #0x38]
	str r5, [sp, #0x3c]
	str r3, [sp, #0x40]
	bl ov66_22369FC
	mov r0, #0
	add r2, sp, #0x48
	strb r0, [sp, #0x21]
	strb r0, [sp, #0x20]
	ldr r0, [sp, #0x44]
	add r7, sp, #0x64
	str r0, [sp, #0x60]
	add r1, r5, #0x9c
	add r6, r2, #8
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0x24
	add r2, sp, #0x60
	str r4, [r7, #0x10]
	bl FUN_0223AABC
	add r0, sp, #0x118
	bl ov66_223BADC
	add r0, sp, #0x124
	bl ov66_223BADC
	add r0, sp, #0x130
	bl ov66_223BADC
	add sp, sp, #0x13c
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02244DC8:
	bl FUN_022463D8
	ldr r0, _02244E30 // =0x0225B6DC
	ldr r5, [r0]
	cmp r5, #0
	beq _02244E0C
	beq _02244E00
	mov r0, r5
	bl FUN_02246754
	cmp r5, #0
	beq _02244E00
	mov r0, #0
	mov r1, r5
	mov r2, r0
	bl DWC_Free
_02244E00:
	ldr r0, _02244E30 // =0x0225B6DC
	mov r1, #0
	str r1, [r0]
_02244E0C:
	add r0, sp, #0x118
	bl ov66_223BADC
	add r0, sp, #0x124
	bl ov66_223BADC
	add r0, sp, #0x130
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0x13c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02244E30: .word 0x0225B6DC
_02244E34: .word 0x02259208
_02244E38: .word 0x02259210
_02244E3C: .word ov66_2242C08
_02244E40: .word ov66_2242C24
_02244E44: .word ov66_22430D0
_02244E48: .word ov66_2242CE0
_02244E4C: .word 0x02259218
_02244E50: .word 0x0225921C
_02244E54: .word ov66_2243944
_02244E58: .word ov66_22438CC
_02244E5C: .word ov66_2243834
_02244E60: .word 0x00001A0B
_02244E64: .word ov66_2244758
	arm_func_end ov66_2244A18

	arm_func_start FUN_02244E68
FUN_02244E68: // 0x02244E68
	ldr ip, _02244E74 // =ov66_223B938
	add r1, r1, #0x30
	bx ip
	.align 2, 0
_02244E74: .word ov66_223B938
	arm_func_end FUN_02244E68

	arm_func_start FUN_02244E78
FUN_02244E78: // 0x02244E78
	ldr ip, _02244E84 // =ov66_223B938
	add r1, r1, #0x24
	bx ip
	.align 2, 0
_02244E84: .word ov66_223B938
	arm_func_end FUN_02244E78

	arm_func_start FUN_02244E88
FUN_02244E88: // 0x02244E88
	ldr ip, _02244E94 // =ov66_223B938
	add r1, r1, #0x3c
	bx ip
	.align 2, 0
_02244E94: .word ov66_223B938
	arm_func_end FUN_02244E88

	arm_func_start FUN_02244E98
FUN_02244E98: // 0x02244E98
	stmfd sp!, {r4, lr}
	ldr r0, _02244F40 // =0x0225B6DC
	ldr r0, [r0]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r0, [r0, #4]
	cmp r0, #4
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	bl FUN_022463D8
	ldr r0, _02244F40 // =0x0225B6DC
	ldr r2, [r0]
	ldr r1, [r2, #0x60]
	cmp r1, #0
	beq _02244F24
	ldr r0, [r2]
	bl FUN_022543DC
	ldr r0, _02244F40 // =0x0225B6DC
	ldr r4, [r0]
	cmp r4, #0
	beq _02244F38
	beq _02244F14
	mov r0, r4
	bl FUN_02246754
	cmp r4, #0
	beq _02244F14
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02244F14:
	ldr r0, _02244F40 // =0x0225B6DC
	mov r1, #0
	str r1, [r0]
	b _02244F38
_02244F24:
	mov r1, #4
	str r1, [r2, #4]
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_022543DC
_02244F38:
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_02244F40: .word 0x0225B6DC
	arm_func_end FUN_02244E98

	arm_func_start FUN_02244F44
FUN_02244F44: // 0x02244F44
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x88
	ldr r4, _02245184 // =0x0225B6DC
	mov r8, r0
	ldr r4, [r4]
	mov r7, r1
	cmp r4, #0
	mov r6, r2
	mov r5, r3
	addeq sp, sp, #0x88
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _02244F8C
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x88
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
_02244F8C:
	cmp r4, #0
	beq _02244FB0
	ldr r0, _02245184 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x88
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
_02244FB0:
	mov r0, r8
	bl strlen
	mov r0, #0x10
	bl FUN_02246290
	cmp r0, #0
	beq _02245004
	ldr r2, _02245188 // =0x02259138
	ldr r1, _0224518C // =0x0225B6E0
	str r2, [r0]
	mov ip, #0
	ldr r2, [r1]
	ldr r3, [sp, #0xa4]
	str ip, [r0, #4]
	str r3, [r0, #8]
	add ip, r2, #1
	ldr r3, _02245190 // =0x02259180
	str r2, [r0, #4]
	ldr r2, [sp, #0xa0]
	str r3, [r0]
	str ip, [r1]
	str r2, [r0, #0xc]
_02245004:
	str r0, [sp, #0x10]
	add r0, sp, #0x14
	add r2, sp, #0x10
	add r1, r4, #0x90
	bl FUN_02245F68
	mov r1, #0
	ldr r0, [sp, #0x10]
	strb r1, [sp, #0xc]
	strb r1, [sp, #0xd]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x4c
	mov r1, r8
	bl ov66_223BA6C
	ldr r1, _02245184 // =0x0225B6DC
	add r0, sp, #0x40
	ldr r1, [r1]
	add r2, sp, #0x4c
	bl FUN_022467C4
	ldr r0, [sp, #0x40]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x41
	ldrne r1, [sp, #0x48]
	add r0, sp, #0x34
	bl ov66_223BA6C
	ldr r0, _02245184 // =0x0225B6DC
	add r1, sp, #0x34
	ldr r0, [r0]
	mov r2, r5
	mov r3, r6
	bl FUN_022469C8
	add r0, sp, #0x34
	bl ov66_223BADC
	add r0, sp, #0x40
	bl ov66_223BADC
	add r0, sp, #0x4c
	bl ov66_223BADC
	add r0, sp, #0x58
	mov r1, #0
	mov r2, #0x30
	bl MI_CpuFill8
	ldr r0, _02245194 // =ov66_22437FC
	ldr r2, _02245198 // =ov66_22430EC
	str r0, [sp, #0x70]
	ldr r0, _0224519C // =ov66_2243744
	str r2, [sp, #0x58]
	ldr r2, _022451A0 // =ov66_22431E0
	str r0, [sp, #0x6c]
	ldr r0, _022451A4 // =ov66_2243234
	str r2, [sp, #0x5c]
	ldr r2, _022451A8 // =ov66_22434D0
	str r0, [sp, #0x60]
	ldr r0, _022451AC // =ov66_2243670
	str r2, [sp, #0x64]
	ldr r2, _022451B0 // =ov66_2243818
	str r0, [sp, #0x74]
	ldr r0, _022451B4 // =ov66_2244528
	str r2, [sp, #0x78]
	ldr r2, _022451B8 // =ov66_22435FC
	str r0, [sp, #0x80]
	str r2, [sp, #0x68]
	ldr r2, [r5, #0x14]
	mov r1, r8
	add r0, sp, #0x28
	str r2, [sp, #0x84]
	bl ov66_223BA6C
	ldr r1, _02245184 // =0x0225B6DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl FUN_022467C4
	ldr r0, [sp, #0x1c]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x1d
	ldrne r1, [sp, #0x24]
	ldr r0, _022451BC // =ov66_224413C
	add r3, sp, #0x58
	stmia sp, {r0, r4}
	str r2, [sp, #8]
	ldr r0, _02245184 // =0x0225B6DC
	mov r2, r7
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_02254584
	add r0, sp, #0x1c
	bl ov66_223BADC
	add r0, sp, #0x28
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02245184: .word 0x0225B6DC
_02245188: .word 0x02259138
_0224518C: .word 0x0225B6E0
_02245190: .word 0x02259180
_02245194: .word ov66_22437FC
_02245198: .word ov66_22430EC
_0224519C: .word ov66_2243744
_022451A0: .word ov66_22431E0
_022451A4: .word ov66_2243234
_022451A8: .word ov66_22434D0
_022451AC: .word ov66_2243670
_022451B0: .word ov66_2243818
_022451B4: .word ov66_2244528
_022451B8: .word ov66_22435FC
_022451BC: .word ov66_224413C
	arm_func_end FUN_02244F44
_022451C0:
	.byte 0x04, 0x00, 0x90, 0xE5, 0x1E, 0xFF, 0x2F, 0xE1

	arm_func_start FUN_022451C8
FUN_022451C8: // 0x022451C8
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x30
	ldr r1, _022452CC // =0x0225B6DC
	mov r4, r0
	ldr r1, [r1]
	cmp r1, #0
	addeq sp, sp, #0x30
	moveq r0, #4
	ldmeqia sp!, {r4, pc}
	beq _02245204
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x30
	moveq r0, #7
	ldmeqia sp!, {r4, pc}
_02245204:
	cmp r1, #0
	beq _02245228
	ldr r0, _022452CC // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x30
	movne r0, #4
	ldmneia sp!, {r4, pc}
_02245228:
	add r0, sp, #0x24
	mov r1, r4
	bl ov66_223BA6C
	ldr r1, _022452CC // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl FUN_022467C4
	ldr r0, [sp, #0x18]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _022452CC // =0x0225B6DC
	addeq r1, sp, #0x19
	ldr r0, [r0]
	ldrne r1, [sp, #0x20]
	ldr r0, [r0]
	bl FUN_022546D4
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0xc
	mov r1, r4
	bl ov66_223BA6C
	ldr r1, _022452CC // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467C4
	ldr r0, _022452CC // =0x0225B6DC
	add r1, sp, #0
	ldr r0, [r0]
	bl FUN_02247258
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x30
	ldmia sp!, {r4, pc}
	.align 2, 0
_022452CC: .word 0x0225B6DC
	arm_func_end FUN_022451C8

	arm_func_start FUN_022452D0
FUN_022452D0: // 0x022452D0
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	ldr r2, _022453B8 // =0x0225B6DC
	mov r5, r0
	ldr r2, [r2]
	mov r4, r1
	cmp r2, #0
	addeq sp, sp, #0x18
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, pc}
	beq _02245310
	ldr r0, [r2, #4]
	cmp r0, #5
	addeq sp, sp, #0x18
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, pc}
_02245310:
	cmp r2, #0
	beq _02245334
	ldr r0, _022453B8 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x18
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, pc}
_02245334:
	mov r0, r4
	bl strlen
	add r0, r0, #1
	cmp r0, #0x190
	addhi sp, sp, #0x18
	movhi r0, #3
	ldmhiia sp!, {r3, r4, r5, pc}
	add r0, sp, #0xc
	mov r1, r5
	bl ov66_223BA6C
	ldr r1, _022453B8 // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467C4
	ldr r0, [sp]
	mov r2, r4
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _022453B8 // =0x0225B6DC
	addeq r1, sp, #1
	ldr r0, [r0]
	ldrne r1, [sp, #8]
	ldr r0, [r0]
	mov r3, #3
	bl FUN_02254798
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_022453B8: .word 0x0225B6DC
	arm_func_end FUN_022452D0

	arm_func_start FUN_022453BC
FUN_022453BC: // 0x022453BC
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	ldr r2, _022454C0 // =0x0225B6DC
	mov r6, r0
	ldr r4, [r2]
	mov r5, r1
	cmp r4, #0
	addeq sp, sp, #0xc
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	beq _022453FC
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, pc}
_022453FC:
	cmp r4, #0
	beq _02245420
	ldr r0, _022454C0 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0xc
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, pc}
_02245420:
	mov r0, r5
	bl strlen
	add r0, r0, #1
	cmp r0, #0x190
	addhi sp, sp, #0xc
	movhi r0, #3
	ldmhiia sp!, {r3, r4, r5, r6, pc}
	add r0, sp, #0
	mov r1, r4
	mov r2, r6
	bl FUN_02247578
	ldr r1, _022454C4 // =0x02259204
	add r0, sp, #0
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _02245480
	add r0, sp, #0
	bl ov66_223BADC
	add sp, sp, #0xc
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, pc}
_02245480:
	ldr r0, [sp]
	mov r2, r5
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _022454C0 // =0x0225B6DC
	addeq r1, sp, #1
	ldr r0, [r0]
	ldrne r1, [sp, #8]
	ldr r0, [r0]
	mov r3, #3
	bl FUN_0225524C
	add r0, sp, #0
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_022454C0: .word 0x0225B6DC
_022454C4: .word 0x02259204
	arm_func_end FUN_022453BC

	arm_func_start FUN_022454C8
FUN_022454C8: // 0x022454C8
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	ldr r2, _022456A8 // =0x0225B6DC
	mov r5, r0
	ldr r2, [r2]
	mov r4, r1
	cmp r2, #0
	addeq sp, sp, #0x38
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
	beq _0224551C
	ldr r0, [r2, #4]
	cmp r0, #5
	addeq sp, sp, #0x38
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
_0224551C:
	cmp r2, #0
	beq _02245548
	ldr r0, _022456A8 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x38
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, lr}
	addne sp, sp, #0x10
	bxne lr
_02245548:
	ldr r0, [sp, #0x54]
	bl strlen
	add r0, r0, #1
	cmp r0, #0x190
	bhi _02245570
	ldr r0, [sp, #0x50]
	bl strlen
	add r0, r0, #1
	cmp r0, #0x14
	bls _02245584
_02245570:
	add sp, sp, #0x38
	mov r0, #3
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02245584:
	ldr r1, _022456AC // =0x02259204
	add r0, sp, #0x2c
	bl ov66_223BA6C
	mvn r0, #0
	cmp r4, r0
	bne _022455AC
	ldr r1, _022456AC // =0x02259204
	add r0, sp, #0x2c
	bl ov66_22419DC
	b _02245610
_022455AC:
	ldr r1, _022456A8 // =0x0225B6DC
	add r0, sp, #0x20
	ldr r1, [r1]
	mov r2, r4
	bl FUN_02247578
	add r0, sp, #0x2c
	add r1, sp, #0x20
	bl ov66_223BC5C
	add r0, sp, #0x20
	bl ov66_223BADC
	ldr r1, _022456AC // =0x02259204
	add r0, sp, #0x2c
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _02245610
	add r0, sp, #0x2c
	bl ov66_223BADC
	add sp, sp, #0x38
	mov r0, #3
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02245610:
	add r0, sp, #0x14
	mov r1, r5
	bl ov66_223BA6C
	ldr r1, _022456A8 // =0x0225B6DC
	add r0, sp, #8
	ldr r1, [r1]
	add r2, sp, #0x14
	bl FUN_022467C4
	ldr r0, [sp, #0x2c]
	add r3, sp, #0x54
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #8]
	addeq r2, sp, #0x2d
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0x34]
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #9
	ldrne r1, [sp, #0x10]
	add r0, sp, #0x50
	str r0, [sp]
	str r3, [sp, #4]
	ldr r0, _022456A8 // =0x0225B6DC
	mov r3, #1
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_02255450
	add r0, sp, #8
	bl ov66_223BADC
	add r0, sp, #0x14
	bl ov66_223BADC
	add r0, sp, #0x2c
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_022456A8: .word 0x0225B6DC
_022456AC: .word 0x02259204
	arm_func_end FUN_022454C8

	arm_func_start FUN_022456B0
FUN_022456B0: // 0x022456B0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x50
	ldr r2, _02245904 // =0x0225B6DC
	mov r6, r0
	ldr r2, [r2]
	mov r4, r1
	cmp r2, #0
	mov r5, r3
	addeq sp, sp, #0x50
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	beq _02245708
	ldr r0, [r2, #4]
	cmp r0, #5
	addeq sp, sp, #0x50
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
_02245708:
	cmp r2, #0
	beq _02245734
	ldr r0, _02245904 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x50
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, lr}
	addne sp, sp, #0x10
	bxne lr
_02245734:
	ldr r0, [sp, #0x68]
	bl strlen
	add r0, r0, #1
	cmp r0, #0x14
	addhi sp, sp, #0x50
	movhi r0, #3
	ldmhiia sp!, {r4, r5, r6, lr}
	addhi sp, sp, #0x10
	bxhi lr
	ldr r1, _02245908 // =0x02259204
	add r0, sp, #0x44
	bl ov66_223BA6C
	mvn r0, #0
	cmp r4, r0
	bne _02245780
	ldr r1, _02245908 // =0x02259204
	add r0, sp, #0x44
	bl ov66_22419DC
	b _022457E4
_02245780:
	ldr r1, _02245904 // =0x0225B6DC
	add r0, sp, #0x38
	ldr r1, [r1]
	mov r2, r4
	bl FUN_02247578
	add r0, sp, #0x44
	add r1, sp, #0x38
	bl ov66_223BC5C
	add r0, sp, #0x38
	bl ov66_223BADC
	ldr r1, _02245908 // =0x02259204
	add r0, sp, #0x44
	bl ov66_223BEEC
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022457E4
	add r0, sp, #0x44
	bl ov66_223BADC
	add sp, sp, #0x50
	mov r0, #3
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_022457E4:
	ldr r1, _02245904 // =0x0225B6DC
	mov r0, #0x10
	ldr r4, [r1]
	bl FUN_02246290
	cmp r0, #0
	beq _02245830
	ldr r2, _0224590C // =0x02259138
	ldr r1, _02245910 // =0x0225B6E0
	str r2, [r0]
	mov r3, #0
	ldr r2, [sp, #0x70]
	str r3, [r0, #4]
	ldr ip, [r1]
	str r2, [r0, #8]
	ldr r2, _02245914 // =0x02259168
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r5, [r0, #0xc]
_02245830:
	str r0, [sp, #0x14]
	add r0, sp, #0x18
	add r2, sp, #0x14
	add r1, r4, #0x90
	bl FUN_02245F68
	mov r1, #0
	ldr r0, [sp, #0x14]
	strb r1, [sp, #0x10]
	strb r1, [sp, #0x11]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x2c
	mov r1, r6
	bl ov66_223BA6C
	ldr r1, _02245904 // =0x0225B6DC
	add r0, sp, #0x20
	ldr r1, [r1]
	add r2, sp, #0x2c
	bl FUN_022467C4
	ldr r0, [sp, #0x44]
	add r3, sp, #0x68
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0x20]
	addeq r2, sp, #0x45
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0x4c]
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x21
	ldrne r1, [sp, #0x28]
	ldr r0, _02245918 // =ov66_224439C
	str r3, [sp]
	stmib sp, {r0, r4}
	mov r3, #0
	str r3, [sp, #0xc]
	ldr r0, _02245904 // =0x0225B6DC
	mov r3, #1
	ldr r0, [r0]
	ldr r0, [r0]
	bl FUN_022557EC
	add r0, sp, #0x20
	bl ov66_223BADC
	add r0, sp, #0x2c
	bl ov66_223BADC
	add r0, sp, #0x44
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_02245904: .word 0x0225B6DC
_02245908: .word 0x02259204
_0224590C: .word 0x02259138
_02245910: .word 0x0225B6E0
_02245914: .word 0x02259168
_02245918: .word ov66_224439C
	arm_func_end FUN_022456B0

	arm_func_start FUN_0224591C
FUN_0224591C: // 0x0224591C
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	ldr r3, _02245A68 // =0x0225B6DC
	mov r7, r0
	ldr r4, [r3]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x30
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	beq _02245960
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x30
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_02245960:
	cmp r4, #0
	beq _02245984
	ldr r0, _02245A68 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x30
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
_02245984:
	mov r0, #0x10
	bl FUN_02246290
	cmp r0, #0
	beq _022459C0
	ldr r2, _02245A6C // =0x02259138
	ldr r1, _02245A70 // =0x0225B6E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02245A74 // =0x022591B0
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_022459C0:
	str r0, [sp, #0xc]
	add r0, sp, #0x10
	add r2, sp, #0xc
	add r1, r4, #0x90
	bl FUN_02245F68
	mov r1, #0
	ldr r0, [sp, #0xc]
	strb r1, [sp, #8]
	strb r1, [sp, #9]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x24
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _02245A68 // =0x0225B6DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl FUN_022467C4
	ldr r0, [sp, #0x18]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x19
	ldrne r1, [sp, #0x20]
	ldr r0, _02245A68 // =0x0225B6DC
	str r4, [sp]
	str r2, [sp, #4]
	ldr r0, [r0]
	ldr r2, _02245A78 // =ov66_2243D28
	ldr r0, [r0]
	ldr r3, _02245A7C // =ov66_2243B74
	bl FUN_0225449C
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02245A68: .word 0x0225B6DC
_02245A6C: .word 0x02259138
_02245A70: .word 0x0225B6E0
_02245A74: .word 0x022591B0
_02245A78: .word ov66_2243D28
_02245A7C: .word ov66_2243B74
	arm_func_end FUN_0224591C

	arm_func_start FUN_02245A80
FUN_02245A80: // 0x02245A80
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x44
	ldr r3, _02245C40 // =0x0225B6DC
	mov r7, r0
	ldr r3, [r3]
	mov r6, r1
	cmp r3, #0
	mov r5, r2
	addeq sp, sp, #0x44
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	beq _02245AC4
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #0x44
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02245AC4:
	cmp r3, #0
	beq _02245AE8
	ldr r0, _02245C40 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x44
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, pc}
_02245AE8:
	add r0, sp, #0x38
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _02245C40 // =0x0225B6DC
	add r0, sp, #0x2c
	ldr r1, [r1]
	add r2, sp, #0x38
	bl FUN_022467C4
	ldr r0, [sp, #0x2c]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _02245C40 // =0x0225B6DC
	addeq r1, sp, #0x2d
	ldr r0, [r0]
	ldrne r1, [sp, #0x34]
	ldr r0, [r0]
	bl FUN_02254F20
	cmp r0, #0
	moveq r4, #1
	add r0, sp, #0x2c
	movne r4, #0
	bl ov66_223BADC
	add r0, sp, #0x38
	bl ov66_223BADC
	cmp r4, #0
	addne sp, sp, #0x44
	movne r0, #5
	ldmneia sp!, {r4, r5, r6, r7, pc}
	ldr r1, _02245C40 // =0x0225B6DC
	mov r0, #0x10
	ldr r4, [r1]
	bl FUN_02246290
	cmp r0, #0
	beq _02245B9C
	ldr r2, _02245C44 // =0x02259138
	ldr r1, _02245C48 // =0x0225B6E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02245C4C // =0x02259198
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_02245B9C:
	str r0, [sp, #8]
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	bl FUN_02245F68
	mov r1, #0
	ldr r0, [sp, #8]
	strb r1, [sp, #4]
	strb r1, [sp, #5]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x20
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _02245C40 // =0x0225B6DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl FUN_022467C4
	ldr r0, [sp, #0x14]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x15
	ldrne r1, [sp, #0x1c]
	ldr r0, _02245C40 // =0x0225B6DC
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02245C50 // =ov66_2243D30
	ldr r0, [r0]
	mov r3, r4
	bl FUN_02255104
	add r0, sp, #0x14
	bl ov66_223BADC
	add r0, sp, #0x20
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x44
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_02245C40: .word 0x0225B6DC
_02245C44: .word 0x02259138
_02245C48: .word 0x0225B6E0
_02245C4C: .word 0x02259198
_02245C50: .word ov66_2243D30
	arm_func_end FUN_02245A80

	arm_func_start FUN_02245C54
FUN_02245C54: // 0x02245C54
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r2, _02245D1C // =0x0225B6DC
	mov r4, r1
	ldr r2, [r2]
	mov r3, r0
	cmp r2, #0
	addeq sp, sp, #0x18
	moveq r0, #4
	ldmeqia sp!, {r4, pc}
	beq _02245C94
	ldr r0, [r2, #4]
	cmp r0, #5
	addeq sp, sp, #0x18
	moveq r0, #7
	ldmeqia sp!, {r4, pc}
_02245C94:
	cmp r2, #0
	beq _02245CB8
	ldr r0, _02245D1C // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x18
	movne r0, #4
	ldmneia sp!, {r4, pc}
_02245CB8:
	add r0, sp, #0xc
	mov r1, r3
	bl ov66_223BA6C
	ldr r1, _02245D1C // =0x0225B6DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl FUN_022467C4
	ldr r0, [sp]
	mov r2, r4
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _02245D1C // =0x0225B6DC
	addeq r1, sp, #1
	ldr r0, [r0]
	ldrne r1, [sp, #8]
	ldr r0, [r0]
	bl FUN_0225497C
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.align 2, 0
_02245D1C: .word 0x0225B6DC
	arm_func_end FUN_02245C54

	arm_func_start FUN_02245D20
FUN_02245D20: // 0x02245D20
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2c
	ldr r3, _02245E68 // =0x0225B6DC
	mov r7, r0
	ldr r4, [r3]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x2c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	beq _02245D64
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x2c
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02245D64:
	cmp r4, #0
	beq _02245D88
	ldr r0, _02245E68 // =0x0225B6DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x2c
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, pc}
_02245D88:
	mov r0, #0x10
	bl FUN_02246290
	cmp r0, #0
	beq _02245DC4
	ldr r2, _02245E6C // =0x02259138
	ldr r1, _02245E70 // =0x0225B6E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02245E74 // =0x02259150
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_02245DC4:
	str r0, [sp, #8]
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	bl FUN_02245F68
	mov r1, #0
	ldr r0, [sp, #8]
	strb r1, [sp, #4]
	strb r1, [sp, #5]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x20
	mov r1, r7
	bl ov66_223BA6C
	ldr r1, _02245E68 // =0x0225B6DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl FUN_022467C4
	ldr r0, [sp, #0x14]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x15
	ldrne r1, [sp, #0x1c]
	ldr r0, _02245E68 // =0x0225B6DC
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02245E78 // =ov66_2244620
	ldr r0, [r0]
	mov r3, r4
	bl FUN_02254B7C
	add r0, sp, #0x14
	bl ov66_223BADC
	add r0, sp, #0x20
	bl ov66_223BADC
	mov r0, #0
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_02245E68: .word 0x0225B6DC
_02245E6C: .word 0x02259138
_02245E70: .word 0x0225B6E0
_02245E74: .word 0x02259150
_02245E78: .word ov66_2244620
	arm_func_end FUN_02245D20
_02245E7C:
	.byte 0x10, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x00, 0x20, 0xA0, 0xE1, 0x4A, 0x46, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8
	.byte 0x1E, 0xFF, 0x2F, 0xE1, 0x10, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1, 0x00, 0x20, 0xA0, 0xE1, 0x40, 0x46, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8, 0x1E, 0xFF, 0x2F, 0xE1, 0x10, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x00, 0x20, 0xA0, 0xE1, 0x36, 0x46, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8
	.byte 0x1E, 0xFF, 0x2F, 0xE1, 0x10, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1, 0x00, 0x20, 0xA0, 0xE1, 0x2C, 0x46, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8, 0x1E, 0xFF, 0x2F, 0xE1, 0x10, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x00, 0x20, 0xA0, 0xE1, 0x22, 0x46, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8
	.byte 0x1E, 0xFF, 0x2F, 0xE1, 0x10, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x0A
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x04, 0x10, 0xA0, 0xE1, 0x00, 0x20, 0xA0, 0xE1, 0x18, 0x46, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x10, 0x80, 0xBD, 0xE8

	arm_func_start FUN_02245F68
FUN_02245F68: // 0x02245F68
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r4, r2
	mov r5, r1
	mov r6, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r5
	mov r1, r4
	str ip, [sp]
	bl FUN_0224621C
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _02245FC0
	ldr r2, [r3, #0xc]
	ldr r0, [r4]
	ldr r2, [r2, #4]
	ldr r0, [r0, #4]
	cmp r2, r0
	bhs _02245FEC
_02245FC0:
	str r4, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r5
	bl FUN_02246008
	ldr r1, _02246004 // =0x02259128
	str r0, [r6]
	ldrb r0, [r1, #1]
	add sp, sp, #0xc
	strb r0, [r6, #4]
	ldmia sp!, {r3, r4, r5, r6, pc}
_02245FEC:
	ldr r0, _02246004 // =0x02259128
	str r3, [r6]
	ldrb r0, [r0, #5]
	strb r0, [r6, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_02246004: .word 0x02259128
	arm_func_end FUN_02245F68

	arm_func_start FUN_02246008
FUN_02246008: // 0x02246008
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	mov r8, r0
	ldr r4, [r8]
	mvn r0, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	cmp r4, r0
	bne _0224603C
	ldr r0, _022460DC // =0x0225923C
	bl printf
	bl abort
_0224603C:
	mov r1, #0
	mov r0, #0x10
	strb r1, [sp]
	bl FUN_02246290
	mov r4, r0
	add r0, r8, #4
	str r0, [sp, #0xc]
	adds r1, r4, #0xc
	ldrne r0, [sp, #0x30]
	str r4, [sp, #0x10]
	ldrne r0, [r0]
	str r4, [sp, #4]
	strne r0, [r1]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r7, [r0]
	cmp r6, #0
	strne r4, [r7]
	streq r4, [r7, #4]
	ldr r1, [r8]
	mov r0, r4
	add r1, r1, #1
	str r1, [r8]
	ldr r1, [r8, #4]
	str r4, [sp, #8]
	bl ov66_223ADD8
	cmp r5, #0
	ldr r1, [sp, #0x10]
	strne r4, [r8, #8]
	cmp r1, #0
	beq _022460D0
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_022460D0:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_022460DC: .word 0x0225923C
	arm_func_end FUN_02246008

	arm_func_start FUN_022460E0
FUN_022460E0: // 0x022460E0
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _02246128
	beq _0224611C
	mov r2, r0
	bl DWC_Free
_0224611C:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_02246128:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov66_224290C
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_022460E0

	arm_func_start FUN_02246150
FUN_02246150: // 0x02246150
	stmfd sp!, {r3, lr}
	ldr r3, [r1]
	mov r3, r3, lsl #0x1f
	movs r3, r3, lsr #0x1f
	ldmneib r1, {r3, ip}
	bne _02246178
	ldrb r3, [r1]
	add ip, r1, #1
	mov r1, r3, lsl #0x18
	mov r3, r1, lsr #0x19
_02246178:
	mov r1, ip
	bl FUN_02246184
	ldmia sp!, {r3, pc}
	arm_func_end FUN_02246150

	arm_func_start FUN_02246184
FUN_02246184: // 0x02246184
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	ldr ip, [r0]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldrne lr, [r0, #8]
	ldrne r0, [r0, #4]
	bne _022461B0
	ldrb ip, [r0]
	add lr, r0, #1
	mov r0, ip, lsl #0x18
	mov r0, r0, lsr #0x19
_022461B0:
	cmp r2, r0
	bhs _02246214
	cmp r3, #0
	beq _02246214
	add ip, lr, r0
	add r4, lr, r2
	cmp r4, ip
	bhs _02246214
	mov r2, #0
_022461D4:
	mov r5, r1
	mov r6, r2
	cmp r3, #0
	bls _02246208
	ldrsb r7, [r4]
_022461E8:
	ldrsb r0, [r5]
	cmp r7, r0
	subeq r0, r4, lr
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r6, r6, #1
	cmp r6, r3
	add r5, r5, #1
	blo _022461E8
_02246208:
	add r4, r4, #1
	cmp r4, ip
	blo _022461D4
_02246214:
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_02246184

	arm_func_start FUN_0224621C
FUN_0224621C: // 0x0224621C
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_02246250:
	ldr r4, [r1]
	ldr r0, [r6, #0xc]
	ldr r5, [r4, #4]
	ldr r4, [r0, #4]
	mov r0, r6
	cmp r5, r4
	ldrlo r6, [r6]
	strlob lr, [r3]
	blo _02246284
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_02246284:
	cmp r6, #0
	bne _02246250
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_0224621C

	arm_func_start FUN_02246290
FUN_02246290: // 0x02246290
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	ldr r4, _022462D8 // =0x0225B6E4
	moveq r6, #1
	mov r5, #0
_022462A4:
	mov r0, r5
	mov r1, r6
	bl DWC_Alloc
	cmp r0, #0
	ldmneia sp!, {r4, r5, r6, pc}
	ldr r0, [r4]
	str r0, [r4]
	cmp r0, #0
	beq _022462D0
	blx r0
	b _022462A4
_022462D0:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_022462D8: .word 0x0225B6E4
	arm_func_end FUN_02246290
_022462DC:
	.byte 0x08, 0x40, 0x2D, 0xE9
	.byte 0x18, 0x00, 0x9F, 0xE5, 0x00, 0x00, 0x90, 0xE5, 0x00, 0x00, 0x90, 0xE5, 0x3A, 0x38, 0x00, 0xEB
	.byte 0x0C, 0x00, 0x9F, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x10, 0x80, 0xE5, 0x08, 0x80, 0xBD, 0xE8
	.byte 0xE8, 0xB6, 0x25, 0x02, 0xE4, 0xB6, 0x25, 0x02, 0x08, 0x40, 0x2D, 0xE9, 0x48, 0x00, 0x9F, 0xE5
	.byte 0x04, 0x10, 0x90, 0xE5, 0x00, 0x00, 0x51, 0xE3, 0x05, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0x20, 0xA0, 0xE1, 0x26, 0x45, 0xFE, 0xEB, 0x2C, 0x00, 0x9F, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x04, 0x10, 0x80, 0xE5, 0x20, 0x00, 0x9F, 0xE5, 0x02, 0x10, 0xA0, 0xE3, 0x00, 0x30, 0x90, 0xE5
	.byte 0x05, 0x20, 0xA0, 0xE3, 0x58, 0x10, 0x83, 0xE5, 0x10, 0x10, 0x9F, 0xE5, 0x10, 0x00, 0x9F, 0xE5
	.byte 0x04, 0x20, 0x83, 0xE5, 0x00, 0x10, 0x80, 0xE5, 0x08, 0x80, 0xBD, 0xE8, 0xE8, 0xB6, 0x25, 0x02
	.byte 0xDC, 0x62, 0x24, 0x02, 0xE4, 0xB6, 0x25, 0x02

	arm_func_start FUN_02246368
FUN_02246368: // 0x02246368
	stmfd sp!, {r3, lr}
	ldr r1, _022463CC // =0x0225B6E8
	str r0, [r1]
	ldr r1, [r1, #4]
	cmp r1, #0
	beq _02246398
	mov r0, #0
	mov r2, r0
	bl DWC_Free
	ldr r0, _022463CC // =0x0225B6E8
	mov r1, #0
	str r1, [r0, #4]
_02246398:
	mov r0, #0
	mov r1, #0x400
	bl DWC_Alloc
	ldr r1, _022463CC // =0x0225B6E8
	cmp r0, #0
	str r0, [r1, #4]
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r2, _022463D0 // =0x02246308
	ldr r1, _022463D4 // =0x0225B6E4
	mov r0, #1
	str r2, [r1]
	ldmia sp!, {r3, pc}
	.align 2, 0
_022463CC: .word 0x0225B6E8
_022463D0: .word 0x02246308
_022463D4: .word 0x0225B6E4
	arm_func_end FUN_02246368

	arm_func_start FUN_022463D8
FUN_022463D8: // 0x022463D8
	stmfd sp!, {r3, lr}
	ldr r0, _02246408 // =0x0225B6E8
	ldr r1, [r0, #4]
	cmp r1, #0
	ldmeqia sp!, {r3, pc}
	mov r0, #0
	mov r2, r0
	bl DWC_Free
	ldr r0, _02246408 // =0x0225B6E8
	mov r1, #0
	str r1, [r0, #4]
	ldmia sp!, {r3, pc}
	.align 2, 0
_02246408: .word 0x0225B6E8
	arm_func_end FUN_022463D8

	arm_func_start FUN_0224640C
FUN_0224640C: // 0x0224640C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldr lr, _02246498 // =0x02259138
	add r3, sp, #0x10
	str r1, [sp, #0x14]
	mov ip, #0
	add r0, sp, #4
	add r2, sp, #8
	mov r1, r4
	str lr, [sp, #0x10]
	str ip, [sp, #0x18]
	str r3, [sp, #8]
	bl ov66_2243C94
	ldr r1, [sp, #4]
	add r0, r4, #4
	cmp r1, r0
	str r1, [sp, #0xc]
	addeq sp, sp, #0x1c
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0xc]
	cmp r0, #0
	beq _02246478
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02246478:
	add r1, sp, #0xc
	ldr r2, [r1]
	add r0, sp, #0
	mov r1, r4
	bl FUN_02246518
	mov r0, #1
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_02246498: .word 0x02259138
	arm_func_end FUN_0224640C

	arm_func_start FUN_0224649C
FUN_0224649C: // 0x0224649C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r5, r0
	ldr r0, [r5, #8]
	add r4, r5, #4
	str r0, [sp, #8]
	cmp r0, r4
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r8, sp, #8
	add r7, sp, #0
	add r6, sp, #4
_022464CC:
	ldr r0, [r0, #0xc]
	cmp r0, #0
	beq _022464E4
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022464E4:
	ldr r1, [sp, #8]
	mov r0, r8
	str r1, [sp, #4]
	bl ov66_223AD6C
	ldr r2, [r6]
	mov r0, r7
	mov r1, r5
	bl FUN_02246518
	ldr r0, [sp, #8]
	cmp r0, r4
	bne _022464CC
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end FUN_0224649C

	arm_func_start FUN_02246518
FUN_02246518: // 0x02246518
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #8]
	mov r6, r0
	cmp r4, r1
	bne _02246548
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #8]
_02246548:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	cmp r4, #0
	beq _02246570
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02246570:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_02246518

	arm_func_start FUN_02246588
FUN_02246588: // 0x02246588
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x84
	mov r5, r0
	mov r0, #0
	mov r7, r1
	str r0, [r5]
	str r0, [r5, #4]
	ldr ip, [sp, #0x98]
	ldr r1, [sp, #0x9c]
	add r0, r5, #0xc
	mov r6, r2
	mov r4, r3
	str ip, [r5, #8]
	bl ov66_223B938
	ldr r1, [sp, #0xa0]
	add r0, r5, #0x18
	bl ov66_223B938
	ldr r1, [sp, #0xa4]
	add r0, r5, #0x24
	bl ov66_223B938
	mov r1, r7
	add r0, r5, #0x30
	bl ov66_223B938
	mov r1, r6
	add r0, r5, #0x3c
	bl ov66_223B938
	mov r1, #0
	add r0, r5, #0x48
	mov ip, r1
_022465FC:
	str ip, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022465FC
	str ip, [r5, #0x54]
	str ip, [r5, #0x58]
	str ip, [r5, #0x5c]
	ldr r0, [sp, #0xa8]
	str ip, [r5, #0x60]
	add r3, r5, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	str ip, [r5, #0x70]
	str ip, [r5, #0x74]
	add r0, r5, #0x74
	str r0, [r5, #0x7c]
	str ip, [r5, #0x80]
	str ip, [r5, #0x84]
	add r0, r5, #0x84
	str r0, [r5, #0x8c]
	str ip, [r5, #0x90]
	str ip, [r5, #0x94]
	add r0, r5, #0x94
	str r0, [r5, #0x98]
	str ip, [r5, #0x9c]
	str ip, [r5, #0xa0]
	add r0, r5, #0xa0
	ldr lr, _02246748 // =0x02258CB4
	str r0, [r5, #0xa8]
	add ip, sp, #0x54
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	ldr r1, _0224674C // =0x02258CAC
	add r0, sp, #0x48
	bl ov66_223BA6C
	ldr r2, _02246750 // =0x02259298
	add r0, sp, #0x3c
	add r1, sp, #0x48
	bl ov66_22379D0
	add r0, sp, #0x30
	add r1, sp, #0x3c
	add r2, r5, #0x30
	bl ov66_2237908
	ldr r2, _02246750 // =0x02259298
	add r0, sp, #0x24
	add r1, sp, #0x30
	bl ov66_22379D0
	add r1, sp, #0x54
	ldr r1, [r1, r4, lsl #2]
	add r0, sp, #0x18
	bl ov66_223BA6C
	add r0, sp, #0xc
	add r1, sp, #0x24
	add r2, sp, #0x18
	bl ov66_2237908
	ldr r2, _02246750 // =0x02259298
	add r0, sp, #0
	add r1, sp, #0xc
	bl ov66_22379D0
	add r1, sp, #0
	add r0, r5, #0x48
	bl ov66_223BC5C
	add r0, sp, #0
	bl ov66_223BADC
	add r0, sp, #0xc
	bl ov66_223BADC
	add r0, sp, #0x18
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0x30
	bl ov66_223BADC
	add r0, sp, #0x3c
	bl ov66_223BADC
	add r0, sp, #0x48
	bl ov66_223BADC
	mov r0, r5
	add sp, sp, #0x84
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_02246748: .word 0x02258CB4
_0224674C: .word 0x02258CAC
_02246750: .word 0x02259298
	arm_func_end FUN_02246588

	arm_func_start FUN_02246754
FUN_02246754: // 0x02246754
	stmfd sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x9c
	bl FUN_0223ABC4
	add r0, r4, #0x9c
	bl ov66_223AD4C
	add r0, r4, #0x90
	bl FUN_0224649C
	add r0, r4, #0x90
	bl FUN_0224804C
	add r0, r4, #0x80
	bl FUN_02247994
	add r0, r4, #0x70
	bl FUN_0224802C
	add r0, r4, #0x48
	bl ov66_223BADC
	add r0, r4, #0x3c
	bl ov66_223BADC
	add r0, r4, #0x30
	bl ov66_223BADC
	add r0, r4, #0x24
	bl ov66_223BADC
	add r0, r4, #0x18
	bl ov66_223BADC
	add r0, r4, #0xc
	bl ov66_223BADC
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_02246754

	arm_func_start FUN_022467C4
FUN_022467C4: // 0x022467C4
	ldr ip, _022467D0 // =ov66_2237908
	add r1, r1, #0x48
	bx ip
	.align 2, 0
_022467D0: .word ov66_2237908
	arm_func_end FUN_022467C4

	arm_func_start FUN_022467D4
FUN_022467D4: // 0x022467D4
	ldr r3, [r1, #0x48]
	mov r3, r3, lsl #0x1f
	movs r3, r3, lsr #0x1f
	ldrne r3, [r1, #0x4c]
	bne _022467F4
	ldrb r1, [r1, #0x48]
	mov r1, r1, lsl #0x18
	mov r3, r1, lsr #0x19
_022467F4:
	ldr ip, _02246808 // =ov66_223A594
	mov r1, r2
	mov r2, r3
	mvn r3, #0
	bx ip
	.align 2, 0
_02246808: .word ov66_223A594
	arm_func_end FUN_022467D4

	arm_func_start FUN_0224680C
FUN_0224680C: // 0x0224680C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	mov sl, r0
	mov r6, #0
	str r6, [sl]
	mov r8, r2
	str r6, [sl, #4]
	cmp r8, #0
	mov sb, r1
	mov r7, r3
	addle sp, sp, #8
	str r6, [sl, #8]
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	sub r4, sp, #4
	mov r5, r6
	add fp, sp, #4
_0224684C:
	ldr r0, [sb, #0x48]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r1, [sb, #0x4c]
	bne _0224686C
	ldrb r0, [sb, #0x48]
	mov r0, r0, lsl #0x18
	mov r1, r0, lsr #0x19
_0224686C:
	ldr r0, [r7, r6, lsl #2]
	add r0, r0, r1
	str r0, [sp, #4]
	strb r5, [sp]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _022468AC
	mov r0, r1
	add r2, r0, #1
	str r2, [sl, #4]
	ldr r0, [sl]
	ldr r1, [sp, #4]
	add r0, r0, r2, lsl #2
	str r1, [r0, #-4]
	b _022468C8
_022468AC:
	strb r5, [sp, #1]
	and r0, r5, #0xff
	strb r0, [r4]
	ldr r2, [r4]
	mov r0, sl
	mov r1, fp
	bl ov66_2242818
_022468C8:
	add r6, r6, #1
	cmp r6, r8
	blt _0224684C
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end FUN_0224680C

	arm_func_start FUN_022468DC
FUN_022468DC: // 0x022468DC
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	bl current_time
	ldr r1, _02246920 // =0x10624DD3
	mov r3, #0x3e8
	umull r1, ip, r0, r1
	mov ip, ip, lsr #6
	umull r1, r2, r3, ip
	sub ip, r0, r1
	ldr r2, _02246924 // =0x0225929C
	mov r0, r5
	mov r3, r4
	mov r1, #0x14
	str ip, [sp]
	bl ov66_2236A38
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02246920: .word 0x10624DD3
_02246924: .word 0x0225929C
	arm_func_end FUN_022468DC

	arm_func_start FUN_02246928
FUN_02246928: // 0x02246928
	stmfd sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r1, _022469C4 // =0x022592A4
	mov r4, r0
	add r0, sp, #0
	bl ov66_223BA6C
	add r0, sp, #0xc
	add r2, sp, #0
	mov r1, r4
	mov r3, #0
	bl ov66_2242F08
	add r0, sp, #0
	bl ov66_223BADC
	ldr r0, [sp, #0x10]
	cmp r0, #2
	bhs _0224697C
	add r0, sp, #0xc
	bl ov66_223B578
	add sp, sp, #0x18
	mvn r0, #0
	ldmia sp!, {r4, pc}
_0224697C:
	ldr r1, [sp, #0xc]
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0xd
	ldrne r0, [r1, #0x14]
	bl atoi
	movs r4, r0
	add r0, sp, #0xc
	bne _022469B4
	bl ov66_223B578
	add sp, sp, #0x18
	mvn r0, #0
	ldmia sp!, {r4, pc}
_022469B4:
	bl ov66_223B578
	mov r0, r4
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.align 2, 0
_022469C4: .word 0x022592A4
	arm_func_end FUN_02246928

	arm_func_start FUN_022469C8
FUN_022469C8: // 0x022469C8
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xb4
	mov r4, r0
	add r0, sp, #0xa8
	mov r6, r2
	mov r5, r3
	bl ov66_223B938
	add r1, sp, #0x80
	mov r2, #0
	add r0, r1, #4
	add ip, sp, #0x90
	str r0, [r1, #8]
	str r2, [sp, #0x7c]
	str r2, [r1]
	str r2, [r1, #4]
	str r5, [sp, #0x8c]
	ldmia r6!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia r6, {r0, r1}
	stmia ip, {r0, r1}
	add r0, sp, #0x44
	add r1, sp, #0xa8
	add r2, sp, #0x7c
	bl FUN_02246ADC
	mov r1, #0
	strb r1, [sp]
	add r0, sp, #0xc
	add r1, sp, #0x44
	bl ov66_223B938
	add r6, sp, #0x50
	mov r1, #0
	ldr r0, [r6]
	add r5, sp, #0x18
	stmia r5, {r0, r1}
	add r0, r5, #8
	str r0, [r5, #0xc]
	strb r1, [sp, #1]
	str r1, [r5, #8]
	add r0, r5, #4
	add r1, r6, #4
	bl FUN_02247ABC
	ldr r0, [r6, #0x10]
	add lr, r6, #0x14
	str r0, [r5, #0x10]
	add ip, r5, #0x14
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1}
	stmia ip, {r0, r1}
	add r1, r4, #0x80
	add r0, sp, #4
	add r2, sp, #0xc
	bl FUN_0224773C
	add r0, sp, #0x1c
	ldrb r4, [sp, #8]
	bl FUN_02247E18
	add r0, sp, #0xc
	bl ov66_223BADC
	add r0, sp, #0x54
	bl FUN_02247E18
	add r0, sp, #0x44
	bl ov66_223BADC
	add r0, sp, #0x80
	bl FUN_02247E18
	add r0, sp, #0xa8
	bl ov66_223BADC
	mov r0, r4
	add sp, sp, #0xb4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end FUN_022469C8

	arm_func_start FUN_02246ADC
FUN_02246ADC: // 0x02246ADC
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r2
	mov r5, r0
	bl ov66_223B938
	ldr r1, [r4]
	mov r0, #0
	str r1, [r5, #0xc]
	str r0, [r5, #0x10]
	str r0, [r5, #0x14]
	add r0, r5, #0x14
	str r0, [r5, #0x18]
	add r0, r5, #0x10
	add r1, r4, #4
	bl FUN_02247ABC
	ldr r0, [r4, #0x10]
	add lr, r4, #0x14
	str r0, [r5, #0x1c]
	add ip, r5, #0x20
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1}
	stmia ip, {r0, r1}
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_02246ADC

	arm_func_start FUN_02246B38
FUN_02246B38: // 0x02246B38
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	mov r2, r1
	add r0, sp, #0
	add r1, r4, #0x80
	bl FUN_02246B70
	ldr r1, [sp]
	add r0, r4, #0x84
	cmp r1, r0
	moveq r0, #0
	addne r0, r1, #0x2c
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_02246B38

	arm_func_start FUN_02246B70
FUN_02246B70: // 0x02246B70
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r1
	mov r5, r2
	mov r7, r0
	ldr r2, [r6, #4]
	mov r0, r6
	mov r1, r5
	add r3, r6, #4
	bl FUN_02246BD8
	mov r4, r0
	add r0, r6, #4
	cmp r4, r0
	beq _02246BC4
	mov r0, r5
	add r1, r4, #0xc
	bl ov66_223BE90
	cmp r0, #0
	movlt r0, #1
	movge r0, #0
	cmp r0, #0
	beq _02246BD0
_02246BC4:
	add r0, r6, #4
	str r0, [r7]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02246BD0:
	str r4, [r7]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_02246B70

	arm_func_start FUN_02246BD8
FUN_02246BD8: // 0x02246BD8
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	movs r7, r2
	mov r8, r1
	mov r6, r3
	beq _02246C24
	mov r4, #0
	mov r5, #1
_02246BF4:
	mov r1, r8
	add r0, r7, #0xc
	bl ov66_223BE90
	cmp r0, #0
	movlt r0, r5
	movge r0, r4
	cmp r0, #0
	moveq r6, r7
	ldreq r7, [r7]
	ldrne r7, [r7, #4]
	cmp r7, #0
	bne _02246BF4
_02246C24:
	mov r0, r6
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end FUN_02246BD8

	arm_func_start FUN_02246C2C
FUN_02246C2C: // 0x02246C2C
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	mov r5, r1
	mov r6, r0
	mov r0, r5
	mov r4, r2
	bl FUN_02246928
	mvn r1, #0
	str r0, [sp, #0x10]
	cmp r0, r1
	addeq sp, sp, #0x14
	moveq r0, r1
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	add r0, sp, #0xc
	mov r2, r4
	add r1, r6, #0x80
	bl FUN_02246B70
	ldr r1, [sp, #0xc]
	add r0, r6, #0x84
	cmp r1, r0
	addeq sp, sp, #0x14
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	add r0, sp, #4
	add r2, sp, #0x10
	add r1, r1, #0x1c
	bl FUN_022479B4
	mov r2, #0
	add r1, sp, #0x10
	add r0, r6, #0x70
	strb r2, [sp, #1]
	strb r2, [sp]
	bl FUN_02246CC8
	mov r1, r5
	add r0, r0, #4
	bl ov66_223BC5C
	ldr r0, [sp, #0x10]
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end FUN_02246C2C

	arm_func_start FUN_02246CC8
FUN_02246CC8: // 0x02246CC8
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x28
	add r4, sp, #4
	add r2, sp, #8
	add r3, sp, #5
	str r4, [sp]
	mov r6, r0
	mov r5, r1
	bl FUN_02246D84
	ldr r2, [sp, #8]
	mov r4, r0
	cmp r2, #0
	beq _02246D0C
	ldr r1, [r2, #0xc]
	ldr r0, [r5]
	cmp r1, r0
	bge _02246D78
_02246D0C:
	mov r2, #0
	add r1, sp, #0x1c
	mov r0, r2
_02246D18:
	str r0, [r1, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #3
	blo _02246D18
	ldr r2, [r5]
	add r0, sp, #0x10
	add r1, sp, #0x1c
	str r2, [sp, #0xc]
	bl ov66_223B938
	add r1, sp, #0xc
	str r1, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r6
	mov r1, r4
	bl FUN_02247F48
	mov r4, r0
	add r0, sp, #0x10
	bl ov66_223BADC
	add r0, sp, #0x1c
	bl ov66_223BADC
	add sp, sp, #0x28
	add r0, r4, #0xc
	ldmia sp!, {r4, r5, r6, pc}
_02246D78:
	add r0, r2, #0xc
	add sp, sp, #0x28
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end FUN_02246CC8

	arm_func_start FUN_02246D84
FUN_02246D84: // 0x02246D84
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_02246DB8:
	ldr r5, [r1]
	ldr r4, [r6, #0xc]
	mov r0, r6
	cmp r5, r4
	ldrlt r6, [r6]
	strltb lr, [r3]
	blt _02246DE4
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_02246DE4:
	cmp r6, #0
	bne _02246DB8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_02246D84

	arm_func_start FUN_02246DF0
FUN_02246DF0: // 0x02246DF0
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x34
	mov sb, r1
	movs r1, r2
	ldr r1, [sp, #0x58]
	mov r4, #0
	mov sl, r0
	str r4, [sl]
	str r4, [sl, #4]
	str r2, [sp]
	mov r8, r3
	str r4, [sl, #8]
	str r1, [sp, #0x58]
	beq _02246E30
	mov r1, r2
	bl ov66_2242738
_02246E30:
	ldr r0, [sp]
	mov r7, #0
	cmp r0, #0
	addle sp, sp, #0x34
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mvn r0, #0
	sub r4, sp, #4
	str r0, [sp, #4]
_02246E50:
	ldr r1, [r8, r7, lsl #2]
	add r0, sp, #0x28
	bl ov66_223BA6C
	mov fp, #0
	ldr r6, [sb, #8]
	add r0, sp, #0x28
	mov r5, fp
	bl FUN_02246928
	cmp r0, r6
	bne _02246EAC
	ldr r6, [r8, r7, lsl #2]
	add r0, sp, #0x1c
	mov r1, sb
	bl ov66_2243B64
	mov r1, r6
	mov fp, #1
	add r0, sp, #0x1c
	bl ov66_223BEEC
	cmp r0, #0
	movne r0, fp
	moveq r0, r5
	cmp r0, #0
	movne r5, #1
_02246EAC:
	cmp fp, #0
	beq _02246EBC
	add r0, sp, #0x1c
	bl ov66_223BADC
_02246EBC:
	add r0, sp, #0x28
	bl ov66_223BADC
	cmp r5, #0
	bne _02246F60
	ldr r1, [r8, r7, lsl #2]
	add r0, sp, #0x10
	bl ov66_223BA6C
	ldr r2, [sp, #0x58]
	mov r0, sb
	add r1, sp, #0x10
	bl FUN_02246C2C
	str r0, [sp, #0xc]
	add r0, sp, #0x10
	bl ov66_223BADC
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #4]
	cmp r1, r0
	beq _02246F60
	mov r0, #0
	strb r0, [sp, #8]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _02246F3C
	mov r0, r1
	add r2, r0, #1
	str r2, [sl, #4]
	ldr r0, [sl]
	ldr r1, [sp, #0xc]
	add r0, r0, r2, lsl #2
	str r1, [r0, #-4]
	b _02246F60
_02246F3C:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #9]
	mov r2, r2
	strb r2, [r4]
	ldr r2, [r4]
	mov r0, sl
	add r1, sp, #0xc
	bl ov66_2242818
_02246F60:
	ldr r0, [sp]
	add r7, r7, #1
	cmp r7, r0
	blt _02246E50
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end FUN_02246DF0

	arm_func_start FUN_02246F78
FUN_02246F78: // 0x02246F78
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x10
	ldr r1, [sp, #0x34]
	mov r7, r0
	str r1, [sp, #4]
	ldr r0, [r7, #0x8c]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	add r6, r7, #0x84
	add r5, sp, #4
	add r8, sp, #0
	add r4, sp, #0xc
	b _02246FB8
_02246FB0:
	mov r0, r4
	bl ov66_223AD6C
_02246FB8:
	ldr sb, [sp, #0xc]
	cmp sb, r6
	beq _02246FE4
	mov r0, r8
	mov r2, r5
	add r1, sb, #0x1c
	bl FUN_02247024
	ldr r1, [sp]
	add r0, sb, #0x20
	cmp r1, r0
	beq _02246FB0
_02246FE4:
	ldr r1, [sp, #0xc]
	add r0, r7, #0x84
	cmp r1, r0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	addne sp, sp, #0x10
	bxne lr
	add r1, sp, #0x34
	add r0, r7, #0x70
	bl FUN_022470A8
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_02246F78

	arm_func_start FUN_02247024
FUN_02247024: // 0x02247024
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	mov r4, r2
	mov r6, r0
	ldr r2, [r5, #4]
	mov r0, r5
	mov r1, r4
	add r3, r5, #4
	bl FUN_02247078
	add r1, r5, #4
	cmp r0, r1
	beq _02247064
	ldr r2, [r4]
	ldr r1, [r0, #0xc]
	cmp r2, r1
	bge _02247070
_02247064:
	add r0, r5, #4
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
_02247070:
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end FUN_02247024

	arm_func_start FUN_02247078
FUN_02247078: // 0x02247078
	cmp r2, #0
	beq _022470A0
	ldr r1, [r1]
_02247084:
	ldr r0, [r2, #0xc]
	cmp r0, r1
	movge r3, r2
	ldrge r2, [r2]
	ldrlt r2, [r2, #4]
	cmp r2, #0
	bne _02247084
_022470A0:
	mov r0, r3
	bx lr
	arm_func_end FUN_02247078

	arm_func_start FUN_022470A8
FUN_022470A8: // 0x022470A8
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	mov r2, r1
	add r0, sp, #8
	mov r1, r4
	bl FUN_022470F8
	ldr r2, [sp, #8]
	add r0, r4, #4
	cmp r2, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #4
	mov r1, r4
	str r2, [sp]
	bl FUN_02247ED0
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_022470A8

	arm_func_start FUN_022470F8
FUN_022470F8: // 0x022470F8
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	mov r4, r2
	mov r6, r0
	ldr r2, [r5, #4]
	mov r0, r5
	mov r1, r4
	add r3, r5, #4
	bl FUN_0224714C
	add r1, r5, #4
	cmp r0, r1
	beq _02247138
	ldr r2, [r4]
	ldr r1, [r0, #0xc]
	cmp r2, r1
	bge _02247144
_02247138:
	add r0, r5, #4
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
_02247144:
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end FUN_022470F8

	arm_func_start FUN_0224714C
FUN_0224714C: // 0x0224714C
	cmp r2, #0
	beq _02247174
	ldr r1, [r1]
_02247158:
	ldr r0, [r2, #0xc]
	cmp r0, r1
	movge r3, r2
	ldrge r2, [r2]
	ldrlt r2, [r2, #4]
	cmp r2, #0
	bne _02247158
_02247174:
	mov r0, r3
	bx lr
	arm_func_end FUN_0224714C

	arm_func_start FUN_0224717C
FUN_0224717C: // 0x0224717C
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r5, r0
	mov r0, r1
	mov r4, r2
	bl FUN_02246928
	mvn r1, #0
	str r0, [sp, #4]
	cmp r0, r1
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	add r0, sp, #0
	mov r2, r4
	add r1, r5, #0x80
	bl FUN_02246B70
	ldr r2, [sp]
	add r0, r5, #0x84
	cmp r2, r0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	add r1, sp, #4
	add r0, r2, #0x1c
	bl FUN_02247208
	cmp r0, #0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r1, [sp, #4]
	mov r0, r5
	bl FUN_02246F78
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_0224717C

	arm_func_start FUN_02247208
FUN_02247208: // 0x02247208
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	mov r2, r1
	add r0, sp, #8
	mov r1, r4
	bl FUN_02247024
	ldr r2, [sp, #8]
	add r0, r4, #4
	cmp r2, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #4
	mov r1, r4
	str r2, [sp]
	bl FUN_02247A4C
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_02247208

	arm_func_start FUN_02247258
FUN_02247258: // 0x02247258
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x64
	mov r5, r0
	mov r4, r1
	add r0, sp, #0x3c
	mov r2, r4
	add r1, r5, #0x80
	bl FUN_02246B70
	ldr r1, [sp, #0x3c]
	add r0, r5, #0x84
	cmp r1, r0
	addeq sp, sp, #0x64
	mov r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	str r0, [sp, #0x58]
	str r0, [sp, #0x5c]
	str r0, [sp, #0x60]
	ldr r2, [r1, #0x24]
	add ip, r1, #0x20
	strb r0, [sp, #9]
	strb r0, [sp, #0xa]
	strb r0, [sp, #8]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x20]
	str ip, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r0, sp, #0x58
	add r1, sp, #0x14
	str r2, [sp, #0x14]
	str ip, [sp, #0x34]
	str ip, [sp, #0x24]
	str r2, [sp, #0x38]
	str r2, [sp, #0x28]
	str ip, [sp, #0x1c]
	ldmia r1, {r1, r2}
	bl FUN_022473BC
	mov r1, r4
	add r0, r5, #0x80
	bl FUN_022474A4
	ldr r0, _022473B8 // =0x02259258
	ldr r4, [sp, #0x58]
	ldr lr, [r0, #8]
	ldr r2, [r0, #0xc]
	ldr r0, [sp, #0x5c]
	add r1, sp, #0x4c
	add r3, r4, r0, lsl #2
	add ip, sp, #0x40
	str lr, [sp, #0x4c]
	str r5, [sp, #0x54]
	str r2, [sp, #0x50]
	ldmia r1, {r0, r1, r2}
	stmia ip, {r0, r1, r2}
	cmp r4, r3
	movne r0, #1
	moveq r0, #0
	str r3, [sp, #0x2c]
	str r3, [sp, #0xc]
	str r4, [sp, #0x30]
	str r4, [sp, #0x10]
	cmp r0, #0
	beq _022473A4
	cmp r0, #0
	add r6, sp, #0x40
	beq _022473A4
	ldr r5, [sp, #0x10]
	ldr r4, [sp, #0xc]
_02247368:
	ldr r1, [sp, #0x44]
	ldr r0, [sp, #0x48]
	tst r1, #1
	add r0, r0, r1, asr #1
	ldr r1, [r5]
	ldreq r2, [r6]
	beq _02247390
	ldr r3, [r0]
	ldr r2, [r6]
	ldr r2, [r3, r2]
_02247390:
	blx r2
	add r5, r5, #4
	cmp r5, r4
	bne _02247368
	str r5, [sp, #0x10]
_022473A4:
	add r0, sp, #0x58
	bl ov66_223B5D0
	mov r0, #1
	add sp, sp, #0x64
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_022473B8: .word 0x02259258
	arm_func_end FUN_02247258

	arm_func_start FUN_022473BC
FUN_022473BC: // 0x022473BC
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x20
	ldr r2, [sp, #0x4c]
	ldr r1, [sp, #0x50]
	ldr r4, [sp, #0x54]
	mov sl, r0
	str r2, [sp, #0x18]
	str r1, [sp, #0x1c]
	cmp r2, r4
	add r5, sp, #0x18
	mov r8, #0
	beq _02247408
_022473F0:
	mov r0, r5
	add r8, r8, #1
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	cmp r0, r4
	bne _022473F0
_02247408:
	ldr r1, [sl, #8]
	cmp r8, r1
	bls _02247438
	mov r2, #0
	mov r0, sl
	sub r1, r8, r1
	strb r2, [sp, #1]
	bl ov66_224293C
	mov r2, r0
	mov r0, sl
	mov r1, r8
	bl FUN_022460E0
_02247438:
	ldr sb, [sl]
	ldr r1, [sp, #0x4c]
	ldr r0, [sp, #0x50]
	ldr r7, [sp, #0x54]
	str r1, [sp, #0x10]
	str r0, [sp, #0x14]
	cmp r1, r7
	add fp, sp, #0x10
	beq _02247490
	mov r5, #0
_02247460:
	ldr r6, [sp, #0x10]
	strb r5, [sp]
	ldr r4, [r6, #0xc]
	mov r0, fp
	str r4, [sb], #4
	bl ov66_223AD6C
	ldr r0, [sp, #0x10]
	cmp r0, r7
	bne _02247460
	str r4, [sp, #8]
	str r6, [sp, #4]
	str r6, [sp, #0xc]
_02247490:
	str r8, [sl, #4]
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_022473BC

	arm_func_start FUN_022474A4
FUN_022474A4: // 0x022474A4
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	mov r2, r1
	add r0, sp, #8
	mov r1, r4
	bl FUN_02246B70
	ldr r2, [sp, #8]
	add r0, r4, #4
	cmp r2, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #4
	mov r1, r4
	str r2, [sp]
	bl FUN_022477EC
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_022474A4

	arm_func_start FUN_022474F4
FUN_022474F4: // 0x022474F4
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl FUN_02246928
	mvn r1, #0
	str r0, [sp, #4]
	cmp r0, r1
	addeq sp, sp, #8
	moveq r0, r1
	ldmeqia sp!, {r3, r4, r5, pc}
	add r0, sp, #0
	add r2, sp, #4
	add r1, r5, #0x70
	bl FUN_022470F8
	ldr r2, [sp]
	add r0, r5, #0x74
	cmp r2, r0
	addeq sp, sp, #8
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, r4
	add r0, r2, #0x10
	bl ov66_223800C
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	mvnne r0, #0
	ldreq r0, [sp, #4]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_022474F4

	arm_func_start FUN_02247578
FUN_02247578: // 0x02247578
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	add r0, sp, #0
	add r2, sp, #0x18
	add r1, r4, #0x70
	bl FUN_022470F8
	ldr r1, [sp]
	add r0, r4, #0x74
	cmp r1, r0
	bne _022475C0
	ldr r1, _022475D8 // =0x022592A8
	mov r0, r5
	bl ov66_223BA6C
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_022475C0:
	mov r0, r5
	add r1, r1, #0x10
	bl ov66_223B938
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_022475D8: .word 0x022592A8
	arm_func_end FUN_02247578

	arm_func_start FUN_022475DC
FUN_022475DC: // 0x022475DC
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x3c
	mov r5, r0
	mov r4, r2
	bl FUN_022474F4
	str r0, [sp, #0xc]
	add r1, sp, #0xc
	add r0, r5, #0x70
	bl FUN_022470A8
	add r0, sp, #0x30
	mov r1, r4
	bl ov66_223B938
	ldr r1, [sp, #0xc]
	add r0, sp, #0x20
	add r2, sp, #0x30
	bl FUN_0224767C
	mov r0, #0
	ldr r1, [sp, #0x20]
	strb r0, [sp]
	strb r0, [sp, #1]
	str r1, [sp, #0x10]
	add r0, sp, #0x14
	add r1, sp, #0x24
	bl ov66_223B938
	add r1, r5, #0x70
	add r0, sp, #4
	add r2, sp, #0x10
	bl FUN_02247E38
	ldrb r4, [sp, #8]
	add r0, sp, #0x14
	bl ov66_223BADC
	add r0, sp, #0x24
	bl ov66_223BADC
	add r0, sp, #0x30
	bl ov66_223BADC
	cmp r4, #0
	ldrne r0, [sp, #0xc]
	mvneq r0, #0
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, pc}
	arm_func_end FUN_022475DC

	arm_func_start FUN_0224767C
FUN_0224767C: // 0x0224767C
	ldr ip, _0224768C // =ov66_223B938
	str r1, [r0], #4
	mov r1, r2
	bx ip
	.align 2, 0
_0224768C: .word ov66_223B938
	arm_func_end FUN_0224767C

	arm_func_start FUN_02247690
FUN_02247690: // 0x02247690
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	mov r2, r1
	add r0, sp, #0
	add r1, r4, #0x80
	bl FUN_02246B70
	ldr r1, [sp]
	add r0, r4, #0x84
	cmp r1, r0
	moveq r0, #0
	ldrne r0, [r1, #0x18]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_02247690

	arm_func_start FUN_022476C8
FUN_022476C8: // 0x022476C8
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	mov r2, r1
	add r0, sp, #0
	add r1, r4, #0x80
	bl FUN_02246B70
	ldr r1, [sp]
	add r0, r4, #0x84
	cmp r1, r0
	moveq r0, #0
	movne r0, #1
	strne r0, [r1, #0x18]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_022476C8

	arm_func_start FUN_02247704
FUN_02247704: // 0x02247704
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	mov r2, r1
	add r0, sp, #0
	add r1, r4, #0x80
	bl FUN_02246B70
	ldr r1, [sp]
	add r0, r4, #0x84
	cmp r1, r0
	mvneq r0, #0
	ldrne r0, [r1, #0x28]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_02247704

	arm_func_start FUN_0224773C
FUN_0224773C: // 0x0224773C
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r6, r2
	mov r7, r1
	mov r4, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r7
	mov r1, r6
	str ip, [sp]
	bl FUN_0224806C
	ldr r2, [sp, #8]
	mov r5, r0
	cmp r2, #0
	beq _0224779C
	mov r1, r6
	add r0, r2, #0xc
	bl ov66_223BE90
	cmp r0, #0
	movlt r0, #1
	movge r0, #0
	cmp r0, #0
	beq _022477CC
_0224779C:
	str r6, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r7
	mov r1, r5
	bl FUN_0224786C
	ldr r1, _022477E8 // =0x02259258
	str r0, [r4]
	ldrb r0, [r1, #4]
	add sp, sp, #0xc
	strb r0, [r4, #4]
	ldmia sp!, {r4, r5, r6, r7, pc}
_022477CC:
	ldr r1, [sp, #8]
	ldr r0, _022477E8 // =0x02259258
	str r1, [r4]
	ldrb r0, [r0, #5]
	strb r0, [r4, #4]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_022477E8: .word 0x02259258
	arm_func_end FUN_0224773C

	arm_func_start FUN_022477EC
FUN_022477EC: // 0x022477EC
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #0xc]
	mov r6, r0
	cmp r4, r1
	bne _0224781C
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #0xc]
_0224781C:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	add r0, r4, #0x1c
	bl FUN_02247E18
	add r0, r4, #0xc
	bl ov66_223BADC
	cmp r4, #0
	beq _02247854
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02247854:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_022477EC

	arm_func_start FUN_0224786C
FUN_0224786C: // 0x0224786C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	mov sb, r0
	ldr r4, [sb]
	mvn r0, #0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	cmp r4, r0
	ldr r5, [sp, #0x30]
	bne _022478A4
	ldr r0, _02247990 // =0x022592AC
	bl printf
	bl abort
_022478A4:
	mov r1, #0
	mov r0, #0x44
	strb r1, [sp, #1]
	bl FUN_02246290
	add r1, sb, #4
	str r0, [sp, #4]
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	adds r4, r0, #0xc
	beq _02247924
	mov r0, r4
	mov r1, r5
	bl ov66_223B938
	ldr r1, [r5, #0xc]
	mov r0, #0
	str r1, [r4, #0xc]
	str r0, [r4, #0x10]
	str r0, [r4, #0x14]
	add r0, r4, #0x14
	str r0, [r4, #0x18]
	add r0, r4, #0x10
	add r1, r5, #0x10
	bl FUN_02247ABC
	ldr r0, [r5, #0x1c]
	add r5, r5, #0x20
	str r0, [r4, #0x1c]
	add r4, r4, #0x20
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5, {r0, r1}
	stmia r4, {r0, r1}
_02247924:
	ldr r4, [sp, #0x10]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r8, [r0]
	cmp r7, #0
	strne r4, [r8]
	streq r4, [r8, #4]
	ldr r1, [sb]
	mov r0, r4
	add r1, r1, #1
	str r1, [sb]
	ldr r1, [sb, #4]
	bl ov66_223ADD8
	cmp r6, #0
	ldr r1, [sp, #0x10]
	strne r4, [sb, #0xc]
	cmp r1, #0
	beq _02247984
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_02247984:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_02247990: .word 0x022592AC
	arm_func_end FUN_0224786C

	arm_func_start FUN_02247994
FUN_02247994: // 0x02247994
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _022479AC
	bl FUN_022481D0
_022479AC:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_02247994

	arm_func_start FUN_022479B4
FUN_022479B4: // 0x022479B4
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r5, r2
	mov r6, r1
	mov r4, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r6
	mov r1, r5
	str ip, [sp]
	bl FUN_022480F8
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _02247A04
	ldr r2, [r3, #0xc]
	ldr r0, [r5]
	cmp r2, r0
	bge _02247A30
_02247A04:
	str r5, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r6
	bl FUN_02247D40
	ldr r1, _02247A48 // =0x02259258
	str r0, [r4]
	ldrb r0, [r1, #3]
	add sp, sp, #0xc
	strb r0, [r4, #4]
	ldmia sp!, {r3, r4, r5, r6, pc}
_02247A30:
	ldr r0, _02247A48 // =0x02259258
	str r3, [r4]
	ldrb r0, [r0, #7]
	strb r0, [r4, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_02247A48: .word 0x02259258
	arm_func_end FUN_022479B4

	arm_func_start FUN_02247A4C
FUN_02247A4C: // 0x02247A4C
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #8]
	mov r6, r0
	cmp r4, r1
	bne _02247A7C
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #8]
_02247A7C:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	cmp r4, #0
	beq _02247AA4
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02247AA4:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_02247A4C

	arm_func_start FUN_02247ABC
FUN_02247ABC: // 0x02247ABC
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x20
	mov r4, r1
	ldr r1, [r4, #4]
	mov r5, r0
	cmp r1, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	mov r0, #0x10
	strb r1, [sp, #1]
	bl FUN_02246290
	add r1, r5, #4
	str r1, [sp, #0xc]
	str r0, [sp, #4]
	str r0, [sp, #8]
	str r0, [sp, #0x10]
	adds r1, r0, #0xc
	ldr r0, [r4, #4]
	add r2, sp, #0x18
	ldrne r0, [r0, #0xc]
	mov r3, #0
	strne r0, [r1]
	ldr r1, [sp, #0x10]
	str r3, [sp, #0x14]
	str r3, [sp, #0x18]
	str r2, [sp, #0x1c]
	str r3, [r1, #4]
	str r3, [r1]
	ldr r0, [r1, #8]
	and r0, r0, #1
	orr r0, r2, r0
	str r0, [r1, #8]
	ldr r0, [r4, #4]
	ldr r1, [sp, #0x10]
	ldr r0, [r0, #8]
	tst r0, #1
	movne r3, #1
	cmp r3, #0
	ldr r0, [r1, #8]
	mov r3, #0
	orrne r0, r0, #1
	biceq r0, r0, #1
	str r0, [r1, #8]
	ldr r1, [sp, #0x10]
	mov r0, #1
	str r1, [sp, #0x18]
	str r0, [sp, #0x14]
	str r1, [sp, #0x1c]
	ldr r2, [r4, #4]
	str r3, [sp, #0x10]
	ldr r0, [r2, #4]
	cmp r0, #0
	beq _02247B9C
	add r0, sp, #0x14
	bl FUN_02247BF4
_02247B9C:
	ldr r2, [r4, #4]
	ldr r0, [r2]
	cmp r0, #0
	beq _02247BBC
	ldr r1, [sp, #0x18]
	add r0, sp, #0x14
	mov r3, #1
	bl FUN_02247BF4
_02247BBC:
	add r1, sp, #0x14
	mov r0, r5
	bl FUN_0224824C
	add r0, sp, #0x14
	bl FUN_0224822C
	ldr r1, [sp, #0x10]
	cmp r1, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r2, r0
	bl DWC_Free
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_02247ABC

	arm_func_start FUN_02247BF4
FUN_02247BF4: // 0x02247BF4
	stmfd sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x14
	mov r7, r0
	mov ip, #0
	mov r0, #0x10
	mov r6, r1
	mov r5, r2
	mov r4, r3
	strb ip, [sp]
	bl FUN_02246290
	mov r1, #0
	str r1, [r0, #4]
	str r1, [r0]
	ldr r1, [r0, #8]
	add r2, r7, #4
	and r1, r1, #1
	orr r1, r6, r1
	str r0, [sp, #4]
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r1, [r0, #8]
	cmp r4, #0
	beq _02247C84
	adds r1, r0, #0xc
	ldr r0, [r5]
	ldrne r0, [r0, #0xc]
	strne r0, [r1]
	ldr r1, [sp, #0x10]
	mov r0, #0
	str r1, [r7, #8]
	str r1, [r6]
	mov r6, r1
	str r0, [sp, #0x10]
	ldr r5, [r5]
	b _02247CAC
_02247C84:
	adds r1, r0, #0xc
	ldr r0, [r5, #4]
	ldrne r0, [r0, #0xc]
	strne r0, [r1]
	ldr r1, [sp, #0x10]
	mov r0, #0
	str r1, [r6, #4]
	ldr r5, [r5, #4]
	mov r6, r1
	str r0, [sp, #0x10]
_02247CAC:
	ldr r0, [r5, #8]
	tst r0, #1
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	ldr r0, [r6, #8]
	orrne r0, r0, #1
	biceq r0, r0, #1
	str r0, [r6, #8]
	ldr r0, [r7]
	add r0, r0, #1
	str r0, [r7]
	ldr r0, [r5, #4]
	cmp r0, #0
	beq _02247CFC
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, #0
	bl FUN_02247BF4
_02247CFC:
	ldr r0, [r5]
	cmp r0, #0
	beq _02247D1C
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, #1
	bl FUN_02247BF4
_02247D1C:
	ldr r1, [sp, #0x10]
	cmp r1, #0
	addeq sp, sp, #0x14
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r0, #0
	mov r2, r0
	bl DWC_Free
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end FUN_02247BF4

	arm_func_start FUN_02247D40
FUN_02247D40: // 0x02247D40
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	mov r8, r0
	ldr r4, [r8]
	mvn r0, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	cmp r4, r0
	bne _02247D74
	ldr r0, _02247E14 // =0x022592AC
	bl printf
	bl abort
_02247D74:
	mov r1, #0
	mov r0, #0x10
	strb r1, [sp]
	bl FUN_02246290
	mov r4, r0
	add r0, r8, #4
	str r0, [sp, #0xc]
	adds r1, r4, #0xc
	ldrne r0, [sp, #0x30]
	str r4, [sp, #0x10]
	ldrne r0, [r0]
	str r4, [sp, #4]
	strne r0, [r1]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r7, [r0]
	cmp r6, #0
	strne r4, [r7]
	streq r4, [r7, #4]
	ldr r1, [r8]
	mov r0, r4
	add r1, r1, #1
	str r1, [r8]
	ldr r1, [r8, #4]
	str r4, [sp, #8]
	bl ov66_223ADD8
	cmp r5, #0
	ldr r1, [sp, #0x10]
	strne r4, [r8, #8]
	cmp r1, #0
	beq _02247E08
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_02247E08:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02247E14: .word 0x022592AC
	arm_func_end FUN_02247D40

	arm_func_start FUN_02247E18
FUN_02247E18: // 0x02247E18
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02247E30
	bl FUN_022482D8
_02247E30:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_02247E18

	arm_func_start FUN_02247E38
FUN_02247E38: // 0x02247E38
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r5, r2
	mov r6, r1
	mov r4, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r6
	mov r1, r5
	str ip, [sp]
	bl FUN_02248164
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _02247E88
	ldr r2, [r3, #0xc]
	ldr r0, [r5]
	cmp r2, r0
	bge _02247EB4
_02247E88:
	str r5, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r6
	bl FUN_02247F48
	ldr r1, _02247ECC // =0x02259258
	str r0, [r4]
	ldrb r0, [r1]
	add sp, sp, #0xc
	strb r0, [r4, #4]
	ldmia sp!, {r3, r4, r5, r6, pc}
_02247EB4:
	ldr r0, _02247ECC // =0x02259258
	str r3, [r4]
	ldrb r0, [r0, #1]
	strb r0, [r4, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_02247ECC: .word 0x02259258
	arm_func_end FUN_02247E38

	arm_func_start FUN_02247ED0
FUN_02247ED0: // 0x02247ED0
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r4, [sp, #0x18]
	ldr r1, [r5, #0xc]
	mov r6, r0
	cmp r4, r1
	bne _02247F00
	add r0, sp, #0x18
	bl ov66_223AD6C
	ldr r0, [sp, #0x18]
	str r0, [r5, #0xc]
_02247F00:
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov66_223AFE4
	str r0, [r6]
	add r0, r4, #0x10
	bl ov66_223BADC
	cmp r4, #0
	beq _02247F30
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
_02247F30:
	ldr r0, [r5]
	sub r0, r0, #1
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end FUN_02247ED0

	arm_func_start FUN_02247F48
FUN_02247F48: // 0x02247F48
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	mov r8, r0
	ldr r4, [r8]
	mvn r0, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	cmp r4, r0
	bne _02247F7C
	ldr r0, _02248028 // =0x022592AC
	bl printf
	bl abort
_02247F7C:
	mov r1, #0
	mov r0, #0x1c
	strb r1, [sp]
	bl FUN_02246290
	add r1, r8, #4
	str r0, [sp, #4]
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	adds r3, r0, #0xc
	beq _02247FBC
	ldr r1, [sp, #0x30]
	add r0, r3, #4
	ldr r2, [r1], #4
	str r2, [r3]
	bl ov66_223B938
_02247FBC:
	ldr r4, [sp, #0x10]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r7, [r0]
	cmp r6, #0
	strne r4, [r7]
	streq r4, [r7, #4]
	ldr r1, [r8]
	mov r0, r4
	add r1, r1, #1
	str r1, [r8]
	ldr r1, [r8, #4]
	bl ov66_223ADD8
	cmp r5, #0
	ldr r1, [sp, #0x10]
	strne r4, [r8, #0xc]
	cmp r1, #0
	beq _0224801C
	mov r0, #0
	mov r2, r0
	bl DWC_Free
_0224801C:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02248028: .word 0x022592AC
	arm_func_end FUN_02247F48

	arm_func_start FUN_0224802C
FUN_0224802C: // 0x0224802C
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248044
	bl FUN_02248324
_02248044:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_0224802C

	arm_func_start FUN_0224804C
FUN_0224804C: // 0x0224804C
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248064
	bl FUN_02248378
_02248064:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_0224804C

	arm_func_start FUN_0224806C
FUN_0224806C: // 0x0224806C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r2
	mov r2, #0
	str r2, [sb]
	ldr r6, [r0, #4]
	mov r8, r3
	mov r2, #1
	ldr r7, [sp, #0x28]
	strb r2, [r8]
	mov sl, r1
	strb r2, [r7]
	cmp r6, #0
	add r5, r0, #4
	beq _022480F0
	mov fp, #0
	mov r4, r2
_022480AC:
	mov r0, sl
	add r1, r6, #0xc
	mov r5, r6
	bl ov66_223BE90
	cmp r0, #0
	movlt r0, #1
	movge r0, #0
	cmp r0, #0
	ldrne r6, [r6]
	strneb r4, [r8]
	bne _022480E8
	str r6, [sb]
	ldr r6, [r6, #4]
	strb fp, [r8]
	strb fp, [r7]
_022480E8:
	cmp r6, #0
	bne _022480AC
_022480F0:
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end FUN_0224806C

	arm_func_start FUN_022480F8
FUN_022480F8: // 0x022480F8
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_0224812C:
	ldr r5, [r1]
	ldr r4, [r6, #0xc]
	mov r0, r6
	cmp r5, r4
	ldrlt r6, [r6]
	strltb lr, [r3]
	blt _02248158
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_02248158:
	cmp r6, #0
	bne _0224812C
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_022480F8

	arm_func_start FUN_02248164
FUN_02248164: // 0x02248164
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_02248198:
	ldr r5, [r1]
	ldr r4, [r6, #0xc]
	mov r0, r6
	cmp r5, r4
	ldrlt r6, [r6]
	strltb lr, [r3]
	blt _022481C4
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_022481C4:
	cmp r6, #0
	bne _02248198
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_02248164

	arm_func_start FUN_022481D0
FUN_022481D0: // 0x022481D0
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _022481EC
	bl FUN_022481D0
_022481EC:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248200
	mov r0, r5
	bl FUN_022481D0
_02248200:
	add r0, r4, #0x1c
	bl FUN_02247E18
	add r0, r4, #0xc
	bl ov66_223BADC
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_022481D0

	arm_func_start FUN_0224822C
FUN_0224822C: // 0x0224822C
	stmfd sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248244
	bl FUN_022482D8
_02248244:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end FUN_0224822C

	arm_func_start FUN_0224824C
FUN_0224824C: // 0x0224824C
	cmp r0, r1
	bxeq lr
	ldr r3, [r0]
	ldr r2, [r1]
	str r2, [r0]
	str r3, [r1]
	ldr r3, [r0, #4]
	ldr r2, [r1, #4]
	str r2, [r0, #4]
	str r3, [r1, #4]
	ldr r3, [r0, #8]
	ldr r2, [r1, #8]
	str r2, [r0, #8]
	str r3, [r1, #8]
	ldr ip, [r0, #4]
	cmp ip, #0
	addeq r2, r0, #4
	streq r2, [r0, #8]
	beq _022482AC
	ldr r2, [ip, #8]
	add r3, r0, #4
	and r0, r2, #1
	orr r0, r3, r0
	str r0, [ip, #8]
_022482AC:
	ldr r2, [r1, #4]
	cmp r2, #0
	addeq r0, r1, #4
	streq r0, [r1, #8]
	bxeq lr
	ldr r0, [r2, #8]
	add r1, r1, #4
	and r0, r0, #1
	orr r0, r1, r0
	str r0, [r2, #8]
	bx lr
	arm_func_end FUN_0224824C

	arm_func_start FUN_022482D8
FUN_022482D8: // 0x022482D8
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _022482F4
	bl FUN_022482D8
_022482F4:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248308
	mov r0, r5
	bl FUN_022482D8
_02248308:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_022482D8

	arm_func_start FUN_02248324
FUN_02248324: // 0x02248324
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _02248340
	bl FUN_02248324
_02248340:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02248354
	mov r0, r5
	bl FUN_02248324
_02248354:
	add r0, r4, #0x10
	bl ov66_223BADC
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_02248324

	arm_func_start FUN_02248378
FUN_02248378: // 0x02248378
	stmfd sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _02248394
	bl FUN_02248378
_02248394:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _022483A8
	mov r0, r5
	bl FUN_02248378
_022483A8:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl DWC_Free
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_02248378
_022483C4:
	.byte 0x10, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x18, 0x00, 0x9F, 0xE5, 0x18, 0x10, 0x9F, 0xE5, 0x18, 0x20, 0x9F, 0xE5, 0xA4, 0x30, 0xA0, 0xE3
	.byte 0x25, 0x2F, 0xFA, 0xEB, 0x14, 0x00, 0x94, 0xE5, 0x30, 0x3D, 0xFE, 0xEB, 0x10, 0x80, 0xBD, 0xE8
	.byte 0x58, 0x93, 0x25, 0x02, 0x68, 0x93, 0x25, 0x02, 0x38, 0x93, 0x25, 0x02

	arm_func_start FUN_022483FC
FUN_022483FC: // 0x022483FC
	stmfd sp!, {r4, r5, r6, lr}
	movs r4, r0
	bne _0224841C
	ldr r0, _02248A70 // =0x02259358
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_0224841C:
	ldr r0, [r4]
	cmp r0, #0
	bge _0224843C
	ldr r0, _02248A7C // =0x02259378
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_0224843C:
	ldr r0, [r4]
	cmp r0, #0x20
	blt _0224845C
	ldr r0, _02248A80 // =0x02259388
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_0224845C:
	ldr r0, [r4, #4]
	cmp r0, #0
	bne _0224847C
	ldr r0, _02248A84 // =0x022593A4
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_0224847C:
	ldr r0, [r4, #8]
	cmp r0, #0
	bne _0224849C
	ldr r0, _02248A88 // =0x022593BC
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_0224849C:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	bge _022484BC
	ldr r0, _02248A8C // =0x022593DC
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	mov r3, #0xab
	bl __msl_assertion_failed
_022484BC:
	ldr r0, [r4]
	cmp r0, #0x1f
	addls pc, pc, r0, lsl #2
	b _02248A50
_022484CC: // jump table
	b _0224854C // case 0
	b _0224855C // case 1
	b _0224856C // case 2
	b _02248584 // case 3
	b _0224859C // case 4
	b _022485BC // case 5
	b _022485DC // case 6
	b _022485F4 // case 7
	b _0224861C // case 8
	b _0224863C // case 9
	b _02248654 // case 10
	b _0224866C // case 11
	b _02248684 // case 12
	b _02248694 // case 13
	b _022486AC // case 14
	b _02248704 // case 15
	b _02248714 // case 16
	b _0224872C // case 17
	b _02248744 // case 18
	b _0224875C // case 19
	b _022487A8 // case 20
	b _02248804 // case 21
	b _02248824 // case 22
	b _0224884C // case 23
	b _02248864 // case 24
	b _022488A8 // case 25
	b _022488EC // case 26
	b _02248904 // case 27
	b _02248950 // case 28
	b _02248978 // case 29
	b _022489D8 // case 30
	b _02248A40 // case 31
_0224854C:
	ldr r0, [r4, #8]
	ldr r0, [r0]
	bl DWCi_GsFree
	b _02248A64
_0224855C:
	ldr r0, [r4, #8]
	ldr r0, [r0]
	bl DWCi_GsFree
	b _02248A64
_0224856C:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_02248584:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_0224859C:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_022485BC:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_022485DC:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_022485F4:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	ldr r0, [r5, #0x10]
	bl DWCi_GsFree
	b _02248A64
_0224861C:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_0224863C:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_02248654:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_0224866C:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	b _02248A64
_02248684:
	ldr r0, [r4, #8]
	ldr r0, [r0]
	bl DWCi_GsFree
	b _02248A64
_02248694:
	ldr r5, [r4, #8]
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	b _02248A64
_022486AC:
	ldr r6, [r4, #8]
	mov r5, #0
	ldr r0, [r6, #4]
	cmp r0, #0
	ble _022486E8
_022486C0:
	ldr r0, [r6, #8]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #4]
	add r5, r5, #1
	cmp r5, r0
	blt _022486C0
_022486E8:
	ldr r0, [r6, #8]
	bl DWCi_GsFree
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	ldr r0, [r6, #0x10]
	bl DWCi_GsFree
	b _02248A64
_02248704:
	ldr r0, [r4, #8]
	ldr r0, [r0, #8]
	bl DWCi_GsFree
	b _02248A64
_02248714:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_0224872C:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_02248744:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	b _02248A64
_0224875C:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	mov r5, #0
	cmp r0, #0
	ble _02248794
_02248778:
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	add r5, r5, #1
	cmp r5, r0
	blt _02248778
_02248794:
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	ldr r0, [r6, #0x10]
	bl DWCi_GsFree
	b _02248A64
_022487A8:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	bl DWCi_GsFree
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	ldr r0, [r6, #0x10]
	bl DWCi_GsFree
	ldr r0, [r6, #0x14]
	mov r5, #0
	cmp r0, #0
	ble _022487F8
_022487DC:
	ldr r0, [r6, #0x18]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #0x14]
	add r5, r5, #1
	cmp r5, r0
	blt _022487DC
_022487F8:
	ldr r0, [r6, #0x18]
	bl DWCi_GsFree
	b _02248A64
_02248804:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	b _02248A64
_02248824:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	ldr r0, [r5, #0x10]
	bl DWCi_GsFree
	b _02248A64
_0224884C:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_02248864:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	mov r5, #0
	cmp r0, #0
	ble _0224889C
_02248880:
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	add r5, r5, #1
	cmp r5, r0
	blt _02248880
_0224889C:
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	b _02248A64
_022488A8:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	mov r5, #0
	cmp r0, #0
	ble _022488E0
_022488C4:
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	add r5, r5, #1
	cmp r5, r0
	blt _022488C4
_022488E0:
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	b _02248A64
_022488EC:
	ldr r5, [r4, #8]
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	b _02248A64
_02248904:
	ldr r6, [r4, #8]
	ldr r0, [r6]
	bl DWCi_GsFree
	ldr r0, [r6, #4]
	mov r5, #0
	cmp r0, #0
	ble _0224893C
_02248920:
	ldr r0, [r6, #8]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #4]
	add r5, r5, #1
	cmp r5, r0
	blt _02248920
_0224893C:
	ldr r0, [r6, #8]
	bl DWCi_GsFree
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	b _02248A64
_02248950:
	ldr r5, [r4, #8]
	ldr r0, [r5]
	bl DWCi_GsFree
	ldr r0, [r5, #4]
	bl DWCi_GsFree
	ldr r0, [r5, #8]
	bl DWCi_GsFree
	ldr r0, [r5, #0xc]
	bl DWCi_GsFree
	b _02248A64
_02248978:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	mov r5, #0
	cmp r0, #0
	ble _022489C4
_02248994:
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #0x10]
	cmp r0, #0
	beq _022489B4
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
_022489B4:
	ldr r0, [r6, #8]
	add r5, r5, #1
	cmp r5, r0
	blt _02248994
_022489C4:
	ldr r0, [r6, #0xc]
	bl DWCi_GsFree
	ldr r0, [r6, #0x10]
	bl DWCi_GsFree
	b _02248A64
_022489D8:
	ldr r6, [r4, #8]
	ldr r0, [r6, #4]
	bl DWCi_GsFree
	ldr r0, [r6, #8]
	bl DWCi_GsFree
	ldr r0, [r6, #0xc]
	mov r5, #0
	cmp r0, #0
	ble _02248A2C
_022489FC:
	ldr r0, [r6, #0x10]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r6, #0x14]
	cmp r0, #0
	beq _02248A1C
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
_02248A1C:
	ldr r0, [r6, #0xc]
	add r5, r5, #1
	cmp r5, r0
	blt _022489FC
_02248A2C:
	ldr r0, [r6, #0x10]
	bl DWCi_GsFree
	ldr r0, [r6, #0x14]
	bl DWCi_GsFree
	b _02248A64
_02248A40:
	ldr r0, [r4, #8]
	ldr r0, [r0, #4]
	bl DWCi_GsFree
	b _02248A64
_02248A50:
	ldr r0, _02248A90 // =0x022593EC
	ldr r1, _02248A74 // =0x02259368
	ldr r2, _02248A78 // =0x022592F8
	ldr r3, _02248A94 // =0x000001E5
	bl __msl_assertion_failed
_02248A64:
	ldr r0, [r4, #8]
	bl DWCi_GsFree
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02248A70: .word 0x02259358
_02248A74: .word 0x02259368
_02248A78: .word 0x022592F8
_02248A7C: .word 0x02259378
_02248A80: .word 0x02259388
_02248A84: .word 0x022593A4
_02248A88: .word 0x022593BC
_02248A8C: .word 0x022593DC
_02248A90: .word 0x022593EC
_02248A94: .word 0x000001E5
	arm_func_end FUN_022483FC

	arm_func_start FUN_02248A98
FUN_02248A98: // 0x02248A98
	stmfd sp!, {r4, lr}
	ldr r2, _02248AC4 // =0x022483C4
	mov r4, r0
	mov r0, #0x18
	mov r1, #0x80
	bl ArrayNew
	str r0, [r4, #0x820]
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r4, pc}
	.align 2, 0
_02248AC4: .word 0x022483C4
	arm_func_end FUN_02248A98

	arm_func_start FUN_02248AC8
FUN_02248AC8: // 0x02248AC8
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	movs sl, r0
	bne _02248AE8
	ldr r0, _02248C00 // =0x022593F0
	ldr r1, _02248C04 // =0x02259368
	ldr r2, _02248C08 // =0x0225930C
	ldr r3, _02248C0C // =0x000001FA
	bl __msl_assertion_failed
_02248AE8:
	ldr r0, [sl, #0x820]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl ArrayLength
	mov r8, r0
	cmp r8, #0
	mov sb, #0
	ble _02248BF4
	ldr fp, _02248C10 // =0x02259358
	ldr r6, _02248C04 // =0x02259368
	ldr r5, _02248C08 // =0x0225930C
	ldr r4, _02248C14 // =0x0000020D
_02248B18:
	ldr r0, [sl, #0x820]
	mov r1, sb
	bl ArrayNth
	movs r7, r0
	bne _02248B40
	mov r0, fp
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248B40:
	ldr r0, [r7]
	cmp r0, #0
	bge _02248B60
	ldr r0, _02248C18 // =0x02259378
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248B60:
	ldr r0, [r7]
	cmp r0, #0x20
	blt _02248B80
	ldr r0, _02248C1C // =0x02259388
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248B80:
	ldr r0, [r7, #4]
	cmp r0, #0
	bne _02248BA0
	ldr r0, _02248C20 // =0x022593A4
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248BA0:
	ldr r0, [r7, #8]
	cmp r0, #0
	bne _02248BC0
	ldr r0, _02248C24 // =0x022593BC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248BC0:
	ldr r0, [r7, #0x10]
	cmp r0, #0
	bge _02248BE0
	ldr r0, _02248C28 // =0x022593DC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_02248BE0:
	mov r0, r7
	bl FUN_022483FC
	add sb, sb, #1
	cmp sb, r8
	blt _02248B18
_02248BF4:
	ldr r0, [sl, #0x820]
	bl ArrayFree
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_02248C00: .word 0x022593F0
_02248C04: .word 0x02259368
_02248C08: .word 0x0225930C
_02248C0C: .word 0x000001FA
_02248C10: .word 0x02259358
_02248C14: .word 0x0000020D
_02248C18: .word 0x02259378
_02248C1C: .word 0x02259388
_02248C20: .word 0x022593A4
_02248C24: .word 0x022593BC
_02248C28: .word 0x022593DC
	arm_func_end FUN_02248AC8

	arm_func_start FUN_02248C2C
FUN_02248C2C: // 0x02248C2C
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x18
	movs r5, r0
	mov r7, r1
	mov r6, r2
	mov r4, r3
	bne _02248C5C
	ldr r0, _02249B5C // =0x022593F0
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B68 // =0x0000021F
	bl __msl_assertion_failed
_02248C5C:
	cmp r7, #0
	bge _02248C78
	ldr r0, _02249B6C // =0x02259400
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B70 // =0x00000221
	bl __msl_assertion_failed
_02248C78:
	cmp r7, #0x20
	blt _02248C94
	ldr r0, _02249B74 // =0x0225940C
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B78 // =0x00000222
	bl __msl_assertion_failed
_02248C94:
	ldr r0, [r5, #0x820]
	cmp r0, #0
	bne _02248CB4
	ldr r0, _02249B7C // =0x02259420
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B80 // =0x00000223
	bl __msl_assertion_failed
_02248CB4:
	cmp r6, #0
	bne _02248CD0
	ldr r0, _02249B84 // =0x02259444
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	mov r3, #0x224
	bl __msl_assertion_failed
_02248CD0:
	cmp r4, #0
	bne _02248CEC
	ldr r0, _02249B88 // =0x02259458
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B8C // =0x00000225
	bl __msl_assertion_failed
_02248CEC:
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	bne _02248D0C
	ldr r0, _02249B90 // =0x02259470
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B94 // =0x00000226
	bl __msl_assertion_failed
_02248D0C:
	ldr r0, [sp, #0x34]
	cmp r0, #0
	bge _02248D2C
	ldr r0, _02249B98 // =0x02259488
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249B9C // =0x00000227
	bl __msl_assertion_failed
_02248D2C:
	add r2, sp, #0
	mov r1, #0
	str r1, [r2]
	str r1, [r2, #4]
	ldr r0, [sp, #0x3c]
	str r1, [r2, #8]
	str r1, [r2, #0xc]
	str r1, [r2, #0x10]
	str r1, [r2, #0x14]
	str r7, [sp]
	str r6, [sp, #4]
	bl DWCi_GsMalloc
	str r0, [sp, #8]
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r2, [sp, #0x3c]
	mov r1, r4
	bl memcpy
	ldr r0, [sp, #0x38]
	ldr r2, [sp, #0x30]
	ldr r1, [sp, #0x34]
	cmp r0, #0
	moveq r0, #0
	str r2, [sp, #0xc]
	str r1, [sp, #0x10]
	streq r0, [sp, #0x14]
	beq _02248DDC
	bl strlen
	mov r6, r0
	add r0, r6, #1
	bl DWCi_GsMalloc
	str r0, [sp, #0x14]
	cmp r0, #0
	bne _02248DD0
	ldr r0, [sp, #8]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248DD0:
	ldr r1, [sp, #0x38]
	add r2, r6, #1
	bl memcpy
_02248DDC:
	ldr r0, [sp]
	cmp r0, #0x1f
	addls pc, pc, r0, lsl #2
	b _0224AA48
_02248DEC: // jump table
	b _02248E6C // case 0
	b _02248EA8 // case 1
	b _02248EE4 // case 2
	b _02248F58 // case 3
	b _02248FCC // case 4
	b _02249078 // case 5
	b _02249124 // case 6
	b _02249198 // case 7
	b _0224927C // case 8
	b _02249328 // case 9
	b _0224939C // case 10
	b _02249424 // case 11
	b _02249498 // case 12
	b _022494D4 // case 13
	b _02249548 // case 14
	b _022497E0 // case 15
	b _0224981C // case 16
	b _02249890 // case 17
	b _02249918 // case 18
	b _0224998C // case 19
	b _02249BC0 // case 20
	b _02249DA8 // case 21
	b _02249E54 // case 22
	b _02249F38 // case 23
	b _02249FAC // case 24
	b _0224A0EC // case 25
	b _0224A22C // case 26
	b _0224A2A0 // case 27
	b _0224A470 // case 28
	b _0224A554 // case 29
	b _0224A794 // case 30
	b _0224AA0C // case 31
_02248E6C:
	ldr r0, [r4]
	ldr r4, [sp, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r4
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248EA8:
	ldr r0, [r4]
	ldr r4, [sp, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r4
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248EE4:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02248F20
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02248F20
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248F20:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248F58:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02248F94
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02248F94
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248F94:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02248FCC:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249008
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02249008
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249008:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _02249040
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249040
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249040:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249078:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _022490B4
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _022490B4
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022490B4:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _022490EC
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _022490EC
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022490EC:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249124:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249160
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02249160
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249160:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249198:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _022491D4
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _022491D4
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022491D4:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224920C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224920C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224920C:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _02249244
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _02249244
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249244:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0x10]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224927C:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _022492B8
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _022492B8
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022492B8:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _022492F0
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _022492F0
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022492F0:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249328:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249364
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02249364
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249364:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224939C:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _022493D8
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _022493D8
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022493D8:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	mov r0, #0x20
	bl DWCi_GsMalloc
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249414
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249414:
	ldr r1, [r4, #4]
	mov r2, #0x20
	bl memcpy
	b _0224AA5C
_02249424:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249460
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _02249460
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249460:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249498:
	ldr r0, [r4]
	ldr r4, [sp, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r4
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022494D4:
	ldr r0, [r4, #8]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249510
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _02249510
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249510:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249548:
	ldr r0, [r4, #4]
	ldr r8, [sp, #8]
	cmp r0, #0
	bge _0224956C
	ldr r0, _02249BA0 // =0x02259490
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BA4 // =0x000002D3
	bl __msl_assertion_failed
_0224956C:
	ldr r0, [r4, #8]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r8, #8]
	beq _0224964C
	ldr r0, [r4, #4]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r8, #8]
	cmp r0, #0
	bne _022495B4
	mov r0, r8
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022495B4:
	ldr r0, [r4, #4]
	mov r7, #0
	cmp r0, #0
	ble _0224964C
	mov r6, r7
_022495C8:
	ldr r0, [r4, #8]
	ldr r0, [r0, r7, lsl #2]
	cmp r0, #0
	ldreq r0, [r8, #8]
	streq r6, [r0, r7, lsl #2]
	beq _0224963C
	bl goastrdup
	ldr r1, [r8, #8]
	str r0, [r1, r7, lsl #2]
	ldr r0, [r8, #8]
	ldr r0, [r0, r7, lsl #2]
	cmp r0, #0
	bne _0224963C
	subs r7, r7, #1
	bmi _02249618
_02249604:
	ldr r0, [r8, #8]
	ldr r0, [r0, r7, lsl #2]
	bl DWCi_GsFree
	subs r7, r7, #1
	bpl _02249604
_02249618:
	ldr r0, [r8, #8]
	bl DWCi_GsFree
	mov r0, r8
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224963C:
	ldr r0, [r4, #4]
	add r7, r7, #1
	cmp r7, r0
	blt _022495C8
_0224964C:
	ldr r0, [r4, #4]
	cmp r0, #0
	bge _0224966C
	ldr r0, _02249BA0 // =0x02259490
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	mov r3, #0x2d4
	bl __msl_assertion_failed
_0224966C:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r8, #0xc]
	beq _0224974C
	ldr r0, [r4, #4]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r8, #0xc]
	cmp r0, #0
	bne _022496B4
	mov r0, r8
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022496B4:
	ldr r0, [r4, #4]
	mov r7, #0
	cmp r0, #0
	ble _0224974C
	mov r6, r7
_022496C8:
	ldr r0, [r4, #0xc]
	ldr r0, [r0, r7, lsl #2]
	cmp r0, #0
	ldreq r0, [r8, #0xc]
	streq r6, [r0, r7, lsl #2]
	beq _0224973C
	bl goastrdup
	ldr r1, [r8, #0xc]
	str r0, [r1, r7, lsl #2]
	ldr r0, [r8, #0xc]
	ldr r0, [r0, r7, lsl #2]
	cmp r0, #0
	bne _0224973C
	subs r7, r7, #1
	bmi _02249718
_02249704:
	ldr r0, [r8, #0xc]
	ldr r0, [r0, r7, lsl #2]
	bl DWCi_GsFree
	subs r7, r7, #1
	bpl _02249704
_02249718:
	ldr r0, [r8, #0xc]
	bl DWCi_GsFree
	mov r0, r8
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224973C:
	ldr r0, [r4, #4]
	add r7, r7, #1
	cmp r7, r0
	blt _022496C8
_0224974C:
	ldr r0, [r4, #4]
	cmp r0, #0
	bge _0224976C
	ldr r0, _02249BA0 // =0x02259490
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BA8 // =0x000002D5
	bl __msl_assertion_failed
_0224976C:
	ldr r0, [r4, #4]
	cmp r0, #0
	ble _0224AA5C
	ldr r0, [r4, #0x10]
	cmp r0, #0
	bne _02249798
	ldr r0, _02249BAC // =0x022594B0
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BA8 // =0x000002D5
	bl __msl_assertion_failed
_02249798:
	ldr r0, [r4, #4]
	mov r6, r0, lsl #2
	mov r0, r6
	bl DWCi_GsMalloc
	str r0, [r8, #0x10]
	cmp r0, #0
	bne _022497D0
	mov r0, r8
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022497D0:
	ldr r1, [r4, #0x10]
	mov r2, r6
	bl memcpy
	b _0224AA5C
_022497E0:
	ldr r0, [r4, #8]
	ldr r4, [sp, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r4, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r4
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224981C:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249858
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249858
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249858:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249890:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _022498CC
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _022498CC
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022498CC:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	mov r0, #0x20
	bl DWCi_GsMalloc
	str r0, [r6, #8]
	cmp r0, #0
	bne _02249908
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249908:
	ldr r1, [r4, #8]
	mov r2, #0x20
	bl memcpy
	b _0224AA5C
_02249918:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249954
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249954
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249954:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224998C:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _022499C8
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _022499C8
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022499C8:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _022499E8
	ldr r0, _02249BB0 // =0x022594CC
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BB4 // =0x00000301
	bl __msl_assertion_failed
_022499E8:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0xc]
	beq _02249AC8
	ldr r0, [r4, #8]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _02249A30
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249A30:
	ldr r0, [r4, #8]
	mov r8, #0
	cmp r0, #0
	ble _02249AC8
	mov r6, r8
_02249A44:
	ldr r0, [r4, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0xc]
	streq r6, [r0, r8, lsl #2]
	beq _02249AB8
	bl goastrdup
	ldr r1, [r7, #0xc]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _02249AB8
	subs r8, r8, #1
	bmi _02249A94
_02249A80:
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _02249A80
_02249A94:
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249AB8:
	ldr r0, [r4, #8]
	add r8, r8, #1
	cmp r8, r0
	blt _02249A44
_02249AC8:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _02249AE8
	ldr r0, _02249BB0 // =0x022594CC
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BB8 // =0x00000302
	bl __msl_assertion_failed
_02249AE8:
	ldr r0, [r4, #8]
	cmp r0, #0
	ble _0224AA5C
	ldr r0, [r4, #0x10]
	cmp r0, #0
	bne _02249B14
	ldr r0, _02249BBC // =0x022594E8
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _02249BB8 // =0x00000302
	bl __msl_assertion_failed
_02249B14:
	ldr r0, [r4, #8]
	mov r6, r0, lsl #2
	mov r0, r6
	bl DWCi_GsMalloc
	str r0, [r7, #0x10]
	cmp r0, #0
	bne _02249B4C
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249B4C:
	ldr r1, [r4, #0x10]
	mov r2, r6
	bl memcpy
	b _0224AA5C
	.align 2, 0
_02249B5C: .word 0x022593F0
_02249B60: .word 0x02259368
_02249B64: .word 0x022592C8
_02249B68: .word 0x0000021F
_02249B6C: .word 0x02259400
_02249B70: .word 0x00000221
_02249B74: .word 0x0225940C
_02249B78: .word 0x00000222
_02249B7C: .word 0x02259420
_02249B80: .word 0x00000223
_02249B84: .word 0x02259444
_02249B88: .word 0x02259458
_02249B8C: .word 0x00000225
_02249B90: .word 0x02259470
_02249B94: .word 0x00000226
_02249B98: .word 0x02259488
_02249B9C: .word 0x00000227
_02249BA0: .word 0x02259490
_02249BA4: .word 0x000002D3
_02249BA8: .word 0x000002D5
_02249BAC: .word 0x022594B0
_02249BB0: .word 0x022594CC
_02249BB4: .word 0x00000301
_02249BB8: .word 0x00000302
_02249BBC: .word 0x022594E8
_02249BC0:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _02249BFC
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _02249BFC
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249BFC:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _02249C34
	bl goastrdup
	str r0, [r7, #8]
	cmp r0, #0
	bne _02249C34
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249C34:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _02249C6C
	bl goastrdup
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _02249C6C
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249C6C:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _02249CA4
	bl goastrdup
	str r0, [r7, #0x10]
	cmp r0, #0
	bne _02249CA4
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249CA4:
	ldr r0, [r4, #0x14]
	cmp r0, #0
	bge _02249CC4
	ldr r0, _02249BA0 // =0x02259490
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA74 // =0x0000030E
	bl __msl_assertion_failed
_02249CC4:
	ldr r0, [r4, #0x18]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0x18]
	beq _0224AA5C
	ldr r0, [r4, #0x14]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0x18]
	cmp r0, #0
	bne _02249D0C
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249D0C:
	ldr r0, [r4, #0x14]
	mov r8, #0
	cmp r0, #0
	ble _0224AA5C
	mov r6, r8
_02249D20:
	ldr r0, [r4, #0x18]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0x18]
	streq r6, [r0, r8, lsl #2]
	beq _02249D94
	bl goastrdup
	ldr r1, [r7, #0x18]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0x18]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _02249D94
	subs r8, r8, #1
	bmi _02249D70
_02249D5C:
	ldr r0, [r7, #0x18]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _02249D5C
_02249D70:
	ldr r0, [r7, #0x18]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249D94:
	ldr r0, [r4, #0x14]
	add r8, r8, #1
	cmp r8, r0
	blt _02249D20
	b _0224AA5C
_02249DA8:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249DE4
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249DE4
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249DE4:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _02249E1C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _02249E1C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249E1C:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249E54:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249E90
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249E90
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249E90:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _02249EC8
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _02249EC8
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249EC8:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _02249F00
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _02249F00
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249F00:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0x10]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249F38:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _02249F74
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _02249F74
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249F74:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249FAC:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _02249FE8
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _02249FE8
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02249FE8:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _0224A008
	ldr r0, _0224AA78 // =0x02259504
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA7C // =0x00000335
	bl __msl_assertion_failed
_0224A008:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0xc]
	beq _0224AA5C
	ldr r0, [r4, #8]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _0224A050
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A050:
	ldr r0, [r4, #8]
	mov r8, #0
	cmp r0, #0
	ble _0224AA5C
	mov r6, r8
_0224A064:
	ldr r0, [r4, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0xc]
	streq r6, [r0, r8, lsl #2]
	beq _0224A0D8
	bl goastrdup
	ldr r1, [r7, #0xc]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A0D8
	subs r8, r8, #1
	bmi _0224A0B4
_0224A0A0:
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A0A0
_0224A0B4:
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A0D8:
	ldr r0, [r4, #8]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A064
	b _0224AA5C
_0224A0EC:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _0224A128
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _0224A128
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A128:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _0224A148
	ldr r0, _0224AA80 // =0x02259520
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA84 // =0x0000033E
	bl __msl_assertion_failed
_0224A148:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0xc]
	beq _0224AA5C
	ldr r0, [r4, #8]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _0224A190
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A190:
	ldr r0, [r4, #8]
	mov r8, #0
	cmp r0, #0
	ble _0224AA5C
	mov r6, r8
_0224A1A4:
	ldr r0, [r4, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0xc]
	streq r6, [r0, r8, lsl #2]
	beq _0224A218
	bl goastrdup
	ldr r1, [r7, #0xc]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A218
	subs r8, r8, #1
	bmi _0224A1F4
_0224A1E0:
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A1E0
_0224A1F4:
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A218:
	ldr r0, [r4, #8]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A1A4
	b _0224AA5C
_0224A22C:
	ldr r0, [r4, #4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _0224A268
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224A268
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A268:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A2A0:
	ldr r0, [r4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _0224A2DC
	bl goastrdup
	str r0, [r7]
	cmp r0, #0
	bne _0224A2DC
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A2DC:
	ldr r0, [r4, #4]
	cmp r0, #0
	bge _0224A2FC
	ldr r0, _02249BB0 // =0x022594CC
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	mov r3, #0x350
	bl __msl_assertion_failed
_0224A2FC:
	ldr r0, [r4, #8]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #8]
	beq _0224A3DC
	ldr r0, [r4, #4]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #8]
	cmp r0, #0
	bne _0224A344
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A344:
	ldr r0, [r4, #4]
	mov r8, #0
	cmp r0, #0
	ble _0224A3DC
	mov r6, r8
_0224A358:
	ldr r0, [r4, #8]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #8]
	streq r6, [r0, r8, lsl #2]
	beq _0224A3CC
	bl goastrdup
	ldr r1, [r7, #8]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #8]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A3CC
	subs r8, r8, #1
	bmi _0224A3A8
_0224A394:
	ldr r0, [r7, #8]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A394
_0224A3A8:
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A3CC:
	ldr r0, [r4, #4]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A358
_0224A3DC:
	ldr r0, [r4, #4]
	cmp r0, #0
	bge _0224A3FC
	ldr r0, _02249BB0 // =0x022594CC
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA88 // =0x00000351
	bl __msl_assertion_failed
_0224A3FC:
	ldr r0, [r4, #4]
	cmp r0, #0
	ble _0224AA5C
	ldr r0, [r4, #0xc]
	cmp r0, #0
	bne _0224A428
	ldr r0, _02249BBC // =0x022594E8
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA88 // =0x00000351
	bl __msl_assertion_failed
_0224A428:
	ldr r0, [r4, #4]
	mov r6, r0, lsl #2
	mov r0, r6
	bl DWCi_GsMalloc
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _0224A460
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A460:
	ldr r1, [r4, #0xc]
	mov r2, r6
	bl memcpy
	b _0224AA5C
_0224A470:
	ldr r0, [r4]
	ldr r6, [sp, #8]
	cmp r0, #0
	beq _0224A4AC
	bl goastrdup
	str r0, [r6]
	cmp r0, #0
	bne _0224A4AC
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A4AC:
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0224A4E4
	bl goastrdup
	str r0, [r6, #4]
	cmp r0, #0
	bne _0224A4E4
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A4E4:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224A51C
	bl goastrdup
	str r0, [r6, #8]
	cmp r0, #0
	bne _0224A51C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A51C:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r6, #0xc]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r6
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A554:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _0224A590
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _0224A590
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A590:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _0224A5B0
	ldr r0, _0224AA8C // =0x02259544
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA90 // =0x00000365
	bl __msl_assertion_failed
_0224A5B0:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0xc]
	beq _0224A690
	ldr r0, [r4, #8]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0xc]
	cmp r0, #0
	bne _0224A5F8
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A5F8:
	ldr r0, [r4, #8]
	mov r8, #0
	cmp r0, #0
	ble _0224A690
	mov r6, r8
_0224A60C:
	ldr r0, [r4, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0xc]
	streq r6, [r0, r8, lsl #2]
	beq _0224A680
	bl goastrdup
	ldr r1, [r7, #0xc]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A680
	subs r8, r8, #1
	bmi _0224A65C
_0224A648:
	ldr r0, [r7, #0xc]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A648
_0224A65C:
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A680:
	ldr r0, [r4, #8]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A60C
_0224A690:
	ldr r0, [r4, #8]
	cmp r0, #0
	bge _0224A6B0
	ldr r0, _0224AA8C // =0x02259544
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA94 // =0x00000366
	bl __msl_assertion_failed
_0224A6B0:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0x10]
	beq _0224AA5C
	ldr r0, [r4, #8]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0x10]
	cmp r0, #0
	bne _0224A6F8
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A6F8:
	ldr r0, [r4, #8]
	mov r8, #0
	cmp r0, #0
	ble _0224AA5C
	mov r6, r8
_0224A70C:
	ldr r0, [r4, #0x10]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0x10]
	streq r6, [r0, r8, lsl #2]
	beq _0224A780
	bl goastrdup
	ldr r1, [r7, #0x10]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A780
	subs r8, r8, #1
	bmi _0224A75C
_0224A748:
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A748
_0224A75C:
	ldr r0, [r7, #0x10]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A780:
	ldr r0, [r4, #8]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A70C
	b _0224AA5C
_0224A794:
	ldr r0, [r4, #4]
	ldr r7, [sp, #8]
	cmp r0, #0
	beq _0224A7D0
	bl goastrdup
	str r0, [r7, #4]
	cmp r0, #0
	bne _0224A7D0
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A7D0:
	ldr r0, [r4, #8]
	cmp r0, #0
	beq _0224A808
	bl goastrdup
	str r0, [r7, #8]
	cmp r0, #0
	bne _0224A808
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A808:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	bge _0224A828
	ldr r0, _0224AA8C // =0x02259544
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	mov r3, #0x370
	bl __msl_assertion_failed
_0224A828:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0x10]
	beq _0224A908
	ldr r0, [r4, #0xc]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0x10]
	cmp r0, #0
	bne _0224A870
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A870:
	ldr r0, [r4, #0xc]
	mov r8, #0
	cmp r0, #0
	ble _0224A908
	mov r6, r8
_0224A884:
	ldr r0, [r4, #0x10]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0x10]
	streq r6, [r0, r8, lsl #2]
	beq _0224A8F8
	bl goastrdup
	ldr r1, [r7, #0x10]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A8F8
	subs r8, r8, #1
	bmi _0224A8D4
_0224A8C0:
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A8C0
_0224A8D4:
	ldr r0, [r7, #0x10]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A8F8:
	ldr r0, [r4, #0xc]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A884
_0224A908:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	bge _0224A928
	ldr r0, _0224AA8C // =0x02259544
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	ldr r3, _0224AA98 // =0x00000371
	bl __msl_assertion_failed
_0224A928:
	ldr r0, [r4, #0x14]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r7, #0x14]
	beq _0224AA5C
	ldr r0, [r4, #0xc]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #0x14]
	cmp r0, #0
	bne _0224A970
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A970:
	ldr r0, [r4, #0xc]
	mov r8, #0
	cmp r0, #0
	ble _0224AA5C
	mov r6, r8
_0224A984:
	ldr r0, [r4, #0x14]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	ldreq r0, [r7, #0x14]
	streq r6, [r0, r8, lsl #2]
	beq _0224A9F8
	bl goastrdup
	ldr r1, [r7, #0x14]
	str r0, [r1, r8, lsl #2]
	ldr r0, [r7, #0x14]
	ldr r0, [r0, r8, lsl #2]
	cmp r0, #0
	bne _0224A9F8
	subs r8, r8, #1
	bmi _0224A9D4
_0224A9C0:
	ldr r0, [r7, #0x14]
	ldr r0, [r0, r8, lsl #2]
	bl DWCi_GsFree
	subs r8, r8, #1
	bpl _0224A9C0
_0224A9D4:
	ldr r0, [r7, #0x14]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224A9F8:
	ldr r0, [r4, #0xc]
	add r8, r8, #1
	cmp r8, r0
	blt _0224A984
	b _0224AA5C
_0224AA0C:
	ldr r0, [r4, #4]
	ldr r4, [sp, #8]
	cmp r0, #0
	beq _0224AA5C
	bl goastrdup
	str r0, [r4, #4]
	cmp r0, #0
	bne _0224AA5C
	mov r0, r4
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0224AA48:
	ldr r0, _0224AA9C // =0x022593EC
	ldr r1, _02249B60 // =0x02259368
	ldr r2, _02249B64 // =0x022592C8
	mov r3, #0x380
	bl __msl_assertion_failed
_0224AA5C:
	ldr r0, [r5, #0x820]
	add r1, sp, #0
	bl ArrayAppend
	mov r0, #1
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0224AA74: .word 0x0000030E
_0224AA78: .word 0x02259504
_0224AA7C: .word 0x00000335
_0224AA80: .word 0x02259520
_0224AA84: .word 0x0000033E
_0224AA88: .word 0x00000351
_0224AA8C: .word 0x02259544
_0224AA90: .word 0x00000365
_0224AA94: .word 0x00000366
_0224AA98: .word 0x00000371
_0224AA9C: .word 0x022593EC
	arm_func_end FUN_02248C2C

	arm_func_start FUN_0224AAA0
FUN_0224AAA0: // 0x0224AAA0
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	movs r6, r0
	mov r5, r1
	bne _0224AAC8
	ldr r0, _0224B068 // =0x022593F0
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B074 // =0x0000038D
	bl __msl_assertion_failed
_0224AAC8:
	cmp r5, #0
	bne _0224AAE4
	ldr r0, _0224B078 // =0x02259358
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AAE4:
	ldr r0, [r5]
	cmp r0, #0
	bge _0224AB04
	ldr r0, _0224B080 // =0x02259378
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AB04:
	ldr r0, [r5]
	cmp r0, #0x20
	blt _0224AB24
	ldr r0, _0224B084 // =0x02259388
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AB24:
	ldr r0, [r5, #4]
	cmp r0, #0
	bne _0224AB44
	ldr r0, _0224B088 // =0x022593A4
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AB44:
	ldr r0, [r5, #8]
	cmp r0, #0
	bne _0224AB64
	ldr r0, _0224B08C // =0x022593BC
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AB64:
	ldr r0, [r5, #0x10]
	cmp r0, #0
	bge _0224AB84
	ldr r0, _0224B090 // =0x022593DC
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B07C // =0x0000038F
	bl __msl_assertion_failed
_0224AB84:
	ldr r0, [r5]
	ldr r4, [r5, #0xc]
	cmp r0, #0x1f
	addls pc, pc, r0, lsl #2
	b _0224B044
_0224AB98: // jump table
	b _0224AC18 // case 0
	b _0224AC34 // case 1
	b _0224AC50 // case 2
	b _0224AC6C // case 3
	b _0224AC88 // case 4
	b _0224ACA8 // case 5
	b _0224ACC4 // case 6
	b _0224ACE0 // case 7
	b _0224AD08 // case 8
	b _0224AD24 // case 9
	b _0224AD40 // case 10
	b _0224AD5C // case 11
	b _0224AD78 // case 12
	b _0224AD94 // case 13
	b _0224ADBC // case 14
	b _0224ADE4 // case 15
	b _0224AE0C // case 16
	b _0224AE28 // case 17
	b _0224AE44 // case 18
	b _0224AE64 // case 19
	b _0224AE8C // case 20
	b _0224AEC8 // case 21
	b _0224AEE8 // case 22
	b _0224AF10 // case 23
	b _0224AF30 // case 24
	b _0224AF50 // case 25
	b _0224AF70 // case 26
	b _0224AF8C // case 27
	b _0224AFAC // case 28
	b _0224AFCC // case 29
	b _0224AFF4 // case 30
	b _0224B028 // case 31
_0224AC18:
	ldr r0, [r5, #8]
	ldr r3, [r5, #4]
	ldr r1, [r0]
	mov r0, r6
	mov r2, r4
	blx r3
	b _0224B058
_0224AC34:
	ldr r0, [r5, #8]
	ldr r3, [r5, #4]
	ldr r1, [r0]
	mov r0, r6
	mov r2, r4
	blx r3
	b _0224B058
_0224AC50:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AC6C:
	ldr r0, [r5, #8]
	ldr ip, [r5, #4]
	ldmia r0, {r1, r2}
	mov r0, r6
	mov r3, r4
	blx ip
	b _0224B058
_0224AC88:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224ACA8:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224ACC4:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224ACE0:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AD08:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AD24:
	ldr r0, [r5, #8]
	ldr ip, [r5, #4]
	ldmia r0, {r1, r2}
	mov r0, r6
	mov r3, r4
	blx ip
	b _0224B058
_0224AD40:
	ldr r0, [r5, #8]
	ldr ip, [r5, #4]
	ldmia r0, {r1, r2}
	mov r0, r6
	mov r3, r4
	blx ip
	b _0224B058
_0224AD5C:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AD78:
	ldr r0, [r5, #8]
	ldr r3, [r5, #4]
	ldr r1, [r0]
	mov r0, r6
	mov r2, r4
	blx r3
	b _0224B058
_0224AD94:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224ADBC:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224ADE4:
	ldr r7, [r5, #8]
	ldr r8, [r5, #4]
	ldr r1, [r7, #8]
	mov r0, r6
	bl FUN_0224C454
	str r4, [sp]
	mov r0, r6
	ldmia r7, {r1, r2, r3}
	blx r8
	b _0224B058
_0224AE0C:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AE28:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AE44:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AE64:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AE8C:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	str r1, [sp, #4]
	ldr r1, [r3, #0x14]
	str r1, [sp, #8]
	ldr r1, [r3, #0x18]
	str r1, [sp, #0xc]
	str r4, [sp, #0x10]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AEC8:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AEE8:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AF10:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AF30:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AF50:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AF70:
	ldr r3, [r5, #8]
	mov r0, r6
	str r4, [sp]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AF8C:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AFAC:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	stmia sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AFCC:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	stmib sp, {r1, r4}
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224AFF4:
	ldr r3, [r5, #8]
	mov r0, r6
	ldr r1, [r3, #0xc]
	str r1, [sp]
	ldr r1, [r3, #0x10]
	str r1, [sp, #4]
	ldr r1, [r3, #0x14]
	str r1, [sp, #8]
	str r4, [sp, #0xc]
	ldr r4, [r5, #4]
	ldmia r3, {r1, r2, r3}
	blx r4
	b _0224B058
_0224B028:
	ldr r0, [r5, #8]
	ldr ip, [r5, #4]
	ldmia r0, {r1, r2}
	mov r0, r6
	mov r3, r4
	blx ip
	b _0224B058
_0224B044:
	ldr r0, _0224B094 // =0x022593EC
	ldr r1, _0224B06C // =0x02259368
	ldr r2, _0224B070 // =0x022592D8
	ldr r3, _0224B098 // =0x000005EF
	bl __msl_assertion_failed
_0224B058:
	mov r0, r5
	bl FUN_022483FC
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0224B068: .word 0x022593F0
_0224B06C: .word 0x02259368
_0224B070: .word 0x022592D8
_0224B074: .word 0x0000038D
_0224B078: .word 0x02259358
_0224B07C: .word 0x0000038F
_0224B080: .word 0x02259378
_0224B084: .word 0x02259388
_0224B088: .word 0x022593A4
_0224B08C: .word 0x022593BC
_0224B090: .word 0x022593DC
_0224B094: .word 0x022593EC
_0224B098: .word 0x000005EF
	arm_func_end FUN_0224AAA0

	arm_func_start FUN_0224B09C
FUN_0224B09C: // 0x0224B09C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	movs sl, r0
	mov sb, r1
	bne _0224B0C4
	ldr r0, _0224B26C // =0x022593F0
	ldr r1, _0224B270 // =0x02259368
	ldr r2, _0224B274 // =0x022592E8
	ldr r3, _0224B278 // =0x000005FC
	bl __msl_assertion_failed
_0224B0C4:
	ldr r0, [sl, #0x820]
	mov r8, #0
	bl ArrayLength
	cmp r0, #0
	addle sp, sp, #0x18
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr fp, _0224B27C // =0x02259358
	ldr r6, _0224B270 // =0x02259368
	ldr r5, _0224B274 // =0x022592E8
	ldr r4, _0224B280 // =0x00000605
_0224B0EC:
	ldr r0, [sl, #0x820]
	mov r1, r8
	bl ArrayNth
	movs r7, r0
	bne _0224B114
	mov r0, fp
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B114:
	ldr r0, [r7]
	cmp r0, #0
	bge _0224B134
	ldr r0, _0224B284 // =0x02259378
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B134:
	ldr r0, [r7]
	cmp r0, #0x20
	blt _0224B154
	ldr r0, _0224B288 // =0x02259388
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B154:
	ldr r0, [r7, #4]
	cmp r0, #0
	bne _0224B174
	ldr r0, _0224B28C // =0x022593A4
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B174:
	ldr r0, [r7, #8]
	cmp r0, #0
	bne _0224B194
	ldr r0, _0224B290 // =0x022593BC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B194:
	ldr r0, [r7, #0x10]
	cmp r0, #0
	bge _0224B1B4
	ldr r0, _0224B294 // =0x022593DC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B1B4:
	ldr r1, [r7, #0x14]
	cmp r1, #0
	beq _0224B1E8
	mov r0, sl
	bl FUN_0224C134
	cmp r0, #0
	bne _0224B1E8
	mov r0, r7
	bl FUN_022483FC
	ldr r0, [sl, #0x820]
	mov r1, r8
	bl ArrayDeleteAt
	b _0224B254
_0224B1E8:
	ldr r1, [r7, #0x14]
	cmp r1, #0
	beq _0224B204
	mov r0, sl
	bl FUN_0224C518
	cmp r0, #0
	beq _0224B250
_0224B204:
	cmp sb, #0
	ldrne r0, [r7, #0x10]
	cmpne r0, sb
	bne _0224B250
	add ip, sp, #0
	ldmia r7!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia r7, {r0, r1}
	stmia ip, {r0, r1}
	ldr r0, [sl, #0x820]
	mov r1, r8
	bl ArrayDeleteAt
	mov r0, sl
	add r1, sp, #0
	bl FUN_0224AAA0
	cmp sb, #0
	beq _0224B254
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224B250:
	add r8, r8, #1
_0224B254:
	ldr r0, [sl, #0x820]
	bl ArrayLength
	cmp r8, r0
	blt _0224B0EC
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0224B26C: .word 0x022593F0
_0224B270: .word 0x02259368
_0224B274: .word 0x022592E8
_0224B278: .word 0x000005FC
_0224B27C: .word 0x02259358
_0224B280: .word 0x00000605
_0224B284: .word 0x02259378
_0224B288: .word 0x02259388
_0224B28C: .word 0x022593A4
_0224B290: .word 0x022593BC
_0224B294: .word 0x022593DC
	arm_func_end FUN_0224B09C

	arm_func_start FUN_0224B298
FUN_0224B298: // 0x0224B298
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	movs sl, r0
	mov sb, r1
	bne _0224B2BC
	ldr r0, _0224B3CC // =0x022593F0
	ldr r1, _0224B3D0 // =0x02259368
	ldr r2, _0224B3D4 // =0x02259320
	ldr r3, _0224B3D8 // =0x0000063B
	bl __msl_assertion_failed
_0224B2BC:
	ldr r0, [sl, #0x820]
	bl ArrayLength
	mov fp, r0
	cmp fp, #0
	mov r8, #0
	ble _0224B3C4
	ldr r6, _0224B3D0 // =0x02259368
	ldr r5, _0224B3D4 // =0x02259320
	ldr r4, _0224B3DC // =0x00000648
_0224B2E0:
	ldr r0, [sl, #0x820]
	mov r1, r8
	bl ArrayNth
	movs r7, r0
	bne _0224B308
	ldr r0, _0224B3E0 // =0x02259358
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B308:
	ldr r0, [r7]
	cmp r0, #0
	bge _0224B328
	ldr r0, _0224B3E4 // =0x02259378
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B328:
	ldr r0, [r7]
	cmp r0, #0x20
	blt _0224B348
	ldr r0, _0224B3E8 // =0x02259388
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B348:
	ldr r0, [r7, #4]
	cmp r0, #0
	bne _0224B368
	ldr r0, _0224B3EC // =0x022593A4
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B368:
	ldr r0, [r7, #8]
	cmp r0, #0
	bne _0224B388
	ldr r0, _0224B3F0 // =0x022593BC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B388:
	ldr r0, [r7, #0x10]
	cmp r0, #0
	bge _0224B3A8
	ldr r0, _0224B3F4 // =0x022593DC
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl __msl_assertion_failed
_0224B3A8:
	ldr r0, [r7, #0x10]
	cmp r0, sb
	moveq r0, r8
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r8, r8, #1
	cmp r8, fp
	blt _0224B2E0
_0224B3C4:
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0224B3CC: .word 0x022593F0
_0224B3D0: .word 0x02259368
_0224B3D4: .word 0x02259320
_0224B3D8: .word 0x0000063B
_0224B3DC: .word 0x00000648
_0224B3E0: .word 0x02259358
_0224B3E4: .word 0x02259378
_0224B3E8: .word 0x02259388
_0224B3EC: .word 0x022593A4
_0224B3F0: .word 0x022593BC
_0224B3F4: .word 0x022593DC
	arm_func_end FUN_0224B298

	arm_func_start FUN_0224B3F8
FUN_0224B3F8: // 0x0224B3F8
	stmfd sp!, {r3, lr}
	bl FUN_0224B298
	mvn r1, #0
	cmp r0, r1
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end FUN_0224B3F8
_0224B414:
	.byte 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x74, 0x00, 0x9F, 0xE5, 0x74, 0x10, 0x9F, 0xE5, 0x74, 0x20, 0x9F, 0xE5
	.byte 0x99, 0x30, 0xA0, 0xE3, 0x10, 0x23, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x60, 0x00, 0x9F, 0xE5, 0x54, 0x10, 0x9F, 0xE5, 0x54, 0x20, 0x9F, 0xE5
	.byte 0x99, 0x30, 0xA0, 0xE3, 0x08, 0x23, 0xFA, 0xEB, 0xD1, 0x20, 0xD5, 0xE0, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0x00, 0x52, 0xE3, 0x09, 0x00, 0x00, 0x0A, 0x40, 0x10, 0x9F, 0xE5, 0x00, 0x00, 0x52, 0xE3
	.byte 0x02, 0x00, 0x00, 0xBA, 0x80, 0x00, 0x52, 0xE3, 0x00, 0x00, 0x00, 0xAA, 0x02, 0x20, 0xD1, 0xE7
	.byte 0x02, 0x00, 0x80, 0xE0, 0xD1, 0x20, 0xD5, 0xE0, 0x00, 0x00, 0x52, 0xE3, 0xF6, 0xFF, 0xFF, 0x1A
	.byte 0x04, 0x10, 0xA0, 0xE1, 0xB4, 0x5A, 0xFA, 0xEB, 0x01, 0x00, 0xA0, 0xE1, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x0C, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x68, 0x95, 0x25, 0x02, 0x28, 0x98, 0x25, 0x02
	.byte 0x64, 0xE7, 0x0F, 0x02

	arm_func_start FUN_0224B4B4
FUN_0224B4B4: // 0x0224B4B4
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x1e4
	ldr r2, _0224B4E8 // =0x00000101
	mov r4, r0
	add r0, sp, #0
	bl strncpy
	mov r0, #0
	strb r0, [sp, #0x100]
	ldr r0, [r4, #0x80c]
	add r1, sp, #0
	bl TableLookup
	add sp, sp, #0x1e4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_0224B4E8: .word 0x00000101
	arm_func_end FUN_0224B4B4
_0224B4EC:
	.byte 0x38, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x38, 0x00, 0x9F, 0xE5
	.byte 0x38, 0x10, 0x9F, 0xE5, 0x38, 0x20, 0x9F, 0xE5, 0xB4, 0x30, 0xA0, 0xE3, 0xDA, 0x22, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0xCA, 0x28, 0x00, 0x9F, 0xE5, 0x1C, 0x10, 0x9F, 0xE5
	.byte 0x1C, 0x20, 0x9F, 0xE5, 0xB5, 0x30, 0xA0, 0xE3, 0xD3, 0x22, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0xB6, 0xFF, 0xFF, 0xEB, 0x38, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0xFC, 0x96, 0x25, 0x02, 0x48, 0x98, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xE8, 0x00, 0x9F, 0xE5
	.byte 0xE8, 0x10, 0x9F, 0xE5, 0xE8, 0x20, 0x9F, 0xE5, 0xC1, 0x30, 0xA0, 0xE3, 0xC2, 0x22, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xD8, 0x00, 0x9F, 0xE5, 0xCC, 0x10, 0x9F, 0xE5
	.byte 0xCC, 0x20, 0x9F, 0xE5, 0xC2, 0x30, 0xA0, 0xE3, 0xBB, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xC0, 0x00, 0x9F, 0xE5, 0xB0, 0x10, 0x9F, 0xE5, 0xB0, 0x20, 0x9F, 0xE5
	.byte 0xC6, 0x30, 0xA0, 0xE3, 0xB4, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0xA8, 0x00, 0x9F, 0xE5, 0x94, 0x10, 0x9F, 0xE5, 0x94, 0x20, 0x9F, 0xE5, 0xC7, 0x30, 0xA0, 0xE3
	.byte 0xAD, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x88, 0x00, 0x9F, 0xE5
	.byte 0x78, 0x10, 0x9F, 0xE5, 0x78, 0x20, 0x9F, 0xE5, 0xC8, 0x30, 0xA0, 0xE3, 0xA6, 0x22, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x70, 0x00, 0x9F, 0xE5
	.byte 0x58, 0x10, 0x9F, 0xE5, 0x58, 0x20, 0x9F, 0xE5, 0xC8, 0x30, 0xA0, 0xE3, 0x9E, 0x22, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x50, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5
	.byte 0x3C, 0x20, 0x9F, 0xE5, 0xC9, 0x30, 0xA0, 0xE3, 0x97, 0x22, 0xFA, 0xEB, 0xD0, 0x00, 0xD4, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x38, 0x00, 0x9F, 0xE5, 0x1C, 0x10, 0x9F, 0xE5
	.byte 0x1C, 0x20, 0x9F, 0xE5, 0xC9, 0x30, 0xA0, 0xE3, 0x8F, 0x22, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0xD9, 0x50, 0xFA, 0xEB, 0x38, 0x80, 0xBD, 0xE8, 0x58, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0xBC, 0x97, 0x25, 0x02, 0x68, 0x98, 0x25, 0x02, 0x78, 0x98, 0x25, 0x02
	.byte 0x88, 0x98, 0x25, 0x02, 0x98, 0x98, 0x25, 0x02, 0xA8, 0x98, 0x25, 0x02, 0x10, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x28, 0x00, 0x9F, 0xE5, 0x28, 0x10, 0x9F, 0xE5
	.byte 0x28, 0x20, 0x9F, 0xE5, 0xD2, 0x30, 0xA0, 0xE3, 0x7B, 0x22, 0xFA, 0xEB, 0x5C, 0x01, 0x94, 0xE5
	.byte 0x86, 0x30, 0xFE, 0xEB, 0x34, 0x01, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x10, 0x80, 0xBD, 0x08
	.byte 0x2F, 0x7B, 0xFE, 0xEB, 0x10, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02
	.byte 0xF0, 0x97, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x38, 0x00, 0x9F, 0xE5, 0x38, 0x10, 0x9F, 0xE5, 0x38, 0x20, 0x9F, 0xE5
	.byte 0xE1, 0x30, 0xA0, 0xE3, 0x68, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0xCA
	.byte 0x28, 0x00, 0x9F, 0xE5, 0x1C, 0x10, 0x9F, 0xE5, 0x1C, 0x20, 0x9F, 0xE5, 0xE2, 0x30, 0xA0, 0xE3
	.byte 0x61, 0x22, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x44, 0xFF, 0xFF, 0xEB
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x14, 0x96, 0x25, 0x02
	.byte 0x48, 0x98, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0xDC, 0x00, 0x9F, 0xE5, 0xDC, 0x10, 0x9F, 0xE5, 0xDC, 0x20, 0x9F, 0xE5
	.byte 0xEE, 0x30, 0xA0, 0xE3, 0x50, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0xCC, 0x00, 0x9F, 0xE5, 0xC0, 0x10, 0x9F, 0xE5, 0xC0, 0x20, 0x9F, 0xE5, 0xEF, 0x30, 0xA0, 0xE3
	.byte 0x49, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x00, 0x00, 0x54, 0x13, 0x07, 0x00, 0x00, 0x1A
	.byte 0x00, 0x00, 0x55, 0xE3, 0x00, 0x00, 0x54, 0x03, 0x00, 0x00, 0xA0, 0x03, 0x38, 0x80, 0xBD, 0x08
	.byte 0x00, 0x00, 0x55, 0xE3, 0x00, 0x00, 0xE0, 0x03, 0x01, 0x00, 0xA0, 0x13, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x88, 0x00, 0x9F, 0xE5, 0x78, 0x10, 0x9F, 0xE5
	.byte 0x78, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0x37, 0x22, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x70, 0x00, 0x9F, 0xE5, 0x58, 0x10, 0x9F, 0xE5
	.byte 0x58, 0x20, 0x9F, 0xE5, 0x60, 0x30, 0x9F, 0xE5, 0x2F, 0x22, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x58, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0x50, 0x30, 0x9F, 0xE5, 0x28, 0x22, 0xFA, 0xEB, 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x40, 0x00, 0x9F, 0xE5, 0x1C, 0x10, 0x9F, 0xE5, 0x1C, 0x20, 0x9F, 0xE5
	.byte 0x30, 0x30, 0x9F, 0xE5, 0x20, 0x22, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x6A, 0x50, 0xFA, 0xEB, 0x38, 0x80, 0xBD, 0xE8, 0x58, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02
	.byte 0xB4, 0x96, 0x25, 0x02, 0x68, 0x98, 0x25, 0x02, 0x78, 0x98, 0x25, 0x02, 0x02, 0x01, 0x00, 0x00
	.byte 0x98, 0x98, 0x25, 0x02, 0x88, 0x98, 0x25, 0x02, 0x03, 0x01, 0x00, 0x00, 0xA8, 0x98, 0x25, 0x02
	.byte 0x08, 0x40, 0x2D, 0xE9, 0x00, 0x00, 0x50, 0xE3, 0x08, 0x80, 0xBD, 0x18, 0x10, 0x00, 0x9F, 0xE5
	.byte 0x10, 0x10, 0x9F, 0xE5, 0x10, 0x20, 0x9F, 0xE5, 0x10, 0x30, 0x9F, 0xE5, 0x0A, 0x22, 0xFA, 0xEB
	.byte 0x08, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0xD4, 0x97, 0x25, 0x02
	.byte 0x0A, 0x01, 0x00, 0x00

	arm_func_start FUN_0224B864
FUN_0224B864: // 0x0224B864
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0224B8E0 // =0x0224B54C
	ldr ip, _0224B8E4 // =0x0224B66C
	str r1, [sp]
	mov r4, r0
	ldr r3, _0224B8E8 // =0x0224B4EC
	mov r0, #0x1e4
	mov r1, #7
	mov r2, #2
	str ip, [sp, #4]
	bl TableNew2
	cmp r0, #0
	str r0, [r4, #0x80c]
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	mov r1, #0
	mov r2, r1
	mov r0, #0x1e4
	bl ArrayNew
	cmp r0, #0
	str r0, [r4, #0x810]
	addne sp, sp, #8
	movne r0, #1
	ldmneia sp!, {r4, pc}
	ldr r0, [r4, #0x80c]
	bl TableFree
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224B8E0: .word 0x0224B54C
_0224B8E4: .word 0x0224B66C
_0224B8E8: .word 0x0224B4EC
	arm_func_end FUN_0224B864

	arm_func_start FUN_0224B8EC
FUN_0224B8EC: // 0x0224B8EC
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _0224B90C
	ldr r0, _0224B930 // =0x022598B8
	ldr r1, _0224B934 // =0x02259818
	ldr r2, _0224B938 // =0x02259600
	ldr r3, _0224B93C // =0x00000127
	bl __msl_assertion_failed
_0224B90C:
	ldr r0, [r4, #0x80c]
	cmp r0, #0
	beq _0224B91C
	bl TableFree
_0224B91C:
	ldr r0, [r4, #0x810]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	bl ArrayFree
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224B930: .word 0x022598B8
_0224B934: .word 0x02259818
_0224B938: .word 0x02259600
_0224B93C: .word 0x00000127
	arm_func_end FUN_0224B8EC

	arm_func_start FUN_0224B940
FUN_0224B940: // 0x0224B940
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x1e4
	movs r5, r0
	mov r4, r1
	bne _0224B968
	ldr r0, _0224B9C0 // =0x022598B8
	ldr r1, _0224B9C4 // =0x02259818
	ldr r2, _0224B9C8 // =0x0225963C
	mov r3, #0x134
	bl __msl_assertion_failed
_0224B968:
	mov r0, #0
	add lr, sp, #0
	mov r1, r0
	mov r2, r0
	mov r3, r0
	mov ip, #0xf
_0224B980:
	stmia lr!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs ip, ip, #1
	bne _0224B980
	ldr r2, _0224B9CC // =0x00000101
	str r0, [lr]
	add r0, sp, #0
	mov r1, r4
	bl strncpy
	mov r0, #0
	strb r0, [sp, #0x100]
	ldr r0, [r5, #0x810]
	add r1, sp, #0
	bl ArrayAppend
	add sp, sp, #0x1e4
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_0224B9C0: .word 0x022598B8
_0224B9C4: .word 0x02259818
_0224B9C8: .word 0x0225963C
_0224B9CC: .word 0x00000101
	arm_func_end FUN_0224B940
_0224B9D0:
	.byte 0x00, 0xC0, 0x9F, 0xE5, 0x1C, 0xFF, 0x2F, 0xE1, 0xB0, 0xF9, 0x0D, 0x02, 0xF8, 0x4F, 0x2D, 0xE9
	.byte 0x00, 0xA0, 0xB0, 0xE1, 0x01, 0x90, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x84, 0x00, 0x9F, 0xE5
	.byte 0x84, 0x10, 0x9F, 0xE5, 0x84, 0x20, 0x9F, 0xE5, 0x84, 0x30, 0x9F, 0xE5, 0x9E, 0x21, 0xFA, 0xEB
	.byte 0x10, 0x08, 0x9A, 0xE5, 0x6C, 0x78, 0xFE, 0xEB, 0x00, 0x70, 0xA0, 0xE1, 0x00, 0x00, 0x57, 0xE3
	.byte 0x00, 0x60, 0xA0, 0xE3, 0x15, 0x00, 0x00, 0xDA, 0x68, 0x50, 0x9F, 0xE5, 0x58, 0x40, 0x9F, 0xE5
	.byte 0x58, 0xB0, 0x9F, 0xE5, 0x10, 0x08, 0x9A, 0xE5, 0x06, 0x10, 0xA0, 0xE1, 0x64, 0x78, 0xFE, 0xEB
	.byte 0x00, 0x80, 0xB0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x4C, 0x30, 0x9F, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x0B, 0x20, 0xA0, 0xE1, 0x8B, 0x21, 0xFA, 0xEB, 0x08, 0x00, 0xA0, 0xE1
	.byte 0x09, 0x10, 0xA0, 0xE1, 0xD5, 0x4F, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x01, 0x00, 0xA0, 0x03
	.byte 0xF8, 0x8F, 0xBD, 0x08, 0x01, 0x60, 0x86, 0xE2, 0x07, 0x00, 0x56, 0xE1, 0xEC, 0xFF, 0xFF, 0xBA
	.byte 0x00, 0x00, 0xA0, 0xE3, 0xF8, 0x8F, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02
	.byte 0xA0, 0x96, 0x25, 0x02, 0x4D, 0x01, 0x00, 0x00, 0xC8, 0x98, 0x25, 0x02, 0x53, 0x01, 0x00, 0x00
	.byte 0xF0, 0x40, 0x2D, 0xE9, 0x7B, 0xDF, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1
	.byte 0x02, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x94, 0x01, 0x9F, 0xE5, 0x94, 0x11, 0x9F, 0xE5
	.byte 0x94, 0x21, 0x9F, 0xE5, 0x16, 0x3E, 0xA0, 0xE3, 0x6F, 0x21, 0xFA, 0xEB, 0x00, 0x00, 0x56, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x84, 0x01, 0x9F, 0xE5, 0x78, 0x11, 0x9F, 0xE5, 0x78, 0x21, 0x9F, 0xE5
	.byte 0x7C, 0x31, 0x9F, 0xE5, 0x68, 0x21, 0xFA, 0xEB, 0xD0, 0x00, 0xD6, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x6C, 0x01, 0x9F, 0xE5, 0x58, 0x11, 0x9F, 0xE5, 0x58, 0x21, 0x9F, 0xE5
	.byte 0x5C, 0x31, 0x9F, 0xE5, 0x60, 0x21, 0xFA, 0xEB, 0x06, 0x00, 0xA0, 0xE1, 0x17, 0x34, 0xFA, 0xEB
	.byte 0x54, 0x31, 0x9F, 0xE5, 0x03, 0x00, 0x50, 0xE1, 0x04, 0x00, 0x00, 0x3A, 0x4C, 0x01, 0x9F, 0xE5
	.byte 0x30, 0x11, 0x9F, 0xE5, 0x30, 0x21, 0x9F, 0xE5, 0x61, 0x30, 0x83, 0xE2, 0x56, 0x21, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x34, 0x01, 0x9F, 0xE5, 0x14, 0x11, 0x9F, 0xE5
	.byte 0x14, 0x21, 0x9F, 0xE5, 0x2C, 0x31, 0x9F, 0xE5, 0x4F, 0x21, 0xFA, 0xEB, 0x02, 0x00, 0xA0, 0xE3
	.byte 0x4E, 0x2F, 0xFE, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x7B, 0xDF, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08
	.byte 0x14, 0x11, 0x9F, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0xC0, 0xD1, 0xE5, 0x08, 0xE0, 0x8D, 0xE2
	.byte 0x00, 0x10, 0xA0, 0xE1, 0x00, 0xC0, 0xC4, 0xE5, 0x00, 0x20, 0xA0, 0xE1, 0x00, 0x30, 0xA0, 0xE1
	.byte 0x0F, 0xC0, 0xA0, 0xE3, 0x0F, 0x00, 0xAE, 0xE8, 0x0F, 0x00, 0xAE, 0xE8, 0x01, 0xC0, 0x5C, 0xE2
	.byte 0xFB, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0x8E, 0xE5, 0x43, 0xCF, 0x8D, 0xE2, 0x0F, 0x00, 0xB5, 0xE8
	.byte 0x0F, 0x00, 0xAC, 0xE8, 0x0F, 0x00, 0xB5, 0xE8, 0x0F, 0x00, 0xAC, 0xE8, 0x0F, 0x00, 0x95, 0xE8
	.byte 0x0F, 0x00, 0x8C, 0xE8, 0xB0, 0x20, 0x9F, 0xE5, 0x06, 0x10, 0xA0, 0xE1, 0x08, 0x00, 0x8D, 0xE2
	.byte 0x23, 0x34, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0x08, 0x01, 0xCD, 0xE5, 0xAC, 0x10, 0x9F, 0xE5
	.byte 0xAC, 0x00, 0x9F, 0xE5, 0x00, 0x10, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5, 0xA4, 0x30, 0x9F, 0xE5
	.byte 0xE0, 0x00, 0xA0, 0xE3, 0x3D, 0x10, 0xA0, 0xE3, 0x02, 0x20, 0xA0, 0xE3, 0x90, 0x79, 0xFE, 0xEB
	.byte 0x3C, 0x01, 0x8D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x7B, 0xDF, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08
	.byte 0x00, 0x30, 0xA0, 0xE3, 0x60, 0x31, 0x8D, 0xE5, 0x64, 0x41, 0x8D, 0xE5, 0x68, 0x31, 0x8D, 0xE5
	.byte 0x6C, 0x31, 0xCD, 0xE5, 0x00, 0x30, 0x8D, 0xE5, 0x10, 0x08, 0x97, 0xE5, 0x68, 0x20, 0x9F, 0xE5
	.byte 0x08, 0x10, 0x8D, 0xE2, 0xBF, 0x78, 0xFE, 0xEB, 0x00, 0x10, 0xA0, 0xE1, 0x00, 0x00, 0xE0, 0xE3
	.byte 0x00, 0x00, 0x51, 0xE1, 0x01, 0x00, 0x00, 0x0A, 0x10, 0x08, 0x97, 0xE5, 0x5F, 0x78, 0xFE, 0xEB
	.byte 0x0C, 0x08, 0x97, 0xE5, 0x08, 0x10, 0x8D, 0xE2, 0x03, 0x7A, 0xFE, 0xEB, 0x7B, 0xDF, 0x8D, 0xE2
	.byte 0xF0, 0x80, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x9C, 0x95, 0x25, 0x02
	.byte 0xD4, 0x98, 0x25, 0x02, 0x62, 0x01, 0x00, 0x00, 0xE4, 0x98, 0x25, 0x02, 0x01, 0x01, 0x00, 0x00
	.byte 0xF8, 0x98, 0x25, 0x02, 0x18, 0x99, 0x25, 0x02, 0x63, 0x01, 0x00, 0x00, 0x2C, 0x99, 0x25, 0x02
	.byte 0x14, 0xB7, 0x24, 0x02, 0x30, 0xB8, 0x24, 0x02, 0xB4, 0xB6, 0x24, 0x02, 0xD0, 0xB9, 0x24, 0x02

	arm_func_start FUN_0224BC80
FUN_0224BC80: // 0x0224BC80
	stmfd sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x1e8
	movs r5, r0
	mov r4, r1
	bne _0224BCA8
	ldr r0, _0224BD6C // =0x022598B8
	ldr r1, _0224BD70 // =0x02259818
	ldr r2, _0224BD74 // =0x02259578
	ldr r3, _0224BD78 // =0x00000189
	bl __msl_assertion_failed
_0224BCA8:
	cmp r4, #0
	bne _0224BCC4
	ldr r0, _0224BD7C // =0x022598D4
	ldr r1, _0224BD70 // =0x02259818
	ldr r2, _0224BD74 // =0x02259578
	ldr r3, _0224BD80 // =0x0000018B
	bl __msl_assertion_failed
_0224BCC4:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _0224BCE4
	ldr r0, _0224BD84 // =0x022598E4
	ldr r1, _0224BD70 // =0x02259818
	ldr r2, _0224BD74 // =0x02259578
	ldr r3, _0224BD80 // =0x0000018B
	bl __msl_assertion_failed
_0224BCE4:
	mov r0, r4
	bl strlen
	ldr r3, _0224BD88 // =0x00000101
	cmp r0, r3
	blo _0224BD0C
	ldr r0, _0224BD8C // =0x022598F8
	ldr r1, _0224BD70 // =0x02259818
	ldr r2, _0224BD74 // =0x02259578
	add r3, r3, #0x8a
	bl __msl_assertion_failed
_0224BD0C:
	ldr r2, _0224BD88 // =0x00000101
	add r0, sp, #4
	mov r1, r4
	bl strncpy
	mov r3, #0
	strb r3, [sp, #0x104]
	str r3, [sp]
	ldr r0, [r5, #0x810]
	ldr r2, _0224BD90 // =0x0224B9D0
	add r1, sp, #4
	bl ArraySearch
	mov r1, r0
	mvn r0, #0
	cmp r1, r0
	beq _0224BD58
	ldr r0, [r5, #0x810]
	bl ArrayRemoveAt
	add sp, sp, #0x1e8
	ldmia sp!, {r3, r4, r5, pc}
_0224BD58:
	ldr r0, [r5, #0x80c]
	add r1, sp, #4
	bl TableRemove
	add sp, sp, #0x1e8
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224BD6C: .word 0x022598B8
_0224BD70: .word 0x02259818
_0224BD74: .word 0x02259578
_0224BD78: .word 0x00000189
_0224BD7C: .word 0x022598D4
_0224BD80: .word 0x0000018B
_0224BD84: .word 0x022598E4
_0224BD88: .word 0x00000101
_0224BD8C: .word 0x022598F8
_0224BD90: .word 0x0224B9D0
	arm_func_end FUN_0224BC80

	arm_func_start FUN_0224BD94
FUN_0224BD94: // 0x0224BD94
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224BDB8
	ldr r0, _0224BE38 // =0x022598B8
	ldr r1, _0224BE3C // =0x02259818
	ldr r2, _0224BE40 // =0x02259744
	ldr r3, _0224BE44 // =0x000001A9
	bl __msl_assertion_failed
_0224BDB8:
	cmp r4, #0
	bne _0224BDD4
	ldr r0, _0224BE48 // =0x022598D4
	ldr r1, _0224BE3C // =0x02259818
	ldr r2, _0224BE40 // =0x02259744
	ldr r3, _0224BE4C // =0x000001AB
	bl __msl_assertion_failed
_0224BDD4:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _0224BDF4
	ldr r0, _0224BE50 // =0x022598E4
	ldr r1, _0224BE3C // =0x02259818
	ldr r2, _0224BE40 // =0x02259744
	ldr r3, _0224BE4C // =0x000001AB
	bl __msl_assertion_failed
_0224BDF4:
	mov r0, r4
	bl strlen
	ldr r3, _0224BE54 // =0x00000101
	cmp r0, r3
	blo _0224BE1C
	ldr r0, _0224BE58 // =0x022598F8
	ldr r1, _0224BE3C // =0x02259818
	ldr r2, _0224BE40 // =0x02259744
	add r3, r3, #0xaa
	bl __msl_assertion_failed
_0224BE1C:
	mov r0, r5
	mov r1, r4
	bl FUN_0224B4B4
	cmp r0, #0
	moveq r0, #0
	addne r0, r0, #0x104
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224BE38: .word 0x022598B8
_0224BE3C: .word 0x02259818
_0224BE40: .word 0x02259744
_0224BE44: .word 0x000001A9
_0224BE48: .word 0x022598D4
_0224BE4C: .word 0x000001AB
_0224BE50: .word 0x022598E4
_0224BE54: .word 0x00000101
_0224BE58: .word 0x022598F8
	arm_func_end FUN_0224BD94
_0224BE5C:
	.byte 0x38, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x40, 0x01, 0x9F, 0xE5
	.byte 0x40, 0x11, 0x9F, 0xE5, 0x40, 0x21, 0x9F, 0xE5, 0x6F, 0x3F, 0xA0, 0xE3, 0x7E, 0x20, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x30, 0x01, 0x9F, 0xE5, 0x24, 0x11, 0x9F, 0xE5
	.byte 0x24, 0x21, 0x9F, 0xE5, 0x28, 0x31, 0x9F, 0xE5, 0x77, 0x20, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x1C, 0x01, 0x9F, 0xE5, 0x08, 0x11, 0x9F, 0xE5, 0x08, 0x21, 0x9F, 0xE5
	.byte 0x14, 0x31, 0x9F, 0xE5, 0x70, 0x20, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x04, 0x01, 0x9F, 0xE5, 0xE8, 0x10, 0x9F, 0xE5, 0xE8, 0x20, 0x9F, 0xE5
	.byte 0xF4, 0x30, 0x9F, 0xE5, 0x68, 0x20, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0x1F, 0x33, 0xFA, 0xEB
	.byte 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0xE4, 0x00, 0x9F, 0xE5, 0xC4, 0x10, 0x9F, 0xE5
	.byte 0xC4, 0x20, 0x9F, 0xE5, 0xD0, 0x30, 0x9F, 0xE5, 0x5F, 0x20, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0xAA, 0xC8, 0x00, 0x9F, 0xE5, 0xA4, 0x10, 0x9F, 0xE5
	.byte 0xA4, 0x20, 0x9F, 0xE5, 0xC0, 0x30, 0x9F, 0xE5, 0x57, 0x20, 0xFA, 0xEB, 0x04, 0x10, 0x94, 0xE5
	.byte 0x08, 0x00, 0x94, 0xE5, 0x01, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0x58, 0x2E, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A, 0xA0, 0x00, 0x9F, 0xE5, 0x74, 0x10, 0x9F, 0xE5
	.byte 0x74, 0x20, 0x9F, 0xE5, 0x98, 0x30, 0x9F, 0xE5, 0x4B, 0x20, 0xFA, 0xEB, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x08, 0x00, 0x84, 0xE5, 0x04, 0x10, 0x94, 0xE5, 0x0C, 0x00, 0x94, 0xE5, 0x01, 0x10, 0x81, 0xE2
	.byte 0x01, 0x11, 0xA0, 0xE1, 0x4A, 0x2E, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A
	.byte 0x68, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5, 0x1E, 0x3E, 0xA0, 0xE3
	.byte 0x3D, 0x20, 0xFA, 0xEB, 0x38, 0x80, 0xBD, 0xE8, 0x0C, 0x00, 0x84, 0xE5, 0x03, 0x00, 0x94, 0xE9
	.byte 0x00, 0x51, 0x81, 0xE7, 0xDC, 0x20, 0x95, 0xE5, 0x0C, 0x10, 0x94, 0xE5, 0x04, 0x00, 0x94, 0xE5
	.byte 0x00, 0x21, 0x81, 0xE7, 0x04, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x80, 0xE2, 0x04, 0x00, 0x84, 0xE5
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x5C, 0x97, 0x25, 0x02
	.byte 0x30, 0x99, 0x25, 0x02, 0xBD, 0x01, 0x00, 0x00, 0x44, 0x99, 0x25, 0x02, 0xC2, 0x01, 0x00, 0x00
	.byte 0x58, 0x99, 0x25, 0x02, 0x70, 0x99, 0x25, 0x02, 0x90, 0x99, 0x25, 0x02, 0xC7, 0x01, 0x00, 0x00
	.byte 0xA4, 0x99, 0x25, 0x02, 0xD9, 0x01, 0x00, 0x00

	arm_func_start FUN_0224BFE8
FUN_0224BFE8: // 0x0224BFE8
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x18
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bne _0224C018
	ldr r0, _0224C108 // =0x022598B8
	ldr r1, _0224C10C // =0x02259818
	ldr r2, _0224C110 // =0x02259678
	mov r3, #0x1f0
	bl __msl_assertion_failed
_0224C018:
	cmp r6, #0
	bne _0224C034
	ldr r0, _0224C114 // =0x022598D4
	ldr r1, _0224C10C // =0x02259818
	ldr r2, _0224C110 // =0x02259678
	ldr r3, _0224C118 // =0x000001F2
	bl __msl_assertion_failed
_0224C034:
	ldrsb r0, [r6]
	cmp r0, #0
	bne _0224C054
	ldr r0, _0224C11C // =0x022598E4
	ldr r1, _0224C10C // =0x02259818
	ldr r2, _0224C110 // =0x02259678
	ldr r3, _0224C118 // =0x000001F2
	bl __msl_assertion_failed
_0224C054:
	mov r0, r6
	bl strlen
	ldr r3, _0224C120 // =0x00000101
	cmp r0, r3
	blo _0224C07C
	ldr r0, _0224C124 // =0x022598F8
	ldr r1, _0224C10C // =0x02259818
	ldr r2, _0224C110 // =0x02259678
	add r3, r3, #0xf1
	bl __msl_assertion_failed
_0224C07C:
	cmp r5, #0
	bne _0224C098
	ldr r0, _0224C128 // =0x022599A8
	ldr r1, _0224C10C // =0x02259818
	ldr r2, _0224C110 // =0x02259678
	ldr r3, _0224C12C // =0x000001F3
	bl __msl_assertion_failed
_0224C098:
	mov r0, r7
	mov r1, r6
	bl FUN_0224B4B4
	cmp r0, #0
	addeq sp, sp, #0x18
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, #0
	str r7, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	ldr r0, [r0, #0x134]
	ldr r1, _0224C130 // =0x0224BE5C
	add r2, sp, #8
	bl TableMap
	ldr r1, [sp, #0x14]
	mov r0, r7
	stmia sp, {r1, r4}
	ldr r2, [sp, #0xc]
	ldr r3, [sp, #0x10]
	mov r1, r6
	blx r5
	ldr r0, [sp, #0x10]
	bl DWCi_GsFree
	ldr r0, [sp, #0x14]
	bl DWCi_GsFree
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_0224C108: .word 0x022598B8
_0224C10C: .word 0x02259818
_0224C110: .word 0x02259678
_0224C114: .word 0x022598D4
_0224C118: .word 0x000001F2
_0224C11C: .word 0x022598E4
_0224C120: .word 0x00000101
_0224C124: .word 0x022598F8
_0224C128: .word 0x022599A8
_0224C12C: .word 0x000001F3
_0224C130: .word 0x0224BE5C
	arm_func_end FUN_0224BFE8

	arm_func_start FUN_0224C134
FUN_0224C134: // 0x0224C134
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224C158
	ldr r0, _0224C174 // =0x022598B8
	ldr r1, _0224C178 // =0x02259818
	ldr r2, _0224C17C // =0x0225955C
	ldr r3, _0224C180 // =0x0000020F
	bl __msl_assertion_failed
_0224C158:
	mov r0, r5
	mov r1, r4
	bl FUN_0224B4B4
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224C174: .word 0x022598B8
_0224C178: .word 0x02259818
_0224C17C: .word 0x0225955C
_0224C180: .word 0x0000020F
	arm_func_end FUN_0224C134

	arm_func_start FUN_0224C184
FUN_0224C184: // 0x0224C184
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	mov r5, r1
	mov r4, r2
	bne _0224C1AC
	ldr r0, _0224C250 // =0x022598B8
	ldr r1, _0224C254 // =0x02259818
	ldr r2, _0224C258 // =0x022595B0
	ldr r3, _0224C25C // =0x0000021A
	bl __msl_assertion_failed
_0224C1AC:
	cmp r5, #0
	bne _0224C1C8
	ldr r0, _0224C260 // =0x022598D4
	ldr r1, _0224C254 // =0x02259818
	ldr r2, _0224C258 // =0x022595B0
	mov r3, #0x21c
	bl __msl_assertion_failed
_0224C1C8:
	ldrsb r0, [r5]
	cmp r0, #0
	bne _0224C1E8
	ldr r0, _0224C264 // =0x022598E4
	ldr r1, _0224C254 // =0x02259818
	ldr r2, _0224C258 // =0x022595B0
	mov r3, #0x21c
	bl __msl_assertion_failed
_0224C1E8:
	mov r0, r5
	bl strlen
	ldr r1, _0224C268 // =0x00000101
	cmp r0, r1
	blo _0224C210
	ldr r0, _0224C26C // =0x022598F8
	ldr r1, _0224C254 // =0x02259818
	ldr r2, _0224C258 // =0x022595B0
	mov r3, #0x21c
	bl __msl_assertion_failed
_0224C210:
	mov r0, r6
	mov r1, r5
	bl FUN_0224B4B4
	movs r1, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, [r1, #0x158]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r4
	add r1, r1, #0x138
	mov r2, #0x20
	bl memcpy
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_0224C250: .word 0x022598B8
_0224C254: .word 0x02259818
_0224C258: .word 0x022595B0
_0224C25C: .word 0x0000021A
_0224C260: .word 0x022598D4
_0224C264: .word 0x022598E4
_0224C268: .word 0x00000101
_0224C26C: .word 0x022598F8
	arm_func_end FUN_0224C184
_0224C270:
	.byte 0x70, 0x40, 0x2D, 0xE9, 0x00, 0x60, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x02, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0xA0, 0x00, 0x9F, 0xE5, 0xA0, 0x10, 0x9F, 0xE5, 0xA0, 0x20, 0x9F, 0xE5
	.byte 0xA0, 0x30, 0x9F, 0xE5, 0x78, 0x1F, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x94, 0x00, 0x9F, 0xE5, 0x84, 0x10, 0x9F, 0xE5, 0x84, 0x20, 0x9F, 0xE5, 0x8C, 0x30, 0x9F, 0xE5
	.byte 0x71, 0x1F, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x7C, 0x00, 0x9F, 0xE5, 0x64, 0x10, 0x9F, 0xE5, 0x64, 0x20, 0x9F, 0xE5, 0x6C, 0x30, 0x9F, 0xE5
	.byte 0x69, 0x1F, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0x20, 0x32, 0xFA, 0xEB, 0x64, 0x30, 0x9F, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE1, 0x04, 0x00, 0x00, 0x3A, 0x5C, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5
	.byte 0x3C, 0x20, 0x9F, 0xE5, 0x4D, 0x3F, 0x83, 0xE2, 0x5F, 0x1F, 0xFA, 0xEB, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x05, 0x10, 0xA0, 0xE1, 0x6A, 0xFC, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x70, 0x80, 0xBD, 0x08
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x58, 0x11, 0x80, 0xE5, 0x04, 0x10, 0xA0, 0xE1, 0x4E, 0x0F, 0x80, 0xE2
	.byte 0x20, 0x20, 0xA0, 0xE3, 0x63, 0x23, 0xFA, 0xEB, 0x70, 0x80, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0x88, 0x95, 0x25, 0x02, 0x33, 0x02, 0x00, 0x00, 0xD4, 0x98, 0x25, 0x02
	.byte 0x35, 0x02, 0x00, 0x00, 0xE4, 0x98, 0x25, 0x02, 0x01, 0x01, 0x00, 0x00, 0xF8, 0x98, 0x25, 0x02
	.byte 0x70, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x02, 0x60, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0xC4, 0x00, 0x9F, 0xE5, 0xC4, 0x10, 0x9F, 0xE5, 0xC4, 0x20, 0x9F, 0xE5
	.byte 0xC4, 0x30, 0x9F, 0xE5, 0x40, 0x1F, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0xB8, 0x00, 0x9F, 0xE5, 0xA8, 0x10, 0x9F, 0xE5, 0xA8, 0x20, 0x9F, 0xE5, 0x93, 0x3F, 0xA0, 0xE3
	.byte 0x39, 0x1F, 0xFA, 0xEB, 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x9C, 0x00, 0x9F, 0xE5, 0x88, 0x10, 0x9F, 0xE5, 0x88, 0x20, 0x9F, 0xE5, 0x93, 0x3F, 0xA0, 0xE3
	.byte 0x31, 0x1F, 0xFA, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0xE8, 0x31, 0xFA, 0xEB, 0x84, 0x10, 0x9F, 0xE5
	.byte 0x01, 0x00, 0x50, 0xE1, 0x04, 0x00, 0x00, 0x3A, 0x7C, 0x00, 0x9F, 0xE5, 0x60, 0x10, 0x9F, 0xE5
	.byte 0x60, 0x20, 0x9F, 0xE5, 0x93, 0x3F, 0xA0, 0xE3, 0x27, 0x1F, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x32, 0xFC, 0xFF, 0xEB, 0x00, 0x50, 0xB0, 0xE1, 0x70, 0x80, 0xBD, 0x08
	.byte 0x5C, 0x01, 0x95, 0xE5, 0x2D, 0x2D, 0xFE, 0xEB, 0x00, 0x00, 0x56, 0xE3, 0x4C, 0x60, 0x9F, 0x05
	.byte 0x06, 0x00, 0xA0, 0xE1, 0xD5, 0x31, 0xFA, 0xEB, 0x00, 0x40, 0xA0, 0xE1, 0x01, 0x00, 0x84, 0xE2
	.byte 0x1A, 0x2D, 0xFE, 0xEB, 0x5C, 0x01, 0x85, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x70, 0x80, 0xBD, 0x08
	.byte 0x06, 0x10, 0xA0, 0xE1, 0x01, 0x20, 0x84, 0xE2, 0x22, 0x23, 0xFA, 0xEB, 0x70, 0x80, 0xBD, 0xE8
	.byte 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0xCC, 0x96, 0x25, 0x02, 0x4A, 0x02, 0x00, 0x00
	.byte 0xD4, 0x98, 0x25, 0x02, 0xE4, 0x98, 0x25, 0x02, 0x01, 0x01, 0x00, 0x00, 0xF8, 0x98, 0x25, 0x02
	.byte 0x2C, 0x99, 0x25, 0x02

	arm_func_start FUN_0224C454
FUN_0224C454: // 0x0224C454
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224C478
	ldr r0, _0224C4F8 // =0x022598B8
	ldr r1, _0224C4FC // =0x02259818
	ldr r2, _0224C500 // =0x022596E4
	mov r3, #0x278
	bl __msl_assertion_failed
_0224C478:
	cmp r4, #0
	bne _0224C494
	ldr r0, _0224C504 // =0x022598D4
	ldr r1, _0224C4FC // =0x02259818
	ldr r2, _0224C500 // =0x022596E4
	ldr r3, _0224C508 // =0x0000027A
	bl __msl_assertion_failed
_0224C494:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _0224C4B4
	ldr r0, _0224C50C // =0x022598E4
	ldr r1, _0224C4FC // =0x02259818
	ldr r2, _0224C500 // =0x022596E4
	ldr r3, _0224C508 // =0x0000027A
	bl __msl_assertion_failed
_0224C4B4:
	mov r0, r4
	bl strlen
	ldr r1, _0224C510 // =0x00000101
	cmp r0, r1
	blo _0224C4DC
	ldr r0, _0224C514 // =0x022598F8
	ldr r1, _0224C4FC // =0x02259818
	ldr r2, _0224C500 // =0x022596E4
	ldr r3, _0224C508 // =0x0000027A
	bl __msl_assertion_failed
_0224C4DC:
	mov r0, r5
	mov r1, r4
	bl FUN_0224B4B4
	cmp r0, #0
	movne r1, #1
	strne r1, [r0, #0x160]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224C4F8: .word 0x022598B8
_0224C4FC: .word 0x02259818
_0224C500: .word 0x022596E4
_0224C504: .word 0x022598D4
_0224C508: .word 0x0000027A
_0224C50C: .word 0x022598E4
_0224C510: .word 0x00000101
_0224C514: .word 0x022598F8
	arm_func_end FUN_0224C454

	arm_func_start FUN_0224C518
FUN_0224C518: // 0x0224C518
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224C53C
	ldr r0, _0224C5BC // =0x022598B8
	ldr r1, _0224C5C0 // =0x02259818
	ldr r2, _0224C5C4 // =0x022597A4
	ldr r3, _0224C5C8 // =0x0000028A
	bl __msl_assertion_failed
_0224C53C:
	cmp r4, #0
	bne _0224C558
	ldr r0, _0224C5CC // =0x022598D4
	ldr r1, _0224C5C0 // =0x02259818
	ldr r2, _0224C5C4 // =0x022597A4
	mov r3, #0x28c
	bl __msl_assertion_failed
_0224C558:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _0224C578
	ldr r0, _0224C5D0 // =0x022598E4
	ldr r1, _0224C5C0 // =0x02259818
	ldr r2, _0224C5C4 // =0x022597A4
	mov r3, #0x28c
	bl __msl_assertion_failed
_0224C578:
	mov r0, r4
	bl strlen
	ldr r1, _0224C5D4 // =0x00000101
	cmp r0, r1
	blo _0224C5A0
	ldr r0, _0224C5D8 // =0x022598F8
	ldr r1, _0224C5C0 // =0x02259818
	ldr r2, _0224C5C4 // =0x022597A4
	mov r3, #0x28c
	bl __msl_assertion_failed
_0224C5A0:
	mov r0, r5
	mov r1, r4
	bl FUN_0224B4B4
	cmp r0, #0
	moveq r0, #0
	ldrne r0, [r0, #0x160]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224C5BC: .word 0x022598B8
_0224C5C0: .word 0x02259818
_0224C5C4: .word 0x022597A4
_0224C5C8: .word 0x0000028A
_0224C5CC: .word 0x022598D4
_0224C5D0: .word 0x022598E4
_0224C5D4: .word 0x00000101
_0224C5D8: .word 0x022598F8
	arm_func_end FUN_0224C518
_0224C5DC:
	.byte 0x70, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x60, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x02, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xA0, 0x00, 0x9F, 0xE5, 0xA0, 0x10, 0x9F, 0xE5, 0xA0, 0x20, 0x9F, 0xE5, 0xA0, 0x30, 0x9F, 0xE5
	.byte 0x9D, 0x1E, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x94, 0x00, 0x9F, 0xE5
	.byte 0x84, 0x10, 0x9F, 0xE5, 0x84, 0x20, 0x9F, 0xE5, 0xA7, 0x3F, 0xA0, 0xE3, 0x96, 0x1E, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x78, 0x00, 0x9F, 0xE5
	.byte 0x64, 0x10, 0x9F, 0xE5, 0x64, 0x20, 0x9F, 0xE5, 0xA7, 0x3F, 0xA0, 0xE3, 0x8E, 0x1E, 0xFA, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x45, 0x31, 0xFA, 0xEB, 0x60, 0x10, 0x9F, 0xE5, 0x01, 0x00, 0x50, 0xE1
	.byte 0x04, 0x00, 0x00, 0x3A, 0x58, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0xA7, 0x3F, 0xA0, 0xE3, 0x84, 0x1E, 0xFA, 0xEB, 0x06, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x8F, 0xFB, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x70, 0x80, 0xBD, 0x08, 0x05, 0x10, 0xA0, 0xE1
	.byte 0x59, 0x0F, 0x84, 0xE2, 0x80, 0x20, 0xA0, 0xE3, 0x6D, 0x31, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3
	.byte 0xE3, 0x01, 0xC4, 0xE5, 0x70, 0x80, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02
	.byte 0x28, 0x96, 0x25, 0x02, 0x9A, 0x02, 0x00, 0x00, 0xD4, 0x98, 0x25, 0x02, 0xE4, 0x98, 0x25, 0x02
	.byte 0x01, 0x01, 0x00, 0x00, 0xF8, 0x98, 0x25, 0x02

	arm_func_start FUN_0224C6B8
FUN_0224C6B8: // 0x0224C6B8
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224C6DC
	ldr r0, _0224C764 // =0x022598B8
	ldr r1, _0224C768 // =0x02259818
	ldr r2, _0224C76C // =0x02259714
	mov r3, #0x2bc
	bl __msl_assertion_failed
_0224C6DC:
	cmp r4, #0
	bne _0224C6F8
	ldr r0, _0224C770 // =0x022598D4
	ldr r1, _0224C768 // =0x02259818
	ldr r2, _0224C76C // =0x02259714
	ldr r3, _0224C774 // =0x000002BE
	bl __msl_assertion_failed
_0224C6F8:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _0224C718
	ldr r0, _0224C778 // =0x022598E4
	ldr r1, _0224C768 // =0x02259818
	ldr r2, _0224C76C // =0x02259714
	ldr r3, _0224C774 // =0x000002BE
	bl __msl_assertion_failed
_0224C718:
	mov r0, r4
	bl strlen
	ldr r1, _0224C77C // =0x00000101
	cmp r0, r1
	blo _0224C740
	ldr r0, _0224C780 // =0x022598F8
	ldr r1, _0224C768 // =0x02259818
	ldr r2, _0224C76C // =0x02259714
	ldr r3, _0224C774 // =0x000002BE
	bl __msl_assertion_failed
_0224C740:
	mov r0, r5
	mov r1, r4
	bl FUN_0224B4B4
	cmp r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r0, #0x134]
	bl TableCount
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224C764: .word 0x022598B8
_0224C768: .word 0x02259818
_0224C76C: .word 0x02259714
_0224C770: .word 0x022598D4
_0224C774: .word 0x000002BE
_0224C778: .word 0x022598E4
_0224C77C: .word 0x00000101
_0224C780: .word 0x022598F8
	arm_func_end FUN_0224C6B8
_0224C784:
	.byte 0xF8, 0x40, 0x2D, 0xE9, 0xE0, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1
	.byte 0x01, 0x60, 0xA0, 0xE1, 0x02, 0x40, 0xA0, 0xE1, 0x03, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xB8, 0x01, 0x9F, 0xE5, 0xB8, 0x11, 0x9F, 0xE5, 0xB8, 0x21, 0x9F, 0xE5, 0x2D, 0x3E, 0xA0, 0xE3
	.byte 0x31, 0x1E, 0xFA, 0xEB, 0x00, 0x00, 0x56, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xA8, 0x01, 0x9F, 0xE5
	.byte 0x9C, 0x11, 0x9F, 0xE5, 0x9C, 0x21, 0x9F, 0xE5, 0xA0, 0x31, 0x9F, 0xE5, 0x2A, 0x1E, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD6, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x90, 0x01, 0x9F, 0xE5
	.byte 0x7C, 0x11, 0x9F, 0xE5, 0x7C, 0x21, 0x9F, 0xE5, 0x80, 0x31, 0x9F, 0xE5, 0x22, 0x1E, 0xFA, 0xEB
	.byte 0x06, 0x00, 0xA0, 0xE1, 0xD9, 0x30, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A
	.byte 0x70, 0x01, 0x9F, 0xE5, 0x58, 0x11, 0x9F, 0xE5, 0x58, 0x21, 0x9F, 0xE5, 0x5C, 0x31, 0x9F, 0xE5
	.byte 0x19, 0x1E, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x58, 0x01, 0x9F, 0xE5
	.byte 0x3C, 0x11, 0x9F, 0xE5, 0x3C, 0x21, 0x9F, 0xE5, 0x50, 0x31, 0x9F, 0xE5, 0x12, 0x1E, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x40, 0x01, 0x9F, 0xE5
	.byte 0x1C, 0x11, 0x9F, 0xE5, 0x1C, 0x21, 0x9F, 0xE5, 0x30, 0x31, 0x9F, 0xE5, 0x0A, 0x1E, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x55, 0xE3, 0x01, 0x00, 0x00, 0xBA, 0x03, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0xDA
	.byte 0x20, 0x01, 0x9F, 0xE5, 0xF8, 0x10, 0x9F, 0xE5, 0xF8, 0x20, 0x9F, 0xE5, 0xB5, 0x3F, 0xA0, 0xE3
	.byte 0x01, 0x1E, 0xFA, 0xEB, 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x0C, 0xFB, 0xFF, 0xEB
	.byte 0x00, 0x40, 0xB0, 0xE1, 0xE0, 0xD0, 0x8D, 0x02, 0xF8, 0x80, 0xBD, 0x08, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0xE0, 0x8D, 0xE2, 0x00, 0x10, 0xA0, 0xE1, 0x00, 0x20, 0xA0, 0xE1, 0x00, 0x30, 0xA0, 0xE1
	.byte 0x07, 0xC0, 0xA0, 0xE3, 0x0F, 0x00, 0xAE, 0xE8, 0x0F, 0x00, 0xAE, 0xE8, 0x01, 0xC0, 0x5C, 0xE2
	.byte 0xFB, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0x8D, 0xE2, 0x06, 0x10, 0xA0, 0xE1, 0x80, 0x20, 0xA0, 0xE3
	.byte 0xDF, 0x30, 0xFA, 0xEB, 0xF8, 0x10, 0x9D, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x7F, 0x00, 0xCD, 0xE5
	.byte 0x00, 0x00, 0x51, 0xE3, 0xFC, 0x00, 0x9D, 0x15, 0x00, 0x00, 0x50, 0x13, 0x00, 0x00, 0xA0, 0x03
	.byte 0xD8, 0x00, 0x8D, 0x05, 0x0C, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x8D, 0xE2, 0x18, 0x20, 0xA0, 0xE3
	.byte 0xD3, 0x30, 0xFA, 0xEB, 0x00, 0x30, 0xA0, 0xE3, 0xFC, 0x10, 0x9D, 0xE5, 0x98, 0x00, 0x8D, 0xE2
	.byte 0x40, 0x20, 0xA0, 0xE3, 0x97, 0x30, 0xCD, 0xE5, 0xCD, 0x30, 0xFA, 0xEB, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x01, 0x00, 0xA0, 0xE3, 0xD7, 0x10, 0xCD, 0xE5, 0xD8, 0x00, 0x8D, 0xE5, 0xDC, 0x50, 0x8D, 0xE5
	.byte 0x34, 0x01, 0x94, 0xE5, 0x00, 0x10, 0x8D, 0xE2, 0xC7, 0x76, 0xFE, 0xEB, 0x34, 0x01, 0x94, 0xE5
	.byte 0x00, 0x10, 0x8D, 0xE2, 0x17, 0x77, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0xE0, 0xD0, 0x8D, 0x12
	.byte 0xF8, 0x80, 0xBD, 0x18, 0x40, 0x00, 0x9F, 0xE5, 0x14, 0x10, 0x9F, 0xE5, 0x14, 0x20, 0x9F, 0xE5
	.byte 0x38, 0x30, 0x9F, 0xE5, 0xC8, 0x1D, 0xFA, 0xEB, 0xE0, 0xD0, 0x8D, 0xE2, 0xF8, 0x80, 0xBD, 0xE8
	.byte 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x2C, 0x97, 0x25, 0x02, 0xBC, 0x99, 0x25, 0x02
	.byte 0xD2, 0x02, 0x00, 0x00, 0xCC, 0x99, 0x25, 0x02, 0xDC, 0x99, 0x25, 0x02, 0xD4, 0x98, 0x25, 0x02
	.byte 0xD3, 0x02, 0x00, 0x00, 0xE4, 0x98, 0x25, 0x02, 0xF4, 0x99, 0x25, 0x02, 0x10, 0x9A, 0x25, 0x02
	.byte 0xF7, 0x02, 0x00, 0x00, 0x70, 0x40, 0x2D, 0xE9, 0xE0, 0xD0, 0x4D, 0xE2, 0x00, 0x60, 0xB0, 0xE1
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x02, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xEC, 0x00, 0x9F, 0xE5
	.byte 0xEC, 0x10, 0x9F, 0xE5, 0xEC, 0x20, 0x9F, 0xE5, 0xEC, 0x30, 0x9F, 0xE5, 0xAE, 0x1D, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xE0, 0x00, 0x9F, 0xE5, 0xD0, 0x10, 0x9F, 0xE5
	.byte 0xD0, 0x20, 0x9F, 0xE5, 0x03, 0x3C, 0xA0, 0xE3, 0xA7, 0x1D, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xC4, 0x00, 0x9F, 0xE5, 0xB0, 0x10, 0x9F, 0xE5
	.byte 0xB0, 0x20, 0x9F, 0xE5, 0x03, 0x3C, 0xA0, 0xE3, 0x9F, 0x1D, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x56, 0x30, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0xA4, 0x00, 0x9F, 0xE5
	.byte 0x8C, 0x10, 0x9F, 0xE5, 0x8C, 0x20, 0x9F, 0xE5, 0x03, 0x3C, 0xA0, 0xE3, 0x96, 0x1D, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x8C, 0x00, 0x9F, 0xE5, 0x70, 0x10, 0x9F, 0xE5
	.byte 0x70, 0x20, 0x9F, 0xE5, 0x84, 0x30, 0x9F, 0xE5, 0x8F, 0x1D, 0xFA, 0xEB, 0xD0, 0x00, 0xD4, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x74, 0x00, 0x9F, 0xE5, 0x50, 0x10, 0x9F, 0xE5
	.byte 0x50, 0x20, 0x9F, 0xE5, 0x64, 0x30, 0x9F, 0xE5, 0x87, 0x1D, 0xFA, 0xEB, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x92, 0xFA, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0xE0, 0xD0, 0x8D, 0x02
	.byte 0x70, 0x80, 0xBD, 0x08, 0x00, 0x00, 0x8D, 0xE2, 0x05, 0x10, 0xA0, 0xE1, 0x80, 0x20, 0xA0, 0xE3
	.byte 0x6F, 0x30, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0x7F, 0x00, 0xCD, 0xE5, 0x34, 0x01, 0x94, 0xE5
	.byte 0x00, 0x10, 0x8D, 0xE2, 0x96, 0x76, 0xFE, 0xEB, 0xE0, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8
	.byte 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0xEC, 0x95, 0x25, 0x02, 0xFE, 0x02, 0x00, 0x00
	.byte 0x44, 0x9A, 0x25, 0x02, 0x54, 0x9A, 0x25, 0x02, 0x64, 0x9A, 0x25, 0x02, 0xD4, 0x98, 0x25, 0x02
	.byte 0x01, 0x03, 0x00, 0x00, 0xE4, 0x98, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xD4, 0x00, 0x9F, 0xE5, 0xD4, 0x10, 0x9F, 0xE5
	.byte 0xD4, 0x20, 0x9F, 0xE5, 0xC6, 0x3F, 0xA0, 0xE3, 0x63, 0x1D, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xC4, 0x00, 0x9F, 0xE5, 0xB8, 0x10, 0x9F, 0xE5, 0xB8, 0x20, 0x9F, 0xE5
	.byte 0xBC, 0x30, 0x9F, 0xE5, 0x5C, 0x1D, 0xFA, 0xEB, 0x34, 0x01, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xAC, 0x00, 0x9F, 0xE5, 0x98, 0x10, 0x9F, 0xE5, 0x98, 0x20, 0x9F, 0xE5
	.byte 0xA4, 0x30, 0x9F, 0xE5, 0x54, 0x1D, 0xFA, 0xEB, 0x04, 0x00, 0x95, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x94, 0x00, 0x9F, 0xE5, 0x78, 0x10, 0x9F, 0xE5, 0x78, 0x20, 0x9F, 0xE5
	.byte 0x8C, 0x30, 0x9F, 0xE5, 0x4C, 0x1D, 0xFA, 0xEB, 0x04, 0x00, 0x95, 0xE5, 0xD0, 0x00, 0xD0, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x78, 0x00, 0x9F, 0xE5, 0x54, 0x10, 0x9F, 0xE5
	.byte 0x54, 0x20, 0x9F, 0xE5, 0xC9, 0x3F, 0xA0, 0xE3, 0x43, 0x1D, 0xFA, 0xEB, 0x08, 0x00, 0x95, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x5C, 0x00, 0x9F, 0xE5, 0x34, 0x10, 0x9F, 0xE5
	.byte 0x34, 0x20, 0x9F, 0xE5, 0x54, 0x30, 0x9F, 0xE5, 0x3B, 0x1D, 0xFA, 0xEB, 0x34, 0x01, 0x94, 0xE5
	.byte 0x04, 0x10, 0x95, 0xE5, 0x7F, 0x76, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x38, 0x80, 0xBD, 0x08
	.byte 0x0C, 0x30, 0x95, 0xE5, 0x04, 0x20, 0xA0, 0xE1, 0x03, 0x10, 0x95, 0xE8, 0x3C, 0xFF, 0x2F, 0xE1
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x8C, 0x97, 0x25, 0x02
	.byte 0x30, 0x99, 0x25, 0x02, 0x19, 0x03, 0x00, 0x00, 0x7C, 0x9A, 0x25, 0x02, 0x1E, 0x03, 0x00, 0x00
	.byte 0x94, 0x9A, 0x25, 0x02, 0x23, 0x03, 0x00, 0x00, 0xA8, 0x9A, 0x25, 0x02, 0xC4, 0x9A, 0x25, 0x02
	.byte 0x25, 0x03, 0x00, 0x00, 0xF8, 0x40, 0x2D, 0xE9, 0xF0, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1
	.byte 0x01, 0x60, 0xA0, 0xE1, 0x02, 0x50, 0xA0, 0xE1, 0x03, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xCC, 0x00, 0x9F, 0xE5, 0xCC, 0x10, 0x9F, 0xE5, 0xCC, 0x20, 0x9F, 0xE5, 0xCC, 0x30, 0x9F, 0xE5
	.byte 0x19, 0x1D, 0xFA, 0xEB, 0x00, 0x00, 0x56, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xC0, 0x00, 0x9F, 0xE5
	.byte 0xB0, 0x10, 0x9F, 0xE5, 0xB0, 0x20, 0x9F, 0xE5, 0xCE, 0x3F, 0xA0, 0xE3, 0x12, 0x1D, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD6, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xA4, 0x00, 0x9F, 0xE5
	.byte 0x90, 0x10, 0x9F, 0xE5, 0x90, 0x20, 0x9F, 0xE5, 0xCE, 0x3F, 0xA0, 0xE3, 0x0A, 0x1D, 0xFA, 0xEB
	.byte 0x06, 0x00, 0xA0, 0xE1, 0xC1, 0x2F, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A
	.byte 0x84, 0x00, 0x9F, 0xE5, 0x6C, 0x10, 0x9F, 0xE5, 0x6C, 0x20, 0x9F, 0xE5, 0xCE, 0x3F, 0xA0, 0xE3
	.byte 0x01, 0x1D, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x6C, 0x00, 0x9F, 0xE5
	.byte 0x50, 0x10, 0x9F, 0xE5, 0x50, 0x20, 0x9F, 0xE5, 0x64, 0x30, 0x9F, 0xE5, 0xFA, 0x1C, 0xFA, 0xEB
	.byte 0x10, 0x00, 0x8D, 0xE2, 0x06, 0x10, 0xA0, 0xE1, 0x80, 0x20, 0xA0, 0xE3, 0xE8, 0x2F, 0xFA, 0xEB
	.byte 0x00, 0x10, 0xA0, 0xE3, 0x10, 0x00, 0x8D, 0xE2, 0x8F, 0x10, 0xCD, 0xE5, 0x00, 0x70, 0x8D, 0xE5
	.byte 0x08, 0x50, 0x8D, 0xE5, 0x0C, 0x40, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5, 0x0C, 0x08, 0x97, 0xE5
	.byte 0x30, 0x10, 0x9F, 0xE5, 0x00, 0x20, 0x8D, 0xE2, 0x5A, 0x76, 0xFE, 0xEB, 0xF0, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x80, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0x50, 0x96, 0x25, 0x02
	.byte 0x36, 0x03, 0x00, 0x00, 0x44, 0x9A, 0x25, 0x02, 0x54, 0x9A, 0x25, 0x02, 0x64, 0x9A, 0x25, 0x02
	.byte 0xA8, 0x99, 0x25, 0x02, 0x39, 0x03, 0x00, 0x00, 0xC8, 0xCA, 0x24, 0x02, 0x78, 0x40, 0x2D, 0xE9
	.byte 0xFC, 0xD0, 0x4D, 0xE2, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xF4, 0x01, 0x9F, 0xE5, 0xF4, 0x11, 0x9F, 0xE5, 0xF4, 0x21, 0x9F, 0xE5, 0xF4, 0x31, 0x9F, 0xE5
	.byte 0xD5, 0x1C, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xE8, 0x01, 0x9F, 0xE5
	.byte 0xD8, 0x11, 0x9F, 0xE5, 0xD8, 0x21, 0x9F, 0xE5, 0x35, 0x3E, 0xA0, 0xE3, 0xCE, 0x1C, 0xFA, 0xEB
	.byte 0x34, 0x01, 0x95, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xCC, 0x01, 0x9F, 0xE5
	.byte 0xB8, 0x11, 0x9F, 0xE5, 0xB8, 0x21, 0x9F, 0xE5, 0xC4, 0x31, 0x9F, 0xE5, 0xC6, 0x1C, 0xFA, 0xEB
	.byte 0x08, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xB4, 0x01, 0x9F, 0xE5
	.byte 0x98, 0x11, 0x9F, 0xE5, 0x98, 0x21, 0x9F, 0xE5, 0xAC, 0x31, 0x9F, 0xE5, 0xBE, 0x1C, 0xFA, 0xEB
	.byte 0x08, 0x00, 0x94, 0xE5, 0xD0, 0x00, 0xD0, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x98, 0x01, 0x9F, 0xE5, 0x74, 0x11, 0x9F, 0xE5, 0x74, 0x21, 0x9F, 0xE5, 0x88, 0x31, 0x9F, 0xE5
	.byte 0xB5, 0x1C, 0xFA, 0xEB, 0x08, 0x00, 0x94, 0xE5, 0x6C, 0x2F, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x3A, 0x78, 0x01, 0x9F, 0xE5, 0x50, 0x11, 0x9F, 0xE5, 0x50, 0x21, 0x9F, 0xE5
	.byte 0x64, 0x31, 0x9F, 0xE5, 0xAC, 0x1C, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x5C, 0x01, 0x9F, 0xE5, 0x30, 0x11, 0x9F, 0xE5, 0x30, 0x21, 0x9F, 0xE5
	.byte 0x54, 0x31, 0x9F, 0xE5, 0xA4, 0x1C, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0xD0, 0x00, 0xD0, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x40, 0x01, 0x9F, 0xE5, 0x0C, 0x11, 0x9F, 0xE5
	.byte 0x0C, 0x21, 0x9F, 0xE5, 0x30, 0x31, 0x9F, 0xE5, 0x9B, 0x1C, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5
	.byte 0x52, 0x2F, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0x20, 0x01, 0x9F, 0xE5
	.byte 0xE8, 0x10, 0x9F, 0xE5, 0xE8, 0x20, 0x9F, 0xE5, 0x0C, 0x31, 0x9F, 0xE5, 0x92, 0x1C, 0xFA, 0xEB
	.byte 0x34, 0x01, 0x95, 0xE5, 0x04, 0x10, 0x94, 0xE5, 0xD6, 0x75, 0xFE, 0xEB, 0x00, 0x60, 0xB0, 0xE1
	.byte 0xFC, 0xD0, 0x8D, 0x02, 0x78, 0x80, 0xBD, 0x08, 0x1C, 0x00, 0x8D, 0xE2, 0x06, 0x10, 0xA0, 0xE1
	.byte 0xE0, 0x20, 0xA0, 0xE3, 0x97, 0x20, 0xFA, 0xEB, 0x34, 0x01, 0x95, 0xE5, 0x06, 0x10, 0xA0, 0xE1
	.byte 0xA3, 0x75, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xD4, 0x00, 0x9F, 0xE5
	.byte 0x98, 0x10, 0x9F, 0xE5, 0x98, 0x20, 0x9F, 0xE5, 0xCC, 0x30, 0x9F, 0xE5, 0x7E, 0x1C, 0xFA, 0xEB
	.byte 0x08, 0x10, 0x94, 0xE5, 0x1C, 0x00, 0x8D, 0xE2, 0x80, 0x20, 0xA0, 0xE3, 0x6C, 0x2F, 0xFA, 0xEB
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x9B, 0x00, 0xCD, 0xE5, 0x34, 0x01, 0x95, 0xE5, 0x1C, 0x10, 0x8D, 0xE2
	.byte 0x69, 0x75, 0xFE, 0xEB, 0x00, 0x00, 0x94, 0xE5, 0x05, 0x10, 0xA0, 0xE1, 0x99, 0xFD, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x14, 0x01, 0x95, 0x15, 0x00, 0x00, 0x50, 0x13, 0xFC, 0xD0, 0x8D, 0x02
	.byte 0x78, 0x80, 0xBD, 0x08, 0x10, 0x50, 0x8D, 0xE5, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x14, 0x00, 0x8D, 0xE5, 0x08, 0x20, 0x94, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x18, 0x20, 0x8D, 0xE5
	.byte 0x30, 0x21, 0x95, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x00, 0x20, 0x8D, 0xE5, 0x22, 0x00, 0x8D, 0xE9
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x94, 0xE5, 0x14, 0x21, 0x95, 0xE5, 0x08, 0x10, 0xA0, 0xE3
	.byte 0x49, 0xEF, 0xFF, 0xEB, 0xFC, 0xD0, 0x8D, 0xE2, 0x78, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0x8C, 0x96, 0x25, 0x02, 0x4F, 0x03, 0x00, 0x00, 0x30, 0x99, 0x25, 0x02
	.byte 0x7C, 0x9A, 0x25, 0x02, 0x55, 0x03, 0x00, 0x00, 0xDC, 0x9A, 0x25, 0x02, 0x5A, 0x03, 0x00, 0x00
	.byte 0xF4, 0x9A, 0x25, 0x02, 0x10, 0x9B, 0x25, 0x02, 0x34, 0x9B, 0x25, 0x02, 0x5B, 0x03, 0x00, 0x00
	.byte 0x4C, 0x9B, 0x25, 0x02, 0x68, 0x9B, 0x25, 0x02, 0x8C, 0x9B, 0x25, 0x02, 0x67, 0x03, 0x00, 0x00
	.byte 0x78, 0x40, 0x2D, 0xE9, 0x0C, 0xD0, 0x4D, 0xE2, 0x00, 0x60, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1
	.byte 0x02, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xF0, 0x00, 0x9F, 0xE5, 0xF0, 0x10, 0x9F, 0xE5
	.byte 0xF0, 0x20, 0x9F, 0xE5, 0xF0, 0x30, 0x9F, 0xE5, 0x3F, 0x1C, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xE4, 0x00, 0x9F, 0xE5, 0xD4, 0x10, 0x9F, 0xE5, 0xD4, 0x20, 0x9F, 0xE5
	.byte 0xE2, 0x3F, 0xA0, 0xE3, 0x38, 0x1C, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xC8, 0x00, 0x9F, 0xE5, 0xB4, 0x10, 0x9F, 0xE5, 0xB4, 0x20, 0x9F, 0xE5
	.byte 0xE2, 0x3F, 0xA0, 0xE3, 0x30, 0x1C, 0xFA, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0xE7, 0x2E, 0xFA, 0xEB
	.byte 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0xA8, 0x00, 0x9F, 0xE5, 0x90, 0x10, 0x9F, 0xE5
	.byte 0x90, 0x20, 0x9F, 0xE5, 0xE2, 0x3F, 0xA0, 0xE3, 0x27, 0x1C, 0xFA, 0xEB, 0x00, 0x00, 0x54, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x90, 0x00, 0x9F, 0xE5, 0x74, 0x10, 0x9F, 0xE5, 0x74, 0x20, 0x9F, 0xE5
	.byte 0x88, 0x30, 0x9F, 0xE5, 0x20, 0x1C, 0xFA, 0xEB, 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x78, 0x00, 0x9F, 0xE5, 0x54, 0x10, 0x9F, 0xE5, 0x54, 0x20, 0x9F, 0xE5
	.byte 0x68, 0x30, 0x9F, 0xE5, 0x18, 0x1C, 0xFA, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0xCF, 0x2E, 0xFA, 0xEB
	.byte 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0x58, 0x00, 0x9F, 0xE5, 0x30, 0x10, 0x9F, 0xE5
	.byte 0x30, 0x20, 0x9F, 0xE5, 0x44, 0x30, 0x9F, 0xE5, 0x0F, 0x1C, 0xFA, 0xEB, 0x00, 0x60, 0x8D, 0xE5
	.byte 0x04, 0x50, 0x8D, 0xE5, 0x08, 0x40, 0x8D, 0xE5, 0x0C, 0x08, 0x96, 0xE5, 0x38, 0x10, 0x9F, 0xE5
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x77, 0x75, 0xFE, 0xEB, 0x0C, 0xD0, 0x8D, 0xE2, 0x78, 0x80, 0xBD, 0xE8
	.byte 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0xC4, 0x95, 0x25, 0x02, 0x86, 0x03, 0x00, 0x00
	.byte 0x98, 0x9B, 0x25, 0x02, 0xA8, 0x9B, 0x25, 0x02, 0xBC, 0x9B, 0x25, 0x02, 0xD8, 0x9B, 0x25, 0x02
	.byte 0x89, 0x03, 0x00, 0x00, 0xE8, 0x9B, 0x25, 0x02, 0xFC, 0x9B, 0x25, 0x02, 0xFC, 0xCC, 0x24, 0x02
	.byte 0xF0, 0x41, 0x2D, 0xE9, 0x2E, 0xDE, 0x4D, 0xE2, 0x00, 0x80, 0xB0, 0xE1, 0x01, 0x70, 0xA0, 0xE1
	.byte 0x02, 0x60, 0xA0, 0xE1, 0x03, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xA0, 0x01, 0x9F, 0xE5
	.byte 0xA0, 0x11, 0x9F, 0xE5, 0xA0, 0x21, 0x9F, 0xE5, 0xA0, 0x31, 0x9F, 0xE5, 0xEE, 0x1B, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x57, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x94, 0x01, 0x9F, 0xE5, 0x84, 0x11, 0x9F, 0xE5
	.byte 0x84, 0x21, 0x9F, 0xE5, 0x8C, 0x31, 0x9F, 0xE5, 0xE7, 0x1B, 0xFA, 0xEB, 0xD0, 0x00, 0xD7, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x7C, 0x01, 0x9F, 0xE5, 0x64, 0x11, 0x9F, 0xE5
	.byte 0x64, 0x21, 0x9F, 0xE5, 0x6C, 0x31, 0x9F, 0xE5, 0xDF, 0x1B, 0xFA, 0xEB, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x96, 0x2E, 0xFA, 0xEB, 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0x5C, 0x01, 0x9F, 0xE5
	.byte 0x40, 0x11, 0x9F, 0xE5, 0x40, 0x21, 0x9F, 0xE5, 0x48, 0x31, 0x9F, 0xE5, 0xD6, 0x1B, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x56, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x44, 0x01, 0x9F, 0xE5, 0x24, 0x11, 0x9F, 0xE5
	.byte 0x24, 0x21, 0x9F, 0xE5, 0x3C, 0x31, 0x9F, 0xE5, 0xCF, 0x1B, 0xFA, 0xEB, 0xD0, 0x00, 0xD6, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x2C, 0x01, 0x9F, 0xE5, 0x04, 0x11, 0x9F, 0xE5
	.byte 0x04, 0x21, 0x9F, 0xE5, 0x1C, 0x31, 0x9F, 0xE5, 0xC7, 0x1B, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x01, 0x00, 0x00, 0xBA, 0x03, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0xDA, 0x0C, 0x01, 0x9F, 0xE5
	.byte 0xE0, 0x10, 0x9F, 0xE5, 0xE0, 0x20, 0x9F, 0xE5, 0x04, 0x31, 0x9F, 0xE5, 0xBE, 0x1B, 0xFA, 0xEB
	.byte 0x00, 0x21, 0x9F, 0xE5, 0xFC, 0x00, 0x8D, 0xE2, 0x06, 0x10, 0xA0, 0xE1, 0xAC, 0x2E, 0xFA, 0xEB
	.byte 0x00, 0x00, 0xA0, 0xE3, 0xFC, 0x01, 0xCD, 0xE5, 0x0C, 0x08, 0x98, 0xE5, 0xFC, 0x10, 0x8D, 0xE2
	.byte 0xFC, 0x74, 0xFE, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x2E, 0xDE, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08
	.byte 0x1C, 0x00, 0x8D, 0xE2, 0x07, 0x10, 0xA0, 0xE1, 0x80, 0x20, 0xA0, 0xE3, 0xA0, 0x2E, 0xFA, 0xEB
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x9B, 0x00, 0xCD, 0xE5, 0x34, 0x01, 0x94, 0xE5, 0x1C, 0x10, 0x8D, 0xE2
	.byte 0xF0, 0x74, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x2E, 0xDE, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08
	.byte 0xF8, 0x12, 0x9D, 0xE5, 0x00, 0x00, 0x51, 0xE3, 0xDC, 0x10, 0x90, 0x15, 0xDC, 0x20, 0x90, 0x05
	.byte 0x05, 0x10, 0x81, 0x11, 0x05, 0x10, 0xE0, 0x01, 0x01, 0x10, 0x02, 0x00, 0xDC, 0x10, 0x80, 0xE5
	.byte 0x20, 0x11, 0x94, 0xE5, 0x00, 0x00, 0x51, 0xE3, 0x2E, 0xDE, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08
	.byte 0x10, 0x60, 0x8D, 0xE5, 0x14, 0x70, 0x8D, 0xE5, 0xDC, 0x00, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x18, 0x00, 0x8D, 0xE5, 0x30, 0x21, 0x94, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5
	.byte 0x42, 0x00, 0x8D, 0xE9, 0x0C, 0x00, 0x8D, 0xE5, 0x20, 0x21, 0x94, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x08, 0x00, 0xA0, 0xE1, 0x0B, 0x10, 0xA0, 0xE3, 0x77, 0xEE, 0xFF, 0xEB, 0x2E, 0xDE, 0x8D, 0xE2
	.byte 0xF0, 0x81, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02, 0x18, 0x98, 0x25, 0x02, 0xD8, 0x95, 0x25, 0x02
	.byte 0x9B, 0x03, 0x00, 0x00, 0x44, 0x9A, 0x25, 0x02, 0x9D, 0x03, 0x00, 0x00, 0x54, 0x9A, 0x25, 0x02
	.byte 0x64, 0x9A, 0x25, 0x02, 0xD4, 0x98, 0x25, 0x02, 0x9E, 0x03, 0x00, 0x00, 0xE4, 0x98, 0x25, 0x02
	.byte 0xF4, 0x99, 0x25, 0x02, 0x9F, 0x03, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x98, 0x00, 0x9F, 0xE5
	.byte 0x98, 0x10, 0x9F, 0xE5, 0x98, 0x20, 0x9F, 0xE5, 0x98, 0x30, 0x9F, 0xE5, 0x72, 0x1B, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x8C, 0x00, 0x9F, 0xE5, 0x7C, 0x10, 0x9F, 0xE5
	.byte 0x7C, 0x20, 0x9F, 0xE5, 0x84, 0x30, 0x9F, 0xE5, 0x6B, 0x1B, 0xFA, 0xEB, 0x34, 0x01, 0x94, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x74, 0x00, 0x9F, 0xE5, 0x5C, 0x10, 0x9F, 0xE5
	.byte 0x5C, 0x20, 0x9F, 0xE5, 0x6C, 0x30, 0x9F, 0xE5, 0x63, 0x1B, 0xFA, 0xEB, 0x34, 0x01, 0x94, 0xE5
	.byte 0x00, 0x10, 0x95, 0xE5, 0xA7, 0x74, 0xFE, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x38, 0x80, 0xBD, 0x08
	.byte 0x04, 0x10, 0x95, 0xE5, 0x80, 0x00, 0x84, 0xE2, 0x18, 0x20, 0xA0, 0xE3, 0x4C, 0x2E, 0xFA, 0xEB
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x97, 0x00, 0xC4, 0xE5, 0x08, 0x10, 0x95, 0xE5, 0x98, 0x00, 0x84, 0xE2
	.byte 0x40, 0x20, 0xA0, 0xE3, 0x46, 0x2E, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0xD7, 0x00, 0xC4, 0xE5
	.byte 0x01, 0x00, 0xA0, 0xE3, 0xD8, 0x00, 0x84, 0xE5, 0x38, 0x80, 0xBD, 0xE8, 0x38, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0x74, 0x97, 0x25, 0x02, 0x1A, 0x04, 0x00, 0x00, 0x30, 0x99, 0x25, 0x02
	.byte 0x1B, 0x04, 0x00, 0x00, 0x7C, 0x9A, 0x25, 0x02, 0x24, 0x04, 0x00, 0x00, 0xF0, 0x40, 0x2D, 0xE9
	.byte 0xEC, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1, 0x02, 0x50, 0xA0, 0xE1
	.byte 0x03, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xAC, 0x00, 0x9F, 0xE5, 0xAC, 0x10, 0x9F, 0xE5
	.byte 0xAC, 0x20, 0x9F, 0xE5, 0xAC, 0x30, 0x9F, 0xE5, 0x3B, 0x1B, 0xFA, 0xEB, 0x00, 0x00, 0x56, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xA0, 0x00, 0x9F, 0xE5, 0x90, 0x10, 0x9F, 0xE5, 0x90, 0x20, 0x9F, 0xE5
	.byte 0x98, 0x30, 0x9F, 0xE5, 0x34, 0x1B, 0xFA, 0xEB, 0xD0, 0x00, 0xD6, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x88, 0x00, 0x9F, 0xE5, 0x70, 0x10, 0x9F, 0xE5, 0x70, 0x20, 0x9F, 0xE5
	.byte 0x78, 0x30, 0x9F, 0xE5, 0x2C, 0x1B, 0xFA, 0xEB, 0x06, 0x00, 0xA0, 0xE1, 0xE3, 0x2D, 0xFA, 0xEB
	.byte 0x80, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0x68, 0x00, 0x9F, 0xE5, 0x4C, 0x10, 0x9F, 0xE5
	.byte 0x4C, 0x20, 0x9F, 0xE5, 0x54, 0x30, 0x9F, 0xE5, 0x23, 0x1B, 0xFA, 0xEB, 0x0C, 0x00, 0x8D, 0xE2
	.byte 0x06, 0x10, 0xA0, 0xE1, 0x80, 0x20, 0xA0, 0xE3, 0x11, 0x2E, 0xFA, 0xEB, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x0C, 0x00, 0x8D, 0xE2, 0x8B, 0x10, 0xCD, 0xE5, 0x04, 0x50, 0x8D, 0xE5, 0x08, 0x40, 0x8D, 0xE5
	.byte 0x00, 0x00, 0x8D, 0xE5, 0x0C, 0x08, 0x97, 0xE5, 0x2C, 0x10, 0x9F, 0xE5, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x84, 0x74, 0xFE, 0xEB, 0xEC, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0xB8, 0x98, 0x25, 0x02
	.byte 0x18, 0x98, 0x25, 0x02, 0x64, 0x96, 0x25, 0x02, 0x3D, 0x04, 0x00, 0x00, 0x18, 0x9C, 0x25, 0x02
	.byte 0x3F, 0x04, 0x00, 0x00, 0x28, 0x9C, 0x25, 0x02, 0x38, 0x9C, 0x25, 0x02, 0x8C, 0xD2, 0x24, 0x02
	.byte 0xF0, 0x41, 0x2D, 0xE9, 0x01, 0x70, 0xA0, 0xE1, 0x00, 0x80, 0xA0, 0xE1, 0x02, 0x60, 0xA0, 0xE1
	.byte 0x00, 0x10, 0xA0, 0xE3, 0xFF, 0x00, 0x61, 0xE2, 0x00, 0x10, 0xC6, 0xE7, 0x01, 0x10, 0x81, 0xE2
	.byte 0x01, 0x0C, 0x51, 0xE3, 0xFA, 0xFF, 0xFF, 0xBA, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x01, 0xC6, 0xE5
	.byte 0x00, 0x40, 0xA0, 0xE1, 0x00, 0x50, 0xA0, 0xE1, 0x01, 0x01, 0xC6, 0xE5, 0x05, 0x20, 0xD6, 0xE7
	.byte 0x00, 0x30, 0xD8, 0xE7, 0x07, 0x10, 0xA0, 0xE1, 0x01, 0x00, 0x80, 0xE2, 0x02, 0x30, 0x83, 0xE0
	.byte 0x03, 0x30, 0x84, 0xE0, 0xA3, 0x4F, 0xA0, 0xE1, 0x03, 0x3C, 0x64, 0xE0, 0x63, 0x3C, 0x84, 0xE0
	.byte 0xFF, 0x40, 0x03, 0xE2, 0x04, 0x30, 0xD6, 0xE7, 0x05, 0x30, 0xC6, 0xE7, 0x04, 0x20, 0xC6, 0xE7
	.byte 0xA9, 0x52, 0xFA, 0xEB, 0x01, 0x50, 0x85, 0xE2, 0x01, 0x0C, 0x55, 0xE3, 0xFF, 0x00, 0x01, 0xE2
	.byte 0xED, 0xFF, 0xFF, 0xBA, 0xF0, 0x81, 0xBD, 0xE8

	arm_func_start FUN_0224D4D8
FUN_0224D4D8: // 0x0224D4D8
	stmfd sp!, {r4, r5, r6, lr}
	cmp r1, #0
	ldrb r3, [r2, #0x100]
	ldrb ip, [r2, #0x101]
	mov lr, #0
	ble _0224D55C
_0224D4F0:
	add r3, r3, #1
	mov r4, r3, lsr #0x1f
	rsb r3, r4, r3, lsl #24
	add r3, r4, r3, ror #24
	and r3, r3, #0xff
	ldrb r5, [r2, r3]
	add r4, ip, r5
	mov ip, r4, lsr #0x1f
	rsb r4, ip, r4, lsl #24
	add r4, ip, r4, ror #24
	and ip, r4, #0xff
	ldrb r4, [r2, ip]
	strb r4, [r2, r3]
	strb r5, [r2, ip]
	ldrb r4, [r2, r3]
	ldrb r6, [r0, lr]
	add r4, r4, r5
	mov r5, r4, lsr #0x1f
	rsb r4, r5, r4, lsl #24
	add r4, r5, r4, ror #24
	and r4, r4, #0xff
	ldrb r4, [r2, r4]
	eor r4, r6, r4
	strb r4, [r0, lr]
	add lr, lr, #1
	cmp lr, r1
	blt _0224D4F0
_0224D55C:
	strb r3, [r2, #0x100]
	strb ip, [r2, #0x101]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end FUN_0224D4D8
_0224D568:
	.byte 0x10, 0x40, 0x2D, 0xE9, 0x02, 0x40, 0xA0, 0xE1
	.byte 0x00, 0x00, 0x51, 0xE3, 0x00, 0xE0, 0xA0, 0xE3, 0x10, 0x80, 0xBD, 0xD8, 0xDE, 0xC0, 0x90, 0xE1
	.byte 0xD0, 0x30, 0xD4, 0xE1, 0x03, 0x30, 0x2C, 0xE0, 0x0E, 0x30, 0xC0, 0xE7, 0xD1, 0x30, 0xF4, 0xE1
	.byte 0x01, 0xE0, 0x8E, 0xE2, 0x00, 0x00, 0x53, 0xE3, 0x02, 0x40, 0xA0, 0x01, 0x01, 0x00, 0x5E, 0xE1
	.byte 0xF5, 0xFF, 0xFF, 0xBA, 0x10, 0x80, 0xBD, 0xE8, 0xF8, 0x4F, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x01, 0xA0, 0xA0, 0xE1, 0x00, 0x20, 0x8D, 0xE5, 0x04, 0x00, 0x00, 0x1A, 0x50, 0x01, 0x9F, 0xE5
	.byte 0x50, 0x11, 0x9F, 0xE5, 0x50, 0x21, 0x9F, 0xE5, 0x50, 0x31, 0x9F, 0xE5, 0xAA, 0x1A, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x5A, 0xE3, 0x04, 0x00, 0x00, 0xCA, 0x44, 0x01, 0x9F, 0xE5, 0x34, 0x11, 0x9F, 0xE5
	.byte 0x34, 0x21, 0x9F, 0xE5, 0x3C, 0x31, 0x9F, 0xE5, 0xA3, 0x1A, 0xFA, 0xEB, 0x00, 0x00, 0x9D, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x2C, 0x01, 0x9F, 0xE5, 0x14, 0x11, 0x9F, 0xE5
	.byte 0x14, 0x21, 0x9F, 0xE5, 0x24, 0x31, 0x9F, 0xE5, 0x9B, 0x1A, 0xFA, 0xEB, 0x14, 0x88, 0x94, 0xE5
	.byte 0x00, 0x00, 0x58, 0xE3, 0x3C, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x5A, 0xE3, 0x00, 0x60, 0xA0, 0xE3
	.byte 0x36, 0x00, 0x00, 0xDA, 0x00, 0x90, 0x9D, 0xE5, 0x04, 0xB1, 0x9F, 0xE5, 0x01, 0x40, 0xA0, 0xE3
	.byte 0x06, 0x50, 0xA0, 0xE1, 0x00, 0x00, 0x99, 0xE5, 0x05, 0x10, 0xA0, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x01, 0x00, 0x00, 0xBA, 0x11, 0x00, 0x50, 0xE3, 0x04, 0x10, 0xA0, 0xB1, 0x00, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xBC, 0x10, 0x9F, 0xE5, 0xBC, 0x20, 0x9F, 0xE5, 0x0B, 0x00, 0xA0, 0xE1
	.byte 0x59, 0x3F, 0xA0, 0xE3, 0x84, 0x1A, 0xFA, 0xEB, 0x00, 0x10, 0x98, 0xE5, 0x00, 0x00, 0x99, 0xE5
	.byte 0x00, 0x00, 0x51, 0xE1, 0x1D, 0x00, 0x00, 0x1A, 0x81, 0x00, 0x99, 0xE9, 0x00, 0x00, 0x50, 0xE3
	.byte 0x08, 0x10, 0x98, 0x05, 0x00, 0x00, 0x51, 0x03, 0x06, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x50, 0xE3
	.byte 0x08, 0x10, 0x98, 0x15, 0x00, 0x00, 0x51, 0x13, 0x14, 0x00, 0x00, 0x0A, 0xC3, 0x48, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x11, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x57, 0xE3, 0x0C, 0x00, 0x98, 0x05
	.byte 0x00, 0x00, 0x50, 0x03, 0x07, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x57, 0xE3, 0x0C, 0x10, 0x98, 0x15
	.byte 0x00, 0x00, 0x51, 0x13, 0x09, 0x00, 0x00, 0x0A, 0x07, 0x00, 0xA0, 0xE1, 0xB7, 0x48, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A, 0x58, 0x74, 0xFE, 0xEB, 0xA6, 0x0E, 0x80, 0xE2
	.byte 0x0E, 0x1A, 0x80, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0x88, 0xE5, 0xF8, 0x8F, 0xBD, 0xE8
	.byte 0x01, 0x60, 0x86, 0xE2, 0x0A, 0x00, 0x56, 0xE1, 0x0C, 0x90, 0x89, 0xE2, 0xCC, 0xFF, 0xFF, 0xBA
	.byte 0x24, 0x80, 0x98, 0xE5, 0x00, 0x00, 0x58, 0xE3, 0xC2, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0xA0, 0xE3
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xB4, 0x9D, 0x25, 0x02
	.byte 0x5B, 0x01, 0x00, 0x00, 0xE0, 0xA4, 0x25, 0x02, 0x5D, 0x01, 0x00, 0x00, 0xF0, 0xA4, 0x25, 0x02
	.byte 0x5E, 0x01, 0x00, 0x00, 0xF8, 0xA4, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x90, 0x00, 0x9F, 0xE5, 0x90, 0x10, 0x9F, 0xE5
	.byte 0x90, 0x20, 0x9F, 0xE5, 0x90, 0x30, 0x9F, 0xE5, 0x47, 0x1A, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x84, 0x00, 0x9F, 0xE5, 0x74, 0x10, 0x9F, 0xE5, 0x74, 0x20, 0x9F, 0xE5
	.byte 0x7C, 0x30, 0x9F, 0xE5, 0x40, 0x1A, 0xFA, 0xEB, 0xD0, 0x00, 0xD5, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x6C, 0x00, 0x9F, 0xE5, 0x54, 0x10, 0x9F, 0xE5, 0x54, 0x20, 0x9F, 0xE5
	.byte 0x64, 0x30, 0x9F, 0xE5, 0x38, 0x1A, 0xFA, 0xEB, 0x14, 0x48, 0x94, 0xE5, 0x00, 0x00, 0x54, 0xE3
	.byte 0x0C, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x94, 0xE5, 0x0C, 0x00, 0x50, 0xE3, 0x06, 0x00, 0x00, 0x1A
	.byte 0x1C, 0x00, 0x94, 0xE5, 0x05, 0x10, 0xA0, 0xE1, 0x08, 0x00, 0x90, 0xE5, 0x7B, 0x48, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0xA0, 0x01, 0x38, 0x80, 0xBD, 0x08, 0x24, 0x40, 0x94, 0xE5
	.byte 0x00, 0x00, 0x54, 0xE3, 0xF2, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0xA0, 0xE3, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x9C, 0x9F, 0x25, 0x02, 0x7F, 0x01, 0x00, 0x00
	.byte 0x30, 0xA5, 0x25, 0x02, 0x81, 0x01, 0x00, 0x00, 0x38, 0xA5, 0x25, 0x02, 0x82, 0x01, 0x00, 0x00

	arm_func_start FUN_0224D800
FUN_0224D800: // 0x0224D800
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _0224D820
	ldr r0, _0224D844 // =0x0225A544
	ldr r1, _0224D848 // =0x0225A4D0
	ldr r2, _0224D84C // =0x02259EC4
	ldr r3, _0224D850 // =0x00000193
	bl __msl_assertion_failed
_0224D820:
	ldr r0, [r4, #0x1c]
	bl DWCi_GsFree
	ldr r0, [r4, #8]
	bl DWCi_GsFree
	ldr r0, [r4, #0xc]
	bl DWCi_GsFree
	mov r0, r4
	bl DWCi_GsFree
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224D844: .word 0x0225A544
_0224D848: .word 0x0225A4D0
_0224D84C: .word 0x02259EC4
_0224D850: .word 0x00000193
	arm_func_end FUN_0224D800

	arm_func_start FUN_0224D854
FUN_0224D854: // 0x0224D854
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	mov r5, r1
	mov r4, #0
	bne _0224D87C
	ldr r0, _0224D8F0 // =0x0225A4C0
	ldr r1, _0224D8F4 // =0x0225A4D0
	ldr r2, _0224D8F8 // =0x02259E74
	ldr r3, _0224D8FC // =0x0000019F
	bl __msl_assertion_failed
_0224D87C:
	cmp r5, #0
	bne _0224D898
	ldr r0, _0224D900 // =0x0225A544
	ldr r1, _0224D8F4 // =0x0225A4D0
	ldr r2, _0224D8F8 // =0x02259E74
	ldr r3, _0224D904 // =0x000001A1
	bl __msl_assertion_failed
_0224D898:
	ldr r1, [r6, #0x814]
	mov r0, r1
	cmp r1, #0
	ldmeqia sp!, {r4, r5, r6, pc}
_0224D8A8:
	cmp r0, r5
	bne _0224D8DC
	cmp r1, r0
	ldreq r1, [r0, #0x24]
	streq r1, [r6, #0x814]
	ldr r1, [r6, #0x818]
	cmp r1, r0
	streq r4, [r6, #0x818]
	cmp r4, #0
	ldrne r1, [r0, #0x24]
	strne r1, [r4, #0x24]
	bl FUN_0224D800
	ldmia sp!, {r4, r5, r6, pc}
_0224D8DC:
	mov r4, r0
	ldr r0, [r0, #0x24]
	cmp r0, #0
	bne _0224D8A8
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_0224D8F0: .word 0x0225A4C0
_0224D8F4: .word 0x0225A4D0
_0224D8F8: .word 0x02259E74
_0224D8FC: .word 0x0000019F
_0224D900: .word 0x0225A544
_0224D904: .word 0x000001A1
	arm_func_end FUN_0224D854

	arm_func_start FUN_0224D908
FUN_0224D908: // 0x0224D908
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	movs r5, r1
	mov r6, r0
	mov r4, r2
	bne _0224D934
	ldr r0, _0224E02C // =0x0225A554
	ldr r1, _0224E030 // =0x0225A4D0
	ldr r2, _0224E034 // =0x02259E84
	ldr r3, _0224E038 // =0x000001C1
	bl __msl_assertion_failed
_0224D934:
	ldr r0, [r5]
	mov r1, #0
	cmp r0, #0
	blt _0224D94C
	cmp r0, #0x11
	movlt r1, #1
_0224D94C:
	cmp r1, #0
	bne _0224D968
	ldr r0, _0224E03C // =0x0225A55C
	ldr r1, _0224E030 // =0x0225A4D0
	ldr r2, _0224E034 // =0x02259E84
	ldr r3, _0224E040 // =0x000001C2
	bl __msl_assertion_failed
_0224D968:
	ldr r0, [r5]
	cmp r0, #0
	bne _0224DA08
	ldr r2, [r5, #0x14]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224D9B4
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x14
	mov r1, #0xe
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224D9B4:
	ldr r0, [r7, #4]
	mov r4, #0
	cmp r0, #0
	ble _0224D9EC
_0224D9C4:
	ldr r0, [r7, #8]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7, #4]
	add r4, r4, #1
	cmp r4, r0
	blt _0224D9C4
_0224D9EC:
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	ldr r0, [r7, #0x10]
	bl DWCi_GsFree
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	b _0224E018
_0224DA08:
	cmp r0, #1
	bne _0224DA50
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0xc
	mov r1, #0xf
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DA50:
	cmp r0, #2
	bne _0224DA94
	ldr r2, [r5, #0x10]
	ldr r7, [r4, #4]
	cmp r2, #0
	beq _0224E018
	ldr r1, [r5, #0x18]
	mov r3, r4
	str r1, [sp]
	ldr r1, [r5, #0x20]
	mov r0, r6
	stmib sp, {r1, r7}
	mov r4, #0xc
	mov r1, #0x10
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DA94:
	cmp r0, #3
	bne _0224DB1C
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DADC
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x14
	mov r1, #0x13
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DADC:
	ldr r0, [r7, #4]
	mov r4, #0
	cmp r0, #0
	ble _0224DB08
_0224DAEC:
	ldr r0, [r7, #8]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7, #4]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DAEC
_0224DB08:
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	b _0224E018
_0224DB1C:
	cmp r0, #4
	bne _0224DBB4
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DB64
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x1c
	mov r1, #0x14
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DB64:
	ldr r0, [r7, #0xc]
	mov r4, #0
	cmp r0, #0
	ble _0224DB90
_0224DB74:
	ldr r0, [r7, #0x10]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7, #0xc]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DB74
_0224DB90:
	ldr r0, [r7, #0x10]
	bl DWCi_GsFree
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	ldr r0, [r7]
	bl DWCi_GsFree
	b _0224E018
_0224DBB4:
	cmp r0, #0xa
	bne _0224DBFC
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x10
	mov r1, #0x15
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DBFC:
	cmp r0, #0xb
	bne _0224DC44
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x10
	mov r1, #0x16
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DC44:
	cmp r0, #5
	bne _0224DC8C
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0xc
	mov r1, #0x11
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DC8C:
	cmp r0, #6
	bne _0224DCD4
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x10
	mov r1, #0x17
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DCD4:
	cmp r0, #7
	bne _0224DCEC
	ldr r0, [r5, #0x1c]
	ldr r0, [r0]
	bl DWCi_GsFree
	b _0224E018
_0224DCEC:
	cmp r0, #8
	bne _0224DD64
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DD34
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x10
	mov r1, #0x18
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DD34:
	ldr r0, [r7]
	mov r4, #0
	cmp r0, #0
	ble _0224E018
_0224DD44:
	ldr r0, [r7, #4]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DD44
	b _0224E018
_0224DD64:
	cmp r0, #9
	bne _0224DDAC
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0xc
	mov r1, #0x1a
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224DDAC:
	cmp r0, #0xc
	bne _0224DE34
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DDF4
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x14
	mov r1, #0x1d
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DDF4:
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	ldr r0, [r7]
	mov r4, #0
	cmp r0, #0
	ble _0224DE28
_0224DE0C:
	ldr r0, [r7, #4]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DE0C
_0224DE28:
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	b _0224E018
_0224DE34:
	cmp r0, #0xd
	bne _0224DEB4
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DE7C
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x18
	mov r1, #0x1e
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DE7C:
	ldr r0, [r7]
	mov r4, #0
	cmp r0, #0
	ble _0224DEA8
_0224DE8C:
	ldr r0, [r7, #4]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DE8C
_0224DEA8:
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	b _0224E018
_0224DEB4:
	cmp r0, #0xe
	bne _0224DF34
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DEFC
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x18
	mov r1, #0x1e
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DEFC:
	ldr r0, [r7]
	mov r4, #0
	cmp r0, #0
	ble _0224DF28
_0224DF0C:
	ldr r0, [r7, #4]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DF0C
_0224DF28:
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	b _0224E018
_0224DF34:
	cmp r0, #0xf
	bne _0224DFBC
	ldr r2, [r5, #0x10]
	ldr r7, [r5, #0x1c]
	cmp r2, #0
	beq _0224DF7C
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #0x10
	mov r1, #0x1b
	str r4, [sp, #0xc]
	bl FUN_02248C2C
_0224DF7C:
	ldr r0, [r7, #4]
	mov r4, #0
	cmp r0, #0
	ble _0224DFA8
_0224DF8C:
	ldr r0, [r7, #8]
	ldr r0, [r0, r4, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r7, #4]
	add r4, r4, #1
	cmp r4, r0
	blt _0224DF8C
_0224DFA8:
	ldr r0, [r7, #8]
	bl DWCi_GsFree
	ldr r0, [r7, #0xc]
	bl DWCi_GsFree
	b _0224E018
_0224DFBC:
	cmp r0, #0x10
	bne _0224E004
	ldr r2, [r5, #0x10]
	cmp r2, #0
	beq _0224E018
	ldr r0, [r5, #0x18]
	mov r1, #0
	str r0, [sp]
	ldr r3, [r5, #0x20]
	mov r0, r6
	str r3, [sp, #4]
	mov r3, r4
	str r1, [sp, #8]
	mov r4, #8
	mov r1, #0x1f
	str r4, [sp, #0xc]
	bl FUN_02248C2C
	b _0224E018
_0224E004:
	ldr r0, _0224E044 // =0x0225A590
	ldr r1, _0224E030 // =0x0225A4D0
	ldr r2, _0224E034 // =0x02259E84
	mov r3, #0x25c
	bl __msl_assertion_failed
_0224E018:
	mov r0, r6
	mov r1, r5
	bl FUN_0224D854
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_0224E02C: .word 0x0225A554
_0224E030: .word 0x0225A4D0
_0224E034: .word 0x02259E84
_0224E038: .word 0x000001C1
_0224E03C: .word 0x0225A55C
_0224E040: .word 0x000001C2
_0224E044: .word 0x0225A590
	arm_func_end FUN_0224D908

	arm_func_start FUN_0224E048
FUN_0224E048: // 0x0224E048
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x104
	movs r4, r1
	mov r5, r0
	bne _0224E070
	ldr r0, _0224E454 // =0x0225A554
	ldr r1, _0224E458 // =0x0225A4D0
	ldr r2, _0224E45C // =0x02259F10
	mov r3, #0x268
	bl __msl_assertion_failed
_0224E070:
	ldr r0, [r4]
	mov r1, #0
	cmp r0, #0
	blt _0224E088
	cmp r0, #0x11
	movlt r1, #1
_0224E088:
	cmp r1, #0
	bne _0224E0A4
	ldr r0, _0224E460 // =0x0225A55C
	ldr r1, _0224E458 // =0x0225A4D0
	ldr r2, _0224E45C // =0x02259F10
	ldr r3, _0224E464 // =0x00000269
	bl __msl_assertion_failed
_0224E0A4:
	ldr r0, [r4]
	cmp r0, #0
	bne _0224E0E0
	mov r3, #0
	add r2, sp, #0xf0
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0xf0]
	str r3, [sp, #0xf4]
	str r3, [sp, #0xf8]
	str r3, [sp, #0xfc]
	str r3, [sp, #0x100]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E0E0:
	cmp r0, #1
	bne _0224E118
	mov r1, #0
	mov r0, #7
	str r1, [sp, #0xe4]
	str r0, [sp, #0xe8]
	ldr r3, [r4, #8]
	add r2, sp, #0xe4
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0xec]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E118:
	cmp r0, #2
	bne _0224E14C
	mov ip, #0
	str ip, [sp, #0xd8]
	ldr r3, [r4, #8]
	add r2, sp, #0xd8
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0xdc]
	str ip, [sp, #0xe0]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E14C:
	cmp r0, #3
	bne _0224E188
	mov ip, #0
	str ip, [sp, #0xc4]
	ldr r3, [r4, #8]
	add r2, sp, #0xc4
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0xc8]
	str ip, [sp, #0xcc]
	str ip, [sp, #0xd0]
	str ip, [sp, #0xd4]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E188:
	cmp r0, #4
	bne _0224E1CC
	mov ip, #0
	str ip, [sp, #0xa8]
	ldr r3, [r4, #8]
	add r2, sp, #0xa8
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0xac]
	str ip, [sp, #0xb0]
	str ip, [sp, #0xb4]
	str ip, [sp, #0xb8]
	str ip, [sp, #0xbc]
	str ip, [sp, #0xc0]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E1CC:
	cmp r0, #0xa
	bne _0224E204
	mov ip, #0
	str ip, [sp, #0x98]
	ldr r3, [r4, #8]
	add r2, sp, #0x98
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x9c]
	str ip, [sp, #0xa0]
	str ip, [sp, #0xa4]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E204:
	cmp r0, #0xb
	bne _0224E240
	mov ip, #0
	str ip, [sp, #0x84]
	ldr r3, [r4, #8]
	add r2, sp, #0x84
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x88]
	str ip, [sp, #0x8c]
	str ip, [sp, #0x90]
	str ip, [sp, #0x94]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E240:
	cmp r0, #5
	bne _0224E274
	mov ip, #0
	str ip, [sp, #0x78]
	ldr r3, [r4, #8]
	add r2, sp, #0x78
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x7c]
	str ip, [sp, #0x80]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E274:
	cmp r0, #6
	bne _0224E2B0
	mov ip, #0
	str ip, [sp, #0x68]
	ldr r0, [r4, #0xc]
	add r2, sp, #0x68
	str r0, [sp, #0x6c]
	ldr r3, [r4, #8]
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x70]
	str ip, [sp, #0x74]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E2B0:
	cmp r0, #7
	bne _0224E2D0
	mov r0, r5
	mov r1, r4
	mov r2, #0
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E2D0:
	cmp r0, #8
	bne _0224E304
	ldr ip, [r4, #8]
	mov r3, #0
	add r2, sp, #0x58
	mov r0, r5
	mov r1, r4
	str ip, [sp, #0x5c]
	str r3, [sp, #0x60]
	str r3, [sp, #0x64]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E304:
	cmp r0, #9
	bne _0224E33C
	mov r0, #0
	str r0, [sp, #0x4c]
	ldr r0, [r4, #8]
	add r2, sp, #0x4c
	str r0, [sp, #0x50]
	ldr r3, [r4, #0xc]
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x54]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E33C:
	cmp r0, #0xc
	bne _0224E374
	mov r3, #0
	add r2, sp, #0x38
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x38]
	str r3, [sp, #0x3c]
	str r3, [sp, #0x40]
	str r3, [sp, #0x44]
	str r3, [sp, #0x48]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E374:
	cmp r0, #0xd
	bne _0224E3B0
	mov r3, #0
	add r2, sp, #0x20
	mov r0, r5
	mov r1, r4
	str r3, [sp, #0x20]
	str r3, [sp, #0x24]
	str r3, [sp, #0x28]
	str r3, [sp, #0x2c]
	str r3, [sp, #0x30]
	str r3, [sp, #0x34]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E3B0:
	cmp r0, #0xe
	bne _0224E3EC
	mov r3, #0
	add r2, sp, #8
	mov r0, r5
	mov r1, r4
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	str r3, [sp, #0x10]
	str r3, [sp, #0x14]
	str r3, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E3EC:
	cmp r0, #0xf
	bne _0224E408
	mov r0, r5
	mov r1, r4
	bl FUN_0224D854
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E408:
	cmp r0, #0x10
	bne _0224E438
	ldr r3, _0224E468 // =0x0225A594
	mov ip, #0
	add r2, sp, #0
	mov r0, r5
	mov r1, r4
	str ip, [sp]
	str r3, [sp, #4]
	bl FUN_0224D908
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
_0224E438:
	ldr r0, _0224E46C // =0x0225A590
	ldr r1, _0224E458 // =0x0225A4D0
	ldr r2, _0224E45C // =0x02259F10
	ldr r3, _0224E470 // =0x00000311
	bl __msl_assertion_failed
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_0224E454: .word 0x0225A554
_0224E458: .word 0x0225A4D0
_0224E45C: .word 0x02259F10
_0224E460: .word 0x0225A55C
_0224E464: .word 0x00000269
_0224E468: .word 0x0225A594
_0224E46C: .word 0x0225A590
_0224E470: .word 0x00000311
	arm_func_end FUN_0224E048

	arm_func_start FUN_0224E474
FUN_0224E474: // 0x0224E474
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	bne _0224E494
	ldr r0, _0224E4D0 // =0x0225A4C0
	ldr r1, _0224E4D4 // =0x0225A4D0
	ldr r2, _0224E4D8 // =0x02259DC4
	ldr r3, _0224E4DC // =0x0000031A
	bl __msl_assertion_failed
_0224E494:
	bl current_time
	ldr r1, [r6, #0x814]
	mov r5, r0
	cmp r1, #0
	ldmeqia sp!, {r4, r5, r6, pc}
_0224E4A8:
	ldr r0, [r1, #4]
	ldr r4, [r1, #0x24]
	cmp r5, r0
	bls _0224E4C0
	mov r0, r6
	bl FUN_0224E048
_0224E4C0:
	mov r1, r4
	cmp r4, #0
	bne _0224E4A8
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_0224E4D0: .word 0x0225A4C0
_0224E4D4: .word 0x0225A4D0
_0224E4D8: .word 0x02259DC4
_0224E4DC: .word 0x0000031A
	arm_func_end FUN_0224E474

	arm_func_start FUN_0224E4E0
FUN_0224E4E0: // 0x0224E4E0
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _0224E500
	ldr r0, _0224E51C // =0x0225A4C0
	ldr r1, _0224E520 // =0x0225A4D0
	ldr r2, _0224E524 // =0x02259D60
	ldr r3, _0224E528 // =0x0000032D
	bl __msl_assertion_failed
_0224E500:
	ldr r0, [r4, #0x81c]
	mvn r1, #0x80000000
	cmp r0, r1
	moveq r1, #1
	addne r1, r0, #1
	str r1, [r4, #0x81c]
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224E51C: .word 0x0225A4C0
_0224E520: .word 0x0225A4D0
_0224E524: .word 0x02259D60
_0224E528: .word 0x0000032D
	arm_func_end FUN_0224E4E0

	arm_func_start FUN_0224E52C
FUN_0224E52C: // 0x0224E52C
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0224E550
	ldr r0, _0224E59C // =0x0225A4C0
	ldr r1, _0224E5A0 // =0x0225A4D0
	ldr r2, _0224E5A4 // =0x02259FB0
	mov r3, #0x340
	bl __msl_assertion_failed
_0224E550:
	cmp r4, #0
	bgt _0224E56C
	ldr r0, _0224E5A8 // =0x0225A5A0
	ldr r1, _0224E5A0 // =0x0225A4D0
	ldr r2, _0224E5A4 // =0x02259FB0
	ldr r3, _0224E5AC // =0x00000342
	bl __msl_assertion_failed
_0224E56C:
	ldr r1, [r5, #0x814]
	cmp r1, #0
	beq _0224E594
_0224E578:
	ldr r0, [r1, #0x20]
	cmp r0, r4
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r1, [r1, #0x24]
	cmp r1, #0
	bne _0224E578
_0224E594:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0224E59C: .word 0x0225A4C0
_0224E5A0: .word 0x0225A4D0
_0224E5A4: .word 0x02259FB0
_0224E5A8: .word 0x0225A5A0
_0224E5AC: .word 0x00000342
	arm_func_end FUN_0224E52C

	arm_func_start FUN_0224E5B0
FUN_0224E5B0: // 0x0224E5B0
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _0224E5D0
	ldr r0, _0224E5F4 // =0x0225A4C0
	ldr r1, _0224E5F8 // =0x0225A4D0
	ldr r2, _0224E5FC // =0x02259ED4
	ldr r3, _0224E600 // =0x00000353
	bl __msl_assertion_failed
_0224E5D0:
	ldr r1, [r4, #0x814]
	cmp r1, #0
	ldmeqia sp!, {r4, pc}
_0224E5DC:
	mov r0, r4
	bl FUN_0224E048
	ldr r1, [r4, #0x814]
	cmp r1, #0
	bne _0224E5DC
	ldmia sp!, {r4, pc}
	.align 2, 0
_0224E5F4: .word 0x0225A4C0
_0224E5F8: .word 0x0225A4D0
_0224E5FC: .word 0x02259ED4
_0224E600: .word 0x00000353
	arm_func_end FUN_0224E5B0

	arm_func_start FUN_0224E604
FUN_0224E604: // 0x0224E604
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	movs r7, r0
	mov r8, r1
	mov r6, r2
	mov r5, r3
	bne _0224E630
	ldr r0, _0224E6E0 // =0x0225A4C0
	ldr r1, _0224E6E4 // =0x0225A4D0
	ldr r2, _0224E6E8 // =0x02259D6C
	mov r3, #0x35c
	bl __msl_assertion_failed
_0224E630:
	mov r0, #0x28
	bl DWCi_GsMalloc
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r1, #0
	mov r2, #0x28
	bl memset
	str r8, [r4]
	bl current_time
	add r0, r0, #0xa60
	add r0, r0, #0xe000
	str r0, [r4, #4]
	ldr r1, [sp, #0x18]
	ldr r0, [sp, #0x1c]
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
	ldr r1, [sp, #0x20]
	ldr r0, [sp, #0x24]
	str r1, [r4, #0x18]
	str r0, [r4, #0x1c]
	cmp r6, #0
	moveq r0, #0
	beq _0224E698
	mov r0, r6
	bl goastrdup
_0224E698:
	str r0, [r4, #8]
	cmp r5, #0
	moveq r0, #0
	beq _0224E6B0
	mov r0, r5
	bl goastrdup
_0224E6B0:
	str r0, [r4, #0xc]
	mov r0, r7
	bl FUN_0224E4E0
	str r0, [r4, #0x20]
	ldr r0, [r7, #0x814]
	cmp r0, #0
	streq r4, [r7, #0x814]
	ldrne r0, [r7, #0x818]
	strne r4, [r0, #0x24]
	str r4, [r7, #0x818]
	ldr r0, [r4, #0x20]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0224E6E0: .word 0x0225A4C0
_0224E6E4: .word 0x0225A4D0
_0224E6E8: .word 0x02259D6C
	arm_func_end FUN_0224E604

	arm_func_start FUN_0224E6EC
FUN_0224E6EC: // 0x0224E6EC
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	mov r7, r0
	mov r0, #0x14
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl DWCi_GsMalloc
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r3, r0
	mov r2, #5
	mov r1, #0
_0224E728:
	strb r1, [r3]
	strb r1, [r3, #1]
	strb r1, [r3, #2]
	strb r1, [r3, #3]
	add r3, r3, #4
	subs r2, r2, #1
	bne _0224E728
	str r6, [sp]
	str r5, [sp, #4]
	mov r1, #0
	str r4, [sp, #8]
	str r0, [sp, #0xc]
	mov r0, r7
	mov r2, r1
	mov r3, r1
	bl FUN_0224E604
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_0224E6EC

	arm_func_start FUN_0224E770
FUN_0224E770: // 0x0224E770
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r8, [sp, #0x2c]
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	cmp r8, #0
	bne _0224E7A8
	ldr r0, _0224E85C // =0x0225A5A8
	ldr r1, _0224E860 // =0x0225A4D0
	ldr r2, _0224E864 // =0x02259E94
	ldr r3, _0224E868 // =0x00000391
	bl __msl_assertion_failed
_0224E7A8:
	ldr r0, [sp, #0x2c]
	bl strlen
	cmp r0, #0x20
	blo _0224E7CC
	ldr r0, _0224E86C // =0x0225A5BC
	ldr r1, _0224E860 // =0x0225A4D0
	ldr r2, _0224E864 // =0x02259E94
	ldr r3, _0224E870 // =0x00000392
	bl __msl_assertion_failed
_0224E7CC:
	mov r0, #0x54
	bl DWCi_GsMalloc
	movs r8, r0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r1, #0
	mov r2, #0x54
	bl memset
	ldr lr, [sp, #0x28]
	mov ip, r8
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	ldr r1, [sp, #0x2c]
	add r0, r8, #0x34
	mov r2, #0x20
	bl strncpy
	mov r3, #0
	strb r3, [r8, #0x53]
	str r5, [sp]
	mov r0, r7
	mov r2, r6
	mov r1, #1
	stmib sp, {r3, r4, r8}
	bl FUN_0224E604
	movs r4, r0
	bne _0224E850
	mov r0, r8
	bl DWCi_GsFree
_0224E850:
	mov r0, r4
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0224E85C: .word 0x0225A5A8
_0224E860: .word 0x0225A4D0
_0224E864: .word 0x02259E94
_0224E868: .word 0x00000391
_0224E86C: .word 0x0225A5BC
_0224E870: .word 0x00000392
	arm_func_end FUN_0224E770

	arm_func_start FUN_0224E874
FUN_0224E874: // 0x0224E874
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	mov r7, r0
	mov r0, #0x10
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl DWCi_GsMalloc
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r3, r0
	mov r2, #4
	mov r1, #0
_0224E8B0:
	strb r1, [r3]
	strb r1, [r3, #1]
	strb r1, [r3, #2]
	strb r1, [r3, #3]
	add r3, r3, #4
	subs r2, r2, #1
	bne _0224E8B0
	str r5, [sp]
	mov r3, #0
	stmib sp, {r3, r4}
	str r0, [sp, #0xc]
	mov r0, r7
	mov r2, r6
	mov r1, #3
	bl FUN_0224E604
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FUN_0224E874

	arm_func_start FUN_0224E8F4
FUN_0224E8F4: // 0x0224E8F4
	stmfd sp!, {r3, lr}
	sub sp, sp, #0x10
	str r2, [sp]
	mov ip, #0
	str ip, [sp, #4]
	str r3, [sp, #8]
	mov r2, r1
	mov r3, ip
	mov r1, #5
	str ip, [sp, #0xc]
	bl FUN_0224E604
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	arm_func_end FUN_0224E8F4

	arm_func_start FUN_0224E928
FUN_0224E928: // 0x0224E928
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	mov fp, r0
	mov r0, #0x10
	str r1, [sp, #0x10]
	mov sl, r2
	mov sb, r3
	bl DWCi_GsMalloc
	movs r7, r0
	addeq sp, sp, #0x14
	mov r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r2, r7
	mov r1, #4
_0224E960:
	strb r0, [r2]
	strb r0, [r2, #1]
	strb r0, [r2, #2]
	strb r0, [r2, #3]
	add r2, r2, #4
	subs r1, r1, #1
	bne _0224E960
	ldr r1, [sp, #0x3c]
	ldr r0, [sp, #0x38]
	str r1, [r7, #0xc]
	str sl, [r7]
	str r0, [r7, #8]
	cmp r1, #0
	ldrne r0, [r7]
	subne r0, r0, #1
	strne r0, [r7]
	ldr r0, [r7]
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #4]
	cmp r0, #0
	bne _0224E9CC
	mov r0, r7
	bl DWCi_GsFree
	add sp, sp, #0x14
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224E9CC:
	mov r5, #0
	mov r6, r5
	cmp sl, #0
	ble _0224EA60
	ldr r4, _0224EA98 // =0x0225A5DC
_0224E9E0:
	ldr r8, [sb, r5, lsl #2]
	mov r1, r4
	mov r0, r8
	bl strcmp
	cmp r0, #0
	beq _0224EA54
	mov r0, r8
	bl goastrdup
	ldr r1, [r7, #4]
	str r0, [r1, r6, lsl #2]
	ldr r0, [r7, #4]
	ldr r0, [r0, r6, lsl #2]
	cmp r0, #0
	bne _0224EA50
	subs r6, r6, #1
	bmi _0224EA34
_0224EA20:
	ldr r0, [r7, #4]
	ldr r0, [r0, r6, lsl #2]
	bl DWCi_GsFree
	subs r6, r6, #1
	bpl _0224EA20
_0224EA34:
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	add sp, sp, #0x14
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224EA50:
	add r6, r6, #1
_0224EA54:
	add r5, r5, #1
	cmp r5, sl
	blt _0224E9E0
_0224EA60:
	ldr r0, [sp, #0x40]
	str r6, [r7]
	str r0, [sp]
	mov r3, #0
	ldr r0, [sp, #0x44]
	str r3, [sp, #4]
	str r0, [sp, #8]
	ldr r2, [sp, #0x10]
	mov r0, fp
	mov r1, #0xd
	str r7, [sp, #0xc]
	bl FUN_0224E604
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0224EA98: .word 0x0225A5DC
	arm_func_end FUN_0224E928

	arm_func_start FUN_0224EA9C
FUN_0224EA9C: // 0x0224EA9C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	mov fp, r0
	mov r0, #0xc
	str r1, [sp, #0x10]
	mov sl, r2
	mov sb, r3
	bl DWCi_GsMalloc
	movs r7, r0
	addeq sp, sp, #0x14
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, #0
	strb r1, [r7]
	strb r1, [r7, #1]
	strb r1, [r7, #2]
	strb r1, [r7, #3]
	strb r1, [r7, #4]
	strb r1, [r7, #5]
	strb r1, [r7, #6]
	strb r1, [r7, #7]
	strb r1, [r7, #8]
	strb r1, [r7, #9]
	strb r1, [r7, #0xa]
	ldr r0, [sp, #0x38]
	strb r1, [r7, #0xb]
	str r0, [r7, #8]
	str sl, [r7]
	cmp r0, #0
	ldrne r0, [r7]
	subne r0, r0, #1
	strne r0, [r7]
	ldr r0, [r7]
	cmp r0, #0
	beq _0224EBE8
	mov r0, r0, lsl #2
	bl DWCi_GsMalloc
	str r0, [r7, #4]
	cmp r0, #0
	bne _0224EB50
	mov r0, r7
	bl DWCi_GsFree
	add sp, sp, #0x14
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224EB50:
	mov r5, #0
	mov r6, r5
	cmp sl, #0
	ble _0224EBE4
	ldr r4, _0224EC1C // =0x0225A5DC
_0224EB64:
	ldr r8, [sb, r5, lsl #2]
	mov r1, r4
	mov r0, r8
	bl strcmp
	cmp r0, #0
	beq _0224EBD8
	mov r0, r8
	bl goastrdup
	ldr r1, [r7, #4]
	str r0, [r1, r6, lsl #2]
	ldr r0, [r7, #4]
	ldr r0, [r0, r6, lsl #2]
	cmp r0, #0
	bne _0224EBD4
	subs r6, r6, #1
	bmi _0224EBB8
_0224EBA4:
	ldr r0, [r7, #4]
	ldr r0, [r0, r6, lsl #2]
	bl DWCi_GsFree
	subs r6, r6, #1
	bpl _0224EBA4
_0224EBB8:
	ldr r0, [r7, #4]
	bl DWCi_GsFree
	mov r0, r7
	bl DWCi_GsFree
	add sp, sp, #0x14
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0224EBD4:
	add r6, r6, #1
_0224EBD8:
	add r5, r5, #1
	cmp r5, sl
	blt _0224EB64
_0224EBE4:
	str r6, [r7]
_0224EBE8:
	ldr r1, [sp, #0x3c]
	ldr r0, [sp, #0x40]
	str r1, [sp]
	mov r3, #0
	str r3, [sp, #4]
	str r0, [sp, #8]
	ldr r2, [sp, #0x10]
	mov r0, fp
	mov r1, #0xe
	str r7, [sp, #0xc]
	bl FUN_0224E604
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0224EC1C: .word 0x0225A5DC
	arm_func_end FUN_0224EA9C
_0224EC20:
	.byte 0xF8, 0x4F, 0x2D, 0xE9, 0x08, 0xD0, 0x4D, 0xE2, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x8D, 0xE5
	.byte 0x00, 0xB0, 0xB0, 0xE1, 0x00, 0x10, 0x8D, 0xE5, 0x02, 0x90, 0xA0, 0xE1, 0x03, 0x80, 0xA0, 0xE1
	.byte 0x03, 0x70, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x74, 0x02, 0x9F, 0xE5, 0x74, 0x12, 0x9F, 0xE5
	.byte 0x74, 0x22, 0x9F, 0xE5, 0x74, 0x32, 0x9F, 0xE5, 0x07, 0x15, 0xFA, 0xEB, 0xD0, 0x00, 0xDB, 0xE1
	.byte 0x2B, 0x00, 0x50, 0xE3, 0x01, 0xA0, 0xA0, 0x03, 0x05, 0x00, 0x00, 0x0A, 0x2D, 0x00, 0x50, 0xE3
	.byte 0x00, 0xA0, 0xA0, 0x03, 0x02, 0x00, 0x00, 0x0A, 0x08, 0xD0, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0x01, 0xB0, 0x8B, 0xE2, 0x00, 0x40, 0xE0, 0xE3, 0xD1, 0x50, 0xDB, 0xE0
	.byte 0x2D, 0x00, 0x55, 0xE3, 0x08, 0x00, 0x00, 0xCA, 0x2B, 0x00, 0x55, 0xE3, 0x03, 0x00, 0x00, 0xBA
	.byte 0x20, 0x00, 0x00, 0x0A, 0x2D, 0x00, 0x55, 0xE3, 0x21, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0xEA
	.byte 0x00, 0x00, 0x55, 0xE3, 0x21, 0x00, 0x00, 0x0A, 0x4D, 0x00, 0x00, 0xEA, 0x62, 0x00, 0x55, 0xE3
	.byte 0x01, 0x00, 0x00, 0xCA, 0x3E, 0x00, 0x00, 0x0A, 0x49, 0x00, 0x00, 0xEA, 0x65, 0x00, 0x45, 0xE2
	.byte 0x12, 0x00, 0x50, 0xE3, 0x00, 0xF1, 0x8F, 0x90, 0x45, 0x00, 0x00, 0xEA, 0x41, 0x00, 0x00, 0xEA
	.byte 0x43, 0x00, 0x00, 0xEA, 0x42, 0x00, 0x00, 0xEA, 0x41, 0x00, 0x00, 0xEA, 0x16, 0x00, 0x00, 0xEA
	.byte 0x3F, 0x00, 0x00, 0xEA, 0x20, 0x00, 0x00, 0xEA, 0x16, 0x00, 0x00, 0xEA, 0x21, 0x00, 0x00, 0xEA
	.byte 0x23, 0x00, 0x00, 0xEA, 0x28, 0x00, 0x00, 0xEA, 0x15, 0x00, 0x00, 0xEA, 0x38, 0x00, 0x00, 0xEA
	.byte 0x37, 0x00, 0x00, 0xEA, 0x15, 0x00, 0x00, 0xEA, 0x20, 0x00, 0x00, 0xEA, 0x2B, 0x00, 0x00, 0xEA
	.byte 0x24, 0x00, 0x00, 0xEA, 0x2C, 0x00, 0x00, 0xEA, 0x01, 0xA0, 0xA0, 0xE3, 0x04, 0x60, 0xA0, 0xE1
	.byte 0x30, 0x00, 0x00, 0xEA, 0x00, 0xA0, 0xA0, 0xE3, 0x04, 0x60, 0xA0, 0xE1, 0x2D, 0x00, 0x00, 0xEA
	.byte 0x00, 0x60, 0xA0, 0xE3, 0x06, 0x70, 0xA0, 0xE1, 0x2A, 0x00, 0x00, 0xEA, 0x02, 0x60, 0xA0, 0xE3
	.byte 0x00, 0x70, 0xA0, 0xE3, 0x27, 0x00, 0x00, 0xEA, 0x03, 0x60, 0xA0, 0xE3, 0x01, 0x70, 0xA0, 0xE3
	.byte 0x24, 0x00, 0x00, 0xEA, 0x04, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x21, 0x00, 0x00, 0xEA
	.byte 0x05, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x1E, 0x00, 0x00, 0xEA, 0x06, 0x60, 0xA0, 0xE3
	.byte 0x01, 0x70, 0xA0, 0xE3, 0x1B, 0x00, 0x00, 0xEA, 0x07, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3
	.byte 0x18, 0x00, 0x00, 0xEA, 0x08, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x15, 0x00, 0x00, 0xEA
	.byte 0x09, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x12, 0x00, 0x00, 0xEA, 0x0A, 0x60, 0xA0, 0xE3
	.byte 0x01, 0x70, 0xA0, 0xE3, 0x0F, 0x00, 0x00, 0xEA, 0x0B, 0x60, 0xA0, 0xE3, 0x01, 0x70, 0xA0, 0xE3
	.byte 0x0C, 0x00, 0x00, 0xEA, 0x01, 0x60, 0xA0, 0xE3, 0x06, 0x70, 0xA0, 0xE1, 0x09, 0x00, 0x00, 0xEA
	.byte 0x0C, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x06, 0x00, 0x00, 0xEA, 0x0D, 0x60, 0xA0, 0xE3
	.byte 0x00, 0x70, 0xA0, 0xE3, 0x03, 0x00, 0x00, 0xEA, 0x0E, 0x60, 0xA0, 0xE3, 0x00, 0x70, 0xA0, 0xE3
	.byte 0x00, 0x00, 0x00, 0xEA, 0x04, 0x60, 0xA0, 0xE1, 0x04, 0x00, 0x56, 0xE1, 0x2B, 0x00, 0x00, 0x0A
	.byte 0x01, 0x20, 0x88, 0xE2, 0x0C, 0x10, 0xA0, 0xE3, 0x92, 0x01, 0x01, 0xE0, 0x04, 0x00, 0x9D, 0xE5
	.byte 0x9F, 0x22, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x04, 0x00, 0x9D, 0xE5
	.byte 0xA2, 0x22, 0xFE, 0xEB, 0x08, 0xD0, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3, 0xF8, 0x8F, 0xBD, 0xE8
	.byte 0x0C, 0x10, 0xA0, 0xE3, 0x98, 0x01, 0x01, 0xE0, 0x00, 0x20, 0xA0, 0xE3, 0x01, 0x20, 0xC0, 0xE7
	.byte 0x01, 0x20, 0x80, 0xE0, 0x00, 0x30, 0xA0, 0xE3, 0x01, 0x30, 0xC2, 0xE5, 0x02, 0x30, 0xC2, 0xE5
	.byte 0x03, 0x30, 0xC2, 0xE5, 0x04, 0x30, 0xC2, 0xE5, 0x05, 0x30, 0xC2, 0xE5, 0x06, 0x30, 0xC2, 0xE5
	.byte 0x07, 0x30, 0xC2, 0xE5, 0x08, 0x30, 0xC2, 0xE5, 0x09, 0x30, 0xC2, 0xE5, 0x0A, 0x30, 0xC2, 0xE5
	.byte 0x0B, 0x30, 0xC2, 0xE5, 0x04, 0xA0, 0x82, 0xE5, 0x00, 0x00, 0x57, 0xE3, 0x04, 0x00, 0x8D, 0xE5
	.byte 0x01, 0x80, 0x88, 0xE2, 0x01, 0x60, 0x80, 0xE7, 0x08, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x59, 0xE3
	.byte 0x03, 0x00, 0xA0, 0xD1, 0x08, 0x00, 0x82, 0xD5, 0x04, 0x00, 0x00, 0xDA, 0x00, 0x00, 0x9D, 0xE5
	.byte 0x01, 0x90, 0x49, 0xE2, 0x04, 0x10, 0x90, 0xE4, 0x00, 0x00, 0x8D, 0xE5, 0x08, 0x10, 0x82, 0xE5
	.byte 0x00, 0x00, 0x55, 0xE3, 0x74, 0xFF, 0xFF, 0x1A, 0x04, 0x00, 0x9D, 0xE5, 0x08, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0xE0, 0xA5, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x78, 0x9D, 0x25, 0x02
	.byte 0x94, 0x04, 0x00, 0x00, 0xF0, 0x47, 0x2D, 0xE9, 0x01, 0x80, 0xA0, 0xE1, 0x00, 0x10, 0x98, 0xE5
	.byte 0x00, 0x90, 0xA0, 0xE1, 0x00, 0x00, 0x51, 0xE3, 0xF0, 0x87, 0xBD, 0x08, 0xF4, 0x60, 0x9F, 0xE5
	.byte 0xF4, 0x50, 0x9F, 0xE5, 0xF4, 0x40, 0x9F, 0xE5, 0xF4, 0xA0, 0x9F, 0xE5, 0x00, 0x70, 0xA0, 0xE3
	.byte 0x00, 0x00, 0x98, 0xE5, 0x0E, 0x00, 0x50, 0xE3, 0x00, 0xF1, 0x8F, 0x90, 0x2C, 0x00, 0x00, 0xEA
	.byte 0x2B, 0x00, 0x00, 0xEA, 0x2F, 0x00, 0x00, 0xEA, 0x0B, 0x00, 0x00, 0xEA, 0x0D, 0x00, 0x00, 0xEA
	.byte 0x15, 0x00, 0x00, 0xEA, 0x17, 0x00, 0x00, 0xEA, 0x2A, 0x00, 0x00, 0xEA, 0x18, 0x00, 0x00, 0xEA
	.byte 0x1A, 0x00, 0x00, 0xEA, 0x1C, 0x00, 0x00, 0xEA, 0x26, 0x00, 0x00, 0xEA, 0x25, 0x00, 0x00, 0xEA
	.byte 0x24, 0x00, 0x00, 0xEA, 0x23, 0x00, 0x00, 0xEA, 0x1A, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5
	.byte 0x00, 0x00, 0x89, 0xE5, 0x1F, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x08, 0x00, 0x98, 0x15, 0x00, 0x00, 0x50, 0x13, 0x1C, 0x70, 0x89, 0x05, 0x19, 0x00, 0x00, 0x0A
	.byte 0xA5, 0x2F, 0xFA, 0xEB, 0x1C, 0x00, 0x89, 0xE5, 0x16, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5
	.byte 0x04, 0x00, 0x89, 0xE5, 0x13, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5, 0x08, 0x00, 0x89, 0xE5
	.byte 0x10, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5, 0x0C, 0x00, 0x89, 0xE5, 0x0D, 0x00, 0x00, 0xEA
	.byte 0x04, 0x00, 0x98, 0xE5, 0x10, 0x00, 0x89, 0xE5, 0x0A, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5
	.byte 0x14, 0x00, 0x89, 0xE5, 0x07, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x98, 0xE5, 0x18, 0x00, 0x89, 0xE5
	.byte 0x04, 0x00, 0x00, 0xEA, 0x06, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x04, 0x20, 0xA0, 0xE1
	.byte 0x0A, 0x30, 0xA0, 0xE1, 0x28, 0x14, 0xFA, 0xEB, 0x0C, 0x00, 0xB8, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0xC6, 0xFF, 0xFF, 0x1A, 0xF0, 0x87, 0xBD, 0xE8, 0x90, 0xA5, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x28, 0xA0, 0x25, 0x02, 0x6C, 0x05, 0x00, 0x00, 0xF0, 0x43, 0x2D, 0xE9, 0x2C, 0xD0, 0x4D, 0xE2
	.byte 0x00, 0x90, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x00, 0x70, 0xA0, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0xD4, 0x01, 0x9F, 0xE5, 0xD4, 0x11, 0x9F, 0xE5, 0xD4, 0x21, 0x9F, 0xE5, 0xD4, 0x31, 0x9F, 0xE5
	.byte 0x15, 0x14, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0xC4, 0x01, 0x9F, 0xE5, 0xB4, 0x11, 0x9F, 0xE5, 0xB4, 0x21, 0x9F, 0xE5, 0xBC, 0x31, 0x9F, 0xE5
	.byte 0x0D, 0x14, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x83, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5, 0x08, 0x60, 0x94, 0xE5, 0x20, 0x01, 0x90, 0xE8
	.byte 0x08, 0x00, 0xA0, 0xE1, 0xBD, 0x26, 0xFA, 0xEB, 0xD0, 0x10, 0xD8, 0xE1, 0x90, 0x41, 0x9F, 0xE5
	.byte 0x01, 0x00, 0x51, 0xE3, 0x17, 0x00, 0x00, 0x1A, 0xD1, 0x10, 0xD8, 0xE1, 0x41, 0x00, 0x51, 0xE3
	.byte 0x01, 0x00, 0x00, 0xBA, 0x5A, 0x00, 0x51, 0xE3, 0x03, 0x00, 0x00, 0xDA, 0x61, 0x00, 0x51, 0xE3
	.byte 0x10, 0x00, 0x00, 0xBA, 0x7A, 0x00, 0x51, 0xE3, 0x0E, 0x00, 0x00, 0xCA, 0x01, 0x30, 0x40, 0xE2
	.byte 0xD3, 0x00, 0x98, 0xE1, 0x01, 0x00, 0x50, 0xE3, 0x0A, 0x00, 0x00, 0x1A, 0x00, 0x20, 0xA0, 0xE3
	.byte 0x08, 0x00, 0xA0, 0xE1, 0x20, 0x10, 0xA0, 0xE3, 0x03, 0x20, 0xC8, 0xE7, 0x66, 0x27, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x10, 0xA0, 0xE3, 0x01, 0x40, 0x88, 0xE2
	.byte 0x00, 0x10, 0xC0, 0xE5, 0x01, 0x80, 0x80, 0xE2, 0x28, 0x11, 0x9F, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x0B, 0x27, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x01, 0x70, 0xA0, 0x03, 0x03, 0x00, 0x00, 0x0A
	.byte 0xD0, 0x00, 0xD4, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x12, 0xF0, 0x83, 0xBD, 0x18
	.byte 0x05, 0x00, 0xA0, 0xE1, 0xDB, 0x1F, 0x89, 0xE2, 0x28, 0x42, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x18, 0x00, 0x00, 0x1A, 0x00, 0x08, 0x99, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x83, 0xBD, 0x08, 0x00, 0x00, 0x57, 0xE3, 0x01, 0x00, 0xA0, 0x13, 0x28, 0x00, 0x8D, 0x15
	.byte 0x00, 0x00, 0xA0, 0x03, 0x28, 0x00, 0x8D, 0x05, 0x20, 0x60, 0x8D, 0xE5, 0x24, 0x80, 0x8D, 0xE5
	.byte 0x08, 0x18, 0x99, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5
	.byte 0x08, 0x00, 0x8D, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x00, 0x28, 0x99, 0xE5
	.byte 0x20, 0x30, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1, 0x02, 0x10, 0xA0, 0xE3, 0xAE, 0xE6, 0xFF, 0xEB
	.byte 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x83, 0xBD, 0xE8, 0x09, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1
	.byte 0x03, 0xF3, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x00, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13
	.byte 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x00, 0x00, 0x57, 0xE3, 0x01, 0x10, 0xA0, 0x13
	.byte 0x1C, 0x10, 0x8D, 0x15, 0x00, 0x10, 0xA0, 0x03, 0x1C, 0x10, 0x8D, 0x05, 0x10, 0x50, 0x8D, 0xE5
	.byte 0x14, 0x60, 0x8D, 0xE5, 0x18, 0x80, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x00, 0x20, 0x8D, 0xE5, 0x22, 0x00, 0x8D, 0xE9, 0x10, 0x10, 0xA0, 0xE3, 0x0C, 0x10, 0x8D, 0xE5
	.byte 0x00, 0x20, 0x90, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE3
	.byte 0x91, 0xE6, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x83, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xE8, 0x9E, 0x25, 0x02, 0x7C, 0x05, 0x00, 0x00, 0xF0, 0xA5, 0x25, 0x02
	.byte 0x82, 0x05, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02, 0x0C, 0xA6, 0x25, 0x02, 0xF0, 0x40, 0x2D, 0xE9
	.byte 0x2C, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x34, 0x01, 0x9F, 0xE5, 0x34, 0x11, 0x9F, 0xE5, 0x34, 0x21, 0x9F, 0xE5, 0x34, 0x31, 0x9F, 0xE5
	.byte 0x91, 0x13, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x24, 0x01, 0x9F, 0xE5, 0x14, 0x11, 0x9F, 0xE5, 0x14, 0x21, 0x9F, 0xE5, 0x5F, 0x3E, 0xA0, 0xE3
	.byte 0x89, 0x13, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x96, 0xE5, 0xDB, 0x1F, 0x87, 0xE2, 0x30, 0x00, 0x90, 0xE8
	.byte 0x04, 0x00, 0xA0, 0xE1, 0xCD, 0x41, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x18, 0x00, 0x00, 0x1A
	.byte 0x00, 0x08, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08
	.byte 0x08, 0x00, 0x96, 0xE5, 0x02, 0x10, 0xA0, 0xE3, 0x00, 0x00, 0x50, 0xE3, 0x20, 0x00, 0x8D, 0x15
	.byte 0x00, 0x00, 0xA0, 0x03, 0x20, 0x00, 0x8D, 0x05, 0x24, 0x50, 0x8D, 0xE5, 0x28, 0x10, 0x8D, 0xE5
	.byte 0x08, 0x28, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5
	.byte 0x08, 0x00, 0x8D, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x00, 0x28, 0x97, 0xE5
	.byte 0x20, 0x30, 0x8D, 0xE2, 0x07, 0x00, 0xA0, 0xE1, 0x53, 0xE6, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x80, 0xBD, 0xE8, 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0xA8, 0xF2, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x00, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13, 0x2C, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x80, 0xBD, 0x08, 0x10, 0x40, 0x8D, 0xE5, 0x08, 0x10, 0x96, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x00, 0x00, 0x51, 0xE3, 0x14, 0x10, 0x8D, 0x15, 0x00, 0x10, 0xA0, 0x03, 0x14, 0x10, 0x8D, 0x05
	.byte 0x02, 0x10, 0xA0, 0xE3, 0x1C, 0x10, 0x8D, 0xE5, 0x18, 0x50, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5
	.byte 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5, 0x12, 0x00, 0x8D, 0xE9, 0x10, 0x10, 0xA0, 0xE3
	.byte 0x0C, 0x10, 0x8D, 0xE5, 0x00, 0x20, 0x90, 0xE5, 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE3
	.byte 0x35, 0xE6, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xA4, 0x9E, 0x25, 0x02, 0xEA, 0x05, 0x00, 0x00, 0xF0, 0xA5, 0x25, 0x02
	.byte 0xF0, 0x40, 0x2D, 0xE9, 0x2C, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x38, 0x01, 0x9F, 0xE5, 0x38, 0x11, 0x9F, 0xE5, 0x38, 0x21, 0x9F, 0xE5
	.byte 0x38, 0x31, 0x9F, 0xE5, 0x38, 0x13, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x28, 0x01, 0x9F, 0xE5, 0x18, 0x11, 0x9F, 0xE5, 0x18, 0x21, 0x9F, 0xE5
	.byte 0x20, 0x31, 0x9F, 0xE5, 0x30, 0x13, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3
	.byte 0x2C, 0xD0, 0x8D, 0x12, 0xF0, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x96, 0xE5, 0xDB, 0x1F, 0x87, 0xE2
	.byte 0x30, 0x00, 0x90, 0xE8, 0x04, 0x00, 0xA0, 0xE1, 0x74, 0x41, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x19, 0x00, 0x00, 0x1A, 0x00, 0x08, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x80, 0xBD, 0x08, 0x08, 0x00, 0x96, 0xE5, 0x20, 0x30, 0x8D, 0xE2, 0x00, 0x00, 0x50, 0xE3
	.byte 0x20, 0x00, 0x8D, 0x15, 0x00, 0x00, 0xA0, 0x03, 0x20, 0x00, 0x8D, 0x05, 0x03, 0x00, 0xA0, 0xE3
	.byte 0x28, 0x00, 0x8D, 0xE5, 0x24, 0x50, 0x8D, 0xE5, 0x08, 0x18, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0x10, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x8D, 0xE5, 0x0C, 0x00, 0xA0, 0xE3
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0x00, 0x28, 0x97, 0xE5, 0x07, 0x00, 0xA0, 0xE1, 0x02, 0x10, 0xA0, 0xE3
	.byte 0xF9, 0xE5, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x4E, 0xF2, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x00, 0x10, 0x90, 0x15
	.byte 0x00, 0x00, 0x51, 0x13, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x10, 0x40, 0x8D, 0xE5
	.byte 0x08, 0x10, 0x96, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x00, 0x00, 0x51, 0xE3, 0x14, 0x10, 0x8D, 0x15
	.byte 0x00, 0x10, 0xA0, 0x03, 0x14, 0x10, 0x8D, 0x05, 0x03, 0x10, 0xA0, 0xE3, 0x1C, 0x10, 0x8D, 0xE5
	.byte 0x18, 0x50, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5
	.byte 0x12, 0x00, 0x8D, 0xE9, 0x10, 0x10, 0xA0, 0xE3, 0x0C, 0x10, 0x8D, 0xE5, 0x00, 0x20, 0x90, 0xE5
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE3, 0xDB, 0xE5, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x94, 0x9D, 0x25, 0x02
	.byte 0x1F, 0x06, 0x00, 0x00, 0xF0, 0xA5, 0x25, 0x02, 0x25, 0x06, 0x00, 0x00, 0xF0, 0x40, 0x2D, 0xE9
	.byte 0x2C, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x34, 0x01, 0x9F, 0xE5, 0x34, 0x11, 0x9F, 0xE5, 0x34, 0x21, 0x9F, 0xE5, 0x34, 0x31, 0x9F, 0xE5
	.byte 0xDD, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x24, 0x01, 0x9F, 0xE5, 0x14, 0x11, 0x9F, 0xE5, 0x14, 0x21, 0x9F, 0xE5, 0x1C, 0x31, 0x9F, 0xE5
	.byte 0xD5, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x96, 0xE5, 0xDB, 0x1F, 0x87, 0xE2, 0x30, 0x00, 0x90, 0xE8
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x19, 0x41, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x19, 0x00, 0x00, 0x1A
	.byte 0x00, 0x08, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08
	.byte 0x08, 0x00, 0x96, 0xE5, 0x20, 0x30, 0x8D, 0xE2, 0x00, 0x00, 0x50, 0xE3, 0x20, 0x00, 0x8D, 0x15
	.byte 0x00, 0x00, 0xA0, 0x03, 0x20, 0x00, 0x8D, 0x05, 0x04, 0x00, 0xA0, 0xE3, 0x28, 0x00, 0x8D, 0xE5
	.byte 0x24, 0x50, 0x8D, 0xE5, 0x08, 0x18, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5
	.byte 0x04, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x8D, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5
	.byte 0x00, 0x28, 0x97, 0xE5, 0x07, 0x00, 0xA0, 0xE1, 0x02, 0x10, 0xA0, 0xE3, 0x9E, 0xE5, 0xFF, 0xEB
	.byte 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0xF3, 0xF1, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x00, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13
	.byte 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x10, 0x40, 0x8D, 0xE5, 0x08, 0x10, 0x96, 0xE5
	.byte 0x00, 0x20, 0xA0, 0xE3, 0x00, 0x00, 0x51, 0xE3, 0x14, 0x10, 0x8D, 0x15, 0x00, 0x10, 0xA0, 0x03
	.byte 0x14, 0x10, 0x8D, 0x05, 0x04, 0x10, 0xA0, 0xE3, 0x18, 0x50, 0x8D, 0xE5, 0x1C, 0x10, 0x8D, 0xE5
	.byte 0x2C, 0x30, 0x90, 0xE5, 0x00, 0x30, 0x8D, 0xE5, 0x14, 0x00, 0x8D, 0xE9, 0x10, 0x20, 0xA0, 0xE3
	.byte 0x0C, 0x20, 0x8D, 0xE5, 0x00, 0x20, 0x90, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x81, 0xE5, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xA4, 0x9D, 0x25, 0x02, 0x54, 0x06, 0x00, 0x00, 0xF0, 0xA5, 0x25, 0x02
	.byte 0x5A, 0x06, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x20, 0x00, 0x9F, 0xE5, 0x20, 0x10, 0x9F, 0xE5, 0x20, 0x20, 0x9F, 0xE5
	.byte 0x20, 0x30, 0x9F, 0xE5, 0x84, 0x12, 0xFA, 0xEB, 0x1C, 0x20, 0x94, 0xE5, 0x18, 0x10, 0x9F, 0xE5
	.byte 0x1C, 0x00, 0x85, 0xE2, 0xFD, 0x1D, 0x00, 0xEB, 0x38, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xD4, 0x9D, 0x25, 0x02, 0x87, 0x06, 0x00, 0x00, 0x14, 0xA6, 0x25, 0x02
	.byte 0x70, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x60, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x00, 0x01, 0x9F, 0xE5, 0x00, 0x11, 0x9F, 0xE5, 0x00, 0x21, 0x9F, 0xE5
	.byte 0x00, 0x31, 0x9F, 0xE5, 0x70, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0xF0, 0x00, 0x9F, 0xE5, 0xE0, 0x10, 0x9F, 0xE5, 0xE0, 0x20, 0x9F, 0xE5
	.byte 0xE8, 0x30, 0x9F, 0xE5, 0x68, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x18, 0xD0, 0x8D, 0x12, 0x70, 0x80, 0xBD, 0x18, 0x08, 0x50, 0x96, 0xE5, 0x20, 0x20, 0x96, 0xE5
	.byte 0x05, 0x00, 0xA0, 0xE1, 0xDB, 0x1F, 0x84, 0xE2, 0x00, 0x60, 0x92, 0xE5, 0xAB, 0x40, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x22, 0x00, 0x00, 0x1A, 0x06, 0x00, 0xA0, 0xE1, 0x13, 0x25, 0xFA, 0xEB
	.byte 0x40, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x3A, 0xA4, 0x00, 0x9F, 0xE5, 0x8C, 0x10, 0x9F, 0xE5
	.byte 0x8C, 0x20, 0x9F, 0xE5, 0x9C, 0x30, 0x9F, 0xE5, 0x53, 0x12, 0xFA, 0xEB, 0x06, 0x10, 0xA0, 0xE1
	.byte 0xDB, 0x0F, 0x84, 0xE2, 0x40, 0x20, 0xA0, 0xE3, 0x41, 0x25, 0xFA, 0xEB, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x0C, 0x20, 0x8D, 0xE2, 0xAB, 0x13, 0xC4, 0xE5, 0x04, 0x10, 0x82, 0xE5, 0x08, 0x10, 0x82, 0xE5
	.byte 0x00, 0x10, 0x82, 0xE5, 0x09, 0x30, 0xA0, 0xE3, 0x04, 0x00, 0xA0, 0xE1, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x0C, 0x30, 0x8D, 0xE5, 0x10, 0x50, 0x8D, 0xE5, 0x14, 0x60, 0x8D, 0xE5, 0x8D, 0xF7, 0xFF, 0xEB
	.byte 0x00, 0x10, 0xB0, 0xE1, 0x06, 0x00, 0x00, 0x0A, 0x01, 0x30, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x8D, 0xE5, 0x04, 0x50, 0x8D, 0xE5, 0x08, 0x60, 0x8D, 0xE5
	.byte 0x5C, 0xF8, 0xFF, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1
	.byte 0xEA, 0xF5, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xE4, 0x9D, 0x25, 0x02, 0x94, 0x06, 0x00, 0x00, 0x1C, 0xA6, 0x25, 0x02
	.byte 0x9A, 0x06, 0x00, 0x00, 0x34, 0xA6, 0x25, 0x02, 0xAA, 0x06, 0x00, 0x00, 0xF0, 0x43, 0x2D, 0xE9
	.byte 0x2C, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x80, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x3C, 0x02, 0x9F, 0xE5, 0x3C, 0x12, 0x9F, 0xE5, 0x3C, 0x22, 0x9F, 0xE5, 0x3C, 0x32, 0x9F, 0xE5
	.byte 0x21, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x98, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x2C, 0x02, 0x9F, 0xE5, 0x1C, 0x12, 0x9F, 0xE5, 0x1C, 0x22, 0x9F, 0xE5, 0x24, 0x32, 0x9F, 0xE5
	.byte 0x19, 0x12, 0xFA, 0xEB, 0x24, 0x00, 0x98, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x83, 0xBD, 0x18, 0x08, 0x60, 0x98, 0xE5, 0x20, 0x10, 0x98, 0xE5, 0xD0, 0x00, 0xD6, 0xE1
	.byte 0x00, 0x50, 0x91, 0xE5, 0x0C, 0x70, 0x98, 0xE5, 0x40, 0x00, 0x50, 0xE3, 0x10, 0x80, 0x98, 0xE5
	.byte 0x09, 0x00, 0x00, 0x1A, 0xD1, 0x00, 0xF6, 0xE1, 0x02, 0x90, 0xA0, 0xE3, 0x00, 0x00, 0x50, 0xE3
	.byte 0x12, 0x00, 0x00, 0x1A, 0xE0, 0x01, 0x9F, 0xE5, 0xC8, 0x11, 0x9F, 0xE5, 0xC8, 0x21, 0x9F, 0xE5
	.byte 0xD8, 0x31, 0x9F, 0xE5, 0x04, 0x12, 0xFA, 0xEB, 0x0C, 0x00, 0x00, 0xEA, 0x2B, 0x00, 0x50, 0xE3
	.byte 0x09, 0x00, 0x00, 0x1A, 0xD1, 0x00, 0xF6, 0xE1, 0x01, 0x90, 0xA0, 0xE3, 0x00, 0x00, 0x50, 0xE3
	.byte 0x06, 0x00, 0x00, 0x1A, 0xB0, 0x01, 0x9F, 0xE5, 0x98, 0x11, 0x9F, 0xE5, 0x98, 0x21, 0x9F, 0xE5
	.byte 0x6F, 0x3E, 0x89, 0xE2, 0xF8, 0x11, 0xFA, 0xEB, 0x00, 0x00, 0x00, 0xEA, 0x00, 0x90, 0xA0, 0xE3
	.byte 0x06, 0x00, 0xA0, 0xE1, 0xDB, 0x1F, 0x84, 0xE2, 0x19, 0x25, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x21, 0x00, 0x00, 0x1A, 0x46, 0xF0, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x20, 0x20, 0x8D, 0xE2
	.byte 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x82, 0xE5
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5, 0x20, 0x10, 0x8D, 0xE5, 0x24, 0x50, 0x8D, 0xE5
	.byte 0x2C, 0xF7, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08
	.byte 0x1C, 0x60, 0x90, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1
	.byte 0x5E, 0xF0, 0xFF, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x34, 0x20, 0x86, 0xE2
	.byte 0x8A, 0xF2, 0xFF, 0xEB, 0x01, 0x30, 0xA0, 0xE3, 0x14, 0x11, 0x9F, 0xE5, 0x05, 0x20, 0xA0, 0xE1
	.byte 0x1C, 0x00, 0x84, 0xE2, 0x30, 0x30, 0x86, 0xE5, 0x4C, 0x1D, 0x00, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x83, 0xBD, 0xE8, 0xFA, 0xF1, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x0A
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1, 0x05, 0x20, 0xA0, 0xE1, 0x09, 0x30, 0xA0, 0xE1
	.byte 0x80, 0x01, 0x8D, 0xE8, 0x86, 0xF3, 0xFF, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1
	.byte 0xE8, 0xF2, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x01, 0xF1, 0xFF, 0xEB, 0x00, 0x70, 0xB0, 0xE1
	.byte 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x08, 0x00, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x0D, 0x00, 0x00, 0x0A, 0x14, 0x50, 0x8D, 0xE5, 0x18, 0x60, 0x8D, 0xE5, 0x1C, 0x90, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x21, 0x00, 0x8D, 0xE9
	.byte 0x0C, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x08, 0x20, 0x97, 0xE5, 0x14, 0x30, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE3, 0x93, 0xE4, 0xFF, 0xEB, 0x20, 0x00, 0x97, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x2C, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x10, 0x50, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x21, 0x00, 0x8D, 0xE9
	.byte 0x04, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x97, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x0C, 0x10, 0xA0, 0xE3, 0x83, 0xE4, 0xFF, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x83, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xF4, 0x9D, 0x25, 0x02
	.byte 0xD6, 0x06, 0x00, 0x00, 0x1C, 0xA6, 0x25, 0x02, 0xDC, 0x06, 0x00, 0x00, 0x50, 0xA6, 0x25, 0x02
	.byte 0xEB, 0x06, 0x00, 0x00, 0x60, 0xA6, 0x25, 0x02, 0xF0, 0x41, 0x2D, 0xE9, 0x28, 0xD0, 0x4D, 0xE2
	.byte 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x10, 0x01, 0x9F, 0xE5
	.byte 0x10, 0x11, 0x9F, 0xE5, 0x10, 0x21, 0x9F, 0xE5, 0x10, 0x31, 0x9F, 0xE5, 0x82, 0x11, 0xFA, 0xEB
	.byte 0x20, 0x10, 0x94, 0xE5, 0x24, 0x00, 0x94, 0xE5, 0x08, 0x40, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x04, 0x80, 0x91, 0xC5, 0x00, 0x50, 0x91, 0xE5, 0xF4, 0x80, 0x9F, 0xD5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0xDB, 0x1F, 0x87, 0xE2, 0x9E, 0x24, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x81, 0xBD, 0x08, 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x05, 0x20, 0xA0, 0xE1
	.byte 0xB7, 0xF3, 0xFF, 0xEB, 0x07, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x95, 0xF2, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x05, 0x10, 0xA0, 0xE1, 0xAE, 0xF0, 0xFF, 0xEB, 0x00, 0x60, 0xB0, 0xE1, 0x28, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x81, 0xBD, 0x08, 0x0C, 0x00, 0x96, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x0E, 0x00, 0x00, 0x0A
	.byte 0x00, 0x20, 0xA0, 0xE3, 0x14, 0x50, 0x8D, 0xE5, 0x18, 0x40, 0x8D, 0xE5, 0x1C, 0x20, 0x8D, 0xE5
	.byte 0x20, 0x80, 0x8D, 0xE5, 0x24, 0x20, 0x8D, 0xE5, 0x2C, 0x10, 0x96, 0xE5, 0x14, 0x00, 0xA0, 0xE3
	.byte 0x26, 0x00, 0x8D, 0xE8, 0x0C, 0x00, 0x8D, 0xE5, 0x0C, 0x20, 0x96, 0xE5, 0x14, 0x30, 0x8D, 0xE2
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE3, 0x3F, 0xE4, 0xFF, 0xEB, 0x20, 0x00, 0x96, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08, 0x10, 0x50, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x96, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x21, 0x00, 0x8D, 0xE9
	.byte 0x04, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x96, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x0C, 0x10, 0xA0, 0xE3, 0x2F, 0xE4, 0xFF, 0xEB, 0x28, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x81, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x04, 0x9E, 0x25, 0x02
	.byte 0x52, 0x07, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02, 0xF0, 0x43, 0x2D, 0xE9, 0x34, 0xD0, 0x4D, 0xE2
	.byte 0x00, 0x90, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xB0, 0x01, 0x9F, 0xE5
	.byte 0xB0, 0x11, 0x9F, 0xE5, 0xB0, 0x21, 0x9F, 0xE5, 0xB0, 0x31, 0x9F, 0xE5, 0x32, 0x11, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x02, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x50, 0x13
	.byte 0x00, 0x10, 0xA0, 0x13, 0x00, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x90, 0x01, 0x9F, 0xE5
	.byte 0x80, 0x11, 0x9F, 0xE5, 0x80, 0x21, 0x9F, 0xE5, 0x88, 0x31, 0x9F, 0xE5, 0x26, 0x11, 0xFA, 0xEB
	.byte 0x24, 0x10, 0x95, 0xE5, 0x02, 0x00, 0x51, 0xE3, 0x03, 0x00, 0x51, 0x13, 0x34, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x83, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x51, 0xE3, 0x08, 0x80, 0x90, 0x05
	.byte 0x50, 0x00, 0x90, 0xE8, 0x09, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1, 0x04, 0x20, 0xA0, 0xE1
	.byte 0x08, 0x50, 0x95, 0xE5, 0x50, 0x81, 0x9F, 0x15, 0x5D, 0xF3, 0xFF, 0xEB, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x5A, 0xF0, 0xFF, 0xEB, 0x00, 0x70, 0xB0, 0xE1, 0x34, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x83, 0xBD, 0x08, 0x06, 0x00, 0xA0, 0xE1, 0xDB, 0x1F, 0x89, 0xE2, 0x5B, 0x3F, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x16, 0x00, 0x00, 0x1A, 0x04, 0x00, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x0E, 0x00, 0x00, 0x0A, 0x28, 0x40, 0x8D, 0xE5, 0x2C, 0x50, 0x8D, 0xE5, 0x30, 0x80, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5
	.byte 0x08, 0x00, 0x8D, 0xE5, 0x0C, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x04, 0x20, 0x97, 0xE5
	.byte 0x28, 0x30, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE3, 0xE6, 0xE3, 0xFF, 0xEB
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0xF8, 0xEF, 0xFF, 0xEB, 0x34, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x83, 0xBD, 0xE8, 0x09, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x19, 0xF2, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x34, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x0C, 0x00, 0x97, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x10, 0x00, 0x00, 0x0A, 0x02, 0x00, 0xA0, 0xE3, 0x1C, 0x00, 0x8D, 0xE5
	.byte 0x14, 0x40, 0x8D, 0xE5, 0x18, 0x60, 0x8D, 0xE5, 0x20, 0x80, 0x8D, 0xE5, 0x24, 0x50, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x11, 0x00, 0x8D, 0xE9
	.byte 0x14, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x0C, 0x20, 0x97, 0xE5, 0x14, 0x30, 0x8D, 0xE2
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE3, 0xC7, 0xE3, 0xFF, 0xEB, 0x20, 0x00, 0x97, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x34, 0xD0, 0x8D, 0x02, 0xF0, 0x83, 0xBD, 0x08, 0x10, 0x40, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5, 0x11, 0x00, 0x8D, 0xE9
	.byte 0x04, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x97, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x0C, 0x10, 0xA0, 0xE3, 0xB7, 0xE3, 0xFF, 0xEB, 0x34, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x83, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x14, 0x9E, 0x25, 0x02
	.byte 0x94, 0x07, 0x00, 0x00, 0x68, 0xA6, 0x25, 0x02, 0x9A, 0x07, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02
	.byte 0xF0, 0x41, 0x2D, 0xE9, 0x28, 0xD0, 0x4D, 0xE2, 0x01, 0x70, 0xB0, 0xE1, 0x00, 0x80, 0xA0, 0xE1
	.byte 0x02, 0x60, 0xA0, 0xE1, 0x03, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x58, 0x01, 0x9F, 0xE5
	.byte 0x58, 0x11, 0x9F, 0xE5, 0x58, 0x21, 0x9F, 0xE5, 0x58, 0x31, 0x9F, 0xE5, 0xB6, 0x10, 0xFA, 0xEB
	.byte 0xD0, 0x00, 0xD7, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x48, 0x01, 0x9F, 0xE5
	.byte 0x38, 0x11, 0x9F, 0xE5, 0x38, 0x21, 0x9F, 0xE5, 0x38, 0x31, 0x9F, 0xE5, 0xAE, 0x10, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x56, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x30, 0x01, 0x9F, 0xE5, 0x1C, 0x11, 0x9F, 0xE5
	.byte 0x1C, 0x21, 0x9F, 0xE5, 0x28, 0x31, 0x9F, 0xE5, 0xA7, 0x10, 0xFA, 0xEB, 0xD0, 0x00, 0xD6, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x18, 0x01, 0x9F, 0xE5, 0xFC, 0x10, 0x9F, 0xE5
	.byte 0xFC, 0x20, 0x9F, 0xE5, 0x08, 0x31, 0x9F, 0xE5, 0x9F, 0x10, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x00, 0x01, 0x9F, 0xE5, 0xE0, 0x10, 0x9F, 0xE5, 0xE0, 0x20, 0x9F, 0xE5
	.byte 0xF8, 0x30, 0x9F, 0xE5, 0x98, 0x10, 0xFA, 0xEB, 0x08, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE1
	.byte 0x06, 0x20, 0xA0, 0xE1, 0xDA, 0xF2, 0xFF, 0xEB, 0x08, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1
	.byte 0xB8, 0xF1, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08
	.byte 0x08, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1, 0xD1, 0xEF, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08, 0x0C, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x0F, 0x00, 0x00, 0x0A, 0x01, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0xA0, 0xE3, 0x1C, 0x00, 0x8D, 0xE5
	.byte 0x14, 0x60, 0x8D, 0xE5, 0x18, 0x70, 0x8D, 0xE5, 0x20, 0x50, 0x8D, 0xE5, 0x24, 0x20, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x94, 0xE5, 0x14, 0x00, 0xA0, 0xE3, 0x46, 0x00, 0x8D, 0xE8, 0x0C, 0x00, 0x8D, 0xE5
	.byte 0x0C, 0x20, 0x94, 0xE5, 0x14, 0x30, 0x8D, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE3
	.byte 0x61, 0xE3, 0xFF, 0xEB, 0x20, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x81, 0xBD, 0x08, 0x10, 0x60, 0x8D, 0xE5, 0x2C, 0x10, 0x94, 0xE5, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0x10, 0x8D, 0xE5, 0x41, 0x00, 0x8D, 0xE9, 0x04, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5
	.byte 0x20, 0x20, 0x94, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0x0C, 0x10, 0xA0, 0xE3
	.byte 0x51, 0xE3, 0xFF, 0xEB, 0x28, 0xD0, 0x8D, 0xE2, 0xF0, 0x81, 0xBD, 0xE8, 0xA0, 0xA6, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0x24, 0xA2, 0x25, 0x02, 0xE6, 0x07, 0x00, 0x00, 0xB0, 0xA6, 0x25, 0x02
	.byte 0xC0, 0xA6, 0x25, 0x02, 0xE7, 0x07, 0x00, 0x00, 0xD0, 0xA6, 0x25, 0x02, 0xE4, 0xA6, 0x25, 0x02
	.byte 0xE8, 0x07, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x54, 0x00, 0x9F, 0xE5, 0x54, 0x10, 0x9F, 0xE5, 0x54, 0x20, 0x9F, 0xE5
	.byte 0x54, 0x30, 0x9F, 0xE5, 0x50, 0x10, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x44, 0x00, 0x9F, 0xE5, 0x34, 0x10, 0x9F, 0xE5, 0x34, 0x20, 0x9F, 0xE5
	.byte 0x3C, 0x30, 0x9F, 0xE5, 0x48, 0x10, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5, 0x08, 0x10, 0x94, 0xE5, 0x00, 0x30, 0x90, 0xE5
	.byte 0x20, 0x20, 0x9F, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x19, 0xF3, 0xFF, 0xEB, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x24, 0x9E, 0x25, 0x02, 0x13, 0x08, 0x00, 0x00
	.byte 0x1C, 0xA6, 0x25, 0x02, 0x19, 0x08, 0x00, 0x00, 0x70, 0xFD, 0x24, 0x02, 0xF0, 0x41, 0x2D, 0xE9
	.byte 0x28, 0xD0, 0x4D, 0xE2, 0x01, 0x70, 0xB0, 0xE1, 0x00, 0x80, 0xA0, 0xE1, 0x02, 0x60, 0xA0, 0xE1
	.byte 0x03, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x58, 0x01, 0x9F, 0xE5, 0x58, 0x11, 0x9F, 0xE5
	.byte 0x58, 0x21, 0x9F, 0xE5, 0x58, 0x31, 0x9F, 0xE5, 0x2B, 0x10, 0xFA, 0xEB, 0xD0, 0x00, 0xD7, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x48, 0x01, 0x9F, 0xE5, 0x38, 0x11, 0x9F, 0xE5
	.byte 0x38, 0x21, 0x9F, 0xE5, 0x38, 0x31, 0x9F, 0xE5, 0x23, 0x10, 0xFA, 0xEB, 0x00, 0x00, 0x56, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x30, 0x01, 0x9F, 0xE5, 0x1C, 0x11, 0x9F, 0xE5, 0x1C, 0x21, 0x9F, 0xE5
	.byte 0x28, 0x31, 0x9F, 0xE5, 0x1C, 0x10, 0xFA, 0xEB, 0xD0, 0x00, 0xD6, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x18, 0x01, 0x9F, 0xE5, 0xFC, 0x10, 0x9F, 0xE5, 0xFC, 0x20, 0x9F, 0xE5
	.byte 0x08, 0x31, 0x9F, 0xE5, 0x14, 0x10, 0xFA, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x00, 0x01, 0x9F, 0xE5, 0xE0, 0x10, 0x9F, 0xE5, 0xE0, 0x20, 0x9F, 0xE5, 0xF8, 0x30, 0x9F, 0xE5
	.byte 0x0D, 0x10, 0xFA, 0xEB, 0x08, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1
	.byte 0x4F, 0xF2, 0xFF, 0xEB, 0x08, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1, 0x2D, 0xF1, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08, 0x08, 0x00, 0xA0, 0xE1
	.byte 0x06, 0x10, 0xA0, 0xE1, 0x46, 0xEF, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x28, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x81, 0xBD, 0x08, 0x0C, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x0F, 0x00, 0x00, 0x0A
	.byte 0x03, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0xA0, 0xE3, 0x1C, 0x00, 0x8D, 0xE5, 0x14, 0x60, 0x8D, 0xE5
	.byte 0x18, 0x70, 0x8D, 0xE5, 0x20, 0x50, 0x8D, 0xE5, 0x24, 0x20, 0x8D, 0xE5, 0x2C, 0x10, 0x94, 0xE5
	.byte 0x14, 0x00, 0xA0, 0xE3, 0x46, 0x00, 0x8D, 0xE8, 0x0C, 0x00, 0x8D, 0xE5, 0x0C, 0x20, 0x94, 0xE5
	.byte 0x14, 0x30, 0x8D, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE3, 0xD6, 0xE2, 0xFF, 0xEB
	.byte 0x20, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF0, 0x81, 0xBD, 0x08
	.byte 0x10, 0x60, 0x8D, 0xE5, 0x2C, 0x10, 0x94, 0xE5, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x10, 0x8D, 0xE5
	.byte 0x41, 0x00, 0x8D, 0xE9, 0x04, 0x00, 0xA0, 0xE3, 0x0C, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x94, 0xE5
	.byte 0x10, 0x30, 0x8D, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0x0C, 0x10, 0xA0, 0xE3, 0xC6, 0xE2, 0xFF, 0xEB
	.byte 0x28, 0xD0, 0x8D, 0xE2, 0xF0, 0x81, 0xBD, 0xE8, 0xA0, 0xA6, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x40, 0xA2, 0x25, 0x02, 0x28, 0x08, 0x00, 0x00, 0xB0, 0xA6, 0x25, 0x02, 0xC0, 0xA6, 0x25, 0x02
	.byte 0x29, 0x08, 0x00, 0x00, 0xD0, 0xA6, 0x25, 0x02, 0xE4, 0xA6, 0x25, 0x02, 0x2A, 0x08, 0x00, 0x00
	.byte 0x38, 0x40, 0x2D, 0xE9, 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1
	.byte 0x02, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x30, 0x00, 0x9F, 0xE5, 0x30, 0x10, 0x9F, 0xE5
	.byte 0x30, 0x20, 0x9F, 0xE5, 0x30, 0x30, 0x9F, 0xE5, 0xC3, 0x0F, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x02, 0x00, 0x50, 0xE3, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5, 0x1C, 0x20, 0x9F, 0xE5
	.byte 0x0A, 0x00, 0x90, 0xE8, 0x05, 0x00, 0xA0, 0xE1, 0x95, 0xF2, 0xFF, 0xEB, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xF0, 0xA5, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x34, 0x9E, 0x25, 0x02, 0x59, 0x08, 0x00, 0x00
	.byte 0x9C, 0xFF, 0x24, 0x02, 0x70, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xB0, 0x00, 0x9F, 0xE5, 0xB0, 0x10, 0x9F, 0xE5
	.byte 0xB0, 0x20, 0x9F, 0xE5, 0xB0, 0x30, 0x9F, 0xE5, 0xAB, 0x0F, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0xA0, 0x00, 0x9F, 0xE5, 0x90, 0x10, 0x9F, 0xE5
	.byte 0x90, 0x20, 0x9F, 0xE5, 0x87, 0x3E, 0xA0, 0xE3, 0xA3, 0x0F, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x02, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12, 0x70, 0x80, 0xBD, 0x18, 0x20, 0x10, 0x95, 0xE5
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x60, 0x00, 0x91, 0xE8, 0x05, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1
	.byte 0xF1, 0xF0, 0xFF, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0xDC, 0xEE, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x14, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13, 0x18, 0xD0, 0x8D, 0x02
	.byte 0x70, 0x80, 0xBD, 0x08, 0x10, 0x50, 0x8D, 0xE5, 0x14, 0x60, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5
	.byte 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5, 0x22, 0x00, 0x8D, 0xE9, 0x08, 0x10, 0xA0, 0xE3
	.byte 0x0C, 0x10, 0x8D, 0xE5, 0x14, 0x20, 0x90, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x09, 0x10, 0xA0, 0xE3, 0x70, 0xE2, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x54, 0x9E, 0x25, 0x02, 0x6A, 0x08, 0x00, 0x00
	.byte 0xF0, 0xA5, 0x25, 0x02, 0xF8, 0x4F, 0x2D, 0xE9, 0x28, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x94, 0xE5, 0x00, 0x90, 0xA0, 0xE1, 0x02, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0xAA
	.byte 0xD0, 0x01, 0x9F, 0xE5, 0xD0, 0x11, 0x9F, 0xE5, 0xD0, 0x21, 0x9F, 0xE5, 0xD0, 0x31, 0x9F, 0xE5
	.byte 0x71, 0x0F, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0xB2
	.byte 0xF8, 0x8F, 0xBD, 0xB8, 0x20, 0x10, 0x94, 0xE5, 0x09, 0x00, 0xA0, 0xE1, 0x00, 0x80, 0x91, 0xE5
	.byte 0x04, 0x50, 0x91, 0xE5, 0x08, 0x10, 0xA0, 0xE1, 0x95, 0xEF, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x28, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08, 0x20, 0x10, 0x94, 0xE5, 0x24, 0x20, 0x94, 0xE5
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0x81, 0xE2, 0x02, 0x20, 0x42, 0xE2, 0x47, 0xFA, 0xFF, 0xEB
	.byte 0x04, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08
	.byte 0x00, 0x00, 0x90, 0xE5, 0x04, 0x70, 0x9D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x40, 0x00, 0x00, 0x0A
	.byte 0x60, 0xA1, 0x9F, 0xE5, 0x50, 0xB1, 0x9F, 0xE5, 0x01, 0x40, 0xA0, 0xE3, 0x02, 0x50, 0xA0, 0xE3
	.byte 0x00, 0x60, 0xA0, 0xE3, 0x00, 0x00, 0x97, 0xE5, 0x0E, 0x00, 0x50, 0xE3, 0x00, 0xF1, 0x8F, 0x90
	.byte 0x2F, 0x00, 0x00, 0xEA, 0x2E, 0x00, 0x00, 0xEA, 0x32, 0x00, 0x00, 0xEA, 0x31, 0x00, 0x00, 0xEA
	.byte 0x30, 0x00, 0x00, 0xEA, 0x2F, 0x00, 0x00, 0xEA, 0x2E, 0x00, 0x00, 0xEA, 0x07, 0x00, 0x00, 0xEA
	.byte 0x2C, 0x00, 0x00, 0xEA, 0x2B, 0x00, 0x00, 0xEA, 0x2A, 0x00, 0x00, 0xEA, 0x10, 0x00, 0x00, 0xEA
	.byte 0x19, 0x00, 0x00, 0xEA, 0x27, 0x00, 0x00, 0xEA, 0x26, 0x00, 0x00, 0xEA, 0x25, 0x00, 0x00, 0xEA
	.byte 0x04, 0x00, 0x97, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x08, 0x20, 0x97, 0xE5
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0xA0, 0xE1, 0xEC, 0xEF, 0xFF, 0xEB, 0x1D, 0x00, 0x00, 0xEA
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1, 0xE7, 0xEF, 0xFF, 0xEB
	.byte 0x18, 0x00, 0x00, 0xEA, 0x08, 0x10, 0x97, 0xE5, 0x00, 0x00, 0x51, 0xE3, 0x15, 0x00, 0x00, 0x0A
	.byte 0x04, 0x20, 0x97, 0xE5, 0x09, 0x00, 0xA0, 0xE1, 0x00, 0x20, 0x8D, 0xE5, 0x08, 0x20, 0xA0, 0xE1
	.byte 0x05, 0x30, 0xA0, 0xE1, 0x2D, 0xF3, 0xFF, 0xEB, 0x0E, 0x00, 0x00, 0xEA, 0x08, 0x10, 0x97, 0xE5
	.byte 0x00, 0x00, 0x51, 0xE3, 0x0B, 0x00, 0x00, 0x0A, 0x04, 0x20, 0x97, 0xE5, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x00, 0x20, 0x8D, 0xE5, 0x08, 0x20, 0xA0, 0xE1, 0x04, 0x30, 0xA0, 0xE1, 0x23, 0xF3, 0xFF, 0xEB
	.byte 0x04, 0x00, 0x00, 0xEA, 0x74, 0x20, 0x9F, 0xE5, 0x7C, 0x30, 0x9F, 0xE5, 0x0A, 0x00, 0xA0, 0xE1
	.byte 0x0B, 0x10, 0xA0, 0xE1, 0x18, 0x0F, 0xFA, 0xEB, 0x0C, 0x00, 0xB7, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0xC3, 0xFF, 0xFF, 0x1A, 0x08, 0x20, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0xA0, 0xE1
	.byte 0x53, 0xEF, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x1A, 0x04, 0x00, 0x9D, 0xE5
	.byte 0x1A, 0x1D, 0xFE, 0xEB, 0x28, 0xD0, 0x8D, 0xE2, 0xF8, 0x8F, 0xBD, 0xE8, 0x04, 0x10, 0x9D, 0xE5
	.byte 0x08, 0x00, 0x8D, 0xE2, 0x9E, 0xFA, 0xFF, 0xEB, 0x08, 0x20, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x08, 0x10, 0xA0, 0xE1, 0x81, 0xEF, 0xFF, 0xEB, 0x04, 0x00, 0x9D, 0xE5, 0x0F, 0x1D, 0xFE, 0xEB
	.byte 0x28, 0xD0, 0x8D, 0xE2, 0xF8, 0x8F, 0xBD, 0xE8, 0xF4, 0xA6, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x44, 0x9E, 0x25, 0x02, 0x93, 0x08, 0x00, 0x00, 0x90, 0xA5, 0x25, 0x02, 0xDC, 0x08, 0x00, 0x00
	.byte 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x4C, 0x00, 0x9F, 0xE5, 0x4C, 0x10, 0x9F, 0xE5, 0x4C, 0x20, 0x9F, 0xE5, 0x4C, 0x30, 0x9F, 0xE5
	.byte 0xF1, 0x0E, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x3C, 0x00, 0x9F, 0xE5, 0x2C, 0x10, 0x9F, 0xE5, 0x2C, 0x20, 0x9F, 0xE5, 0x34, 0x30, 0x9F, 0xE5
	.byte 0xE9, 0x0E, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x38, 0x80, 0xBD, 0x18
	.byte 0x20, 0x10, 0x94, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0x10, 0x91, 0xE5, 0x29, 0x0C, 0x00, 0xEB
	.byte 0x38, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x64, 0x9E, 0x25, 0x02
	.byte 0xF1, 0x08, 0x00, 0x00, 0x1C, 0xA6, 0x25, 0x02, 0xF7, 0x08, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x18, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0x98, 0x00, 0x9F, 0xE5, 0x98, 0x10, 0x9F, 0xE5, 0x98, 0x20, 0x9F, 0xE5, 0x98, 0x30, 0x9F, 0xE5
	.byte 0xD1, 0x0E, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x88, 0x00, 0x9F, 0xE5, 0x78, 0x10, 0x9F, 0xE5, 0x78, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5
	.byte 0xC9, 0x0E, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12
	.byte 0x38, 0x80, 0xBD, 0x18, 0x04, 0x08, 0x94, 0xE5, 0x20, 0x10, 0x95, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x18, 0xD0, 0x8D, 0x02, 0x08, 0x20, 0x95, 0xE5, 0x04, 0x00, 0x91, 0xE5, 0x38, 0x80, 0xBD, 0x08
	.byte 0x10, 0x00, 0x8D, 0xE5, 0x14, 0x20, 0x8D, 0xE5, 0x08, 0x18, 0x94, 0xE5, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x00, 0x10, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0xA0, 0xE3
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0x04, 0x28, 0x94, 0xE5, 0x10, 0x30, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x03, 0x10, 0xA0, 0xE3, 0x9C, 0xE1, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xB4, 0x9E, 0x25, 0x02, 0x04, 0x09, 0x00, 0x00
	.byte 0xF0, 0xA5, 0x25, 0x02, 0x0A, 0x09, 0x00, 0x00, 0xF8, 0x4F, 0x2D, 0xE9, 0x40, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x08, 0x00, 0x8D, 0xE5, 0x04, 0x00, 0x51, 0xE3
	.byte 0x00, 0x80, 0xA0, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0xA4, 0x02, 0x9F, 0xE5, 0xA4, 0x12, 0x9F, 0xE5
	.byte 0xA4, 0x22, 0x9F, 0xE5, 0xA4, 0x32, 0x9F, 0xE5, 0x9B, 0x0E, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x04, 0x00, 0x50, 0xE3, 0x40, 0xD0, 0x8D, 0x12, 0xF8, 0x8F, 0xBD, 0x18, 0x20, 0x10, 0x94, 0xE5
	.byte 0x10, 0x20, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3, 0x08, 0x70, 0x91, 0xE5, 0x0C, 0x40, 0x91, 0xE5
	.byte 0x02, 0x30, 0xA0, 0xE1, 0x00, 0x10, 0xA0, 0xE1, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8
	.byte 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0x83, 0xE8
	.byte 0x03, 0x30, 0xA0, 0xE3, 0x01, 0x60, 0xA0, 0xE3, 0x0F, 0x50, 0xA0, 0xE3, 0x08, 0x00, 0x9D, 0xE5
	.byte 0x04, 0x10, 0xA0, 0xE3, 0x10, 0x60, 0x8D, 0xE5, 0x14, 0x70, 0x8D, 0xE5, 0x1C, 0x50, 0x8D, 0xE5
	.byte 0x20, 0x70, 0x8D, 0xE5, 0x28, 0x30, 0x8D, 0xE5, 0x2C, 0x70, 0x8D, 0xE5, 0x34, 0x30, 0x8D, 0xE5
	.byte 0xC8, 0xF3, 0xFF, 0xEB, 0x00, 0x60, 0xB0, 0xE1, 0x40, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08
	.byte 0x00, 0x00, 0x96, 0xE5, 0x18, 0x12, 0x9F, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x1C, 0x80, 0x96, 0x15
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x2B, 0x22, 0xFA, 0xEB, 0x00, 0x90, 0xB0, 0xE1, 0x40, 0xD0, 0x8D, 0x02
	.byte 0xF8, 0x8F, 0xBD, 0x08, 0xFC, 0x01, 0x9F, 0xE5, 0xE8, 0xB1, 0x9F, 0xE5, 0x06, 0x00, 0x40, 0xE2
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0xD0, 0x00, 0xD9, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0xE4, 0x01, 0x9F, 0xE5, 0xD0, 0x21, 0x9F, 0xE5, 0x0C, 0x30, 0x9D, 0xE5, 0x0B, 0x10, 0xA0, 0xE1
	.byte 0x65, 0x0E, 0xFA, 0xEB, 0xD0, 0x00, 0xD9, 0xE1, 0x40, 0x00, 0x50, 0xE3, 0x0A, 0x00, 0x00, 0x1A
	.byte 0xD1, 0x00, 0xD9, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xBC, 0x01, 0x9F, 0xE5
	.byte 0xA4, 0x21, 0x9F, 0xE5, 0xAC, 0x31, 0x9F, 0xE5, 0x0B, 0x10, 0xA0, 0xE1, 0x5A, 0x0E, 0xFA, 0xEB
	.byte 0x02, 0xA0, 0xA0, 0xE3, 0x01, 0x90, 0x89, 0xE2, 0x0E, 0x00, 0x00, 0xEA, 0x2B, 0x00, 0x50, 0xE3
	.byte 0x0B, 0x00, 0x00, 0x1A, 0xD1, 0x00, 0xD9, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A
	.byte 0x80, 0x31, 0x9F, 0xE5, 0x84, 0x01, 0x9F, 0xE5, 0x6C, 0x21, 0x9F, 0xE5, 0x0B, 0x10, 0xA0, 0xE1
	.byte 0x06, 0x30, 0x83, 0xE2, 0x4C, 0x0E, 0xFA, 0xEB, 0x01, 0xA0, 0xA0, 0xE3, 0x01, 0x90, 0x89, 0xE2
	.byte 0x00, 0x00, 0x00, 0xEA, 0x00, 0xA0, 0xA0, 0xE3, 0x00, 0x00, 0x96, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x3C, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x98, 0xE5, 0x04, 0x00, 0x98, 0xE5, 0x01, 0x00, 0x50, 0xE1
	.byte 0x1F, 0x00, 0x00, 0x1A, 0x64, 0x00, 0x81, 0xE2, 0x00, 0x11, 0xA0, 0xE1, 0x08, 0x00, 0x98, 0xE5
	.byte 0x43, 0x1C, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x06, 0x00, 0x00, 0x1A, 0x30, 0x01, 0x9F, 0xE5
	.byte 0x10, 0x11, 0x9F, 0xE5, 0x10, 0x21, 0x9F, 0xE5, 0x28, 0x31, 0x9F, 0xE5, 0x36, 0x0E, 0xFA, 0xEB
	.byte 0x40, 0xD0, 0x8D, 0xE2, 0xF8, 0x8F, 0xBD, 0xE8, 0x08, 0x00, 0x88, 0xE5, 0x0C, 0x00, 0x98, 0xE5
	.byte 0x00, 0x10, 0x98, 0xE5, 0x64, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0x34, 0x1C, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x06, 0x00, 0x00, 0x1A, 0xF4, 0x00, 0x9F, 0xE5, 0xD4, 0x10, 0x9F, 0xE5
	.byte 0xD4, 0x20, 0x9F, 0xE5, 0xF0, 0x30, 0x9F, 0xE5, 0x27, 0x0E, 0xFA, 0xEB, 0x40, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0x0C, 0x00, 0x88, 0xE5, 0x00, 0x00, 0x98, 0xE5, 0x64, 0x00, 0x80, 0xE2
	.byte 0x00, 0x00, 0x88, 0xE5, 0x09, 0x00, 0xA0, 0xE1, 0xD8, 0x20, 0xFA, 0xEB, 0x00, 0x50, 0xA0, 0xE1
	.byte 0x01, 0x00, 0x85, 0xE2, 0x1D, 0x1C, 0xFE, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x06, 0x00, 0x00, 0x1A
	.byte 0xAC, 0x00, 0x9F, 0xE5, 0x8C, 0x10, 0x9F, 0xE5, 0x8C, 0x20, 0x9F, 0xE5, 0xAC, 0x30, 0x9F, 0xE5
	.byte 0x15, 0x0E, 0xFA, 0xEB, 0x40, 0xD0, 0x8D, 0xE2, 0xF8, 0x8F, 0xBD, 0xE8, 0x01, 0x20, 0x85, 0xE2
	.byte 0x09, 0x10, 0xA0, 0xE1, 0x1F, 0x12, 0xFA, 0xEB, 0x03, 0x00, 0x98, 0xE9, 0x00, 0x41, 0x81, 0xE7
	.byte 0x0C, 0x10, 0x98, 0xE5, 0x04, 0x00, 0x98, 0xE5, 0x00, 0xA1, 0x81, 0xE7, 0x04, 0x00, 0x98, 0xE5
	.byte 0x01, 0x00, 0x80, 0xE2, 0x04, 0x00, 0x88, 0xE5, 0x00, 0x00, 0x96, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x0F, 0x00, 0x50, 0x13, 0x07, 0x00, 0x00, 0x1A, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x8D, 0xE5
	.byte 0x04, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x9D, 0xE5, 0x09, 0x10, 0xA0, 0xE1, 0x0A, 0x30, 0xA0, 0xE1
	.byte 0x07, 0x20, 0xA0, 0xE1, 0xBE, 0xEF, 0xFF, 0xEB, 0x24, 0x10, 0x9F, 0xE5, 0x00, 0x00, 0xA0, 0xE3
	.byte 0xB0, 0x21, 0xFA, 0xEB, 0x00, 0x90, 0xB0, 0xE1, 0x89, 0xFF, 0xFF, 0x1A, 0x40, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x88, 0x9F, 0x25, 0x02
	.byte 0x2F, 0x09, 0x00, 0x00, 0x24, 0xA7, 0x25, 0x02, 0x57, 0x09, 0x00, 0x00, 0x28, 0xA7, 0x25, 0x02
	.byte 0x38, 0xA7, 0x25, 0x02, 0x90, 0xA5, 0x25, 0x02, 0x6D, 0x09, 0x00, 0x00, 0x74, 0x09, 0x00, 0x00
	.byte 0x81, 0x09, 0x00, 0x00, 0x70, 0x40, 0x2D, 0xE9, 0x60, 0xD0, 0x4D, 0xE2, 0x00, 0x60, 0xB0, 0xE1
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x9C, 0x01, 0x9F, 0xE5, 0x9C, 0x11, 0x9F, 0xE5
	.byte 0x9C, 0x21, 0x9F, 0xE5, 0x9C, 0x31, 0x9F, 0xE5, 0xDF, 0x0D, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x8C, 0x01, 0x9F, 0xE5, 0x7C, 0x11, 0x9F, 0xE5
	.byte 0x7C, 0x21, 0x9F, 0xE5, 0x84, 0x31, 0x9F, 0xE5, 0xD7, 0x0D, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x60, 0xD0, 0x8D, 0x12, 0x70, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5
	.byte 0x6C, 0x11, 0x9F, 0xE5, 0x04, 0x50, 0x90, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0xF4, 0x20, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x30, 0x20, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3, 0x02, 0x30, 0xA0, 0xE1
	.byte 0x00, 0x10, 0xA0, 0xE1, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8
	.byte 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0x83, 0xE8, 0x00, 0x50, 0xA0, 0x03
	.byte 0x03, 0x30, 0xA0, 0xE3, 0x01, 0xC0, 0xA0, 0xE3, 0x0F, 0x40, 0xA0, 0xE3, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE3, 0x30, 0xC0, 0x8D, 0xE5, 0x34, 0x50, 0x8D, 0xE5, 0x3C, 0x40, 0x8D, 0xE5
	.byte 0x40, 0x50, 0x8D, 0xE5, 0x48, 0x30, 0x8D, 0xE5, 0x4C, 0x50, 0x8D, 0xE5, 0x54, 0x30, 0x8D, 0xE5
	.byte 0x00, 0xF3, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x60, 0xD0, 0x8D, 0x02, 0x70, 0x80, 0xBD, 0x08
	.byte 0x00, 0x00, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x10, 0x00, 0x00, 0x1A, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x24, 0x10, 0x8D, 0xE5, 0x28, 0x00, 0x8D, 0xE5, 0x2C, 0x50, 0x8D, 0xE5
	.byte 0x10, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x02, 0x00, 0x00, 0x1A, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x05, 0x10, 0xA0, 0xE1, 0x9A, 0xEE, 0xFF, 0xEB, 0x24, 0x20, 0x8D, 0xE2, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0xC3, 0xF3, 0xFF, 0xEB, 0x60, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8
	.byte 0x0F, 0x00, 0x50, 0xE3, 0x0D, 0x00, 0x00, 0x1A, 0x1C, 0xC0, 0x94, 0xE5, 0x14, 0x20, 0x8D, 0xE2
	.byte 0x14, 0x50, 0x8D, 0xE5, 0x04, 0x10, 0x9C, 0xE5, 0x06, 0x00, 0xA0, 0xE1, 0x18, 0x10, 0x8D, 0xE5
	.byte 0x08, 0x30, 0x9C, 0xE5, 0x04, 0x10, 0xA0, 0xE1, 0x1C, 0x30, 0x8D, 0xE5, 0x0C, 0x30, 0x9C, 0xE5
	.byte 0x20, 0x30, 0x8D, 0xE5, 0xB3, 0xF3, 0xFF, 0xEB, 0x60, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8
	.byte 0x03, 0x00, 0x50, 0xE3, 0x60, 0xD0, 0x8D, 0x12, 0x70, 0x80, 0xBD, 0x18, 0x1C, 0x30, 0x94, 0xE5
	.byte 0x01, 0x00, 0xA0, 0xE3, 0x04, 0x50, 0x8D, 0xE5, 0x00, 0x00, 0x8D, 0xE5, 0x04, 0x00, 0x93, 0xE5
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x08, 0x00, 0x8D, 0xE5, 0x08, 0x10, 0x93, 0xE5, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x0C, 0x10, 0x8D, 0xE5, 0x0C, 0x30, 0x93, 0xE5, 0x04, 0x10, 0xA0, 0xE1, 0x10, 0x30, 0x8D, 0xE5
	.byte 0xA0, 0xF3, 0xFF, 0xEB, 0x60, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xD8, 0x9F, 0x25, 0x02, 0x9F, 0x09, 0x00, 0x00, 0x48, 0xA7, 0x25, 0x02
	.byte 0xA5, 0x09, 0x00, 0x00, 0x60, 0xA7, 0x25, 0x02, 0x70, 0x40, 0x2D, 0xE9, 0x30, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0xE8, 0x00, 0x9F, 0xE5, 0xE8, 0x10, 0x9F, 0xE5, 0xE8, 0x20, 0x9F, 0xE5
	.byte 0xE8, 0x30, 0x9F, 0xE5, 0x68, 0x0D, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x30, 0xD0, 0x8D, 0x12, 0x70, 0x80, 0xBD, 0x18, 0x20, 0x10, 0x94, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x50, 0x00, 0x91, 0xE9, 0x04, 0x10, 0xA0, 0xE1, 0x06, 0x20, 0xA0, 0xE1, 0xB6, 0xEE, 0xFF, 0xEB
	.byte 0x24, 0x20, 0x8D, 0xE2, 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0xC0, 0x82, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x00, 0xC0, 0x82, 0xE5, 0x02, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5
	.byte 0x24, 0x30, 0x8D, 0xE5, 0x28, 0x40, 0x8D, 0xE5, 0x9E, 0xF2, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x08, 0x00, 0x00, 0x0A, 0x01, 0x30, 0xA0, 0xE3, 0x18, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x18, 0x30, 0x8D, 0xE5, 0x1C, 0x40, 0x8D, 0xE5, 0x20, 0x60, 0x8D, 0xE5, 0x6D, 0xF3, 0xFF, 0xEB
	.byte 0x30, 0xD0, 0x8D, 0xE2, 0x70, 0x80, 0xBD, 0xE8, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x8B, 0xEC, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x14, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13
	.byte 0x30, 0xD0, 0x8D, 0x02, 0x70, 0x80, 0xBD, 0x08, 0x10, 0x40, 0x8D, 0xE5, 0x14, 0x60, 0x8D, 0xE5
	.byte 0x2C, 0x20, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5, 0x12, 0x00, 0x8D, 0xE9
	.byte 0x08, 0x10, 0xA0, 0xE3, 0x0C, 0x10, 0x8D, 0xE5, 0x14, 0x20, 0x90, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x09, 0x10, 0xA0, 0xE3, 0x1F, 0xE0, 0xFF, 0xEB, 0x30, 0xD0, 0x8D, 0xE2
	.byte 0x70, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x4C, 0x9F, 0x25, 0x02
	.byte 0x05, 0x0A, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x30, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1, 0x02, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0xAA
	.byte 0xE0, 0x00, 0x9F, 0xE5, 0xE0, 0x10, 0x9F, 0xE5, 0xE0, 0x20, 0x9F, 0xE5, 0xE0, 0x30, 0x9F, 0xE5
	.byte 0x21, 0x0D, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x02, 0x00, 0x50, 0xE3, 0x30, 0xD0, 0x8D, 0xB2
	.byte 0x38, 0x80, 0xBD, 0xB8, 0x20, 0x00, 0x94, 0xE5, 0x24, 0x20, 0x8D, 0xE2, 0x04, 0x40, 0x90, 0xE5
	.byte 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0xC0, 0x82, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x82, 0xE5
	.byte 0x02, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5, 0x24, 0x30, 0x8D, 0xE5
	.byte 0x28, 0x40, 0x8D, 0xE5, 0x5B, 0xF2, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x09, 0x00, 0x00, 0x0A
	.byte 0x90, 0x30, 0x9F, 0xE5, 0x01, 0xC0, 0xA0, 0xE3, 0x18, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x18, 0xC0, 0x8D, 0xE5, 0x1C, 0x40, 0x8D, 0xE5, 0x20, 0x30, 0x8D, 0xE5, 0x29, 0xF3, 0xFF, 0xEB
	.byte 0x30, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x47, 0xEC, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x14, 0x10, 0x90, 0x15, 0x00, 0x00, 0x51, 0x13
	.byte 0x30, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08, 0x48, 0x10, 0x9F, 0xE5, 0x10, 0x40, 0x8D, 0xE5
	.byte 0x14, 0x10, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5
	.byte 0x12, 0x00, 0x8D, 0xE9, 0x08, 0x10, 0xA0, 0xE3, 0x0C, 0x10, 0x8D, 0xE5, 0x14, 0x20, 0x90, 0xE5
	.byte 0x10, 0x30, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x09, 0x10, 0xA0, 0xE3, 0xDA, 0xDF, 0xFF, 0xEB
	.byte 0x30, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0xF4, 0xA6, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x00, 0xA0, 0x25, 0x02, 0x3C, 0x0A, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02, 0x78, 0x40, 0x2D, 0xE9
	.byte 0x1C, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xF0, 0x00, 0x9F, 0xE5, 0xF0, 0x10, 0x9F, 0xE5, 0xF0, 0x20, 0x9F, 0xE5, 0xF0, 0x30, 0x9F, 0xE5
	.byte 0xDD, 0x0C, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0xE0, 0x00, 0x9F, 0xE5, 0xD0, 0x10, 0x9F, 0xE5, 0xD0, 0x20, 0x9F, 0xE5, 0xD8, 0x30, 0x9F, 0xE5
	.byte 0xD5, 0x0C, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x1C, 0xD0, 0x8D, 0x12
	.byte 0x78, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x10, 0x20, 0x8D, 0xE2, 0x60, 0x00, 0x90, 0xE8
	.byte 0x00, 0x10, 0xA0, 0xE3, 0x04, 0x10, 0x82, 0xE5, 0x08, 0x10, 0x82, 0xE5, 0x00, 0x10, 0x82, 0xE5
	.byte 0x09, 0x30, 0xA0, 0xE3, 0x04, 0x00, 0xA0, 0xE1, 0x01, 0x10, 0xA0, 0xE3, 0x10, 0x30, 0x8D, 0xE5
	.byte 0x14, 0x50, 0x8D, 0xE5, 0x18, 0x60, 0x8D, 0xE5, 0x0E, 0xF2, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x08, 0x00, 0x00, 0x0A, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x30, 0x8D, 0xE5, 0x08, 0x50, 0x8D, 0xE5, 0x0C, 0x60, 0x8D, 0xE5, 0xDD, 0xF2, 0xFF, 0xEB
	.byte 0x1C, 0xD0, 0x8D, 0xE2, 0x78, 0x80, 0xBD, 0xE8, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x54, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0x4C, 0x30, 0x9F, 0xE5, 0xB0, 0x0C, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x1C, 0xD0, 0x8D, 0x02, 0x78, 0x80, 0xBD, 0x08, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x00, 0xA0, 0xE1
	.byte 0xDB, 0x2F, 0x84, 0xE2, 0x01, 0x10, 0xA0, 0xE3, 0x00, 0x30, 0x8D, 0xE5, 0x09, 0x13, 0x00, 0xEB
	.byte 0x1C, 0xD0, 0x8D, 0xE2, 0x78, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA0, 0x25, 0x02, 0x6D, 0x0A, 0x00, 0x00, 0x48, 0xA7, 0x25, 0x02, 0x73, 0x0A, 0x00, 0x00
	.byte 0x64, 0xA7, 0x25, 0x02, 0x93, 0x0A, 0x00, 0x00, 0xF0, 0x47, 0x2D, 0xE9, 0x68, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x90, 0xA0, 0xE1, 0x24, 0x10, 0x99, 0xE5, 0x00, 0xA0, 0xA0, 0xE1, 0x08, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0xE8, 0x01, 0x9F, 0xE5, 0xE8, 0x11, 0x9F, 0xE5, 0xE8, 0x21, 0x9F, 0xE5
	.byte 0xE8, 0x31, 0x9F, 0xE5, 0x90, 0x0C, 0xFA, 0xEB, 0x24, 0x00, 0x99, 0xE5, 0x08, 0x00, 0x50, 0xE3
	.byte 0x68, 0xD0, 0x8D, 0x12, 0xF0, 0x87, 0xBD, 0x18, 0x20, 0x40, 0x99, 0xE5, 0x0A, 0x00, 0xA0, 0xE1
	.byte 0x08, 0x60, 0x94, 0xE5, 0x0C, 0x70, 0x94, 0xE5, 0x14, 0x50, 0x94, 0xE5, 0x06, 0x20, 0xA0, 0xE1
	.byte 0x05, 0x10, 0xA0, 0xE1, 0x07, 0x30, 0xA0, 0xE1, 0x04, 0x40, 0x94, 0xE5, 0x3A, 0xF1, 0xFF, 0xEB
	.byte 0x44, 0x20, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3, 0x02, 0x30, 0xA0, 0xE1, 0x00, 0x10, 0xA0, 0xE1
	.byte 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8, 0x03, 0x00, 0xA3, 0xE8
	.byte 0x00, 0x00, 0x83, 0xE5, 0x06, 0x10, 0xA0, 0xE3, 0x0A, 0x00, 0xA0, 0xE3, 0x44, 0x10, 0x8D, 0xE5
	.byte 0x0B, 0x10, 0xA0, 0xE3, 0x50, 0x00, 0x8D, 0xE5, 0x5C, 0x10, 0x8D, 0xE5, 0x0A, 0x00, 0xA0, 0xE1
	.byte 0x03, 0x10, 0xA0, 0xE3, 0x48, 0x50, 0x8D, 0xE5, 0x4C, 0x40, 0x8D, 0xE5, 0x54, 0x50, 0x8D, 0xE5
	.byte 0x60, 0x40, 0x8D, 0xE5, 0xB7, 0xF1, 0xFF, 0xEB, 0x00, 0x80, 0xB0, 0xE1, 0x68, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x87, 0xBD, 0x08, 0x00, 0x00, 0x98, 0xE5, 0x06, 0x00, 0x50, 0xE3, 0x17, 0x00, 0x00, 0x1A
	.byte 0x20, 0x00, 0x99, 0xE5, 0x40, 0x10, 0xA0, 0xE3, 0x18, 0x70, 0x90, 0xE5, 0x00, 0x60, 0xA0, 0xE3
	.byte 0x07, 0x00, 0xA0, 0xE1, 0xD8, 0x1F, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x02, 0x60, 0x86, 0x13
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x2B, 0x10, 0xA0, 0xE3, 0xD3, 0x1F, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x01, 0x60, 0x86, 0x13, 0x01, 0x30, 0xA0, 0xE3, 0x34, 0x20, 0x8D, 0xE2, 0x0A, 0x00, 0xA0, 0xE1
	.byte 0x08, 0x10, 0xA0, 0xE1, 0x34, 0x30, 0x8D, 0xE5, 0x38, 0x40, 0x8D, 0xE5, 0x3C, 0x50, 0x8D, 0xE5
	.byte 0x40, 0x60, 0x8D, 0xE5, 0x73, 0xF2, 0xFF, 0xEB, 0x68, 0xD0, 0x8D, 0xE2, 0xF0, 0x87, 0xBD, 0xE8
	.byte 0x0A, 0x00, 0x50, 0xE3, 0x15, 0x00, 0x00, 0x1A, 0x01, 0x00, 0xA0, 0xE3, 0x28, 0x50, 0x8D, 0xE5
	.byte 0x2C, 0x60, 0x8D, 0xE5, 0x30, 0x70, 0x8D, 0xE5, 0x24, 0x00, 0x8D, 0xE5, 0x18, 0x00, 0x98, 0xE5
	.byte 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x98, 0xE5, 0x10, 0x00, 0xA0, 0xE3
	.byte 0x04, 0x20, 0x8D, 0xE5, 0x08, 0x10, 0x8D, 0xE5, 0x0C, 0x00, 0x8D, 0xE5, 0x10, 0x20, 0x98, 0xE5
	.byte 0x24, 0x30, 0x8D, 0xE2, 0x0A, 0x00, 0xA0, 0xE1, 0x15, 0x10, 0xA0, 0xE3, 0x26, 0xDF, 0xFF, 0xEB
	.byte 0x00, 0x00, 0xA0, 0xE3, 0x68, 0xD0, 0x8D, 0xE2, 0x10, 0x00, 0x88, 0xE5, 0xF0, 0x87, 0xBD, 0xE8
	.byte 0x0B, 0x00, 0x50, 0xE3, 0x68, 0xD0, 0x8D, 0x12, 0xF0, 0x87, 0xBD, 0x18, 0x10, 0x00, 0x98, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x68, 0xD0, 0x8D, 0x02, 0xF0, 0x87, 0xBD, 0x08, 0x01, 0x00, 0xA0, 0xE3
	.byte 0x10, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x98, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x14, 0x00, 0x8D, 0xE5
	.byte 0x18, 0x50, 0x8D, 0xE5, 0x1C, 0x60, 0x8D, 0xE5, 0x20, 0x70, 0x8D, 0xE5, 0x18, 0x20, 0x98, 0xE5
	.byte 0x14, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5, 0x20, 0x20, 0x98, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x04, 0x20, 0x8D, 0xE5, 0x08, 0x10, 0x8D, 0xE5, 0x0C, 0x00, 0x8D, 0xE5, 0x10, 0x20, 0x98, 0xE5
	.byte 0x0A, 0x00, 0xA0, 0xE1, 0x16, 0x10, 0xA0, 0xE3, 0x07, 0xDF, 0xFF, 0xEB, 0x68, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x87, 0xBD, 0xE8, 0x7C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x40, 0xA0, 0x25, 0x02
	.byte 0xA6, 0x0A, 0x00, 0x00, 0x30, 0x40, 0x2D, 0xE9, 0x3C, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0xE4, 0x00, 0x9F, 0xE5, 0xE4, 0x10, 0x9F, 0xE5, 0xE4, 0x20, 0x9F, 0xE5, 0xE4, 0x30, 0x9F, 0xE5
	.byte 0x09, 0x0C, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x3C, 0xD0, 0x8D, 0x12
	.byte 0x30, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5, 0x24, 0x20, 0x8D, 0xE2, 0x04, 0x40, 0x90, 0xE5
	.byte 0x00, 0xE0, 0xA0, 0xE3, 0x04, 0xE0, 0x82, 0xE5, 0x10, 0xE0, 0x82, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x00, 0xE0, 0x82, 0xE5, 0x0A, 0xC0, 0xA0, 0xE3, 0x0C, 0xE0, 0x82, 0xE5, 0x0B, 0x30, 0xA0, 0xE3
	.byte 0x02, 0x10, 0xA0, 0xE3, 0x08, 0xE0, 0x82, 0xE5, 0x14, 0xE0, 0x82, 0xE5, 0x24, 0xC0, 0x8D, 0xE5
	.byte 0x28, 0x40, 0x8D, 0xE5, 0x30, 0x30, 0x8D, 0xE5, 0x34, 0x40, 0x8D, 0xE5, 0x3D, 0xF1, 0xFF, 0xEB
	.byte 0x00, 0x10, 0xB0, 0xE1, 0x3C, 0xD0, 0x8D, 0x02, 0x30, 0x80, 0xBD, 0x08, 0x00, 0x00, 0x91, 0xE5
	.byte 0x0A, 0x00, 0x50, 0xE3, 0x09, 0x00, 0x00, 0x1A, 0x00, 0x30, 0xA0, 0xE3, 0x14, 0x20, 0x8D, 0xE2
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x14, 0x30, 0x8D, 0xE5, 0x18, 0x40, 0x8D, 0xE5, 0x1C, 0x30, 0x8D, 0xE5
	.byte 0x20, 0x30, 0x8D, 0xE5, 0x07, 0xF2, 0xFF, 0xEB, 0x3C, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8
	.byte 0x0B, 0x00, 0x50, 0xE3, 0x3C, 0xD0, 0x8D, 0x12, 0x30, 0x80, 0xBD, 0x18, 0x00, 0x30, 0xA0, 0xE3
	.byte 0x01, 0xC0, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5
	.byte 0x04, 0x40, 0x8D, 0xE5, 0x08, 0x30, 0x8D, 0xE5, 0x0C, 0x30, 0x8D, 0xE5, 0x10, 0x30, 0x8D, 0xE5
	.byte 0xF8, 0xF1, 0xFF, 0xEB, 0x3C, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0x58, 0xA0, 0x25, 0x02, 0x0D, 0x0B, 0x00, 0x00, 0xF8, 0x40, 0x2D, 0xE9
	.byte 0x00, 0x70, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x9C, 0x00, 0x9F, 0xE5
	.byte 0x9C, 0x10, 0x9F, 0xE5, 0x9C, 0x20, 0x9F, 0xE5, 0x9C, 0x30, 0x9F, 0xE5, 0xC6, 0x0B, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x90, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5
	.byte 0x80, 0x20, 0x9F, 0xE5, 0x88, 0x30, 0x9F, 0xE5, 0xBF, 0x0B, 0xFA, 0xEB, 0x00, 0x00, 0x57, 0xE3
	.byte 0x02, 0x00, 0x00, 0x0A, 0xD0, 0x00, 0xD7, 0xE1, 0x5C, 0x00, 0x50, 0xE3, 0x01, 0x00, 0x00, 0x0A
	.byte 0x00, 0x00, 0xA0, 0xE3, 0xF8, 0x80, 0xBD, 0xE8, 0x00, 0x60, 0xA0, 0xE3, 0x00, 0x00, 0x00, 0xEA
	.byte 0x01, 0x60, 0x86, 0xE2, 0x06, 0x00, 0x87, 0xE0, 0xD1, 0x00, 0xD0, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x5C, 0x00, 0x50, 0x13, 0xF9, 0xFF, 0xFF, 0x1A, 0x01, 0x00, 0x86, 0xE2, 0xAF, 0x19, 0xFE, 0xEB
	.byte 0x00, 0x50, 0xB0, 0xE1, 0x00, 0x00, 0xA0, 0x03, 0xF8, 0x80, 0xBD, 0x08, 0x06, 0x20, 0xA0, 0xE1
	.byte 0x01, 0x10, 0x87, 0xE2, 0xB7, 0x0F, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0x06, 0x00, 0xC5, 0xE7
	.byte 0x01, 0x10, 0x86, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0x10, 0x84, 0xE5, 0xF8, 0x80, 0xBD, 0xE8
	.byte 0x94, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x84, 0x9D, 0x25, 0x02, 0x48, 0x0B, 0x00, 0x00
	.byte 0x9C, 0xA7, 0x25, 0x02, 0x49, 0x0B, 0x00, 0x00, 0xF0, 0x4F, 0x2D, 0xE9, 0x3C, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x10, 0x00, 0x8D, 0xE5, 0x04, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x78, 0x01, 0x9F, 0xE5, 0x78, 0x11, 0x9F, 0xE5, 0x78, 0x21, 0x9F, 0xE5
	.byte 0x78, 0x31, 0x9F, 0xE5, 0x90, 0x0B, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x04, 0x00, 0x50, 0xE3
	.byte 0x3C, 0xD0, 0x8D, 0x12, 0xF0, 0x8F, 0xBD, 0x18, 0x20, 0x10, 0x94, 0xE5, 0x30, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0x91, 0xE5, 0x08, 0x50, 0x91, 0xE5, 0x0C, 0x70, 0x91, 0xE5, 0x00, 0x40, 0xA0, 0xE3
	.byte 0x14, 0x00, 0x8D, 0xE5, 0x04, 0x40, 0x82, 0xE5, 0x10, 0x00, 0x9D, 0xE5, 0x00, 0x40, 0x82, 0xE5
	.byte 0x0C, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x40, 0x82, 0xE5, 0x30, 0x30, 0x8D, 0xE5
	.byte 0x34, 0x50, 0x8D, 0xE5, 0xC7, 0xF0, 0xFF, 0xEB, 0x00, 0x60, 0xB0, 0xE1, 0x3C, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x8F, 0xBD, 0x08, 0x1C, 0xB0, 0x96, 0xE5, 0x00, 0x80, 0x9B, 0xE5, 0x08, 0x01, 0xA0, 0xE1
	.byte 0x76, 0x19, 0xFE, 0xEB, 0x00, 0x90, 0xB0, 0xE1, 0x3C, 0xD0, 0x8D, 0x02, 0xF0, 0x8F, 0xBD, 0x08
	.byte 0x00, 0x00, 0x58, 0xE3, 0x04, 0xA0, 0xA0, 0xE1, 0x10, 0x00, 0x00, 0xDA, 0xF0, 0x40, 0x9F, 0xE5
	.byte 0x18, 0x50, 0x8D, 0xE2, 0x07, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1, 0x9A, 0xFF, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x0A, 0x0A, 0x01, 0x89, 0xE7, 0x18, 0x00, 0x9D, 0xE5
	.byte 0x00, 0x70, 0x87, 0xE0, 0x02, 0x00, 0x00, 0xEA, 0x04, 0x00, 0xA0, 0xE1, 0x6E, 0x65, 0xFE, 0xEB
	.byte 0x0A, 0x01, 0x89, 0xE7, 0x01, 0xA0, 0x8A, 0xE2, 0x08, 0x00, 0x5A, 0xE1, 0xF0, 0xFF, 0xFF, 0xBA
	.byte 0x01, 0x00, 0xA0, 0xE3, 0x1C, 0x00, 0x8D, 0xE5, 0x14, 0x00, 0x9D, 0xE5, 0x24, 0x80, 0x8D, 0xE5
	.byte 0x20, 0x00, 0x8D, 0xE5, 0x04, 0x00, 0x9B, 0xE5, 0x2C, 0x90, 0x8D, 0xE5, 0x28, 0x00, 0x8D, 0xE5
	.byte 0x08, 0x00, 0x9B, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x10, 0x00, 0x9D, 0xE5
	.byte 0x1C, 0x20, 0x8D, 0xE2, 0x06, 0x10, 0xA0, 0xE1, 0x72, 0xF1, 0xFF, 0xEB, 0x0C, 0x00, 0x00, 0xEA
	.byte 0x18, 0x00, 0x96, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x96, 0xE5
	.byte 0x14, 0x00, 0xA0, 0xE3, 0x04, 0x20, 0x8D, 0xE5, 0x08, 0x10, 0x8D, 0xE5, 0x0C, 0x00, 0x8D, 0xE5
	.byte 0x10, 0x20, 0x96, 0xE5, 0x10, 0x00, 0x9D, 0xE5, 0x1C, 0x30, 0x8D, 0xE2, 0x1D, 0x10, 0xA0, 0xE3
	.byte 0x2D, 0xDE, 0xFF, 0xEB, 0x00, 0x00, 0x58, 0xE3, 0x00, 0x40, 0xA0, 0xE3, 0x04, 0x00, 0x00, 0xDA
	.byte 0x04, 0x01, 0x99, 0xE7, 0x49, 0x19, 0xFE, 0xEB, 0x01, 0x40, 0x84, 0xE2, 0x08, 0x00, 0x54, 0xE1
	.byte 0xFA, 0xFF, 0xFF, 0xBA, 0x09, 0x00, 0xA0, 0xE1, 0x44, 0x19, 0xFE, 0xEB, 0x3C, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x8F, 0xBD, 0xE8, 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x60, 0x9F, 0x25, 0x02
	.byte 0x76, 0x0B, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9, 0x20, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x98, 0x00, 0x9F, 0xE5, 0x98, 0x10, 0x9F, 0xE5, 0x98, 0x20, 0x9F, 0xE5
	.byte 0x98, 0x30, 0x9F, 0xE5, 0x24, 0x0B, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x04, 0x00, 0x50, 0xE3
	.byte 0x20, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x14, 0x20, 0x8D, 0xE2
	.byte 0x08, 0xE0, 0x90, 0xE5, 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0xC0, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x00, 0xC0, 0x82, 0xE5, 0x0C, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5
	.byte 0x14, 0x30, 0x8D, 0xE5, 0x18, 0xE0, 0x8D, 0xE5, 0x5E, 0xF0, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x20, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08, 0x1C, 0x30, 0x91, 0xE5, 0x00, 0xC0, 0xA0, 0xE3
	.byte 0x01, 0x00, 0xA0, 0xE3, 0x04, 0xC0, 0x8D, 0xE5, 0x00, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x93, 0xE5
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x08, 0x00, 0x8D, 0xE5, 0x04, 0x30, 0x93, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x0C, 0x30, 0x8D, 0xE5, 0x10, 0xC0, 0x8D, 0xE5, 0x26, 0xF1, 0xFF, 0xEB, 0x20, 0xD0, 0x8D, 0xE2
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x88, 0xA0, 0x25, 0x02
	.byte 0xCC, 0x0B, 0x00, 0x00, 0xF0, 0x4F, 0x2D, 0xE9, 0x54, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x94, 0xE5, 0x10, 0x00, 0x8D, 0xE5, 0x05, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x60, 0x03, 0x9F, 0xE5, 0x60, 0x13, 0x9F, 0xE5, 0x60, 0x23, 0x9F, 0xE5, 0x60, 0x33, 0x9F, 0xE5
	.byte 0xF1, 0x0A, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x05, 0x00, 0x50, 0xE3, 0x54, 0xD0, 0x8D, 0x12
	.byte 0xF0, 0x8F, 0xBD, 0x18, 0x20, 0x30, 0x94, 0xE5, 0x48, 0x13, 0x9F, 0xE5, 0x0C, 0x50, 0x93, 0xE5
	.byte 0x08, 0x20, 0x93, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0xB0, 0x93, 0xE5, 0x14, 0x20, 0x8D, 0xE5
	.byte 0x10, 0x80, 0x93, 0xE5, 0x0A, 0x1E, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x3D, 0x00, 0x00, 0x1A
	.byte 0x10, 0x00, 0x9D, 0xE5, 0x0B, 0x10, 0xA0, 0xE1, 0x25, 0xEA, 0xFF, 0xEB, 0x00, 0x70, 0xB0, 0xE1
	.byte 0x28, 0x00, 0x97, 0x15, 0x00, 0x00, 0x50, 0x13, 0x54, 0xD0, 0x8D, 0x02, 0xF0, 0x8F, 0xBD, 0x08
	.byte 0x38, 0xA0, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x8A, 0xE5, 0x04, 0x00, 0x8A, 0xE5
	.byte 0x08, 0x00, 0x8A, 0xE5, 0x0C, 0x00, 0x8A, 0xE5, 0x14, 0x00, 0x9D, 0xE5, 0x38, 0xB0, 0x8D, 0xE5
	.byte 0x3C, 0x00, 0x8D, 0xE5, 0xD0, 0x00, 0xD8, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x54, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x8F, 0xBD, 0x08, 0x00, 0x60, 0xA0, 0xE3, 0x06, 0x50, 0xA0, 0xE1, 0x10, 0x40, 0xA0, 0xE3
	.byte 0xC4, 0x12, 0x9F, 0xE5, 0x08, 0x00, 0xA0, 0xE1, 0xD4, 0x1E, 0xFA, 0xEB, 0x00, 0x80, 0xA0, 0xE1
	.byte 0x00, 0x00, 0x00, 0xEA, 0x01, 0x80, 0x88, 0xE2, 0xD0, 0x10, 0xD8, 0xE1, 0x00, 0x00, 0x51, 0xE3
	.byte 0x5C, 0x00, 0x51, 0x13, 0xFA, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0x51, 0xE3, 0x54, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x8F, 0xBD, 0x08, 0x01, 0x60, 0xC8, 0xE4, 0x08, 0x10, 0xA0, 0xE1, 0x00, 0x00, 0x00, 0xEA
	.byte 0x01, 0x80, 0x88, 0xE2, 0xD0, 0x90, 0xD8, 0xE1, 0x00, 0x00, 0x59, 0xE3, 0x5C, 0x00, 0x59, 0x13
	.byte 0xFA, 0xFF, 0xFF, 0x1A, 0x00, 0x50, 0xC8, 0xE5, 0x40, 0x00, 0x8D, 0xE5, 0x44, 0x10, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x97, 0xE5, 0x10, 0x00, 0x9D, 0xE5, 0x22, 0x08, 0x8D, 0xE8, 0x0C, 0x40, 0x8D, 0xE5
	.byte 0x28, 0x20, 0x97, 0xE5, 0x1C, 0x10, 0xA0, 0xE3, 0x0A, 0x30, 0xA0, 0xE1, 0x96, 0xDD, 0xFF, 0xEB
	.byte 0x00, 0x90, 0xC8, 0xE5, 0xD0, 0x00, 0xD8, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0xDB, 0xFF, 0xFF, 0x1A
	.byte 0x54, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8, 0x48, 0x20, 0x8D, 0xE2, 0x00, 0x40, 0xA0, 0xE3
	.byte 0x04, 0x40, 0x82, 0xE5, 0x10, 0x00, 0x9D, 0xE5, 0x00, 0x40, 0x82, 0xE5, 0x0D, 0x30, 0xA0, 0xE3
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x40, 0x82, 0xE5, 0x48, 0x30, 0x8D, 0xE5, 0x4C, 0x50, 0x8D, 0xE5
	.byte 0xE4, 0xEF, 0xFF, 0xEB, 0x18, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x54, 0xD0, 0x8D, 0x02
	.byte 0xF0, 0x8F, 0xBD, 0x08, 0x1C, 0x60, 0x90, 0xE5, 0x00, 0x50, 0x96, 0xE5, 0x05, 0x01, 0xA0, 0xE1
	.byte 0x92, 0x18, 0xFE, 0xEB, 0x00, 0x70, 0xB0, 0xE1, 0x54, 0xD0, 0x8D, 0x02, 0xF0, 0x8F, 0xBD, 0x08
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x90, 0xA0, 0xE1, 0x0A, 0x00, 0x00, 0xDA, 0x1C, 0x40, 0x8D, 0xE2
	.byte 0x08, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0xB7, 0xFE, 0xFF, 0xEB, 0x09, 0x01, 0x87, 0xE7
	.byte 0x00, 0x00, 0x50, 0xE3, 0x1C, 0x00, 0x9D, 0x15, 0x01, 0x90, 0x89, 0xE2, 0x00, 0x80, 0x88, 0x10
	.byte 0x05, 0x00, 0x59, 0xE1, 0xF5, 0xFF, 0xFF, 0xBA, 0x0C, 0x00, 0x96, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x33, 0x00, 0x00, 0x0A, 0x1C, 0x10, 0x8D, 0xE2, 0x08, 0x00, 0xA0, 0xE1, 0xAA, 0xFE, 0xFF, 0xEB
	.byte 0x00, 0x90, 0xB0, 0xE1, 0x2D, 0x00, 0x00, 0x0A, 0x1C, 0x00, 0x9D, 0xE5, 0x1C, 0x10, 0x8D, 0xE2
	.byte 0x00, 0x80, 0x88, 0xE0, 0x08, 0x00, 0xA0, 0xE1, 0xA3, 0xFE, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x1E, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x85, 0xE2, 0x00, 0xA1, 0xA0, 0xE1, 0x1C, 0x10, 0x9D, 0xE5
	.byte 0x04, 0x00, 0x96, 0xE5, 0x01, 0x80, 0x88, 0xE0, 0x0A, 0x10, 0xA0, 0xE1, 0x70, 0x18, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x10, 0x00, 0x00, 0x0A, 0x04, 0x00, 0x86, 0xE5, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x0A, 0x10, 0xA0, 0xE1, 0x6A, 0x18, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x0A
	.byte 0x04, 0x10, 0x96, 0xE5, 0x00, 0x70, 0xA0, 0xE1, 0x05, 0x91, 0x81, 0xE7, 0x05, 0x41, 0x80, 0xE7
	.byte 0x01, 0x50, 0x85, 0xE2, 0x0C, 0x00, 0x00, 0xEA, 0x09, 0x00, 0xA0, 0xE1, 0x67, 0x18, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x65, 0x18, 0xFE, 0xEB, 0x07, 0x00, 0x00, 0xEA, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x62, 0x18, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0x60, 0x18, 0xFE, 0xEB, 0x02, 0x00, 0x00, 0xEA
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x5D, 0x18, 0xFE, 0xEB, 0x04, 0x00, 0x00, 0xEA, 0x08, 0x00, 0xA0, 0xE1
	.byte 0x1C, 0x10, 0x8D, 0xE2, 0x7C, 0xFE, 0xFF, 0xEB, 0x00, 0x90, 0xB0, 0xE1, 0xD1, 0xFF, 0xFF, 0x1A
	.byte 0x00, 0x50, 0x86, 0xE5, 0x01, 0x00, 0xA0, 0xE3, 0x20, 0x00, 0x8D, 0xE5, 0x14, 0x00, 0x9D, 0xE5
	.byte 0x24, 0xB0, 0x8D, 0xE5, 0x2C, 0x50, 0x8D, 0xE5, 0x28, 0x00, 0x8D, 0xE5, 0x04, 0x00, 0x96, 0xE5
	.byte 0x34, 0x70, 0x8D, 0xE5, 0x30, 0x00, 0x8D, 0xE5, 0x08, 0x00, 0x96, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x10, 0x00, 0x9D, 0xE5, 0x18, 0x10, 0x9D, 0xE5, 0x20, 0x20, 0x8D, 0xE2
	.byte 0x5C, 0xF0, 0xFF, 0xEB, 0x0F, 0x00, 0x00, 0xEA, 0x18, 0x00, 0x9D, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x18, 0x00, 0x90, 0xE5, 0x20, 0x30, 0x8D, 0xE2, 0x00, 0x00, 0x8D, 0xE5, 0x18, 0x00, 0x9D, 0xE5
	.byte 0x20, 0x20, 0x90, 0xE5, 0x18, 0x00, 0xA0, 0xE3, 0x04, 0x20, 0x8D, 0xE5, 0x08, 0x10, 0x8D, 0xE5
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0x18, 0x00, 0x9D, 0xE5, 0x1E, 0x10, 0xA0, 0xE3, 0x10, 0x20, 0x90, 0xE5
	.byte 0x10, 0x00, 0x9D, 0xE5, 0x14, 0xDD, 0xFF, 0xEB, 0x00, 0x00, 0x55, 0xE3, 0x00, 0x40, 0xA0, 0xE3
	.byte 0x04, 0x00, 0x00, 0xDA, 0x04, 0x01, 0x97, 0xE7, 0x30, 0x18, 0xFE, 0xEB, 0x01, 0x40, 0x84, 0xE2
	.byte 0x05, 0x00, 0x54, 0xE1, 0xFA, 0xFF, 0xFF, 0xBA, 0x07, 0x00, 0xA0, 0xE1, 0x2B, 0x18, 0xFE, 0xEB
	.byte 0x54, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8, 0xA0, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xC4, 0x9F, 0x25, 0x02, 0xFC, 0x0B, 0x00, 0x00, 0xB8, 0xA7, 0x25, 0x02, 0xC0, 0xA7, 0x25, 0x02
	.byte 0x30, 0x40, 0x2D, 0xE9, 0x24, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5
	.byte 0x00, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x9C, 0x00, 0x9F, 0xE5
	.byte 0x9C, 0x10, 0x9F, 0xE5, 0x9C, 0x20, 0x9F, 0xE5, 0xCB, 0x3E, 0xA0, 0xE3, 0x0A, 0x0A, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x94, 0xE5, 0x04, 0x00, 0x50, 0xE3, 0x24, 0xD0, 0x8D, 0x12, 0x30, 0x80, 0xBD, 0x18
	.byte 0x20, 0x00, 0x94, 0xE5, 0x18, 0x20, 0x8D, 0xE2, 0x10, 0x40, 0x90, 0xE9, 0x00, 0xC0, 0xA0, 0xE3
	.byte 0x04, 0xC0, 0x82, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x82, 0xE5, 0x0D, 0x30, 0xA0, 0xE3
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5, 0x18, 0x30, 0x8D, 0xE5, 0x1C, 0xE0, 0x8D, 0xE5
	.byte 0x44, 0xEF, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x24, 0xD0, 0x8D, 0x02, 0x30, 0x80, 0xBD, 0x08
	.byte 0x1C, 0x30, 0x91, 0xE5, 0x00, 0xC0, 0xA0, 0xE3, 0x01, 0x00, 0xA0, 0xE3, 0x04, 0x40, 0x8D, 0xE5
	.byte 0x08, 0xC0, 0x8D, 0xE5, 0x00, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x93, 0xE5, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x0C, 0x00, 0x8D, 0xE5, 0x04, 0x30, 0x93, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x10, 0x30, 0x8D, 0xE5
	.byte 0x14, 0xC0, 0x8D, 0xE5, 0x0B, 0xF0, 0xFF, 0xEB, 0x24, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8
	.byte 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xE8, 0xA0, 0x25, 0x02, 0xF8, 0x4F, 0x2D, 0xE9
	.byte 0x58, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x10, 0x00, 0x8D, 0xE5
	.byte 0x04, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x44, 0x04, 0x9F, 0xE5, 0x44, 0x14, 0x9F, 0xE5
	.byte 0x44, 0x24, 0x9F, 0xE5, 0x44, 0x34, 0x9F, 0xE5, 0xD7, 0x09, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x04, 0x00, 0x50, 0xE3, 0x58, 0xD0, 0x8D, 0x12, 0xF8, 0x8F, 0xBD, 0x18, 0x20, 0x20, 0x94, 0xE5
	.byte 0x2C, 0x14, 0x9F, 0xE5, 0x21, 0x00, 0x92, 0xE9, 0x18, 0x00, 0x8D, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x0C, 0x90, 0x92, 0xE5, 0xF2, 0x1C, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x40, 0x00, 0x00, 0x1A
	.byte 0x10, 0x00, 0x9D, 0xE5, 0x18, 0x10, 0x9D, 0xE5, 0x0D, 0xE9, 0xFF, 0xEB, 0x00, 0x80, 0xB0, 0xE1
	.byte 0x28, 0x00, 0x98, 0x15, 0x00, 0x00, 0x50, 0x13, 0x58, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08
	.byte 0x3C, 0x70, 0x8D, 0xE2, 0x00, 0x10, 0xA0, 0xE3, 0x04, 0x10, 0x87, 0xE5, 0x18, 0x00, 0x9D, 0xE5
	.byte 0x00, 0x10, 0x87, 0xE5, 0x08, 0x10, 0x87, 0xE5, 0x0C, 0x10, 0x87, 0xE5, 0x40, 0x10, 0x8D, 0xE5
	.byte 0x3C, 0x00, 0x8D, 0xE5, 0xD0, 0x00, 0xD9, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x58, 0xD0, 0x8D, 0x02
	.byte 0xF8, 0x8F, 0xBD, 0x08, 0xBC, 0xB3, 0x9F, 0xE5, 0x01, 0x60, 0xA0, 0xE1, 0x01, 0x50, 0xA0, 0xE1
	.byte 0x10, 0x40, 0xA0, 0xE3, 0x09, 0x00, 0xA0, 0xE1, 0x0B, 0x10, 0xA0, 0xE1, 0xBB, 0x1D, 0xFA, 0xEB
	.byte 0x00, 0x90, 0xA0, 0xE1, 0x00, 0x00, 0x00, 0xEA, 0x01, 0x90, 0x89, 0xE2, 0xD0, 0x10, 0xD9, 0xE1
	.byte 0x00, 0x00, 0x51, 0xE3, 0x5C, 0x00, 0x51, 0x13, 0xFA, 0xFF, 0xFF, 0x1A, 0x00, 0x00, 0x51, 0xE3
	.byte 0x58, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08, 0x01, 0x60, 0xC9, 0xE4, 0x09, 0x10, 0xA0, 0xE1
	.byte 0x00, 0x00, 0x00, 0xEA, 0x01, 0x90, 0x89, 0xE2, 0xD0, 0xA0, 0xD9, 0xE1, 0x00, 0x00, 0x5A, 0xE3
	.byte 0x5C, 0x00, 0x5A, 0x13, 0xFA, 0xFF, 0xFF, 0x1A, 0x00, 0x50, 0xC9, 0xE5, 0x44, 0x00, 0x8D, 0xE5
	.byte 0x48, 0x10, 0x8D, 0xE5, 0x2C, 0x10, 0x98, 0xE5, 0x10, 0x00, 0x9D, 0xE5, 0x22, 0x00, 0x8D, 0xE8
	.byte 0x18, 0x10, 0x9D, 0xE5, 0x07, 0x30, 0xA0, 0xE1, 0x08, 0x10, 0x8D, 0xE5, 0x0C, 0x40, 0x8D, 0xE5
	.byte 0x28, 0x20, 0x98, 0xE5, 0x1C, 0x10, 0xA0, 0xE3, 0x7B, 0xDC, 0xFF, 0xEB, 0x00, 0xA0, 0xC9, 0xE5
	.byte 0xD0, 0x00, 0xD9, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0xD9, 0xFF, 0xFF, 0x1A, 0x58, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0x4C, 0x20, 0x8D, 0xE2, 0x00, 0x40, 0xA0, 0xE3, 0x04, 0x40, 0x82, 0xE5
	.byte 0x10, 0x00, 0x9D, 0xE5, 0x00, 0x40, 0x82, 0xE5, 0x0E, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x08, 0x40, 0x82, 0xE5, 0x4C, 0x30, 0x8D, 0xE5, 0x50, 0x50, 0x8D, 0xE5, 0xC9, 0xEE, 0xFF, 0xEB
	.byte 0x1C, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x58, 0xD0, 0x8D, 0x02, 0xF8, 0x8F, 0xBD, 0x08
	.byte 0x1C, 0x40, 0x90, 0xE5, 0x00, 0xA0, 0x94, 0xE5, 0x00, 0x00, 0x5A, 0xE3, 0x4B, 0x00, 0x00, 0x0A
	.byte 0x0A, 0x01, 0xA0, 0xE1, 0x75, 0x17, 0xFE, 0xEB, 0x00, 0x50, 0xB0, 0xE1, 0x58, 0xD0, 0x8D, 0x02
	.byte 0xF8, 0x8F, 0xBD, 0x08, 0x00, 0x00, 0x5A, 0xE3, 0x00, 0x70, 0xA0, 0xE3, 0x0A, 0x00, 0x00, 0xDA
	.byte 0x20, 0x60, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1, 0x06, 0x10, 0xA0, 0xE1, 0x9A, 0xFD, 0xFF, 0xEB
	.byte 0x07, 0x01, 0x85, 0xE7, 0x00, 0x00, 0x50, 0xE3, 0x20, 0x00, 0x9D, 0x15, 0x01, 0x70, 0x87, 0xE2
	.byte 0x00, 0x90, 0x89, 0x10, 0x0A, 0x00, 0x57, 0xE1, 0xF5, 0xFF, 0xFF, 0xBA, 0x08, 0x00, 0x94, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x7C, 0x00, 0x00, 0x0A, 0x20, 0x10, 0x8D, 0xE2, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x8D, 0xFD, 0xFF, 0xEB, 0x00, 0x70, 0xB0, 0xE1, 0x2E, 0x00, 0x00, 0x0A, 0x20, 0xB0, 0x8D, 0xE2
	.byte 0x20, 0x00, 0x9D, 0xE5, 0x0B, 0x10, 0xA0, 0xE1, 0x00, 0x90, 0x89, 0xE0, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x85, 0xFD, 0xFF, 0xEB, 0x00, 0x60, 0xB0, 0xE1, 0x1E, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x8A, 0xE2
	.byte 0x00, 0x81, 0xA0, 0xE1, 0x20, 0x20, 0x9D, 0xE5, 0x04, 0x00, 0x94, 0xE5, 0x08, 0x10, 0xA0, 0xE1
	.byte 0x02, 0x90, 0x89, 0xE0, 0x52, 0x17, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x10, 0x00, 0x00, 0x0A
	.byte 0x04, 0x00, 0x84, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0xA0, 0xE1, 0x4C, 0x17, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x0A, 0x04, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1
	.byte 0x0A, 0x71, 0x81, 0xE7, 0x0A, 0x61, 0x80, 0xE7, 0x01, 0xA0, 0x8A, 0xE2, 0x0C, 0x00, 0x00, 0xEA
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x49, 0x17, 0xFE, 0xEB, 0x06, 0x00, 0xA0, 0xE1, 0x47, 0x17, 0xFE, 0xEB
	.byte 0x07, 0x00, 0x00, 0xEA, 0x07, 0x00, 0xA0, 0xE1, 0x44, 0x17, 0xFE, 0xEB, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x42, 0x17, 0xFE, 0xEB, 0x02, 0x00, 0x00, 0xEA, 0x07, 0x00, 0xA0, 0xE1, 0x3F, 0x17, 0xFE, 0xEB
	.byte 0x04, 0x00, 0x00, 0xEA, 0x09, 0x00, 0xA0, 0xE1, 0x0B, 0x10, 0xA0, 0xE1, 0x5E, 0xFD, 0xFF, 0xEB
	.byte 0x00, 0x70, 0xB0, 0xE1, 0xD1, 0xFF, 0xFF, 0x1A, 0x00, 0xA0, 0x84, 0xE5, 0x46, 0x00, 0x00, 0xEA
	.byte 0x00, 0x60, 0xA0, 0xE3, 0x06, 0x50, 0xA0, 0xE1, 0x06, 0xA0, 0xA0, 0xE1, 0x20, 0x10, 0x8D, 0xE2
	.byte 0x09, 0x00, 0xA0, 0xE1, 0x54, 0xFD, 0xFF, 0xEB, 0x00, 0xB0, 0xB0, 0xE1, 0x3C, 0x00, 0x00, 0x0A
	.byte 0x20, 0x00, 0x9D, 0xE5, 0x20, 0x10, 0x8D, 0xE2, 0x00, 0x90, 0x89, 0xE0, 0x09, 0x00, 0xA0, 0xE1
	.byte 0x4D, 0xFD, 0xFF, 0xEB, 0x14, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x02, 0x00, 0x00, 0x1A
	.byte 0x0B, 0x00, 0xA0, 0xE1, 0x25, 0x17, 0xFE, 0xEB, 0x31, 0x00, 0x00, 0xEA, 0x01, 0x00, 0x8A, 0xE2
	.byte 0x00, 0x71, 0xA0, 0xE1, 0x20, 0x20, 0x9D, 0xE5, 0x06, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE1
	.byte 0x02, 0x90, 0x89, 0xE0, 0x16, 0x17, 0xFE, 0xEB, 0x07, 0x10, 0xA0, 0xE1, 0x00, 0x70, 0xA0, 0xE1
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x12, 0x17, 0xFE, 0xEB, 0x00, 0x80, 0xA0, 0xE1, 0x00, 0x00, 0x57, 0xE3
	.byte 0x00, 0x00, 0x58, 0x13, 0x1B, 0x00, 0x00, 0x1A, 0x0B, 0x00, 0xA0, 0xE1, 0x13, 0x17, 0xFE, 0xEB
	.byte 0x14, 0x00, 0x9D, 0xE5, 0x11, 0x17, 0xFE, 0xEB, 0x00, 0x00, 0x5A, 0xE3, 0x01, 0xA0, 0x4A, 0xE2
	.byte 0x06, 0x00, 0x00, 0x0A, 0x0A, 0x01, 0x96, 0xE7, 0x0C, 0x17, 0xFE, 0xEB, 0x0A, 0x01, 0x95, 0xE7
	.byte 0x0A, 0x17, 0xFE, 0xEB, 0x00, 0x00, 0x5A, 0xE3, 0x01, 0xA0, 0x4A, 0xE2, 0xF8, 0xFF, 0xFF, 0x1A
	.byte 0x00, 0x00, 0x57, 0xE3, 0x02, 0x00, 0x00, 0x0A, 0x07, 0x00, 0xA0, 0xE1, 0x03, 0x17, 0xFE, 0xEB
	.byte 0x01, 0x00, 0x00, 0xEA, 0x06, 0x00, 0xA0, 0xE1, 0x00, 0x17, 0xFE, 0xEB, 0x00, 0x00, 0x58, 0xE3
	.byte 0x02, 0x00, 0x00, 0x0A, 0x08, 0x00, 0xA0, 0xE1, 0xFC, 0x16, 0xFE, 0xEB, 0x01, 0x00, 0x00, 0xEA
	.byte 0x05, 0x00, 0xA0, 0xE1, 0xF9, 0x16, 0xFE, 0xEB, 0x14, 0x00, 0x9D, 0xE5, 0x0A, 0xB1, 0x87, 0xE7
	.byte 0x0A, 0x01, 0x88, 0xE7, 0x07, 0x60, 0xA0, 0xE1, 0x08, 0x50, 0xA0, 0xE1, 0x01, 0xA0, 0x8A, 0xE2
	.byte 0xBD, 0xFF, 0xFF, 0xEA, 0x00, 0xA0, 0x84, 0xE5, 0x04, 0x60, 0x84, 0xE5, 0x01, 0x00, 0xA0, 0xE3
	.byte 0x24, 0x00, 0x8D, 0xE5, 0x18, 0x00, 0x9D, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x28, 0x00, 0x8D, 0xE5
	.byte 0x2C, 0x10, 0x8D, 0xE5, 0x30, 0xA0, 0x8D, 0xE5, 0x04, 0x30, 0x94, 0xE5, 0x10, 0x00, 0x9D, 0xE5
	.byte 0x1C, 0x10, 0x9D, 0xE5, 0x24, 0x20, 0x8D, 0xE2, 0x34, 0x30, 0x8D, 0xE5, 0x38, 0x50, 0x8D, 0xE5
	.byte 0xF8, 0xEE, 0xFF, 0xEB, 0x00, 0x00, 0x5A, 0xE3, 0x00, 0x40, 0xA0, 0xE3, 0x04, 0x00, 0x00, 0xDA
	.byte 0x04, 0x01, 0x95, 0xE7, 0xDD, 0x16, 0xFE, 0xEB, 0x01, 0x40, 0x84, 0xE2, 0x0A, 0x00, 0x54, 0xE1
	.byte 0xFA, 0xFF, 0xFF, 0xBA, 0x05, 0x00, 0xA0, 0xE1, 0xD8, 0x16, 0xFE, 0xEB, 0x58, 0xD0, 0x8D, 0xE2
	.byte 0xF8, 0x8F, 0xBD, 0xE8, 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x00, 0xA1, 0x25, 0x02
	.byte 0xE1, 0x0C, 0x00, 0x00, 0xB8, 0xA7, 0x25, 0x02, 0xC0, 0xA7, 0x25, 0x02, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x80, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xAC, 0x00, 0x9F, 0xE5, 0xAC, 0x10, 0x9F, 0xE5, 0xAC, 0x20, 0x9F, 0xE5, 0xAC, 0x30, 0x9F, 0xE5
	.byte 0xB9, 0x08, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0xAA
	.byte 0x9C, 0x00, 0x9F, 0xE5, 0x8C, 0x10, 0x9F, 0xE5, 0x8C, 0x20, 0x9F, 0xE5, 0x94, 0x30, 0x9F, 0xE5
	.byte 0xB1, 0x08, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x80, 0xD0, 0x8D, 0xB2
	.byte 0x38, 0x80, 0xBD, 0xB8, 0x20, 0x10, 0x95, 0xE5, 0x01, 0x00, 0x40, 0xE2, 0x00, 0x01, 0x91, 0xE7
	.byte 0x40, 0x10, 0xA0, 0xE3, 0x20, 0x1C, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x10, 0x10, 0x94, 0x15
	.byte 0x00, 0x00, 0x51, 0x13, 0x0E, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x80, 0xE2, 0x4A, 0x64, 0xFE, 0xEB
	.byte 0x00, 0x10, 0xA0, 0xE1, 0x18, 0x30, 0x94, 0xE5, 0x10, 0xC0, 0x94, 0xE5, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x3C, 0xFF, 0x2F, 0xE1, 0x2C, 0x00, 0x84, 0xE2, 0x00, 0x10, 0x8D, 0xE2
	.byte 0x01, 0x0B, 0x80, 0xE2, 0x80, 0x20, 0xA0, 0xE3, 0x89, 0x1B, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3
	.byte 0xAB, 0x04, 0xC4, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x9B, 0x06, 0x00, 0xEB, 0x80, 0xD0, 0x8D, 0xE2
	.byte 0x38, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x74, 0x9F, 0x25, 0x02
	.byte 0xBD, 0x0D, 0x00, 0x00, 0xC4, 0xA7, 0x25, 0x02, 0xC3, 0x0D, 0x00, 0x00, 0x18, 0x40, 0x2D, 0xE9
	.byte 0x0C, 0xD0, 0x4D, 0xE2, 0x00, 0x20, 0x8D, 0xE2, 0x00, 0x30, 0xA0, 0xE3, 0x00, 0x30, 0x82, 0xE5
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x08, 0x30, 0x82, 0xE5, 0x00, 0x30, 0x8D, 0xE5
	.byte 0xCC, 0xED, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x0C, 0xD0, 0x8D, 0x02, 0x18, 0x80, 0xBD, 0x08
	.byte 0x1C, 0x40, 0x90, 0xE5, 0x00, 0x00, 0x54, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x1C, 0x00, 0x9F, 0xE5
	.byte 0x1C, 0x10, 0x9F, 0xE5, 0x1C, 0x20, 0x9F, 0xE5, 0x1C, 0x30, 0x9F, 0xE5, 0x76, 0x08, 0xFA, 0xEB
	.byte 0x01, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x84, 0xE5, 0x0C, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8
	.byte 0xDC, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xA0, 0xA0, 0x25, 0x02, 0xEF, 0x0D, 0x00, 0x00
	.byte 0xF0, 0x4F, 0x2D, 0xE9, 0x34, 0xD0, 0x4D, 0xE2, 0x01, 0x90, 0xA0, 0xE1, 0x24, 0x10, 0x99, 0xE5
	.byte 0x00, 0xA0, 0xA0, 0xE1, 0x04, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x10, 0x02, 0x9F, 0xE5
	.byte 0x10, 0x12, 0x9F, 0xE5, 0x10, 0x22, 0x9F, 0xE5, 0x10, 0x32, 0x9F, 0xE5, 0x62, 0x08, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x99, 0xE5, 0x04, 0x00, 0x50, 0xE3, 0x34, 0xD0, 0x8D, 0x12, 0xF0, 0x8F, 0xBD, 0x18
	.byte 0x28, 0x20, 0x8D, 0xE2, 0x00, 0x30, 0xA0, 0xE3, 0x00, 0x30, 0x82, 0xE5, 0x0A, 0x00, 0xA0, 0xE1
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x08, 0x30, 0x82, 0xE5, 0x28, 0x30, 0x8D, 0xE5
	.byte 0xA0, 0xED, 0xFF, 0xEB, 0x00, 0x70, 0xB0, 0xE1, 0x10, 0x00, 0x97, 0x15, 0x00, 0x00, 0x50, 0x13
	.byte 0x34, 0xD0, 0x8D, 0x02, 0xF0, 0x8F, 0xBD, 0x08, 0x1C, 0x80, 0x97, 0xE5, 0x00, 0x00, 0x58, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0xB8, 0x01, 0x9F, 0xE5, 0xA8, 0x11, 0x9F, 0xE5, 0xA8, 0x21, 0x9F, 0xE5
	.byte 0xB0, 0x31, 0x9F, 0xE5, 0x48, 0x08, 0xFA, 0xEB, 0x20, 0x00, 0x99, 0xE5, 0x04, 0x00, 0x90, 0xE5
	.byte 0xFE, 0x1A, 0xFA, 0xEB, 0x00, 0x40, 0xA0, 0xE1, 0x01, 0x00, 0x84, 0xE2, 0x43, 0x16, 0xFE, 0xEB
	.byte 0x00, 0x60, 0xB0, 0xE1, 0x34, 0xD0, 0x8D, 0x02, 0xF0, 0x8F, 0xBD, 0x08, 0x20, 0x10, 0x99, 0xE5
	.byte 0x01, 0x20, 0x84, 0xE2, 0x04, 0x10, 0x91, 0xE5, 0x4A, 0x0C, 0xFA, 0xEB, 0x20, 0x00, 0x99, 0xE5
	.byte 0x08, 0x00, 0x90, 0xE5, 0x9C, 0x23, 0xFA, 0xEB, 0x20, 0x10, 0x99, 0xE5, 0x00, 0x50, 0xA0, 0xE1
	.byte 0x0C, 0x00, 0x91, 0xE5, 0xED, 0x1A, 0xFA, 0xEB, 0x00, 0xB0, 0xA0, 0xE1, 0x01, 0x00, 0x8B, 0xE2
	.byte 0x32, 0x16, 0xFE, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x03, 0x00, 0x00, 0x1A, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x3A, 0x16, 0xFE, 0xEB, 0x34, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8, 0x20, 0x10, 0x99, 0xE5
	.byte 0x01, 0x20, 0x8B, 0xE2, 0x0C, 0x10, 0x91, 0xE5, 0x36, 0x0C, 0xFA, 0xEB, 0x04, 0x90, 0x98, 0xE5
	.byte 0x01, 0x00, 0xA0, 0xE3, 0x14, 0x90, 0x8D, 0xE5, 0x18, 0x60, 0x8D, 0xE5, 0x1C, 0x40, 0x8D, 0xE5
	.byte 0x20, 0x50, 0x8D, 0xE5, 0x10, 0x00, 0x8D, 0xE5, 0x18, 0x00, 0x97, 0xE5, 0x00, 0x10, 0xA0, 0xE3
	.byte 0x00, 0x00, 0x8D, 0xE5, 0x20, 0x20, 0x97, 0xE5, 0x18, 0x00, 0xA0, 0xE3, 0x04, 0x20, 0x8D, 0xE5
	.byte 0x08, 0x10, 0x8D, 0xE5, 0x0C, 0x00, 0x8D, 0xE5, 0x10, 0x20, 0x97, 0xE5, 0x10, 0x30, 0x8D, 0xE2
	.byte 0x0A, 0x00, 0xA0, 0xE1, 0x0D, 0x10, 0xA0, 0xE3, 0xFF, 0xDA, 0xFF, 0xEB, 0x04, 0x10, 0x98, 0xE5
	.byte 0x08, 0x00, 0x98, 0xE5, 0x01, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0x14, 0x16, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A, 0x06, 0x00, 0xA0, 0xE1, 0x17, 0x16, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x15, 0x16, 0xFE, 0xEB, 0x34, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8
	.byte 0x08, 0x00, 0x88, 0xE5, 0x09, 0x61, 0x80, 0xE7, 0x04, 0x10, 0x98, 0xE5, 0x0C, 0x00, 0x98, 0xE5
	.byte 0x01, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0x05, 0x16, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x05, 0x00, 0x00, 0x1A, 0x06, 0x00, 0xA0, 0xE1, 0x08, 0x16, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x06, 0x16, 0xFE, 0xEB, 0x34, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8, 0x0C, 0x00, 0x88, 0xE5
	.byte 0x09, 0x51, 0x80, 0xE7, 0x04, 0x10, 0x98, 0xE5, 0x10, 0x00, 0x98, 0xE5, 0x01, 0x10, 0x81, 0xE2
	.byte 0x01, 0x11, 0xA0, 0xE1, 0xF6, 0x15, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x05, 0x00, 0x00, 0x1A
	.byte 0x06, 0x00, 0xA0, 0xE1, 0xF9, 0x15, 0xFE, 0xEB, 0x04, 0x00, 0xA0, 0xE1, 0xF7, 0x15, 0xFE, 0xEB
	.byte 0x34, 0xD0, 0x8D, 0xE2, 0xF0, 0x8F, 0xBD, 0xE8, 0x10, 0x00, 0x88, 0xE5, 0x09, 0x41, 0x80, 0xE7
	.byte 0x04, 0x00, 0x98, 0xE5, 0x01, 0x00, 0x80, 0xE2, 0x04, 0x00, 0x88, 0xE5, 0x34, 0xD0, 0x8D, 0xE2
	.byte 0xF0, 0x8F, 0xBD, 0xE8, 0x0C, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xFC, 0x9E, 0x25, 0x02
	.byte 0x01, 0x0E, 0x00, 0x00, 0xDC, 0xA7, 0x25, 0x02, 0x15, 0x0E, 0x00, 0x00, 0x10, 0x40, 0x2D, 0xE9
	.byte 0x20, 0xD0, 0x4D, 0xE2, 0x14, 0x20, 0x8D, 0xE2, 0x00, 0x30, 0xA0, 0xE3, 0x00, 0x30, 0x82, 0xE5
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x00, 0x40, 0xA0, 0xE1, 0x04, 0x30, 0x82, 0xE5, 0x08, 0x30, 0x82, 0xE5
	.byte 0x14, 0x30, 0x8D, 0xE5, 0x1B, 0xED, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x20, 0xD0, 0x8D, 0x02
	.byte 0x10, 0x80, 0xBD, 0x08, 0x1C, 0xC0, 0x91, 0xE5, 0x01, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x8D, 0xE5
	.byte 0x04, 0x00, 0x9C, 0xE5, 0x00, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0x8D, 0xE5, 0x08, 0x30, 0x9C, 0xE5
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x08, 0x30, 0x8D, 0xE5, 0x0C, 0x30, 0x9C, 0xE5, 0x10, 0x30, 0x8D, 0xE5
	.byte 0x10, 0x30, 0x9C, 0xE5, 0x0C, 0x30, 0x8D, 0xE5, 0xE2, 0xED, 0xFF, 0xEB, 0x20, 0xD0, 0x8D, 0xE2
	.byte 0x10, 0x80, 0xBD, 0xE8, 0x70, 0x40, 0x2D, 0xE9, 0x70, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x94, 0xE5, 0x00, 0x60, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0xAA
	.byte 0x4C, 0x01, 0x9F, 0xE5, 0x4C, 0x11, 0x9F, 0xE5, 0x4C, 0x21, 0x9F, 0xE5, 0x4C, 0x31, 0x9F, 0xE5
	.byte 0xB1, 0x07, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x70, 0xD0, 0x8D, 0xB2
	.byte 0x70, 0x80, 0xBD, 0xB8, 0x20, 0x30, 0x94, 0xE5, 0x03, 0x20, 0x40, 0xE2, 0x08, 0x00, 0x93, 0xE5
	.byte 0x0C, 0x10, 0x83, 0xE2, 0x04, 0x40, 0x93, 0xE5, 0x90, 0xF2, 0xFF, 0xEB, 0x00, 0x50, 0xB0, 0xE1
	.byte 0x70, 0xD0, 0x8D, 0x02, 0x70, 0x80, 0xBD, 0x08, 0x50, 0x30, 0x8D, 0xE2, 0x00, 0x00, 0xA0, 0xE3
	.byte 0x03, 0x20, 0xA0, 0xE1, 0x00, 0x10, 0xA0, 0xE1, 0x03, 0x00, 0xA2, 0xE8, 0x03, 0x00, 0xA2, 0xE8
	.byte 0x03, 0x00, 0xA2, 0xE8, 0x03, 0x00, 0x82, 0xE8, 0x03, 0x00, 0xA0, 0xE1, 0x05, 0x10, 0xA0, 0xE1
	.byte 0x2F, 0xF3, 0xFF, 0xEB, 0x30, 0x20, 0x8D, 0xE2, 0x06, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0xD7, 0xE7, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x18, 0x00, 0x00, 0x1A, 0x50, 0x20, 0x8D, 0xE2
	.byte 0x06, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x0C, 0xE8, 0xFF, 0xEB, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x10, 0xA0, 0xE1, 0xD2, 0xE6, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x18, 0x10, 0x90, 0x15
	.byte 0x00, 0x00, 0x51, 0x13, 0x0D, 0x00, 0x00, 0x0A, 0x50, 0x10, 0x8D, 0xE2, 0x14, 0x10, 0x8D, 0xE5
	.byte 0x10, 0x40, 0x8D, 0xE5, 0x2C, 0x20, 0x90, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE5
	.byte 0x12, 0x00, 0x8D, 0xE9, 0x08, 0x10, 0xA0, 0xE3, 0x0C, 0x10, 0x8D, 0xE5, 0x18, 0x20, 0x90, 0xE5
	.byte 0x10, 0x30, 0x8D, 0xE2, 0x06, 0x00, 0xA0, 0xE1, 0x0A, 0x10, 0xA0, 0xE3, 0x66, 0xDA, 0xFF, 0xEB
	.byte 0x24, 0x20, 0x8D, 0xE2, 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0xC0, 0x82, 0xE5, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x00, 0xC0, 0x82, 0xE5, 0x05, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5
	.byte 0x24, 0x30, 0x8D, 0xE5, 0x28, 0x40, 0x8D, 0xE5, 0xBA, 0xEC, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x07, 0x00, 0x00, 0x0A, 0x50, 0x30, 0x8D, 0xE2, 0x01, 0xC0, 0xA0, 0xE3, 0x18, 0x20, 0x8D, 0xE2
	.byte 0x06, 0x00, 0xA0, 0xE1, 0x18, 0xC0, 0x8D, 0xE5, 0x1C, 0x40, 0x8D, 0xE5, 0x20, 0x30, 0x8D, 0xE5
	.byte 0x88, 0xED, 0xFF, 0xEB, 0x05, 0x00, 0xA0, 0xE1, 0x70, 0x15, 0xFE, 0xEB, 0x70, 0xD0, 0x8D, 0xE2
	.byte 0x70, 0x80, 0xBD, 0xE8, 0xEC, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xD0, 0xA1, 0x25, 0x02
	.byte 0x93, 0x0E, 0x00, 0x00, 0xF8, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2, 0x00, 0x70, 0xB0, 0xE1
	.byte 0x01, 0x60, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x98, 0x01, 0x9F, 0xE5, 0x98, 0x11, 0x9F, 0xE5
	.byte 0x98, 0x21, 0x9F, 0xE5, 0x98, 0x31, 0x9F, 0xE5, 0x53, 0x07, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5
	.byte 0x06, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x88, 0x01, 0x9F, 0xE5, 0x78, 0x11, 0x9F, 0xE5
	.byte 0x78, 0x21, 0x9F, 0xE5, 0x80, 0x31, 0x9F, 0xE5, 0x4B, 0x07, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5
	.byte 0x06, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12, 0xF8, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x96, 0xE5
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x04, 0xC0, 0x90, 0xE5, 0x00, 0x50, 0xA0, 0xE3, 0x04, 0x50, 0x82, 0xE5
	.byte 0x10, 0x50, 0x82, 0xE5, 0x07, 0x00, 0xA0, 0xE1, 0x00, 0x50, 0x82, 0xE5, 0x04, 0x40, 0xA0, 0xE3
	.byte 0x0C, 0x50, 0x82, 0xE5, 0x07, 0x30, 0xA0, 0xE3, 0x02, 0x10, 0xA0, 0xE3, 0x08, 0x50, 0x82, 0xE5
	.byte 0x14, 0x50, 0x82, 0xE5, 0x00, 0x40, 0x8D, 0xE5, 0x04, 0xC0, 0x8D, 0xE5, 0x0C, 0x30, 0x8D, 0xE5
	.byte 0x10, 0xC0, 0x8D, 0xE5, 0x7F, 0xEC, 0xFF, 0xEB, 0x00, 0x40, 0xB0, 0xE1, 0x18, 0xD0, 0x8D, 0x02
	.byte 0xF8, 0x80, 0xBD, 0x08, 0x00, 0x00, 0x94, 0xE5, 0x04, 0x00, 0x50, 0xE3, 0x15, 0x00, 0x00, 0x1A
	.byte 0x20, 0x10, 0x96, 0xE5, 0x1C, 0x60, 0x94, 0xE5, 0x08, 0x00, 0x91, 0xE5, 0x14, 0x40, 0x91, 0xE5
	.byte 0x0C, 0x50, 0x91, 0xE5, 0x34, 0x61, 0xFE, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x02
	.byte 0x00, 0x00, 0x86, 0xE5, 0xF8, 0x80, 0xBD, 0x08, 0x04, 0x00, 0xA0, 0xE1, 0x2E, 0x61, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x02, 0x04, 0x00, 0x86, 0xE5, 0xF8, 0x80, 0xBD, 0x08
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x28, 0x61, 0xFE, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x08, 0x00, 0x86, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0xF8, 0x80, 0xBD, 0xE8, 0x07, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12
	.byte 0xF8, 0x80, 0xBD, 0x18, 0x1C, 0x50, 0x94, 0xE5, 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x98, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5, 0x80, 0x20, 0x9F, 0xE5, 0x90, 0x30, 0x9F, 0xE5
	.byte 0x0D, 0x07, 0xFA, 0xEB, 0x00, 0x00, 0x95, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A
	.byte 0x80, 0x00, 0x9F, 0xE5, 0x60, 0x10, 0x9F, 0xE5, 0x60, 0x20, 0x9F, 0xE5, 0x78, 0x30, 0x9F, 0xE5
	.byte 0x05, 0x07, 0xFA, 0xEB, 0x00, 0x00, 0x95, 0xE5, 0xD0, 0x00, 0xD0, 0xE1, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x64, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0x54, 0x30, 0x9F, 0xE5, 0xFC, 0x06, 0xFA, 0xEB, 0x20, 0x00, 0x96, 0xE5, 0x00, 0x20, 0x95, 0xE5
	.byte 0x0C, 0x30, 0x90, 0xE5, 0x48, 0x10, 0x9F, 0xE5, 0x1C, 0x00, 0x87, 0xE2, 0x73, 0x12, 0x00, 0xEB
	.byte 0x07, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x00, 0x20, 0xA0, 0xE3, 0x15, 0xED, 0xFF, 0xEB
	.byte 0x18, 0xD0, 0x8D, 0xE2, 0xF8, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xB8, 0xA0, 0x25, 0x02, 0xD5, 0x0E, 0x00, 0x00, 0x04, 0xA8, 0x25, 0x02, 0xDB, 0x0E, 0x00, 0x00
	.byte 0xDC, 0xA7, 0x25, 0x02, 0x1D, 0x0F, 0x00, 0x00, 0x1C, 0xA8, 0x25, 0x02, 0x1E, 0x0F, 0x00, 0x00
	.byte 0x34, 0xA8, 0x25, 0x02, 0x50, 0xA8, 0x25, 0x02, 0xF0, 0x40, 0x2D, 0xE9, 0x0C, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x60, 0xA0, 0xE1, 0x24, 0x10, 0x96, 0xE5, 0x00, 0x70, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x10, 0x01, 0x9F, 0xE5, 0x10, 0x11, 0x9F, 0xE5, 0x10, 0x21, 0x9F, 0xE5
	.byte 0x10, 0x31, 0x9F, 0xE5, 0xD8, 0x06, 0xFA, 0xEB, 0x24, 0x00, 0x96, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x0C, 0xD0, 0x8D, 0x12, 0xF0, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x96, 0xE5, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x50, 0x90, 0xE5, 0x00, 0x40, 0xA0, 0xE3, 0x04, 0x40, 0x82, 0xE5, 0x07, 0x00, 0xA0, 0xE1
	.byte 0x00, 0x40, 0x82, 0xE5, 0x04, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x40, 0x82, 0xE5
	.byte 0x00, 0x30, 0x8D, 0xE5, 0x04, 0x50, 0x8D, 0xE5, 0x12, 0xEC, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x20, 0x10, 0x96, 0xE5, 0x1C, 0x40, 0x90, 0xE5
	.byte 0x08, 0x00, 0x91, 0xE5, 0xB0, 0x10, 0x9F, 0xE5, 0x76, 0x1A, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x9C, 0x50, 0x9F, 0xE5, 0x00, 0x60, 0xA0, 0xE3
	.byte 0xD0, 0x10, 0xD0, 0xE1, 0x2D, 0x00, 0x51, 0xE3, 0x01, 0x00, 0x80, 0x02, 0xD0, 0x10, 0xD0, 0xE1
	.byte 0x40, 0x00, 0x51, 0xE3, 0x2B, 0x00, 0x51, 0x13, 0x01, 0x00, 0x80, 0x02, 0xBE, 0x60, 0xFE, 0xEB
	.byte 0x00, 0x70, 0xB0, 0xE1, 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x0C, 0x10, 0x94, 0xE5
	.byte 0x10, 0x00, 0x94, 0xE5, 0x01, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0xB0, 0x14, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x1A, 0x07, 0x00, 0xA0, 0xE1, 0xB3, 0x14, 0xFE, 0xEB
	.byte 0x0C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0x10, 0x00, 0x84, 0xE5, 0x0C, 0x10, 0x94, 0xE5
	.byte 0x01, 0x71, 0x80, 0xE7, 0x0C, 0x10, 0x94, 0xE5, 0x06, 0x00, 0xA0, 0xE1, 0x01, 0x20, 0x81, 0xE2
	.byte 0x05, 0x10, 0xA0, 0xE1, 0x0C, 0x20, 0x84, 0xE5, 0x52, 0x1A, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0xDE, 0xFF, 0xFF, 0x1A, 0x0C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0xEC, 0xA1, 0x25, 0x02, 0x34, 0x0F, 0x00, 0x00, 0x24, 0xA7, 0x25, 0x02
	.byte 0x38, 0x40, 0x2D, 0xE9, 0x28, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5
	.byte 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0xB4, 0x00, 0x9F, 0xE5
	.byte 0xB4, 0x10, 0x9F, 0xE5, 0xB4, 0x20, 0x9F, 0xE5, 0xB4, 0x30, 0x9F, 0xE5, 0x86, 0x06, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x28, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18
	.byte 0x20, 0x00, 0x95, 0xE5, 0x1C, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5, 0x00, 0xC0, 0xA0, 0xE3
	.byte 0x04, 0xC0, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x82, 0xE5, 0x04, 0x30, 0xA0, 0xE3
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5, 0x1C, 0x30, 0x8D, 0xE5, 0x20, 0x50, 0x8D, 0xE5
	.byte 0xC0, 0xEB, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x28, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08
	.byte 0x1C, 0xC0, 0x91, 0xE5, 0x00, 0x20, 0x8D, 0xE2, 0x00, 0x00, 0x9C, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x01, 0x00, 0xA0, 0x13, 0x00, 0x00, 0xA0, 0x03, 0x04, 0x50, 0x8D, 0xE5, 0x00, 0x00, 0x8D, 0xE5
	.byte 0x00, 0x00, 0x9C, 0xE5, 0x08, 0x00, 0x8D, 0xE5, 0x04, 0x30, 0x9C, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x0C, 0x30, 0x8D, 0xE5, 0x08, 0x30, 0x9C, 0xE5, 0x10, 0x30, 0x8D, 0xE5, 0x0C, 0x30, 0x9C, 0xE5
	.byte 0x14, 0x30, 0x8D, 0xE5, 0x10, 0x30, 0x9C, 0xE5, 0x18, 0x30, 0x8D, 0xE5, 0x81, 0xEC, 0xFF, 0xEB
	.byte 0x28, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x18, 0xA1, 0x25, 0x02, 0x76, 0x0F, 0x00, 0x00, 0xF0, 0x40, 0x2D, 0xE9, 0x0C, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0xAA, 0x0C, 0x01, 0x9F, 0xE5, 0x0C, 0x11, 0x9F, 0xE5, 0x0C, 0x21, 0x9F, 0xE5
	.byte 0x0C, 0x31, 0x9F, 0xE5, 0x4C, 0x06, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x0C, 0xD0, 0x8D, 0xB2, 0xF0, 0x80, 0xBD, 0xB8, 0x20, 0x00, 0x94, 0xE5, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x70, 0x90, 0xE5, 0x08, 0x40, 0x90, 0xE5, 0x00, 0x60, 0xA0, 0xE3, 0x04, 0x60, 0x82, 0xE5
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x00, 0x60, 0x82, 0xE5, 0x08, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x08, 0x60, 0x82, 0xE5, 0x00, 0x30, 0x8D, 0xE5, 0x04, 0x70, 0x8D, 0xE5, 0x85, 0xEB, 0xFF, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x1C, 0x50, 0x90, 0xE5
	.byte 0x00, 0x00, 0x55, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xA8, 0x00, 0x9F, 0xE5, 0x98, 0x10, 0x9F, 0xE5
	.byte 0x98, 0x20, 0x9F, 0xE5, 0xA0, 0x30, 0x9F, 0xE5, 0x2F, 0x06, 0xFA, 0xEB, 0x00, 0x00, 0x95, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0xAA, 0x90, 0x00, 0x9F, 0xE5, 0x78, 0x10, 0x9F, 0xE5
	.byte 0x78, 0x20, 0x9F, 0xE5, 0xFB, 0x3E, 0xA0, 0xE3, 0x27, 0x06, 0xFA, 0xEB, 0x00, 0x10, 0x95, 0xE5
	.byte 0x04, 0x00, 0x95, 0xE5, 0x01, 0x10, 0x81, 0xE2, 0x01, 0x11, 0xA0, 0xE1, 0x28, 0x14, 0xFE, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08, 0x04, 0x00, 0x85, 0xE5
	.byte 0x04, 0x00, 0xA0, 0xE1, 0xD5, 0x18, 0xFA, 0xEB, 0x00, 0x70, 0xA0, 0xE1, 0x01, 0x00, 0x87, 0xE2
	.byte 0x1A, 0x14, 0xFE, 0xEB, 0x00, 0x60, 0xB0, 0xE1, 0x0C, 0xD0, 0x8D, 0x02, 0xF0, 0x80, 0xBD, 0x08
	.byte 0x04, 0x10, 0xA0, 0xE1, 0x01, 0x20, 0x87, 0xE2, 0x22, 0x0A, 0xFA, 0xEB, 0x03, 0x00, 0x95, 0xE8
	.byte 0x00, 0x61, 0x81, 0xE7, 0x00, 0x00, 0x95, 0xE5, 0x01, 0x00, 0x80, 0xE2, 0x00, 0x00, 0x85, 0xE5
	.byte 0x0C, 0xD0, 0x8D, 0xE2, 0xF0, 0x80, 0xBD, 0xE8, 0x64, 0xA8, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xEC, 0x9F, 0x25, 0x02, 0x9D, 0x0F, 0x00, 0x00, 0xDC, 0xA7, 0x25, 0x02, 0xAF, 0x0F, 0x00, 0x00
	.byte 0x7C, 0xA8, 0x25, 0x02, 0x30, 0x40, 0x2D, 0xE9, 0x1C, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x90, 0x00, 0x9F, 0xE5, 0x90, 0x10, 0x9F, 0xE5, 0x90, 0x20, 0x9F, 0xE5, 0x90, 0x30, 0x9F, 0xE5
	.byte 0xF9, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x1C, 0xD0, 0x8D, 0x12
	.byte 0x30, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x10, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5
	.byte 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0xC0, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x82, 0xE5
	.byte 0x08, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0xC0, 0x82, 0xE5, 0x10, 0x30, 0x8D, 0xE5
	.byte 0x14, 0x50, 0x8D, 0xE5, 0x33, 0xEB, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x1C, 0xD0, 0x8D, 0x02
	.byte 0x30, 0x80, 0xBD, 0x08, 0x1C, 0x30, 0x91, 0xE5, 0x01, 0x00, 0xA0, 0xE3, 0x04, 0x50, 0x8D, 0xE5
	.byte 0x00, 0x00, 0x8D, 0xE5, 0x00, 0x00, 0x93, 0xE5, 0x00, 0x20, 0x8D, 0xE2, 0x08, 0x00, 0x8D, 0xE5
	.byte 0x04, 0x30, 0x93, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x0C, 0x30, 0x8D, 0xE5, 0xFD, 0xEB, 0xFF, 0xEB
	.byte 0x1C, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x7C, 0xA1, 0x25, 0x02, 0xCF, 0x0F, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x98, 0x00, 0x9F, 0xE5, 0x98, 0x10, 0x9F, 0xE5
	.byte 0x98, 0x20, 0x9F, 0xE5, 0x98, 0x30, 0x9F, 0xE5, 0xCB, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x02, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x88, 0x00, 0x9F, 0xE5, 0x78, 0x10, 0x9F, 0xE5
	.byte 0x78, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0xC3, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x02, 0x00, 0x50, 0xE3, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x10, 0x95, 0xE5, 0xDB, 0x0F, 0x84, 0xE2
	.byte 0x00, 0x50, 0x91, 0xE5, 0x05, 0x10, 0xA0, 0xE1, 0xE1, 0x18, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x05, 0x00, 0x00, 0x0A, 0x05, 0x10, 0xA0, 0xE1, 0xDB, 0x0F, 0x84, 0xE2, 0x40, 0x20, 0xA0, 0xE3
	.byte 0xA7, 0x18, 0xFA, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0xAB, 0x03, 0xC4, 0xE5, 0x00, 0x20, 0xA0, 0xE3
	.byte 0x04, 0x20, 0x84, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x00, 0x10, 0x84, 0xE5, 0x14, 0xC0, 0x94, 0xE5
	.byte 0x00, 0x00, 0x5C, 0xE3, 0x38, 0x80, 0xBD, 0x08, 0x18, 0x30, 0x94, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x3C, 0xFF, 0x2F, 0xE1, 0x38, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x14, 0xA0, 0x25, 0x02, 0xEC, 0x0F, 0x00, 0x00, 0xF0, 0xA5, 0x25, 0x02, 0xF2, 0x0F, 0x00, 0x00
	.byte 0xF0, 0x41, 0x2D, 0xE9, 0x00, 0x60, 0xB0, 0xE1, 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A
	.byte 0xEC, 0x00, 0x9F, 0xE5, 0xEC, 0x10, 0x9F, 0xE5, 0xEC, 0x20, 0x9F, 0xE5, 0xEC, 0x30, 0x9F, 0xE5
	.byte 0x99, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0xDC, 0x00, 0x9F, 0xE5, 0xCC, 0x10, 0x9F, 0xE5, 0xCC, 0x20, 0x9F, 0xE5, 0xD4, 0x30, 0x9F, 0xE5
	.byte 0x91, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0xF0, 0x81, 0xBD, 0x18
	.byte 0x20, 0x00, 0x94, 0xE5, 0x30, 0x00, 0x90, 0xE9, 0x04, 0x00, 0xA0, 0xE1, 0x43, 0x18, 0xFA, 0xEB
	.byte 0x00, 0x80, 0xA0, 0xE1, 0x05, 0x00, 0xA0, 0xE1, 0x40, 0x18, 0xFA, 0xEB, 0x28, 0x20, 0x86, 0xE2
	.byte 0x00, 0x70, 0xA0, 0xE1, 0x04, 0x00, 0xA0, 0xE1, 0x08, 0x10, 0xA0, 0xE1, 0x02, 0x2B, 0x82, 0xE2
	.byte 0xBC, 0xEA, 0xFF, 0xEB, 0x28, 0x20, 0x86, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x07, 0x10, 0xA0, 0xE1
	.byte 0x02, 0x2B, 0x82, 0xE2, 0xB7, 0xEA, 0xFF, 0xEB, 0x42, 0x20, 0x86, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x08, 0x10, 0xA0, 0xE1, 0x02, 0x2C, 0x82, 0xE2, 0x6C, 0xEA, 0xFF, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x07, 0x10, 0xA0, 0xE1, 0x05, 0x2D, 0x86, 0xE2, 0x68, 0xEA, 0xFF, 0xEB, 0x01, 0x00, 0xA0, 0xE3
	.byte 0x3C, 0x01, 0x86, 0xE5, 0xA8, 0x08, 0x96, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x02, 0x00, 0x00, 0x0A
	.byte 0x06, 0x00, 0xA0, 0xE1, 0x7A, 0x03, 0x00, 0xEB, 0xF0, 0x81, 0xBD, 0xE8, 0x10, 0x00, 0x96, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x0A, 0x2C, 0x10, 0x9F, 0xE5, 0x1C, 0x00, 0x86, 0xE2
	.byte 0x32, 0x10, 0x00, 0xEB, 0xF0, 0x81, 0xBD, 0xE8, 0x06, 0x00, 0xA0, 0xE1, 0x6A, 0x03, 0x00, 0xEB
	.byte 0xF0, 0x81, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x70, 0xA0, 0x25, 0x02
	.byte 0x15, 0x10, 0x00, 0x00, 0x48, 0xA7, 0x25, 0x02, 0x1B, 0x10, 0x00, 0x00, 0x90, 0xA8, 0x25, 0x02
	.byte 0x78, 0x40, 0x2D, 0xE9, 0x14, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5
	.byte 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x84, 0x00, 0x9F, 0xE5
	.byte 0x84, 0x10, 0x9F, 0xE5, 0x84, 0x20, 0x9F, 0xE5, 0x84, 0x30, 0x9F, 0xE5, 0x4E, 0x05, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x14, 0xD0, 0x8D, 0x12, 0x78, 0x80, 0xBD, 0x18
	.byte 0x20, 0x00, 0x95, 0xE5, 0x04, 0x00, 0x90, 0xE5, 0xAB, 0x20, 0xFA, 0xEB, 0x20, 0x10, 0x95, 0xE5
	.byte 0x00, 0x50, 0xA0, 0xE1, 0x08, 0x60, 0x91, 0xE5, 0x08, 0x20, 0x8D, 0xE2, 0x00, 0xC0, 0xA0, 0xE3
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x82, 0xE5, 0x10, 0x30, 0xA0, 0xE3, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x04, 0xC0, 0x82, 0xE5, 0x08, 0xC0, 0x82, 0xE5, 0x08, 0x30, 0x8D, 0xE5, 0x85, 0xEA, 0xFF, 0xEB
	.byte 0x00, 0x10, 0xB0, 0xE1, 0x14, 0xD0, 0x8D, 0x02, 0x78, 0x80, 0xBD, 0x08, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x50, 0x8D, 0xE5, 0x04, 0x60, 0x8D, 0xE5, 0x55, 0xEB, 0xFF, 0xEB
	.byte 0x14, 0xD0, 0x8D, 0xE2, 0x78, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x24, 0x9F, 0x25, 0x02, 0x4E, 0x10, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x00, 0x50, 0xB0, 0xE1
	.byte 0x01, 0x40, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5
	.byte 0x80, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0x23, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0xAA, 0x70, 0x00, 0x9F, 0xE5, 0x60, 0x10, 0x9F, 0xE5
	.byte 0x60, 0x20, 0x9F, 0xE5, 0x68, 0x30, 0x9F, 0xE5, 0x1B, 0x05, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x38, 0x80, 0xBD, 0xB8, 0x20, 0x00, 0x94, 0xE5, 0x04, 0x00, 0x90, 0xE5
	.byte 0x79, 0x20, 0xFA, 0xEB, 0xAC, 0x08, 0x85, 0xE5, 0x20, 0x00, 0x94, 0xE5, 0x08, 0x00, 0x90, 0xE5
	.byte 0x75, 0x20, 0xFA, 0xEB, 0xB0, 0x08, 0x85, 0xE5, 0x10, 0x00, 0x95, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x03, 0x00, 0x00, 0x0A, 0x2C, 0x10, 0x9F, 0xE5, 0x1C, 0x00, 0x85, 0xE2, 0xD7, 0x0F, 0x00, 0xEB
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x05, 0x00, 0xA0, 0xE1, 0x0F, 0x03, 0x00, 0xEB, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x38, 0x9F, 0x25, 0x02, 0x69, 0x10, 0x00, 0x00
	.byte 0x64, 0xA8, 0x25, 0x02, 0x6F, 0x10, 0x00, 0x00, 0x90, 0xA8, 0x25, 0x02, 0x30, 0x40, 0x2D, 0xE9
	.byte 0x44, 0xD0, 0x4D, 0xE2, 0x01, 0x40, 0xA0, 0xE1, 0x24, 0x10, 0x94, 0xE5, 0x00, 0x50, 0xA0, 0xE1
	.byte 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x08, 0x01, 0x9F, 0xE5, 0x08, 0x11, 0x9F, 0xE5
	.byte 0x08, 0x21, 0x9F, 0xE5, 0x08, 0x31, 0x9F, 0xE5, 0xF3, 0x04, 0xFA, 0xEB, 0x24, 0x00, 0x94, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x44, 0xD0, 0x8D, 0x12, 0x30, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x94, 0xE5
	.byte 0x2C, 0x20, 0x8D, 0xE2, 0x04, 0x40, 0x90, 0xE5, 0x00, 0xE0, 0xA0, 0xE3, 0x04, 0xE0, 0x82, 0xE5
	.byte 0x05, 0x30, 0xA0, 0xE3, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0xE0, 0x82, 0xE5, 0x01, 0xC0, 0xA0, 0xE3
	.byte 0x02, 0x10, 0xA0, 0xE3, 0x08, 0xE0, 0x82, 0xE5, 0x2C, 0xC0, 0x8D, 0xE5, 0x30, 0x40, 0x8D, 0xE5
	.byte 0x38, 0x30, 0x8D, 0xE5, 0x3C, 0x40, 0x8D, 0xE5, 0x2A, 0xEA, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x17, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x91, 0xE5, 0x01, 0x00, 0x50, 0xE3, 0x09, 0x00, 0x00, 0x1A
	.byte 0x00, 0xC0, 0xA0, 0xE3, 0x01, 0x30, 0xA0, 0xE3, 0x20, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x20, 0xC0, 0x8D, 0xE5, 0x24, 0x30, 0x8D, 0xE5, 0x28, 0x40, 0x8D, 0xE5, 0xF5, 0xEA, 0xFF, 0xEB
	.byte 0x44, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8, 0x05, 0x00, 0x50, 0xE3, 0x08, 0x00, 0x00, 0x1A
	.byte 0x00, 0x30, 0xA0, 0xE3, 0x14, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x14, 0x30, 0x8D, 0xE5
	.byte 0x18, 0x40, 0x8D, 0xE5, 0x1C, 0x30, 0x8D, 0xE5, 0xEA, 0xEA, 0xFF, 0xEB, 0x44, 0xD0, 0x8D, 0xE2
	.byte 0x30, 0x80, 0xBD, 0xE8, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x71, 0xEA, 0xFF, 0xEB
	.byte 0x00, 0x10, 0xB0, 0xE1, 0x44, 0xD0, 0x8D, 0x02, 0x30, 0x80, 0xBD, 0x08, 0x00, 0x30, 0xA0, 0xE3
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x05, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5
	.byte 0x08, 0x30, 0x8D, 0xE5, 0x0C, 0x30, 0x8D, 0xE5, 0x10, 0x30, 0x8D, 0xE5, 0xD9, 0xEA, 0xFF, 0xEB
	.byte 0x44, 0xD0, 0x8D, 0xE2, 0x30, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xB4, 0xA1, 0x25, 0x02, 0x90, 0x10, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5, 0x80, 0x20, 0x9F, 0xE5
	.byte 0x80, 0x30, 0x9F, 0xE5, 0xA4, 0x04, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x18, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x0C, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x50, 0x90, 0xE5, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x00, 0x30, 0x82, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5, 0x0C, 0x10, 0x8D, 0xE5
	.byte 0x10, 0x50, 0x8D, 0xE5, 0xDF, 0xE9, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x18, 0xD0, 0x8D, 0x02
	.byte 0x38, 0x80, 0xBD, 0x08, 0x00, 0xC0, 0xA0, 0xE3, 0x06, 0x30, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5, 0x08, 0x50, 0x8D, 0xE5
	.byte 0xAC, 0xEA, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0x94, 0xA2, 0x25, 0x02, 0xDE, 0x10, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x18, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1
	.byte 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5
	.byte 0x80, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0x77, 0x04, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5
	.byte 0x0C, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x82, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5
	.byte 0x0C, 0x10, 0x8D, 0xE5, 0x10, 0x50, 0x8D, 0xE5, 0xB2, 0xE9, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x18, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08, 0x00, 0xC0, 0xA0, 0xE3, 0x02, 0x30, 0xA0, 0xE3
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5
	.byte 0x08, 0x50, 0x8D, 0xE5, 0x7F, 0xEA, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x08, 0xA2, 0x25, 0x02, 0x01, 0x11, 0x00, 0x00
	.byte 0x38, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5
	.byte 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x9F, 0xE5
	.byte 0x80, 0x10, 0x9F, 0xE5, 0x80, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0x4A, 0x04, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18
	.byte 0x20, 0x00, 0x95, 0xE5, 0x0C, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5, 0x00, 0x30, 0xA0, 0xE3
	.byte 0x04, 0x30, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x82, 0xE5, 0x01, 0x10, 0xA0, 0xE3
	.byte 0x08, 0x30, 0x82, 0xE5, 0x0C, 0x10, 0x8D, 0xE5, 0x10, 0x50, 0x8D, 0xE5, 0x85, 0xE9, 0xFF, 0xEB
	.byte 0x00, 0x10, 0xB0, 0xE1, 0x18, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08, 0x00, 0xC0, 0xA0, 0xE3
	.byte 0x03, 0x30, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5
	.byte 0x04, 0x30, 0x8D, 0xE5, 0x08, 0x50, 0x8D, 0xE5, 0x52, 0xEA, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x5C, 0xA2, 0x25, 0x02
	.byte 0x24, 0x11, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1
	.byte 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A
	.byte 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5, 0x80, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5
	.byte 0x1D, 0x04, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12
	.byte 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x0C, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5
	.byte 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x82, 0xE5
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5, 0x0C, 0x10, 0x8D, 0xE5, 0x10, 0x50, 0x8D, 0xE5
	.byte 0x58, 0xE9, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x18, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08
	.byte 0x00, 0xC0, 0xA0, 0xE3, 0x04, 0x30, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x00, 0xC0, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5, 0x08, 0x50, 0x8D, 0xE5, 0x25, 0xEA, 0xFF, 0xEB
	.byte 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0x78, 0xA2, 0x25, 0x02, 0x47, 0x11, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9, 0x18, 0xD0, 0x4D, 0xE2
	.byte 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1, 0x03, 0x00, 0x51, 0xE3
	.byte 0x04, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5, 0x80, 0x20, 0x9F, 0xE5
	.byte 0x80, 0x30, 0x9F, 0xE5, 0xF0, 0x03, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x18, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5, 0x0C, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x50, 0x90, 0xE5, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x00, 0x30, 0x82, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5, 0x0C, 0x10, 0x8D, 0xE5
	.byte 0x10, 0x50, 0x8D, 0xE5, 0x2B, 0xE9, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1, 0x18, 0xD0, 0x8D, 0x02
	.byte 0x38, 0x80, 0xBD, 0x08, 0x00, 0xC0, 0xA0, 0xE3, 0x05, 0x30, 0xA0, 0xE3, 0x00, 0x20, 0x8D, 0xE2
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5, 0x08, 0x50, 0x8D, 0xE5
	.byte 0xF8, 0xE9, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x48, 0xA7, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0x98, 0xA1, 0x25, 0x02, 0x6A, 0x11, 0x00, 0x00, 0x38, 0x40, 0x2D, 0xE9
	.byte 0x18, 0xD0, 0x4D, 0xE2, 0x01, 0x50, 0xA0, 0xE1, 0x24, 0x10, 0x95, 0xE5, 0x00, 0x40, 0xA0, 0xE1
	.byte 0x03, 0x00, 0x51, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x9F, 0xE5, 0x80, 0x10, 0x9F, 0xE5
	.byte 0x80, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0x9F, 0xE5, 0xC3, 0x03, 0xFA, 0xEB, 0x24, 0x00, 0x95, 0xE5
	.byte 0x03, 0x00, 0x50, 0xE3, 0x18, 0xD0, 0x8D, 0x12, 0x38, 0x80, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5
	.byte 0x0C, 0x20, 0x8D, 0xE2, 0x04, 0x50, 0x90, 0xE5, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x30, 0x82, 0xE5
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x00, 0x30, 0x82, 0xE5, 0x01, 0x10, 0xA0, 0xE3, 0x08, 0x30, 0x82, 0xE5
	.byte 0x0C, 0x10, 0x8D, 0xE5, 0x10, 0x50, 0x8D, 0xE5, 0xFE, 0xE8, 0xFF, 0xEB, 0x00, 0x10, 0xB0, 0xE1
	.byte 0x18, 0xD0, 0x8D, 0x02, 0x38, 0x80, 0xBD, 0x08, 0x00, 0xC0, 0xA0, 0xE3, 0x08, 0x30, 0xA0, 0xE3
	.byte 0x00, 0x20, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1, 0x00, 0xC0, 0x8D, 0xE5, 0x04, 0x30, 0x8D, 0xE5
	.byte 0x08, 0x50, 0x8D, 0xE5, 0xCB, 0xE9, 0xFF, 0xEB, 0x18, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x48, 0xA7, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x30, 0xA1, 0x25, 0x02, 0x8D, 0x11, 0x00, 0x00
	.byte 0x1E, 0xFF, 0x2F, 0xE1, 0x18, 0x40, 0x2D, 0xE9, 0x04, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x3C, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0x3C, 0x30, 0x9F, 0xE5, 0x98, 0x03, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0xD0, 0x8D, 0x02, 0x18, 0x80, 0xBD, 0x08, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x00, 0xA0, 0xE1
	.byte 0xDB, 0x2F, 0x84, 0xE2, 0x02, 0x10, 0xA0, 0xE3, 0x00, 0x30, 0x8D, 0xE5, 0xF1, 0x09, 0x00, 0xEB
	.byte 0x04, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xB0, 0xA2, 0x25, 0x02, 0xB2, 0x11, 0x00, 0x00, 0x10, 0x40, 0x2D, 0xE9, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x40, 0x00, 0x9F, 0xE5, 0x40, 0x10, 0x9F, 0xE5, 0x40, 0x20, 0x9F, 0xE5
	.byte 0x40, 0x30, 0x9F, 0xE5, 0x80, 0x03, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x10, 0x80, 0xBD, 0x08, 0x00, 0x10, 0xA0, 0xE3, 0x04, 0x10, 0x84, 0xE5, 0x14, 0xC0, 0x94, 0xE5
	.byte 0x00, 0x00, 0x5C, 0xE3, 0x10, 0x80, 0xBD, 0x08, 0x18, 0x30, 0x94, 0xE5, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x02, 0x20, 0xA0, 0xE3, 0x3C, 0xFF, 0x2F, 0xE1, 0x10, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA4, 0x25, 0x02, 0x48, 0xA1, 0x25, 0x02, 0xC2, 0x11, 0x00, 0x00, 0x18, 0x40, 0x2D, 0xE9
	.byte 0x04, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0x3C, 0x00, 0x9F, 0xE5
	.byte 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5, 0x3C, 0x30, 0x9F, 0xE5, 0x66, 0x03, 0xFA, 0xEB
	.byte 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x04, 0xD0, 0x8D, 0x02, 0x18, 0x80, 0xBD, 0x08
	.byte 0x28, 0x20, 0x9F, 0xE5, 0x00, 0x30, 0xA0, 0xE3, 0x04, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE3
	.byte 0x00, 0x30, 0x8D, 0xE5, 0xBF, 0x09, 0x00, 0xEB, 0x04, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0x60, 0xA1, 0x25, 0x02, 0xDB, 0x11, 0x00, 0x00
	.byte 0x08, 0xA6, 0x25, 0x02, 0x18, 0x40, 0x2D, 0xE9, 0x04, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0x3C, 0x00, 0x9F, 0xE5, 0x3C, 0x10, 0x9F, 0xE5, 0x3C, 0x20, 0x9F, 0xE5
	.byte 0x3C, 0x30, 0x9F, 0xE5, 0x4C, 0x03, 0xFA, 0xEB, 0x04, 0x00, 0x94, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0xD0, 0x8D, 0x02, 0x18, 0x80, 0xBD, 0x08, 0x28, 0x20, 0x9F, 0xE5, 0x00, 0x30, 0xA0, 0xE3
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x03, 0x10, 0xA0, 0xE3, 0x00, 0x30, 0x8D, 0xE5, 0xA5, 0x09, 0x00, 0xEB
	.byte 0x04, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8, 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02
	.byte 0xD0, 0xA2, 0x25, 0x02, 0xEB, 0x11, 0x00, 0x00, 0x08, 0xA6, 0x25, 0x02, 0xF8, 0x43, 0x2D, 0xE9
	.byte 0x00, 0x40, 0xB0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xFC, 0x00, 0x9F, 0xE5
	.byte 0xFC, 0x10, 0x9F, 0xE5, 0xFC, 0x20, 0x9F, 0xE5, 0x12, 0x3C, 0xA0, 0xE3, 0x32, 0x03, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x04, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x0A, 0xE8, 0x00, 0x9F, 0xE5
	.byte 0xDC, 0x10, 0x9F, 0xE5, 0xDC, 0x20, 0x9F, 0xE5, 0xE0, 0x30, 0x9F, 0xE5, 0x2A, 0x03, 0xFA, 0xEB
	.byte 0x24, 0x00, 0x95, 0xE5, 0x04, 0x00, 0x50, 0xE3, 0xF8, 0x83, 0xBD, 0x18, 0x20, 0x00, 0x95, 0xE5
	.byte 0x04, 0x00, 0x90, 0xE5, 0x88, 0x1E, 0xFA, 0xEB, 0x20, 0x20, 0x95, 0xE5, 0x04, 0x10, 0x94, 0xE5
	.byte 0x08, 0x50, 0x92, 0xE5, 0x00, 0x70, 0xA0, 0xE1, 0x00, 0x00, 0x51, 0xE3, 0xF8, 0x83, 0xBD, 0x08
	.byte 0x07, 0x01, 0xA0, 0xE1, 0x1D, 0x11, 0xFE, 0xEB, 0x00, 0x80, 0xB0, 0xE1, 0xF8, 0x83, 0xBD, 0x08
	.byte 0x9C, 0x10, 0x9F, 0xE5, 0x05, 0x00, 0xA0, 0xE1, 0xCE, 0x16, 0xFA, 0xEB, 0x00, 0x90, 0xA0, 0xE3
	.byte 0x09, 0x60, 0xA0, 0xE1, 0x88, 0x50, 0x9F, 0xE5, 0x07, 0x00, 0x00, 0xEA, 0x1E, 0x5D, 0xFE, 0xEB
	.byte 0x09, 0x01, 0x88, 0xE7, 0x00, 0x00, 0x50, 0xE3, 0x07, 0x00, 0x00, 0x0A, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x05, 0x10, 0xA0, 0xE1, 0xC3, 0x16, 0xFA, 0xEB, 0x01, 0x90, 0x89, 0xE2, 0x07, 0x00, 0x59, 0xE1
	.byte 0x01, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x50, 0xE3, 0xF3, 0xFF, 0xFF, 0x1A, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x09, 0x30, 0xA0, 0xE1, 0x51, 0x2E, 0x84, 0xE2, 0x05, 0x10, 0xA0, 0xE3, 0x00, 0x80, 0x8D, 0xE5
	.byte 0x64, 0x09, 0x00, 0xEB, 0x00, 0x00, 0x59, 0xE3, 0x00, 0x40, 0xA0, 0xE3, 0x04, 0x00, 0x00, 0xDA
	.byte 0x04, 0x01, 0x98, 0xE7, 0x09, 0x11, 0xFE, 0xEB, 0x01, 0x40, 0x84, 0xE2, 0x09, 0x00, 0x54, 0xE1
	.byte 0xFA, 0xFF, 0xFF, 0xBA, 0x08, 0x00, 0xA0, 0xE1, 0x04, 0x11, 0xFE, 0xEB, 0xF8, 0x83, 0xBD, 0xE8
	.byte 0xC0, 0xA4, 0x25, 0x02, 0xD0, 0xA4, 0x25, 0x02, 0xF0, 0xA2, 0x25, 0x02, 0x0C, 0xA7, 0x25, 0x02
	.byte 0x06, 0x12, 0x00, 0x00, 0x98, 0xA8, 0x25, 0x02

	arm_func_start FUN_022534B8
FUN_022534B8: // 0x022534B8
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	movs r5, r1
	mov r6, r0
	bne _022534DC
	ldr r0, _02253544 // =0x0225AA84
	ldr r1, _02253548 // =0x0225AA94
	ldr r2, _0225354C // =0x0225AA24
	mov r3, #0x4f
	bl __msl_assertion_failed
_022534DC:
	ldr r7, _02253550 // =0x02259CA0
	mov r4, #0
	ldr r0, [r7]
	cmp r0, #0
	ble _0225353C
	ldr r8, _02253554 // =0x0225A310
_022534F4:
	ldr r0, [r5, #0x14]
	ldr r1, [r8, r4, lsl #3]
	bl strcasecmp
	cmp r0, #0
	bne _0225352C
	ldr r0, _02253558 // =0x0225A314
	ldr r2, [r0, r4, lsl #3]
	cmp r2, #0
	beq _02253524
	mov r0, r6
	mov r1, r5
	blx r2
_02253524:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0225352C:
	ldr r0, [r7]
	add r4, r4, #1
	cmp r4, r0
	blt _022534F4
_0225353C:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02253544: .word 0x0225AA84
_02253548: .word 0x0225AA94
_0225354C: .word 0x0225AA24
_02253550: .word 0x02259CA0
_02253554: .word 0x0225A310
_02253558: .word 0x0225A314
	arm_func_end FUN_022534B8

	arm_func_start FUN_0225355C
FUN_0225355C: // 0x0225355C
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	bl FUN_0224E52C
	cmp r0, #0
	bne _02253588
	mov r0, r5
	mov r1, r4
	bl FUN_0224B3F8
	cmp r0, #0
	beq _02253590
_02253588:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
_02253590:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end FUN_0225355C

	arm_func_start FUN_02253598
FUN_02253598: // 0x02253598
	stmfd sp!, {r4, r5, lr}
	sub sp, sp, #0x14
	movs r4, r0
	mov r5, r1
	bne _022535C0
	ldr r0, _02253660 // =0x0225AAA0
	ldr r1, _02253664 // =0x0225AA94
	ldr r2, _02253668 // =0x0225A954
	mov r3, #0x73
	bl __msl_assertion_failed
_022535C0:
	ldr r0, [r4, #8]
	cmp r0, #0
	addne sp, sp, #0x14
	ldmneia sp!, {r4, r5, pc}
	mov r1, #0
	str r1, [r4]
	str r1, [r4, #4]
	mov ip, #1
	str ip, [r4, #8]
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _02253618
	ldr ip, [r4, #0x14]
	cmp ip, #0
	addeq sp, sp, #0x14
	ldmeqia sp!, {r4, r5, pc}
	ldr r3, [r4, #0x18]
	mov r0, r4
	mov r2, r1
	blx ip
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, pc}
_02253618:
	ldr r0, [r4, #0x7fc]
	cmp r0, #0
	addeq sp, sp, #0x14
	ldmeqia sp!, {r4, r5, pc}
	str r5, [sp, #0x10]
	ldr r2, [r4, #0x808]
	mov r0, #4
	str r2, [sp]
	str r1, [sp, #4]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	ldr r2, [r4, #0x7fc]
	add r3, sp, #0x10
	mov r0, r4
	mov r1, ip
	bl FUN_02248C2C
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_02253660: .word 0x0225AAA0
_02253664: .word 0x0225AA94
_02253668: .word 0x0225A954
	arm_func_end FUN_02253598

	arm_func_start FUN_0225366C
FUN_0225366C: // 0x0225366C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	movs r6, r0
	mov r5, r1
	bne _02253694
	ldr r0, _0225374C // =0x0225AAA0
	ldr r1, _02253750 // =0x0225AA94
	ldr r2, _02253754 // =0x0225A8A0
	mov r3, #0x9a
	bl __msl_assertion_failed
_02253694:
	ldr r0, [r6, #0x20]
	cmp r0, #1
	bne _02253730
	add r0, r6, #0x1c
	bl FUN_0225699C
	add r0, r6, #0x1c
	bl FUN_02257A58
	movs r4, r0
	beq _02253718
	mov sb, #0
	mov r8, #4
	add r7, sp, #0x10
_022536C4:
	ldr r0, [r6, #0x7f8]
	cmp r0, #0
	beq _022536FC
	ldr r1, [r4]
	mov r0, r6
	str r1, [sp, #0x10]
	ldr r2, [r6, #0x808]
	mov r1, sb
	stmia sp, {r2, sb}
	str sb, [sp, #8]
	str r8, [sp, #0xc]
	ldr r2, [r6, #0x7f8]
	mov r3, r7
	bl FUN_02248C2C
_022536FC:
	mov r0, r6
	mov r1, r4
	bl FUN_022534B8
	add r0, r6, #0x1c
	bl FUN_02257A58
	movs r4, r0
	bne _022536C4
_02253718:
	ldr r0, [r6, #0x20]
	cmp r0, #2
	bne _02253730
	ldr r1, _02253758 // =0x0225AAB0
	mov r0, r6
	bl FUN_02253598
_02253730:
	mov r0, r6
	bl FUN_0224E474
	mov r0, r6
	mov r1, r5
	bl FUN_0224B09C
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_0225374C: .word 0x0225AAA0
_02253750: .word 0x0225AA94
_02253754: .word 0x0225A8A0
_02253758: .word 0x0225AAB0
	arm_func_end FUN_0225366C

	arm_func_start FUN_0225375C
FUN_0225375C: // 0x0225375C
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #4
	movs r4, r0
	bne _02253780
	ldr r0, _0225381C // =0x0225AAA0
	ldr r1, _02253820 // =0x0225AA94
	ldr r2, _02253824 // =0x0225A8A8
	mov r3, #0xce
	bl __msl_assertion_failed
_02253780:
	ldr r0, [r4, #0x8a8]
	cmp r0, #0
	bne _022537C0
	add r0, r4, #0x36c
	bl FUN_02255988
	movs r1, r0
	beq _022537B8
	mov r3, #0
	mov r0, r4
	add r2, r4, #0x36c
	str r3, [sp]
	bl FUN_02255A08
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_022537B8:
	add r2, r4, #0x36c
	b _02253808
_022537C0:
	cmp r0, #2
	ldreq r0, [r4, #0x4ac]
	cmpeq r0, #0
	bne _02253804
	add r0, r4, #0x4f0
	bl FUN_02255988
	movs r1, r0
	beq _022537FC
	mov r3, #0
	mov r0, r4
	add r2, r4, #0x4f0
	str r3, [sp]
	bl FUN_02255A08
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_022537FC:
	add r2, r4, #0x4f0
	b _02253808
_02253804:
	ldr r2, _02253828 // =0x0225AAC0
_02253808:
	ldr r1, _0225382C // =0x0225AAC4
	add r0, r4, #0x1c
	bl FUN_02256E70
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_0225381C: .word 0x0225AAA0
_02253820: .word 0x0225AA94
_02253824: .word 0x0225A8A8
_02253828: .word 0x0225AAC0
_0225382C: .word 0x0225AAC4
	arm_func_end FUN_0225375C

	arm_func_start FUN_02253830
FUN_02253830: // 0x02253830
	stmfd sp!, {r4, lr}
	sub sp, sp, #8
	movs r4, r0
	bne _02253854
	ldr r0, _02253888 // =0x0225AAA0
	ldr r1, _0225388C // =0x0225AA94
	ldr r2, _02253890 // =0x0225A8B4
	mov r3, #0x104
	bl __msl_assertion_failed
_02253854:
	add r0, r4, #0x374
	add r0, r0, #0x400
	add r2, r4, #0x2c
	ldr r1, _02253894 // =0x0225AACC
	ldr r3, _02253898 // =0x0225AAE0
	str r0, [sp]
	add ip, r4, #0x3ac
	add r0, r4, #0x1c
	add r2, r2, #0x400
	str ip, [sp, #4]
	bl FUN_02256E70
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.align 2, 0
_02253888: .word 0x0225AAA0
_0225388C: .word 0x0225AA94
_02253890: .word 0x0225A8B4
_02253894: .word 0x0225AACC
_02253898: .word 0x0225AAE0
	arm_func_end FUN_02253830

	arm_func_start FUN_0225389C
FUN_0225389C: // 0x0225389C
	stmfd sp!, {r4, lr}
	mov r4, r0
	bl FUN_02253830
	mov r0, r4
	bl FUN_0225375C
	ldmia sp!, {r4, pc}
	arm_func_end FUN_0225389C
_022538B4:
	.byte 0x18, 0x40, 0x2D, 0xE9, 0x2C, 0xD0, 0x4D, 0xE2, 0x00, 0x40, 0xB0, 0xE1
	.byte 0x04, 0x00, 0x00, 0x1A, 0xCC, 0x00, 0x9F, 0xE5, 0xCC, 0x10, 0x9F, 0xE5, 0xCC, 0x20, 0x9F, 0xE5
	.byte 0x46, 0x3F, 0xA0, 0xE3, 0xE8, 0x01, 0xFA, 0xEB, 0xA8, 0x08, 0x94, 0xE5, 0x03, 0x00, 0x50, 0xE3
	.byte 0x06, 0x00, 0x00, 0x1A, 0xB8, 0x10, 0x9F, 0xE5, 0x1C, 0x00, 0x84, 0xE2, 0x57, 0x2E, 0x84, 0xE2
	.byte 0x67, 0x3E, 0x84, 0xE2, 0x5D, 0x0D, 0x00, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8
	.byte 0x55, 0x0E, 0x84, 0xE2, 0x95, 0x14, 0xFA, 0xEB, 0x00, 0x10, 0xA0, 0xE1, 0x08, 0x20, 0x8D, 0xE2
	.byte 0x55, 0x0E, 0x84, 0xE2, 0xB6, 0x5B, 0xFE, 0xEB, 0xA8, 0x08, 0x94, 0xE5, 0x01, 0x00, 0x50, 0xE3
	.byte 0x08, 0x00, 0x00, 0x1A, 0x08, 0x00, 0x8D, 0xE2, 0x00, 0x00, 0x8D, 0xE5, 0x74, 0x10, 0x9F, 0xE5
	.byte 0xAC, 0x24, 0x94, 0xE5, 0x1C, 0x00, 0x84, 0xE2, 0x51, 0x3E, 0x84, 0xE2, 0x4B, 0x0D, 0x00, 0xEB
	.byte 0x2C, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8, 0x02, 0x00, 0x50, 0xE3, 0x0A, 0x00, 0x00, 0x1A
	.byte 0x4F, 0x0E, 0x84, 0xE2, 0x00, 0x00, 0x8D, 0xE5, 0x4B, 0x0E, 0x84, 0xE2, 0x04, 0x00, 0x8D, 0xE5
	.byte 0xAC, 0x24, 0x94, 0xE5, 0x40, 0x10, 0x9F, 0xE5, 0x08, 0x30, 0x8D, 0xE2, 0x1C, 0x00, 0x84, 0xE2
	.byte 0x3E, 0x0D, 0x00, 0xEB, 0x2C, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8, 0x2C, 0x00, 0x9F, 0xE5
	.byte 0x14, 0x10, 0x9F, 0xE5, 0x14, 0x20, 0x9F, 0xE5, 0x24, 0x30, 0x9F, 0xE5, 0xBA, 0x01, 0xFA, 0xEB
	.byte 0x2C, 0xD0, 0x8D, 0xE2, 0x18, 0x80, 0xBD, 0xE8, 0xA0, 0xAA, 0x25, 0x02, 0x94, 0xAA, 0x25, 0x02
	.byte 0xCC, 0xA8, 0x25, 0x02, 0xEC, 0xAA, 0x25, 0x02, 0x00, 0xAB, 0x25, 0x02, 0x10, 0xAB, 0x25, 0x02
	.byte 0x28, 0xAB, 0x25, 0x02, 0x3E, 0x01, 0x00, 0x00

	arm_func_start FUN_022539B8
FUN_022539B8: // 0x022539B8
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov fp, r0
	movs r0, r1
	ldr r0, [sp, #0x38]
	str r1, [sp]
	str r0, [sp, #0x38]
	ldr r0, [sp, #0x3c]
	str r2, [sp, #4]
	str r0, [sp, #0x3c]
	ldr r0, [sp, #0x4c]
	mov sl, r3
	str r0, [sp, #0x4c]
	ldr r0, [sp, #0x50]
	ldr sb, [sp, #0x40]
	str r0, [sp, #0x50]
	ldr r0, _02254140 // =0x0225AB2C
	ldr r8, [sp, #0x44]
	ldr r7, [sp, #0x48]
	ldr r6, [sp, #0x54]
	ldr r5, [sp, #0x58]
	str r0, [sp, #8]
	bne _02253A28
	ldr r0, _02254144 // =0x0225AB30
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254150 // =0x0000015B
	bl __msl_assertion_failed
_02253A28:
	ldr r0, [sp, #0x5c]
	cmp r0, #0
	bne _02253A48
	ldr r0, _02254154 // =0x0225AB48
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	mov r3, #0x15c
	bl __msl_assertion_failed
_02253A48:
	ldr r0, [sp, #0x68]
	cmp r0, #0
	bne _02253A68
	ldr r0, _02254158 // =0x0225AB5C
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _0225415C // =0x0000015D
	bl __msl_assertion_failed
_02253A68:
	cmp fp, #0
	bne _02253AF0
	cmp sl, #0
	bne _02253A8C
	ldr r0, _02254160 // =0x0225AB74
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254164 // =0x00000163
	bl __msl_assertion_failed
_02253A8C:
	ldrsb r0, [sl]
	cmp r0, #0
	bne _02253AAC
	ldr r0, _02254168 // =0x0225AB84
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254164 // =0x00000163
	bl __msl_assertion_failed
_02253AAC:
	mov r0, sl
	bl strlen
	cmp r0, #0x40
	blo _02253AD0
	ldr r0, _0225416C // =0x0225AB94
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254164 // =0x00000163
	bl __msl_assertion_failed
_02253AD0:
	cmp sl, #0
	ldrnesb r0, [sl]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	str sl, [sp, #8]
	b _02253D44
_02253AF0:
	cmp fp, #1
	bne _02253BAC
	ldr r0, [sp, #0x38]
	cmp r0, #0
	bgt _02253B18
	ldr r0, _02254170 // =0x0225ABAC
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254174 // =0x0000016A
	bl __msl_assertion_failed
_02253B18:
	ldr r0, [sp, #0x38]
	cmp r0, #0
	addle sp, sp, #0xc
	movle r0, #0
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r8, #0
	ldrnesb r0, [r8]
	cmpne r0, #0
	bne _02253B50
	ldr r0, _02254178 // =0x0225ABBC
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _0225417C // =0x0000016D
	bl __msl_assertion_failed
_02253B50:
	cmp r8, #0
	ldrnesb r0, [r8]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0
	ldrnesb r0, [r7]
	cmpne r0, #0
	bne _02253B8C
	ldr r0, _02254180 // =0x0225ABD8
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	mov r3, #0x170
	bl __msl_assertion_failed
_02253B8C:
	cmp r7, #0
	ldrnesb r0, [r7]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	str r8, [sp, #8]
	b _02253D44
_02253BAC:
	cmp fp, #2
	bne _02253CAC
	ldr r0, [sp, #0x38]
	cmp r0, #0
	bge _02253BD4
	ldr r0, _02254184 // =0x0225ABF0
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254188 // =0x00000177
	bl __msl_assertion_failed
_02253BD4:
	ldr r0, [sp, #0x38]
	cmp r0, #0
	addlt sp, sp, #0xc
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	bne _02253C10
	ldr r0, _0225418C // =0x0225AC04
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254190 // =0x0000017A
	bl __msl_assertion_failed
_02253C10:
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sb, #0
	ldrnesb r0, [sb]
	cmpne r0, #0
	bne _02253C50
	ldr r0, _02254194 // =0x0225AC18
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _02254198 // =0x0000017D
	bl __msl_assertion_failed
_02253C50:
	cmp sb, #0
	ldrnesb r0, [sb]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0
	ldrnesb r0, [r7]
	cmpne r0, #0
	bne _02253C8C
	ldr r0, _02254180 // =0x0225ABD8
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	mov r3, #0x180
	bl __msl_assertion_failed
_02253C8C:
	cmp r7, #0
	ldrnesb r0, [r7]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	str sb, [sp, #8]
	b _02253D44
_02253CAC:
	cmp fp, #3
	bne _02253D44
	ldr r0, [sp, #0x4c]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	bne _02253CDC
	ldr r0, _0225419C // =0x0225AC38
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _022541A0 // =0x00000187
	bl __msl_assertion_failed
_02253CDC:
	ldr r0, [sp, #0x4c]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #0x50]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	bne _02253D20
	ldr r0, _022541A4 // =0x0225AC54
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _022541A8 // =0x0000018A
	bl __msl_assertion_failed
_02253D20:
	ldr r0, [sp, #0x50]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _022541AC // =0x0225AC7C
	str r0, [sp, #8]
_02253D44:
	cmp fp, #0
	beq _02253DC4
	cmp r6, #0
	ldrnesb r0, [r6]
	cmpne r0, #0
	bne _02253D70
	ldr r0, _022541B0 // =0x0225AC84
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	ldr r3, _022541B4 // =0x00000191
	bl __msl_assertion_failed
_02253D70:
	cmp r6, #0
	ldrnesb r0, [r6]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	bne _02253DAC
	ldr r0, _022541B8 // =0x0225AC9C
	ldr r1, _02254148 // =0x0225AA94
	ldr r2, _0225414C // =0x0225A908
	mov r3, #0x194
	bl __msl_assertion_failed
_02253DAC:
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02253DC4:
	bl SocketStartUp
	ldr r0, _022541BC // =0x000008B4
	bl DWCi_GsMalloc
	movs r4, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, #0
	ldr r2, _022541BC // =0x000008B4
	bl memset
	str fp, [r4, #0x8a8]
	cmp sl, #0
	beq _02253E10
	mov r1, sl
	add r0, r4, #0x36c
	mov r2, #0x40
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x3ab]
_02253E10:
	ldr r1, [sp, #0x30]
	cmp r1, #0
	beq _02253E34
	add r0, r4, #0x2c
	add r0, r0, #0x400
	mov r2, #0x80
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x4ab]
_02253E34:
	ldr r1, [sp, #0x34]
	cmp r1, #0
	beq _02253E54
	add r0, r4, #0x3ac
	mov r2, #0x80
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x42b]
_02253E54:
	ldr r0, [sp, #0x38]
	str r0, [r4, #0x4ac]
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	beq _02253E80
	mov r1, r0
	add r0, r4, #0x4b0
	mov r2, #0x40
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x4ef]
_02253E80:
	cmp sb, #0
	beq _02253EA0
	mov r1, sb
	add r0, r4, #0x4f0
	mov r2, #0x20
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x50f]
_02253EA0:
	cmp r8, #0
	beq _02253EC0
	mov r1, r8
	add r0, r4, #0x510
	mov r2, #0x40
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x54f]
_02253EC0:
	cmp r7, #0
	beq _02253EE0
	mov r1, r7
	add r0, r4, #0x550
	mov r2, #0x20
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x56f]
_02253EE0:
	ldr r0, [sp, #0x4c]
	cmp r0, #0
	beq _02253F04
	mov r1, r0
	add r0, r4, #0x570
	mov r2, #0x100
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x66f]
_02253F04:
	ldr r0, [sp, #0x50]
	cmp r0, #0
	beq _02253F28
	mov r1, r0
	add r0, r4, #0x670
	mov r2, #0x100
	bl strncpy
	mov r0, #0
	strb r0, [r4, #0x76f]
_02253F28:
	add r0, r4, #0x374
	ldr r1, [sp]
	add r0, r0, #0x400
	mov r2, #0x80
	bl strncpy
	mov r7, #0
	add r1, r4, #0x3f8
	ldr sb, [sp, #0x5c]
	ldr r0, [sp, #4]
	strb r7, [r4, #0x7f3]
	str r0, [r4, #0x7f4]
	add r8, r1, #0x400
	ldmia sb!, {r0, r1, r2, r3}
	stmia r8!, {r0, r1, r2, r3}
	ldr r1, [sb]
	mov r0, #1
	str r1, [r8]
	str r0, [r4, #0x81c]
	str r0, [r4, #4]
	mov r0, r4
	str r7, [r4, #0x824]
	bl FUN_0224B864
	cmp r0, #0
	mov r0, r4
	bne _02253FA0
	bl DWCi_GsFree
	bl SocketShutDown
	add sp, sp, #0xc
	mov r0, r7
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02253FA0:
	bl FUN_02248A98
	cmp r0, #0
	bne _02253FCC
	mov r0, r4
	bl FUN_0224B8EC
	mov r0, r4
	bl DWCi_GsFree
	bl SocketShutDown
	add sp, sp, #0xc
	mov r0, r7
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02253FCC:
	ldr r1, [sp, #8]
	add r0, r4, #0x1c
	bl FUN_02255DEC
	cmp r0, #0
	bne _02254008
	mov r0, r4
	bl FUN_02248AC8
	mov r0, r4
	bl FUN_0224B8EC
	mov r0, r4
	bl DWCi_GsFree
	bl SocketShutDown
	add sp, sp, #0xc
	mov r0, r7
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02254008:
	ldr r1, [sp]
	ldr r2, [sp, #4]
	add r0, r4, #0x1c
	bl FUN_02255E68
	cmp r0, #0
	bne _02254050
	add r0, r4, #0x1c
	bl FUN_022561B0
	mov r0, r4
	bl FUN_02248AC8
	mov r0, r4
	bl FUN_0224B8EC
	mov r0, r4
	bl DWCi_GsFree
	bl SocketShutDown
	add sp, sp, #0xc
	mov r0, r7
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02254050:
	ldr r1, [sp, #0x60]
	ldr r0, [sp, #0x64]
	str r1, [r4, #0xc]
	str r0, [r4, #0x10]
	ldr r1, [sp, #0x68]
	ldr r0, [sp, #0x6c]
	str r1, [r4, #0x14]
	str r0, [r4, #0x18]
	cmp r6, #0
	ldrnesb r0, [r6]
	cmpne r0, #0
	cmpne r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	beq _022540C4
	add r0, r4, #0x28
	mov r1, r5
	add r0, r0, #0x800
	mov r2, #0x80
	bl strncpy
	mov r1, r7
	strb r1, [r4, #0x8a7]
	ldr r0, _022541C0 // =0x0225A89C
	ldr r1, _022541C4 // =0x0225ACB8
	ldr r2, [r0]
	mov r3, r6
	add r0, r4, #0x1c
	bl FUN_02256E70
	b _022540E8
_022540C4:
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _022540E0
	ldr r1, _022541C8 // =0x0225ACC8
	add r0, r4, #0x1c
	bl FUN_02256BB0
	b _022540E8
_022540E0:
	mov r0, r4
	bl FUN_0225389C
_022540E8:
	ldr r0, [sp, #0x70]
	cmp r0, #0
	beq _02254134
	mov r6, #0
	mov r5, #0xa
_022540FC:
	mov r0, r4
	mov r1, r6
	bl FUN_0225366C
	mov r0, r5
	bl msleep
	ldr r0, [r4, #4]
	cmp r0, #0
	bne _022540FC
	ldr r0, [r4]
	cmp r0, #0
	bne _02254134
	mov r0, r4
	bl FUN_022543DC
	mov r4, #0
_02254134:
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_02254140: .word 0x0225AB2C
_02254144: .word 0x0225AB30
_02254148: .word 0x0225AA94
_0225414C: .word 0x0225A908
_02254150: .word 0x0000015B
_02254154: .word 0x0225AB48
_02254158: .word 0x0225AB5C
_0225415C: .word 0x0000015D
_02254160: .word 0x0225AB74
_02254164: .word 0x00000163
_02254168: .word 0x0225AB84
_0225416C: .word 0x0225AB94
_02254170: .word 0x0225ABAC
_02254174: .word 0x0000016A
_02254178: .word 0x0225ABBC
_0225417C: .word 0x0000016D
_02254180: .word 0x0225ABD8
_02254184: .word 0x0225ABF0
_02254188: .word 0x00000177
_0225418C: .word 0x0225AC04
_02254190: .word 0x0000017A
_02254194: .word 0x0225AC18
_02254198: .word 0x0000017D
_0225419C: .word 0x0225AC38
_022541A0: .word 0x00000187
_022541A4: .word 0x0225AC54
_022541A8: .word 0x0000018A
_022541AC: .word 0x0225AC7C
_022541B0: .word 0x0225AC84
_022541B4: .word 0x00000191
_022541B8: .word 0x0225AC9C
_022541BC: .word 0x000008B4
_022541C0: .word 0x0225A89C
_022541C4: .word 0x0225ACB8
_022541C8: .word 0x0225ACC8
	arm_func_end FUN_022539B8

	arm_func_start FUN_022541CC
FUN_022541CC: // 0x022541CC
	stmfd sp!, {r3, r4, lr}
	sub sp, sp, #0x44
	mov ip, r0
	mov r0, #0
	stmia sp, {r0, r3}
	str r0, [sp, #8]
	str r0, [sp, #0xc]
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	str r0, [sp, #0x18]
	str r0, [sp, #0x1c]
	ldr lr, [sp, #0x50]
	str r0, [sp, #0x20]
	str lr, [sp, #0x24]
	ldr r3, [sp, #0x54]
	mov r4, r1
	str r3, [sp, #0x28]
	ldr lr, [sp, #0x58]
	mov r3, r2
	str lr, [sp, #0x2c]
	ldr r2, [sp, #0x5c]
	ldr r1, [sp, #0x60]
	str r2, [sp, #0x30]
	str r1, [sp, #0x34]
	ldr r2, [sp, #0x64]
	ldr r1, [sp, #0x68]
	str r2, [sp, #0x38]
	str r1, [sp, #0x3c]
	ldr lr, [sp, #0x6c]
	mov r1, ip
	mov r2, r4
	str lr, [sp, #0x40]
	bl FUN_022539B8
	add sp, sp, #0x44
	ldmia sp!, {r3, r4, pc}
	arm_func_end FUN_022541CC

	arm_func_start FUN_02254258
FUN_02254258: // 0x02254258
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _0225427C
	ldr r0, _0225430C // =0x0225AAA0
	ldr r1, _02254310 // =0x0225AA94
	ldr r2, _02254314 // =0x0225A968
	ldr r3, _02254318 // =0x00000385
	bl __msl_assertion_failed
_0225427C:
	ldr r0, [r5]
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	bne _022542B8
	mov r1, #0
	str r1, [r5, #4]
	ldr ip, [r5, #0x14]
	cmp ip, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r3, [r5, #0x18]
	mov r0, r5
	mov r2, #1
	blx ip
	ldmia sp!, {r3, r4, r5, pc}
_022542B8:
	mov r1, r4
	add r0, r5, #0x36c
	mov r2, #0x40
	bl strncpy
	mov r1, #0
	mov r0, r4
	strb r1, [r5, #0x3ab]
	bl FUN_02255988
	movs r1, r0
	beq _022542F8
	mov r3, #0
	mov r0, r5
	mov r2, r4
	str r3, [sp]
	bl FUN_02255A08
	ldmia sp!, {r3, r4, r5, pc}
_022542F8:
	ldr r1, _0225431C // =0x0225ACD0
	mov r2, r4
	add r0, r5, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_0225430C: .word 0x0225AAA0
_02254310: .word 0x0225AA94
_02254314: .word 0x0225A968
_02254318: .word 0x00000385
_0225431C: .word 0x0225ACD0
	arm_func_end FUN_02254258

	arm_func_start FUN_02254320
FUN_02254320: // 0x02254320
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bne _0225434C
	ldr r0, _022543C4 // =0x0225AAA0
	ldr r1, _022543C8 // =0x0225AA94
	ldr r2, _022543CC // =0x0225AA3C
	ldr r3, _022543D0 // =0x000003BD
	bl __msl_assertion_failed
_0225434C:
	ldr r0, [r7]
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r5, #0
	bne _02254388
	mov r1, #0
	str r1, [r7, #4]
	ldr ip, [r7, #0x14]
	cmp ip, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r3, [r7, #0x18]
	mov r0, r7
	mov r2, #1
	blx ip
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02254388:
	cmp r4, #0
	ldreq r4, _022543D4 // =0x0225AB2C
	ldr r1, _022543D8 // =0x0225ACDC
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	str r4, [sp]
	bl FUN_02256E70
	mov r1, r5
	add r0, r7, #0x510
	mov r2, #0x40
	bl strncpy
	mov r0, #0
	strb r0, [r7, #0x54f]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022543C4: .word 0x0225AAA0
_022543C8: .word 0x0225AA94
_022543CC: .word 0x0225AA3C
_022543D0: .word 0x000003BD
_022543D4: .word 0x0225AB2C
_022543D8: .word 0x0225ACDC
	arm_func_end FUN_02254320

	arm_func_start FUN_022543DC
FUN_022543DC: // 0x022543DC
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _022543FC
	ldr r0, _02254474 // =0x0225AAA0
	ldr r1, _02254478 // =0x0225AA94
	ldr r2, _0225447C // =0x0225A8E8
	ldr r3, _02254480 // =0x000003EF
	bl __msl_assertion_failed
_022543FC:
	mov r0, r4
	bl FUN_0224E5B0
	ldr r0, [r4, #8]
	cmp r0, #0
	bne _0225442C
	ldr r3, [r4, #0x7fc]
	cmp r3, #0
	beq _0225442C
	ldr r2, [r4, #0x808]
	ldr r1, _02254484 // =0x0225AB2C
	mov r0, r4
	blx r3
_0225442C:
	ldr r0, [r4]
	cmp r0, #0
	beq _0225444C
	ldr r1, _02254488 // =0x0225ACF4
	add r0, r4, #0x1c
	bl FUN_02256BB0
	add r0, r4, #0x1c
	bl FUN_0225699C
_0225444C:
	mov r0, r4
	bl FUN_0224B8EC
	mov r0, r4
	bl FUN_02248AC8
	add r0, r4, #0x1c
	bl FUN_022561B0
	mov r0, r4
	bl DWCi_GsFree
	bl SocketShutDown
	ldmia sp!, {r4, pc}
	.align 2, 0
_02254474: .word 0x0225AAA0
_02254478: .word 0x0225AA94
_0225447C: .word 0x0225A8E8
_02254480: .word 0x000003EF
_02254484: .word 0x0225AB2C
_02254488: .word 0x0225ACF4
	arm_func_end FUN_022543DC

	arm_func_start FUN_0225448C
FUN_0225448C: // 0x0225448C
	ldr ip, _02254498 // =FUN_0225366C
	mov r1, #0
	bx ip
	.align 2, 0
_02254498: .word FUN_0225366C
	arm_func_end FUN_0225448C

	arm_func_start FUN_0225449C
FUN_0225449C: // 0x0225449C
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	movs r4, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bne _022544C8
	ldr r0, _02254564 // =0x0225AAA0
	ldr r1, _02254568 // =0x0225AA94
	ldr r2, _0225456C // =0x0225A918
	ldr r3, _02254570 // =0x0000058E
	bl __msl_assertion_failed
_022544C8:
	cmp r4, #0
	ldrne r0, [r4]
	cmpne r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r5, #0
	cmpeq r6, #0
	bne _022544F8
	ldr r0, _02254574 // =0x0225AD64
	ldr r1, _02254568 // =0x0225AA94
	ldr r2, _0225456C // =0x0225A918
	ldr r3, _02254578 // =0x00000591
	bl __msl_assertion_failed
_022544F8:
	cmp r7, #0
	ldreq r7, _0225457C // =0x0225AB2C
	ldr r1, _02254580 // =0x0225AD94
	mov r2, r7
	add r0, r4, #0x1c
	bl FUN_02256E70
	ldr r3, [sp, #0x18]
	mov r0, r4
	mov r1, r6
	mov r2, r5
	bl FUN_0224E6EC
	ldr r1, [sp, #0x1c]
	mov r6, r0
	cmp r1, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r5, #0xa
_02254538:
	mov r0, r4
	mov r1, r6
	bl FUN_0225366C
	mov r0, r5
	bl msleep
	mov r0, r4
	mov r1, r6
	bl FUN_0225355C
	cmp r0, #0
	bne _02254538
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02254564: .word 0x0225AAA0
_02254568: .word 0x0225AA94
_0225456C: .word 0x0225A918
_02254570: .word 0x0000058E
_02254574: .word 0x0225AD64
_02254578: .word 0x00000591
_0225457C: .word 0x0225AB2C
_02254580: .word 0x0225AD94
	arm_func_end FUN_0225449C

	arm_func_start FUN_02254584
FUN_02254584: // 0x02254584
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	movs r4, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bne _022545B4
	ldr r0, _022546A8 // =0x0225AAA0
	ldr r1, _022546AC // =0x0225AA94
	ldr r2, _022546B0 // =0x0225A92C
	ldr r3, _022546B4 // =0x000005B3
	bl __msl_assertion_failed
_022545B4:
	cmp r4, #0
	ldrne r0, [r4]
	cmpne r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r7, #0
	bne _022545E4
	ldr r0, _022546B8 // =0x0225AD9C
	ldr r1, _022546AC // =0x0225AA94
	ldr r2, _022546B0 // =0x0225A92C
	ldr r3, _022546BC // =0x000005B6
	bl __msl_assertion_failed
_022545E4:
	ldrsb r0, [r7]
	cmp r0, #0
	bne _02254604
	ldr r0, _022546C0 // =0x0225ADAC
	ldr r1, _022546AC // =0x0225AA94
	ldr r2, _022546B0 // =0x0225A92C
	ldr r3, _022546BC // =0x000005B6
	bl __msl_assertion_failed
_02254604:
	cmp r5, #0
	bne _02254620
	ldr r0, _022546C4 // =0x0225AB48
	ldr r1, _022546AC // =0x0225AA94
	ldr r2, _022546B0 // =0x0225A92C
	ldr r3, _022546C8 // =0x000005B7
	bl __msl_assertion_failed
_02254620:
	cmp r6, #0
	ldreq r6, _022546CC // =0x0225AB2C
	ldr r1, _022546D0 // =0x0225ADC0
	mov r2, r7
	mov r3, r6
	add r0, r4, #0x1c
	bl FUN_02256E70
	ldr r2, [sp, #0x20]
	ldr r3, [sp, #0x24]
	mov r0, r4
	mov r1, r7
	stmia sp, {r5, r6}
	bl FUN_0224E770
	mov r6, r0
	mov r0, r4
	mov r1, r7
	bl FUN_0224B940
	ldr r0, [sp, #0x28]
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r5, #0xa
_02254678:
	mov r0, r4
	mov r1, r6
	bl FUN_0225366C
	mov r0, r5
	bl msleep
	mov r0, r4
	mov r1, r6
	bl FUN_0225355C
	cmp r0, #0
	bne _02254678
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022546A8: .word 0x0225AAA0
_022546AC: .word 0x0225AA94
_022546B0: .word 0x0225A92C
_022546B4: .word 0x000005B3
_022546B8: .word 0x0225AD9C
_022546BC: .word 0x000005B6
_022546C0: .word 0x0225ADAC
_022546C4: .word 0x0225AB48
_022546C8: .word 0x000005B7
_022546CC: .word 0x0225AB2C
_022546D0: .word 0x0225ADC0
	arm_func_end FUN_02254584

	arm_func_start FUN_022546D4
FUN_022546D4: // 0x022546D4
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	mov r5, r1
	mov r4, r2
	bne _022546FC
	ldr r0, _02254774 // =0x0225AAA0
	ldr r1, _02254778 // =0x0225AA94
	ldr r2, _0225477C // =0x0225A940
	ldr r3, _02254780 // =0x000005DB
	bl __msl_assertion_failed
_022546FC:
	cmp r6, #0
	ldrne r0, [r6]
	cmpne r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	cmp r5, #0
	bne _02254728
	ldr r0, _02254784 // =0x0225AD9C
	ldr r1, _02254778 // =0x0225AA94
	ldr r2, _0225477C // =0x0225A940
	ldr r3, _02254788 // =0x000005DE
	bl __msl_assertion_failed
_02254728:
	ldrsb r0, [r5]
	cmp r0, #0
	bne _02254748
	ldr r0, _0225478C // =0x0225ADAC
	ldr r1, _02254778 // =0x0225AA94
	ldr r2, _0225477C // =0x0225A940
	ldr r3, _02254788 // =0x000005DE
	bl __msl_assertion_failed
_02254748:
	cmp r4, #0
	ldreq r4, _02254790 // =0x0225AB2C
	ldr r1, _02254794 // =0x0225ADCC
	mov r2, r5
	mov r3, r4
	add r0, r6, #0x1c
	bl FUN_02256E70
	mov r0, r6
	mov r1, r5
	bl FUN_0224BC80
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02254774: .word 0x0225AAA0
_02254778: .word 0x0225AA94
_0225477C: .word 0x0225A940
_02254780: .word 0x000005DB
_02254784: .word 0x0225AD9C
_02254788: .word 0x000005DE
_0225478C: .word 0x0225ADAC
_02254790: .word 0x0225AB2C
_02254794: .word 0x0225ADCC
	arm_func_end FUN_022546D4

	arm_func_start FUN_02254798
FUN_02254798: // 0x02254798
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x20
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bne _022547C8
	ldr r0, _02254948 // =0x0225AAA0
	ldr r1, _0225494C // =0x0225AA94
	ldr r2, _02254950 // =0x0225AA6C
	ldr r3, _02254954 // =0x000005FC
	bl __msl_assertion_failed
_022547C8:
	cmp r7, #0
	ldrne r0, [r7]
	cmpne r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	bne _022547F8
	ldr r0, _02254958 // =0x0225AD9C
	ldr r1, _0225494C // =0x0225AA94
	ldr r2, _02254950 // =0x0225AA6C
	ldr r3, _0225495C // =0x000005FF
	bl __msl_assertion_failed
_022547F8:
	ldrsb r0, [r6]
	cmp r0, #0
	bne _02254818
	ldr r0, _02254960 // =0x0225ADAC
	ldr r1, _0225494C // =0x0225AA94
	ldr r2, _02254950 // =0x0225AA6C
	ldr r3, _0225495C // =0x000005FF
	bl __msl_assertion_failed
_02254818:
	cmp r4, #4
	bls _02254834
	ldr r0, _02254964 // =0x0225ADD8
	ldr r1, _0225494C // =0x0225AA94
	ldr r2, _02254950 // =0x0225AA6C
	mov r3, #0x600
	bl __msl_assertion_failed
_02254834:
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r4, #0
	bne _02254868
	ldr r1, _02254968 // =0x0225AE50
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	b _022548E8
_02254868:
	cmp r4, #1
	bne _02254888
	ldr r1, _0225496C // =0x0225AE60
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	b _022548E8
_02254888:
	cmp r4, #2
	bne _022548A8
	ldr r1, _02254970 // =0x0225AE78
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	b _022548E8
_022548A8:
	cmp r4, #3
	bne _022548C8
	ldr r1, _02254974 // =0x0225AE88
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	b _022548E8
_022548C8:
	cmp r4, #4
	addne sp, sp, #0x20
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _02254978 // =0x0225AE94
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
_022548E8:
	mov r0, r7
	mov r1, r6
	bl FUN_0224BD94
	cmp r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r1, r7, #0x36c
	str r1, [sp, #0x14]
	str r6, [sp, #0x10]
	str r5, [sp, #0x18]
	str r4, [sp, #0x1c]
	ldr r2, [r0, #0x2c]
	mov r1, #0
	str r2, [sp]
	stmib sp, {r1, r6}
	mov r1, #0x10
	str r1, [sp, #0xc]
	ldr r2, [r0]
	add r3, sp, #0x10
	mov r0, r7
	mov r1, #4
	bl FUN_02248C2C
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02254948: .word 0x0225AAA0
_0225494C: .word 0x0225AA94
_02254950: .word 0x0225AA6C
_02254954: .word 0x000005FC
_02254958: .word 0x0225AD9C
_0225495C: .word 0x000005FF
_02254960: .word 0x0225ADAC
_02254964: .word 0x0225ADD8
_02254968: .word 0x0225AE50
_0225496C: .word 0x0225AE60
_02254970: .word 0x0225AE78
_02254974: .word 0x0225AE88
_02254978: .word 0x0225AE94
	arm_func_end FUN_02254798

	arm_func_start FUN_0225497C
FUN_0225497C: // 0x0225497C
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x40
	movs r6, r0
	mov r5, r1
	mov r4, r2
	bne _022549A8
	ldr r0, _02254B4C // =0x0225AAA0
	ldr r1, _02254B50 // =0x0225AA94
	ldr r2, _02254B54 // =0x0225A9CC
	ldr r3, _02254B58 // =0x0000067F
	bl __msl_assertion_failed
_022549A8:
	cmp r6, #0
	ldrne r0, [r6]
	cmpne r0, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r4, r5, r6, pc}
	cmp r5, #0
	bne _022549D8
	ldr r0, _02254B5C // =0x0225AD9C
	ldr r1, _02254B50 // =0x0225AA94
	ldr r2, _02254B54 // =0x0225A9CC
	ldr r3, _02254B60 // =0x00000682
	bl __msl_assertion_failed
_022549D8:
	ldrsb r0, [r5]
	cmp r0, #0
	bne _022549F8
	ldr r0, _02254B64 // =0x0225ADAC
	ldr r1, _02254B50 // =0x0225AA94
	ldr r2, _02254B54 // =0x0225A9CC
	ldr r3, _02254B60 // =0x00000682
	bl __msl_assertion_failed
_022549F8:
	cmp r4, #0
	bne _02254A14
	ldr r0, _02254B68 // =0x0225AEB4
	ldr r1, _02254B50 // =0x0225AA94
	ldr r2, _02254B54 // =0x0225A9CC
	ldr r3, _02254B6C // =0x00000683
	bl __msl_assertion_failed
_02254A14:
	ldr r3, _02254B70 // =0x0225AEC4
	add ip, sp, #0
	mov r2, #8
_02254A20:
	ldrb r1, [r3]
	ldrb r0, [r3, #1]
	add r3, r3, #2
	strb r1, [ip]
	strb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	bne _02254A20
	ldrb r0, [r3]
	strb r0, [ip]
	ldr r0, [r4]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp]
	moveq r0, #0x2d
	streqb r0, [sp]
	ldr r0, [r4, #4]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #2]
	moveq r0, #0x2d
	streqb r0, [sp, #2]
	ldr r0, [r4, #8]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #4]
	moveq r0, #0x2d
	streqb r0, [sp, #4]
	ldr r0, [r4, #0xc]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #6]
	moveq r0, #0x2d
	streqb r0, [sp, #6]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #8]
	moveq r0, #0x2d
	streqb r0, [sp, #8]
	ldr r0, [r4, #0x14]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #0xa]
	moveq r0, #0x2d
	streqb r0, [sp, #0xa]
	ldr r0, [r4, #0x1c]
	cmp r0, #0
	movgt r0, #0x2b
	strgtb r0, [sp, #0xc]
	movle r0, #0x2d
	strleb r0, [sp, #0xc]
	ldr r0, [r4, #0x18]
	cmp r0, #0
	movne r0, #0x2b
	strneb r0, [sp, #0xe]
	moveq r0, #0x2d
	streqb r0, [sp, #0xe]
	ldr r4, [r4, #0x1c]
	cmp r4, #0
	ble _02254B30
	add r0, sp, #0
	bl strlen
	add r3, sp, #0
	ldr r1, _02254B74 // =0x0225AED8
	mov r2, r4
	add r0, r3, r0
	bl sprintf
_02254B30:
	ldr r1, _02254B78 // =0x0225AEDC
	add r3, sp, #0
	mov r2, r5
	add r0, r6, #0x1c
	bl FUN_02256E70
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02254B4C: .word 0x0225AAA0
_02254B50: .word 0x0225AA94
_02254B54: .word 0x0225A9CC
_02254B58: .word 0x0000067F
_02254B5C: .word 0x0225AD9C
_02254B60: .word 0x00000682
_02254B64: .word 0x0225ADAC
_02254B68: .word 0x0225AEB4
_02254B6C: .word 0x00000683
_02254B70: .word 0x0225AEC4
_02254B74: .word 0x0225AED8
_02254B78: .word 0x0225AEDC
	arm_func_end FUN_0225497C

	arm_func_start FUN_02254B7C
FUN_02254B7C: // 0x02254B7C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x3c
	movs r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bne _02254BAC
	ldr r0, _02254D40 // =0x0225AAA0
	ldr r1, _02254D44 // =0x0225AA94
	ldr r2, _02254D48 // =0x0225A9E0
	ldr r3, _02254D4C // =0x000006C2
	bl __msl_assertion_failed
_02254BAC:
	cmp r8, #0
	ldrne r0, [r8]
	cmpne r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	cmp r7, #0
	bne _02254BDC
	ldr r0, _02254D50 // =0x0225AD9C
	ldr r1, _02254D44 // =0x0225AA94
	ldr r2, _02254D48 // =0x0225A9E0
	ldr r3, _02254D54 // =0x000006C5
	bl __msl_assertion_failed
_02254BDC:
	ldrsb r0, [r7]
	cmp r0, #0
	bne _02254BFC
	ldr r0, _02254D58 // =0x0225ADAC
	ldr r1, _02254D44 // =0x0225AA94
	ldr r2, _02254D48 // =0x0225A9E0
	ldr r3, _02254D54 // =0x000006C5
	bl __msl_assertion_failed
_02254BFC:
	cmp r6, #0
	bne _02254C18
	ldr r0, _02254D5C // =0x0225AEA0
	ldr r1, _02254D44 // =0x0225AA94
	ldr r2, _02254D48 // =0x0225A9E0
	ldr r3, _02254D60 // =0x000006C6
	bl __msl_assertion_failed
_02254C18:
	mov r0, r8
	mov r1, r7
	bl FUN_0224C134
	cmp r0, #0
	beq _02254CD4
	add r2, sp, #0x1c
	mov r0, r8
	mov r1, r7
	bl FUN_0224C184
	cmp r0, #0
	beq _02254CD4
	mov r0, r8
	bl FUN_0224E4E0
	add r1, sp, #0x1c
	mov r2, #1
	str r2, [sp, #0x10]
	str r1, [sp, #0x18]
	str r7, [sp, #0x14]
	mov r4, r0
	str r5, [sp]
	str r4, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	mov r5, #0xc
	add r3, sp, #0x10
	mov r0, r8
	mov r2, r6
	mov r1, #0x11
	str r5, [sp, #0xc]
	bl FUN_02248C2C
	ldr r0, [sp, #0x58]
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r5, #0xa
_02254CA4:
	mov r0, r8
	mov r1, r4
	bl FUN_0225366C
	mov r0, r5
	bl msleep
	mov r0, r8
	mov r1, r4
	bl FUN_0225355C
	cmp r0, #0
	bne _02254CA4
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02254CD4:
	ldr r1, _02254D64 // =0x0225AEE8
	mov r2, r7
	add r0, r8, #0x1c
	bl FUN_02256E70
	mov r0, r8
	mov r1, r7
	mov r2, r6
	mov r3, r5
	bl FUN_0224E8F4
	ldr r1, [sp, #0x58]
	mov r5, r0
	cmp r1, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r4, #0xa
_02254D10:
	mov r0, r8
	mov r1, r5
	bl FUN_0225366C
	mov r0, r4
	bl msleep
	mov r0, r8
	mov r1, r5
	bl FUN_0225355C
	cmp r0, #0
	bne _02254D10
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02254D40: .word 0x0225AAA0
_02254D44: .word 0x0225AA94
_02254D48: .word 0x0225A9E0
_02254D4C: .word 0x000006C2
_02254D50: .word 0x0225AD9C
_02254D54: .word 0x000006C5
_02254D58: .word 0x0225ADAC
_02254D5C: .word 0x0225AEA0
_02254D60: .word 0x000006C6
_02254D64: .word 0x0225AEE8
	arm_func_end FUN_02254B7C

	arm_func_start FUN_02254D68
FUN_02254D68: // 0x02254D68
	stmfd sp!, {r4, r5, r6, lr}
	movs r6, r0
	mov r5, r1
	mov r4, r2
	bne _02254D90
	ldr r0, _02254E2C // =0x0225AAA0
	ldr r1, _02254E30 // =0x0225AA94
	ldr r2, _02254E34 // =0x0225A9F4
	ldr r3, _02254E38 // =0x0000074E
	bl __msl_assertion_failed
_02254D90:
	cmp r6, #0
	ldrne r0, [r6]
	cmpne r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	cmp r5, #0
	bne _02254DBC
	ldr r0, _02254E3C // =0x0225AD9C
	ldr r1, _02254E30 // =0x0225AA94
	ldr r2, _02254E34 // =0x0225A9F4
	ldr r3, _02254E40 // =0x00000751
	bl __msl_assertion_failed
_02254DBC:
	ldrsb r0, [r5]
	cmp r0, #0
	bne _02254DDC
	ldr r0, _02254E44 // =0x0225ADAC
	ldr r1, _02254E30 // =0x0225AA94
	ldr r2, _02254E34 // =0x0225A9F4
	ldr r3, _02254E40 // =0x00000751
	bl __msl_assertion_failed
_02254DDC:
	cmp r4, #0
	bge _02254DF8
	ldr r0, _02254E48 // =0x0225AEF0
	ldr r1, _02254E30 // =0x0225AA94
	ldr r2, _02254E34 // =0x0225A9F4
	ldr r3, _02254E4C // =0x00000752
	bl __msl_assertion_failed
_02254DF8:
	cmp r4, #0
	beq _02254E18
	ldr r1, _02254E50 // =0x0225AEFC
	mov r2, r5
	mov r3, r4
	add r0, r6, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r4, r5, r6, pc}
_02254E18:
	ldr r1, _02254E54 // =0x0225AF0C
	mov r2, r5
	add r0, r6, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02254E2C: .word 0x0225AAA0
_02254E30: .word 0x0225AA94
_02254E34: .word 0x0225A9F4
_02254E38: .word 0x0000074E
_02254E3C: .word 0x0225AD9C
_02254E40: .word 0x00000751
_02254E44: .word 0x0225ADAC
_02254E48: .word 0x0225AEF0
_02254E4C: .word 0x00000752
_02254E50: .word 0x0225AEFC
_02254E54: .word 0x0225AF0C
	arm_func_end FUN_02254D68

	arm_func_start FUN_02254E58
FUN_02254E58: // 0x02254E58
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _02254E7C
	ldr r0, _02254F04 // =0x0225AAA0
	ldr r1, _02254F08 // =0x0225AA94
	ldr r2, _02254F0C // =0x0225AA54
	ldr r3, _02254F10 // =0x000007D6
	bl __msl_assertion_failed
_02254E7C:
	ldr r0, [r5]
	cmp r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	bne _02254EA8
	ldr r0, _02254F14 // =0x0225AD9C
	ldr r1, _02254F08 // =0x0225AA94
	ldr r2, _02254F0C // =0x0225AA54
	ldr r3, _02254F18 // =0x000007DA
	bl __msl_assertion_failed
_02254EA8:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _02254EC8
	ldr r0, _02254F1C // =0x0225ADAC
	ldr r1, _02254F08 // =0x0225AA94
	ldr r2, _02254F0C // =0x0225AA54
	ldr r3, _02254F18 // =0x000007DA
	bl __msl_assertion_failed
_02254EC8:
	cmp r4, #0
	ldrnesb r0, [r4]
	cmpne r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5
	mov r1, r4
	bl FUN_0224C134
	cmp r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5
	mov r1, r4
	bl FUN_0224C6B8
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02254F04: .word 0x0225AAA0
_02254F08: .word 0x0225AA94
_02254F0C: .word 0x0225AA54
_02254F10: .word 0x000007D6
_02254F14: .word 0x0225AD9C
_02254F18: .word 0x000007DA
_02254F1C: .word 0x0225ADAC
	arm_func_end FUN_02254E58

	arm_func_start FUN_02254F20
FUN_02254F20: // 0x02254F20
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _02254F44
	ldr r0, _02254FB4 // =0x0225AAA0
	ldr r1, _02254FB8 // =0x0225AA94
	ldr r2, _02254FBC // =0x0225A8D8
	ldr r3, _02254FC0 // =0x000007F4
	bl __msl_assertion_failed
_02254F44:
	ldr r0, [r5]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	bne _02254F70
	ldr r0, _02254FC4 // =0x0225AD9C
	ldr r1, _02254FB8 // =0x0225AA94
	ldr r2, _02254FBC // =0x0225A8D8
	ldr r3, _02254FC8 // =0x000007F8
	bl __msl_assertion_failed
_02254F70:
	ldrsb r0, [r4]
	cmp r0, #0
	bne _02254F90
	ldr r0, _02254FCC // =0x0225ADAC
	ldr r1, _02254FB8 // =0x0225AA94
	ldr r2, _02254FBC // =0x0225A8D8
	ldr r3, _02254FC8 // =0x000007F8
	bl __msl_assertion_failed
_02254F90:
	cmp r4, #0
	ldrnesb r0, [r4]
	cmpne r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5
	mov r1, r4
	bl FUN_0224C134
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02254FB4: .word 0x0225AAA0
_02254FB8: .word 0x0225AA94
_02254FBC: .word 0x0225A8D8
_02254FC0: .word 0x000007F4
_02254FC4: .word 0x0225AD9C
_02254FC8: .word 0x000007F8
_02254FCC: .word 0x0225ADAC
	arm_func_end FUN_02254F20
_02254FD0:
	.byte 0xF8, 0x41, 0x2D, 0xE9, 0x0C, 0xD0, 0x4D, 0xE2, 0x00, 0x80, 0xB0, 0xE1, 0x01, 0x70, 0xA0, 0xE1
	.byte 0x02, 0x60, 0xA0, 0xE1, 0x03, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0x00, 0x1A, 0xDC, 0x00, 0x9F, 0xE5
	.byte 0xDC, 0x10, 0x9F, 0xE5, 0xDC, 0x20, 0x9F, 0xE5, 0xDC, 0x30, 0x9F, 0xE5, 0x1E, 0xFC, 0xF9, 0xEB
	.byte 0x00, 0x00, 0x57, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xD0, 0x00, 0x9F, 0xE5, 0xC0, 0x10, 0x9F, 0xE5
	.byte 0xC0, 0x20, 0x9F, 0xE5, 0xC8, 0x30, 0x9F, 0xE5, 0x17, 0xFC, 0xF9, 0xEB, 0xD0, 0x00, 0xD7, 0xE1
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0xB8, 0x00, 0x9F, 0xE5, 0xA0, 0x10, 0x9F, 0xE5
	.byte 0xA0, 0x20, 0x9F, 0xE5, 0xA8, 0x30, 0x9F, 0xE5, 0x0F, 0xFC, 0xF9, 0xEB, 0x00, 0x00, 0x56, 0xE3
	.byte 0x04, 0x00, 0x00, 0xAA, 0xA0, 0x00, 0x9F, 0xE5, 0x84, 0x10, 0x9F, 0xE5, 0x84, 0x20, 0x9F, 0xE5
	.byte 0x98, 0x30, 0x9F, 0xE5, 0x08, 0xFC, 0xF9, 0xEB, 0x2C, 0x00, 0x9D, 0xE5, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x88, 0x00, 0x9F, 0xE5, 0x64, 0x10, 0x9F, 0xE5, 0x64, 0x20, 0x9F, 0xE5
	.byte 0x80, 0x30, 0x9F, 0xE5, 0x00, 0xFC, 0xF9, 0xEB, 0x2C, 0x40, 0x9D, 0xE5, 0x00, 0x00, 0x94, 0xE5
	.byte 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x6C, 0x00, 0x9F, 0xE5, 0x40, 0x10, 0x9F, 0xE5
	.byte 0x40, 0x20, 0x9F, 0xE5, 0x64, 0x30, 0x9F, 0xE5, 0xF7, 0xFB, 0xF9, 0xEB, 0x28, 0x00, 0x9D, 0xE5
	.byte 0x00, 0x50, 0x8D, 0xE5, 0x04, 0x00, 0x8D, 0xE5, 0x04, 0x10, 0x94, 0xE5, 0x08, 0x00, 0xA0, 0xE1
	.byte 0x08, 0x10, 0x8D, 0xE5, 0x00, 0x40, 0x94, 0xE5, 0x07, 0x20, 0xA0, 0xE1, 0x06, 0x30, 0xA0, 0xE1
	.byte 0x01, 0x10, 0xA0, 0xE3, 0x34, 0xFF, 0x2F, 0xE1, 0x0C, 0xD0, 0x8D, 0xE2, 0xF8, 0x81, 0xBD, 0xE8
	.byte 0xA0, 0xAA, 0x25, 0x02, 0x94, 0xAA, 0x25, 0x02, 0x90, 0xA9, 0x25, 0x02, 0x12, 0x08, 0x00, 0x00
	.byte 0x9C, 0xAD, 0x25, 0x02, 0x16, 0x08, 0x00, 0x00, 0xAC, 0xAD, 0x25, 0x02, 0x18, 0xAF, 0x25, 0x02
	.byte 0x17, 0x08, 0x00, 0x00, 0x28, 0xAF, 0x25, 0x02, 0x27, 0x08, 0x00, 0x00, 0x38, 0xAF, 0x25, 0x02
	.byte 0x2C, 0x08, 0x00, 0x00

	arm_func_start FUN_02255104
FUN_02255104: // 0x02255104
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	movs r4, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bne _02255134
	ldr r0, _02255228 // =0x0225AAA0
	ldr r1, _0225522C // =0x0225AA94
	ldr r2, _02255230 // =0x0225A8F8
	ldr r3, _02255234 // =0x00000847
	bl __msl_assertion_failed
_02255134:
	cmp r4, #0
	ldrne r0, [r4]
	cmpne r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	bne _02255164
	ldr r0, _02255238 // =0x0225AEA0
	ldr r1, _0225522C // =0x0225AA94
	ldr r2, _02255230 // =0x0225A8F8
	ldr r3, _0225523C // =0x0000084B
	bl __msl_assertion_failed
_02255164:
	cmp r7, #0
	ldreq r7, _02255240 // =0x0225AB2C
	ldrsb r0, [r7]
	cmp r0, #0
	beq _022551B0
	mov r0, r4
	mov r1, r7
	bl FUN_0224C134
	cmp r0, #0
	beq _022551B0
	ldr r2, _02255244 // =0x02254FD0
	add r3, sp, #0
	mov r0, r4
	mov r1, r7
	str r6, [sp]
	str r5, [sp, #4]
	bl FUN_0224BFE8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_022551B0:
	ldr r1, _02255248 // =0x0225AF50
	mov r2, r7
	add r0, r4, #0x1c
	bl FUN_02256E70
	ldrsb r0, [r7]
	mov r2, r6
	mov r3, r5
	cmp r0, #0
	moveq r7, #0
	mov r0, r4
	mov r1, r7
	bl FUN_0224E874
	ldr r1, [sp, #0x20]
	mov r6, r0
	cmp r1, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r5, #0xa
_022551F8:
	mov r0, r4
	mov r1, r6
	bl FUN_0225366C
	mov r0, r5
	bl msleep
	mov r0, r4
	mov r1, r6
	bl FUN_0225355C
	cmp r0, #0
	bne _022551F8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02255228: .word 0x0225AAA0
_0225522C: .word 0x0225AA94
_02255230: .word 0x0225A8F8
_02255234: .word 0x00000847
_02255238: .word 0x0225AEA0
_0225523C: .word 0x0000084B
_02255240: .word 0x0225AB2C
_02255244: .word 0x02254FD0
_02255248: .word 0x0225AF50
	arm_func_end FUN_02255104

	arm_func_start FUN_0225524C
FUN_0225524C: // 0x0225524C
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bne _02255278
	ldr r0, _022553B4 // =0x0225AAA0
	ldr r1, _022553B8 // =0x0225AA94
	ldr r2, _022553BC // =0x0225AA0C
	ldr r3, _022553C0 // =0x00000889
	bl __msl_assertion_failed
_02255278:
	cmp r7, #0
	ldrne r0, [r7]
	cmpne r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	bne _022552A4
	ldr r0, _022553C4 // =0x0225AF5C
	ldr r1, _022553B8 // =0x0225AA94
	ldr r2, _022553BC // =0x0225AA0C
	ldr r3, _022553C8 // =0x0000088C
	bl __msl_assertion_failed
_022552A4:
	ldrsb r0, [r6]
	cmp r0, #0
	bne _022552C4
	ldr r0, _022553CC // =0x0225AF6C
	ldr r1, _022553B8 // =0x0225AA94
	ldr r2, _022553BC // =0x0225AA0C
	ldr r3, _022553C8 // =0x0000088C
	bl __msl_assertion_failed
_022552C4:
	mov r0, r6
	bl strlen
	cmp r0, #0x80
	blo _022552E8
	ldr r0, _022553D0 // =0x0225AF7C
	ldr r1, _022553B8 // =0x0225AA94
	ldr r2, _022553BC // =0x0225AA0C
	ldr r3, _022553C8 // =0x0000088C
	bl __msl_assertion_failed
_022552E8:
	cmp r4, #4
	bls _02255304
	ldr r0, _022553D4 // =0x0225ADD8
	ldr r1, _022553B8 // =0x0225AA94
	ldr r2, _022553BC // =0x0225AA0C
	ldr r3, _022553D8 // =0x0000088D
	bl __msl_assertion_failed
_02255304:
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r4, #0
	bne _02255334
	ldr r1, _022553DC // =0x0225AE50
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02255334:
	cmp r4, #1
	bne _02255354
	ldr r1, _022553E0 // =0x0225AE60
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02255354:
	cmp r4, #2
	bne _02255374
	ldr r1, _022553E4 // =0x0225AE78
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02255374:
	cmp r4, #3
	bne _02255394
	ldr r1, _022553E8 // =0x0225AE88
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02255394:
	cmp r4, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _022553EC // =0x0225AE94
	mov r2, r6
	mov r3, r5
	add r0, r7, #0x1c
	bl FUN_02256E70
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022553B4: .word 0x0225AAA0
_022553B8: .word 0x0225AA94
_022553BC: .word 0x0225AA0C
_022553C0: .word 0x00000889
_022553C4: .word 0x0225AF5C
_022553C8: .word 0x0000088C
_022553CC: .word 0x0225AF6C
_022553D0: .word 0x0225AF7C
_022553D4: .word 0x0225ADD8
_022553D8: .word 0x0000088D
_022553DC: .word 0x0225AE50
_022553E0: .word 0x0225AE60
_022553E4: .word 0x0225AE78
_022553E8: .word 0x0225AE88
_022553EC: .word 0x0225AE94
	arm_func_end FUN_0225524C

	arm_func_start FUN_022553F0
FUN_022553F0: // 0x022553F0
	stmfd sp!, {r4, lr}
	ldr r3, _02255440 // =0x0225B6F4
	ldr r0, _02255444 // =0x0225B6F0
	ldr r2, [r3]
	ldr r1, _02255448 // =0x0225AF9C
	add r4, r2, #1
	str r4, [r3]
	bl sprintf
	ldr ip, _02255440 // =0x0225B6F4
	ldr r3, _0225544C // =0x10624DD3
	ldr lr, [ip]
	mov r2, #0x3e8
	mov r0, lr, lsr #0x1f
	smull r1, r4, r3, lr
	add r4, r0, r4, asr #6
	smull r0, r1, r2, r4
	sub r4, lr, r0
	ldr r0, _02255444 // =0x0225B6F0
	str r4, [ip]
	ldmia sp!, {r4, pc}
	.align 2, 0
_02255440: .word 0x0225B6F4
_02255444: .word 0x0225B6F0
_02255448: .word 0x0225AF9C
_0225544C: .word 0x10624DD3
	arm_func_end FUN_022553F0

	arm_func_start FUN_02255450
FUN_02255450: // 0x02255450
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x200
	movs sl, r0
	mov r5, r1
	mov r4, r2
	mov sb, r3
	ldr r8, [sp, #0x228]
	ldr r7, [sp, #0x22c]
	bne _02255488
	ldr r0, _0225552C // =0x0225AAA0
	ldr r1, _02255530 // =0x0225AA94
	ldr r2, _02255534 // =0x0225A9B8
	ldr r3, _02255538 // =0x00000AD7
	bl __msl_assertion_failed
_02255488:
	cmp sl, #0
	ldrne r0, [sl]
	cmpne r0, #0
	addeq sp, sp, #0x200
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r4, #0
	ldrnesb r0, [r4]
	cmpne r0, #0
	add r0, sp, #0
	bne _022554C0
	ldr r1, _0225553C // =0x0225AFC4
	mov r2, r5
	bl sprintf
	b _022554D0
_022554C0:
	ldr r1, _02255540 // =0x0225AFD4
	mov r2, r5
	mov r3, r4
	bl sprintf
_022554D0:
	cmp sb, #0
	mov r6, #0
	ble _02255518
	ldr fp, _02255544 // =0x0225AB2C
	add r4, sp, #0
_022554E4:
	ldr r5, [r7, r6, lsl #2]
	mov r0, r4
	cmp r5, #0
	moveq r5, fp
	bl strlen
	ldr r2, [r8, r6, lsl #2]
	ldr r1, _02255548 // =0x0225AF94
	mov r3, r5
	add r0, r4, r0
	bl sprintf
	add r6, r6, #1
	cmp r6, sb
	blt _022554E4
_02255518:
	add r1, sp, #0
	add r0, sl, #0x1c
	bl FUN_02256BB0
	add sp, sp, #0x200
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0225552C: .word 0x0225AAA0
_02255530: .word 0x0225AA94
_02255534: .word 0x0225A9B8
_02255538: .word 0x00000AD7
_0225553C: .word 0x0225AFC4
_02255540: .word 0x0225AFD4
_02255544: .word 0x0225AB2C
_02255548: .word 0x0225AF94
	arm_func_end FUN_02255450

	arm_func_start FUN_0225554C
FUN_0225554C: // 0x0225554C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x210
	str r0, [sp, #4]
	cmp r0, #0
	mov r0, #0
	mov r5, r1
	str r2, [sp, #8]
	mov r4, r3
	ldr sl, [sp, #0x238]
	ldr sb, [sp, #0x23c]
	str r0, [sp, #0xc]
	bne _02255590
	ldr r0, _022557B0 // =0x0225AAA0
	ldr r1, _022557B4 // =0x0225AA94
	ldr r2, _022557B8 // =0x0225A97C
	ldr r3, _022557BC // =0x00000B19
	bl __msl_assertion_failed
_02255590:
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	bne _022555B4
	ldr r0, _022557C0 // =0x0225AFE4
	ldr r1, _022557B4 // =0x0225AA94
	ldr r2, _022557B8 // =0x0225A97C
	ldr r3, _022557C4 // =0x00000B1B
	bl __msl_assertion_failed
_022555B4:
	cmp r4, #0
	ldrnesb r0, [r4]
	cmpne r0, #0
	bne _022555D8
	ldr r0, _022557C8 // =0x0225AFA4
	ldr r1, _022557B4 // =0x0225AA94
	ldr r2, _022557B8 // =0x0225A97C
	ldr r3, _022557CC // =0x00000B1C
	bl __msl_assertion_failed
_022555D8:
	cmp sl, #0
	beq _022555FC
	cmp sb, #0
	bne _022555FC
	ldr r0, _022557D0 // =0x0225AFFC
	ldr r1, _022557B4 // =0x0225AA94
	ldr r2, _022557B8 // =0x0225A97C
	ldr r3, _022557D4 // =0x00000B1D
	bl __msl_assertion_failed
_022555FC:
	ldr r0, [sp, #8]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	add r0, sp, #0x10
	bne _02255628
	ldr r1, _022557D8 // =0x0225B00C
	mov r2, r5
	mov r3, r4
	bl sprintf
	b _0225563C
_02255628:
	ldr r1, _022557DC // =0x0225B024
	ldr r3, [sp, #8]
	mov r2, r5
	str r4, [sp]
	bl sprintf
_0225563C:
	add r0, sp, #0x10
	bl strlen
	mov r7, r0
	cmp sl, #0
	mov r5, #0
	ble _0225570C
	mov r4, #0x2f
	add fp, sp, #0x10
_0225565C:
	ldr r6, [sb, r5, lsl #2]
	cmp r6, #0
	ldrnesb r0, [r6]
	cmpne r0, #0
	beq _02255700
	ldr r1, _022557E0 // =0x0225B03C
	mov r0, r6
	bl strcmp
	cmp r0, #0
	moveq r0, #1
	streq r0, [sp, #0xc]
	beq _02255700
	mov r0, r6
	bl strlen
	mov r6, r0
	add r0, r7, r6
	add r0, r0, #1
	cmp r0, #0x200
	bge _02255700
	mov r0, #0x5c
	strb r0, [fp, r7]
	add r7, r7, #1
	add r8, fp, r7
	ldr r1, [sb, r5, lsl #2]
	mov r0, r8
	mov r2, r6
	bl memcpy
	add r2, r7, r6
	mov r1, r7
	cmp r7, r2
	bge _022556F4
_022556D8:
	ldrsb r0, [r8]
	add r1, r1, #1
	cmp r0, #0x5c
	streqb r4, [r8]
	add r8, r8, #1
	cmp r1, r2
	blt _022556D8
_022556F4:
	add r7, r7, r6
	mov r0, #0
	strb r0, [fp, r7]
_02255700:
	add r5, r5, #1
	cmp r5, sl
	blt _0225565C
_0225570C:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0225575C
	add r5, r7, #4
	cmp r5, #0x200
	bge _0225575C
	ldr r3, _022557E4 // =0x0225B040
	add r4, sp, #0x10
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	ldrb r0, [r3, #2]
	strb r2, [r4, r7]
	add r2, r4, r7
	strb r1, [r2, #1]
	strb r0, [r2, #2]
	ldrb r1, [r3, #3]
	ldrb r0, [r3, #4]
	mov r7, r5
	strb r1, [r2, #3]
	strb r0, [r2, #4]
_0225575C:
	cmp sl, #0
	bne _02255794
	ldr r0, [sp, #8]
	cmp r0, #0
	ldrnesb r0, [r0]
	cmpne r0, #0
	bne _02255794
	ldr r0, _022557E8 // =0x0225AAC0
	add r2, sp, #0x10
	ldrb r1, [r0]
	ldrb r0, [r0, #1]
	add r3, r2, r7
	strb r1, [r2, r7]
	strb r0, [r3, #1]
_02255794:
	ldr r0, [sp, #4]
	add r1, sp, #0x10
	add r0, r0, #0x1c
	bl FUN_02256BB0
	ldr r0, [sp, #0xc]
	add sp, sp, #0x210
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_022557B0: .word 0x0225AAA0
_022557B4: .word 0x0225AA94
_022557B8: .word 0x0225A97C
_022557BC: .word 0x00000B19
_022557C0: .word 0x0225AFE4
_022557C4: .word 0x00000B1B
_022557C8: .word 0x0225AFA4
_022557CC: .word 0x00000B1C
_022557D0: .word 0x0225AFFC
_022557D4: .word 0x00000B1D
_022557D8: .word 0x0225B00C
_022557DC: .word 0x0225B024
_022557E0: .word 0x0225B03C
_022557E4: .word 0x0225B040
_022557E8: .word 0x0225AAC0
	arm_func_end FUN_0225554C

	arm_func_start FUN_022557EC
FUN_022557EC: // 0x022557EC
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x10
	movs sb, r0
	mov r5, r1
	mov r8, r2
	mov r7, r3
	ldr r6, [sp, #0x30]
	bne _02255820
	ldr r0, _02255964 // =0x0225AAA0
	ldr r1, _02255968 // =0x0225AA94
	ldr r2, _0225596C // =0x0225A9A4
	ldr r3, _02255970 // =0x00000B6F
	bl __msl_assertion_failed
_02255820:
	cmp sb, #0
	ldrne r0, [sb]
	cmpne r0, #0
	addeq sp, sp, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r7, #0
	bge _02255850
	ldr r0, _02255974 // =0x0225AFB8
	ldr r1, _02255968 // =0x0225AA94
	ldr r2, _0225596C // =0x0225A9A4
	ldr r3, _02255978 // =0x00000B72
	bl __msl_assertion_failed
_02255850:
	cmp r7, #0
	beq _02255874
	cmp r6, #0
	bne _02255874
	ldr r0, _0225597C // =0x0225AFFC
	ldr r1, _02255968 // =0x0225AA94
	ldr r2, _0225596C // =0x0225A9A4
	ldr r3, _02255980 // =0x00000B73
	bl __msl_assertion_failed
_02255874:
	bl FUN_022553F0
	mov r4, r0
	str r7, [sp]
	mov r0, sb
	mov r1, r5
	mov r2, r8
	mov r3, r4
	str r6, [sp, #4]
	bl FUN_0225554C
	mov r5, r0
	cmp r8, #0
	ldrnesb r0, [r8]
	cmpne r0, #0
	bne _022558D8
	ldr r0, [sp, #0x34]
	str r5, [sp]
	str r0, [sp, #4]
	ldr r5, [sp, #0x38]
	mov r0, sb
	mov r1, r4
	mov r2, r7
	mov r3, r6
	str r5, [sp, #8]
	bl FUN_0224EA9C
	b _0225591C
_022558D8:
	ldr r1, _02255984 // =0x0225AAC0
	mov r0, r8
	bl strcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	str r0, [sp]
	str r5, [sp, #4]
	ldr r0, [sp, #0x34]
	ldr r5, [sp, #0x38]
	str r0, [sp, #8]
	mov r0, sb
	mov r1, r4
	mov r2, r7
	mov r3, r6
	str r5, [sp, #0xc]
	bl FUN_0224E928
_0225591C:
	mov r5, r0
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r4, #0xa
_02255934:
	mov r0, sb
	mov r1, r5
	bl FUN_0225366C
	mov r0, r4
	bl msleep
	mov r0, sb
	mov r1, r5
	bl FUN_0225355C
	cmp r0, #0
	bne _02255934
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_02255964: .word 0x0225AAA0
_02255968: .word 0x0225AA94
_0225596C: .word 0x0225A9A4
_02255970: .word 0x00000B6F
_02255974: .word 0x0225AFB8
_02255978: .word 0x00000B72
_0225597C: .word 0x0225AFFC
_02255980: .word 0x00000B73
_02255984: .word 0x0225AAC0
	arm_func_end FUN_022557EC

	arm_func_start FUN_02255988
FUN_02255988: // 0x02255988
	stmfd sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl strlen
	cmp r0, #0x15
	movhs r0, #6
	ldmhsia sp!, {r3, r4, r5, pc}
	cmp r5, #0
	ldrnesb r0, [r5]
	cmpne r0, #0
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, pc}
	cmp r0, #0x40
	cmpne r0, #0x23
	cmpne r0, #0x2b
	cmpne r0, #0x3a
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, pc}
	cmp r0, #0
	beq _022559FC
	ldr r4, _02255A04 // =0x0225AD04
_022559D8:
	ldrsb r1, [r5], #1
	mov r0, r4
	bl strchr
	cmp r0, #0
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrsb r0, [r5]
	cmp r0, #0
	bne _022559D8
_022559FC:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02255A04: .word 0x0225AD04
	arm_func_end FUN_02255988

	arm_func_start FUN_02255A08
FUN_02255A08: // 0x02255A08
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x20
	movs r4, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bne _02255A38
	ldr r0, _02255ACC // =0x0225AAA0
	ldr r1, _02255AD0 // =0x0225AA94
	ldr r2, _02255AD4 // =0x0225A8C0
	ldr r3, _02255AD8 // =0x00000BC9
	bl __msl_assertion_failed
_02255A38:
	ldr r0, [r4, #0xc]
	cmp r0, #0
	beq _02255A9C
	add r3, sp, #0x10
	mov r2, #0
	str r2, [r3]
	str r2, [r3, #4]
	str r2, [r3, #8]
	ldr r0, [sp, #0x38]
	str r2, [r3, #0xc]
	str r0, [sp, #0x1c]
	str r7, [sp, #0x10]
	str r6, [sp, #0x14]
	str r5, [sp, #0x18]
	ldr r1, [r4, #0x18]
	mov r0, #0x10
	stmia sp, {r1, r2}
	str r2, [sp, #8]
	str r0, [sp, #0xc]
	ldr r2, [r4, #0xc]
	mov r0, r4
	mov r1, #0x19
	bl FUN_02248C2C
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02255A9C:
	mov r1, #0
	str r1, [r4, #4]
	ldr ip, [r4, #0x14]
	cmp ip, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r3, [r4, #0x18]
	mov r0, r4
	mov r2, #1
	blx ip
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_02255ACC: .word 0x0225AAA0
_02255AD0: .word 0x0225AA94
_02255AD4: .word 0x0225A8C0
_02255AD8: .word 0x00000BC9
	arm_func_end FUN_02255A08

	arm_func_start FUN_02255ADC
FUN_02255ADC: // 0x02255ADC
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _02255AFC
	ldr r0, _02255B30 // =0x0225B148
	ldr r1, _02255B34 // =0x0225B158
	ldr r2, _02255B38 // =0x0225B074
	mov r3, #0x5f
	bl __msl_assertion_failed
_02255AFC:
	mov r0, #0
	mov r1, #0x2000
	str r0, [r4, #4]
	add r0, r1, #1
	str r1, [r4, #8]
	bl DWCi_GsMalloc
	cmp r0, #0
	str r0, [r4]
	moveq r0, #0
	movne r1, #0
	strneb r1, [r0]
	movne r0, #1
	ldmia sp!, {r4, pc}
	.align 2, 0
_02255B30: .word 0x0225B148
_02255B34: .word 0x0225B158
_02255B38: .word 0x0225B074
	arm_func_end FUN_02255ADC

	arm_func_start FUN_02255B3C
FUN_02255B3C: // 0x02255B3C
	ldr ip, _02255B48 // =DWCi_GsFree
	ldr r0, [r0]
	bx ip
	.align 2, 0
_02255B48: .word DWCi_GsFree
	arm_func_end FUN_02255B3C

	arm_func_start FUN_02255B4C
FUN_02255B4C: // 0x02255B4C
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _02255B70
	ldr r0, _02255C88 // =0x0225B168
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x78
	bl __msl_assertion_failed
_02255B70:
	ldr r0, [r5, #8]
	cmp r0, #0
	bge _02255B90
	ldr r0, _02255C94 // =0x0225B17C
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x78
	bl __msl_assertion_failed
_02255B90:
	ldr r0, [r5, #8]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02255BB8
	ldr r0, _02255C98 // =0x0225B190
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x78
	bl __msl_assertion_failed
_02255BB8:
	ldr r0, [r5, #4]
	cmp r0, #0
	bge _02255BD8
	ldr r0, _02255C9C // =0x0225B1B4
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x78
	bl __msl_assertion_failed
_02255BD8:
	ldr r1, [r5, #4]
	ldr r0, [r5, #8]
	cmp r1, r0
	ble _02255BFC
	ldr r0, _02255CA0 // =0x0225B1CC
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x78
	bl __msl_assertion_failed
_02255BFC:
	cmp r4, #0
	bge _02255C18
	ldr r0, _02255CA4 // =0x0225B1F0
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x79
	bl __msl_assertion_failed
_02255C18:
	ldr r0, _02255CA8 // =0x00007FFF
	cmp r4, r0
	ble _02255C38
	ldr r0, _02255CAC // =0x0225B1FC
	ldr r1, _02255C8C // =0x0225B158
	ldr r2, _02255C90 // =0x0225B0E4
	mov r3, #0x7a
	bl __msl_assertion_failed
_02255C38:
	ldr r1, [r5, #4]
	ldr r0, [r5, #8]
	add r1, r1, r4
	cmp r1, r0
	movle r0, #1
	ldmleia sp!, {r3, r4, r5, pc}
	add r2, r1, #0x2000
	mov r1, r2, lsr #0x1f
	rsb r0, r1, r2, lsl #19
	add r0, r1, r0, ror #19
	sub r4, r2, r0
	ldr r0, [r5]
	add r1, r4, #1
	bl DWCi_GsRealloc
	cmp r0, #0
	moveq r0, #0
	strne r0, [r5]
	strne r4, [r5, #8]
	movne r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02255C88: .word 0x0225B168
_02255C8C: .word 0x0225B158
_02255C90: .word 0x0225B0E4
_02255C94: .word 0x0225B17C
_02255C98: .word 0x0225B190
_02255C9C: .word 0x0225B1B4
_02255CA0: .word 0x0225B1CC
_02255CA4: .word 0x0225B1F0
_02255CA8: .word 0x00007FFF
_02255CAC: .word 0x0225B1FC
	arm_func_end FUN_02255B4C

	arm_func_start FUN_02255CB0
FUN_02255CB0: // 0x02255CB0
	stmfd sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r1
	bne _02255CD4
	ldr r0, _02255DC8 // =0x0225B168
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x97
	bl __msl_assertion_failed
_02255CD4:
	ldr r0, [r5, #8]
	cmp r0, #0
	bge _02255CF4
	ldr r0, _02255DD4 // =0x0225B17C
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x97
	bl __msl_assertion_failed
_02255CF4:
	ldr r0, [r5, #8]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02255D1C
	ldr r0, _02255DD8 // =0x0225B190
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x97
	bl __msl_assertion_failed
_02255D1C:
	ldr r0, [r5, #4]
	cmp r0, #0
	bge _02255D3C
	ldr r0, _02255DDC // =0x0225B1B4
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x97
	bl __msl_assertion_failed
_02255D3C:
	ldr r1, [r5, #4]
	ldr r0, [r5, #8]
	cmp r1, r0
	ble _02255D60
	ldr r0, _02255DE0 // =0x0225B1CC
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x97
	bl __msl_assertion_failed
_02255D60:
	cmp r4, #0
	bge _02255D7C
	ldr r0, _02255DE4 // =0x0225B1F0
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x98
	bl __msl_assertion_failed
_02255D7C:
	ldr r0, [r5, #4]
	cmp r4, r0
	ble _02255D9C
	ldr r0, _02255DE8 // =0x0225B20C
	ldr r1, _02255DCC // =0x0225B158
	ldr r2, _02255DD0 // =0x0225B0F8
	mov r3, #0x99
	bl __msl_assertion_failed
_02255D9C:
	ldr r0, [r5, #4]
	sub r2, r0, r4
	str r2, [r5, #4]
	ldr r0, [r5]
	add r1, r0, r4
	bl memmove
	ldr r1, [r5]
	ldr r0, [r5, #4]
	mov r2, #0
	strb r2, [r1, r0]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_02255DC8: .word 0x0225B168
_02255DCC: .word 0x0225B158
_02255DD0: .word 0x0225B0F8
_02255DD4: .word 0x0225B17C
_02255DD8: .word 0x0225B190
_02255DDC: .word 0x0225B1B4
_02255DE0: .word 0x0225B1CC
_02255DE4: .word 0x0225B1F0
_02255DE8: .word 0x0225B20C
	arm_func_end FUN_02255CB0

	arm_func_start FUN_02255DEC
FUN_02255DEC: // 0x02255DEC
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _02255E0C
	ldr r0, _02255E5C // =0x0225B224
	ldr r1, _02255E60 // =0x0225B158
	ldr r2, _02255E64 // =0x0225B094
	mov r3, #0xb1
	bl __msl_assertion_failed
_02255E0C:
	mov r0, r4
	mov r1, #0
	mov r2, #0x350
	bl memset
	mvn r1, #0
	add r0, r4, #0x108
	str r1, [r4]
	bl FUN_02255ADC
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	add r0, r4, #0x114
	bl FUN_02255ADC
	cmp r0, #0
	movne r0, #1
	ldmneia sp!, {r4, pc}
	add r0, r4, #0x108
	bl FUN_02255B3C
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_02255E5C: .word 0x0225B224
_02255E60: .word 0x0225B158
_02255E64: .word 0x0225B094
	arm_func_end FUN_02255DEC

	arm_func_start FUN_02255E68
FUN_02255E68: // 0x02255E68
	stmfd sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	movs r6, r0
	mov r5, r1
	mov r4, r2
	bne _02255E94
	ldr r0, _02256164 // =0x0225B234
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255E94:
	ldr r0, [r6, #4]
	cmp r0, #2
	bls _02255EB4
	ldr r0, _02256170 // =0x0225B244
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255EB4:
	adds r0, r6, #0x108
	bne _02255ED0
	ldr r0, _02256174 // =0x0225B2C4
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255ED0:
	ldr r0, [r6, #0x110]
	cmp r0, #0
	bge _02255EF0
	ldr r0, _02256178 // =0x0225B2E4
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255EF0:
	ldr r0, [r6, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02255F18
	ldr r0, _0225617C // =0x0225B308
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255F18:
	ldr r0, [r6, #0x10c]
	cmp r0, #0
	bge _02255F38
	ldr r0, _02256180 // =0x0225B338
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255F38:
	ldr r1, [r6, #0x10c]
	ldr r0, [r6, #0x110]
	cmp r1, r0
	ble _02255F5C
	ldr r0, _02256184 // =0x0225B35C
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255F5C:
	adds r0, r6, #0x114
	bne _02255F78
	ldr r0, _02256188 // =0x0225B39C
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255F78:
	ldr r0, [r6, #0x11c]
	cmp r0, #0
	bge _02255F98
	ldr r0, _0225618C // =0x0225B3BC
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255F98:
	ldr r0, [r6, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02255FC0
	ldr r0, _02256190 // =0x0225B3E0
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255FC0:
	ldr r0, [r6, #0x118]
	cmp r0, #0
	bge _02255FE0
	ldr r0, _02256194 // =0x0225B414
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02255FE0:
	ldr r1, [r6, #0x118]
	ldr r0, [r6, #0x11c]
	cmp r1, r0
	ble _02256004
	ldr r0, _02256198 // =0x0225B438
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd8
	bl __msl_assertion_failed
_02256004:
	cmp r5, #0
	bne _02256020
	ldr r0, _0225619C // =0x0225B478
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xd9
	bl __msl_assertion_failed
_02256020:
	cmp r4, #0
	bge _0225603C
	ldr r0, _022561A0 // =0x0225B490
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xda
	bl __msl_assertion_failed
_0225603C:
	ldr r0, _022561A4 // =0x0000FFFF
	cmp r4, r0
	ble _0225605C
	ldr r0, _022561A8 // =0x0225B49C
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xdb
	bl __msl_assertion_failed
_0225605C:
	ldr r0, [r6, #4]
	cmp r0, #0
	beq _0225607C
	ldr r0, _022561AC // =0x0225B4B0
	ldr r1, _02256168 // =0x0225B158
	ldr r2, _0225616C // =0x0225B0D4
	mov r3, #0xdc
	bl __msl_assertion_failed
_0225607C:
	mov r1, r5
	add r0, r6, #8
	mov r2, #0xff
	bl strncpy
	mov r1, #0
	mov r0, r5
	strb r1, [r6, #0x106]
	bl inet_addr
	mvn r1, #0
	cmp r0, r1
	bne _022560CC
	mov r0, r5
	bl SOC_GetHostByName
	cmp r0, #0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, [r0, #0xc]
	ldr r0, [r0]
	ldr r0, [r0]
_022560CC:
	mov r1, r4, lsl #0x10
	mov r1, r1, lsr #0x10
	mov r3, r1, asr #8
	mov r1, r1, lsl #8
	add r4, sp, #0
	mov r2, #0
	str r2, [r4, #4]
	and r3, r3, #0xff
	and r1, r1, #0xff00
	orr r1, r3, r1
	str r2, [r4]
	mov r3, #2
	str r0, [sp, #4]
	strh r1, [sp, #2]
	mov r0, r3
	mov r1, #1
	strb r3, [sp, #1]
	bl socket
	mvn r1, #0
	cmp r0, r1
	str r0, [r6]
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r1, r4
	mov r2, #8
	bl connect
	mvn r1, #0
	cmp r0, r1
	movne r0, #1
	addne sp, sp, #8
	strne r0, [r6, #4]
	ldmneia sp!, {r4, r5, r6, pc}
	ldr r0, [r6]
	bl closesocket
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_02256164: .word 0x0225B234
_02256168: .word 0x0225B158
_0225616C: .word 0x0225B0D4
_02256170: .word 0x0225B244
_02256174: .word 0x0225B2C4
_02256178: .word 0x0225B2E4
_0225617C: .word 0x0225B308
_02256180: .word 0x0225B338
_02256184: .word 0x0225B35C
_02256188: .word 0x0225B39C
_0225618C: .word 0x0225B3BC
_02256190: .word 0x0225B3E0
_02256194: .word 0x0225B414
_02256198: .word 0x0225B438
_0225619C: .word 0x0225B478
_022561A0: .word 0x0225B490
_022561A4: .word 0x0000FFFF
_022561A8: .word 0x0225B49C
_022561AC: .word 0x0225B4B0
	arm_func_end FUN_02255E68

	arm_func_start FUN_022561B0
FUN_022561B0: // 0x022561B0
	stmfd sp!, {r3, r4, r5, lr}
	movs r4, r0
	bne _022561D0
	ldr r0, _022563F0 // =0x0225B234
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_022561D0:
	ldr r0, [r4, #4]
	cmp r0, #2
	bls _022561F0
	ldr r0, _02256400 // =0x0225B244
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_022561F0:
	adds r0, r4, #0x108
	bne _0225620C
	ldr r0, _02256404 // =0x0225B2C4
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_0225620C:
	ldr r0, [r4, #0x110]
	cmp r0, #0
	bge _0225622C
	ldr r0, _02256408 // =0x0225B2E4
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_0225622C:
	ldr r0, [r4, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256254
	ldr r0, _0225640C // =0x0225B308
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_02256254:
	ldr r0, [r4, #0x10c]
	cmp r0, #0
	bge _02256274
	ldr r0, _02256410 // =0x0225B338
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_02256274:
	ldr r1, [r4, #0x10c]
	ldr r0, [r4, #0x110]
	cmp r1, r0
	ble _02256298
	ldr r0, _02256414 // =0x0225B35C
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_02256298:
	adds r0, r4, #0x114
	bne _022562B4
	ldr r0, _02256418 // =0x0225B39C
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_022562B4:
	ldr r0, [r4, #0x11c]
	cmp r0, #0
	bge _022562D4
	ldr r0, _0225641C // =0x0225B3BC
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_022562D4:
	ldr r0, [r4, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _022562FC
	ldr r0, _02256420 // =0x0225B3E0
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_022562FC:
	ldr r0, [r4, #0x118]
	cmp r0, #0
	bge _0225631C
	ldr r0, _02256424 // =0x0225B414
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_0225631C:
	ldr r1, [r4, #0x118]
	ldr r0, [r4, #0x11c]
	cmp r1, r0
	ble _02256340
	ldr r0, _02256428 // =0x0225B438
	ldr r1, _022563F4 // =0x0225B158
	ldr r2, _022563F8 // =0x0225B134
	ldr r3, _022563FC // =0x0000011B
	bl __msl_assertion_failed
_02256340:
	ldr r0, [r4]
	mvn r1, #0
	cmp r0, r1
	beq _02256360
	mov r1, #2
	bl shutdown
	ldr r0, [r4]
	bl closesocket
_02256360:
	mov r1, #2
	add r0, r4, #0x108
	str r1, [r4, #4]
	bl FUN_02255B3C
	add r0, r4, #0x114
	bl FUN_02255B3C
	ldr r0, [r4, #0x328]
	bl DWCi_GsFree
	ldr r0, [r4, #0x32c]
	bl DWCi_GsFree
	ldr r0, [r4, #0x330]
	bl DWCi_GsFree
	ldr r0, [r4, #0x334]
	bl DWCi_GsFree
	ldr r0, [r4, #0x338]
	bl DWCi_GsFree
	ldr r0, [r4, #0x33c]
	bl DWCi_GsFree
	ldr r0, [r4, #0x340]
	bl DWCi_GsFree
	ldr r0, [r4, #0x344]
	bl DWCi_GsFree
	ldr r0, [r4, #0x34c]
	mov r5, #0
	cmp r0, #0
	ble _022563E4
_022563C8:
	ldr r0, [r4, #0x348]
	ldr r0, [r0, r5, lsl #2]
	bl DWCi_GsFree
	ldr r0, [r4, #0x34c]
	add r5, r5, #1
	cmp r5, r0
	blt _022563C8
_022563E4:
	ldr r0, [r4, #0x348]
	bl DWCi_GsFree
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_022563F0: .word 0x0225B234
_022563F4: .word 0x0225B158
_022563F8: .word 0x0225B134
_022563FC: .word 0x0000011B
_02256400: .word 0x0225B244
_02256404: .word 0x0225B2C4
_02256408: .word 0x0225B2E4
_0225640C: .word 0x0225B308
_02256410: .word 0x0225B338
_02256414: .word 0x0225B35C
_02256418: .word 0x0225B39C
_0225641C: .word 0x0225B3BC
_02256420: .word 0x0225B3E0
_02256424: .word 0x0225B414
_02256428: .word 0x0225B438
	arm_func_end FUN_022561B0

	arm_func_start FUN_0225642C
FUN_0225642C: // 0x0225642C
	stmfd sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov ip, #0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	add r1, sp, #8
	add r2, sp, #4
	add r3, sp, #0
	str ip, [sp, #8]
	str ip, [sp, #4]
	str ip, [sp]
	bl GSISocketSelect
	cmp r6, #0
	ldrne r0, [sp, #8]
	strne r0, [r6]
	cmp r5, #0
	ldrne r0, [sp, #4]
	strne r0, [r5]
	cmp r4, #0
	ldrne r0, [sp]
	strne r0, [r4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end FUN_0225642C

	arm_func_start FUN_0225648C
FUN_0225648C: // 0x0225648C
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	movs r6, r0
	bne _022564AC
	ldr r0, _022566BC // =0x0225B234
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022564AC:
	ldr r0, [r6, #4]
	cmp r0, #2
	bls _022564CC
	ldr r0, _022566C8 // =0x0225B244
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022564CC:
	adds r0, r6, #0x108
	bne _022564E8
	ldr r0, _022566CC // =0x0225B2C4
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022564E8:
	ldr r0, [r6, #0x110]
	cmp r0, #0
	bge _02256508
	ldr r0, _022566D0 // =0x0225B2E4
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_02256508:
	ldr r0, [r6, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256530
	ldr r0, _022566D4 // =0x0225B308
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_02256530:
	ldr r0, [r6, #0x10c]
	cmp r0, #0
	bge _02256550
	ldr r0, _022566D8 // =0x0225B338
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_02256550:
	ldr r1, [r6, #0x10c]
	ldr r0, [r6, #0x110]
	cmp r1, r0
	ble _02256574
	ldr r0, _022566DC // =0x0225B35C
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_02256574:
	adds r0, r6, #0x114
	bne _02256590
	ldr r0, _022566E0 // =0x0225B39C
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_02256590:
	ldr r0, [r6, #0x11c]
	cmp r0, #0
	bge _022565B0
	ldr r0, _022566E4 // =0x0225B3BC
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022565B0:
	ldr r0, [r6, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _022565D8
	ldr r0, _022566E8 // =0x0225B3E0
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022565D8:
	ldr r0, [r6, #0x118]
	cmp r0, #0
	bge _022565F8
	ldr r0, _022566EC // =0x0225B414
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_022565F8:
	ldr r1, [r6, #0x118]
	ldr r0, [r6, #0x11c]
	cmp r1, r0
	ble _0225661C
	ldr r0, _022566F0 // =0x0225B438
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	mov r3, #0x154
	bl __msl_assertion_failed
_0225661C:
	ldr r0, [r6, #4]
	cmp r0, #1
	beq _0225663C
	ldr r0, _022566F4 // =0x0225B4D8
	ldr r1, _022566C0 // =0x0225B158
	ldr r2, _022566C4 // =0x0225B10C
	ldr r3, _022566F8 // =0x00000155
	bl __msl_assertion_failed
_0225663C:
	ldr r0, [r6, #0x118]
	cmp r0, #0
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r8, #0
	mov sb, #0x400
	mov r5, r8
	add r4, sp, #0
	mvn r7, #0
_0225665C:
	ldr r0, [r6]
	mov r1, r5
	mov r2, r4
	mov r3, r5
	bl FUN_0225642C
	ldr r0, [sp]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r2, [r6, #0x118]
	ldr r0, [r6]
	cmp r2, #0x400
	ldr r1, [r6, #0x114]
	movge r2, sb
	mov r3, r8
	bl send
	movs r1, r0
	cmpne r1, r7
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r6, #0x114
	bl FUN_02255CB0
	ldr r0, [r6, #0x118]
	cmp r0, #0
	bgt _0225665C
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_022566BC: .word 0x0225B234
_022566C0: .word 0x0225B158
_022566C4: .word 0x0225B10C
_022566C8: .word 0x0225B244
_022566CC: .word 0x0225B2C4
_022566D0: .word 0x0225B2E4
_022566D4: .word 0x0225B308
_022566D8: .word 0x0225B338
_022566DC: .word 0x0225B35C
_022566E0: .word 0x0225B39C
_022566E4: .word 0x0225B3BC
_022566E8: .word 0x0225B3E0
_022566EC: .word 0x0225B414
_022566F0: .word 0x0225B438
_022566F4: .word 0x0225B4D8
_022566F8: .word 0x00000155
	arm_func_end FUN_0225648C

	arm_func_start FUN_022566FC
FUN_022566FC: // 0x022566FC
	stmfd sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	movs sl, r0
	bne _0225671C
	ldr r0, _02256958 // =0x0225B234
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_0225671C:
	ldr r0, [sl, #4]
	cmp r0, #2
	bls _0225673C
	ldr r0, _02256968 // =0x0225B244
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_0225673C:
	adds r0, sl, #0x108
	bne _02256758
	ldr r0, _0225696C // =0x0225B2C4
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256758:
	ldr r0, [sl, #0x110]
	cmp r0, #0
	bge _02256778
	ldr r0, _02256970 // =0x0225B2E4
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256778:
	ldr r0, [sl, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _022567A0
	ldr r0, _02256974 // =0x0225B308
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_022567A0:
	ldr r0, [sl, #0x10c]
	cmp r0, #0
	bge _022567C0
	ldr r0, _02256978 // =0x0225B338
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_022567C0:
	ldr r1, [sl, #0x10c]
	ldr r0, [sl, #0x110]
	cmp r1, r0
	ble _022567E4
	ldr r0, _0225697C // =0x0225B35C
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_022567E4:
	adds r0, sl, #0x114
	bne _02256800
	ldr r0, _02256980 // =0x0225B39C
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256800:
	ldr r0, [sl, #0x11c]
	cmp r0, #0
	bge _02256820
	ldr r0, _02256984 // =0x0225B3BC
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256820:
	ldr r0, [sl, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256848
	ldr r0, _02256988 // =0x0225B3E0
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256848:
	ldr r0, [sl, #0x118]
	cmp r0, #0
	bge _02256868
	ldr r0, _0225698C // =0x0225B414
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_02256868:
	ldr r1, [sl, #0x118]
	ldr r0, [sl, #0x11c]
	cmp r1, r0
	ble _0225688C
	ldr r0, _02256990 // =0x0225B438
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256964 // =0x0000017A
	bl __msl_assertion_failed
_0225688C:
	ldr r0, [sl, #4]
	cmp r0, #1
	beq _022568AC
	ldr r0, _02256994 // =0x0225B4D8
	ldr r1, _0225695C // =0x0225B158
	ldr r2, _02256960 // =0x0225B120
	ldr r3, _02256998 // =0x0000017B
	bl __msl_assertion_failed
_022568AC:
	mov r7, #0x1000
	mov fp, r7
	mov r4, #0
	add r8, sp, #0
_022568BC:
	mov r2, #0
	ldr r0, [sl]
	mov r1, r8
	mov r3, r2
	bl FUN_0225642C
	ldr r0, [sp]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, r7
	add r0, sl, #0x108
	bl FUN_02255B4C
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r6, [sl, #0x108]
	ldr r5, [sl, #0x10c]
	ldr r0, [sl]
	mov r2, fp
	mov r3, #0
	add r1, r6, r5
	bl recv
	mov sb, r0
	cmp sb, #0
	movle r0, #2
	strle r0, [sl, #4]
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sl, #0x120]
	cmp r0, #0
	beq _0225693C
	mov r1, sb
	add r0, r6, r5
	add r2, sl, #0x124
	bl FUN_0224D4D8
_0225693C:
	ldr r0, [sl, #0x10c]
	add r1, r0, sb
	str r1, [sl, #0x10c]
	ldr r0, [sl, #0x108]
	strb r4, [r0, r1]
	b _022568BC
_02256954:
	.byte 0xF8, 0x8F, 0xBD, 0xE8
_02256958: .word 0x0225B234
_0225695C: .word 0x0225B158
_02256960: .word 0x0225B120
_02256964: .word 0x0000017A
_02256968: .word 0x0225B244
_0225696C: .word 0x0225B2C4
_02256970: .word 0x0225B2E4
_02256974: .word 0x0225B308
_02256978: .word 0x0225B338
_0225697C: .word 0x0225B35C
_02256980: .word 0x0225B39C
_02256984: .word 0x0225B3BC
_02256988: .word 0x0225B3E0
_0225698C: .word 0x0225B414
_02256990: .word 0x0225B438
_02256994: .word 0x0225B4D8
_02256998: .word 0x0000017B
	arm_func_end FUN_022566FC

	arm_func_start FUN_0225699C
FUN_0225699C: // 0x0225699C
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _022569BC
	ldr r0, _02256B6C // =0x0225B234
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_022569BC:
	ldr r0, [r4, #4]
	cmp r0, #2
	bls _022569DC
	ldr r0, _02256B7C // =0x0225B244
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_022569DC:
	adds r0, r4, #0x108
	bne _022569F8
	ldr r0, _02256B80 // =0x0225B2C4
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_022569F8:
	ldr r0, [r4, #0x110]
	cmp r0, #0
	bge _02256A18
	ldr r0, _02256B84 // =0x0225B2E4
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256A18:
	ldr r0, [r4, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256A40
	ldr r0, _02256B88 // =0x0225B308
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256A40:
	ldr r0, [r4, #0x10c]
	cmp r0, #0
	bge _02256A60
	ldr r0, _02256B8C // =0x0225B338
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256A60:
	ldr r1, [r4, #0x10c]
	ldr r0, [r4, #0x110]
	cmp r1, r0
	ble _02256A84
	ldr r0, _02256B90 // =0x0225B35C
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256A84:
	adds r0, r4, #0x114
	bne _02256AA0
	ldr r0, _02256B94 // =0x0225B39C
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256AA0:
	ldr r0, [r4, #0x11c]
	cmp r0, #0
	bge _02256AC0
	ldr r0, _02256B98 // =0x0225B3BC
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256AC0:
	ldr r0, [r4, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256AE8
	ldr r0, _02256B9C // =0x0225B3E0
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256AE8:
	ldr r0, [r4, #0x118]
	cmp r0, #0
	bge _02256B08
	ldr r0, _02256BA0 // =0x0225B414
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256B08:
	ldr r1, [r4, #0x118]
	ldr r0, [r4, #0x11c]
	cmp r1, r0
	ble _02256B2C
	ldr r0, _02256BA4 // =0x0225B438
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256B78 // =0x000001BE
	bl __msl_assertion_failed
_02256B2C:
	ldr r0, [r4, #4]
	cmp r0, #1
	beq _02256B4C
	ldr r0, _02256BA8 // =0x0225B4D8
	ldr r1, _02256B70 // =0x0225B158
	ldr r2, _02256B74 // =0x0225B0B4
	ldr r3, _02256BAC // =0x000001BF
	bl __msl_assertion_failed
_02256B4C:
	ldr r0, [r4, #4]
	cmp r0, #2
	ldmeqia sp!, {r4, pc}
	mov r0, r4
	bl FUN_0225648C
	mov r0, r4
	bl FUN_022566FC
	ldmia sp!, {r4, pc}
	.align 2, 0
_02256B6C: .word 0x0225B234
_02256B70: .word 0x0225B158
_02256B74: .word 0x0225B0B4
_02256B78: .word 0x000001BE
_02256B7C: .word 0x0225B244
_02256B80: .word 0x0225B2C4
_02256B84: .word 0x0225B2E4
_02256B88: .word 0x0225B308
_02256B8C: .word 0x0225B338
_02256B90: .word 0x0225B35C
_02256B94: .word 0x0225B39C
_02256B98: .word 0x0225B3BC
_02256B9C: .word 0x0225B3E0
_02256BA0: .word 0x0225B414
_02256BA4: .word 0x0225B438
_02256BA8: .word 0x0225B4D8
_02256BAC: .word 0x000001BF
	arm_func_end FUN_0225699C

	arm_func_start FUN_02256BB0
FUN_02256BB0: // 0x02256BB0
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	movs r8, r0
	mov r7, r1
	bne _02256BD4
	ldr r0, _02256E28 // =0x0225B234
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256BD4:
	ldr r0, [r8, #4]
	cmp r0, #2
	bls _02256BF4
	ldr r0, _02256E34 // =0x0225B244
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256BF4:
	adds r0, r8, #0x108
	bne _02256C10
	ldr r0, _02256E38 // =0x0225B2C4
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256C10:
	ldr r0, [r8, #0x110]
	cmp r0, #0
	bge _02256C30
	ldr r0, _02256E3C // =0x0225B2E4
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256C30:
	ldr r0, [r8, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256C58
	ldr r0, _02256E40 // =0x0225B308
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256C58:
	ldr r0, [r8, #0x10c]
	cmp r0, #0
	bge _02256C78
	ldr r0, _02256E44 // =0x0225B338
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256C78:
	ldr r1, [r8, #0x10c]
	ldr r0, [r8, #0x110]
	cmp r1, r0
	ble _02256C9C
	ldr r0, _02256E48 // =0x0225B35C
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256C9C:
	adds r0, r8, #0x114
	bne _02256CB8
	ldr r0, _02256E4C // =0x0225B39C
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256CB8:
	ldr r0, [r8, #0x11c]
	cmp r0, #0
	bge _02256CD8
	ldr r0, _02256E50 // =0x0225B3BC
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256CD8:
	ldr r0, [r8, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256D00
	ldr r0, _02256E54 // =0x0225B3E0
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256D00:
	ldr r0, [r8, #0x118]
	cmp r0, #0
	bge _02256D20
	ldr r0, _02256E58 // =0x0225B414
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256D20:
	ldr r1, [r8, #0x118]
	ldr r0, [r8, #0x11c]
	cmp r1, r0
	ble _02256D44
	ldr r0, _02256E5C // =0x0225B438
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	mov r3, #0x1d8
	bl __msl_assertion_failed
_02256D44:
	ldr r0, [r8, #4]
	cmp r0, #1
	beq _02256D64
	ldr r0, _02256E60 // =0x0225B4D8
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	ldr r3, _02256E64 // =0x000001D9
	bl __msl_assertion_failed
_02256D64:
	cmp r7, #0
	bne _02256D80
	ldr r0, _02256E68 // =0x0225B148
	ldr r1, _02256E2C // =0x0225B158
	ldr r2, _02256E30 // =0x0225B054
	ldr r3, _02256E6C // =0x000001DA
	bl __msl_assertion_failed
_02256D80:
	ldr r0, [r8, #4]
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r7
	bl strlen
	mov r6, r0
	add r0, r8, #0x114
	add r1, r6, #2
	bl FUN_02255B4C
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r5, [r8, #0x114]
	ldr r4, [r8, #0x118]
	mov r1, r7
	mov r2, r6
	add r0, r5, r4
	bl memcpy
	ldr r0, [r8, #0x118]
	mov r2, #0xd
	add r3, r0, r6
	add r0, r3, #1
	str r0, [r8, #0x118]
	ldr r0, [r8, #0x114]
	mov r1, #0xa
	strb r2, [r0, r3]
	ldr r2, [r8, #0x118]
	add r0, r2, #1
	str r0, [r8, #0x118]
	ldr r0, [r8, #0x114]
	strb r1, [r0, r2]
	ldr r0, [r8, #0x120]
	cmp r0, #0
	beq _02256E20
	add r2, r8, #0x26
	add r0, r5, r4
	add r1, r6, #2
	add r2, r2, #0x200
	bl FUN_0224D4D8
_02256E20:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02256E28: .word 0x0225B234
_02256E2C: .word 0x0225B158
_02256E30: .word 0x0225B054
_02256E34: .word 0x0225B244
_02256E38: .word 0x0225B2C4
_02256E3C: .word 0x0225B2E4
_02256E40: .word 0x0225B308
_02256E44: .word 0x0225B338
_02256E48: .word 0x0225B35C
_02256E4C: .word 0x0225B39C
_02256E50: .word 0x0225B3BC
_02256E54: .word 0x0225B3E0
_02256E58: .word 0x0225B414
_02256E5C: .word 0x0225B438
_02256E60: .word 0x0225B4D8
_02256E64: .word 0x000001D9
_02256E68: .word 0x0225B148
_02256E6C: .word 0x000001DA
	arm_func_end FUN_02256BB0

	arm_func_start FUN_02256E70
FUN_02256E70: // 0x02256E70
	stmfd sp!, {r0, r1, r2, r3}
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _02256E94
	ldr r0, _02257090 // =0x0225B234
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256E94:
	ldr r0, [r4, #4]
	cmp r0, #2
	bls _02256EB4
	ldr r0, _022570A0 // =0x0225B244
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256EB4:
	adds r0, r4, #0x108
	bne _02256ED0
	ldr r0, _022570A4 // =0x0225B2C4
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256ED0:
	ldr r0, [r4, #0x110]
	cmp r0, #0
	bge _02256EF0
	ldr r0, _022570A8 // =0x0225B2E4
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256EF0:
	ldr r0, [r4, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256F18
	ldr r0, _022570AC // =0x0225B308
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256F18:
	ldr r0, [r4, #0x10c]
	cmp r0, #0
	bge _02256F38
	ldr r0, _022570B0 // =0x0225B338
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256F38:
	ldr r1, [r4, #0x10c]
	ldr r0, [r4, #0x110]
	cmp r1, r0
	ble _02256F5C
	ldr r0, _022570B4 // =0x0225B35C
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256F5C:
	adds r0, r4, #0x114
	bne _02256F78
	ldr r0, _022570B8 // =0x0225B39C
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256F78:
	ldr r0, [r4, #0x11c]
	cmp r0, #0
	bge _02256F98
	ldr r0, _022570BC // =0x0225B3BC
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256F98:
	ldr r0, [r4, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02256FC0
	ldr r0, _022570C0 // =0x0225B3E0
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256FC0:
	ldr r0, [r4, #0x118]
	cmp r0, #0
	bge _02256FE0
	ldr r0, _022570C4 // =0x0225B414
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02256FE0:
	ldr r1, [r4, #0x118]
	ldr r0, [r4, #0x11c]
	cmp r1, r0
	ble _02257004
	ldr r0, _022570C8 // =0x0225B438
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	ldr r3, _0225709C // =0x00000213
	bl __msl_assertion_failed
_02257004:
	ldr r0, [r4, #4]
	cmp r0, #1
	beq _02257024
	ldr r0, _022570CC // =0x0225B4D8
	ldr r1, _02257094 // =0x0225B158
	ldr r2, _02257098 // =0x0225B0A4
	mov r3, #0x214
	bl __msl_assertion_failed
_02257024:
	ldr r0, [r4, #4]
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r4, lr}
	addeq sp, sp, #0x10
	bxeq lr
	add r0, sp, #0xc
	bic r2, r0, #3
	ldr r1, [sp, #0xc]
	ldr r0, _022570D0 // =0x0225B6F8
	add r2, r2, #4
	bl vsprintf
	mvn r1, #0
	cmp r0, r1
	ldreq r0, _022570D0 // =0x0225B6F8
	moveq r1, #0
	streqb r1, [r0, #0xfff]
	beq _02257078
	ldr r1, _022570D0 // =0x0225B6F8
	mov r2, #0
	strb r2, [r1, r0]
_02257078:
	ldr r1, _022570D0 // =0x0225B6F8
	mov r0, r4
	bl FUN_02256BB0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_02257090: .word 0x0225B234
_02257094: .word 0x0225B158
_02257098: .word 0x0225B0A4
_0225709C: .word 0x00000213
_022570A0: .word 0x0225B244
_022570A4: .word 0x0225B2C4
_022570A8: .word 0x0225B2E4
_022570AC: .word 0x0225B308
_022570B0: .word 0x0225B338
_022570B4: .word 0x0225B35C
_022570B8: .word 0x0225B39C
_022570BC: .word 0x0225B3BC
_022570C0: .word 0x0225B3E0
_022570C4: .word 0x0225B414
_022570C8: .word 0x0225B438
_022570CC: .word 0x0225B4D8
_022570D0: .word 0x0225B6F8
	arm_func_end FUN_02256E70

	arm_func_start FUN_022570D4
FUN_022570D4: // 0x022570D4
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	movs sl, r0
	ldrnesb r0, [sl]
	mov r4, #0
	mov sb, r1
	mov r5, r4
	mov r6, r4
	mov r7, r4
	mov r8, r4
	cmpne r0, #0
	bne _0225711C
	ldr r0, _02257260 // =0x0225B4FC
	ldr r1, _02257264 // =0x0225B158
	ldr r2, _02257268 // =0x0225B048
	ldr r3, _0225726C // =0x00000232
	bl __msl_assertion_failed
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0225711C:
	ldrsb r0, [sl]
	mov r1, sl
	cmp r0, #0
	beq _02257198
_0225712C:
	ldrsb r0, [r1]
	cmp r0, #0x21
	addne r1, r1, #1
	addne r6, r6, #1
	bne _0225718C
	ldrsb r0, [r1, #1]!
	mov r4, r1
	cmp r0, #0
	beq _0225718C
_02257150:
	cmp r0, #0x40
	addne r1, r1, #1
	addne r7, r7, #1
	bne _02257180
	ldrsb r0, [r1, #1]!
	mov r5, r1
	cmp r0, #0
	beq _02257180
_02257170:
	ldrsb r0, [r1, #1]!
	add r8, r8, #1
	cmp r0, #0
	bne _02257170
_02257180:
	ldrsb r0, [r1]
	cmp r0, #0
	bne _02257150
_0225718C:
	ldrsb r0, [r1]
	cmp r0, #0
	bne _0225712C
_02257198:
	cmp r6, #0
	beq _022571D0
	add r0, r6, #1
	bl DWCi_GsMalloc
	str r0, [sb, #8]
	cmp r0, #0
	beq _022571D8
	mov r1, sl
	mov r2, r6
	bl memcpy
	ldr r0, [sb, #8]
	mov r1, #0
	strb r1, [r0, r6]
	b _022571D8
_022571D0:
	mov r0, #0
	str r0, [sb, #8]
_022571D8:
	cmp r7, #0
	beq _02257210
	add r0, r7, #1
	bl DWCi_GsMalloc
	str r0, [sb, #0xc]
	cmp r0, #0
	beq _02257218
	mov r1, r4
	mov r2, r7
	bl memcpy
	ldr r0, [sb, #0xc]
	mov r1, #0
	strb r1, [r0, r7]
	b _02257218
_02257210:
	mov r0, #0
	str r0, [sb, #0xc]
_02257218:
	cmp r8, #0
	beq _02257250
	add r0, r8, #1
	bl DWCi_GsMalloc
	str r0, [sb, #0x10]
	cmp r0, #0
	beq _02257258
	mov r1, r5
	mov r2, r8
	bl memcpy
	ldr r0, [sb, #0x10]
	mov r1, #0
	strb r1, [r0, r8]
	b _02257258
_02257250:
	mov r0, #0
	str r0, [sb, #0x10]
_02257258:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02257260: .word 0x0225B4FC
_02257264: .word 0x0225B158
_02257268: .word 0x0225B048
_0225726C: .word 0x00000232
	arm_func_end FUN_022570D4

	arm_func_start FUN_02257270
FUN_02257270: // 0x02257270
	stmfd sp!, {r4, r5, r6, lr}
	mov r5, r1
	ldr r1, [r5, #0x24]
	mov r6, r0
	add r1, r1, #1
	ldr r0, [r5, #0x20]
	mov r1, r1, lsl #2
	bl DWCi_GsRealloc
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	str r0, [r5, #0x20]
	mov r0, r6
	bl strlen
	add r0, r0, #1
	bl DWCi_GsMalloc
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r1, r6
	bl strcpy
	ldr r2, [r5, #0x24]
	mov r0, #1
	add r1, r2, #1
	str r1, [r5, #0x24]
	ldr r1, [r5, #0x20]
	str r4, [r1, r2, lsl #2]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end FUN_02257270

	arm_func_start FUN_022572E0
FUN_022572E0: // 0x022572E0
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	movs r6, r0
	mov r5, r1
	bne _02257304
	ldr r0, _02257404 // =0x0225B500
	ldr r1, _02257408 // =0x0225B158
	ldr r2, _0225740C // =0x0225B084
	ldr r3, _02257410 // =0x000002A1
	bl __msl_assertion_failed
_02257304:
	cmp r5, #0
	bne _02257320
	ldr r0, _02257414 // =0x0225B510
	ldr r1, _02257408 // =0x0225B158
	ldr r2, _0225740C // =0x0225B084
	ldr r3, _02257418 // =0x000002A2
	bl __msl_assertion_failed
_02257320:
	mov r0, r6
	bl strlen
	add r0, r0, #1
	bl DWCi_GsMalloc
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r1, r6
	bl strcpy
	ldrsb r0, [r4]
	cmp r0, #0x3a
	bne _02257360
	mov r0, #0
	strb r0, [r4]
	add r8, r4, #1
	b _02257378
_02257360:
	ldr r1, _0225741C // =0x0225B520
	mov r0, r4
	bl strstr
	movs r8, r0
	movne r0, #0
	strneb r0, [r8], #2
_02257378:
	ldr r1, _02257420 // =0x0225B524
	mov r0, r4
	bl strtok
	cmp r0, #0
	beq _022573C8
	ldr r6, _02257420 // =0x0225B524
	mov r7, #0
_02257394:
	mov r1, r5
	bl FUN_02257270
	cmp r0, #0
	bne _022573B4
	mov r0, r4
	bl DWCi_GsFree
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022573B4:
	mov r0, r7
	mov r1, r6
	bl strtok
	cmp r0, #0
	bne _02257394
_022573C8:
	cmp r8, #0
	beq _022573F4
	mov r0, r8
	mov r1, r5
	bl FUN_02257270
	cmp r0, #0
	bne _022573F4
	mov r0, r4
	bl DWCi_GsFree
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022573F4:
	mov r0, r4
	bl DWCi_GsFree
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02257404: .word 0x0225B500
_02257408: .word 0x0225B158
_0225740C: .word 0x0225B084
_02257410: .word 0x000002A1
_02257414: .word 0x0225B510
_02257418: .word 0x000002A2
_0225741C: .word 0x0225B520
_02257420: .word 0x0225B524
	arm_func_end FUN_022572E0

	arm_func_start FUN_02257424
FUN_02257424: // 0x02257424
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	movs sb, r1
	mov sl, r0
	ldrnesb r0, [sb]
	mov r5, #0
	mov r6, r5
	mov r7, r5
	mov r8, r5
	cmpne r0, #0
	bne _02257468
	ldr r0, _02257858 // =0x0225B4FC
	ldr r1, _0225785C // =0x0225B158
	ldr r2, _02257860 // =0x0225B0C4
	mov r3, #0x2e4
	bl __msl_assertion_failed
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02257468:
	mov r0, sb
	bl strlen
	mov r4, r0
	add r0, r4, #1
	bl DWCi_GsMalloc
	str r0, [sl, #0x328]
	cmp r0, #0
	moveq r0, r5
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r1, sb
	mov r2, r4
	bl memcpy
	ldr r0, [sl, #0x328]
	mov r1, r5
	strb r1, [r0, r4]
	b _022574AC
_022574A8:
	add sb, sb, #1
_022574AC:
	ldrsb r0, [sb]
	cmp r0, #0xa
	cmpne r0, #0xd
	beq _022574A8
	cmp r0, #0x3a
	bne _022574F4
	add sb, sb, #1
	str sb, [sl, #0x32c]
	ldrsb r0, [sb]
	cmp r0, #0
	beq _022574F4
	b _022574E4
_022574DC:
	add r5, r5, #1
	add sb, sb, #1
_022574E4:
	ldrsb r0, [sb]
	cmp r0, #0x20
	cmpne r0, #0
	bne _022574DC
_022574F4:
	cmp r0, #0x20
	bne _02257508
_022574FC:
	ldrsb r0, [sb, #1]!
	cmp r0, #0x20
	beq _022574FC
_02257508:
	cmp r0, #0
	beq _02257530
	str sb, [sl, #0x33c]
	b _02257520
_02257518:
	add r6, r6, #1
	add sb, sb, #1
_02257520:
	ldrsb r0, [sb]
	cmp r0, #0x20
	cmpne r0, #0
	bne _02257518
_02257530:
	cmp r0, #0x20
	bne _02257544
_02257538:
	ldrsb r0, [sb, #1]!
	cmp r0, #0x20
	beq _02257538
_02257544:
	cmp r0, #0x3a
	cmpne r0, #0
	beq _02257570
	str sb, [sl, #0x340]
	b _02257560
_02257558:
	add r7, r7, #1
	add sb, sb, #1
_02257560:
	ldrsb r0, [sb]
	cmp r0, #0x20
	cmpne r0, #0
	bne _02257558
_02257570:
	cmp r0, #0x20
	bne _02257584
_02257578:
	ldrsb r0, [sb, #1]!
	cmp r0, #0x20
	beq _02257578
_02257584:
	cmp r0, #0
	strne sb, [sl, #0x344]
	ldrnesb r0, [sb]
	cmpne r0, #0
	beq _022575A8
_02257598:
	ldrsb r0, [sb, #1]!
	add r8, r8, #1
	cmp r0, #0
	bne _02257598
_022575A8:
	cmp r5, #0
	beq _0225762C
	ldr r4, [sl, #0x32c]
	add r0, r5, #1
	bl DWCi_GsMalloc
	str r0, [sl, #0x32c]
	cmp r0, #0
	beq _022575E0
	mov r1, r4
	mov r2, r5
	bl memcpy
	ldr r0, [sl, #0x32c]
	mov r1, #0
	strb r1, [r0, r5]
_022575E0:
	ldr r0, [sl, #0x32c]
	add r1, sl, #0x328
	bl FUN_022570D4
	cmp r0, #0
	bne _02257640
	ldr r0, [sl, #0x328]
	cmp r0, #0
	beq _0225760C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x328]
_0225760C:
	ldr r0, [sl, #0x32c]
	cmp r0, #0
	beq _02257624
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x32c]
_02257624:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0225762C:
	mov r0, #0
	str r0, [sl, #0x32c]
	str r0, [sl, #0x330]
	str r0, [sl, #0x334]
	str r0, [sl, #0x338]
_02257640:
	cmp r7, #0
	beq _022576DC
	ldr r0, [sl, #0x340]
	add r1, sl, #0x328
	bl FUN_022572E0
	cmp r0, #0
	bne _02257784
	ldr r0, [sl, #0x328]
	cmp r0, #0
	beq _02257674
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x328]
_02257674:
	ldr r0, [sl, #0x32c]
	cmp r0, #0
	beq _0225768C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x32c]
_0225768C:
	ldr r0, [sl, #0x330]
	cmp r0, #0
	beq _022576A4
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x330]
_022576A4:
	ldr r0, [sl, #0x334]
	cmp r0, #0
	beq _022576BC
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x334]
_022576BC:
	ldr r0, [sl, #0x338]
	cmp r0, #0
	beq _022576D4
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x338]
_022576D4:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_022576DC:
	cmp r8, #0
	beq _02257778
	ldr r0, [sl, #0x344]
	add r1, sl, #0x328
	bl FUN_022572E0
	cmp r0, #0
	bne _02257784
	ldr r0, [sl, #0x328]
	cmp r0, #0
	beq _02257710
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x328]
_02257710:
	ldr r0, [sl, #0x32c]
	cmp r0, #0
	beq _02257728
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x32c]
_02257728:
	ldr r0, [sl, #0x330]
	cmp r0, #0
	beq _02257740
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x330]
_02257740:
	ldr r0, [sl, #0x334]
	cmp r0, #0
	beq _02257758
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x334]
_02257758:
	ldr r0, [sl, #0x338]
	cmp r0, #0
	beq _02257770
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x338]
_02257770:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02257778:
	mov r0, #0
	str r0, [sl, #0x348]
	str r0, [sl, #0x34c]
_02257784:
	cmp r8, #0
	beq _022577C0
	ldr r4, [sl, #0x344]
	add r0, r8, #1
	bl DWCi_GsMalloc
	str r0, [sl, #0x344]
	cmp r0, #0
	beq _022577C8
	mov r1, r4
	mov r2, r8
	bl memcpy
	ldr r0, [sl, #0x344]
	mov r1, #0
	strb r1, [r0, r8]
	b _022577C8
_022577C0:
	mov r0, #0
	str r0, [sl, #0x344]
_022577C8:
	cmp r6, #0
	beq _02257804
	ldr r4, [sl, #0x33c]
	add r0, r6, #1
	bl DWCi_GsMalloc
	str r0, [sl, #0x33c]
	cmp r0, #0
	beq _0225780C
	mov r1, r4
	mov r2, r6
	bl memcpy
	ldr r0, [sl, #0x33c]
	mov r1, #0
	strb r1, [r0, r6]
	b _0225780C
_02257804:
	mov r0, #0
	str r0, [sl, #0x33c]
_0225780C:
	cmp r7, #0
	beq _02257848
	ldr r4, [sl, #0x340]
	add r0, r7, #1
	bl DWCi_GsMalloc
	str r0, [sl, #0x340]
	cmp r0, #0
	beq _02257850
	mov r1, r4
	mov r2, r7
	bl memcpy
	ldr r0, [sl, #0x340]
	mov r1, #0
	strb r1, [r0, r7]
	b _02257850
_02257848:
	mov r0, #0
	str r0, [sl, #0x340]
_02257850:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02257858: .word 0x0225B4FC
_0225785C: .word 0x0225B158
_02257860: .word 0x0225B0C4
	arm_func_end FUN_02257424

	arm_func_start FUN_02257864
FUN_02257864: // 0x02257864
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov sl, r0
	ldr r5, [sl, #0x108]
	ldrsb r0, [r5]
	cmp r0, #0
	beq _02257A50
	b _02257884
_02257880:
	add r5, r5, #1
_02257884:
	ldrsb r0, [r5]
	cmp r0, #0xd
	cmpne r0, #0xa
	beq _02257880
	cmp r0, #0
	beq _02257A50
	mov r6, r5
	mov r7, r5
	b _022578B4
_022578A8:
	cmp r0, #0x20
	movne r7, r6
	add r6, r6, #1
_022578B4:
	ldrsb r0, [r6]
	cmp r0, #0xa
	cmpne r0, #0xd
	cmpne r0, #0
	bne _022578A8
	cmp r0, #0
	beq _02257A50
	ldrsb r8, [r7, #1]
	mov r0, #0
	strb r0, [r7, #1]
	ldr r0, [sl, #0x328]
	cmp r0, #0
	beq _022578F4
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x328]
_022578F4:
	ldr r0, [sl, #0x32c]
	cmp r0, #0
	beq _0225790C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x32c]
_0225790C:
	ldr r0, [sl, #0x330]
	cmp r0, #0
	beq _02257924
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x330]
_02257924:
	ldr r0, [sl, #0x334]
	cmp r0, #0
	beq _0225793C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x334]
_0225793C:
	ldr r0, [sl, #0x338]
	cmp r0, #0
	beq _02257954
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x338]
_02257954:
	ldr r0, [sl, #0x33c]
	cmp r0, #0
	beq _0225796C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x33c]
_0225796C:
	ldr r0, [sl, #0x340]
	cmp r0, #0
	beq _02257984
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x340]
_02257984:
	ldr r0, [sl, #0x344]
	cmp r0, #0
	beq _0225799C
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x344]
_0225799C:
	ldr r0, [sl, #0x34c]
	mov sb, #0
	cmp r0, #0
	ble _022579DC
	mov r4, sb
_022579B0:
	ldr r0, [sl, #0x348]
	ldr r0, [r0, sb, lsl #2]
	cmp r0, #0
	beq _022579CC
	bl DWCi_GsFree
	ldr r0, [sl, #0x348]
	str r4, [r0, sb, lsl #2]
_022579CC:
	ldr r0, [sl, #0x34c]
	add sb, sb, #1
	cmp sb, r0
	blt _022579B0
_022579DC:
	ldr r0, [sl, #0x348]
	cmp r0, #0
	beq _022579F4
	bl DWCi_GsFree
	mov r0, #0
	str r0, [sl, #0x348]
_022579F4:
	mov r1, #0
	add r0, sl, #0x328
	mov r2, #0x28
	str r1, [sl, #0x34c]
	bl memset
	mov r0, sl
	mov r1, r5
	bl FUN_02257424
	cmp r0, #0
	bne _02257A34
	add r0, sl, #0x328
	mov r1, #0
	mov r2, #0x28
	bl memset
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02257A34:
	strb r8, [r7, #1]
	ldr r1, [sl, #0x108]
	add r0, sl, #0x108
	sub r1, r6, r1
	bl FUN_02255CB0
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02257A50:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end FUN_02257864

	arm_func_start FUN_02257A58
FUN_02257A58: // 0x02257A58
	stmfd sp!, {r4, lr}
	movs r4, r0
	bne _02257A78
	ldr r0, _02257C10 // =0x0225B234
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257A78:
	ldr r0, [r4, #4]
	cmp r0, #2
	bls _02257A98
	ldr r0, _02257C20 // =0x0225B244
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257A98:
	adds r0, r4, #0x108
	bne _02257AB4
	ldr r0, _02257C24 // =0x0225B2C4
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257AB4:
	ldr r0, [r4, #0x110]
	cmp r0, #0
	bge _02257AD4
	ldr r0, _02257C28 // =0x0225B2E4
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257AD4:
	ldr r0, [r4, #0x110]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02257AFC
	ldr r0, _02257C2C // =0x0225B308
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257AFC:
	ldr r0, [r4, #0x10c]
	cmp r0, #0
	bge _02257B1C
	ldr r0, _02257C30 // =0x0225B338
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257B1C:
	ldr r1, [r4, #0x10c]
	ldr r0, [r4, #0x110]
	cmp r1, r0
	ble _02257B40
	ldr r0, _02257C34 // =0x0225B35C
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257B40:
	adds r0, r4, #0x114
	bne _02257B5C
	ldr r0, _02257C38 // =0x0225B39C
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257B5C:
	ldr r0, [r4, #0x11c]
	cmp r0, #0
	bge _02257B7C
	ldr r0, _02257C3C // =0x0225B3BC
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257B7C:
	ldr r0, [r4, #0x11c]
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #19
	adds r0, r1, r0, ror #19
	beq _02257BA4
	ldr r0, _02257C40 // =0x0225B3E0
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257BA4:
	ldr r0, [r4, #0x118]
	cmp r0, #0
	bge _02257BC4
	ldr r0, _02257C44 // =0x0225B414
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257BC4:
	ldr r1, [r4, #0x118]
	ldr r0, [r4, #0x11c]
	cmp r1, r0
	ble _02257BE8
	ldr r0, _02257C48 // =0x0225B438
	ldr r1, _02257C14 // =0x0225B158
	ldr r2, _02257C18 // =0x0225B064
	ldr r3, _02257C1C // =0x000003E9
	bl __msl_assertion_failed
_02257BE8:
	ldr r0, [r4, #0x10c]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	mov r0, r4
	bl FUN_02257864
	cmp r0, #0
	addne r0, r4, #0x328
	moveq r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_02257C10: .word 0x0225B234
_02257C14: .word 0x0225B158
_02257C18: .word 0x0225B064
_02257C1C: .word 0x000003E9
_02257C20: .word 0x0225B244
_02257C24: .word 0x0225B2C4
_02257C28: .word 0x0225B2E4
_02257C2C: .word 0x0225B308
_02257C30: .word 0x0225B338
_02257C34: .word 0x0225B35C
_02257C38: .word 0x0225B39C
_02257C3C: .word 0x0225B3BC
_02257C40: .word 0x0225B3E0
_02257C44: .word 0x0225B414
_02257C48: .word 0x0225B438
	arm_func_end FUN_02257A58

	arm_func_start FUN_02257C4C
FUN_02257C4C: // 0x02257C4C
	ldr r1, _02257C80 // =0xAAAAAAAB
	mov r3, #3
	umull r1, r2, r0, r1
	mov r2, r2, lsr #1
	umull r1, r2, r3, r2
	subs r2, r0, r1
	ldr r1, _02257C80 // =0xAAAAAAAB
	movne r3, #1
	umull r1, r2, r0, r1
	moveq r3, #0
	add r0, r3, r2, lsr #1
	mov r0, r0, lsl #2
	bx lr
	.align 2, 0
_02257C80: .word 0xAAAAAAAB
	arm_func_end FUN_02257C4C

	arm_func_start FUN_02257C84
FUN_02257C84: // 0x02257C84
	ldr r1, _02257C94 // =0x0225C6F8
	orr r0, r0, r0, lsl #16
	str r0, [r1]
	bx lr
	.align 2, 0
_02257C94: .word 0x0225C6F8
	arm_func_end FUN_02257C84

	arm_func_start FUN_02257C98
FUN_02257C98: // 0x02257C98
	stmfd sp!, {r3, lr}
	ldr r1, _02257CD0 // =0x0225B52C
	ldr r0, _02257CD4 // =0x0225C6F8
	ldr r3, [r1, #0x48]
	ldr r2, [r1, #0x44]
	ldr r0, [r0]
	ldr r1, [r1, #0x4c]
	mla r0, r2, r0, r3
	bl _u32_div_f
	ldr r0, _02257CD4 // =0x0225C6F8
	mov r2, r1, asr #0x10
	str r1, [r0]
	and r0, r2, #0xff
	ldmia sp!, {r3, pc}
	.align 2, 0
_02257CD0: .word 0x0225B52C
_02257CD4: .word 0x0225C6F8
	arm_func_end FUN_02257C98

	arm_func_start FUN_02257CD8
FUN_02257CD8: // 0x02257CD8
	stmfd sp!, {r4, r5, r6, r7, r8, lr}
	ldr r4, _02257DC8 // =0x0225B52C
	mov ip, #0
	str ip, [r4, #0xc]
	ldr r4, [sp, #0x18]
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bl strlen
	cmp r0, #0x20
	movhs r0, #0
	ldmhsia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r7
	bl strlen
	cmp r0, #0x14
	movne r0, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	sub r0, r6, #5
	tst r0, #7
	movne r0, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	tst r5, #1
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r2, #0
	mov r1, r2
_02257D44:
	mov r0, r4, lsr r1
	and r0, r0, #1
	cmp r0, #1
	add r1, r1, #1
	addeq r2, r2, #1
	cmp r1, #0x20
	blt _02257D44
	cmp r2, #1
	movne r0, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _02257DCC // =0x0225B53C
	mov r1, r8
	mov r2, #0x20
	bl strncpy
	ldr r3, _02257DD0 // =0x0225B55C
	mov r2, #0xa
_02257D84:
	ldrb r1, [r7]
	ldrb r0, [r7, #1]
	add r7, r7, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02257D84
	ldr r1, _02257DC8 // =0x0225B52C
	ldr r0, [sp, #0x1c]
	str r6, [r1, #0x44]
	str r5, [r1, #0x48]
	str r4, [r1, #0x4c]
	str r0, [r1, #0x50]
	mov r0, #1
	str r0, [r1, #0xc]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_02257DC8: .word 0x0225B52C
_02257DCC: .word 0x0225B53C
_02257DD0: .word 0x0225B55C
	arm_func_end FUN_02257CD8
_02257DD4:
	.byte 0xF0, 0x41, 0x2D, 0xE9, 0x50, 0xD0, 0x4D, 0xE2, 0x94, 0x41, 0x9F, 0xE5
	.byte 0x00, 0x30, 0x8D, 0xE2, 0x00, 0x60, 0xA0, 0xE1, 0x01, 0x50, 0xA0, 0xE1, 0x08, 0x20, 0xA0, 0xE3
	.byte 0x00, 0x10, 0xD4, 0xE5, 0x01, 0x00, 0xD4, 0xE5, 0x02, 0x40, 0x84, 0xE2, 0x00, 0x10, 0xC3, 0xE5
	.byte 0x01, 0x00, 0xC3, 0xE5, 0x02, 0x30, 0x83, 0xE2, 0x01, 0x20, 0x52, 0xE2, 0xF7, 0xFF, 0xFF, 0x1A
	.byte 0x00, 0x00, 0xD4, 0xE5, 0x28, 0x00, 0x55, 0xE3, 0x50, 0xD0, 0x8D, 0xD2, 0x00, 0x00, 0xC3, 0xE5
	.byte 0x00, 0x00, 0xA0, 0xD3, 0xF0, 0x81, 0xBD, 0xD8, 0x28, 0x40, 0x45, 0xE2, 0x04, 0x00, 0xA0, 0xE1
	.byte 0x85, 0xFF, 0xFF, 0xEB, 0x29, 0x10, 0x80, 0xE2, 0x07, 0x00, 0xA0, 0xE3, 0x4F, 0xFE, 0xFD, 0xEB
	.byte 0x00, 0x70, 0xB0, 0xE1, 0x50, 0xD0, 0x8D, 0x02, 0x00, 0x00, 0xA0, 0x03, 0xF0, 0x81, 0xBD, 0x08
	.byte 0x24, 0x81, 0x9F, 0xE5, 0x07, 0x30, 0xA0, 0xE1, 0x0A, 0x20, 0xA0, 0xE3, 0x00, 0x10, 0xD8, 0xE5
	.byte 0x01, 0x00, 0xD8, 0xE5, 0x02, 0x80, 0x88, 0xE2, 0x01, 0x20, 0x52, 0xE2, 0x00, 0x10, 0xC3, 0xE5
	.byte 0x01, 0x00, 0xC3, 0xE5, 0x02, 0x30, 0x83, 0xE2, 0xF7, 0xFF, 0xFF, 0x1A, 0x06, 0x00, 0xA0, 0xE1
	.byte 0x04, 0x20, 0xA0, 0xE1, 0x14, 0x10, 0x87, 0xE2, 0x02, 0x30, 0xA0, 0xE3, 0x97, 0x4C, 0xFE, 0xEB
	.byte 0x04, 0x00, 0xA0, 0xE1, 0x6C, 0xFF, 0xFF, 0xEB, 0x14, 0x10, 0x87, 0xE2, 0xD8, 0x30, 0x9F, 0xE5
	.byte 0x00, 0x80, 0x81, 0xE0, 0x0A, 0x20, 0xA0, 0xE3, 0x00, 0x10, 0xD3, 0xE5, 0x01, 0x00, 0xD3, 0xE5
	.byte 0x02, 0x30, 0x83, 0xE2, 0x01, 0x20, 0x52, 0xE2, 0x00, 0x10, 0xC8, 0xE5, 0x01, 0x00, 0xC8, 0xE5
	.byte 0x02, 0x80, 0x88, 0xE2, 0xF7, 0xFF, 0xFF, 0x1A, 0x04, 0x00, 0xA0, 0xE1, 0x5E, 0xFF, 0xFF, 0xEB
	.byte 0x00, 0x20, 0xA0, 0xE1, 0x11, 0x00, 0x8D, 0xE2, 0x07, 0x10, 0xA0, 0xE1, 0x28, 0x20, 0x82, 0xE2
	.byte 0x60, 0xEC, 0xF9, 0xEB, 0x07, 0x10, 0xA0, 0xE1, 0x07, 0x00, 0xA0, 0xE3, 0x00, 0x20, 0xA0, 0xE3
	.byte 0x33, 0xFE, 0xFD, 0xEB, 0x00, 0x20, 0xA0, 0xE3, 0x11, 0x30, 0x8D, 0xE2, 0x02, 0x70, 0xA0, 0xE1
	.byte 0x25, 0x10, 0x8D, 0xE2, 0x00, 0x80, 0x8D, 0xE2, 0x01, 0xC0, 0xD3, 0xE4, 0x01, 0x20, 0x82, 0xE2
	.byte 0x07, 0x00, 0x81, 0xE0, 0x4C, 0xE2, 0xA0, 0xE1, 0x0F, 0xC0, 0x0C, 0xE2, 0xDE, 0xE0, 0x98, 0xE1
	.byte 0xDC, 0xC0, 0x98, 0xE1, 0x14, 0x00, 0x52, 0xE3, 0x07, 0xE0, 0xC1, 0xE7, 0x01, 0xC0, 0xC0, 0xE5
	.byte 0x02, 0x70, 0x87, 0xE2, 0xF3, 0xFF, 0xFF, 0xBA, 0x28, 0x00, 0x45, 0xE2, 0x00, 0x30, 0xA0, 0xE3
	.byte 0x00, 0x00, 0x86, 0xE0, 0x28, 0x20, 0xA0, 0xE3, 0x4D, 0x30, 0xCD, 0xE5, 0xB5, 0x03, 0xFA, 0xEB
	.byte 0x00, 0x00, 0x50, 0xE3, 0x50, 0xD0, 0x8D, 0x12, 0x00, 0x00, 0xA0, 0x13, 0xF0, 0x81, 0xBD, 0x18
	.byte 0x18, 0x10, 0x9F, 0xE5, 0x01, 0x00, 0xA0, 0xE3, 0x68, 0x60, 0x81, 0xE5, 0x6C, 0x40, 0x81, 0xE5
	.byte 0x50, 0xD0, 0x8D, 0xE2, 0xF0, 0x81, 0xBD, 0xE8, 0xE4, 0x8C, 0x25, 0x02, 0x5C, 0xB5, 0x25, 0x02
	.byte 0x2C, 0xB5, 0x25, 0x02, 0x1E, 0xFF, 0x2F, 0xE1, 0x38, 0x40, 0x2D, 0xE9, 0x48, 0xD0, 0x4D, 0xE2
	.byte 0x40, 0x32, 0x9F, 0xE5, 0x00, 0x40, 0xE0, 0xE3, 0x08, 0x40, 0x83, 0xE5, 0x00, 0xC0, 0x93, 0xE5
	.byte 0x00, 0x50, 0xA0, 0xE1, 0x01, 0x00, 0x5C, 0xE3, 0x01, 0x40, 0xA0, 0xE1, 0x48, 0xD0, 0x8D, 0x02
	.byte 0x38, 0x80, 0xBD, 0x08, 0x00, 0x00, 0x52, 0xE3, 0x83, 0x00, 0x00, 0x1A, 0x05, 0x00, 0x5C, 0xE3
	.byte 0x03, 0x00, 0x00, 0x0A, 0x07, 0x00, 0x5C, 0xE3, 0x4B, 0x00, 0x00, 0x0A, 0x48, 0xD0, 0x8D, 0xE2
	.byte 0x38, 0x80, 0xBD, 0xE8, 0x20, 0x00, 0x54, 0xE3, 0x42, 0x00, 0x00, 0x1A, 0x58, 0x00, 0x93, 0xE5
	.byte 0xF4, 0x31, 0x9F, 0xE5, 0x00, 0xE0, 0x8D, 0xE2, 0x14, 0x40, 0x80, 0xE2, 0x08, 0x20, 0xA0, 0xE3
	.byte 0x00, 0x10, 0xD3, 0xE5, 0x01, 0x00, 0xD3, 0xE5, 0x02, 0x30, 0x83, 0xE2, 0x00, 0x10, 0xCE, 0xE5
	.byte 0x01, 0x00, 0xCE, 0xE5, 0x02, 0xE0, 0x8E, 0xE2, 0x01, 0x20, 0x52, 0xE2, 0xF7, 0xFF, 0xFF, 0x1A
	.byte 0x00, 0x00, 0xD3, 0xE5, 0xC4, 0x31, 0x9F, 0xE5, 0x11, 0xC0, 0x8D, 0xE2, 0x00, 0x00, 0xCE, 0xE5
	.byte 0x0A, 0x20, 0xA0, 0xE3, 0x00, 0x10, 0xD3, 0xE5, 0x01, 0x00, 0xD3, 0xE5, 0x02, 0x30, 0x83, 0xE2
	.byte 0x00, 0x10, 0xCC, 0xE5, 0x01, 0x00, 0xCC, 0xE5, 0x02, 0xC0, 0x8C, 0xE2, 0x01, 0x20, 0x52, 0xE2
	.byte 0xF7, 0xFF, 0xFF, 0x1A, 0x25, 0x00, 0x8D, 0xE2, 0x05, 0x10, 0xA0, 0xE1, 0x20, 0x20, 0xA0, 0xE3
	.byte 0x18, 0xF4, 0xF9, 0xEB, 0x11, 0x10, 0x8D, 0xE2, 0x04, 0x00, 0xA0, 0xE1, 0x34, 0x20, 0xA0, 0xE3
	.byte 0x00, 0xEC, 0xF9, 0xEB, 0x00, 0x00, 0xA0, 0xE3, 0x68, 0x21, 0x9F, 0xE5, 0x00, 0x10, 0xA0, 0xE1
	.byte 0x00, 0xE0, 0x8D, 0xE2, 0x00, 0xC0, 0xD4, 0xE7, 0x58, 0x30, 0x92, 0xE5, 0x4C, 0xC2, 0xA0, 0xE1
	.byte 0xDC, 0xC0, 0x9E, 0xE1, 0x01, 0xC0, 0xC3, 0xE7, 0x00, 0xC0, 0xD4, 0xE7, 0x58, 0x30, 0x92, 0xE5
	.byte 0x01, 0x00, 0x80, 0xE2, 0x0F, 0xC0, 0x0C, 0xE2, 0xDC, 0xC0, 0x9E, 0xE1, 0x01, 0x30, 0x83, 0xE0
	.byte 0x14, 0x00, 0x50, 0xE3, 0x01, 0xC0, 0xC3, 0xE5, 0x02, 0x10, 0x81, 0xE2, 0xF0, 0xFF, 0xFF, 0xBA
	.byte 0x2C, 0x01, 0x9F, 0xE5, 0xA9, 0x02, 0xFA, 0xEB, 0x18, 0x11, 0x9F, 0xE5, 0x26, 0x40, 0xA0, 0xE3
	.byte 0x58, 0x30, 0x91, 0xE5, 0x29, 0x20, 0xA0, 0xE3, 0x00, 0x40, 0x43, 0xE7, 0x58, 0x10, 0x91, 0xE5
	.byte 0x10, 0x01, 0x9F, 0xE5, 0xDA, 0x02, 0xFA, 0xEB, 0xF8, 0x00, 0x9F, 0xE5, 0x06, 0x10, 0xA0, 0xE3
	.byte 0x00, 0x10, 0x80, 0xE5, 0x00, 0x00, 0x00, 0xEA, 0xF2, 0x00, 0x00, 0xEB, 0x05, 0x00, 0xA0, 0xE1
	.byte 0xEE, 0xFD, 0xFD, 0xEB, 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0xE8, 0x00, 0x9F, 0xE5
	.byte 0x96, 0x02, 0xFA, 0xEB, 0x00, 0x20, 0xA0, 0xE1, 0xDC, 0x10, 0x9F, 0xE5, 0x05, 0x00, 0xA0, 0xE1
	.byte 0x44, 0x03, 0xFA, 0xEB, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x05, 0x00, 0xA0, 0xE1
	.byte 0xE2, 0xFD, 0xFD, 0xEB, 0xE3, 0x00, 0x00, 0xEB, 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8
	.byte 0xA0, 0x00, 0x9F, 0xE5, 0x60, 0x00, 0x90, 0xE5, 0x00, 0x00, 0x50, 0xE3, 0x03, 0x00, 0x00, 0x0A
	.byte 0xDA, 0xFD, 0xFD, 0xEB, 0x8C, 0x00, 0x9F, 0xE5, 0x00, 0x10, 0xA0, 0xE3, 0x60, 0x10, 0x80, 0xE5
	.byte 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1, 0x1D, 0xFF, 0xFF, 0xEB, 0x00, 0x00, 0x50, 0xE3
	.byte 0x04, 0x00, 0x00, 0x1A, 0x05, 0x00, 0xA0, 0xE1, 0xD0, 0xFD, 0xFD, 0xEB, 0xD1, 0x00, 0x00, 0xEB
	.byte 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x58, 0x00, 0x9F, 0xE5, 0x70, 0x20, 0x90, 0xE5
	.byte 0x00, 0x00, 0x52, 0xE3, 0x09, 0x00, 0x00, 0x0A, 0x05, 0x00, 0xA0, 0xE1, 0x04, 0x10, 0xA0, 0xE1
	.byte 0x32, 0xFF, 0x2F, 0xE1, 0x00, 0x00, 0x50, 0xE3, 0x04, 0x00, 0x00, 0x1A, 0x05, 0x00, 0xA0, 0xE1
	.byte 0xC2, 0xFD, 0xFD, 0xEB, 0xC3, 0x00, 0x00, 0xEB, 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8
	.byte 0x20, 0x00, 0x9F, 0xE5, 0x08, 0x10, 0xA0, 0xE3, 0x60, 0x50, 0x80, 0xE5, 0x64, 0x40, 0x80, 0xE5
	.byte 0x00, 0x10, 0x80, 0xE5, 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0xB9, 0x00, 0x00, 0xEB
	.byte 0x48, 0xD0, 0x8D, 0xE2, 0x38, 0x80, 0xBD, 0xE8, 0x2C, 0xB5, 0x25, 0x02, 0xF5, 0x8C, 0x25, 0x02
	.byte 0x5C, 0xB5, 0x25, 0x02, 0xCC, 0xB5, 0x25, 0x02, 0xA0, 0xB5, 0x25, 0x02, 0xD4, 0xB5, 0x25, 0x02

	arm_func_start FUN_022581F0
FUN_022581F0: // 0x022581F0
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r4, [sp, #0x20]
	mov sb, r2
	add r7, r4, sb
	mov r4, r0
	mov sl, r1
	add r1, r7, #4
	mov r0, #7
	mov r8, r3
	mov r6, #0
	bl DWC_Alloc
	movs r5, r0
	moveq r0, #2
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	cmp sb, #0
	mov r2, r6
	ble _0225824C
_02258234:
	ldrb r1, [sl, r2]
	add r0, r5, r2
	add r2, r2, #1
	strb r1, [r0, #4]
	cmp r2, sb
	blt _02258234
_0225824C:
	ldr r3, [sp, #0x20]
	mov sl, #0
	cmp r3, #0
	ble _02258278
	add r2, r5, sb
_02258260:
	ldrb r1, [r8, sl]
	add r0, r2, sl
	add sl, sl, #1
	strb r1, [r0, #4]
	cmp sl, r3
	blt _02258260
_02258278:
	cmp r7, #0
	mov r1, #0
	ble _0225829C
_02258284:
	add r0, r5, r1
	ldrb r0, [r0, #4]
	add r1, r1, #1
	cmp r1, r7
	add r6, r6, r0
	blt _02258284
_0225829C:
	mov r0, r6
	bl FUN_02257C84
	cmp r7, #0
	mov r8, #0
	ble _022582D0
_022582B0:
	bl FUN_02257C98
	add r2, r5, r8
	ldrb r1, [r2, #4]
	add r8, r8, #1
	cmp r8, r7
	eor r0, r1, r0
	strb r0, [r2, #4]
	blt _022582B0
_022582D0:
	ldr r1, _02258334 // =0x0225B52C
	mov r0, r5
	ldr r2, [r1, #0x50]
	mov r1, r4
	eor r6, r6, r2
	mov r2, r6, lsr #0x18
	strb r2, [r5]
	mov r2, r6, lsr #0x10
	strb r2, [r5, #1]
	mov r2, r6, lsr #8
	strb r2, [r5, #2]
	add r2, r7, #4
	mov r3, #2
	strb r6, [r5, #3]
	bl B64Encode
	mov r1, r5
	mov r0, #7
	mov r2, #0
	bl DWC_Free
	add r0, r7, #4
	bl FUN_02257C4C
	mov r1, #0
	strb r1, [r4, r0]
	mov r0, r1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_02258334: .word 0x0225B52C
	arm_func_end FUN_022581F0

	arm_func_start FUN_02258338
FUN_02258338: // 0x02258338
	stmfd sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x28
	mov r5, #0
	add r4, sp, #8
	ldr r2, _022584A4 // =0x0225B52C
	sub r3, r5, #1
	strb r5, [r4]
	strb r5, [r4, #1]
	strb r5, [r4, #2]
	strb r5, [r4, #3]
	strb r5, [r4, #4]
	strb r5, [r4, #5]
	strb r5, [r4, #6]
	strb r5, [r4, #7]
	strb r5, [r4, #8]
	str r3, [r2, #8]
	str r5, [r2, #0x54]
	str r5, [r2, #0x58]
	str r5, [r2, #0x5c]
	str r5, [r2, #0x60]
	mov r4, r1
	str r5, [r2, #0x64]
	cmp r0, #0
	beq _022583B4
	cmp r0, #1
	beq _022583C4
	cmp r0, #2
	ldreq r1, _022584A8 // =0x0225B628
	ldreq r0, _022584AC // =0x0225B528
	streq r1, [r0]
	b _022583D0
_022583B4:
	ldr r1, _022584B0 // =0x0225B5DC
	ldr r0, _022584AC // =0x0225B528
	str r1, [r0]
	b _022583D0
_022583C4:
	ldr r1, _022584B4 // =0x0225B604
	ldr r0, _022584AC // =0x0225B528
	str r1, [r0]
_022583D0:
	add r0, sp, #0x11
	mov r1, r4
	mov r2, #0x14
	bl strncpy
	mov r3, #0
	add r0, sp, #8
	add r1, r4, #0x14
	mov r2, #8
	strb r3, [sp, #0x25]
	bl strncpy
	mov r1, #0
	mov r2, #0x10
	bl strtoul
	mov r7, r0
	add r0, sp, #8
	add r1, r4, #0x1c
	mov r2, #8
	bl strncpy
	mov r1, #0
	mov r2, #0x10
	bl strtoul
	mov r6, r0
	add r0, sp, #8
	add r1, r4, #0x24
	mov r2, #8
	bl strncpy
	mov r1, #0
	mov r2, #0x10
	bl strtoul
	mov r5, r0
	add r0, sp, #8
	add r1, r4, #0x2c
	mov r2, #8
	bl strncpy
	mov r1, #0
	mov r2, #0x10
	bl strtoul
	str r5, [sp]
	str r0, [sp, #4]
	mov r2, r7
	mov r3, r6
	add r0, r4, #0x34
	add r1, sp, #0x11
	bl FUN_02257CD8
	mov r0, #0
	bl DWC_InitGHTTP
	ldr r0, _022584A4 // =0x0225B52C
	mov r2, #3
	mov r1, #1
	str r2, [r0]
	str r1, [r0, #4]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_022584A4: .word 0x0225B52C
_022584A8: .word 0x0225B628
_022584AC: .word 0x0225B528
_022584B0: .word 0x0225B5DC
_022584B4: .word 0x0225B604
	arm_func_end FUN_02258338

	arm_func_start FUN_022584B8
FUN_022584B8: // 0x022584B8
	stmfd sp!, {r3, lr}
	bl DWCi_IsError
	cmp r0, #0
	bne _022584D4
	ldr r1, _022584E4 // =0xFFFEA048
	mov r0, #6
	bl DWCi_SetError
_022584D4:
	ldr r0, _022584E8 // =0x0225B52C
	mov r1, #1
	str r1, [r0]
	ldmia sp!, {r3, pc}
	.align 2, 0
_022584E4: .word 0xFFFEA048
_022584E8: .word 0x0225B52C
	arm_func_end FUN_022584B8

	arm_func_start FUN_022584EC
FUN_022584EC: // 0x022584EC
	stmfd sp!, {r3, lr}
	sub sp, sp, #8
	ldr r0, _022585E8 // =0x0225B52C
	ldr r1, [r0]
	cmp r1, #8
	addls pc, pc, r1, lsl #2
	b _022585D8
_02258508: // jump table
	b _022585D8 // case 0
	b _0225852C // case 1
	b _022585D8 // case 2
	b _022585D8 // case 3
	b _02258534 // case 4
	b _02258574 // case 5
	b _02258588 // case 6
	b _022585C8 // case 7
	b _022585D8 // case 8
_0225852C:
	bl FUN_022584B8
	b _022585D8
_02258534:
	ldr r2, _022585EC // =0x02257F88
	mov r1, #0
	str r2, [sp]
	str r1, [sp, #4]
	ldr r0, [r0, #0x54]
	ldr r3, _022585F0 // =0x02257F84
	mov r2, r1
	bl DWC_GetGHTTPDataEx
	ldr r1, _022585E8 // =0x0225B52C
	cmp r0, #0
	str r0, [r1, #8]
	movge r0, #5
	strge r0, [r1]
	bge _022585D8
	bl FUN_022584B8
	b _022585D8
_02258574:
	bl DWC_ProcessGHTTP
	cmp r0, #0
	bne _022585D8
	bl FUN_022584B8
	b _022585D8
_02258588:
	ldr r2, _022585EC // =0x02257F88
	mov r1, #0
	str r2, [sp]
	str r1, [sp, #4]
	ldr r0, [r0, #0x54]
	ldr r3, _022585F0 // =0x02257F84
	mov r2, r1
	bl DWC_GetGHTTPDataEx
	ldr r1, _022585E8 // =0x0225B52C
	cmp r0, #0
	str r0, [r1, #8]
	movge r0, #7
	strge r0, [r1]
	bge _022585D8
	bl FUN_022584B8
	b _022585D8
_022585C8:
	bl DWC_ProcessGHTTP
	cmp r0, #0
	bne _022585D8
	bl FUN_022584B8
_022585D8:
	ldr r0, _022585E8 // =0x0225B52C
	ldr r0, [r0]
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.align 2, 0
_022585E8: .word 0x0225B52C
_022585EC: .word 0x02257F88
_022585F0: .word 0x02257F84
	arm_func_end FUN_022584EC

	arm_func_start FUN_022585F4
FUN_022585F4: // 0x022585F4
	stmfd sp!, {r3, lr}
	ldr r0, _02258660 // =0x0225B52C
	ldr r1, [r0, #0x54]
	cmp r1, #0
	beq _02258620
	mov r0, #7
	mov r2, #0
	bl DWC_Free
	ldr r0, _02258660 // =0x0225B52C
	mov r1, #0
	str r1, [r0, #0x54]
_02258620:
	ldr r0, _02258660 // =0x0225B52C
	ldr r0, [r0, #0x60]
	cmp r0, #0
	beq _02258644
	bl DWCi_GsFree
	ldr r0, _02258660 // =0x0225B52C
	mov r1, #0
	str r1, [r0, #0x64]
	str r1, [r0, #0x60]
_02258644:
	bl DWC_ShutdownGHTTP
	ldr r0, _02258660 // =0x0225B52C
	mov r1, #2
	str r1, [r0]
	mov r1, #0
	str r1, [r0, #4]
	ldmia sp!, {r3, pc}
	.align 2, 0
_02258660: .word 0x0225B52C
	arm_func_end FUN_022585F4

	arm_func_start FUN_02258664
FUN_02258664: // 0x02258664
	ldr r1, _02258678 // =0x0225B52C
	ldr r2, [r1, #0x6c]
	str r2, [r0]
	ldr r0, [r1, #0x68]
	bx lr
	.align 2, 0
_02258678: .word 0x0225B52C
	arm_func_end FUN_02258664

	arm_func_start FUN_0225867C
FUN_0225867C: // 0x0225867C
	stmfd sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x34
	ldr r4, _022588A4 // =0x0225B52C
	mov sl, r0
	ldr r0, [r4, #0xc]
	mov sb, r1
	cmp r0, #1
	str r2, [sp, #0x10]
	mov r8, r3
	addne sp, sp, #0x34
	movne r0, #3
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [r4, #0x54]
	cmp r1, #0
	beq _022586D0
	mov r0, #7
	mov r2, #0
	bl DWC_Free
	mov r0, r4
	mov r1, #0
	str r1, [r0, #0x54]
_022586D0:
	add r0, r8, #0xc
	bl FUN_02257C4C
	mov r7, r0
	ldr r0, _022588A8 // =0x0225B650
	bl strlen
	mov r6, r0
	ldr r0, _022588AC // =0x0225B5CC
	bl strlen
	mov r5, r0
	ldr r2, _022588B0 // =0x0225B658
	add r0, sp, #0x24
	mov r1, #0x10
	mov r3, sb
	bl snprintf
	mov fp, r0
	ldr r0, _022588B4 // =0x0225B65C
	bl strlen
	str r0, [sp, #0x14]
	mov r0, sl
	bl strlen
	str r0, [sp, #0x18]
	ldr r0, _022588B8 // =0x0225B528
	ldr r0, [r0]
	bl strlen
	mov r4, r0
	ldr r0, _022588BC // =0x0225B53C
	bl strlen
	add r1, r4, r0
	ldr r0, [sp, #0x18]
	add r1, r0, r1
	ldr r0, [sp, #0x14]
	add r0, r0, r1
	add r0, fp, r0
	add r0, r5, r0
	add r0, r0, #0x29
	add r0, r6, r0
	add r1, r7, r0
	mov r0, #7
	bl DWC_Alloc
	ldr r1, _022588A4 // =0x0225B52C
	cmp r0, #0
	str r0, [r1, #0x54]
	addeq sp, sp, #0x34
	moveq r0, #2
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	str sl, [sp]
	ldr r1, _022588C0 // =0x0225B680
	str sb, [sp, #4]
	str r1, [sp, #8]
	ldr r2, _022588C4 // =0x0225B6AC
	ldr r1, _022588B8 // =0x0225B528
	str r2, [sp, #0xc]
	ldr r2, [r1]
	ldr r1, _022588C8 // =0x0225B664
	ldr r3, _022588BC // =0x0225B53C
	bl sprintf
	ldr r0, _022588A4 // =0x0225B52C
	ldr r4, [r0, #0x54]
	mov r0, r4
	bl strlen
	add r2, r4, r0
	ldr r1, _022588A4 // =0x0225B52C
	ldr r0, _022588A8 // =0x0225B650
	str r2, [r1, #0x5c]
	bl strlen
	ldr r4, _022588A4 // =0x0225B52C
	ldr r3, [sp, #0x10]
	ldr r5, [r4, #0x5c]
	add r1, sp, #0x1c
	sub r0, r5, r0
	sub r0, r0, #0x28
	str r0, [r4, #0x58]
	str sb, [sp, #0x1c]
	str r8, [sp, #0x20]
	str r8, [sp]
	ldr r0, [r4, #0x5c]
	mov r2, #8
	bl FUN_022581F0
	cmp r0, #2
	bne _0225883C
	mov r1, r4
	ldr r1, [r1, #0x54]
	mov r0, #7
	mov r2, #0
	bl DWC_Free
	mov r0, r4
	mov r1, #0
	str r1, [r0, #0x54]
	add sp, sp, #0x34
	mov r0, #2
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0225883C:
	ldr r0, [sp, #0x5c]
	cmp r0, #0
	beq _0225886C
	mov r0, r4
	ldr r1, _022588CC // =0x0225B5A0
	ldr r0, [r0, #0x58]
	mov r2, #0x28
	bl memcpy
	mov r0, r4
	mov r1, #6
	str r1, [r0]
	b _0225888C
_0225886C:
	ldr r0, _022588AC // =0x0225B5CC
	bl strlen
	mov r1, r4
	ldr r3, [r1, #0x58]
	mov r4, #0
	mov r2, #4
	strb r4, [r3, -r0]
	str r2, [r1]
_0225888C:
	ldr r2, [sp, #0x58]
	ldr r1, _022588A4 // =0x0225B52C
	mov r0, #0
	str r2, [r1, #0x70]
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_022588A4: .word 0x0225B52C
_022588A8: .word 0x0225B650
_022588AC: .word 0x0225B5CC
_022588B0: .word 0x0225B658
_022588B4: .word 0x0225B65C
_022588B8: .word 0x0225B528
_022588BC: .word 0x0225B53C
_022588C0: .word 0x0225B680
_022588C4: .word 0x0225B6AC
_022588C8: .word 0x0225B664
_022588CC: .word 0x0225B5A0
	arm_func_end FUN_0225867C

	arm_func_start FUN_022588D0
FUN_022588D0: // 0x022588D0
	stmfd sp!, {r3, lr}
	sub sp, sp, #8
	ldr lr, [sp, #0x10]
	mov ip, #0
	str lr, [sp]
	str ip, [sp, #4]
	bl FUN_0225867C
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	arm_func_end FUN_022588D0

	arm_func_start ov66_22588F4
ov66_22588F4: // 0x022588F4
	stmfd sp!, {r3, lr}
	sub sp, sp, #8
	ldr lr, [sp, #0x10]
	mov ip, #1
	str lr, [sp]
	str ip, [sp, #4]
	bl FUN_0225867C
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	arm_func_end ov66_22588F4
	// 0x02258918

	.rodata

	.global ov66_rodata
ov66_rodata:
	// todo
	.incbin "ov66_rodata.bin"

	.data
	.global ov66_data
ov66_data:
	.incbin "ov66_data_pt1.bin"
	.word 0x2250aa8 // this needed because of a bug in mwasmarm.exe that is replacing 0xa (newline) with 0xd (carriage return)
	.incbin "ov66_data_pt2.bin"

	.bss
	.global ov66_bss
ov66_bss:
	.space 0x1020
