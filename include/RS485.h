#ifndef RS485_H
#define RS485_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include <vector>
#include <cstdint> // for uint8_t and uint16_t

using namespace std;

class RS485ModbusRtuMaster 
{
public:
    // 数据帧
    struct ModbusRTUFrame {
        uint8_t address;             // 从站地址
        uint8_t functionCode;        // 功能码
        std::vector<uint8_t> data;   // 数据字段，使用vector来动态管理数据大小
        uint16_t crc;                // CRC校验值

        // 可以根据需要添加构造函数、方法等
        ModbusRTUFrame(uint8_t addr, uint8_t funcCode, 
            const std::vector<uint8_t>& dataVec, uint16_t crcCode)
            : address(addr), functionCode(funcCode), data(dataVec), crc(crcCode) {
        }
    };

    // 夹爪波特率参数
    enum BaudRate {
        baud9600 = 0x00,
        baud19200 = 0x01,
        baud38400 = 0x02,
        baud57600 = 0x03,
        baud115200 = 0x04,
        baud153600 = 0x05,
        baud256000 = 0x06,
    };

    // 夹爪夹持状态
    enum ClampStatus {
        Arrived = 0x0000,
        Proceeding = 0x0001,
        Clamped = 0x0002,
        Drop = 0x0003,
    };

    enum ClampDirection {
        ClampOpen = 0x0000,
        ClampClose = 0x0001,
    };

    RS485ModbusRtuMaster();
    ~RS485ModbusRtuMaster();
    bool openPort(const QString &, int, QSerialPort::Parity);
    void closePort();

    // 功能函数
    void pawManualInitialize(); // 手动初始化
    void pawSetClampPosition(float); // 设置夹持位置，单位：mm
    void pawSetClosePaw(); // 闭合夹爪
    void pawSetClampSpeed(float); // 设置夹持速度
    void pawSetClampCurrentIntensity(float); // 设置夹持电流
    void pawSetID(); // 设置设备id
    void pawSetBaudRate(BaudRate); // 设置设备波特率

    ClampStatus pawReadClampStatus(); // 读取夹爪夹持状态
    float pawReadClampPosition(); // 读取夹爪夹持位置
    float pawReadClampCurrentIntensity(); // 读取夹爪夹持电流大小

    void pawSetClampInitDirection(ClampDirection); // 设置夹持初始化方向
    // void  // 自动/手动初始化设置?
    void pawSaveParameter(); // 保存参数
    void pawRecoverDefaultParam(); // 恢复默认参数
    void pawSetIOMode(bool turnOn); // 设置IO模式打开/关闭

    vector<uint8_t> pawGetResponse();


private:
    QSerialPort serial;



};
#endif

/**
 * 夹爪功能列举:
*/