#include "RS485.h"

RS485ModbusRtuMaster::RS485ModbusRtuMaster()
{

}

RS485ModbusRtuMaster::~RS485ModbusRtuMaster()
{
    closePort();
}

bool RS485ModbusRtuMaster::openPort(const QString &portName, 
    int baudRate = QSerialPort::Baud115200,
    QSerialPort::Parity parity = QSerialPort::NoParity)
{
    this->serial.setPortName(portName);
    this->serial.setBaudRate(baudRate);
    this->serial.setDataBits(QSerialPort::Data8);
    this->serial.setParity(parity);
    this->serial.setStopBits(QSerialPort::OneStop);
    return this->serial.open(QIODevice::ReadWrite);
}

void RS485ModbusRtuMaster::closePort()
{
    if (this->serial.isOpen()) {
        this->serial.close();
    }
}

void RS485ModbusRtuMaster::pawManualInitialize() // 手动初始化
{

}

void RS485ModbusRtuMaster::pawSetClampPosition(float) // 设置夹持位置，单位：mm
{

}

void RS485ModbusRtuMaster::pawSetClosePaw() // 闭合夹爪
{

}

void RS485ModbusRtuMaster::pawSetClampSpeed(float) // 设置夹持速度
{

}

void RS485ModbusRtuMaster::pawSetClampCurrentIntensity(float) // 设置夹持电流
{

}

void RS485ModbusRtuMaster::pawSetID() // 设置设备id
{

}

// 设置设备波特率
void RS485ModbusRtuMaster::RS485ModbusRtuMaster::pawSetBaudRate(BaudRate baudrate)
{
    vector<uint8_t> data = {0x00, 0x81, 0x00};
    data.push_back(baudrate);
    ModbusRTUFrame frame(0x01, 0x06, data, 0xD821);

}

RS485ModbusRtuMaster::ClampStatus RS485ModbusRtuMaster::pawReadClampStatus() // 读取夹爪夹持状态
{
    return Arrived;
}

float pawReadClampPosition() // 读取夹爪夹持位置
{
    return -1.;
}

float pawReadClampCurrentIntensity() // 读取夹爪夹持电流大小
{
    return -1.;
}

void RS485ModbusRtuMaster::pawSetClampInitDirection(ClampDirection) // 设置夹持初始化方向
{

}

void RS485ModbusRtuMaster::pawSaveParameter() // 保存参数
{

}

void RS485ModbusRtuMaster::pawRecoverDefaultParam() // 恢复默认参数
{

}

void RS485ModbusRtuMaster::pawSetIOMode(bool turnOn) // 设置IO模式打开/关闭
{

}
