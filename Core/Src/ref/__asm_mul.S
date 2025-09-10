#include "macros.i"
#include "mulmod.i"
#include "reduce.i"

.syntax unified


.macro reduce_32_asm a, q, t

    add.w \t, \a, #(1<<22)
    asr.w \t, \t, #23
    mls.w \a, \t, \q, \a

.endm

.macro freeze_32_asm a, q, qhalf, t

    reduce_32_asm \a, \q, \t

    cmp   \a, \qhalf
    it    gt
    subgt \a, \a, \q
    cmn   \a, \qhalf
    it    lt
    addlt  \a, \a, \q

	cmp   \a, \qhalf
    it    gt
    subgt \a, \a, \q
    cmn   \a, \qhalf
    it    lt
    addlt  \a, \a, \q

.endm

.macro kred_reduce a0, a1, t0, t1
	ubfx.w	\t0,	\a0, 	#0, #20
	mov.w   \t1, 	\a0, 	LSR #20
	orr.w	\t1, 	\t1, 	\a1, LSL	#12
	rsb.w 	\a0, 	\t0, 	\t0, LSL	 #3
	sub.w	\a0, 	\a0, 	\t1
.endm

//extern void __asm_point_mul_pre(int32_t*, int32_t*, int32_t*, int32_t*);
//__asm_point_mul_pre(a, a, twist_table, const_buff);

.align 4
.global __asm_point_mul_pre
__asm_point_mul_pre:
    push.w {r4-r12, lr}


    ldr.w r3, [r3, #4]

    add.w r12, r0, #1024
    _point_mul_pre_loop:

    ldr.w r4, [r1], #4
    ldrh.w r6, [r2, #4]
    ldrsh.w r7, [r2, #6]
    ldr.w r5, [r2], #8

    mulmod r4, r5, r6, r7, r3, r8, r9, r10, r11
    reduce_32 r11, r3, r10

    str.w r11, [r0], #4

    cmp.w r0, r12
    bne.w _point_mul_pre_loop

    pop.w {r4-r12, pc}

// int32_t kred7340033(int64_t a)
// a = (a1||a0) = (r1||r0) = (r2||r1)
// c0 = r3
// c1 = r4
.global __asm_kred_modmul
__asm_kred_modmul:
	push.w {r2-r6, lr}
//	ubfx.w	r2,	r0, #0, #20
//	mov.w   r3, r0, LSR #20
//	orr.w	r3, r3, r1, LSL	#12
//	rsb.w 	r0, r2, r2, lsl #3
//	sub.w	r0, r0, r3
	kred_reduce	r0, r1, r2, r3

	pop.w {r2-r6, pc}

//void __asm_kred_poly_modmul(int32_t* ret, int64_t* data)
// r0: ptr of ret
// r1: ptr of data
.global __asm_kred_poly_reduce
__asm_kred_poly_reduce:
	push.w {r2-r8, lr}

	add.w r8, r0, #1024

_point_mul_loop:

	ldr.w r4, [r1], #4		// low 32-bit word
    ldr.w r5, [r1], #4		// high 32-bit word

	kred_reduce	r4, r5, r6, r7

	str.w r4, [r0], #4

	cmp.w r0, r8
    bne.w _point_mul_loop

	pop.w {r2-r8, pc}


//extern void __asm_kred_pointwisemul_reduce(int32_t* ret, int32_t* input1, int32_t* intput2);
// r0: ptr of ret
// r1: ptr of data1
// r2: ptr of data2
// r3: low 16-bit of data1
// r4: high 16-bit of data1
// r5: low 16-bit of data2
// r6  high 16-bit of data2
// r7: c0
// r8: c1
// r9: temp
.global __asm_kred_pointwisemul_reduce
__asm_kred_pointwisemul_reduce:
	push.w {r2-r10, lr}

	add.w r10, r0, #1024	// 256 times

_pointwisemul_loop:

	ldrh.w r3, [r1], #2		// low 16-bit of data1
    ldrsh.w r4, [r1], #2		// high 16-bit of data1
    ldrh.w r5, [r2], #2		// low 16-bit of data2
    ldrsh.w r6, [r2], #2		// high 16-bit of data2

    const_smull r7, r8, r3, r4, r5, r6, r9
	kred_reduce	r7, r8, r3, r4

	str.w r7, [r0], #4

	cmp.w r0, r10
    bne.w _pointwisemul_loop

	pop.w {r2-r10, pc}



//extern void __asm_kred_pointwisemul_acc_reduce(int32_t* ret, int32_t* input1, int32_t* intput2);
.global __asm_kred_pointwisemul_acc_reduce
__asm_kred_pointwisemul_acc_reduce:
	push.w {r2-r10, lr}

	add.w r10, r0, #1024	// 256 times

_pointwisemulacc_loop:
//.rept 256
	ldrh.w 	r3, [r1], #2		// low 16-bit of data1
    ldrsh.w r4, [r1], #2	// high 16-bit of data1
    ldrh.w 	r5, [r2], #2		// low 16-bit of data2
    ldrsh.w r6, [r2], #2	// high 16-bit of data2

    const_smull r7, r8, r3, r4, r5, r6, r9
	kred_reduce	r7, r8, r3, r4
	ldr.w	r9, [r0]
	add.w	r9, r9, r7
	str.w 	r9, [r0], #4

	cmp.w r0, r10
    bne.w _pointwisemulacc_loop
//.endr
	pop.w {r2-r10, pc}

// extern void __asm_kred_pointwisemul_acc_2x_reduce(int32_t* ret, int32_t* input1, int32_t* intput2, int32_t* intput3);
// r0: ptr of ret
// r1: ptr of data1
// r2: ptr of data2
// r3: ptr of data3 (this data is fixed)
// r4: low 16-bit of data1
// r5: high 16-bit of data1
// r6: low 16-bit of data2
// r7  high 16-bit of data2
// r8: c0
// r9: c1
// r10: temp
// r11: ctr
.global __asm_kred_pointwisemul_acc_2x_reduce
__asm_kred_pointwisemul_acc_2x_reduce:
	push.w {r2-r11, lr}

	add.w r11, r0, #1024	// 256 times

_pointwisemulacc_2x_loop:

	ldrh.w 	r4, [r3], #2	// low 16-bit of data3 (fixed data)
    ldrsh.w r5, [r3], #2	// high 16-bit of data3 (fixed data)

    ldrh.w 	r6, [r2], #2	// low 16-bit of data2
    ldrsh.w r7, [r2], #2	// high 16-bit of data2

    const_smull r8, r9, r4, r5, r6, r7, r10
    kred_reduce	r8, r9, r6, r7

    ldr.w	r10, [r0, #1024]
	add.w	r10, r10, r8
	str.w 	r10, [r0, #1024]

	ldrh.w 	r6, [r1], #2	// low 16-bit of data1
    ldrsh.w r7, [r1], #2	// high 16-bit of data1

	const_smull r8, r9, r4, r5, r6, r7, r10
	kred_reduce	r8, r9, r6, r7

	ldr.w	r10, [r0]
	add.w	r10, r10, r8
	str.w 	r10, [r0], #4

	cmp.w r0, r11
    bne.w _pointwisemulacc_2x_loop

	pop.w {r2-r11, pc}

// void __asm_freeze_32(int32_t* a, int32_t q, int32_t qhalf);
// r0: ptr of ret
// r1: prime Q
// r2: Q/2
// r3: [r0]
// r4: temp
.global __asm_freeze_32
__asm_freeze_32:
	push.w {r3-r5, lr}

	add.w r5, r0, #1024	// 256 times

__asm_freeze_32_loop:
	ldr.w	r3, [r0]

	freeze_32_asm r3, r1, r2, r4

	str.w 	r3, [r0], #4

	cmp.w r0, r5
	bne.w __asm_freeze_32_loop

	pop.w {r3-r5, pc}


// extern void __asm_kred_pointwisemul_acc_3x_reduce(int32_t* ret, int32_t* input1, int32_t* intput2, int32_t* intput3, int32_t* input4);
// r0: ptr of ret
// r1: ptr of data1
// r2: ptr of data2
// r3: ptr of data3
// r4: ptr of data4 (fixed)
// r5: low 16-bit of data1
// r6: high 16-bit of data1
// r7: low 16-bit of data2
// r8  high 16-bit of data2
// r9: c0
// r10: c1
// r11: temp
// r12: ctr
.global __asm_kred_pointwisemul_acc_3x_reduce
__asm_kred_pointwisemul_acc_3x_reduce:
	push.w {r2-r12, lr}

	add.w r12, r0, #1024	// 256 times

_pointwisemulacc_3x_loop:

	ldrh.w 	r5, [r4], #2	// low 16-bit of data4 (fixed data)
    ldrsh.w r6, [r4], #2	// high 16-bit of data4 (fixed data)

    ldrh.w 	r7, [r3], #2	// low 16-bit of data3
    ldrsh.w r8, [r3], #2	// high 16-bit of data3

    const_smull r9, r10, r5, r6, r7, r8, r11
    kred_reduce	r9, r10, r7, r8

    ldr.w	r11, [r0, #2048]
	add.w	r11, r11, r9
	str.w 	r11, [r0, #2048]

	ldrh.w 	r7, [r2], #2	// low 16-bit of data2
    ldrsh.w r8, [r2], #2	// high 16-bit of data2

    const_smull r9, r10, r5, r6, r7, r8, r11
    kred_reduce	r9, r10, r7, r8

	ldr.w	r11, [r0, #1024]
	add.w	r11, r11, r9
	str.w 	r11, [r0, #1024]

	ldrh.w 	r7, [r1], #2	// low 16-bit of data1
    ldrsh.w r8, [r1], #2	// high 16-bit of data1

	const_smull r9, r10, r5, r6, r7, r8, r11
    kred_reduce	r9, r10, r7, r8

	ldr.w	r11, [r0]
	add.w	r11, r11, r9
	str.w 	r11, [r0], #4

	cmp.w r0, r12
    bne.w _pointwisemulacc_3x_loop

	pop.w {r2-r12, pc}
