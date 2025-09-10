
#include "ntt.h"
#include "poly.h"
#include "reduce.h"
#include <string.h>

/*************************************************
 * Name:        poly_add
 *
 * Description: Add two polynomials; no modular reduction is performed
 *
 * Arguments: - poly *r: pointer to output polynomial
 *            - poly *a: pointer to first input polynomial
 *            - poly *b: pointer to second input polynomial
 **************************************************/
inline void poly_add(poly *r, const poly *a, const poly *b) {
    unsigned int i;
    for (i = 0; i < LWE_N; i++)
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/*************************************************
 * Name:        poly_sub
 *
 * Description: Subtract two polynomials; no modular reduction is performed
 *
 * Arguments: - poly *r: pointer to output polynomial
 *            - poly *a: pointer to first input polynomial
 *            - poly *b: pointer to second input polynomial
 **************************************************/
inline void poly_sub(poly *r, const poly *a, const poly *b) {
    unsigned int i;
    for (i = 0; i < LWE_N; i++)
        r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}

/*************************************************
 * Name:        vec_vec_mult
 *
 * Description: Two vector of polynomials are multiplied in the NTT domain for
 *              q0 and q1, then transform back with inverse NTT into Rq0 and
 *              Rq1, and finally combined using Chinese Remainder Theorem (CRT).
 *
 * Arguments:   - poly *r: pointer to output polynomial
 *              - polyvec *a: pointer to input vector of polynomials
 *              - polyvec *b: pointer to input vector of polynomials
 **************************************************/
void vec_vec_mult(poly *r, const polyvec *a, const polyvec *b) {
    unsigned int i;
    for (i = 0; i < MODULE_RANK; i++)
        poly_mul_acc(a->vec[i].coeffs, b->vec[i].coeffs, r->coeffs);
}

/*************************************************
 * Name:        vec_vec_mult_add
 *
 * Description: Multiply two vectors of polynomials and add the result to output
 *              polynomial
 *
 * Arguments:   - poly *r: pointer to output polynomial
 *              - polyvec *a: pointer to input vector of polynomials
 *              - polyvec *b: pointer to input vector of polynomials
 *              - uint8_t mod: modulus (16-LOG_P) or (16-LOG_Q)
 **************************************************/
void vec_vec_mult_add_ntt(poly *r,  polyvec *a,  polyvec *b,
                      const uint8_t mod) {
   unsigned int i, j;

    int32_t temp_res[LWE_N] = {0};
//    int64_t temp;
//    int32_t approx;

    i = 0;
    for (j = 0; j < LWE_N; ++j){
		a->vec[i].ntt_coeffs[j] = a->vec[i].coeffs[j] >> mod;
	}
	ntt_barrett(a->vec[i].ntt_coeffs);
//	for (j = 0; j < LWE_N; ++j){
//
//		//==============================
//		// Using KRED reduction
//		//==============================
//		a->vec[i].ntt_coeffs[j] = freeze_32(a->vec[i].ntt_coeffs[j]);
//		b->vec[i].ntt_coeffs[j] = freeze_32(b->vec[i].ntt_coeffs[j]);
//		//temp = (int64_t)a->vec[i].ntt_coeffs[j] * b->vec[i].ntt_coeffs[j];
//		//temp_res[j] += kred7340033(temp);
//	}
	__asm_freeze_32(a->vec[i].ntt_coeffs, _Q, QHalf);
	__asm_freeze_32(b->vec[i].ntt_coeffs, _Q, QHalf);
	 __asm_kred_pointwisemul_reduce(temp_res, a->vec[i].ntt_coeffs, b->vec[i].ntt_coeffs);

    for (i = 1; i < MODULE_RANK; ++i){
        for (j = 0; j < LWE_N; ++j){
            a->vec[i].ntt_coeffs[j] = a->vec[i].coeffs[j] >> mod;
        }
        ntt_barrett(a->vec[i].ntt_coeffs);
//        for (j = 0; j < LWE_N; ++j){
//
//            //==============================
//            // Using KRED reduction
//            //==============================
//            a->vec[i].ntt_coeffs[j] = freeze_32(a->vec[i].ntt_coeffs[j]);
//            b->vec[i].ntt_coeffs[j] = freeze_32(b->vec[i].ntt_coeffs[j]);
//            //temp = (int64_t)a->vec[i].ntt_coeffs[j] * b->vec[i].ntt_coeffs[j];
//            //temp_res[j] += kred7340033(temp);
//        }
        __asm_freeze_32(a->vec[i].ntt_coeffs, _Q, QHalf);
        __asm_freeze_32(b->vec[i].ntt_coeffs, _Q, QHalf);
        __asm_kred_pointwisemul_acc_reduce(temp_res, a->vec[i].ntt_coeffs, b->vec[i].ntt_coeffs);
    }

    invntt_barrett(temp_res);
    for (j = 0; j < LWE_N; ++j)
        r->coeffs[j] += (int16_t)((temp_res[j] & ((1 << LOG_Q) - 1)) << mod);

}

void vec_vec_mult_add_dec_ntt(poly *r,  polyvec *a,  polyvec *b,
                      const uint8_t mod) {
   unsigned int i, j;
    int32_t temp_res[LWE_N] = {0};

    for (i = 0; i < MODULE_RANK; i++){
        for (j = 0; j < LWE_N; ++j){
            a->vec[i].ntt_coeffs[j] = a->vec[i].coeffs[j] >> mod;
            b->vec[i].ntt_coeffs[j] = b->vec[i].coeffs[j];
        }
        ntt_barrett(a->vec[i].ntt_coeffs);
        ntt_barrett(b->vec[i].ntt_coeffs);
//        for (j = 0; j < LWE_N; ++j){
//
//            //==============================
//            // Using KRED reduction
//            //==============================
//            a->vec[i].ntt_coeffs[j] = freeze_32(a->vec[i].ntt_coeffs[j]);
//            b->vec[i].ntt_coeffs[j] = freeze_32(b->vec[i].ntt_coeffs[j]);
//            //temp = (int64_t)a->vec[i].ntt_coeffs[j] * b->vec[i].ntt_coeffs[j];
//            //temp_res[j] += kred7340033(temp);
//        }
        __asm_freeze_32(a->vec[i].ntt_coeffs, _Q, QHalf);
        __asm_freeze_32(b->vec[i].ntt_coeffs, _Q, QHalf);
        __asm_kred_pointwisemul_acc_reduce(temp_res, a->vec[i].ntt_coeffs, b->vec[i].ntt_coeffs);
    }

    invntt_barrett(temp_res);
    for (j = 0; j < LWE_N; ++j)
        r->coeffs[j] += (int16_t)((temp_res[j] & ((1 << LOG_Q) - 1)) << mod);

}
/*************************************************
 * Name:        matrix_vec_mult_add
 *
 * Description: Transpose the matrix of polynomial and multiply it with the
 *              vector of polynomials.
 *
 * Arguments:   - polyvec *r: pointer to output vector of polynomials
 *              - polyvec *a: pointer to input matrix of polynomials
 *              - polyvec *b: pointer to input vector of polynomials
 **************************************************/

int32_t reduce32(int32_t a) {
  int32_t t;

  t = (a + (1 << 22)) >> 23;
  t = a - t * _Q;
  return t;
}
void poly_reduce(int32_t *a) {
  unsigned int i;

  for(i = 0; i < LWE_N; ++i)
    a[i] = reduce32(a[i]);

}


#if 0

void matrix_vec_mult_add_ntt(polyvec *r,  polyvec a[MODULE_RANK], polyvec *b) {
	 unsigned int row, col, k;
	 const int log_q_mask = (1 << LOG_Q) - 1;
	 int32_t temp_res[MODULE_RANK][LWE_N] = { {0, }, };
	 //int64_t temp;
	 //int32_t approx;
	 //int64_t temp_better_acc_res[MODULE_RANK][LWE_N] = { {0, }, };

	 for (int h = 0; h < MODULE_RANK; h++) {
	     ntt_barrett(b->vec[h].ntt_coeffs);
	     for (k = 0; k < LWE_N; k++) {
	         b->vec[h].ntt_coeffs[k] = freeze_32(b->vec[h].ntt_coeffs[k]);
	     }
	 }

	 row = 0;
	 // bi를 고정
	 for (col = 0; col < MODULE_RANK; col++) {
		 for (k = 0; k < LWE_N; ++k) {
			 a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);

		 }
		 ntt_barrett(a[row].vec[col].ntt_coeffs);
		 for (k = 0; k < LWE_N; k++) {
			 //==============================
			 // Using KRED reduction
			 //==============================
			 a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
		 }
		 __asm_kred_pointwisemul_reduce(temp_res[col], a[row].vec[col].ntt_coeffs, b->vec[row].ntt_coeffs);
	 }

	 for (row = 1; row < MODULE_RANK; row++)
	 {
	     // bi를 고정
	     for (col = 0; col < MODULE_RANK; col++) {
	         for (k = 0; k < LWE_N; ++k) {
	             a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);

	         }
	         ntt_barrett(a[row].vec[col].ntt_coeffs);
	         for (k = 0; k < LWE_N; k++) {
	             //==============================
	             // Using KRED reduction
	             //==============================
	             a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
	             //temp = (int64_t)a[row].vec[col].ntt_coeffs[k] * b->vec[row].ntt_coeffs[k];
	             //temp_res[col][k] += kred7340033(temp);
	             //temp_better_acc_res[col][k] += temp;
	         }
	         __asm_kred_pointwisemul_acc_reduce(temp_res[col], a[row].vec[col].ntt_coeffs, b->vec[row].ntt_coeffs);
	     }
	 }
	 for (row = 0; row < MODULE_RANK; row++)
	 {
		 //__asm_kred_poly_reduce(temp_res[row], temp_better_acc_res[row]);
	     poly_reduce(temp_res[row]);
	     invntt_barrett(temp_res[row]);
	     for (k = 0; k < LWE_N; k++)
	         r->vec[row].coeffs[k] = (int16_t)((temp_res[row][k] & (log_q_mask)) << _16_LOG_Q);
	 }
}

#else

// using __asm_kred_pointwisemul_acc_2x_reduce()룰 이용하는 경우
void matrix_vec_mult_add_ntt(polyvec *r,  polyvec a[MODULE_RANK], polyvec *b) {
	 unsigned int row, col, k;
	 const int log_q_mask = (1 << LOG_Q) - 1;
	 int32_t temp_res[MODULE_RANK][LWE_N] = { {0, }, };

	 for (int h = 0; h < MODULE_RANK; h++) {
	     ntt_barrett(b->vec[h].ntt_coeffs);
//	     for (k = 0; k < LWE_N; k++) {
//	         b->vec[h].ntt_coeffs[k] = freeze_32(b->vec[h].ntt_coeffs[k]);
//	     }
	     __asm_freeze_32(b->vec[h].ntt_coeffs, _Q, QHalf);
	 }

	 for (row = 0; row < MODULE_RANK; row++)
	 {
	     // bi를 고정
	     for (col = 0; col < MODULE_RANK; col+=2)
	     {
	         for (k = 0; k < LWE_N; ++k) {
	             a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);
	             a[row].vec[col+1].ntt_coeffs[k] = (a[row].vec[col+1].coeffs[k] >> _16_LOG_Q);
	         }

	         ntt_barrett(a[row].vec[col].ntt_coeffs);
	         ntt_barrett(a[row].vec[col+1].ntt_coeffs);

//	         for (k = 0; k < LWE_N; k++) {
//	             a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
//	             a[row].vec[col+1].ntt_coeffs[k] = freeze_32(a[row].vec[col+1].ntt_coeffs[k]);
//	         }
	         __asm_freeze_32(a[row].vec[col].ntt_coeffs, _Q, QHalf);
	         __asm_freeze_32(a[row].vec[col+1].ntt_coeffs, _Q, QHalf);
	         __asm_kred_pointwisemul_acc_2x_reduce(temp_res[col], a[row].vec[col].ntt_coeffs, a[row].vec[col+1].ntt_coeffs, b->vec[row].ntt_coeffs);
	     }
	 }

	 for (row = 0; row < MODULE_RANK; row++)
	 {
		 //__asm_kred_poly_reduce(temp_res[row], temp_better_acc_res[row]);
	     poly_reduce(temp_res[row]);
	     invntt_barrett(temp_res[row]);
	     for (k = 0; k < LWE_N; k++)
	         r->vec[row].coeffs[k] = (int16_t)((temp_res[row][k] & (log_q_mask)) << _16_LOG_Q);
	 }
}

#endif

/*************************************************
 * Name:        matrix_vec_mult_sub
 *
 * Description: Multiply the matrix of polynomial with the vector of polynomial
 *              and subtract the result to output vector of polynomials.
 *
 * Arguments:   - polyvec *r: pointer to in/output vector of polynomials
 *              - polyvec *a: pointer to input matrix of polynomials
 *              - polyvec *b: pointer to input vector of polynomials
 **************************************************/
#if 0
void matrix_vec_mult_sub_ntt(polyvec *r,  polyvec a[MODULE_RANK], polyvec *b) {
	unsigned int row, col, k;
	const int log_q_mask = (1 << LOG_Q) - 1;
	int32_t temp_res[MODULE_RANK][LWE_N] = { {0, }, };
//	int64_t temp;
//	int32_t approx;
//	int64_t temp_better_acc_res[MODULE_RANK][LWE_N] = { {0, }, };

	for (int h = 0; h < MODULE_RANK; h++) {
	    ntt_barrett(b->vec[h].ntt_coeffs);
	    for (k = 0; k < LWE_N; k++) {
	        b->vec[h].ntt_coeffs[k] = freeze_32(b->vec[h].ntt_coeffs[k]);
	    }
	}

	col = 0;
	// in case of col = 0
	for (row = 0; row < MODULE_RANK; row++)
	{
		for (k = 0; k < LWE_N; k++) {
			a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);
		}

		ntt_barrett(a[row].vec[col].ntt_coeffs);

		for (k = 0; k < LWE_N; k++)
		{
			//==============================
			// Using KRED reduction
			//==============================
			a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
			//temp = (int64_t)a[row].vec[col].ntt_coeffs[k] * b->vec[col].ntt_coeffs[k];
			//temp_res[row][k] += kred7340033(temp);
		}
		__asm_kred_pointwisemul_reduce(temp_res[row], a[row].vec[col].ntt_coeffs, b->vec[col].ntt_coeffs);
	}

	for (col = 1; col < MODULE_RANK; col++)
	{
		// row = 1부터 나머지까지
	    for (row = 0; row < MODULE_RANK; row++)
	    {
	        for (k = 0; k < LWE_N; k++) {
	            a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);
	        }

	        ntt_barrett(a[row].vec[col].ntt_coeffs);

	        for (k = 0; k < LWE_N; k++)
	        {
	        	//==============================
	        	// Using Barrett approximation
	        	//==============================
//	        	a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
//	        	approx = gethi_h(b->vec[col].ntt_coeffs[k]);
//	        	temp_res[row][k] += Barrett_mul_approx_h(a[row].vec[col].ntt_coeffs[k], b->vec[col].ntt_coeffs[k], approx, _Q);
//	        	temp_res[row][k] = freeze_32(temp_res[row][k]);

	            //==============================
	            // Using KRED reduction
	            //==============================
	            a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
	            //temp = (int64_t)a[row].vec[col].ntt_coeffs[k] * b->vec[col].ntt_coeffs[k];
	            //temp_res[row][k] += kred7340033(temp);
	            //temp_better_acc_res[row][k] += temp;
	        }
	        __asm_kred_pointwisemul_acc_reduce(temp_res[row], a[row].vec[col].ntt_coeffs, b->vec[col].ntt_coeffs);
	    }
	}
	for (row = 0; row < MODULE_RANK; row++)
	{
//		for(k = 0; k < LWE_N; k++)
//		{
//			temp_res[row][k] = kred7340033(temp_better_acc_res[row][k]);
//		}
		// 20250619, 아래 asm 함수에 대해 정상동작 체크해야 함
		//__asm_kred_poly_reduce(temp_res[row], temp_better_acc_res[row]);
	    poly_reduce(temp_res[row]);
	    invntt_barrett(temp_res[row]);
	    for (k = 0; k < LWE_N; k++)
	    {
	        r->vec[row].coeffs[k] -= (int16_t)((temp_res[row][k] & (log_q_mask)) << _16_LOG_Q);
	    }
	}
}

#else

// using __asm_kred_pointwisemul_acc_2x_reduce()룰 이용하는 경우
void matrix_vec_mult_sub_ntt(polyvec *r,  polyvec a[MODULE_RANK], polyvec *b) {
	unsigned int row, col, k;
	const int log_q_mask = (1 << LOG_Q) - 1;
	int32_t temp_res[MODULE_RANK][LWE_N] = { {0, }, };

	for (int h = 0; h < MODULE_RANK; h++) {
	    ntt_barrett(b->vec[h].ntt_coeffs);
//	    for (k = 0; k < LWE_N; k++) {
//	        b->vec[h].ntt_coeffs[k] = freeze_32(b->vec[h].ntt_coeffs[k]);
//	    }
	    __asm_freeze_32(b->vec[h].ntt_coeffs, _Q, QHalf);
	}

	for (col = 0; col < MODULE_RANK; col++)
	{
	    for (row = 0; row < MODULE_RANK; row+=2)
	    {
	        for (k = 0; k < LWE_N; k++) {
	            a[row].vec[col].ntt_coeffs[k] = (a[row].vec[col].coeffs[k] >> _16_LOG_Q);
	            a[row+1].vec[col].ntt_coeffs[k] = (a[row+1].vec[col].coeffs[k] >> _16_LOG_Q);
	        }

	        ntt_barrett(a[row].vec[col].ntt_coeffs);
	        ntt_barrett(a[row+1].vec[col].ntt_coeffs);

//	        for (k = 0; k < LWE_N; k++)
//	        {
//	            a[row].vec[col].ntt_coeffs[k] = freeze_32(a[row].vec[col].ntt_coeffs[k]);
//	            a[row+1].vec[col].ntt_coeffs[k] = freeze_32(a[row+1].vec[col].ntt_coeffs[k]);
//	        }
	        __asm_freeze_32(a[row].vec[col].ntt_coeffs, _Q, QHalf);
	        __asm_freeze_32(a[row+1].vec[col].ntt_coeffs, _Q, QHalf);
	        //__asm_kred_pointwisemul_acc_reduce(temp_res[row], a[row].vec[col].ntt_coeffs, b->vec[col].ntt_coeffs);
	        __asm_kred_pointwisemul_acc_2x_reduce(temp_res[row], a[row].vec[col].ntt_coeffs, a[row+1].vec[col].ntt_coeffs, b->vec[col].ntt_coeffs);
	    }
	}
	for (row = 0; row < MODULE_RANK; row++)
	{
	    poly_reduce(temp_res[row]);
	    invntt_barrett(temp_res[row]);
	    for (k = 0; k < LWE_N; k++)
	    {
	        r->vec[row].coeffs[k] -= (int16_t)((temp_res[row][k] & (log_q_mask)) << _16_LOG_Q);
	    }
	}
}

#endif
