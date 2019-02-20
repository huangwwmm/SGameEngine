#include "rotator.h"

const FRotator FRotator::kZero(0.0f, 0.0f, 0.0f);

FRotator::FRotator(const FQuaternion &quaternion)
{
	*this = quaternion.Rotator();
}