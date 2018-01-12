#include <Pyramid.hpp>

#include <cmath>

Pyramid::Pyramid(SizeType facesCount,
                 LenghtType baseRadius,
                 LenghtType topRadius,
                 LenghtType height)
    : FacesCount{facesCount},
      BaseRadius{baseRadius},
      TopRadius{topRadius},
      Height{height} {
    const auto PI = 4 * std::atan(1.0f);
    const auto PHI = 2 * PI / FacesCount;

    auto createVertex = [PHI](auto radius, auto height, auto i) {
        return Vertex(radius * std::cos(i * PHI), radius * std::sin(i * PHI),
                      height);
    };

    for (auto i = 0U; i < FacesCount; i++) {
        VertexVector face;
        face.emplace_back(createVertex(BaseRadius, 0, i));
        face.emplace_back(createVertex(TopRadius, Height, i));
        face.emplace_back(createVertex(TopRadius, Height, i + 1));
        face.emplace_back(createVertex(BaseRadius, 0, i + 1));
        Surfaces.emplace_back(Surface::SurfaceType::FACE, face);
    }

    auto generateBase = [this, createVertex](auto radius, auto height) {
        VertexVector base;
        for (auto i = 0U; i < FacesCount; i++) {
            base.emplace_back(createVertex(radius, height, i));
        }
        return base;
    };

    Surfaces.emplace_back(Surface::SurfaceType::BASE,
                          generateBase(BaseRadius, 0));
    Surfaces.emplace_back(Surface::SurfaceType::BASE,
                          generateBase(TopRadius, Height));
}

const Pyramid::VertexVector& Pyramid::Surface::GetVertices() const {
    return Data;
}

Pyramid::SizeType Pyramid::GetVerticesCount() const {
    auto count = 0UL;
    for (auto&& surface : Surfaces) {
        count += surface.GetVerticesCount();
    }
    return count;
}

Pyramid::SurfaceVector Pyramid::GenerateVertices(
    const Vec4& viewPoint,
    const Mat4x4& rotateAndShit,
    const Mat4x4& projMoveScale) const {
    auto surfaces = ApplyMatrix(Surfaces, rotateAndShit);
    return ApplyMatrix(ApplySurfaces(surfaces, viewPoint), projMoveScale);
}

Pyramid::SurfaceVector Pyramid::ApplyMatrix(const SurfaceVector& surfaces,
                                            const Mat4x4& matrix) {
    SurfaceVector newSurfaces;

    for (auto&& surface : surfaces) {
        VertexVector vertices;
        for (auto&& vertex : surface.GetVertices()) {
            vertices.emplace_back(vertex.GetPosition() * matrix);
        }
        newSurfaces.emplace_back(surface.GetType(), vertices);
    }

    return newSurfaces;
}

Pyramid::SurfaceVector Pyramid::ApplySurfaces(const SurfaceVector& surfaces,
                                              const Vec4& viewPoint) const {
    const auto center = Vec3(0, 0, 0);
    auto point = Vec3(viewPoint[0], viewPoint[1], viewPoint[2]);
    SurfaceVector result;

    for (auto&& surface : surfaces) {
        Vec3 normal = surface.GetNormal(center);
        float dotProduct = point.dot(normal);
        if (dotProduct >= 0) {
            result.emplace_back(surface);
        }
    }

    return result;
}

Vec3 Pyramid::Surface::GetNormal(const Vec3& center) const {
    auto first = VertexToVec3(Data[0]);
    auto middle = VertexToVec3(Data[1]);
    auto last = VertexToVec3(Data[2]);

    Vec3 v1 = first - middle;
    Vec3 v2 = last - middle;

    Vec3 normal = v1.cross(v2);
    normal.normalize();
    if (Vec3 toCenterVec = center - middle; toCenterVec.dot(normal) > 0) {
        normal *= -1.0f;
    }

    return normal;
}

Vec3 Pyramid::Surface::VertexToVec3(const Vertex& v) {
    auto vec = v.GetPosition();
    return Vec3(vec[0], vec[1], vec[2]);
}
