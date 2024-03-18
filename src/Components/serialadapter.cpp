#include "serialadapter.h"

SerialAdapter::SerialAdapter(QQuickItem * parent): QQuickItem(parent)
{

}

QString SerialAdapter::functionName() const 
{
    return m_functionName;
}

void SerialAdapter::setFunctionName(const QString &text)
{
    if (m_functionName == text) {
        return;
    }
    this->m_functionName = text;
    emit functionNameChange();
}

void SerialAdapter::sendCommand()
{
    
}