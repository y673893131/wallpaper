#ifndef WIDGET_H
#define WIDGET_H

#include "framelesswidget/framelesswidget.h"
#include <QSystemTrayIcon>
class QGLDisplayWidget;
class Widget : public QFrameLessWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    static Widget* instance();
    void setCenter(int x, int y);
    ~Widget();
signals:
    void setUrl(const QString&);
private:
    void init();
    void initTray();
private:
    QSystemTrayIcon* m_tray;
    QGLDisplayWidget* m_glwidget;
    static Widget* m_this;
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};
#endif // WIDGET_H
