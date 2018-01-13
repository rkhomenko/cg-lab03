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

    const VertexVector& GetVertices() const;
    SizeType GetItemsCount() const;
    Layer ApplyMatrix(const Mat4x4& matrix) const;

private:
    void GenerateVertices(LenghtType a,
                          LenghtType b,
                          LenghtType c,
                          LenghtType h,
                          SizeType n,
                          LenghtType deltaH,
                          const Mat4x4& transformMatrix,
                          const Vec3& viewPoint,
                          const Lighting& lighting);

    VertexVector Vertices;
};

using LayerVector = std::vector<Layer>;

class Ellipsoid {
public:
    Ellipsoid() = default;
    Ellipsoid(LenghtType a,
              LenghtType b,
              LenghtType c,
              LenghtType deltaH,
              SizeType n,
              const Vec3& viewPoint);

    SizeType GetVertexCount() const;
    LayerVector GenerateVertices(const Mat4x4& transfomMatrix,
                                 const Mat4x4& scaleMatrix,
                                 const Lighting& lighting) const;

private:
    static LayerVector ApplyMatrix(const LayerVector& layers,
                                   const Mat4x4& matrix);

    LenghtType A;
    LenghtType B;
    LenghtType C;
    LenghtType DeltaH;
    SizeType N;
    Vec3 ViewPoint;
};

#endif  // CG_LAB_ELLIPSOID_HPP_
