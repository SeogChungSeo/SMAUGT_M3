#pragma once

#include <stdint.h>

int32_t kred7340033(int64_t a);
extern int32_t __asm_kred_modmul(int64_t a);

extern void __asm_kred_poly_reduce(int32_t* ret, int64_t* data);

// ret = input1 * input2
extern void __asm_kred_pointwisemul_reduce(int32_t* ret, int32_t* input1, int32_t* intput2);

// ret += input1 * input2
extern void __asm_kred_pointwisemul_acc_reduce(int32_t* ret, int32_t* input1, int32_t* intput2);

// ret[i] = input1 * input3
// ret[i+1] = input2 * input3
extern void __asm_kred_pointwisemul_acc_2x_reduce(int32_t* ret, int32_t* input1, int32_t* intput2, int32_t* intput3);

// ret[i] = input1 * input4
// ret[i+1] = input2 * input4
// ret[i+2] = input3 * input4
extern void __asm_kred_pointwisemul_acc_3x_reduce(int32_t* ret, int32_t* input1, int32_t* intput2, int32_t* intput3, int32_t* input4);

extern void __asm_freeze_32(int32_t* a, int32_t q, int32_t qhalf);


