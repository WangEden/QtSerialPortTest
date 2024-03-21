#include "serialthread.h"

SerialThread::SerialThread(QObject * parent) : QThread(parent)
{
    RS485ModbusRtuMaster master(
        "COM1", 
        QSerialPort::Baud115200,
        QSerialPort::Data8,
        QSerialPort::NoParity,
        QSerialPort::OneStop
    );  
    
}

void SerialThread::run() 
{

}