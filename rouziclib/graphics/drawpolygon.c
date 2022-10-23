float erf_right_triangle_acute_integral(float x, float y)
{
	return 0.1f;
	float x2 = x*x, y2 = y*y;
	float v = (((((-1.6663128e-05f*y2 + 5.104393e-06f)*x2 +
			0.0005496131f*y2 - 5.30433e-05f)*x2 +
			(0.0001584783f*y2 - 0.00741157237f)*y2 - 0.0018265954f)*x2 +
			(-0.003881739f*y2 + 0.0523013844f)*y2 + 0.04582956f)*x2 +
			((-0.00368418f*y2 + 0.03692744f)*y2 - 0.1996548f)*y2 - 0.50360028f)*x2 +
			((-0.0012717f*y2 - 0.0101518f)*y2 + 0.0109084f)*y2 - 1.836892f;
	return expf(v) * x2 * y;
}

float fast_atan2pi_unitrange(float x)
{
	float x2 = x*x;
	return (((-0.00609f*x2 + 0.023075f)*x2 - 0.051014f)*x2 + 0.15901703f)*x;
}

float erf_fastf(float x)
{
	float y, xa;

	// Prepare x
	xa = fabsf(x);
	xa = MINN(xa, 4.45f);

	y = ((((-0.001133515f*xa + 0.00830125f)*xa - 0.003218f)*xa - 0.06928657f)*xa - 0.14108256f)*xa + 1.f;

	// (1 - poly(min(abs(x), 4.45))^c1) * sign(x)
	y *= y;
	y *= y;
	y *= y;
	y = 1.f - y;
	y = sign_multiplyf(y, x);

	return y;
}

float calc_right_triangle_pixel_weight(xyf_t rp)
{
	xyf_t rpa;
	int use_obtuse;
	float slope, acute, obtuse;

	rpa = abs_xyf(rp);
	use_obtuse = rpa.y > rpa.x;

	// If the edge is beyond 3 units, use the atan approximation
	if (rpa.x >= 3.f)
	{
		if (use_obtuse)
			return sign_multiplyf(0.25f - fast_atan2pi_unitrange(rpa.x/rpa.y), xor_float(rp.x, rp.y));
		else
			return fast_atan2pi_unitrange(rp.y/rp.x);
		//return fast_atan2pi_unitrange(rpa.y * sse_rcpf_newton1(rpa.x));
	}

	// Pick method
	if (use_obtuse)				// if we use the obtuse method
		swap_float(&rp.x, &rp.y);	// swap axes

	// Calculate slope
	slope = fabsf(rp.x) < 1e-5f ? 0.f : rp.y / rp.x;
	rp.x = rangelimitf(rp.x, -3.f, 3.f);

	acute = erf_right_triangle_acute_integral(rp.x, slope);

	if (use_obtuse)
	{
		obtuse = 0.25f * erf_fastf(rp.y) * erf_fastf(rp.x) - acute;
		obtuse = copysignf(obtuse, slope);
		return obtuse;
	}

	acute = copysignf(acute, slope);

	return acute;
}

xyf_t fast_normalise(xyf_t v)
{
	return mul_xyf(v, set_xyf(sse_rsqrtf_newton1(v.x*v.x+v.y*v.y)));
}

float calc_subtriangle_pixel_weight(xyf_t p0, xyf_t p1)
{
	xyf_t rot, r0, r1;
	float weight;

	// Rotate points
	rot = fast_normalise(sub_xyf(p1, p0));
	r0.x = rot.x*p0.y - rot.y*p0.x;
	r0.y = rot.x*p0.x + rot.y*p0.y;
	r1.x = r0.x;
	r1.y = rot.x*p1.x + rot.y*p1.y;

	// Calc weights
	weight = calc_right_triangle_pixel_weight(r1);
	weight -= calc_right_triangle_pixel_weight(r0);

	return weight;
}

void draw_polygon_lrgb(xy_t *p, int p_count, double radius, lrgb_t colour, double intensity)
{
	int i;
	float rad, weight;
	xyi_t ip;
	xy_t pf;
	xyf_t p0, p1;
	static _Thread_local xy_t *pl=NULL;
	static _Thread_local size_t pl_as=0;

	// Check polygon is on screen
	recti_t bbi;
	double grad = 3. * radius;

	if (get_bounding_box_for_polygon(p, p_count, set_xy(grad), &bbi) == 0)
		return;

	// Prepare parameters
	colour = mul_scalar_lrgb(colour, intensity*ONEF+0.5);
	rad = 1./radius;

	// Scale polygon points
	alloc_enough(&pl, p_count, &pl_as, sizeof(xy_t), 1.0);

	for (i=0; i < p_count; i++)
		pl[i] = mul_xy(p[i], set_xy(rad));

	// Go through all pixels
	for (ip.y=bbi.p0.y; ip.y < bbi.p1.y; ip.y++)
	{
		pf.y = ip.y * rad;
		for (ip.x=bbi.p0.x; ip.x < bbi.p1.x; ip.x++)
		{
			pf.x = ip.x * rad;

			// Calculate weight for each subtriangle
			weight = 0.f;
			p0 = xy_to_xyf(sub_xy(pl[p_count-1], pf));
			for (i=0; i < p_count; i++)
			{
				p1 = xy_to_xyf(sub_xy(pl[i], pf));
				weight += calc_subtriangle_pixel_weight(p0, p1);
				p0 = p1;
			}

			weight = MAXN(0.f, -weight);

			// Apply weight to colour
			blend_add(&fb->r.l[ip.y*fb->w+ip.x], colour, 32768.f*weight);
		}
	}
}

void draw_polygon_dq(xy_t *p, int p_count, double radius, frgb_t colour, double intensity)
{
	int i;
	float *df;
	double grad;
	xyi_t ip;
	recti_t bbi;

	if (p_count < 3 || p_count > 4)
		return;

	grad = 3. * radius;

	if (drawq_get_bounding_box_for_polygon(p, p_count, set_xy(grad), &bbi) == 0)
		return;

	// Store the drawing parameters in the main drawing queue
	df = drawq_add_to_main_queue(p_count==3 ? DQT_TRIANGLE : DQT_TETRAGON);
	df[0] = 1./radius;
	df[1] = colour.r * intensity;
	df[2] = colour.g * intensity;
	df[3] = colour.b * intensity;
	for (i=0; i < p_count; i++)
	{
		df[4+i*2] = p[i].x;
		df[5+i*2] = p[i].y;
	}

	// Find the affected sectors
	for (ip.y=bbi.p0.y; ip.y<=bbi.p1.y; ip.y++)
		for (ip.x=bbi.p0.x; ip.x<=bbi.p1.x; ip.x++)
			drawq_add_sector_id(ip.y*fb->sector_w + ip.x);	// add sector reference
}

int get_bounding_box_for_polygon(xy_t *p, int p_count, xy_t rad, recti_t *bbi)
{
	int i;
	rect_t bb, fb_box = rect(XY0, xy(fb->w-1, fb->h-1));

	// Calculate the bounding box
	bb.p0 = ceil_xy(sub_xy(p[0], rad));
	bb.p1 = floor_xy(add_xy(p[0], rad));
	for (i=1; i < p_count; i++)
	{
		bb.p0 = min_xy(bb.p0, ceil_xy(sub_xy(p[i], rad)));
		bb.p1 = max_xy(bb.p1, floor_xy(add_xy(p[i], rad)));
	}

	// Check against framebuffer boundaries
	if (check_box_box_intersection(bb, fb_box)==0)
		return 0;

	// Convert to sector coordinates
	bbi->p0 = xy_to_xyi(max_xy(bb.p0, fb_box.p0));
	bbi->p1 = xy_to_xyi(min_xy(bb.p1, fb_box.p1));

	return 1;
}

void draw_polygon(xy_t *p, int p_count, double radius, col_t colour, double intensity)
{
	if (fb->discard)
		return;

	radius = drawing_focus_adjust(focus_rlg, radius, NULL, 0);	// adjusts the focus

	if (fb->use_drawq)
		draw_polygon_dq(p, p_count, radius, col_to_frgb(colour), intensity);
	else if (fb->r.use_frgb == 0)
		draw_polygon_lrgb(p, p_count, radius, col_to_lrgb(colour), intensity);
}

void draw_polygon_wc(xy_t *p, int p_count, double radius, col_t colour, double intensity)
{
	int i;
	xy_t p_sc[16];

	// Convert to screen coordinates in reverse order
	for (i=0; i < p_count; i++)
		p_sc[p_count-1-i] = sc_xy(p[i]);

	draw_polygon(p_sc, p_count, radius, colour, intensity);
}

void draw_triangle(triangle_t tri, double radius, col_t colour, double intensity)
{
	draw_polygon((xy_t *) &tri, 3, radius, colour, intensity);
}

void draw_triangle_wc(triangle_t tri, double radius, col_t colour, double intensity)
{
	draw_polygon_wc((xy_t *) &tri, 3, radius, colour, intensity);
}