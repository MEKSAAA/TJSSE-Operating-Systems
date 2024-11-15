#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <cmath>
#include "constant.h"
#include "container.h"
#include "string.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Elevator; }
QT_END_NAMESPACE

class ElevatorThread : public QThread
{
    Q_OBJECT

public:
    explicit ElevatorThread(Container *elevator, QObject *parent = nullptr)
        : QThread(parent), _elevator(elevator)
    {
        _updateTimer = new QTimer(this);
        connect(_updateTimer, &QTimer::timeout, this, &ElevatorThread::updateElevator);
        _updateTimer->start(1000); // 每秒更新一次
    }

signals:
    void elevatorUpdated();

protected:
    void run() override
    {
        exec(); // 运行事件循环
    }

private slots:
    void updateElevator()
    {
        // 电梯状态更新和调度逻辑
        if (_elevator->getStatus() == FREE)
        {        }
        emit elevatorUpdated(); // 发送电梯状态更新信号
    }

private:
    Container *_elevator;
    QTimer *_updateTimer;
};


class Elevator : public QMainWindow
{
    Q_OBJECT

private:
    Ui::Elevator *ui;
    bool _isDamage[ELEVATOR_NUM]{false};     //表示5部电梯的损坏情况
    Container* _elevator[ELEVATOR_NUM];      //5部电梯
    bool _upWaitFloors[MAX_FLOORS];          //上升等待队列
    bool _downWaitFloors[MAX_FLOORS];        //下降等待队列
    QTimer *realTime;    //用于检查
    QTimer *colorTime;   //调整按钮颜色
    QTimer *wakeUp;      //唤醒电梯
    QTimer *updateElevatorOne;   //更新1号电梯
    QTimer *updateElevatorTwo;   //更新2号电梯
    QTimer *updateElevatorThree; //更新3号电梯
    QTimer *updateElevatorFour;  //更新4号电梯
    QTimer *updateElevatorFive;  //更新5号电梯
    QTimer *closeDoors;  //定时关门

public:
    Elevator(QWidget *parent = nullptr);
    ~Elevator();
    void openDoor(int i);  //电梯开门动画
    void closeDoor(int i);  //电梯关门动画


private slots:
    //定时器槽函数
    void checkState();
    void checkColor();
    void updateWake();
    void updateEle1();
    void updateEle2();
    void updateEle3();
    void updateELe4();
    void updateEle5();
    void updateDoors();

    //按下报警键槽函数
    void on_ele1AertButton_clicked();
    void on_ele2AlertButton_clicked();
    void on_ele3AlertButton_clicked();
    void on_ele4AlertButton_clicked();
    void on_ele5AlertButton_clicked();

    //1号电梯按钮函数
    void on_ele1F1Button_clicked();
    void on_ele1F2Button_clicked();
    void on_ele1F3Button_clicked();
    void on_ele1F4Button_clicked();
    void on_ele1F5Button_clicked();
    void on_ele1F6Button_clicked();
    void on_ele1F7Button_clicked();
    void on_ele1F8Button_clicked();
    void on_ele1F9Button_clicked();
    void on_ele1F10Button_clicked();
    void on_ele1F11Button_clicked();
    void on_ele1F12Button_clicked();
    void on_ele1F13Button_clicked();
    void on_ele1F14Button_clicked();
    void on_ele1F15Button_clicked();
    void on_ele1F16Button_clicked();
    void on_ele1F17Button_clicked();
    void on_ele1F18Button_clicked();
    void on_ele1F19Button_clicked();
    void on_ele1F20Button_clicked();
    void on_ele1OpenButton_clicked();
    void on_ele1CloseButton_clicked();

    //2号电梯按钮函数
    void on_ele2F1Button_clicked();
    void on_ele2F2Button_clicked();
    void on_ele2F3Button_clicked();
    void on_ele2F4Button_clicked();
    void on_ele2F5Button_clicked();
    void on_ele2F6Button_clicked();
    void on_ele2F7Button_clicked();
    void on_ele2F8Button_clicked();
    void on_ele2F9Button_clicked();
    void on_ele2F10Button_clicked();
    void on_ele2F11Button_clicked();
    void on_ele2F12Button_clicked();
    void on_ele2F13Button_clicked();
    void on_ele2F14Button_clicked();
    void on_ele2F15Button_clicked();
    void on_ele2F16Button_clicked();
    void on_ele2F17Button_clicked();
    void on_ele2F18Button_clicked();
    void on_ele2F19Button_clicked();
    void on_ele2F20Button_clicked();
    void on_ele2OpenButton_clicked();
    void on_ele2CloseButton_clicked();

    //3号电梯按钮函数
    void on_ele3F1Button_clicked();
    void on_ele3F2Button_clicked();
    void on_ele3F3Button_clicked();
    void on_ele3F4Button_clicked();
    void on_ele3F5Button_clicked();
    void on_ele3F6Button_clicked();
    void on_ele3F7Button_clicked();
    void on_ele3F8Button_clicked();
    void on_ele3F9Button_clicked();
    void on_ele3F10Button_clicked();
    void on_ele3F11Button_clicked();
    void on_ele3F12Button_clicked();
    void on_ele3F13Button_clicked();
    void on_ele3F14Button_clicked();
    void on_ele3F15Button_clicked();
    void on_ele3F16Button_clicked();
    void on_ele3F17Button_clicked();
    void on_ele3F18Button_clicked();
    void on_ele3F19Button_clicked();
    void on_ele3F20Button_clicked();
    void on_ele3OpenButton_clicked();
    void on_ele3CloseButton_clicked();

    //4号电梯按钮函数
    void on_ele4F1Button_clicked();
    void on_ele4F2Button_clicked();
    void on_ele4F3Button_clicked();
    void on_ele4F4Button_clicked();
    void on_ele4F5Button_clicked();
    void on_ele4F6Button_clicked();
    void on_ele4F7Button_clicked();
    void on_ele4F8Button_clicked();
    void on_ele4F9Button_clicked();
    void on_ele4F10Button_clicked();
    void on_ele4F11Button_clicked();
    void on_ele4F12Button_clicked();
    void on_ele4F13Button_clicked();
    void on_ele4F14Button_clicked();
    void on_ele4F15Button_clicked();
    void on_ele4F16Button_clicked();
    void on_ele4F17Button_clicked();
    void on_ele4F18Button_clicked();
    void on_ele4F19Button_clicked();
    void on_ele4F20Button_clicked();
    void on_ele4OpenButton_clicked();
    void on_ele4CloseButton_clicked();

    //5号电梯按钮函数
    void on_ele5F1Button_clicked();
    void on_ele5F2Button_clicked();
    void on_ele5F3Button_clicked();
    void on_ele5F4Button_clicked();
    void on_ele5F5Button_clicked();
    void on_ele5F6Button_clicked();
    void on_ele5F7Button_clicked();
    void on_ele5F8Button_clicked();
    void on_ele5F9Button_clicked();
    void on_ele5F10Button_clicked();
    void on_ele5F11Button_clicked();
    void on_ele5F12Button_clicked();
    void on_ele5F13Button_clicked();
    void on_ele5F14Button_clicked();
    void on_ele5F15Button_clicked();
    void on_ele5F16Button_clicked();
    void on_ele5F17Button_clicked();
    void on_ele5F18Button_clicked();
    void on_ele5F19Button_clicked();
    void on_ele5F20Button_clicked();
    void on_ele5OpenButton_clicked();
    void on_ele5CloseButton_clicked();

    //楼外按钮函数
    void on_f1UpButton_clicked();
    void on_f2UpButton_clicked();
    void on_f2DownButton_clicked();
    void on_f3UpButton_clicked();
    void on_f3DownButton_clicked();
    void on_f4UpButton_clicked();
    void on_f4DownButton_clicked();
    void on_f5UpButton_clicked();
    void on_f5DownButton_clicked();
    void on_f6UpButton_clicked();
    void on_f6DownButton_clicked();
    void on_f7UpButton_clicked();
    void on_f7DownButton_clicked();
    void on_f8UpButton_clicked();
    void on_f8DownButton_clicked();
    void on_f9UpButton_clicked();
    void on_f9DownButton_clicked();
    void on_f10UpButton_clicked();
    void on_f10DownButton_clicked();
    void on_f11UpButton_clicked();
    void on_f11DownButton_clicked();
    void on_f12UpButton_clicked();
    void on_f12DownButton_clicked();
    void on_f13UpButton_clicked();
    void on_f13DownButton_clicked();
    void on_f14UpButton_clicked();
    void on_f14DownButton_clicked();
    void on_f15UpButton_clicked();
    void on_f15DownButton_clicked();
    void on_f16UpButton_clicked();
    void on_f16DownButton_clicked();
    void on_f17UpButton_clicked();
    void on_f17DownButton_clicked();
    void on_f18UpButton_clicked();
    void on_f18DownButton_clicked();
    void on_f19UpButton_clicked();
    void on_f19DownButton_clicked();
    void on_f20DownButton_clicked();
};

#endif // ELEVATOR_H
