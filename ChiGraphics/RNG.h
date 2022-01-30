#pragma once

#include <random>

namespace CHISTUDIO {

class RNG
{
public:
	RNG(int InSeed);
	~RNG();

	/* Returns random float [0,1) */
	float Float();
private:
	int Seed;
	std::mt19937 Generator;
	std::uniform_real_distribution<float> FloatDistribution;
};

}
