/******************************************************************************
* Copyright (c) 2015, Peter J. Gadomski <pete.gadomski@gmail.com>
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include "gtest/gtest.h"

#include "TerrasolidReader.hpp"

#include <pdal/StageFactory.hpp>
#include "Support.hpp"


namespace pdal
{


namespace
{


std::string getTestfilePath()
{
    return Support::datapath("terrasolid/20020715-time-color.bin");
}


class TerrasolidReaderTest : public ::testing::Test
{
public:
    TerrasolidReaderTest()
        : ::testing::Test()
        , m_reader()
    {
        Options options;
        options.add("filename", getTestfilePath());
        m_reader.setOptions(options);
    }

    TerrasolidReader m_reader;
};
}


TEST(TerrasolidReader, Constructor)
{
    TerrasolidReader reader1;

    StageFactory f;
    std::unique_ptr<Stage> reader2(f.createStage("readers.terrasolid"));
}


TEST_F(TerrasolidReaderTest, Header)
{
    PointContext ctx;
    m_reader.prepare(ctx);
    TerraSolidHeader header = m_reader.getHeader();

    EXPECT_EQ(56, header.HdrSize);
    EXPECT_EQ(20020715, header.HdrVersion);
    EXPECT_EQ(970401, header.RecogVal);
    EXPECT_STREQ("CXYZ\xe8\x3", header.RecogStr);
    EXPECT_EQ(1000, header.PntCnt);
    EXPECT_EQ(100, header.Units);
    EXPECT_DOUBLE_EQ(0, header.OrgX);
    EXPECT_DOUBLE_EQ(0, header.OrgY);
    EXPECT_DOUBLE_EQ(0, header.OrgZ);
    EXPECT_EQ(1, header.Time);
    EXPECT_EQ(1, header.Color);
}


TEST_F(TerrasolidReaderTest, ReadingPoints)
{
    PointContext ctx;
    m_reader.prepare(ctx);
    PointBufferSet pbSet = m_reader.execute(ctx);
    EXPECT_EQ(pbSet.size(), 1u);
    PointBufferPtr buf = *pbSet.begin();
    EXPECT_EQ(buf->size(), 1000u);

    EXPECT_DOUBLE_EQ(363127.94, buf->getFieldAs<double>(Dimension::Id::X, 0));
    EXPECT_DOUBLE_EQ(3437612.33, buf->getFieldAs<double>(Dimension::Id::Y, 0));
    EXPECT_DOUBLE_EQ(55.26, buf->getFieldAs<double>(Dimension::Id::Z, 0));
    EXPECT_DOUBLE_EQ(0, buf->getFieldAs<double>(Dimension::Id::OffsetTime, 0));
    EXPECT_EQ(1840, buf->getFieldAs<uint16_t>(Dimension::Id::Intensity, 0));
    EXPECT_EQ(27207, buf->getFieldAs<uint16_t>(Dimension::Id::PointSourceId, 0));
    EXPECT_EQ(239, buf->getFieldAs<uint8_t>(Dimension::Id::Red, 0));
    EXPECT_EQ(252, buf->getFieldAs<uint8_t>(Dimension::Id::Green, 0));
    EXPECT_EQ(95, buf->getFieldAs<uint8_t>(Dimension::Id::Blue, 0));
    EXPECT_EQ(0, buf->getFieldAs<uint8_t>(Dimension::Id::Alpha, 0));
    EXPECT_EQ(1, buf->getFieldAs<uint8_t>(Dimension::Id::ReturnNumber, 0));
    EXPECT_EQ(1, buf->getFieldAs<uint8_t>(Dimension::Id::NumberOfReturns, 0));
    EXPECT_EQ(2, buf->getFieldAs<uint8_t>(Dimension::Id::Classification, 0));
    EXPECT_EQ(0, buf->getFieldAs<uint8_t>(Dimension::Id::Flag, 0));
    EXPECT_EQ(0, buf->getFieldAs<uint8_t>(Dimension::Id::Mark, 0));
}
}
