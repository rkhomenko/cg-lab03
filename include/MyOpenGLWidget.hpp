// Computer graphic lab 2
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_MYOPENGLWIDGET_HPP_
#define CG_LAB_MYOPENGLWIDGET_HPP_

#include <Pyramid.hpp>

#include <array>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    using FloatType = float;
    enum class ProjectionType { ORTHOGRAPHIC, ISOMETRIC, NO_PROJECTION };
    enum class ProjectionSurface : int { X, Y, Z, NO_SURFACE };
    enum class IsometricProjType {
        M_PHI_M_TETA,
        M_PHI_P_TETA,
        P_PHI_M_TETA,
        P_PHI_P_TETA,
        NO_TYPE
    };

    explicit MyOpenGLWidget(QWidget* parent = nullptr);
    explicit MyOpenGLWidget(ProjectionType projType,
                            ProjectionSurface projSurface,
                            const Vec4& viewPoint,
                            QWidget* parent = nullptr);
    explicit MyOpenGLWidget(ProjectionType projType,
                            IsometricProjType isoProjType,
                            const Vec4& viewPoint,
                            QWidget* parent = nullptr);

    static constexpr std::array<ProjectionType, 2>
    GetProjectionTypes() noexcept {
        return {ProjectionType::ORTHOGRAPHIC, ProjectionType::ISOMETRIC};
    }

    static constexpr std::array<IsometricProjType, 4>
    GetIsoProjTypes() noexcept {
        return {
            IsometricProjType::M_PHI_M_TETA, IsometricProjType::M_PHI_P_TETA,
            IsometricProjType::P_PHI_M_TETA, IsometricProjType::P_PHI_P_TETA};
    }

    static constexpr std::array<ProjectionSurface, 3>
    GetProjectionSurfaces() noexcept {
        return {ProjectionSurface::X, ProjectionSurface::Y,
                ProjectionSurface::Z};
    }

public slots:
    void ScaleUpSlot();
    void ScaleDownSlot();

    void OXAngleChangedSlot(FloatType angle);
    void OYAngleChangedSlot(FloatType angle);
    void OZAngleChangedSlot(FloatType angle);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private slots:
    void CleanUp();

private:
    enum RotateType { OX, OY, OZ };

    static constexpr auto WIDGET_DEFAULT_SIZE = QSize(200, 200);
    static constexpr auto IMAGE_DEFAULT_SIZE = QSize(300, 300);

    static constexpr auto VERTEX_SHADER = ":/shaders/vertexShader.glsl";
    static constexpr auto FRAGMENT_SHADER = ":/shaders/fragmentShader.glsl";
    static constexpr auto POSITION = "position";
    static constexpr auto COLOR = "color";

    static constexpr auto SCALE_FACTOR_PER_ONCE = 1.15f;

    void UpdateOnChange(int width, int height);
    void OnWidgetUpdate();

    Mat4x4 GenerateScaleMatrix(int width, int height) const;
    Mat4x4 GenerateRotateMatrix(RotateType rotateType) const;
    Mat4x4 GenerateShiftMatrix() const;

    static Mat4x4 GenerateRotateMatrixByAngle(RotateType rotateType,
                                                  FloatType angle);
    static Mat4x4 GenerateProjectionMatrix(ProjectionType projType,
                                               ProjectionSurface projSurface,
                                               IsometricProjType isoProjType);
    static Mat4x4 GenerateMoveToXYMatrix(ProjectionType projType,
                                             ProjectionSurface projSurface,
                                             IsometricProjType isoProjType);

    QOpenGLShaderProgram* ShaderProgram;
    QOpenGLBuffer* Buffer;
    QOpenGLVertexArrayObject* VertexArray;
    Pyramid Pyramid8Faces;
    FloatType ScaleFactor;
    FloatType AngleOX;
    FloatType AngleOY;
    FloatType AngleOZ;
    ProjectionType ProjType;
    ProjectionSurface ProjSurface;
    IsometricProjType IsoProjType;
    Vec4 ViewPoint;
    Pyramid::SurfaceVector Surfaces;
};

#endif  // CG_LAB_MYOPENGLWIDGET_HPP_
