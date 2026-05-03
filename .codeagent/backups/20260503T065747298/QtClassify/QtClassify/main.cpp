#include <QtQuick/QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "classifierbackend.h"

int main(int argc, char *argv[])
{
    // Force basic (main-thread) render loop on macOS.
    // The threaded render loop crashes on Apple Silicon because
    // CoreText→ImageIO→libpng is not thread-safe, and emoji glyph
    // rendering (CopyEmojiImage) triggers ImageIO's PNG reader which
    // has a libpng symbol conflict with Homebrew's libpng16.16.dylib
    // (loaded transitively by OpenCV).  The basic loop keeps all
    // rendering on the main thread, avoiding thread-safety issues.
    //
    // Additionally, all emoji characters have been removed from
    // Main.qml to avoid triggering the CopyEmojiImage path entirely,
    // which sidesteps the Homebrew/system libpng symbol conflict.
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
