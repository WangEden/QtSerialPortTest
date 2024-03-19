#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>
#include <QQmlContext>

#include <QApplication>
#include <QtQuickWidgets/QQuickWidget>
#include <QWidget>
#include <QVBoxLayout>

#include "imagetransmission.h"
#include "serialadapter.h"
#include "QWidgetRenderer.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QApplication qWidgetApp(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QtSerialPortTest_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    QWidget * widget = new WhiteBackgroundWidget();

    // QWidget *container = QWidget::createWindowContainer(QWindow::fromWinId(window->winId()));
    // container->setParent(window);
    // QVBoxLayout *layout = new QVBoxLayout(container);

    // layout->addWidget(widget);

    // container->show();

    // 全局上下文对象
    {
        QQmlContext * context = engine.rootContext();
        context->setContextProperty("SCREEN_WIDTH", 600 * 2);
        context->setContextProperty("SCREEN_HEIGHT", 300 * 2);
    }

    // 注册对象
    {
        qmlRegisterType<ImageTransmission>("ImageTransmission", 1, 0, "ImageTransmission");
        qmlRegisterType<SerialAdapter>("SerialAdapter", 1, 0, "SerialAdapter");
        qmlRegisterType<QWidgetRenderer>("ManualQWidgets", 1, 0, "QWidgetRenderer");
    }

    const QUrl url(QStringLiteral("qrc:static/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    // 在load之后才可绑定信号和槽
    {
//        auto list = engine.rootObjects();
//        auto camera_window_container = list.first()->findChild<QObject *>("CameraWindowContainer");
//        qDebug() << camera_window_container;
    }

    return app.exec();
}
