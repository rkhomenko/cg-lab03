// Computer graphic lab 2
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_PYRAMID_HPP_
#define CG_LAB_PYRAMID_HPP_

#include <Vertex.hpp>

#include <cstdint>
#include <vector>

#ifdef EIGEN3_INCLUDE_DIR
#include <Eigen/Dense>
#else
#include <eigen3/Eigen/Dense>
#endif

using Vec3 = Eigen::Matrix<float, 1, 3>;
using Vec4 = Eigen::Matrix<float, 1, 4>;
using Mat4x4 = Eigen::Matrix<float, 4, 4>;

class Pyramid {
public:
    using SizeType = std::size_t;
    using LenghtType = float;
    using VertexVector = std::vector<Vertex>;

    class Surface {
    public:
        enum class SurfaceType { BASE, FACE };

        Surface() = default;
        Surface(SurfaceType type, const VertexVector& data)
            : Type{type}, Data{data} {}

        Vec3 GetNormal(const Vec3& center) const;
        SurfaceType GetType() const { return Type; }
        SizeType GetVerticesCount() const { return Data.size(); }

        const VertexVector& GetVertices() const;

        static Vec3 VertexToVec3(const Vertex& v);

    private:
        SurfaceType Type;
        VertexVector Data;
    };

    using SurfaceVector = std::vector<Surface>;

    Pyramid(SizeType facesCount,
            LenghtType baseRadius,
            LenghtType topRadius,
            LenghtType height);

    SizeType GetVerticesCount() const;
    LenghtType GetHeight() const { return Height; }
    SurfaceVector GenerateVertices(const Vec4& viewPoint,
                                   const Mat4x4& rotateAndShit,
                                   const Mat4x4& projMoveScale) const;

private:
    static SurfaceVector ApplyMatrix(const SurfaceVector& surfaces,
                                     const Mat4x4& matrix);

    SurfaceVector ApplySurfaces(const SurfaceVector& surfaces,
                                const Vec4& viewPoint) const;

    SizeType FacesCount;
    LenghtType BaseRadius;
    LenghtType TopRadius;
    LenghtType Height;
    SurfaceVector Surfaces;
};

#endif  //  CG_LAB_PYRAMID_HPP_
