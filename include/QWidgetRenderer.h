#ifndef Q_WIDGET_RENDERER_H
#define Q_WIDGET_RENDERER_H

#include <QQuickPaintedItem>
#include <QPushButton>
#include <QPainter>


class WhiteBackgroundWidget : public QWidget 
{
public:
    WhiteBackgroundWidget();

protected:
    void paintEvent(QPaintEvent *) override;
};


class QWidgetRenderer : public QQuickPaintedItem 
{
    Q_OBJECT

public:
    QWidgetRenderer(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

private:
    QPushButton* button;
    WhiteBackgroundWidget * window;
};



#endif