#include <QtQuick/QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "classifierbackend.h"

int main(int argc, char *argv[])
{
    // Force basic (main-thread) render loop on macOS.
    // The threaded render loop crashes on Apple Silicon when rendering
    // emoji glyphs because CoreText/ImageIO are not thread-safe.
    qputenv("QSG_RENDER_LOOP", "basic");
    QGuiApplication app(argc, argv);
    app.setOrganizationName("QtClassify");
    app.setApplicationName("QtClassify");

    ClassifierBackend backend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("QtClassify", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
