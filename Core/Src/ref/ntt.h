
#pragma once
#ifndef PQCLEAN_DILITHIUM3_CLEAN_NTT_H
#define PQCLEAN_DILITHIUM3_CLEAN_NTT_H
#include "parameters.h"
#include <stdint.h>

// NTT parameters (with Proth number)
#define _Q (7340033)
#define Q (7340033)
#define _Q_INV (-7340031)		// for Montgomery
#define RmodQ	(1047991)
#define RmodQhi	(613224364)
#define Qprime	(7340031)
#define QHalf (3670015)

//#define INV256	(-28672)		// INV256 for Barrett approx reduction
//#define INV256 (-4096)			// INV256*7^{-1} for K-RED reduction
#define INV256 (1047991)			// INV256*49^{01} for K-RED reduction with iNTT
#define omegaQ (8735)
#define omegainvQ (939457)
#define MONT_FINAL (-2395408)

void ntt(int32_t a[LWE_N]);

void invntt_tomont(int32_t a[LWE_N]);
int32_t montgomery_reduce(int64_t a);

void ntt_barrett(int32_t a[LWE_N]);
void invntt_barrett(int32_t a[LWE_N]);
int32_t freeze_32(int32_t a);
int32_t Barrett_mul_approx_h(int32_t a, int32_t b, int32_t bp, int32_t q);
int32_t gethi_h(int32_t a);

extern void __asm_NTT(int32_t*, int32_t*, int32_t*);
extern void __asm_iNTT(int32_t*, int32_t*, int32_t*);
extern void __asm_point_mul_pre(int32_t*, int32_t*, int32_t*, int32_t*);
//extern void __asm_pointwise_mul();

#endif
