#include <iostream>
#include <chrono>

#include "ChiStudioApplication.h"

using namespace CHISTUDIO;

int main() 
{
	std::cout << "Hello ChiStudio" << std::endl;

    std::unique_ptr<ChiStudioApplication> application =
        make_unique<ChiStudioApplication>("Chi Studio", glm::ivec2(1440, 900));

    application->SetupScene();

    // Setup timer controls
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock, std::chrono::duration<double>>;
    TimePoint lastTickTime = Clock::now();
    TimePoint startTimeTime = lastTickTime;

    // Tick application
    while (!application->IsFinished()) {
        TimePoint currentTickTime = Clock::now();
        double deltaTime = (currentTickTime - lastTickTime).count();
        double totalElapsedTime = (currentTickTime - lastTickTime).count();
        application->Tick(deltaTime, totalElapsedTime);
        lastTickTime = currentTickTime;
    }
}