#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "TreeModel.h"
#include "Highlighter.h"

#include <QQmlContext>
#include <QWindow>
#include <QtQuickControls2/QQuickStyle>

#ifdef Q_OS_ANDROID
#include <QIcon>
#endif

int main(int argc, char *argv[])
{
    #ifdef Q_OS_ANDROID
        QQuickStyle::setStyle("Material");
        qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "System");
    #endif

    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    app.setOrganizationName("gstark");
    app.setOrganizationDomain("gstark.com");
    app.setApplicationName("Notes");

    qmlRegisterType<TreeModel>("com.gstark", 1, 0, "TreeModel");
    qmlRegisterType<Highlighter>("com.gstark", 1, 0, "Highlighter");

    const QUrl url(QStringLiteral("qrc:/qt/qml/notes/qml/main.qml"));

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
