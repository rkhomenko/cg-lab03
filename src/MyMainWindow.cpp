// Computer graphic lab 2
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyControlWidget.hpp>
#include <MyMainWindow.hpp>
#include <MyOpenGLWidget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QSurfaceFormat>
#include <QTabWidget>
#include <QVBoxLayout>

MyMainWindow::MyMainWindow(QWidget* parent) : QMainWindow(parent) {
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);

    const Vec4 orthoViewPoints[] = {Vec4{1, 0, 0, 1}, Vec4{0, 1, 0, 1},
                                    Vec4{0, 0, 1, 1}};
    auto i = 0UL;
    for (auto projSurface : MyOpenGLWidget::GetProjectionSurfaces()) {
        OrthoOpenGLWidgets[i] =
            new MyOpenGLWidget(MyOpenGLWidget::ProjectionType::ORTHOGRAPHIC,
                               projSurface, orthoViewPoints[i]);
        OrthoOpenGLWidgets[i++]->setFormat(format);
    }

    const Vec4 isoViewPoints[] = {Vec4{1, -1, 1, 1}, Vec4{1, 1, 1, 1},
                                  Vec4{1, 1, -1, 1}, Vec4{1, -1, -1, 1}};
    i = 0UL;
    for (auto isoProjType : MyOpenGLWidget::GetIsoProjTypes()) {
        IsoOpenGLWidgets[i] =
            new MyOpenGLWidget(MyOpenGLWidget::ProjectionType ::ISOMETRIC,
                               isoProjType, isoViewPoints[i]);
        IsoOpenGLWidgets[i]->setFormat(format);
        i++;
    }

    setCentralWidget(CreateCentralWidget());
}

MyMainWindow::~MyMainWindow() {
    for (auto ptr : OrthoOpenGLWidgets) {
        delete ptr;
    }
    for (auto ptr : IsoOpenGLWidgets) {
        delete ptr;
    }
}

QWidget* MyMainWindow::CreateCentralWidget() {
    const auto fixedSizePolicy =
        QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto widget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto tabWidget = new QTabWidget;

    auto initWidget = [this](auto controlWidget, auto openGLWidget) {
        // set connection for redraw on scale changed
        connect(controlWidget, &MyControlWidget::ScaleUpSignal, openGLWidget,
                &MyOpenGLWidget::ScaleUpSlot);
        connect(controlWidget, &MyControlWidget::ScaleDownSignal, openGLWidget,
                &MyOpenGLWidget::ScaleDownSlot);

        // set connection for redraw on angle changed
        connect(controlWidget, &MyControlWidget::OXAngleChangedSignal,
                openGLWidget, &MyOpenGLWidget::OXAngleChangedSlot);
        connect(controlWidget, &MyControlWidget::OYAngleChangedSignal,
                openGLWidget, &MyOpenGLWidget::OYAngleChangedSlot);
        connect(controlWidget, &MyControlWidget::OZAngleChangedSignal,
                openGLWidget, &MyOpenGLWidget::OZAngleChangedSlot);
    };

    auto createTab = [&](auto tabWidget, auto tabName, auto openGLWidgets,
                         auto widgetInfoStr) {
        auto tabLayout = new QVBoxLayout;
        auto controlWidget = new MyControlWidget;

        auto controlWidgetLayout = new QHBoxLayout;
        auto label = new QLabel(VARIANT_DESCRIPTION);
        label->setSizePolicy(fixedSizePolicy);

        controlWidgetLayout->addWidget(controlWidget);
        controlWidgetLayout->addWidget(label);
        controlWidgetLayout->addStretch();

        tabLayout->addLayout(controlWidgetLayout);

        auto openGLWidgetsLayout = new QHBoxLayout;
        auto i = 0U;
        for (auto ptr : openGLWidgets) {
            auto layout = new QVBoxLayout;
            auto label = new QLabel(widgetInfoStr[i++]);
            label->setSizePolicy(fixedSizePolicy);

            layout->addWidget(label);
            layout->addWidget(ptr);
            openGLWidgetsLayout->addLayout(layout);

            initWidget(controlWidget, ptr);
        }

        tabLayout->addLayout(openGLWidgetsLayout);

        auto tabCentralWidget = new QWidget;
        tabCentralWidget->setLayout(tabLayout);

        tabWidget->addTab(tabCentralWidget, tabName);
    };

    std::vector<const char*> orthoInfoStrs = {"x = 0 (YZ)", "y = 0 (XZ)",
                                              "z = 0 (XY)"};
    std::vector<const char*> isoInfoStrs = {"ϕ < 0,  θ < 0", "ϕ < 0, θ > 0",
                                            "ϕ > 0, θ < 0", "ϕ > 0, θ > 0"};

    createTab(tabWidget, "Orthographic projections", OrthoOpenGLWidgets,
              orthoInfoStrs);
    createTab(tabWidget, "Isometric Projections", IsoOpenGLWidgets,
              isoInfoStrs);

    mainLayout->addWidget(tabWidget);
    widget->setLayout(mainLayout);

    return widget;
}
