#include <catch2/catch_test_macros.hpp>

#include "dsp/MemoryBuffer.h"

TEST_CASE("MemoryBuffer wraps write index", "[buffer]")
{
    MemoryBuffer buffer;
    buffer.prepare(2, 4);

    REQUIRE(buffer.getWriteIndex() == 0);

    for (int i = 0; i < 6; ++i)
        buffer.advanceWrite();

    REQUIRE(buffer.getWriteIndex() == 2);
}

TEST_CASE("MemoryBuffer reads and writes with wrap", "[buffer]")
{
    MemoryBuffer buffer;
    buffer.prepare(1, 4);

    buffer.writeSample(0, 0, 1.0f);
    buffer.writeSample(0, 1, 2.0f);
    buffer.writeSample(0, 2, 3.0f);
    buffer.writeSample(0, 3, 4.0f);

    REQUIRE(buffer.readSample(0, 0) == 1.0f);
    REQUIRE(buffer.readSample(0, 3) == 4.0f);
    REQUIRE(buffer.readSample(0, 4) == 1.0f);
    REQUIRE(buffer.readSample(0, -1) == 4.0f);
}
