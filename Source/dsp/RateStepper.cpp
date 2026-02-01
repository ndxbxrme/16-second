#include "RateStepper.h"

#include <cmath>

void RateStepper::reset(double newPosition)
{
    position = newPosition;
}

void RateStepper::setRate(double newRate)
{
    rate = newRate;
}

double RateStepper::advance()
{
    position += rate;
    return position;
}

int RateStepper::getIndex(int bufferSize) const
{
    if (bufferSize <= 0)
        return 0;

    auto wrapped = std::fmod(position, static_cast<double>(bufferSize));
    if (wrapped < 0.0)
        wrapped += static_cast<double>(bufferSize);

    return static_cast<int>(wrapped);
}
