#include "quaternion.h"

FRotator FQuaternion::Rotator() const
{
	const float singularity_test = z * x - w * y;
	const float yaw_y = 2.f * (w * z + x * y);
	const float yaw_x = (1.f - 2.f * (FMath::Square(y) + FMath::Square(z)));

	// reference 
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

	// this value was found from experience, the above websites recommend different values
	// but that isn't the case for us, so I went through different testing, and finally found the case 
	// where both of world lives happily. 

	FRotator rotator;

	if (singularity_test < -FMath::kSingularityThreshold)
	{
		rotator.pitch = -90.f;
		rotator.yaw = FMath::Atan2(yaw_y, yaw_x) * FMath::kRad2Deg;
		rotator.roll = FRotator::NormalizeAxis(-rotator.yaw - (2.f * FMath::Atan2(x, w) * FMath::kRad2Deg));
	}
	else if (singularity_test > FMath::kSingularityThreshold)
	{
		rotator.pitch = 90.f;
		rotator.yaw = FMath::Atan2(yaw_y, yaw_x) * FMath::kRad2Deg;
		rotator.roll = FRotator::NormalizeAxis(rotator.yaw - (2.f * FMath::Atan2(x, w) * FMath::kRad2Deg));
	}
	else
	{
		rotator.pitch = FMath::FastAsin(2.f*(singularity_test)) * FMath::kRad2Deg;
		rotator.yaw = FMath::Atan2(yaw_y, yaw_x) * FMath::kRad2Deg;
		rotator.roll = FMath::Atan2(-2.f*(w*x + y * z)
			, (1.f - 2.f*(FMath::Square(x) + FMath::Square(y))))
			* FMath::kRad2Deg;
	}

	return rotator;
}