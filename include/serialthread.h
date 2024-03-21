#ifndef SERIALThread_H
#define SERIALThread_H

#include <QThread>
#include <QMutex>
#include "RS485.h"

class SerialThread : public QThread
{
    Q_OBJECT
public:
    enum FunctionCode {

    };

    SerialThread(QObject * parent = nullptr);
    bool openSerialPort();
    void closeSerialPort();
    void recvFunctionCode(FunctionCode);

signals:
    // void ReceiveResponse(const QByteArray &data); // 发送命令后受到回应时触发的信号

public slots:


protected:
    void run() override;

private:
    RS485ModbusRtuMaster * modbusRtuMaster;
    FunctionCode functionCode;

};

#endif // SERIALThread_H
