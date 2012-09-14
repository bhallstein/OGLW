#include "Colour.h"

W::Colour::Colour() : r(0), g(0), b(0), a(1)
{
	
}
W::Colour::Colour(const W::Colour &col) : r(col.r), g(col.g), b(col.b), a(col.a)
{
	
}
W::Colour::Colour(float _r, float _g, float _b, float _a) :
	r(_r), g(_g), b(_b), a(_a)
{
	
}

bool W::Colour::operator==(const Colour &c) {
	return r == c.r && g == c.g && b == c.b && a == c.a;
}


namespace W {
	Colour Colour::Black(0, 0, 0);
	Colour Colour::TransparentBlack(0, 0, 0, 0.5);
	Colour Colour::White(1, 1, 1);
	Colour Colour::Red(1, 0, 0);
	Colour Colour::Green(0, 1, 0);
	Colour Colour::Blue(0, 0, 1);
	Colour Colour::Yellow(1, 1, 0);
	Colour Colour::Purple(0.47, 0, 0.78);
}
