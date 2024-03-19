#include "serialadapter.h"

SerialAdapter::SerialAdapter(QQuickItem * parent): QQuickItem(parent)
{

}

// qml属性functionName的相关设置
QString SerialAdapter::functionName() const 
{
    return this->m_functionName;
}
void SerialAdapter::setFunctionName(const QString &text)
{
    if (m_functionName == text) {
        return;
    }
    this->m_functionName = text;
    emit functionNameChange();
}

// qml属性rewriteFlag的相关设置
bool SerialAdapter::rewriteFlag() const
{
    return this->m_rewriteFlag;
}
void SerialAdapter::setRewriteFlag(const bool &flag)
{
    if(m_rewriteFlag == flag) {
        return;
    }
    this->m_rewriteFlag = flag;
    emit rewriteFlagChange();
}

// 控制执行不同默认功能的qml属性
int SerialAdapter::functionCode() const
{
    return this->m_functionCode;
}
void SerialAdapter::setFunctionCode(const int &value)
{
    if (this->m_functionCode == value) {
        return;
    }
    this->m_functionCode = value;
    emit functionCodeChange();
}

void SerialAdapter::runFunction()
{
    qDebug() << "SerialAdapter run function\n";
    if (this->m_rewriteFlag) {
        rewriteFunction();
    }
    else {
        defaultFunction();
    }
}

void SerialAdapter::defaultFunction()
{
    
}

void SerialAdapter::rewriteFunction()
{

}
