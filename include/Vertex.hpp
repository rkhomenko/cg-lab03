// Computer graphic lab 3
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_VERTEX_HPP_
#define CG_LAB_VERTEX_HPP_

#ifdef EIGEN3_INCLUDE_DIR
#include <Eigen/Dense>
#else
#include <eigen3/Eigen/Dense>
#endif

#include <array>

class Vertex {
public:
    using FloatType = float;
    using IntType = std::int32_t;

    using Vec4 = Eigen::Matrix<float, 1, 4>;
    using PODVec4 = std::array<FloatType, 4>;
    using PositionType = PODVec4;
    using ColorType = PODVec4;

    Vertex() : Vertex{0.0, 0.0, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y) : Vertex{x, y, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z) : Vertex{x, y, z, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z, FloatType h)
        : Position{{x, y, z, h}} {}
    Vertex(const Vec4& position) : Position{ToPODVec4(position)} {}
    Vertex(const Vec4& position, const Vec4& color)
        : Position{ToPODVec4(position)}, Color{ToPODVec4(color)} {}

    Vertex(Vertex&& v) = default;
    Vertex(const Vertex& v) = default;

    void SetColor(const Vec4& color) noexcept { Color = ToPODVec4(color); }

    Vec4 GetPosition() const noexcept { return ToVec4(Position); }
    Vec4 GetColor() const noexcept { return ToVec4(Color); }

    static constexpr IntType GetPositionTupleSize() noexcept {
        return POSITION_TUPLE_SIZE;
    }
    static constexpr IntType GetColorTupleSize() noexcept {
        return COLOR_TUPLE_SIZE;
    }
    static constexpr IntType GetPositionOffset() noexcept {
        return offsetof(Vertex, Position);
    }
    static constexpr IntType GetColorOffset() noexcept {
        return offsetof(Vertex, Position);
    }
    static constexpr IntType GetStride() noexcept { return sizeof(Vertex); }

private:
    static const IntType POSITION_TUPLE_SIZE = 3;
    static const IntType COLOR_TUPLE_SIZE = 4;

    static Vec4 ToVec4(const PODVec4& vec) {
        return Vec4(vec[0], vec[1], vec[2], vec[3]);
    }

    static PODVec4 ToPODVec4(const Vec4& vec) {
        return {{vec[0], vec[1], vec[2], vec[3]}};
    }

    PositionType Position;
    ColorType Color;
};

#endif  // CG_LAB_VERTEX_HPP_
