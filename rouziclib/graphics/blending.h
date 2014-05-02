enum
{
	SOLID,
	ADD,
	SUB,
	MUL,
	BLEND,
	ALPHABLEND,
	ALPHABLENDFG,
	BLENDALPHAONLY,
};

extern lrgb_t blend_pixels(lrgb_t bg, lrgb_t fg, int32_t p, const int mode);
