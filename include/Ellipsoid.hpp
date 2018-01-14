// Computer graphic lab 3
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_ELLIPSOID_HPP_
#define CG_LAB_ELLIPSOID_HPP_

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
using Map4x4 = Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>;

using SizeType = std::size_t;
using LenghtType = float;
using VertexVector = std::vector<Vertex>;

class Lighting {
public:
    Lighting(float ambientCoeff,
             float specularCoeff,
             float diffuseCoeff,
             const Vec3& light,
             const Vec3& toObserverVec)
        : AmbientCoeff{ambientCoeff},
          SpecularCoeff{specularCoeff},
          DiffuseCoeff{diffuseCoeff},
          Light{light},
          ToObserverVec{toObserverVec} {}

    Vec4 Calculate(const Vec3& point,
                   const Vec3& normal,
                   const Vec3& color) const;

private:
    float AmbientCoeff;
    float SpecularCoeff;
    float DiffuseCoeff;

    Vec3 Light;
    Vec3 ToObserverVec;
};

class Layer {
public:
    enum class LayerType { SIDE, BOTTOM };

    Layer() = default;
    Layer(LenghtType a,
          LenghtType b,
          LenghtType c,
          LenghtType h,
          SizeType n,
          LenghtType deltaH,
          const Mat4x4& transformMatrix,
          const Vec3& viewPoint,
          const Lighting& lighting);
    Layer(LenghtType a,
          LenghtType b,
          LenghtType c,
          LenghtType h,
          SizeType n,
          const Mat4x4& transformMatrix,
          const Vec3& viewPoint,
          const Lighting& lighting);

    const VertexVector& GetVertices() const;
    SizeType GetItemsCount() const;
    Layer ApplyMatrix(const Mat4x4& matrix) const;
    LayerType GetType() const { return Type; }

private:
    static const float PI;

    void GenerateVertices(LenghtType a,
                          LenghtType b,
                          LenghtType c,
                          LenghtType h,
                          SizeType n,
                          LenghtType deltaH,
                          const Mat4x4& transformMatrix,
                          const Vec3& viewPoint,
                          const Lighting& lighting);
    void GenerateVertices(LenghtType a,
                          LenghtType b,
                          LenghtType c,
                          LenghtType h,
                          SizeType n,
                          const Mat4x4& transformMatrix,
                          const Vec3& viewPoint,
                          const Lighting& lighting);

    static Vec3 ToVec3(const Vec4& vec) { return Vec3(vec[0], vec[1], vec[2]); }
    static Vec3 GetNormal(const Vec4& first,
                          const Vec4& middle,
                          const Vec4& last);
    static bool CheckNormal(const Vec3& normal, const Vec3& viewPoint);

    VertexVector Vertices;
    LayerType Type;
};

using LayerVector = std::vector<Layer>;

class Ellipsoid {
public:
    Ellipsoid() = default;
    Ellipsoid(LenghtType a,
              LenghtType b,
              LenghtType c,
              SizeType vertexCount,
              SizeType surfaceCount,
              const Vec3& viewPoint);

    SizeType GetVertexCount() const;
    LayerVector GenerateVertices(const Mat4x4& transfomMatrix,
                                 const Mat4x4& scaleMatrix,
                                 const Lighting& lighting) const;

    void SetVertexCount(SizeType count);
    void SetSurfaceCount(SizeType count);

private:
    static LayerVector ApplyMatrix(const LayerVector& layers,
                                   const Mat4x4& matrix);

    LenghtType A;
    LenghtType B;
    LenghtType C;
    SizeType VertexCount;
    SizeType SurfaceCount;
    Vec3 ViewPoint;
};

#endif  // CG_LAB_ELLIPSOID_HPP_
