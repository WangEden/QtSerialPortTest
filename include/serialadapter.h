#ifndef SERIALADAPTER_H
#define SERIALADAPTER_H

#include <QObject>
#include <QQuickItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "serialthread.h"

/**
 * 自定义串口功能QML组件
 * 响应点击事件，根据名称属性决定实现何种功能，功能表写在SerialThread中，功能封装
 * 响应点击事件 ---> 调用自定义函数 ---> 开启子线程 ---> 开启串口 ---> 将串口对象的引用和功能号传递给子线程
 *  ---> 子线程提前写好对应函数，参数为串口对象 ---> 根据功能号调用对应函数 ---> 进入等待串口答复
 *  ---> 收到答复，传递相关数据回主线程 ---> 关闭串口 ---> 关闭子线程
*/

class SerialAdapter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString functionName READ functionName WRITE setFunctionName NOTIFY functionNameChange)
    Q_PROPERTY(bool rewriteFlag READ rewriteFlag WRITE setRewriteFlag NOTIFY rewriteFlagChange) // 决定是否重写按钮功能
    Q_PROPERTY(int functionCode READ functionCode WRITE setFunctionCode NOTIFY functionCodeChange) // 决定是否重写按钮功能

public:
    enum FunctionCode {

    };

    explicit SerialAdapter(QQuickItem *parent = nullptr);

    QString functionName() const;
    void setFunctionName(const QString &text);

    bool rewriteFlag() const;
    void setRewriteFlag(const bool &flag);

    int functionCode() const;
    void setFunctionCode(const int &value);

    // Q_INVOKABLE void sendCommand();

    // 用于qml界面点击事件触发时调用
    Q_INVOKABLE void runFunction();

    void defaultFunction();
    virtual void rewriteFunction();

    // Q_INVOKABLE void open_serial();
    // Q_INVOKABLE void paw_init();
    // Q_INVOKABLE void paw_set_clamp_position(float); // 设置夹持位置 mm
    // Q_INVOKABLE void paw_close(); // 闭合夹爪
    // Q_INVOKABLE void paw_set_clamp_speed(float); // 设置夹持速度
    // Q_INVOKABLE void paw_set_clamp_current_intensity(float); // 设置夹持电流

signals:
    void functionNameChange();
    void rewriteFlagChange();
    void functionCodeChange();

public slots:


private:
    QString m_functionName;
    bool m_rewriteFlag;
    int m_functionCode;
    SerialThread * serialThread = nullptr;

};

#endif // SERIALADAPTER_H
