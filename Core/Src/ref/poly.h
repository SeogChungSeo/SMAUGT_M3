#ifndef SMAUG_POLY_H
#define SMAUG_POLY_H

#include "parameters.h"
#include "toomcook.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "ntt.h"
typedef struct {
    int16_t coeffs[LWE_N];
    int32_t ntt_coeffs[LWE_N];
} poly;

typedef struct {
    poly vec[MODULE_RANK];
} polyvec;

// #define vec_vec_mult_add SMAUG_NAMESPACE(vec_vec_mult_add)
void vec_vec_mult_add(poly *r,  polyvec *a,  polyvec *b,
                      const uint8_t mod);

void vec_vec_mult_add_ntt(poly *r,  polyvec *a,  polyvec *b,
                      const uint8_t mod);


void vec_vec_mult_add_dec_ntt(poly *r,  polyvec *a,  polyvec *b,
                      const uint8_t mod);
// #define matrix_vec_mult_add SMAUG_NAMESPACE(matrix_vec_mult_add)
void matrix_vec_mult_add(polyvec *r, const polyvec a[MODULE_RANK],
                         const polyvec *b);
// #define matrix_vec_mult_sub SMAUG_NAMESPACE(matrix_vec_mult_sub)
void matrix_vec_mult_sub(polyvec *r,  polyvec a[MODULE_RANK],
                         const polyvec *b);

void matrix_vec_mult_sub_ntt(polyvec *r,  polyvec a[MODULE_RANK],
                          polyvec *b);

void matrix_vec_mult_add_ntt(polyvec *r,  polyvec a[MODULE_RANK],
                          polyvec *b);

#endif // SMAUG_POLY_H
