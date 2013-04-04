/* quaternion_base.h  -  Vector library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#ifndef VECTOR_HAVE_QUATERNION_ZERO

quaternion_t quaternion_zero( void )
{
	return vector_zero();
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_UNALIGNED

quaternion_t quaternion_unaligned( const float32_t* RESTRICT q )
{
	return vector_unaligned( q );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_ALIGNED

quaternion_t quaternion_aligned( const float32_aligned128_t* RESTRICT q )
{
	return vector_aligned( q );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_IDENTITY

quaternion_t quaternion_identity( void )
{
	static const ALIGN(16) float32_t _identity_quat[4] = { 0, 0, 0, 1 };
	return quaternion_aligned( _identity_quat );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_CONJUGATE

quaternion_t quaternion_conjugate( const quaternion_t q )
{
	return vector( -q.x, -q.y, -q.z, q.w );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_INVERSE

quaternion_t quaternion_inverse( const quaternion_t q )
{
	quaternion_t r = q;
	const float32_t norm = vector_length_sqr( q ).x;
	const float32_t inv_norm = 1.0f / norm;
	r.x *= -inv_norm;
	r.y *= -inv_norm;
	r.z *= -inv_norm;
	r.w *=  inv_norm;
	return r;
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_NEG

quaternion_t quaternion_neg( const quaternion_t q )
{
	return vector_neg( q );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_NORMALIZE

quaternion_t quaternion_normalize( const quaternion_t q )
{
	return vector_normalize( q );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_MUL

quaternion_t quaternion_mul( const quaternion_t q0, const quaternion_t q1 )
{
	return vector(
	  q1.w * q0.x + q1.x * q0.w + q1.y * q0.z - q1.z * q0.y,
	  q1.w * q0.y - q1.x * q0.z + q1.y * q0.w + q1.z * q0.x,
	  q1.w * q0.z + q1.x * q0.y - q1.y * q0.x + q1.z * q0.w,
	  q1.w * q0.w - q1.x * q0.x - q1.y * q0.y - q1.z * q0.z );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_ADD

quaternion_t quaternion_add( const quaternion_t q0, const quaternion_t q1 )
{
	return vector_add( q0, q1 );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_SUB

quaternion_t quaternion_sub( const quaternion_t q0, const quaternion_t q1 )
{
	return vector_sub( q0, q1 );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_SLERP

quaternion_t quaternion_slerp( const quaternion_t q0, const quaternion_t q1, real factor )
{
	quaternion_t qd;
	float32_t cosval = vector_dot( q0, q1 ).x;

	//if cosval < 0 use slerp to negated target to get acute angle
	//between quaternions and avoid extra spins
	if( cosval < 0 )
	{
		qd = quaternion_neg( q1 );
		cosval = vector_dot( q0, qd ).x;
	}
	else
	{
		qd = q1;
	}

	real angle = 0.0f;
	if( -1.0f < cosval )
	{
		if( cosval < 1.0 )
			angle = math_acos( cosval );
		else
			return qd;
	}
	else
		angle = REAL_PI;

	if( math_realzero( angle ) )
		return qd;

	real sinval = math_sin( angle );
	real invsin = 1.0f / sinval;
	real c1     = math_sin( ( 1.0f - factor ) * angle ) * invsin;
	real c2     = math_sin( factor * angle ) * invsin;

	return vector_add( vector_scale( q0, c1 ), vector_scale( qd, c2 ) );
}

#endif

#ifndef VECTOR_HAVE_QUATERNION_ROTATE

vector_t quaternion_rotate( const quaternion_t q, const vector_t v )
{
	//Quaternion "q" rotation of vector "w" is calculated by constructing
	//a quaternion "v" with values (0,w) and the formula (where q' is the conjugate of q)
	// q * v * q'
	//
	// Breaking this down and calculating q * v =
	//    qs*0 - qv . w = -qv . w   (scalar part)
	//    qs*w + qv x w             (vector part)
	//
	// Final calculation of q * v * q'
	//  (scalar part)  -(qv . w)*qs + (qs*w + qv x w) . qv   = -(qv.w)*qs + (qv.w)*qs + ((qv x w) . qv)    = 0
	//  (vector part)  -qv*(-qv . w) + (qs*w + qv x w)*qs - (qs*w + qv x w) x qv
	//
	// Precalculating the vector v1 = (qs*w + qv x w) yields the final formula
	//
	//  qv * ( qv . w ) + v1 * qs - v1 % qv

	vector_t v1 = vector_cross3( q, v );

	v1.x += v.x * q.w;
	v1.y += v.y * q.w;
	v1.z += v.z * q.w;

	vector_t v2 = vector_cross3( v1, q );
	float32_t dot = ( q.x * v.x + q.y * v.y + q.z * v.z );
	
	vector_t r = {
		q.x * dot + v1.x * q.w - v2.x,
		q.y * dot + v1.y * q.w - v2.y,
		q.z * dot + v1.z * q.w - v2.z,
		1
	};

	return r;
}

#endif


#undef VECTOR_HAVE_QUATERNION_ZERO
#undef VECTOR_HAVE_QUATERNION_IDENTITY
#undef VECTOR_HAVE_QUATERNION_UNALIGNED
#undef VECTOR_HAVE_QUATERNION_ALIGNED
#undef VECTOR_HAVE_QUATERNION_CONJUGATE
#undef VECTOR_HAVE_QUATERNION_INVERSE
#undef VECTOR_HAVE_QUATERNION_NEG
#undef VECTOR_HAVE_QUATERNION_NORMALIZE
#undef VECTOR_HAVE_QUATERNION_MUL
#undef VECTOR_HAVE_QUATERNION_ADD
#undef VECTOR_HAVE_QUATERNION_SUB
#undef VECTOR_HAVE_QUATERNION_SLERP
#undef VECTOR_HAVE_QUATERNION_ROTATE
