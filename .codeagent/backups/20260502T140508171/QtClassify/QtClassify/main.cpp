#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "classifierbackend.h"

int main(int argc, char *argv[])
{
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
