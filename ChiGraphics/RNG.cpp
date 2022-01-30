#include "RNG.h"
#include <iostream>

namespace CHISTUDIO {

RNG::RNG(int InSeed)
	:
	Seed(InSeed),
	Generator(std::mt19937(InSeed)),
	FloatDistribution(std::uniform_real_distribution<float>(0, 1))
{
	//std::cout << "Generating with seed: " << InSeed << std::endl;
}

RNG::~RNG()
{		
	//std::cout << "Destructing seed: " << Seed << std::endl;
}

float RNG::Float()
{
	float result = FloatDistribution(Generator);
		
	return result;
}

}