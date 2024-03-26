#include "RS485.h"
#include <iostream>

RS485ModbusRtuMaster::RS485ModbusRtuMaster()
{

}

RS485ModbusRtuMaster::~RS485ModbusRtuMaster()
{
    closePort();
}

RS485ModbusRtuMaster::RS485ModbusRtuMaster(const QString & portName,
    QSerialPort::BaudRate baudRate = QSerialPort::Baud115200,
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

void RS485ModbusRtuMaster::setPortName(const QString & portName) 
{
    this->portName = portName;
}
void RS485ModbusRtuMaster::setBaudRate(QSerialPort::BaudRate baudRate)
{
    this->baudRate = baudRate;
}
void RS485ModbusRtuMaster::setDataBits(QSerialPort::DataBits dataBits)
{
    this->dataBits = dataBits;
}
void RS485ModbusRtuMaster::setParity(QSerialPort::Parity parity)
{
    this->parity = parity;
}
void RS485ModbusRtuMaster::setStopBits(QSerialPort::StopBits stopBits)
{
    this->stopBits = stopBits;
}

void RS485ModbusRtuMaster::closePort()
{
    if (this->serial.isOpen()) {
        this->serial.close();
    }
}

/**
 * @brief CRC校验值计算
 * 基于多项式为 0xA001 的 Modbus CRC16 算法
 */
uint16_t RS485ModbusRtuMaster::calculate_crc16(vector<uint8_t> data) {
    uint16_t crc = 0xFFFF;
    for (auto byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief 数据转换
 * 单精度浮点 float 和 4字节序列 之间的精确转换（IEEE754小序规范）
 */
void RS485ModbusRtuMaster::float_2_byte(float f, unsigned char *s) {
    uint32_t longdata = 0;
    longdata = *(uint32_t*)&f;
    s[0] = (longdata & 0xFF000000) >> 24;
    s[1] = (longdata & 0x00FF0000) >> 16;
    s[2] = (longdata & 0x0000FF00) >> 8;
    s[3] = (longdata & 0x000000FF);
}
float RS485ModbusRtuMaster::byte_2_float(unsigned char *s) {
    uint32_t longdata = 0;
    float f = 0.0;
    int i = 0;
    for (i = 0; i < 3; i++) {
        longdata += s[i];
        longdata <<= 8;
    }
    longdata += s[3];
    f = *(float*)&longdata;
    return f;
}

/**
 * @brief 创建一个数据帧用于发送
 * 1. 根据不同功能码创建数据帧
 * 2. CRC校验值 低字节在前，高字节在后
 * 
 */
void RS485ModbusRtuMaster::makeFrame(const ModbusRTUFrame * frame, vector<uint8_t> & byteArray) {
    // 写入设备地址
    byteArray.clear();
    byteArray.push_back(frame->address);
    
    // 写入功能代码
    ModbusFuncCode code = frame->functionCode;
    this->funcCode = code; // 发送时同时修改本类中的funcCode
    byteArray.push_back((uint8_t)code);
    uint16_t tmp = 0x0000;

    switch (code)
    {
    // 内容：要读取的寄存器起始地址(2B)，读取的长度(2B)
    case ReadHoldRegister: {
        tmp = (uint16_t)frame->registerStartAddress;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
        tmp = (uint16_t)frame->registerShiftAddress;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
    }break;
    
    // 内容：要写入的寄存器地址(2B)，要写入的内容(2B)
    case WriteSingleRegister: {
        tmp = (uint16_t)frame->registerStartAddress;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
        tmp = (uint16_t)frame->registerWriteContent;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
    }break;

    // 内容：要连续写入的首个寄存器地址(2B)，连续的长度(2B)，写入内容的字节数(1B)，写入的内容(4B)
    case WriteMultiRegister: {
        tmp = (uint16_t)frame->registerStartAddress;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
        tmp = (uint16_t)frame->registerShiftAddress;
        byteArray.push_back((tmp & 0xFF00) >> 8);
        byteArray.push_back((tmp & 0x00FF));
        byteArray.push_back(frame->registerMultiContentLength);
        float f_tmp = frame->registerMultiContent;
        unsigned char floatNumArray[4];
        float_2_byte(f_tmp, floatNumArray);
        for (uint8_t byte: floatNumArray) {
            byteArray.push_back(byte);
        }
    }break;

    default: {
        cout << "function code is invalid" << endl;
        return;
    }break;
    }

    // 添加CRC校验值
    uint16_t crc = calculate_crc16(byteArray);
    byteArray.push_back((crc & 0x00FF));
    byteArray.push_back((crc & 0xFF00) >> 8);
}

void RS485ModbusRtuMaster::sendCommand(vector<uint8_t> & data)
{
    // 检查串口是否打开
    if (!this->serial.isOpen()) {
        openPort();
    }
    // 发送时本类中存储最后一次发送的命令
    sendData.clear();
    for (uint8_t byte: data) {
        this->sendData.append(byte);
    }
    this->serial.write(this->sendData);
}

void RS485ModbusRtuMaster::getResponse(vector<uint8_t> & byteArray) {
    // 检查串口是否打开
    if (!this->serial.isOpen()) {
        openPort();
    }

    byteArray.clear();
    QObject::connect(&serial, &QSerialPort::readyRead, [&](){
        QByteArray data = serial.readAll();
        for (uint8_t byte: data) {
            byteArray.push_back(byte);
        }
    });
}

/**
 * @brief <报文格式>
 * 主机发送报文格式:
 * 设备地址:            01
 * 功能码：             06
 * 写入寄存器起始位置:    00 00
 * 写入寄存器内容:       00 01
 * CRC校验码：          48 0A
 * 从机（机械爪）应答报文格式:
 * 设备地址:            01
 * 功能码:              06
 * 寄存器起始地址:       00 00
 * 写入寄存器内容:       00 01
 * CRC校验码:           48 0A
 */
// 手动初始化
void RS485ModbusRtuMaster::pawManualInitialize() { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = (uint16_t)RegisterAddrForInit;
    frame.registerWriteContent = 0x0001;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置设备id
void RS485ModbusRtuMaster::pawSetID(uint8_t id) { 
    ModbusRTUFrame frame;
    this->DeviceId = id;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = (uint16_t)RegisterAddrForSetId;
    frame.registerWriteContent = 0x0001;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置设备波特率
void RS485ModbusRtuMaster::pawSetBaudRate(BaudRate baudRate) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetBaudrate;
    frame.registerWriteContent = (uint16_t)baudRate;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置夹持初始化方向
void RS485ModbusRtuMaster::pawSetClampInitDirection(ClampDirection clampDirection) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetInitDiret;
    frame.registerWriteContent = (uint16_t)clampDirection;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 上电后进行自动/手动初始化的设置
void RS485ModbusRtuMaster::pawSetInitMode(bool isAuto) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetInitAuto;
    frame.registerWriteContent = (uint16_t)(isAuto ? 0x0000 : 0x0001);
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置是否保存参数
void RS485ModbusRtuMaster::pawSaveParameter(bool isSave) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetIsSavaParam;
    frame.registerWriteContent = (uint16_t)(isSave? 0x0001 : 0x0000);
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 恢复默认参数
void RS485ModbusRtuMaster::pawRecoverDefaultParam() { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetRecoverDefaultParam;
    frame.registerWriteContent = 0x0001;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置IO模式打开/关闭
void RS485ModbusRtuMaster::pawSetIOMode(bool turnOn) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteSingleRegister;
    frame.registerStartAddress = RegisterAddrForSetIOMode;
    frame.registerWriteContent = (uint16_t)(turnOn? 0x0001 : 0x0000);
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}

/**
 * @brief <报文格式>
 * 主机发送报文格式:
 * 设备地址:            01
 * 功能码：             10
 * 写入寄存器起始位置:    00 02
 * 写入寄存器地址长度:    00 02
 * 写入数据长度:         04
 * 写入寄存器内容:       00 00 00 00
 * CRC校验码：          72 76 
 * 从机（机械爪）应答报文格式:
 * 设备地址:            01
 * 功能码:              10
 * 寄存器起始地址:       00 02
 * 修改的寄存器数量:      00 02
 * CRC校验码:           E0 08
 */
// 设置夹持位置，单位：mm(float)
void RS485ModbusRtuMaster::pawSetClampPosition(float position) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteMultiRegister;
    frame.registerStartAddress = RegisterAddrForSetClampPosition;
    frame.registerShiftAddress = 0x0002;
    frame.registerMultiContentLength = 0x04;
    frame.registerMultiContent = position;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 闭合夹爪
void RS485ModbusRtuMaster::pawSetClosePaw() { 
    pawSetClampPosition(0.f);
}
// 设置夹持速度
void RS485ModbusRtuMaster::pawSetClampSpeed(float speed) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteMultiRegister;
    frame.registerStartAddress = RegisterAddrForSetClampSpeed;
    frame.registerShiftAddress = 0x0002;
    frame.registerMultiContentLength = 0x04;
    frame.registerMultiContent = speed;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}
// 设置夹持电流
void RS485ModbusRtuMaster::pawSetClampCurrentIntensity(float intensity) { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = WriteMultiRegister;
    frame.registerStartAddress = RegisterAddrForSetClampIntensity;
    frame.registerShiftAddress = 0x0002;
    frame.registerMultiContentLength = 0x04;
    frame.registerMultiContent = intensity;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    sendCommand(byteArray);
}



/**
 * @brief 异常处理
 * 1.返回的异常功能码为正常功能码+0x80
 * 2.异常类型
 * 
 */


/**
 * @brief 超时处理
 * 
 */

/**
 * @brief 解析数据
 * 
 */
void RS485ModbusRtuMaster::parseResponse(vector<uint8_t> & byteArray, vector<uint8_t> & output, uint16_t & crc) {
    uint8_t deviceId = byteArray.at(0);
    uint8_t funcCode = byteArray.at(1);
    uint8_t dataLength = byteArray.at(2);
    output.clear();
    for (size_t i = 0; i < (size_t)dataLength; i++) {
        output.push_back(byteArray.at(i + 3));
    }
    byteArray.pop_back();
    byteArray.pop_back();
    crc = calculate_crc16(byteArray);
}

/**
 * @brief <报文格式>
 * 主机发送报文格式:
 * 设备地址:            01
 * 功能码：             03
 * 写入寄存器起始位置:    00 02
 * 写入寄存器地址长度:    00 02
 * CRC校验码：          72 76 
 * 从机（机械爪）应答报文格式:
 * 设备地址:            01
 * 功能码:              03
 * 具体情况不同
 * CRC校验码:           E0 08
 */
// 读取夹爪夹持状态
RS485ModbusRtuMaster::ClampStatus RS485ModbusRtuMaster::pawReadClampStatus() { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = ReadHoldRegister;
    frame.registerStartAddress = RegisterAddrForReadClampStatus;
    frame.registerShiftAddress = 0x0001;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    uint8_t crc1L = byteArray.at(byteArray.size() - 2);
    uint8_t crc1H = byteArray.at(byteArray.size() - 1);
    uint16_t crc1 = (uint16_t)(crc1L) + ((uint16_t)(crc1H) << 8);
    sendCommand(byteArray);
    // 接收数据
    getResponse(byteArray);
    vector<uint8_t> data; 
    uint16_t crc2;
    parseResponse(byteArray, data, crc2);
    if (crc1 != crc2)
        return Error;
    uint8_t resultL = data.at(data.size() - 1);
    uint8_t resultH = data.at(data.size() - 2);
    uint16_t result = (uint16_t)(resultL) + ((uint16_t)(resultH) << 8);
    return (ClampStatus)result;
}
// 读取夹爪夹持位置
float RS485ModbusRtuMaster::pawReadClampPosition() { 
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = ReadHoldRegister;
    frame.registerStartAddress = RegisterAddrForReadClampPosition;
    frame.registerShiftAddress = 0x0002;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    uint8_t crc1L = byteArray.at(byteArray.size() - 2);
    uint8_t crc1H = byteArray.at(byteArray.size() - 1);
    uint16_t crc1 = (uint16_t)(crc1L) + ((uint16_t)(crc1H) << 8);
    sendCommand(byteArray);
    // 接收数据
    getResponse(byteArray);
    vector<uint8_t> data;
    uint16_t crc2;
    parseResponse(byteArray, data, crc2);
    if (crc1 != crc2)
        return -1.f;
    return byte_2_float(&data[0]);
}
// 读取夹爪夹持电流大小
float RS485ModbusRtuMaster::pawReadClampCurrentIntensity() {
    ModbusRTUFrame frame;
    frame.address = this->DeviceId;
    frame.functionCode = ReadHoldRegister;
    frame.registerStartAddress = RegisterAddrForReadClampIntensity;
    frame.registerShiftAddress = 0x0002;
    vector<uint8_t> byteArray;
    makeFrame(&frame, byteArray);
    uint8_t crc1L = byteArray.at(byteArray.size() - 2);
    uint8_t crc1H = byteArray.at(byteArray.size() - 1);
    uint16_t crc1 = (uint16_t)(crc1L) + ((uint16_t)(crc1H) << 8);
    sendCommand(byteArray);
    // 接收数据
    getResponse(byteArray);
    vector<uint8_t> data;
    uint16_t crc2;
    parseResponse(byteArray, data, crc2);
    if (crc1 != crc2)
        return -1.f;
    return byte_2_float(&data[0]);
}
