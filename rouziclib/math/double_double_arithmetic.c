// Double to quad basic operations
ddouble_t add_dd_q_quick(double a, double b)
{
	ddouble_t r;
	r.hi = a + b;
	r.lo = b - (r.hi - a);
	return r;
}

ddouble_t add_dd_q(double a, double b)
{
	ddouble_t r;
	r.hi = a + b;
	double v = r.hi - a;
	r.lo = (a - (r.hi - v)) + (b - v);
	return r;
}

ddouble_t sub_dd_q(double a, double b)
{
	ddouble_t r;
	r.hi = a - b;
	double v = r.hi - a;
	r.lo = (a - (r.hi - v)) - (b + v);
	return r;
}

ddouble_t mul_dd_q(double a, double b)
{
	ddouble_t r;
	r.hi = a * b;
	r.lo = fma(a, b, -r.hi);
	return r;
}

// Mixed quad/double operations
ddouble_t add_qd(ddouble_t a, double b)
{
	ddouble_t s = add_dd_q(a.hi, b);
	return add_dd_q_quick(s.hi, a.lo + s.lo);
}

ddouble_t sub_qd(ddouble_t a, double b)
{
	ddouble_t s = sub_dd_q(a.hi, b);
	return add_dd_q_quick(s.hi, a.lo + s.lo);
}

ddouble_t sub_dq(double a, ddouble_t b)
{
	// FIXME probably not ideal
	return add_qd(neg_q(b), a);
}

ddouble_t mul_qd(ddouble_t a, double b)
{
	ddouble_t c = mul_dd_q(a.hi, b);
	return add_dd_q_quick(c.hi, fma(a.lo, b, c.lo));
}

ddouble_t div_qd(ddouble_t a, double b)
{
	double t_hi = a.hi / b;
	ddouble_t p = mul_dd_q(t_hi, b);
	double d_hi = a.hi - p.hi;
	double d_lo = a.lo - p.lo;
	double t_lo = (d_hi + d_lo) / b;
	return add_dd_q_quick(t_hi, t_lo);
}

ddouble_t div_dq(double a, ddouble_t b)
{
	// FIXME probably not ideal
	return mul_qd(recip_q(b), a);
}

// Quad operations
ddouble_t add_qq(ddouble_t a, ddouble_t b)
{
	ddouble_t s = add_dd_q(a.hi, b.hi);
	ddouble_t t = add_dd_q(a.lo, b.lo);
	ddouble_t v = add_dd_q_quick(s.hi, s.lo + t.hi);
	ddouble_t z = add_dd_q_quick(v.hi, t.lo + v.lo);
	return z;
}

ddouble_t sub_qq(ddouble_t a, ddouble_t b)
{
	ddouble_t s = sub_dd_q(a.hi, b.hi);
	ddouble_t t = sub_dd_q(a.lo, b.lo);
	ddouble_t v = add_dd_q_quick(s.hi, s.lo + t.hi);
	ddouble_t z = add_dd_q_quick(v.hi, t.lo + v.lo);
	return z;
}

ddouble_t mul_qq(ddouble_t a, ddouble_t b)
{
	ddouble_t c = mul_dd_q(a.hi, b.hi);
	double t = a.hi * b.lo;
	t = fma(a.lo, b.hi, t);
	return add_dd_q_quick(c.hi, c.lo + t);
}

ddouble_t div_qq(ddouble_t a, ddouble_t b)
{
	double t_hi = a.hi / b.hi;
	ddouble_t r = mul_qd(b, t_hi);
	double pi_hi = a.hi - r.hi;
	double d = pi_hi + (a.lo - r.lo);
	double t_lo = d / b.hi;
	return add_dd_q_quick(t_hi, t_lo);
}

ddouble_t neg_q(ddouble_t a)
{
	ddouble_t r;
	r.hi = -a.hi;
	r.lo = -a.lo;
	return r;
}

ddouble_t recip_q(ddouble_t b)
{
	double t_hi = 1.0 / b.hi;
	ddouble_t r = mul_qd(b, t_hi);
	double pi_hi = 1.0 - r.hi;
	double d = pi_hi - r.lo;
	double t_lo = d / b.hi;
	return add_dd_q_quick(t_hi, t_lo);
}

ddouble_t mul_qd_simple(ddouble_t a, double m)	// multiplier must only change the exponent, not the mantissa
{						// like 0.125, -0.5, 1., -2., 64., ...
	a.hi *= m;
	a.lo *= m;
	return a;
}

int cmp_qq(const ddouble_t *a, const ddouble_t *b)
{
	if (a->hi > b->hi) return 1;
	if (a->hi < b->hi) return -1;
	if (a->lo == b->lo) return 0;
	if (a->lo > b->lo) return 1;
	return -1;
}

int cmp_qd(const ddouble_t a, const double b)
{
	if (a.hi > b) return 1;
	if (a.hi < b) return -1;
	if (a.lo == 0.) return 0;
	if (a.lo > 0.) return 1;
	return -1;
}

ddouble_t floor_q(ddouble_t a)
{
	ddouble_t r;

	r.hi = floor(a.hi);
	r.lo = 0.;

	// If hi is large enough that it is an integer
	if (r.hi == a.hi)
	{
		// Floor lo
		r.lo = floor(a.lo);
		return add_dd_q_quick(r.hi, r.lo);
	}

	return r;
}

ddouble_t cos_tr_q(ddouble_t x)
{
	double endsign = 1.;
	ddouble_t y;
	ddouble_t cm[] = {
		{0.4720012157682347831, -1.564015161780339309e-17},     // T_0
		{-0.4994032582704071022, 1.475862459814066814e-17},     // T_2
		{0.02799207961754761678, 7.301036844898527714e-19},     // T_4
		{-0.0005966951965488464854, -1.390299914748070235e-20}, // T_6
		{6.704394869916839946e-06, 2.035995049262044004e-22},   // T_8
		{-4.653229589731952724e-08, -1.769161868176914029e-24}, // T_10
		{2.193457658956733146e-10, 2.846388138994121544e-27},   // T_12
		{-7.481648701033646176e-13, 4.13389492313632246e-29},   // T_14
		{1.932297845863327654e-15, -7.235428663658852639e-32},  // T_16
		{-3.910170121632590404e-18, 6.942454170604680979e-35},  // T_18
		{6.367040115833800318e-21, 1.579630782651198022e-37},   // T_20
		{-8.522886041732633963e-24, 8.532437825678034454e-41},  // T_22
		{9.544663034057627948e-27, 1.151960091453906345e-43},   // T_24
		{-9.074481245220183057e-30, -4.298448379283649233e-46}, // T_26
	};

	// x = ]-inf , +inf[ --> x = [0 , 1[
	x = sub_qq(x, floor_q(x));

	// Quadrant symmetry
	// x = [0 , 1[ --> x = [0 , 0.5]
	if (cmp_qd(x, 0.5) > 0)
		x = sub_dq(1., x);

	// x = [0 , 0.5] --> x = [0 , 0.25]
	if (cmp_qd(x, 0.25) > 0)
	{
		endsign = -1.;
		x = sub_dq(0.5, x);
	}

	// x = [0 , 0.25] --> x = [0 , 1] to fit the Chebyshev unit span
	x = mul_qd_simple(x, 4.);

	// Chebyshev evaluation
	y = eval_chebyshev_polynomial_even_q(x, cm, 26);

	return mul_qd_simple(y, endsign);
}