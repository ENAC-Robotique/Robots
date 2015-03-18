/*
 * mt_mat.c
 *
 *  Created on: 15 févr. 2015
 *      Author: ludo6431
 */

#ifdef ARCH_X86_LINUX
#define PERFORM_OVERFLOW_TESTS
#endif

#include <stdint.h>
#include <stdlib.h>
#ifdef PERFORM_OVERFLOW_TESTS
#include <assert.h>
#endif

#include "mt_mat.h"

#define MRC(m, r, c) (m)->me[(r)*(m)->cols + (c)]
#define M64(m, r, c) (int64_t)(MRC(m, r, c))

#ifdef PERFORM_OVERFLOW_TESTS

#ifndef ABS
#define ABS(v) ((v)<0ll ? -(v) : (v))
#endif
// 2-steps shift, 1 doesn't work here (x86)
#define MASK_32S(s) ((~((1ull<<(s))-1))<<32)
/**
 * Tests if you can right shit v by s and cast the value to 32bit without loss of most significant bits
 *  s-1 because the sign bit must be 0
 */
#define RSHIFT_C32_OK(v, s) ((ABS(v) & MASK_32S(s-1)) == 0)

#define SIGNBIT32(v) ((v) & 0x80000000u)
#define SUM32_OK(v1, v2, sum) ((SIGNBIT32(v1) || SIGNBIT32(v2) || !SIGNBIT32(sum)) && (!SIGNBIT32(v1) || !SIGNBIT32(v2) || SIGNBIT32(sum)))

#define CHECKED_SUM32(v1, v2, sum) do{  \
    register int32_t t1 = (v1);         \
    register int32_t t2 = (v2);         \
    register int32_t s = t1 + t2;       \
                                        \
    assert(SUM32_OK(t1, t2, s));        \
                                        \
    (sum) = s;                          \
}while(0)

#define SIGNBIT64(v) ((v) & 0x8000000000000000ull)
#define SUM64_OK(v1, v2, sum) ((SIGNBIT64(v1) || SIGNBIT64(v2) || !SIGNBIT64(sum)) && (!SIGNBIT64(v1) || !SIGNBIT64(v2) || SIGNBIT64(sum)))
#define SUB64_OK(v1, v2, sum) ((SIGNBIT64(v1) || !SIGNBIT64(v2) || !SIGNBIT64(sum)) && (!SIGNBIT64(v1) || SIGNBIT64(v2) || SIGNBIT64(sum)))

#define CHECKED_SUM64(v1, v2, sum) do{  \
    register int64_t t1 = (v1);         \
    register int64_t t2 = (v2);         \
    register int64_t s = t1 + t2;       \
                                        \
    assert(SUM64_OK(t1, t2, s));        \
                                        \
    (sum) = s;                          \
}while(0)
#define CHECKED_SUB64(v1, v2, sub) do{  \
    register int64_t t1 = (v1);         \
    register int64_t t2 = (v2);         \
    register int64_t s = t1 - t2;       \
                                        \
    assert(SUB64_OK(t1, t2, s));        \
                                        \
    (sub) = s;                          \
}while(0)

#else

#define CHECKED_SUM32(v1, v2, sum) (sum) = (v1) + (v2)
#define CHECKED_SUM64(v1, v2, sum) (sum) = (v1) + (v2)
#define CHECKED_SUB64(v1, v2, sub) (sub) = (v1) - (v2)

#endif

/**
 * Initializes a new matrix of size rows x cols
 * Assumes the input matrix is garbage,
 * does not frees any already allocated memory
 */
void mt_m_init(MT_MAT* m, uint8_t rows, uint8_t cols, uint8_t shift) {
    m->rows = rows;
    m->cols = cols;
    m->shift = shift;
    m->stack = 0;
    if (rows > 0 && cols > 0) {
        m->me = (int32_t*) calloc(rows * cols, sizeof(*m->me));
    }
    else {
        m->me = NULL;
    }

#ifdef PERFORM_OVERFLOW_TESTS
    int64_t t = -1;
    assert(SIGNBIT64(t));
    t = 1;
    assert(!SIGNBIT64(t));
    assert(SUM64_OK(1, 1, 2));
    assert(SUM64_OK(1, -1, 2));
    assert(!SUM64_OK(1, 1, -2));
    assert(SUB64_OK(1, 1, 2));
    assert(!SUB64_OK(1, -1, -2));
    assert(!SUB64_OK(-1, 1, 2));
#endif
}

/**
 * Matrix-Vector multiplication
 * returns  0 in case of success
 *         -1 if bad input
 *         -2 if bad output
 */
int mt_mv_mlt(const MT_MAT* M, const MT_VEC* v, MT_VEC* out) {
    register int i, j;
    register int64_t sum;

    if (M->cols != v->elts) {
        return -1;
    }

    if (out->elts != M->rows) {
        return -2;
    }

    out->shift = v->shift;

    for (i = 0; i < M->rows; i++) {
        sum = 0;
        for (j = 0; j < M->cols; j++) {
            // sum += M64(M, i, j) * (int64_t) v->ve[j];
            CHECKED_SUM64(sum, M64(M, i, j) * (int64_t) v->ve[j], sum);
        }

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(sum, M->shift));
#endif

        out->ve[i] = sum >> M->shift;
    }

    return 0;
}

/**
 * Matrix-Vector multiplication + addition
 * Performs out = v1 + k*M*v2
 *   k integer must be specified with the same shift as the vectors
 * returns  0 in case of success
 *         -1 if bad input
 *         -2 if bad output
 */
int mt_mv_mltadd(const MT_VEC* v1, int32_t k, const MT_MAT* M, const MT_VEC* v2, MT_VEC* out) {
    register int i, j;
    register int64_t sum, tmp;

    if (M->cols != v2->elts || M->rows != v1->elts || v1->shift != v2->shift) {
        return -1;
    }

    if (out->elts != M->rows) {
        return -2;
    }

    out->shift = v1->shift;

    for (i = 0; i < M->rows; i++) {
        sum = 0;
        for (j = 0; j < M->cols; j++) {
            // sum += M64(M, i, j) * (int64_t) v2->ve[j];
            CHECKED_SUM64(sum, M64(M, i, j) * (int64_t) v2->ve[j], sum);
        }

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(sum, M->shift));
#endif

        tmp = (int64_t) k * (sum >> M->shift);

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(tmp, v1->shift));
#endif

        // out->ve[i] = v1->ve[i] + (int32_t) (tmp >> v1->shift);
        CHECKED_SUM32(v1->ve[i], (int32_t) (tmp >> v1->shift), out->ve[i]);
    }

    return 0;
}

/**
 * Matrix-Matrix multiplication
 * returns  0 in case of success
 *         -1 if bad input
 *         -2 if bad output
 */
int mt_mm_mlt(const MT_MAT* A, const MT_MAT* B, MT_MAT* OUT) {
    register int i, j, k;
    register int64_t sum;

    if (A->cols != B->rows || A->shift != B->shift) {
        return -1;
    }

    if (OUT->rows != A->rows || OUT->cols != B->cols) {
        return -2;
    }

    OUT->shift = A->shift;

    for (i = 0; i < OUT->rows; i++) {
        for (j = 0; j < OUT->cols; j++) {
            sum = 0;
            for (k = 0; k < A->cols; k++) {
                // sum += M64(A, i, k) * M64(B, k, j);
                CHECKED_SUM64(sum, M64(A, i, k) * M64(B, k, j), sum);
            }

#ifdef PERFORM_OVERFLOW_TESTS
            assert(RSHIFT_C32_OK(sum, A->shift));
#endif

            MRC(OUT, i, j)= sum >> A->shift;
        }
    }

    return 0;
}

/**
 * Matrix inversion
 * returns  0 in case of success
 *         -1 if bad input
 *         -2 if bad output
 *         -3 if unsupported operation
 */
int mt_m_inv(const MT_MAT* M, MT_MAT* OUT) {
    register int32_t det;
    register int64_t tmp, tmp2, tmp3, tmp4;

    if (M->rows == 1 || M->rows != M->cols) {
        return -1;
    }

    if (M->rows > 3) {
        return -3;
    }

    if (OUT->rows != M->rows || OUT->cols != M->cols) {
        return -2;
    }

    OUT->shift = M->shift;

    if (M->rows == 2) {
        // tmp = M64(M, 0, 0) * M64(M, 1, 1) - M64(M, 0, 1) * M64(M, 1, 0);
        CHECKED_SUB64(M64(M, 0, 0) * M64(M, 1, 1), M64(M, 0, 1) * M64(M, 1, 0), tmp);

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(tmp, M->shift));
#endif

        det = (int32_t) (tmp >> M->shift);

        MRC(OUT, 0, 0)= (M64(M, 1, 1)<<M->shift)/det;
        MRC(OUT, 0, 1)= -(M64(M, 0, 1)<<M->shift)/det;
        MRC(OUT, 1, 0)= -(M64(M, 1, 0)<<M->shift)/det;
        MRC(OUT, 1, 1)= (M64(M, 0, 0)<<M->shift)/det;
    }
    else if (M->rows == 3) {
        // tmp2 = M64(M, 1, 1) * M64(M, 2, 2) - M64(M, 2, 1) * M64(M, 1, 2);
        CHECKED_SUB64(M64(M, 1, 1) * M64(M, 2, 2), M64(M, 2, 1) * M64(M, 1, 2), tmp2);
        // tmp3 = M64(M, 1, 0) * M64(M, 2, 2) - M64(M, 1, 2) * M64(M, 2, 0);
        CHECKED_SUB64(M64(M, 1, 0) * M64(M, 2, 2), M64(M, 1, 2) * M64(M, 2, 0), tmp3);
        // tmp4 = M64(M, 1, 0) * M64(M, 2, 1) - M64(M, 1, 1) * M64(M, 2, 0);
        CHECKED_SUB64(M64(M, 1, 0) * M64(M, 2, 1), M64(M, 1, 1) * M64(M, 2, 0), tmp4);

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(tmp2, M->shift));
        assert(RSHIFT_C32_OK(tmp3, M->shift));
        assert(RSHIFT_C32_OK(tmp4, M->shift));
#endif

        // tmp = M64(M, 0, 0) * (tmp2 >> M->shift) - M64(M, 0, 1) * (tmp3 >> M->shift);
        CHECKED_SUB64(M64(M, 0, 0) * (tmp2 >> M->shift), M64(M, 0, 1) * (tmp3 >> M->shift), tmp);
        // tmp += M64(M, 0, 2) * (tmp4 >> M->shift);
        CHECKED_SUM64(tmp, M64(M, 0, 2) * (tmp4 >> M->shift), tmp);

#ifdef PERFORM_OVERFLOW_TESTS
        assert(RSHIFT_C32_OK(tmp, M->shift));
#endif

        det = (int32_t) (tmp >> M->shift);

        CHECKED_SUB64(M64(M, 1, 1) * M64(M, 2, 2), M64(M, 2, 1) * M64(M, 1, 2), tmp);
        MRC(OUT, 0, 0)= tmp / det;

        CHECKED_SUB64(M64(M, 0, 2) * M64(M, 2, 1), M64(M, 0, 1) * M64(M, 2, 2), tmp);
        MRC(OUT, 0, 1)= tmp / det;

        CHECKED_SUB64(M64(M, 0, 1) * M64(M, 1, 2), M64(M, 0, 2) * M64(M, 1, 1), tmp);
        MRC(OUT, 0, 2)= tmp / det;

        CHECKED_SUB64(M64(M, 1, 2) * M64(M, 2, 0), M64(M, 1, 0) * M64(M, 2, 2), tmp);
        MRC(OUT, 1, 0)= tmp / det;

        CHECKED_SUB64(M64(M, 0, 0) * M64(M, 2, 2), M64(M, 0, 2) * M64(M, 2, 0), tmp);
        MRC(OUT, 1, 1)= tmp / det;

        CHECKED_SUB64(M64(M, 1, 0) * M64(M, 0, 2), M64(M, 0, 0) * M64(M, 1, 2), tmp);
        MRC(OUT, 1, 2)= tmp / det;

        CHECKED_SUB64(M64(M, 1, 0) * M64(M, 2, 1), M64(M, 2, 0) * M64(M, 1, 1), tmp);
        MRC(OUT, 2, 0)= tmp / det;

        CHECKED_SUB64(M64(M, 2, 0) * M64(M, 0, 1), M64(M, 0, 0) * M64(M, 2, 1), tmp);
        MRC(OUT, 2, 1)= tmp / det;

        CHECKED_SUB64(M64(M, 0, 0) * M64(M, 1, 1), M64(M, 1, 0) * M64(M, 0, 1), tmp);
        MRC(OUT, 2, 2)= tmp / det;
    }

    return 0;
}

void mt_m_free(MT_MAT* m) {
    if (m->me && !m->stack) {
        free(m->me);
    }
}
