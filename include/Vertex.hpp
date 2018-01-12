// Computer graphic lab 2
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
    using PositionType = Eigen::Matrix<float, 1, 4>;
    using ReferencePositionType = PositionType&;
    using ConstReferencePositionType = const PositionType&;

    using FloatType = float;
    using IntType = std::int32_t;

    Vertex() : Vertex{0.0, 0.0, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y) : Vertex{x, y, 0.0, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z) : Vertex{x, y, z, 1.0} {}
    Vertex(FloatType x, FloatType y, FloatType z, FloatType h)
        : Position{x, y, z, h} {}
    Vertex(ConstReferencePositionType position) : Position{position} {}

    constexpr ConstReferencePositionType GetPosition() const {
        return Position;
    }

    static constexpr IntType GetTupleSize() { return TUPLE_SIZE; }
    static constexpr IntType GetOffset() { return offsetOf(Vertex, Position); }
    static constexpr IntType GetStride() { return sizeof(Vertex); }

private:
    static const IntType TUPLE_SIZE = 4;

    PositionType Position;
};

#undef offsetOf

#endif  // CG_LAB_VERTEX_HPP_
