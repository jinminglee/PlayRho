/*
 * Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/Box2D
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "gtest/gtest.h"
#include <Box2D/Collision/DistanceProxy.hpp>
#include <initializer_list>
#include <vector>

using namespace box2d;

TEST(DistanceProxy, ByteSize)
{
    if (sizeof(RealNum) == 4)
    {
        EXPECT_EQ(sizeof(DistanceProxy), std::size_t(24));
    }
    else if (sizeof(RealNum) == 8)
    {
        EXPECT_EQ(sizeof(DistanceProxy), std::size_t(32));
    }
    else if (sizeof(RealNum) == 16)
    {
        EXPECT_EQ(sizeof(DistanceProxy), std::size_t(48));
    }
    else
    {
        FAIL();
    }
}

TEST(DistanceProxy, DefaultInitialization)
{
    const auto defaultDp = DistanceProxy{};
    
    EXPECT_EQ(defaultDp.GetVertexCount(), 0);
    EXPECT_EQ(defaultDp.GetVertexRadius(), (RealNum(0) * Meter));
}

TEST(DistanceProxy, OneVecInitialization)
{
    const auto radius = RealNum{1} * Meter;
    const auto vertex0 = Length2D{RealNum(2) * Meter, RealNum(-3) * Meter};
    const auto normal0 = UnitVec2{};
    const DistanceProxy foo{radius, 1, &vertex0, &normal0};
    EXPECT_EQ(radius, foo.GetVertexRadius());
    EXPECT_EQ(1, foo.GetVertexCount());
    EXPECT_EQ(vertex0, foo.GetVertex(0));
}

TEST(DistanceProxy, OneVecSupportIndex)
{
    const auto radius = RealNum{1} * Meter;
    const auto vertex0 = Length2D{RealNum(2) * Meter, RealNum(-3) * Meter};
    const auto normal0 = UnitVec2{};
    const DistanceProxy foo{radius, 1, &vertex0, &normal0};
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(vertex0)));
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(Length2D(0, 0))));
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(Length2D{vertex0.y, vertex0.x})));
}

TEST(DistanceProxy, TwoVecInitialization)
{
    const auto radius = RealNum{1} * Meter;
    const auto vertex0 = Length2D{RealNum(2) * Meter, RealNum(3) * Meter};
    const auto vertex1 = Length2D{RealNum(-10) * Meter, RealNum(-1) * Meter};
    const Length2D vertices[] = {vertex0, vertex1};
    const auto normal0 = GetUnitVector(vertex1 - vertex0);
    const UnitVec2 normals[] = {normal0, -normal0};
    const DistanceProxy foo{radius, 2, vertices, normals};
    EXPECT_EQ(radius, foo.GetVertexRadius());
    EXPECT_EQ(2, foo.GetVertexCount());
    EXPECT_EQ(vertex0, foo.GetVertex(0));
    EXPECT_EQ(vertex1, foo.GetVertex(1));
}

TEST(DistanceProxy, TwoVecSupportIndex)
{
    const auto radius = RealNum{1} * Meter;
    const auto vertex0 = Length2D{RealNum(2) * Meter, RealNum(3) * Meter};
    const auto vertex1 = Length2D{RealNum(-10) * Meter, RealNum(-1) * Meter};
    const Length2D vertices[] = {vertex0, vertex1};
    const auto normal0 = GetUnitVector(vertex1 - vertex0);
    const UnitVec2 normals[] = {normal0, -normal0};
    const DistanceProxy foo{radius, 2, vertices, normals};
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(vertex0)));
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(Length2D{vertex0.y, vertex0.x})));
    EXPECT_EQ(0, GetSupportIndex(foo, GetVec2(Length2D{RealNum(0) * Meter})));
    EXPECT_EQ(1, GetSupportIndex(foo, GetVec2(vertex1)));
    EXPECT_EQ(1, GetSupportIndex(foo, GetVec2(Length2D{vertex1.y, vertex1.x})));
}

TEST(DistanceProxy, ThreeVertices)
{
    const auto radius = RealNum(33) * Meter;
    const auto count = DistanceProxy::size_type(3);
    const auto v0 = Length2D{RealNum(1) * Meter, RealNum(2) * Meter};
    const auto v1 = Length2D{RealNum(-3) * Meter, RealNum(-4) * Meter};
    const auto v2 = Length2D{RealNum(-6) * Meter, RealNum(5) * Meter};
    const Length2D vertices[] = {v0, v1, v2};
    const auto n0 = GetUnitVector(v1 - v0);
    const auto n1 = GetUnitVector(v2 - v1);
    const auto n2 = GetUnitVector(v0 - v2);
    const UnitVec2 normals[] = {n0, n1, n2};
    
    const DistanceProxy foo{radius, 3, vertices, normals};
    
    EXPECT_EQ(foo.GetVertexRadius(), radius);
    ASSERT_EQ(foo.GetVertexCount(), count);
    EXPECT_EQ(foo.GetVertex(0).x, v0.x);
    EXPECT_EQ(foo.GetVertex(0).y, v0.y);
    EXPECT_EQ(foo.GetVertex(1).x, v1.x);
    EXPECT_EQ(foo.GetVertex(1).y, v1.y);
    EXPECT_EQ(foo.GetVertex(2).x, v2.x);
    EXPECT_EQ(foo.GetVertex(2).y, v2.y);
}

TEST(DistanceProxy, FindLowestRightMostVertex)
{
    const auto vertices = std::vector<Length2D>();
    const auto span = Span<const Length2D>(vertices.data(), std::size_t{0});
    const auto result = FindLowestRightMostVertex(span);
    EXPECT_EQ(result, static_cast<std::size_t>(-1));
}

TEST(DistanceProxy, TestPointWithEmptyProxyReturnsFalse)
{
    const auto defaultDp = DistanceProxy{};
    ASSERT_EQ(defaultDp.GetVertexCount(), 0);
    EXPECT_FALSE(TestPoint(defaultDp, Length2D(0, 0)));
}

TEST(DistanceProxy, TestPoint)
{
    const auto pos1 = Length2D{RealNum(3) * Meter, RealNum(1) * Meter};
    const auto pos2 = Length2D{RealNum(3) * Meter, RealNum(3) * Meter};
    const auto pos3 = Length2D{RealNum(1) * Meter, RealNum(3) * Meter};
    const auto pos4 = Length2D{RealNum(1) * Meter, RealNum(1) * Meter};
    const Length2D squareVerts[] = {pos1, pos2, pos3, pos4};
    const auto n1 = GetUnitVector(GetFwdPerpendicular(pos2 - pos1));
    const auto n2 = GetUnitVector(GetFwdPerpendicular(pos3 - pos2));
    const auto n3 = GetUnitVector(GetFwdPerpendicular(pos4 - pos3));
    const auto n4 = GetUnitVector(GetFwdPerpendicular(pos1 - pos4));
    const UnitVec2 squareNormals[] = {n1, n2, n3, n4};
    const auto radius = RealNum(0.5) * Meter;
    DistanceProxy dp{radius, 4, squareVerts, squareNormals};

    const auto pos0 = Length2D{RealNum(2) * Meter, RealNum(2) * Meter};
    
    ASSERT_EQ(dp.GetVertexCount(), 4);
    EXPECT_TRUE(TestPoint(dp, pos0));
    EXPECT_TRUE(TestPoint(dp, pos1));
    EXPECT_TRUE(TestPoint(dp, pos2));
    EXPECT_TRUE(TestPoint(dp, pos3));
    EXPECT_TRUE(TestPoint(dp, pos4));
    EXPECT_TRUE(TestPoint(dp, Length2D(3.2f * Meter, 3.2f * Meter)));
    EXPECT_TRUE(TestPoint(dp, pos2 + Length2D(radius, radius) / RealNum(2)));
    EXPECT_FALSE(TestPoint(dp, pos2 + Length2D(radius, radius)));
    EXPECT_FALSE(TestPoint(dp, Length2D(RealNum(10) * Meter, RealNum(10) * Meter)));
    EXPECT_FALSE(TestPoint(dp, Length2D{-RealNum(10) * Meter, RealNum(10) * Meter}));
    EXPECT_FALSE(TestPoint(dp, Length2D{RealNum(10) * Meter, -RealNum(10) * Meter}));
}
