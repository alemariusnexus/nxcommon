#pragma once

#include <algorithm>



template <typename NumT>
inline NumT Clamp(NumT x, NumT min, NumT max)
{
	return std::max(std::min(x, max), min);
}
