#include "Graphics/Color.hpp"

namespace SHG
{
	bool operator==(const Color& c1, const Color& c2)
	{
		return c1.r == c2.r
			&& c1.g == c2.g
			&& c1.b == c2.b
			&& c1.a == c2.a;
	}
}