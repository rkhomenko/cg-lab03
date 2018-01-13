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

template <typename T, typename U>
constexpr size_t offsetofImpl(T const* t, U T::*a) {
    return reinterpret_cast<char const*>(t) -
                       reinterpret_cast<char const*>(&(t->*a)) >=
                   0
               ? reinterpret_cast<char const*>(t) -
                     reinterpret_cast<char const*>(&(t->*a))
               : reinterpret_cast<char const*>(&(t->*a)) -
                     reinterpret_cast<char const*>(t);
}

#define offsetOf(Type_, Attr_) \
    offsetofImpl((Type_ const*)nullptr, &Type_::Attr_)

class Vertex {
public:
    using Vec4 = Eigen::Matrix<float, 1, 4>;
    using PositionType = Vec4;
    using ColorType = Vec4;

    using FloatType = float;
    using IntType = std::int32_t;

    Vertex() : Vertex{0.0, 0.0, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y) : Vertex{x, y, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z) : Vertex{x, y, z, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z, FloatType h)
        : Position{x, y, z, h} {}
    Vertex(const PositionType& position) : Position{position} {}
    Vertex(const PositionType& position, const ColorType& color)
        : Position{position}, Color{color} {}

    Vertex(Vertex&& v) = default;
    Vertex(const Vertex& v) = default;

    void SetColor(const ColorType& color) noexcept { Color = color; }

    constexpr const PositionType& GetPosition() const noexcept {
        return Position;
    }
    constexpr const ColorType& GetColor() const noexcept { return Color; }

    static constexpr IntType GetPositionTupleSize() noexcept {
        return POSITION_TUPLE_SIZE;
    }
    static constexpr IntType GetColorTupleSize() noexcept {
        return COLOR_TUPLE_SIZE;
    }
    static constexpr IntType GetPositionOffset() noexcept {
        return offsetOf(Vertex, Position);
    }
    static constexpr IntType GetColorOffset() noexcept {
        return offsetOf(Vertex, Position);
    }
    static constexpr IntType GetStride() noexcept { return sizeof(Vertex); }

private:
    static const IntType POSITION_TUPLE_SIZE = 4;
    static const IntType COLOR_TUPLE_SIZE = 4;

    PositionType Position;
    ColorType Color;
};

#undef offsetOf

#endif  // CG_LAB_VERTEX_HPP_
