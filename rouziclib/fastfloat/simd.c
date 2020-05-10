#ifndef RL_EXCL_APPROX
#ifdef RL_INTEL_INTR

__m128i _mm_index_from_vom_ps(__m128 x, const float offset, const uint32_t mask)	// make index from value+offset&mask
{
	__m128i r;
	r = _mm_castps_si128(_mm_and_ps(_mm_add_ps(x, _mm_set_ps1(offset)), _mm_castsi128_ps(_mm_set1_epi32(mask))));
	return r;
}

#ifdef __GNUC__
__attribute__((__target__("avx2,fma")))
#endif
__m128 _mm_eval_poly_d1_lut_ps(__m128 x, const float *lut, __m128i index)
{
	__m128 r, c0, c1;

	// Look up coefs in LUT
	c0 = _mm_i32gather_ps(lut, index, 4);			// lookup c0
	index = _mm_add_epi32(index, _mm_set1_epi32(1));	// index+1
	c1 = _mm_i32gather_ps(lut, index, 4);			// lookup c1

	// Polynomial
	r = _mm_fmadd_ps(c1, x, c0);				// x*c1 + c0
	return r;
}

#ifdef __GNUC__
__attribute__((__target__("avx2,fma")))
#endif
__m128 _mm_eval_poly_d2_lut_ps(__m128 x, const float *lut, __m128i index)
{
	__m128 r, c0, c1, c2;

	// Look up coefs in LUT
	c0 = _mm_i32gather_ps(lut, index, 4);			// lookup c0
	index = _mm_add_epi32(index, _mm_set1_epi32(1));	// index+1
	c1 = _mm_i32gather_ps(lut, index, 4);			// lookup c1
	index = _mm_add_epi32(index, _mm_set1_epi32(1));	// index+1
	c2 = _mm_i32gather_ps(lut, index, 4);			// lookup c2

	// Polynomial
	r = _mm_fmadd_ps(c2, x, c1);
	r = _mm_fmadd_ps(r, x, c0);
	return r;
}

__m128 _mm_gaussian_d1_ps(__m128 x)
{
	#include "fastgauss_d1.h"	// contains the LUT, offset and limit
	__m128i index;

	x = _mm_abs_ps(x);						// x = |x|

	//if (_mm_movemask_ps(_mm_cmplt_ps(x, _mm_set_ps1(limit))) == 0)	// if all |x| >= limit
	//	return _mm_setzero_ps();

	x = _mm_min_ps(x, _mm_set_ps1(limit));				// x > 4 becomes 4

	index = _mm_index_from_vom_ps(x, offset, 0x007FFFFE);
	return _mm_eval_poly_d1_lut_ps(x, fastgauss_lut, index);
}

__m128 _mm_erfr_d1_ps(__m128 x)
{
	#include "fasterfr_d1.h"	// contains the LUT, offset and limit
	__m128i index;
	__m128 c0, c1;

	x = _mm_max_ps(x, _mm_set_ps1(-limit));				// x < -4 becomes -4
	x = _mm_min_ps(x, _mm_set_ps1(limit));				// x > 4 becomes 4

	index = _mm_index_from_vom_ps(x, offset, 0x007FFFFE);
	return _mm_eval_poly_d1_lut_ps(x, fasterfr_lut, index);
}

#ifdef __GNUC__
__attribute__((__target__("sse4.1")))
#endif
__m128 _mm_frgb_to_srgb(__m128 x)	// output is [0.f , 1.f]
{
	static const float lut[] = 
	#include "fastlsrgb.h"		// 34 x 3 floats, 408 bytes, contains order (2), ish
	__m128 r;
	__m128i index, offset = _mm_set1_epi32(0x3B4D2E1C);	// 0.0031308f

	// Clamp x
	x = _mm_max_ps(x, _mm_setzero_ps());
	x = _mm_min_ps(x, _mm_set_ps1(1.f));

	// Make index
	index = _mm_sub_epi32(_mm_castps_si128(_mm_add_ps(x, _mm_set_ps1(0.0031308f))), offset);
	index = _mm_mullo_epi32(_mm_srli_epi32(index, ish), _mm_set1_epi32(3));

	r = _mm_eval_poly_d2_lut_ps(x, lut, index);
	//r = _mm_mul_ps(r, _mm_set_ps1(255.f));
	return r;
}

#endif
#endif
