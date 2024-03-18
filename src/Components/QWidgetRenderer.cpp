# include "QWidgetRenderer.h"


WhiteBackgroundWidget::WhiteBackgroundWidget()
{
    // 在构造函数中创建按钮，并设置其父对象为当前窗口
    QPushButton *button = new QPushButton("Press Me", this);
    connect(button, &QPushButton::clicked, this, []() {
        qDebug() << "QPushButton clicked";
    });
    button->move(100, 130); // 移动按钮到合适的位置
}

void WhiteBackgroundWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white); // 使用白色填充整个窗口矩形区域
}

// ************************************************

QWidgetRenderer::QWidgetRenderer(QQuickItem * parent) : QQuickPaintedItem(parent) 
{
    // 创建QWidget，这里以QPushButton为例
    this->window = new WhiteBackgroundWidget();
    this->window->resize(400, 300);
}

void QWidgetRenderer::paint(QPainter *painter) {
    // 将QWidget渲染到QQuickPaintedItem上
    QPixmap pixmap(this->window->size());
    this->window->render(&pixmap);
    painter->drawPixmap(0, 0, pixmap);
}
