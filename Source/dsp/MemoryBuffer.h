#pragma once

#include <vector>

class MemoryBuffer
{
public:
    void prepare(int channels, int sizeInSamples);
    void clear();

    int getSize() const { return size; }
    int getNumChannels() const { return numChannels; }

    int getWriteIndex() const { return writeIndex; }
    void setWriteIndex(int index);
    void advanceWrite();

    float readSample(int channel, int index) const;
    void writeSample(int channel, int index, float value);

private:
    int numChannels = 0;
    int size = 0;
    int writeIndex = 0;
    std::vector<float> data;
};
