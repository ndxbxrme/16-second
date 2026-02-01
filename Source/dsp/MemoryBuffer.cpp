#include "MemoryBuffer.h"

#include <algorithm>
#include <cmath>

void MemoryBuffer::prepare(int channels, int sizeInSamples)
{
    numChannels = std::max(1, channels);
    size = std::max(1, sizeInSamples);
    writeIndex = 0;
    data.assign(static_cast<size_t>(numChannels * size), 0.0f);
}

void MemoryBuffer::clear()
{
    std::fill(data.begin(), data.end(), 0.0f);
    writeIndex = 0;
}

void MemoryBuffer::setWriteIndex(int index)
{
    if (size <= 0)
    {
        writeIndex = 0;
        return;
    }

    writeIndex = index % size;
    if (writeIndex < 0)
        writeIndex += size;
}

void MemoryBuffer::advanceWrite()
{
    if (size <= 0)
        return;

    ++writeIndex;
    if (writeIndex >= size)
        writeIndex = 0;
}

float MemoryBuffer::readSample(int channel, int index) const
{
    if (size <= 0 || numChannels <= 0)
        return 0.0f;

    if (channel < 0 || channel >= numChannels)
        return 0.0f;

    auto wrappedIndex = index % size;
    if (wrappedIndex < 0)
        wrappedIndex += size;

    const auto offset = static_cast<size_t>(channel * size + wrappedIndex);
    return data[offset];
}

float MemoryBuffer::readSampleLinear(int channel, float index) const
{
    if (size <= 0 || numChannels <= 0)
        return 0.0f;

    if (channel < 0 || channel >= numChannels)
        return 0.0f;

    auto baseIndex = static_cast<int>(std::floor(index));
    const auto frac = index - static_cast<float>(baseIndex);
    const auto nextIndex = baseIndex + 1;

    const auto sampleA = readSample(channel, baseIndex);
    const auto sampleB = readSample(channel, nextIndex);

    return sampleA + (sampleB - sampleA) * frac;
}

void MemoryBuffer::writeSample(int channel, int index, float value)
{
    if (size <= 0 || numChannels <= 0)
        return;

    if (channel < 0 || channel >= numChannels)
        return;

    auto wrappedIndex = index % size;
    if (wrappedIndex < 0)
        wrappedIndex += size;

    const auto offset = static_cast<size_t>(channel * size + wrappedIndex);
    data[offset] = value;
}
