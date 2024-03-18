#ifndef SERIALThread_H
#define SERIALThread_H

#include <QThread>
#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialThread : public QThread
{
    Q_OBJECT
public:
    SerialThread(QObject * parent = nullptr);
    void openSerial();
    void pawInit();
    void pawSetClampPosition(float); // 设置夹持位置 mm
    void pawClose(); // 闭合夹爪
    void pawSetClampSpeed(float); // 设置夹持速度
    void pawSetClampCurrentIntensity(float); // 设置夹持电流

signals:
    void ReceiveResponse(const QByteArray &data); // 发送命令后受到回应时触发的信号

protected:
    void run() override {
        QSerialPort serial;

    }

};

#endif // SERIALThread_H
