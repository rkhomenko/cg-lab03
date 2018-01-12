// Computer graphic lab 2
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyMainWindow.hpp>
#include <MyOpenGLWidget.hpp>
#include <Pyramid.hpp>

#include <cmath>

#include <QApplication>
#include <QDebug>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QResizeEvent>

using namespace Eigen;

using Map4x4 = Map<Matrix<float, 4, 4, RowMajor>>;

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
    : MyOpenGLWidget(ProjectionType::NO_PROJECTION,
                     ProjectionSurface::NO_SURFACE,
                     Vec4(0, 0, 0, 0),
                     parent) {}

MyOpenGLWidget::MyOpenGLWidget(ProjectionType projType,
                               ProjectionSurface projSurface,
                               const Vec4& viewPoint,
                               QWidget* parent)
    : QOpenGLWidget(parent),
      Pyramid8Faces{8, 0.6f, 0.3f, 0.9f},
      ScaleFactor{1.0f},
      AngleOX{0},
      AngleOY{0},
      AngleOZ{0},
      ProjType{projType},
      ProjSurface{projSurface},
      IsoProjType{IsometricProjType::NO_TYPE},
      ViewPoint{viewPoint} {
    auto sizePolicy =
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setMinimumSize(WIDGET_DEFAULT_SIZE);
}

MyOpenGLWidget::MyOpenGLWidget(ProjectionType projType,
                               IsometricProjType isoProjType,
                               const Vec4& viewPoint,
                               QWidget* parent)
    : MyOpenGLWidget(projType,
                     ProjectionSurface::NO_SURFACE,
                     viewPoint,
                     parent) {
    IsoProjType = isoProjType;
}

void MyOpenGLWidget::ScaleUpSlot() {
    ScaleFactor *= SCALE_FACTOR_PER_ONCE;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::ScaleDownSlot() {
    ScaleFactor /= SCALE_FACTOR_PER_ONCE;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OXAngleChangedSlot(FloatType angle) {
    AngleOX = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OYAngleChangedSlot(FloatType angle) {
    AngleOY = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OZAngleChangedSlot(FloatType angle) {
    AngleOZ = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
            &MyOpenGLWidget::CleanUp);

    ShaderProgram = new QOpenGLShaderProgram(this);
    ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           VERTEX_SHADER);
    ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           FRAGMENT_SHADER);

    if (!ShaderProgram->link()) {
        qDebug() << ShaderProgram->log();
        QApplication::quit();
    }

    Buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    Buffer->create();
    Buffer->bind();
    Buffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    UpdateOnChange(width(), height());
    Buffer->allocate(sizeof(Vertex) * Pyramid8Faces.GetVerticesCount());

    int offset = 0;
    for (auto&& surface : Surfaces) {
        auto& vertices = surface.GetVertices();
        auto bytes = vertices.size() * sizeof(Vertex);
        Buffer->write(offset, vertices.data(), bytes);
        offset += bytes;
    }

    VertexArray = new QOpenGLVertexArrayObject;
    VertexArray->create();
    VertexArray->bind();

    int posAttr = ShaderProgram->attributeLocation(POSITION);
    ShaderProgram->enableAttributeArray(posAttr);
    ShaderProgram->setAttributeBuffer(posAttr, GL_FLOAT, Vertex::GetOffset(),
                                      Vertex::GetTupleSize(),
                                      Vertex::GetStride());
    auto color = QVector4D(0.0f, 0.0f, 1.0f, 1.0f);
    ShaderProgram->bind();
    ShaderProgram->setUniformValue(COLOR, color);

    VertexArray->release();
    Buffer->release();
    ShaderProgram->release();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    UpdateOnChange(width, height);
}

void MyOpenGLWidget::paintGL() {
    if (!ShaderProgram->bind()) {
        qDebug() << "Cannot bind program";
        QApplication::quit();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Buffer->bind();
    {
        int offset = 0;
        for (auto&& surface : Surfaces) {
            auto& vertices = surface.GetVertices();
            auto bytes = vertices.size() * sizeof(Vertex);
            Buffer->write(offset, vertices.data(), bytes);
            offset += bytes;
        }
    }
    Buffer->release();

    VertexArray->bind();
    {
        int offset = 0;
        for (auto&& surface : Surfaces) {
            int count = surface.GetVerticesCount();
            glDrawArrays(GL_LINE_LOOP, offset, count);
            offset += count;
        }
    }

    VertexArray->release();
    ShaderProgram->release();
}

void MyOpenGLWidget::CleanUp() {
    VertexArray->destroy();
    Buffer->destroy();
    delete VertexArray;
    delete Buffer;
    delete ShaderProgram;
}

void MyOpenGLWidget::UpdateOnChange(int width, int height) {
    const Mat4x4 shiftMatrix = GenerateShiftMatrix();
    const Mat4x4 rotateMatrix = GenerateRotateMatrix(RotateType::OX) *
                                GenerateRotateMatrix(RotateType::OY) *
                                GenerateRotateMatrix(RotateType::OZ);
    const Mat4x4 projectionMatrix =
        GenerateProjectionMatrix(ProjType, ProjSurface, IsoProjType);
    const Mat4x4 moveToXYMatrix =
        GenerateMoveToXYMatrix(ProjType, ProjSurface, IsoProjType);
    const Mat4x4 scaleMatrix = GenerateScaleMatrix(width, height);
    const Mat4x4 rotateAndShiftMatrix = shiftMatrix * rotateMatrix;
    const Mat4x4 projMoveScaleMatrix =
        projectionMatrix * moveToXYMatrix * scaleMatrix;

    const auto surfaces = Pyramid8Faces.GenerateVertices(
        ViewPoint, rotateAndShiftMatrix, projMoveScaleMatrix);
    Surfaces = surfaces;
}

void MyOpenGLWidget::OnWidgetUpdate() {
    auto event = new QResizeEvent(size(), size());
    QOpenGLWidget::event(event);
}

Mat4x4 MyOpenGLWidget::GenerateScaleMatrix(int width, int height) const {
    const auto DEFAULT_WIDTH = IMAGE_DEFAULT_SIZE.width();
    const auto DEFAULT_HEIGHT = IMAGE_DEFAULT_SIZE.height();

    auto xScaleFactor = 1.0f * DEFAULT_WIDTH / width;
    auto yScaleFactor = 1.0f * DEFAULT_HEIGHT / height;

    GLfloat matrixData[] = {
        xScaleFactor * ScaleFactor,
        0.0f,
        0.0f,
        0.0f,  // first line
        0.0f,
        yScaleFactor * ScaleFactor,
        0.0f,
        0.0f,  // second line
        0.0f,
        0.0f,
        1.0f,
        0.0f,  // third line
        0.0f,
        0.0f,
        0.0f,
        1.0f  // fourth line
    };

    return Map4x4(matrixData);
}

Mat4x4 MyOpenGLWidget::GenerateRotateMatrix(RotateType rotateType) const {
    FloatType angle = 0;
    switch (rotateType) {
        case RotateType::OX:
            angle = AngleOX;
            break;
        case RotateType::OY:
            angle = AngleOY;
            break;
        case RotateType::OZ:
            angle = AngleOZ;
            break;
    }
    return GenerateRotateMatrixByAngle(rotateType, angle);
}

Mat4x4 MyOpenGLWidget::GenerateShiftMatrix() const {
    FloatType matrixData[] = {
        1, 0, 0,
        0,  // first line
        0, 1, 0,
        0,  // second line
        0, 0, 1,
        0,  // third line
        0, 0, -Pyramid8Faces.GetHeight() / 2,
        1  // fourth
    };

    return Map4x4(matrixData);
}

Mat4x4 MyOpenGLWidget::GenerateRotateMatrixByAngle(RotateType rotateType,
                                                   FloatType angle) {
    FloatType rotateOXData[] = {
        1.0f,
        0,
        0,
        0,  // first line
        0,
        std::cos(angle),
        std::sin(angle),
        0,  // second line
        0,
        -std::sin(angle),
        std::cos(angle),
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType rotateOYData[] = {
        std::cos(angle),
        0,
        -std::sin(angle),
        0,  // fist line
        0,
        1.0f,
        0,
        0,  // second line
        std::sin(angle),
        0,
        std::cos(angle),
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType rotateOZData[] = {
        std::cos(angle),
        std::sin(angle),
        0,
        0,  // first line
        -std::sin(angle),
        std::cos(angle),
        0,
        0,  // second line
        0,
        0,
        1.0f,
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType* matrixData = nullptr;
    switch (rotateType) {
        case RotateType::OX:
            matrixData = rotateOXData;
            break;
        case RotateType::OY:
            matrixData = rotateOYData;
            break;
        case RotateType::OZ:
            matrixData = rotateOZData;
            break;
    }

    return Map4x4(matrixData);
}

Mat4x4 MyOpenGLWidget::GenerateProjectionMatrix(ProjectionType projType,
                                                ProjectionSurface projSurface,
                                                IsometricProjType isoProjType) {
    const auto X_COORD = 0;
    const auto Y_COORD = 1;
    const auto Z_COORD = 2;
    FloatType eMatrixData[] = {
        1, 0, 0, 0,  // first line
        0, 1, 0, 0,  // second line
        0, 0, 1, 0,  // third line
        0, 0, 0, 1   // fourth line
    };

    const auto PHI = std::asin(std::sqrt(2.0f) / 2);
    const auto TETA = std::asin(std::sqrt(1.0f / 3));

    auto generateIsoProjMatrix = [](float phi, float teta) -> Mat4x4 {
        float isoProjMatrixData[] = {
            std::cos(phi),
            std::sin(phi) * std::sin(teta),
            0,
            0,  // first line
            0,
            std::cos(teta),
            0,
            0,  // second line
            std::sin(phi),
            -std::cos(phi) * std::sin(teta),
            0,
            0,  // third line
            0,
            0,
            0,
            1  // fourth line
        };
        return Map4x4(isoProjMatrixData);
    };

    float phi = 0;
    float teta = 0;
    Mat4x4 projectionMatrix;
    switch (projType) {
        case ProjectionType::ORTHOGRAPHIC:
            projectionMatrix = Map4x4(eMatrixData);
            switch (projSurface) {
                case ProjectionSurface::X:
                    projectionMatrix(X_COORD, X_COORD) = 0;
                    break;
                case ProjectionSurface::Y:
                    projectionMatrix(Y_COORD, Y_COORD) = 0;
                    break;
                case ProjectionSurface::Z:
                    projectionMatrix(Z_COORD, Z_COORD) = 0;
                    break;
                case ProjectionSurface::NO_SURFACE:
                    break;
            }
            break;
        case ProjectionType::ISOMETRIC:
            switch (isoProjType) {
                case IsometricProjType::M_PHI_M_TETA:
                    phi = -PHI;
                    teta = -TETA;
                    break;
                case IsometricProjType::M_PHI_P_TETA:
                    phi = -PHI;
                    teta = TETA;
                    break;
                case IsometricProjType::P_PHI_M_TETA:
                    phi = PHI;
                    teta = -TETA;
                    break;
                case IsometricProjType::P_PHI_P_TETA:
                    phi = PHI;
                    teta = TETA;
                    break;
                case IsometricProjType::NO_TYPE:
                    break;
            }
            projectionMatrix = generateIsoProjMatrix(phi, teta);
            break;
        case ProjectionType::NO_PROJECTION:
            break;
    }

    return projectionMatrix;
}

Mat4x4 MyOpenGLWidget::GenerateMoveToXYMatrix(ProjectionType projType,
                                              ProjectionSurface projSurface,
                                              IsometricProjType isoProjType) {
    const auto PI = 4 * std::atan(1.0f);
    FloatType eData[] = {
        1, 0, 0, 0,  // first line
        0, 1, 0, 0,  // second line
        0, 0, 1, 0,  // third line
        0, 0, 0, 1   // fourth line
    };
    const Mat4x4 E = Map4x4(eData);

    Mat4x4 moveToXYMatrix;
    switch (projType) {
        case ProjectionType::ORTHOGRAPHIC:
            switch (projSurface) {
                case ProjectionSurface::X:
                    moveToXYMatrix =
                        GenerateRotateMatrixByAngle(RotateType::OY, -PI / 2);
                    break;
                case ProjectionSurface::Y:
                    moveToXYMatrix =
                        GenerateRotateMatrixByAngle(RotateType::OX, -PI / 2);
                    break;
                case ProjectionSurface::Z:
                    moveToXYMatrix = E;
                    break;
                case ProjectionSurface::NO_SURFACE:
                    break;
            }
            break;
        case ProjectionType::ISOMETRIC:
            switch (isoProjType) {
                case IsometricProjType::M_PHI_M_TETA:
                case IsometricProjType::M_PHI_P_TETA:
                case IsometricProjType::P_PHI_M_TETA:
                case IsometricProjType::P_PHI_P_TETA:
                    moveToXYMatrix = E;
                    break;
                case IsometricProjType::NO_TYPE:
                    break;
            }
            break;
        case ProjectionType::NO_PROJECTION:
            break;
    }

    return moveToXYMatrix;
}
