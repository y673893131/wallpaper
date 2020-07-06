#include "widget.h"
#include <Windows.h>
#include <QDebug>
#include <QAction>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QMenu>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QActionGroup>
#include <QTimer>
#include "qgldisplaywidget.h"

HHOOK mouseHook=NULL;
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    if(nCode == HC_ACTION)
    {
       if(wParam == WM_LBUTTONDOWN)
       {
           POINT pt;
           ::GetCursorPos(&pt);
           Widget::instance()->setCenter(pt.x, pt.y);
       }
    }

    return CallNextHookEx(mouseHook,nCode,wParam,lParam);
}

HWND _workerw = nullptr;
inline BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
{
    HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
    if (defview != nullptr)
    {
        _workerw = FindWindowEx(0, tophandle, L"WorkerW", 0);
    }
    return true;
}

HWND getworkWnd(){
    int result;
    HWND windowHandle = FindWindow(L"Progman", nullptr);
    //使用 0x3e8 命令分割出两个 WorkerW
    SendMessageTimeout(windowHandle, 0x052c, 0 ,0, SMTO_NORMAL, 0x3e8,(PDWORD_PTR)&result);
    //遍历窗体获得窗口句柄
    EnumWindows(EnumWindowsProc,(LPARAM)nullptr);
    // 显示WorkerW
    ShowWindow(_workerw,SW_HIDE);
    return windowHandle;
}

Widget* Widget::m_this=nullptr;
Widget::Widget(QWidget *parent)
    : QFrameLessWidget(parent),m_tray(nullptr)
{
    auto work = getworkWnd();
    SetParent((HWND)this->winId(), work);
    m_glwidget = new QGLDisplayWidget(this);
    resize(qApp->desktop()->size());
    move(0, 0);
    init();

    m_this = this;
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(NULL),NULL);//注册鼠标钩子
}

Widget *Widget::instance()
{
    return m_this;
}

void Widget::setCenter(int x, int y)
{
    emit m_glwidget->flushPoint(QPoint(x, y));
}

Widget::~Widget()
{
    if(m_tray) m_tray->hide();
    if(mouseHook) UnhookWindowsHookEx(mouseHook);
}

#include "QDataModel.h"

void Widget::init()
{
    initTray();

    connect(this, &Widget::setUrl, [this](const QString& file){
        QImage image(file);
        //this->setBackgroundImg(image);
        emit m_glwidget->modifyFile(file);
        update();
    });
}

#include <QWidgetAction>
#include <QSlider>
void Widget::initTray()
{
    auto action_open = new QAction(this);
    auto action_add = new QAction(this);
    auto action_quit = new QAction(this);
    auto action_slider = new QWidgetAction(this);
    action_open->setCheckable(true);
    action_open->setChecked(true);
    action_open->setText(tr("open"));
    action_add->setText(tr("add..."));
    action_quit->setText(tr("quit"));
    action_slider->setText(tr("speed"));

    auto pActionSpeed = new QSlider(Qt::Orientation::Horizontal, this);
    pActionSpeed->setObjectName("slider");
    action_slider->setDefaultWidget(pActionSpeed);
    pActionSpeed->setRange(0, 1000);
    connect(pActionSpeed, &QSlider::valueChanged,  m_glwidget, &QGLDisplayWidget::setSpeed);

    m_tray = new QSystemTrayIcon(QApplication::desktop());
    m_tray->setParent(this);
    m_tray->setObjectName("tary1");
    m_tray->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    m_tray->setToolTip(tr(""));
    m_tray->show();
    QMenu *m_tray_menu = new QMenu();
    m_tray_menu->setObjectName("menu1");
    m_tray_menu->addAction(action_open);
    auto current_menu = m_tray_menu->addMenu(tr("current"));
    m_tray_menu->addAction(action_quit);
    m_tray_menu->addAction(action_slider);
    m_tray->setContextMenu(m_tray_menu);

    auto group = new QActionGroup(this);
    auto model = new QDataModel(this);
    connect(this, &Widget::setUrl, model, &QDataModel::onAddUrl);
    connect(model, &QDataModel::loadsuccess, [current_menu, action_add, group, this](const QVector<QStringList>& data){
        QString selectText;
        auto select_action = group->checkedAction();
        if(select_action)
            selectText = select_action->text();
        for (auto it : current_menu->actions()) {
//            it->setIcon(QIcon());
            group->removeAction(it);
        }

        current_menu->clear();

        for (auto iter = data.begin(); iter != data.end(); ++iter) {
            auto name = iter->at(0);
            auto url = iter->at(1);
            auto icon = QIcon(url);
            auto ac = new QAction(this);
            ac->setText(name);
//            ac->setIcon(icon);
            ac->setCheckable(true);
            if(selectText == name) ac->setChecked(true);
            group->addAction(ac);
            ac->setData(url);
            current_menu->addAction(ac);
        }

        current_menu->addAction(action_add);
    });

    connect(current_menu, &QMenu::triggered, [this, group](QAction *action){
        auto icon = action->data().toString();
        if(icon.isEmpty()) return ;
        qDebug() << "set url:" << icon << group->checkedAction()->text();
        emit setUrl(icon);
    });

    connect(action_open, &QAction::triggered, this, &Widget::setVisible);
    connect(action_quit, &QAction::triggered, [](){
        qApp->quit();
    });

    connect(action_add, &QAction::triggered, [this](){
        auto url = QFileDialog::getOpenFileUrl(nullptr, QString(),
                                    QUrl(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),
                                    tr("glsl (*.fsh)"));
        if(url.isLocalFile())
            emit setUrl(url.toLocalFile());
    });

    model->load();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    __super::resizeEvent(event);
    m_glwidget->resize(size());
}

#include <QKeyEvent>
void Widget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    __super::keyPressEvent(event);
}
