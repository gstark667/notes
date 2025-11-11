#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "FileModel.h"
#include "TreeModel.h"

#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    app.setOrganizationName("gstark");
    app.setOrganizationDomain("gstark.com");
    app.setApplicationName("Notes");

    qmlRegisterType<FileModel>("com.gstark", 1, 0, "FileModel");
    qmlRegisterType<TreeModel>("com.gstark", 1, 0, "TreeModel");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(url);
    return app.exec();
}

