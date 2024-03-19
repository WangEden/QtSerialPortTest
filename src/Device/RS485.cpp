#include "RS485.h"

RS485ModbusRtuMaster::RS485ModbusRtuMaster()
{

}

RS485ModbusRtuMaster::~RS485ModbusRtuMaster()
{
    closePort();
}

RS485ModbusRtuMaster::RS485ModbusRtuMaster(const QString & portName,
    int baudRate = QSerialPort::Baud115200,
    QSerialPort::DataBits dataBits = QSerialPort::Data8,
    QSerialPort::Parity parity = QSerialPort::NoParity,
    QSerialPort::StopBits stopBits = QSerialPort::OneStop)
{
    this->portName = portName;
    this->baudRate = baudRate;
    this->dataBits = dataBits;
    this->parity = parity;
    this->stopBits = stopBits;
}

bool RS485ModbusRtuMaster::openPort()
{
    this->serial.setPortName(this->portName);
    this->serial.setBaudRate(this->baudRate);
    this->serial.setDataBits(this->dataBits);
    this->serial.setParity(this->parity);
    this->serial.setStopBits(this->stopBits);
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

void RS485ModbusRtuMaster::sendCommand(vector<uint8_t> &data)
{
    // 检查串口是否打开
    if (!this->serial.isOpen()) {
        openPort();
    }
    ModbusRTUFrame frame(0x01, 0x06, data, 0xD821);
    QByteArray sendData;
    sendData.append(frame.address);
    sendData.append(frame.functionCode);
    for (uint8_t byte: frame.data) {
        sendData.append(byte);
    }
    sendData.append(static_cast<uint8_t>(frame.crc & 0xFF)); // 获取底字节
    sendData.append((frame.crc >> 8) & 0xFF); // 获取高字节
    this->serial.write(sendData);
}

