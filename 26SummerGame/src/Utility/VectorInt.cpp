#include "VectorInt.h"
#include "vector.h"

Vector VectorInt::ToVector() const
{
	return Vector{ static_cast<float>(m_x),static_cast<float>(m_y),static_cast<float>(m_z) };
}
