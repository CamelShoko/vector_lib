/* quaternion_sse2.h  -  Vector library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform vector math library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/vector_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */


#ifndef VECTOR_HAVE_QUATERNION_CONJUGATE

quaternion_t
quaternion_conjugate(const quaternion_t q) {
	const vector_t qneg = vector_neg(q);
	const vector_t splice = _mm_shuffle_ps(qneg, q, VECTOR_MASK_ZZWW);
	return _mm_shuffle_ps(qneg, splice, VECTOR_MASK_XYXZ);
}
#define VECTOR_HAVE_QUATERNION_CONJUGATE 1

#endif


#ifndef VECTOR_HAVE_QUATERNION_INVERSE

quaternion_t
quaternion_inverse(const quaternion_t q) {
	const vector_t norm = vector_length_sqr(q);
	const vector_t inv_norm = vector_div(vector_one(), norm);
	const vector_t r = vector_mul(q, inv_norm);
	const vector_t rneg = vector_neg(r);
	const vector_t splice = _mm_shuffle_ps(rneg, r, VECTOR_MASK_ZZWW);
	return _mm_shuffle_ps(rneg, splice, VECTOR_MASK_XYXZ);
}
#define VECTOR_HAVE_QUATERNION_INVERSE 1

#endif

#ifndef VECTOR_HAVE_QUATERNION_MUL

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL quaternion_t
quaternion_mul(const quaternion_t q0, const quaternion_t q1) {
	//From http://momchil-velikov.blogspot.de/2013/10/fast-sse-quternion-multiplication.html
	const vector_t q0_wwww = vector_shuffle(q0, VECTOR_MASK_WWWW);
	const vector_t q1_yxwz = vector_shuffle(q1, VECTOR_MASK_YXWZ);
	const vector_t q0_xxxx = vector_shuffle(q0, VECTOR_MASK_XXXX);
	const vector_t q1_zwxy = vector_shuffle(q1, VECTOR_MASK_ZWXY);
	const vector_t q0_yyyy = vector_shuffle(q0, VECTOR_MASK_YYYY);
	const vector_t q1_ywxz = vector_shuffle(q1, VECTOR_MASK_YWXZ);

	/* q0.w * q1.yxwz */
	const vector_t q0w_q1yxzw = _mm_mul_ps(q0_wwww, q1_yxwz);

	/* q0.x * q1.zwxy */
	const vector_t q0x_q1zwxy = _mm_mul_ps(q0_xxxx, q1_zwxy);

	/* q0.y * q1.ywxz */
	const vector_t q0y_q1ywxz = _mm_mul_ps(q0_yyyy, q1_ywxz);

	/* q0.z * q1.yxzw */
	const vector_t q0_zzzz = vector_shuffle(q0, VECTOR_MASK_ZZZZ);
	const vector_t q1_yxzw = vector_shuffle(q1, VECTOR_MASK_YXZW);
	const vector_t q0z_q1yxzw = _mm_mul_ps(q0_zzzz, q1_yxzw);

	vector_t e = _mm_addsub_ps(q0w_q1yxzw, q0x_q1zwxy);
	e = vector_shuffle(e, VECTOR_MASK_ZXWY);

	e = _mm_addsub_ps(e, q0y_q1ywxz);
	e = vector_shuffle(e, VECTOR_MASK_WYXZ);

	e = _mm_addsub_ps(e, q0z_q1yxzw);
	return vector_shuffle(e, VECTOR_MASK_XYWZ);
}
#define VECTOR_HAVE_QUATERNION_MUL 1

#endif

#ifndef VECTOR_HAVE_QUATERNION_ROTATE

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL vector_t
quaternion_rotate(const quaternion_t q, const vector_t v) {
	const vector_t v1 = vector_cross3(q, v);
	const vector_t qw = vector_shuffle(q, VECTOR_MASK_WWWW);
	const vector_t v2 = vector_muladd(v, qw, v1);
	const vector_t v3 = vector_cross3(v2, q);
	const vector_t dot = vector_dot(q, v);
	const vector_t v4 = vector_muladd(v2, qw, vector_neg(v3));
	const vector_t r = vector_muladd(q, dot, v4);

	//store 1 in q component...

	return r;
}
#define VECTOR_HAVE_QUATERNION_ROTATE 1

#endif

#include <vector/quaternion_base.h>
