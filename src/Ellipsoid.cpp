#include <Ellipsoid.hpp>

#include <cmath>
#include <utility>

Layer::Layer(LenghtType a,
             LenghtType b,
             LenghtType c,
             LenghtType h,
             SizeType n,
             LenghtType deltaH,
             const Mat4x4& transformMatrix,
             const Vec3& viewPoint) {
    GenerateVertices(a, b, c, h, n, deltaH, transformMatrix, viewPoint);
}

const VertexVector& Layer::GetVertices() const {
    return Vertices;
}

SizeType Layer::GetItemsCount() const {
    return Vertices.size();
}

Layer Layer::ApplyMatrix(const Mat4x4& matrix) const {
    Layer layer;
    for (auto&& vertex : Vertices) {
        layer.Vertices.emplace_back(vertex.GetPosition() * matrix,
                                    vertex.GetColor());
    }
    return layer;
}

void Layer::GenerateVertices(LenghtType a,
                             LenghtType b,
                             LenghtType c,
                             LenghtType h,
                             SizeType n,
                             LenghtType deltaH,
                             const Mat4x4& transformMatrix,
                             const Vec3& viewPoint) {
    const auto PI = 4 * std::atan(1.0f);
    const auto DELTA_PHI = 2 * PI / n;

    auto generateVertex = [a, b, c, DELTA_PHI](auto&& i, auto&& h) {
        const auto C = (c * c - h * h) / c * c;
        const auto A = std::sqrt(C) * a;
        const auto B = std::sqrt(C) * b;
        return Vertex(A * std::cos(i * DELTA_PHI), B * std::sin(i * DELTA_PHI),
                      h);
    };

    auto toVec3 = [](auto&& vec4) { return Vec3(vec4[0], vec4[1], vec4[2]); };

    auto getNormal = [toVec3](Vec4&& first, Vec4&& middle, Vec4&& last) {
        const auto center = Vec3(0, 0, 0);
        auto v1 = toVec3(middle - first);
        auto v2 = toVec3(last - first);

        Vec3 normal = v1.cross(v2);
        normal.normalize();

        if (Vec3 toCenterVec = center - toVec3(middle);
            toCenterVec.dot(normal) > 0) {
            normal *= -1.0f;
        }

        return normal;
    };

    auto checkNormal = [](auto&& normal, auto&& viewPoint) {
        float dotProduct = viewPoint.dot(normal);
        if (dotProduct > 0) {
            return true;
        }
        return false;
    };

    const auto BLUE = Vec4(0, 0, 1, 1);

    for (auto i = 0UL; i < n; i++) {
        auto first = generateVertex(i, h).GetPosition() * transformMatrix;
        auto second =
            generateVertex(i, h + deltaH).GetPosition() * transformMatrix;
        auto third = generateVertex(i + 1, h).GetPosition() * transformMatrix;
        auto fourth =
            generateVertex(i + 1, h + deltaH).GetPosition() * transformMatrix;

        if (checkNormal(getNormal(first, second, third), viewPoint)) {
            Vertices.emplace_back(first, BLUE);
            Vertices.emplace_back(second, BLUE);
            Vertices.emplace_back(third, BLUE);
        }

        if (checkNormal(getNormal(second, fourth, third), viewPoint)) {
            Vertices.emplace_back(second, BLUE);
            Vertices.emplace_back(third, BLUE);
            Vertices.emplace_back(fourth, BLUE);
        }
    }
}

Ellipsoid::Ellipsoid(LenghtType a,
                     LenghtType b,
                     LenghtType c,
                     LenghtType deltaH,
                     SizeType n,
                     const Vec3& viewPoint)
    : A{a}, B{b}, C{c}, DeltaH{deltaH}, N{n}, ViewPoint{viewPoint} {}

LayerVector Ellipsoid::GenerateVertices(const Mat4x4& rotateMatrix,
                                        const Mat4x4& scaleProjMatrix) const {
    LayerVector layers;
    float start = -0.1f;
    float stop = 0.1f;
    float delta = (stop - start) / DeltaH;
    for (auto h = start; h <= stop; h += delta) {
        auto layer = Layer(A, B, C, h, N, delta, rotateMatrix, ViewPoint);
        if (layer.GetItemsCount() != 0) {
            layers.emplace_back(layer);
        }
    }
    return ApplyMatrix(layers, scaleProjMatrix);
}

LayerVector Ellipsoid::ApplyMatrix(const LayerVector& layers,
                                   const Mat4x4& matrix) {
    LayerVector result;
    for (auto&& layer : layers) {
        result.emplace_back(layer.ApplyMatrix(matrix));
    }
    return result;
}
