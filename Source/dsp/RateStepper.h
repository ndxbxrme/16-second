#pragma once

class RateStepper
{
public:
    void reset(double position = 0.0);
    void setRate(double newRate);
    double getRate() const { return rate; }

    double advance();
    int getIndex(int bufferSize) const;
    double getPosition() const { return position; }

private:
    double position = 0.0;
    double rate = 1.0;
};
