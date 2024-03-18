#ifndef SERIALADAPTER_H
#define SERIALADAPTER_H

#include <QObject>
#include <QQuickItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/**
 * 自定义串口功能QML组件
 * 响应点击事件，根据名称属性决定实现何种功能，功能表写在SerialThread中，功能封装
 * 响应点击事件 ---> 调用自定义函数 ---> 开启子线程 ---> 开启串口 ---> 将串口对象的引用和功能号传递给子线程
 *  ---> 子线程提前写好对应函数，参数为串口对象 ---> 根据功能号调用对应函数 ---> 进入等待串口答复
 *  ---> 收到答复，传递相关数据回主线程 ---> 关闭串口 ---> 关闭子线程
*/

struct SerialThreadData
{   
    int id; // 需要执行的命令的id号（人为规定）
    QSerialPort p_QSerialPort;
};

class SerialAdapter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString functionName READ functionName WRITE setFunctionName NOTIFY functionNameChange)

public:
    explicit SerialAdapter(QQuickItem *parent = nullptr);

    QString functionName() const;
    void setFunctionName(const QString &text);

    Q_INVOKABLE void sendCommand();

    // 用于qml界面点击事件触发时调用
    // Q_INVOKABLE void open_serial();
    // Q_INVOKABLE void paw_init();
    // Q_INVOKABLE void paw_set_clamp_position(float); // 设置夹持位置 mm
    // Q_INVOKABLE void paw_close(); // 闭合夹爪
    // Q_INVOKABLE void paw_set_clamp_speed(float); // 设置夹持速度
    // Q_INVOKABLE void paw_set_clamp_current_intensity(float); // 设置夹持电流
signals:
    void functionNameChange();

private:
    QString m_functionName;
    SerialThreadData serialThreadData;
};

#endif // SERIALADAPTER_H
