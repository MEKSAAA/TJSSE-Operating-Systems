#include "elevator.h"
#include "ui_elevator.h"
#include "constant.h"

/*与Elevator有关的公有函数*/

//Elevator构造函数
Elevator::Elevator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Elevator)
{
    //初始化ui
    ui->setupUi(this);

    //初始化显示屏,最开始都在1层
    ui->lcdOne->display(1);
    ui->lcdOne->setPalette(Qt::red);
    ui->lcdTwo->display(1);
    ui->lcdTwo->setPalette(Qt::red);
    ui->lcdThree->display(1);
    ui->lcdThree->setPalette(Qt::red);
    ui->lcdFour->display(1);
    ui->lcdFour->setPalette(Qt::red);
    ui->lcdFive->display(1);
    ui->lcdFive->setPalette(Qt::red);

    //初始化电梯
    for(int i=0;i<ELEVATOR_NUM;i++)
    {
        _elevator[i]=new Container();
    }

    //初始化上下等待电梯数组
    memset(_upWaitFloors,false,sizeof(_upWaitFloors));
    memset(_downWaitFloors,false,sizeof(_upWaitFloors));


    //设置定时器
    //realTime计时器
    realTime = new QTimer();
    connect(realTime,SIGNAL(timeout()),this,SLOT(checkState()));
    realTime->setTimerType(Qt::PreciseTimer);
    realTime->start(100);

    //颜色计时器
    colorTime=new QTimer();
    connect(colorTime,SIGNAL(timeout()),this,SLOT(checkColor()));
    colorTime->setTimerType(Qt::PreciseTimer);
    colorTime->start(100);

    //唤醒电梯
    wakeUp=new QTimer();
    connect(wakeUp,SIGNAL(timeout()),this,SLOT(updateWake()));
    wakeUp->setTimerType(Qt::PreciseTimer);
    wakeUp->start(100);

    //电梯计时器
    updateElevatorOne=new QTimer();
    connect(updateElevatorOne,SIGNAL(timeout()),this,SLOT(updateEle1()));
    updateElevatorOne->setTimerType(Qt::PreciseTimer);
    updateElevatorOne->start(850);

    updateElevatorTwo=new QTimer();
    connect(updateElevatorTwo,SIGNAL(timeout()),this,SLOT(updateEle2()));
    updateElevatorTwo->setTimerType(Qt::PreciseTimer);
    updateElevatorTwo->start(850);

    updateElevatorThree=new QTimer();
    connect(updateElevatorThree,SIGNAL(timeout()),this,SLOT(updateEle3()));
    updateElevatorThree->setTimerType(Qt::PreciseTimer);
    updateElevatorThree->start(850);

    updateElevatorFour=new QTimer();
    connect(updateElevatorFour,SIGNAL(timeout()),this,SLOT(updateELe4()));
    updateElevatorFour->setTimerType(Qt::PreciseTimer);
    updateElevatorFour->start(850);

    updateElevatorFive=new QTimer();
    connect(updateElevatorFive,SIGNAL(timeout()),this,SLOT(updateEle5()));
    updateElevatorFive->setTimerType(Qt::PreciseTimer);
    updateElevatorFive->start(850);

    //关门计时器
    closeDoors=new QTimer();
    connect(closeDoors,SIGNAL(timeout()),this,SLOT(updateDoors()));
    closeDoors->setTimerType(Qt::PreciseTimer);
    closeDoors->start(4000);

}

//Elevator析构函数
Elevator::~Elevator()
{
    //释放ui对象
    delete ui;

    //释放电梯对象
    for(int i=0;i<ELEVATOR_NUM;i++)
    {
        delete _elevator[i];
    }

    //释放定时器
    delete realTime;
    delete colorTime;
    delete wakeUp;
    delete updateElevatorOne;
    delete updateElevatorTwo;
    delete updateElevatorThree;
    delete updateElevatorFour;
    delete updateElevatorFive;
    delete closeDoors;
}

//电梯开门动画
void Elevator::openDoor(int i)
{
    int xLeft;
    int xRight;
    if(i==0)
    {
        xLeft=ui->ele1LeftDoor->x();
        xRight=ui->ele1RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele1LeftDoor->move(xLeft,ui->ele1RightDoor->y());
        ui->ele1RightDoor->move(xRight,ui->ele1RightDoor->y());
        _elevator[0]->setCloseDoor(false);
    }
    else if(i==1)
    {
        xLeft=ui->ele2LeftDoor->x();
        xRight=ui->ele2RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele2LeftDoor->move(xLeft,ui->ele2RightDoor->y());
        ui->ele2RightDoor->move(xRight,ui->ele2RightDoor->y());
        _elevator[1]->setCloseDoor(false);
    }
    else if(i==2)
    {
        xLeft=ui->ele3LeftDoor->x();
        xRight=ui->ele3RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele3LeftDoor->move(xLeft,ui->ele3RightDoor->y());
        ui->ele3RightDoor->move(xRight,ui->ele3RightDoor->y());
        _elevator[2]->setCloseDoor(false);
    }
    else if(i==3)
    {
        xLeft=ui->ele4LeftDoor->x();
        xRight=ui->ele4RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele4LeftDoor->move(xLeft,ui->ele4RightDoor->y());
        ui->ele4RightDoor->move(xRight,ui->ele4RightDoor->y());
        _elevator[3]->setCloseDoor(false);
    }
    else if(i==4)
    {
        xLeft=ui->ele5LeftDoor->x();
        xRight=ui->ele5RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele5LeftDoor->move(xLeft,ui->ele5RightDoor->y());
        ui->ele5RightDoor->move(xRight,ui->ele5RightDoor->y());
        _elevator[4]->setCloseDoor(false);
    }
}

//电梯关门动画
void Elevator::closeDoor(int i)
{
    //电梯故障则无法关门
    if(_isDamage[i])
    {
        return ;
    }

    int xLeft;
    int xRight;

    if(i==0)
    {
        xLeft=ui->ele1LeftDoor->x();
        xRight=ui->ele1RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele1LeftDoor->move(xLeft,ui->ele1RightDoor->y());
        ui->ele1RightDoor->move(xRight,ui->ele1RightDoor->y());
        _elevator[0]->setCloseDoor(true);
    }
    else if(i==1)
    {
        xLeft=ui->ele2LeftDoor->x();
        xRight=ui->ele2RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele2LeftDoor->move(xLeft,ui->ele2RightDoor->y());
        ui->ele2RightDoor->move(xRight,ui->ele2RightDoor->y());
        _elevator[1]->setCloseDoor(true);
    }
    else if(i==2)
    {
        xLeft=ui->ele3LeftDoor->x();
        xRight=ui->ele3RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele3LeftDoor->move(xLeft,ui->ele3RightDoor->y());
        ui->ele3RightDoor->move(xRight,ui->ele3RightDoor->y());
        _elevator[2]->setCloseDoor(true);
    }
    else if(i==3)
    {
        xLeft=ui->ele4LeftDoor->x();
        xRight=ui->ele4RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele4LeftDoor->move(xLeft,ui->ele4RightDoor->y());
        ui->ele4RightDoor->move(xRight,ui->ele4RightDoor->y());
        _elevator[3]->setCloseDoor(true);
    }
    else if(i==4)
    {
        xLeft=ui->ele5LeftDoor->x();
        xRight=ui->ele5RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele5LeftDoor->move(xLeft,ui->ele5RightDoor->y());
        ui->ele5RightDoor->move(xRight,ui->ele5RightDoor->y());
        _elevator[4]->setCloseDoor(true);
    }
}


/*槽函数相关*/
//唤醒空闲电梯（电梯调度算法）
void Elevator::updateWake()
{
    for(int i=0;i<ELEVATOR_NUM;i++)
    {
        if(_isDamage[i])
            continue;

       //电梯内部唤醒
        if(_elevator[i]->getStatus()==FREE)
        {
            //电梯内唤醒上行
            for(int j=_elevator[i]->getFloor()+1;j<MAX_FLOORS;j++)
            {
                if(_elevator[i]->checkFloor(j))
                {
                   _elevator[i]->setStatus(UP);
                    break;
                }
            }
        }

        if(_elevator[i]->getStatus()==FREE)
        {
            //电梯内唤醒下行
            for(int j=1;j<_elevator[i]->getFloor();j++)
            {
                if(_elevator[i]->checkFloor(j))
                {
                   _elevator[i]->setStatus(DOWN);
                    break;
                }
            }
        }

        //外部同层楼唤醒
        if(_elevator[i]->getStatus()==FREE)
        {
            //楼外同层上行唤醒
            if(_upWaitFloors[_elevator[i]->getFloor()])
            {
               _elevator[i]->setStatus(UP);
               break;
            }
        }

        if(_elevator[i]->getStatus()==FREE)
        {
            //楼外同层下行唤醒
            if(_downWaitFloors[_elevator[i]->getFloor()])
            {
               _elevator[i]->setStatus(DOWN);
               break;
            }
        }
        if(_elevator[i]->getStatus()==FREE)
        {
            //同层内部唤醒，默认是上行
            if(_elevator[i]->checkFloor(_elevator[i]->getFloor()))
            {
                _elevator[i]->setStatus(UP);
                break;
            }
        }
    }

    //外部非同层唤醒：根据外部请求，寻找最近的空闲或适合方向的电梯
    //楼外上行唤醒
    for(int j=1;j<MAX_FLOORS;j++)
    {
        int nearestElevator=0;
        int minFloor=INT_MAX;
        bool  flag=false; //是否需要换向
        bool tag=false;
        if(_upWaitFloors[j])  //当前有楼层等待
        {
            for(int k=0;k<ELEVATOR_NUM;k++)
            {
                if(_isDamage[k])//如果电梯已经损坏
                {
                    continue;
                }
                if((_elevator[k]->getStatus()==UP)&&(_elevator[k]->getFloor()<j))//上行并且比当前楼层低
                {
                    if(j-_elevator[k]->getFloor()<minFloor)
                    {
                        minFloor=j-_elevator[k]->getFloor();
                        nearestElevator=k;
                        flag=false;
                    }
                }
                else if((_elevator[k]->getStatus()==FREE))
                {
                    if(abs(j-_elevator[k]->getFloor())<minFloor)
                    {
                        flag=(j>_elevator[k]->getFloor())?false:true;
                        minFloor=abs(j-_elevator[k]->getFloor());
                        nearestElevator=k;
                    }
                }
                if(_elevator[k]->getExtend())
                {
                    tag=true;
                }
            }

           if(flag)
           {
               if(!tag)
               {
                   _elevator[nearestElevator]->setExtend(true);
                   _elevator[nearestElevator]->setStatus(DOWN);
               }

           }
           else
           {
            if(_elevator[nearestElevator]->getStatus()!=DOWN)
             _elevator[nearestElevator]->setStatus(UP);
           }
           break;
        }
    }

    //楼外下行唤醒
    for(int j=1;j<MAX_FLOORS;j++)
    {
        int nearestElevator=0;
        int minFloor=INT_MAX;
        bool flag=false;  //是否需要换向
        bool tag=false;   //是否已经有电梯换向

       if(_downWaitFloors[j])  //当前有楼层等待
       {
          for(int k=0;k<ELEVATOR_NUM;k++)
          {
              if(_isDamage[k])
              {
                  continue;
              }
              if((_elevator[k]->getStatus()==DOWN)&&(_elevator[k]->getFloor()>j))
              {
                   if(_elevator[k]->getFloor()-j<minFloor)
                   {
                       minFloor=_elevator[k]->getFloor()-j;
                       nearestElevator=k;
                       flag=false;
                    }
              }
              if((_elevator[k]->getStatus()==FREE))
              {
                 if(abs(_elevator[k]->getFloor()-j)<minFloor)
                 {
                    flag=(_elevator[k]->getFloor()>j)?false:true;
                    minFloor=abs(_elevator[k]->getFloor()-j);
                    nearestElevator=k;
                  }
              }
              if(_elevator[k]->getExtend())
              {
                  tag=true;
              }
           }
          if(flag)
          {
              if(!tag)
              {
                  _elevator[nearestElevator]->setExtend(true);
                  _elevator[nearestElevator]->setStatus(UP);
              }
           }
          else
          {
              if(_elevator[nearestElevator]->getStatus()!=UP)
              _elevator[nearestElevator]->setStatus(DOWN);
          }
          break;
       }
     }

}

//更新电梯1
void Elevator::updateEle1()
{
    //若电梯损坏，则直接返回
    if(_isDamage[0])
        return;
    //若电梯门未关闭，则直接返回
    if(!_elevator[0]->getCloseDoor())
    {
        return;
    }
    else
    {
        int floor=_elevator[0]->getFloor();
        int flag=true;             //是否需要切换运行状态
        if(_elevator[0]->getStatus()==UP)//当前运行是上升状态
        {
            for(int i=_elevator[0]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_elevator[0]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[0]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_upWaitFloors[i])
                {
                    flag=false;
                    break;
                }

            }
            if(_elevator[0]->getExtend())
            {
                for(int i=_elevator[0]->getFloor()+1;i<MAX_FLOORS;i++)
                {
                    if(_downWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }
                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[0]->setStatus(FREE);
                _elevator[0]->setExtend(false);
                return;
            }
            else
            {
                floor=min(floor+1,FLOORS);
                _elevator[0]->setFloor(floor);
                 ui->lcdOne->display(floor);
                if(floor==FLOORS)
                {
                    _elevator[0]->setStatus(DOWN); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele1RightDoor->y();
               nowY-=35;
               ui->ele1LeftDoor->move(ui->ele1LeftDoor->x(),nowY);
               ui->ele1RightDoor->move(ui->ele1RightDoor->x(),nowY);

            }
        }
        else if(_elevator[0]->getStatus()==DOWN)//当前是下降状态
        {
            for(int i=_elevator[0]->getFloor()-1;i>0;i--)
            {
                if(_elevator[0]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[0]->getFloor()-1;i>0;i--)
            {
                if(_downWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[0]->getExtend())
            {
                for(int i=_elevator[0]->getFloor()-1;i>0;i--)
                {
                    if(_upWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }

                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[0]->setStatus(FREE);
                _elevator[0]->setExtend(false);
                return;
            }
            else
            {
                floor=max(floor-1,1);
                _elevator[0]->setFloor(floor);
                 ui->lcdOne->display(floor);
                if(floor==1)
                {
                    _elevator[0]->setStatus(UP); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele1RightDoor->y();
               nowY+=35;
               ui->ele1LeftDoor->move(ui->ele1LeftDoor->x(),nowY);
               ui->ele1RightDoor->move(ui->ele1RightDoor->x(),nowY);

            }
        }
        //空闲状态 无事发生
    }
}

//更新电梯2
void Elevator::updateEle2()
{
    if(_isDamage[1])
        return;
    if(!_elevator[1]->getCloseDoor())
    {
        return;
    }
    else
    {
        int floor=_elevator[1]->getFloor();
        int flag=true;             //是否需要切换运行状态
        if(_elevator[1]->getStatus()==UP)//当前运行是上升状态
        {
            for(int i=_elevator[1]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_elevator[1]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[1]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_upWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[1]->getExtend())
            {
                for(int i=_elevator[1]->getFloor()+1;i<MAX_FLOORS;i++)
                {
                    if(_downWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }
                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[1]->setStatus(FREE);
                _elevator[1]->setExtend(false);
                return;
            }
            else
            {
                floor=min(floor+1,FLOORS);
                _elevator[1]->setFloor(floor);
                 ui->lcdTwo->display(floor);
                if(floor==FLOORS)
                {
                    _elevator[1]->setStatus(DOWN); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele2RightDoor->y();
               nowY-=35;
               ui->ele2LeftDoor->move(ui->ele2LeftDoor->x(),nowY);
               ui->ele2RightDoor->move(ui->ele2RightDoor->x(),nowY);
            }
        }
        else if(_elevator[1]->getStatus()==DOWN)//当前是下降状态
        {
            for(int i=_elevator[1]->getFloor()-1;i>0;i--)
            {
                if(_elevator[1]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[1]->getFloor()-1;i>0;i--)
            {
                if(_downWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[1]->getExtend())
            {
                for(int i=_elevator[1]->getFloor()-1;i>0;i--)
                {
                    if(_upWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }

                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[1]->setStatus(FREE);
                _elevator[1]->setExtend(false);
                return;
            }
            else
            {
                floor=max(floor-1,1);
                _elevator[1]->setFloor(floor);
                 ui->lcdTwo->display(floor);
                if(floor==1)
                {
                    _elevator[1]->setStatus(UP); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele2RightDoor->y();
               nowY+=35;
               ui->ele2LeftDoor->move(ui->ele2LeftDoor->x(),nowY);
               ui->ele2RightDoor->move(ui->ele2RightDoor->x(),nowY);

            }
        }
        //空闲状态 无事发生
    }
}

//更新电梯3
void Elevator::updateEle3()
{
    if(_isDamage[2])
        return;
    if(!_elevator[2]->getCloseDoor())
    {
        return;
    }
    else
    {
        int floor=_elevator[2]->getFloor();
        int flag=true;             //是否需要切换运行状态
        if(_elevator[2]->getStatus()==UP)//当前运行是上升状态
        {
            for(int i=_elevator[2]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_elevator[2]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[2]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_upWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[2]->getExtend())
            {
                for(int i=_elevator[2]->getFloor()+1;i<MAX_FLOORS;i++)
                {
                    if(_downWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }
                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[2]->setStatus(FREE);
                _elevator[2]->setExtend(false);
                return;
            }
            else
            {
                floor=min(floor+1,FLOORS);
                _elevator[2]->setFloor(floor);
                 ui->lcdThree->display(floor);
                if(floor==FLOORS)
                {
                    _elevator[2]->setStatus(DOWN); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele3RightDoor->y();
               nowY-=35;
               ui->ele3LeftDoor->move(ui->ele3LeftDoor->x(),nowY);
               ui->ele3RightDoor->move(ui->ele3RightDoor->x(),nowY);
            }
        }
        else if(_elevator[2]->getStatus()==DOWN)//当前是下降状态
        {
            for(int i=_elevator[2]->getFloor()-1;i>0;i--)
            {
                if(_elevator[2]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[2]->getFloor()-1;i>0;i--)
            {
                if(_downWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[2]->getExtend())
            {
                for(int i=_elevator[2]->getFloor()-1;i>0;i--)
                {
                    if(_upWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }

                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[2]->setStatus(FREE);
                _elevator[2]->setExtend(false);
                return;
            }
            else
            {
                floor=max(floor-1,1);
                _elevator[2]->setFloor(floor);
                 ui->lcdThree->display(floor);
                if(floor==1)
                {
                    _elevator[2]->setStatus(UP); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele3RightDoor->y();
               nowY+=35;
               ui->ele3LeftDoor->move(ui->ele3LeftDoor->x(),nowY);
               ui->ele3RightDoor->move(ui->ele3RightDoor->x(),nowY);

            }
        }
        //空闲状态 无事发生
    }
}

//更新电梯4
void Elevator::updateELe4()
{
    if(_isDamage[3])
        return;
    if(!_elevator[3]->getCloseDoor())
    {
        return;
    }
    else
    {
        int floor=_elevator[3]->getFloor();
        int flag=true;             //是否需要切换运行状态
        if(_elevator[3]->getStatus()==UP)//当前运行是上升状态
        {
            for(int i=_elevator[3]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_elevator[3]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[3]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_upWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[3]->getExtend())
            {
                for(int i=_elevator[3]->getFloor()+1;i<MAX_FLOORS;i++)
                {
                    if(_downWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }
                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[3]->setStatus(FREE);
                _elevator[3]->setExtend(false);
                return;
            }
            else
            {
                floor=min(floor+1,FLOORS);
                _elevator[3]->setFloor(floor);
                 ui->lcdFour->display(floor);
                if(floor==FLOORS)
                {
                    _elevator[3]->setStatus(DOWN); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele4RightDoor->y();
               nowY-=35;
               ui->ele4LeftDoor->move(ui->ele4LeftDoor->x(),nowY);
               ui->ele4RightDoor->move(ui->ele4RightDoor->x(),nowY);
            }
        }
        else if(_elevator[3]->getStatus()==DOWN)//当前是下降状态
        {
            for(int i=_elevator[3]->getFloor()-1;i>0;i--)
            {
                if(_elevator[3]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[3]->getFloor()-1;i>0;i--)
            {
                if(_downWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[3]->getExtend())
            {
                for(int i=_elevator[3]->getFloor()-1;i>0;i--)
                {
                    if(_upWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }

                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[3]->setStatus(FREE);
                _elevator[3]->setExtend(false);
                return;
            }
            else
            {
                floor=max(floor-1,1);
                _elevator[3]->setFloor(floor);
                 ui->lcdFour->display(floor);
                if(floor==1)
                {
                    _elevator[3]->setStatus(UP); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele4RightDoor->y();
               nowY+=35;
               ui->ele4LeftDoor->move(ui->ele4LeftDoor->x(),nowY);
               ui->ele4RightDoor->move(ui->ele4RightDoor->x(),nowY);

            }
        }
        //空闲状态 无事发生
    }
}

//更新电梯5
void Elevator::updateEle5()
{
    if(_isDamage[4])
        return;
    if(!_elevator[4]->getCloseDoor())
    {
        return;
    }
    else
    {
        int floor=_elevator[4]->getFloor();
        int flag=true;             //是否需要切换运行状态
        if(_elevator[4]->getStatus()==UP)//当前运行是上升状态
        {
            for(int i=_elevator[4]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_elevator[4]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[4]->getFloor()+1;i<MAX_FLOORS;i++)
            {
                if(_upWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[4]->getExtend())
            {
                for(int i=_elevator[4]->getFloor()+1;i<MAX_FLOORS;i++)
                {
                    if(_downWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }
                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[4]->setStatus(FREE);
                _elevator[4]->setExtend(false);
                return;
            }
            else
            {
                floor=min(floor+1,FLOORS);
                _elevator[4]->setFloor(floor);
                 ui->lcdFive->display(floor);
                if(floor==FLOORS)
                {
                    _elevator[4]->setStatus(DOWN); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele5RightDoor->y();
               nowY-=35;
               ui->ele5LeftDoor->move(ui->ele5LeftDoor->x(),nowY);
               ui->ele5RightDoor->move(ui->ele5RightDoor->x(),nowY);
            }
        }
        else if(_elevator[4]->getStatus()==DOWN)//当前是下降状态
        {
            for(int i=_elevator[4]->getFloor()-1;i>0;i--)
            {
                if(_elevator[4]->checkFloor(i))
                {
                    flag=false;
                    break;
                }
            }
            for(int i=_elevator[4]->getFloor()-1;i>0;i--)
            {
                if(_downWaitFloors[i])
                {
                    flag=false;
                    break;
                }
            }
            if(_elevator[4]->getExtend())
            {
                for(int i=_elevator[4]->getFloor()-1;i>0;i--)
                {
                    if(_upWaitFloors[i])
                    {
                        flag=false;
                        break;
                    }

                }
            }
            if(flag)  //没有等待电梯了
            {
                _elevator[4]->setStatus(FREE);
                _elevator[4]->setExtend(false);
                return;
            }
            else
            {
                floor=max(floor-1,1);
                _elevator[4]->setFloor(floor);
                 ui->lcdFive->display(floor);
                if(floor==1)
                {
                    _elevator[4]->setStatus(UP); //由上升变成下降
                }
                //电梯动画
               int nowY=ui->ele5RightDoor->y();
               nowY+=35;
               ui->ele5LeftDoor->move(ui->ele5LeftDoor->x(),nowY);
               ui->ele5RightDoor->move(ui->ele5RightDoor->x(),nowY);

            }
        }
        //空闲状态 无事发生
    }
}

//定时器槽函数
void Elevator::checkState()
{
    for(int i=0;i<ELEVATOR_NUM;i++)
    {
        if(_isDamage[i])
        {
            continue;
        }
        if(_elevator[i]->getStatus()==UP) //处理上升状态
        {
            if((_upWaitFloors[_elevator[i]->getFloor()])||(_elevator[i]->checkFloor(_elevator[i]->getFloor())))
            {
                if(_elevator[i]->getCloseDoor())//如果这个时候门是关的，就开门
                {
                   openDoor(i);
                }
                _upWaitFloors[_elevator[i]->getFloor()]=false;
                _elevator[i]->clearFloor(_elevator[i]->getFloor());
                _elevator[i]->setCloseDoor(false);
            }
        }
        else if(_elevator[i]->getStatus()==DOWN) //处理下降状态
        {
            if((_downWaitFloors[_elevator[i]->getFloor()])||(_elevator[i]->checkFloor(_elevator[i]->getFloor())))
            {
                if(_elevator[i]->getCloseDoor())//如果这个时候门是关的，就开门
                {
                   openDoor(i);
                }
                _downWaitFloors[_elevator[i]->getFloor()]=false;
                _elevator[i]->clearFloor(_elevator[i]->getFloor());
                _elevator[i]->setCloseDoor(false);
            }
        }
    }
}

//调整电梯按钮颜色
void Elevator::checkColor()
{
    //处理楼外上升按钮
    if(_upWaitFloors[1])
        ui->f1UpButton->setStyleSheet("background-color: red");
    else
        ui->f1UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[2])
        ui->f2UpButton->setStyleSheet("background-color: red");
    else
        ui->f2UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[3])
        ui->f3UpButton->setStyleSheet("background-color: red");
    else
        ui->f3UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[4])
        ui->f4UpButton->setStyleSheet("background-color: red");
    else
        ui->f4UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[5])
        ui->f5UpButton->setStyleSheet("background-color: red");
    else
        ui->f5UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[6])
        ui->f6UpButton->setStyleSheet("background-color: red");
    else
        ui->f6UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[7])
        ui->f7UpButton->setStyleSheet("background-color: red");
    else
        ui->f7UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[8])
        ui->f8UpButton->setStyleSheet("background-color: red");
    else
        ui->f8UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[9])
        ui->f9UpButton->setStyleSheet("background-color: red");
    else
        ui->f9UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[10])
        ui->f10UpButton->setStyleSheet("background-color: red");
    else
        ui->f10UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[11])
        ui->f11UpButton->setStyleSheet("background-color: red");
    else
        ui->f11UpButton->setStyleSheet("background-color:rgb(225,225,225)");
    if(_upWaitFloors[12])
        ui->f12UpButton->setStyleSheet("background-color: red");
    else
        ui->f12UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[13])
        ui->f13UpButton->setStyleSheet("background-color: red");
    else
        ui->f13UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[14])
        ui->f14UpButton->setStyleSheet("background-color: red");
    else
        ui->f14UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[15])
        ui->f15UpButton->setStyleSheet("background-color: red");
    else
        ui->f15UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[16])
        ui->f16UpButton->setStyleSheet("background-color: red");
    else
        ui->f16UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[17])
        ui->f17UpButton->setStyleSheet("background-color: red");
    else
        ui->f17UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[18])
        ui->f18UpButton->setStyleSheet("background-color: red");
    else
        ui->f18UpButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_upWaitFloors[19])
        ui->f19UpButton->setStyleSheet("background-color: red");
    else
        ui->f19UpButton->setStyleSheet("background-color: rgb(225,225,225)");

    //处理楼外下降按钮
    if(_downWaitFloors[2])
        ui->f2DownButton->setStyleSheet("background-color: red");
    else
        ui->f2DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[3])
        ui->f3DownButton->setStyleSheet("background-color: red");
    else
        ui->f3DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[4])
        ui->f4DownButton->setStyleSheet("background-color: red");
    else
        ui->f4DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[5])
        ui->f5DownButton->setStyleSheet("background-color: red");
    else
        ui->f5DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[6])
        ui->f6DownButton->setStyleSheet("background-color: red");
    else
        ui->f6DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[7])
        ui->f7DownButton->setStyleSheet("background-color: red");
    else
        ui->f7DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[8])
        ui->f8DownButton->setStyleSheet("background-color: red");
    else
        ui->f8DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[9])
        ui->f9DownButton->setStyleSheet("background-color: red");
    else
        ui->f9DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[10])
        ui->f10DownButton->setStyleSheet("background-color: red");
    else
        ui->f10DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[11])
        ui->f11DownButton->setStyleSheet("background-color: red");
    else
        ui->f11DownButton->setStyleSheet("background-color:rgb(225,225,225)");
    if(_downWaitFloors[12])
        ui->f12DownButton->setStyleSheet("background-color: red");
    else
        ui->f12DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[13])
        ui->f13DownButton->setStyleSheet("background-color: red");
    else
        ui->f13DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[14])
        ui->f14DownButton->setStyleSheet("background-color: red");
    else
        ui->f14DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[15])
        ui->f15DownButton->setStyleSheet("background-color: red");
    else
        ui->f15DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[16])
        ui->f16DownButton->setStyleSheet("background-color: red");
    else
        ui->f16DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[17])
        ui->f17DownButton->setStyleSheet("background-color: red");
    else
        ui->f17DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[18])
        ui->f18DownButton->setStyleSheet("background-color: red");
    else
        ui->f18DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[19])
        ui->f19DownButton->setStyleSheet("background-color: red");
    else
        ui->f19DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    if(_downWaitFloors[20])
        ui->f20DownButton->setStyleSheet("background-color: red");
    else
        ui->f20DownButton->setStyleSheet("background-color: rgb(225,225,225)");
    //处理一号电梯
    if(_elevator[0]->checkFloor(1))
        ui->ele1F1Button->setStyleSheet("background-color: red");
    else
        ui->ele1F1Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(2))
        ui->ele1F2Button->setStyleSheet("background-color: red");
    else
        ui->ele1F2Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(3))
        ui->ele1F3Button->setStyleSheet("background-color: red");
    else
        ui->ele1F3Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(4))
        ui->ele1F4Button->setStyleSheet("background-color: red");
    else
        ui->ele1F4Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(5))
        ui->ele1F5Button->setStyleSheet("background-color: red");
    else
        ui->ele1F5Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(6))
        ui->ele1F6Button->setStyleSheet("background-color: red");
    else
        ui->ele1F6Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(7))
        ui->ele1F7Button->setStyleSheet("background-color: red");
    else
        ui->ele1F7Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(8))
        ui->ele1F8Button->setStyleSheet("background-color: red");
    else
        ui->ele1F8Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(9))
        ui->ele1F9Button->setStyleSheet("background-color: red");
    else
        ui->ele1F9Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(10))
        ui->ele1F10Button->setStyleSheet("background-color: red");
    else
        ui->ele1F10Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(11))
        ui->ele1F11Button->setStyleSheet("background-color: red");
    else
        ui->ele1F11Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(12))
        ui->ele1F12Button->setStyleSheet("background-color: red");
    else
        ui->ele1F12Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(13))
        ui->ele1F13Button->setStyleSheet("background-color: red");
    else
        ui->ele1F13Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(14))
        ui->ele1F14Button->setStyleSheet("background-color: red");
    else
        ui->ele1F14Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(15))
        ui->ele1F15Button->setStyleSheet("background-color: red");
    else
        ui->ele1F15Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(16))
        ui->ele1F16Button->setStyleSheet("background-color: red");
    else
        ui->ele1F16Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(17))
        ui->ele1F17Button->setStyleSheet("background-color: red");
    else
        ui->ele1F17Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(18))
        ui->ele1F18Button->setStyleSheet("background-color: red");
    else
        ui->ele1F18Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(19))
        ui->ele1F19Button->setStyleSheet("background-color: red");
    else
        ui->ele1F19Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[0]->checkFloor(20))
        ui->ele1F20Button->setStyleSheet("background-color: red");
    else
        ui->ele1F20Button->setStyleSheet("background-color: rgb(225,225,225)");
    //处理二号电梯
    if(_elevator[1]->checkFloor(1))
        ui->ele2F1Button->setStyleSheet("background-color: red");
    else
        ui->ele2F1Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(2))
        ui->ele2F2Button->setStyleSheet("background-color: red");
    else
        ui->ele2F2Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(3))
        ui->ele2F3Button->setStyleSheet("background-color: red");
    else
        ui->ele2F3Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(4))
        ui->ele2F4Button->setStyleSheet("background-color: red");
    else
        ui->ele2F4Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(5))
        ui->ele2F5Button->setStyleSheet("background-color: red");
    else
        ui->ele2F5Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(6))
        ui->ele2F6Button->setStyleSheet("background-color: red");
    else
        ui->ele2F6Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(7))
        ui->ele2F7Button->setStyleSheet("background-color: red");
    else
        ui->ele2F7Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(8))
        ui->ele2F8Button->setStyleSheet("background-color: red");
    else
        ui->ele2F8Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(9))
        ui->ele2F9Button->setStyleSheet("background-color: red");
    else
        ui->ele2F9Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(10))
        ui->ele2F10Button->setStyleSheet("background-color: red");
    else
        ui->ele2F10Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(11))
        ui->ele2F11Button->setStyleSheet("background-color: red");
    else
        ui->ele2F11Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(12))
        ui->ele2F12Button->setStyleSheet("background-color: red");
    else
        ui->ele2F12Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(13))
        ui->ele2F13Button->setStyleSheet("background-color: red");
    else
        ui->ele2F13Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(14))
        ui->ele2F14Button->setStyleSheet("background-color: red");
    else
        ui->ele2F14Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(15))
        ui->ele2F15Button->setStyleSheet("background-color: red");
    else
        ui->ele2F15Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(16))
        ui->ele2F16Button->setStyleSheet("background-color: red");
    else
        ui->ele2F16Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(17))
        ui->ele2F17Button->setStyleSheet("background-color: red");
    else
        ui->ele2F17Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(18))
        ui->ele2F18Button->setStyleSheet("background-color: red");
    else
        ui->ele2F18Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(19))
        ui->ele2F19Button->setStyleSheet("background-color: red");
    else
        ui->ele2F19Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[1]->checkFloor(20))
        ui->ele2F20Button->setStyleSheet("background-color: red");
    else
        ui->ele2F20Button->setStyleSheet("background-color: rgb(225,225,225)");
    //处理三号电梯
    if(_elevator[2]->checkFloor(1))
        ui->ele3F1Button->setStyleSheet("background-color: red");
    else
        ui->ele3F1Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(2))
        ui->ele3F2Button->setStyleSheet("background-color: red");
    else
        ui->ele3F2Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(3))
        ui->ele3F3Button->setStyleSheet("background-color: red");
    else
        ui->ele3F3Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(4))
        ui->ele3F4Button->setStyleSheet("background-color: red");
    else
        ui->ele3F4Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(5))
        ui->ele3F5Button->setStyleSheet("background-color: red");
    else
        ui->ele3F5Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(6))
        ui->ele3F6Button->setStyleSheet("background-color: red");
    else
        ui->ele3F6Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(7))
        ui->ele3F7Button->setStyleSheet("background-color: red");
    else
        ui->ele3F7Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(8))
        ui->ele3F8Button->setStyleSheet("background-color: red");
    else
        ui->ele3F8Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(9))
        ui->ele3F9Button->setStyleSheet("background-color: red");
    else
        ui->ele3F9Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(10))
        ui->ele3F10Button->setStyleSheet("background-color: red");
    else
        ui->ele3F10Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(11))
        ui->ele3F11Button->setStyleSheet("background-color: red");
    else
        ui->ele3F11Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(12))
        ui->ele3F12Button->setStyleSheet("background-color: red");
    else
        ui->ele3F12Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(13))
        ui->ele3F13Button->setStyleSheet("background-color: red");
    else
        ui->ele3F13Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(14))
        ui->ele3F14Button->setStyleSheet("background-color: red");
    else
        ui->ele3F14Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(15))
        ui->ele3F15Button->setStyleSheet("background-color: red");
    else
        ui->ele3F15Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(16))
        ui->ele3F16Button->setStyleSheet("background-color: red");
    else
        ui->ele3F16Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(17))
        ui->ele3F17Button->setStyleSheet("background-color: red");
    else
        ui->ele3F17Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(18))
        ui->ele3F18Button->setStyleSheet("background-color: red");
    else
        ui->ele3F18Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(19))
        ui->ele3F19Button->setStyleSheet("background-color: red");
    else
        ui->ele3F19Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[2]->checkFloor(20))
        ui->ele3F20Button->setStyleSheet("background-color: red");
    else
        ui->ele3F20Button->setStyleSheet("background-color: rgb(225,225,225)");
    //处理四号电梯
    if(_elevator[3]->checkFloor(1))
        ui->ele4F1Button->setStyleSheet("background-color: red");
    else
        ui->ele4F1Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(2))
        ui->ele4F2Button->setStyleSheet("background-color: red");
    else
        ui->ele4F2Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(3))
        ui->ele4F3Button->setStyleSheet("background-color: red");
    else
        ui->ele4F3Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(4))
        ui->ele4F4Button->setStyleSheet("background-color: red");
    else
        ui->ele4F4Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(5))
        ui->ele4F5Button->setStyleSheet("background-color: red");
    else
        ui->ele4F5Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(6))
        ui->ele4F6Button->setStyleSheet("background-color: red");
    else
        ui->ele4F6Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(7))
        ui->ele4F7Button->setStyleSheet("background-color: red");
    else
        ui->ele4F7Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(8))
        ui->ele4F8Button->setStyleSheet("background-color: red");
    else
        ui->ele4F8Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(9))
        ui->ele4F9Button->setStyleSheet("background-color: red");
    else
        ui->ele4F9Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(10))
        ui->ele4F10Button->setStyleSheet("background-color: red");
    else
        ui->ele4F10Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(11))
        ui->ele4F11Button->setStyleSheet("background-color: red");
    else
        ui->ele4F11Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(12))
        ui->ele4F12Button->setStyleSheet("background-color: red");
    else
        ui->ele4F12Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(13))
        ui->ele4F13Button->setStyleSheet("background-color: red");
    else
        ui->ele4F13Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(14))
        ui->ele4F14Button->setStyleSheet("background-color: red");
    else
        ui->ele4F14Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(15))
        ui->ele4F15Button->setStyleSheet("background-color: red");
    else
        ui->ele4F15Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(16))
        ui->ele4F16Button->setStyleSheet("background-color: red");
    else
        ui->ele4F16Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(17))
        ui->ele4F17Button->setStyleSheet("background-color: red");
    else
        ui->ele4F17Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(18))
        ui->ele4F18Button->setStyleSheet("background-color: red");
    else
        ui->ele4F18Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(19))
        ui->ele4F19Button->setStyleSheet("background-color: red");
    else
        ui->ele4F19Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[3]->checkFloor(20))
        ui->ele4F20Button->setStyleSheet("background-color: red");
    else
        ui->ele4F20Button->setStyleSheet("background-color: rgb(225,225,225)");
    //处理五号电梯
    if(_elevator[4]->checkFloor(1))
        ui->ele5F1Button->setStyleSheet("background-color: red");
    else
        ui->ele5F1Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(2))
        ui->ele5F2Button->setStyleSheet("background-color: red");
    else
        ui->ele5F2Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(3))
        ui->ele5F3Button->setStyleSheet("background-color: red");
    else
        ui->ele5F3Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(4))
        ui->ele5F4Button->setStyleSheet("background-color: red");
    else
        ui->ele5F4Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(5))
        ui->ele5F5Button->setStyleSheet("background-color: red");
    else
        ui->ele5F5Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(6))
        ui->ele5F6Button->setStyleSheet("background-color: red");
    else
        ui->ele5F6Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(7))
        ui->ele5F7Button->setStyleSheet("background-color: red");
    else
        ui->ele5F7Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(8))
        ui->ele5F8Button->setStyleSheet("background-color: red");
    else
        ui->ele5F8Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(9))
        ui->ele5F9Button->setStyleSheet("background-color: red");
    else
        ui->ele5F9Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(10))
        ui->ele5F10Button->setStyleSheet("background-color: red");
    else
        ui->ele5F10Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(11))
        ui->ele5F11Button->setStyleSheet("background-color: red");
    else
        ui->ele5F11Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(12))
        ui->ele5F12Button->setStyleSheet("background-color: red");
    else
        ui->ele5F12Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(13))
        ui->ele5F13Button->setStyleSheet("background-color: red");
    else
        ui->ele5F13Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(14))
        ui->ele5F14Button->setStyleSheet("background-color: red");
    else
        ui->ele5F14Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(15))
        ui->ele5F15Button->setStyleSheet("background-color: red");
    else
        ui->ele5F15Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(16))
        ui->ele5F16Button->setStyleSheet("background-color: red");
    else
        ui->ele5F16Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(17))
        ui->ele5F17Button->setStyleSheet("background-color: red");
    else
        ui->ele5F17Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(18))
        ui->ele5F18Button->setStyleSheet("background-color: red");
    else
        ui->ele5F18Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(19))
        ui->ele5F19Button->setStyleSheet("background-color: red");
    else
        ui->ele5F19Button->setStyleSheet("background-color: rgb(225,225,225)");
    if(_elevator[4]->checkFloor(20))
        ui->ele5F20Button->setStyleSheet("background-color: red");
    else
        ui->ele5F20Button->setStyleSheet("background-color: rgb(225,225,225)");
}

//定时关门
void Elevator::updateDoors()
{
    for(int i=0;i<ELEVATOR_NUM;i++)
    {
        //每隔一段时间进行一次检测，若电梯门是开的，则关门
        if(!_elevator[i]->getCloseDoor())
        {
            closeDoor(i);
        }
    }
}

//1号电梯按下报警键
void Elevator::on_ele1AertButton_clicked()
{
    bool flag=_isDamage[ONE];
    _isDamage[ONE]^=1;  //更改电梯状态
    ui->ele1F1Button->setEnabled(flag);
    ui->ele1F2Button->setEnabled(flag);
    ui->ele1F3Button->setEnabled(flag);
    ui->ele1F4Button->setEnabled(flag);
    ui->ele1F5Button->setEnabled(flag);
    ui->ele1F6Button->setEnabled(flag);
    ui->ele1F7Button->setEnabled(flag);
    ui->ele1F8Button->setEnabled(flag);
    ui->ele1F9Button->setEnabled(flag);
    ui->ele1F10Button->setEnabled(flag);
    ui->ele1F11Button->setEnabled(flag);
    ui->ele1F12Button->setEnabled(flag);
    ui->ele1F13Button->setEnabled(flag);
    ui->ele1F14Button->setEnabled(flag);
    ui->ele1F15Button->setEnabled(flag);
    ui->ele1F16Button->setEnabled(flag);
    ui->ele1F17Button->setEnabled(flag);
    ui->ele1F18Button->setEnabled(flag);
    ui->ele1F19Button->setEnabled(flag);
    ui->ele1F20Button->setEnabled(flag);
    ui->ele1OpenButton->setEnabled(flag);
    ui->ele1CloseButton->setEnabled(flag);
}

//2号电梯按下报警键
void Elevator::on_ele2AlertButton_clicked()
{
    bool flag =_isDamage[TWO];
    _isDamage[TWO]^=1;  //更改运行状态
    ui->ele2F1Button->setEnabled(flag);
    ui->ele2F2Button->setEnabled(flag);
    ui->ele2F3Button->setEnabled(flag);
    ui->ele2F4Button->setEnabled(flag);
    ui->ele2F5Button->setEnabled(flag);
    ui->ele2F6Button->setEnabled(flag);
    ui->ele2F7Button->setEnabled(flag);
    ui->ele2F8Button->setEnabled(flag);
    ui->ele2F9Button->setEnabled(flag);
    ui->ele2F10Button->setEnabled(flag);
    ui->ele2F11Button->setEnabled(flag);
    ui->ele2F12Button->setEnabled(flag);
    ui->ele2F13Button->setEnabled(flag);
    ui->ele2F14Button->setEnabled(flag);
    ui->ele2F15Button->setEnabled(flag);
    ui->ele2F16Button->setEnabled(flag);
    ui->ele2F17Button->setEnabled(flag);
    ui->ele2F18Button->setEnabled(flag);
    ui->ele2F19Button->setEnabled(flag);
    ui->ele2F20Button->setEnabled(flag);
    ui->ele2OpenButton->setEnabled(flag);
    ui->ele2CloseButton->setEnabled(flag);
}

//3号电梯按下报警键
void Elevator::on_ele3AlertButton_clicked()
{
    bool flag=_isDamage[THREE];
    _isDamage[THREE]^=1; //更改当前状态
    ui->ele3F1Button->setEnabled(flag);
    ui->ele3F2Button->setEnabled(flag);
    ui->ele3F3Button->setEnabled(flag);
    ui->ele3F4Button->setEnabled(flag);
    ui->ele3F5Button->setEnabled(flag);
    ui->ele3F6Button->setEnabled(flag);
    ui->ele3F7Button->setEnabled(flag);
    ui->ele3F8Button->setEnabled(flag);
    ui->ele3F9Button->setEnabled(flag);
    ui->ele3F10Button->setEnabled(flag);
    ui->ele3F11Button->setEnabled(flag);
    ui->ele3F12Button->setEnabled(flag);
    ui->ele3F13Button->setEnabled(flag);
    ui->ele3F14Button->setEnabled(flag);
    ui->ele3F15Button->setEnabled(flag);
    ui->ele3F16Button->setEnabled(flag);
    ui->ele3F17Button->setEnabled(flag);
    ui->ele3F18Button->setEnabled(flag);
    ui->ele3F19Button->setEnabled(flag);
    ui->ele3F20Button->setEnabled(flag);
    ui->ele3OpenButton->setEnabled(flag);
    ui->ele3CloseButton->setEnabled(flag);
}

//4号电梯按下报警键
void Elevator::on_ele4AlertButton_clicked()
{
    bool flag=_isDamage[FOUR];
    _isDamage[FOUR]^=1;
    ui->ele4F1Button->setEnabled(flag);
    ui->ele4F2Button->setEnabled(flag);
    ui->ele4F3Button->setEnabled(flag);
    ui->ele4F4Button->setEnabled(flag);
    ui->ele4F5Button->setEnabled(flag);
    ui->ele4F6Button->setEnabled(flag);
    ui->ele4F7Button->setEnabled(flag);
    ui->ele4F8Button->setEnabled(flag);
    ui->ele4F9Button->setEnabled(flag);
    ui->ele4F10Button->setEnabled(flag);
    ui->ele4F11Button->setEnabled(flag);
    ui->ele4F12Button->setEnabled(flag);
    ui->ele4F13Button->setEnabled(flag);
    ui->ele4F14Button->setEnabled(flag);
    ui->ele4F15Button->setEnabled(flag);
    ui->ele4F16Button->setEnabled(flag);
    ui->ele4F17Button->setEnabled(flag);
    ui->ele4F18Button->setEnabled(flag);
    ui->ele4F19Button->setEnabled(flag);
    ui->ele4F20Button->setEnabled(flag);
    ui->ele4OpenButton->setEnabled(flag);
    ui->ele4CloseButton->setEnabled(flag);
}

//5号电梯按下报警键
void Elevator::on_ele5AlertButton_clicked()
{
    bool flag=_isDamage[FIVE];
    _isDamage[FIVE]^=1;
    ui->ele5F1Button->setEnabled(flag);
    ui->ele5F2Button->setEnabled(flag);
    ui->ele5F3Button->setEnabled(flag);
    ui->ele5F4Button->setEnabled(flag);
    ui->ele5F5Button->setEnabled(flag);
    ui->ele5F6Button->setEnabled(flag);
    ui->ele5F7Button->setEnabled(flag);
    ui->ele5F8Button->setEnabled(flag);
    ui->ele5F9Button->setEnabled(flag);
    ui->ele5F10Button->setEnabled(flag);
    ui->ele5F11Button->setEnabled(flag);
    ui->ele5F12Button->setEnabled(flag);
    ui->ele5F13Button->setEnabled(flag);
    ui->ele5F14Button->setEnabled(flag);
    ui->ele5F15Button->setEnabled(flag);
    ui->ele5F16Button->setEnabled(flag);
    ui->ele5F17Button->setEnabled(flag);
    ui->ele5F18Button->setEnabled(flag);
    ui->ele5F19Button->setEnabled(flag);
    ui->ele5F20Button->setEnabled(flag);
    ui->ele5OpenButton->setEnabled(flag);
    ui->ele5CloseButton->setEnabled(flag);
}

//1号电梯相关按钮点击函数
void Elevator::on_ele1F1Button_clicked()
{
    _elevator[0]->addFloor(1);
}
void Elevator::on_ele1F2Button_clicked()
{
    _elevator[0]->addFloor(2);
}
void Elevator::on_ele1F3Button_clicked()
{
    _elevator[0]->addFloor(3);
}
void Elevator::on_ele1F4Button_clicked()
{
    _elevator[0]->addFloor(4);
}
void Elevator::on_ele1F5Button_clicked()
{
    _elevator[0]->addFloor(5);
}
void Elevator::on_ele1F6Button_clicked()
{
    _elevator[0]->addFloor(6);
}
void Elevator::on_ele1F7Button_clicked()
{
    _elevator[0]->addFloor(7);
}
void Elevator::on_ele1F8Button_clicked()
{
    _elevator[0]->addFloor(8);
}
void Elevator::on_ele1F9Button_clicked()
{
    _elevator[0]->addFloor(9);
}
void Elevator::on_ele1F10Button_clicked()
{
    _elevator[0]->addFloor(10);
}
void Elevator::on_ele1F11Button_clicked()
{
    _elevator[0]->addFloor(11);
}
void Elevator::on_ele1F12Button_clicked()
{
    _elevator[0]->addFloor(12);
}
void Elevator::on_ele1F13Button_clicked()
{
    _elevator[0]->addFloor(13);
}
void Elevator::on_ele1F14Button_clicked()
{
    _elevator[0]->addFloor(14);
}
void Elevator::on_ele1F15Button_clicked()
{
    _elevator[0]->addFloor(15);
}
void Elevator::on_ele1F16Button_clicked()
{
    _elevator[0]->addFloor(16);
}
void Elevator::on_ele1F17Button_clicked()
{
    _elevator[0]->addFloor(17);
}
void Elevator::on_ele1F18Button_clicked()
{
    _elevator[0]->addFloor(18);
}
void Elevator::on_ele1F19Button_clicked()
{
    _elevator[0]->addFloor(19);
}
void Elevator::on_ele1F20Button_clicked()
{
    _elevator[0]->addFloor(20);
}
void Elevator::on_ele1OpenButton_clicked()
{
    if((_elevator[0]->getStatus()==FREE)&&(_elevator[0]->getCloseDoor()))  //只有当前是空闲且关门才能开电梯
    {
        //开门动画
        int xLeft=ui->ele1LeftDoor->x();
        int xRight=ui->ele1RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele1LeftDoor->move(xLeft,ui->ele1RightDoor->y());
        ui->ele1RightDoor->move(xRight,ui->ele1RightDoor->y());
        _elevator[0]->setCloseDoor(false);
    }
}
void Elevator::on_ele1CloseButton_clicked()
{
    if(!_elevator[0]->getCloseDoor())   //如果当前电梯门是开的才能关门
    {
        //关门动画
        int xLeft=ui->ele1LeftDoor->x();
        int xRight=ui->ele1RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele1LeftDoor->move(xLeft,ui->ele1RightDoor->y());
        ui->ele1RightDoor->move(xRight,ui->ele1RightDoor->y());
        _elevator[0]->setCloseDoor(true);
    }

}

//2号电梯相关按钮点击函数
void Elevator::on_ele2F1Button_clicked()
{
    _elevator[1]->addFloor(1);
}
void Elevator::on_ele2F2Button_clicked()
{
    _elevator[1]->addFloor(2);
}
void Elevator::on_ele2F3Button_clicked()
{
    _elevator[1]->addFloor(3);
}
void Elevator::on_ele2F4Button_clicked()
{
    _elevator[1]->addFloor(4);
}
void Elevator::on_ele2F5Button_clicked()
{
    _elevator[1]->addFloor(5);
}
void Elevator::on_ele2F6Button_clicked()
{
    _elevator[1]->addFloor(6);
}
void Elevator::on_ele2F7Button_clicked()
{
    _elevator[1]->addFloor(7);
}
void Elevator::on_ele2F8Button_clicked()
{
    _elevator[1]->addFloor(8);
}
void Elevator::on_ele2F9Button_clicked()
{
    _elevator[1]->addFloor(9);
}
void Elevator::on_ele2F10Button_clicked()
{
    _elevator[1]->addFloor(10);
}
void Elevator::on_ele2F11Button_clicked()
{
    _elevator[1]->addFloor(11);
}
void Elevator::on_ele2F12Button_clicked()
{
    _elevator[1]->addFloor(12);
}
void Elevator::on_ele2F13Button_clicked()
{
    _elevator[1]->addFloor(13);
}
void Elevator::on_ele2F14Button_clicked()
{
    _elevator[1]->addFloor(14);
}
void Elevator::on_ele2F15Button_clicked()
{
    _elevator[1]->addFloor(15);
}
void Elevator::on_ele2F16Button_clicked()
{
    _elevator[1]->addFloor(16);
}
void Elevator::on_ele2F17Button_clicked()
{
    _elevator[1]->addFloor(17);
}
void Elevator::on_ele2F18Button_clicked()
{
    _elevator[1]->addFloor(18);
}
void Elevator::on_ele2F19Button_clicked()
{
    _elevator[1]->addFloor(19);
}
void Elevator::on_ele2F20Button_clicked()
{
    _elevator[1]->addFloor(20);
}
void Elevator::on_ele2OpenButton_clicked()
{
    if((_elevator[1]->getStatus()==FREE)&&(_elevator[1]->getCloseDoor()))  //只有当前是空闲且关门才能开电梯
    {
        //开门动画
        int xLeft=ui->ele2LeftDoor->x();
        int xRight=ui->ele2RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele2LeftDoor->move(xLeft,ui->ele2RightDoor->y());
        ui->ele2RightDoor->move(xRight,ui->ele2RightDoor->y());
        _elevator[1]->setCloseDoor(false);
    }
}
void Elevator::on_ele2CloseButton_clicked()
{
    if(!_elevator[1]->getCloseDoor())   //如果当前电梯门是开的才能关门
    {
        //关门动画
        int xLeft=ui->ele2LeftDoor->x();
        int xRight=ui->ele2RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele2LeftDoor->move(xLeft,ui->ele2RightDoor->y());
        ui->ele2RightDoor->move(xRight,ui->ele2RightDoor->y());
        _elevator[1]->setCloseDoor(true);
    }
}

//3号电梯相关按钮点击函数
void Elevator::on_ele3F1Button_clicked()
{
    _elevator[2]->addFloor(1);
}
void Elevator::on_ele3F2Button_clicked()
{
    _elevator[2]->addFloor(2);
}
void Elevator::on_ele3F3Button_clicked()
{
    _elevator[2]->addFloor(3);
}
void Elevator::on_ele3F4Button_clicked()
{
    _elevator[2]->addFloor(4);
}
void Elevator::on_ele3F5Button_clicked()
{
    _elevator[2]->addFloor(5);
}
void Elevator::on_ele3F6Button_clicked()
{
    _elevator[2]->addFloor(6);
}
void Elevator::on_ele3F7Button_clicked()
{
    _elevator[2]->addFloor(7);
}
void Elevator::on_ele3F8Button_clicked()
{
    _elevator[2]->addFloor(8);
}
void Elevator::on_ele3F9Button_clicked()
{
    _elevator[2]->addFloor(9);
}
void Elevator::on_ele3F10Button_clicked()
{
    _elevator[2]->addFloor(10);
}
void Elevator::on_ele3F11Button_clicked()
{
    _elevator[2]->addFloor(11);
}
void Elevator::on_ele3F12Button_clicked()
{
    _elevator[2]->addFloor(12);
}
void Elevator::on_ele3F13Button_clicked()
{
    _elevator[2]->addFloor(13);
}
void Elevator::on_ele3F14Button_clicked()
{
    _elevator[2]->addFloor(14);
}
void Elevator::on_ele3F15Button_clicked()
{
    _elevator[2]->addFloor(15);
}
void Elevator::on_ele3F16Button_clicked()
{
    _elevator[2]->addFloor(16);
}
void Elevator::on_ele3F17Button_clicked()
{
    _elevator[2]->addFloor(17);
}
void Elevator::on_ele3F18Button_clicked()
{
    _elevator[2]->addFloor(18);
}
void Elevator::on_ele3F19Button_clicked()
{
    _elevator[2]->addFloor(19);
}
void Elevator::on_ele3F20Button_clicked()
{
    _elevator[2]->addFloor(20);
}
void Elevator::on_ele3OpenButton_clicked()
{
    if((_elevator[2]->getStatus()==FREE)&&(_elevator[2]->getCloseDoor()))  //只有当前是空闲且关门才能开电梯
    {
        //开门动画
        int xLeft=ui->ele3LeftDoor->x();
        int xRight=ui->ele3RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele3LeftDoor->move(xLeft,ui->ele3RightDoor->y());
        ui->ele3RightDoor->move(xRight,ui->ele3RightDoor->y());
        _elevator[2]->setCloseDoor(false);
    }
}
void Elevator::on_ele3CloseButton_clicked()
{
    if(!_elevator[2]->getCloseDoor())   //如果当前电梯门是开的才能关门
    {
        //关门动画
        int xLeft=ui->ele3LeftDoor->x();
        int xRight=ui->ele3RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele3LeftDoor->move(xLeft,ui->ele3RightDoor->y());
        ui->ele3RightDoor->move(xRight,ui->ele3RightDoor->y());
        _elevator[2]->setCloseDoor(true);
    }
}

//4号电梯相关按钮点击函数
void Elevator::on_ele4F1Button_clicked()
{
    _elevator[3]->addFloor(1);
}
void Elevator::on_ele4F2Button_clicked()
{
    _elevator[3]->addFloor(2);
}
void Elevator::on_ele4F3Button_clicked()
{
    _elevator[3]->addFloor(3);
}
void Elevator::on_ele4F4Button_clicked()
{
    _elevator[3]->addFloor(4);
}
void Elevator::on_ele4F5Button_clicked()
{
    _elevator[3]->addFloor(5);
}
void Elevator::on_ele4F6Button_clicked()
{
    _elevator[3]->addFloor(6);
}
void Elevator::on_ele4F7Button_clicked()
{
    _elevator[3]->addFloor(7);
}
void Elevator::on_ele4F8Button_clicked()
{
    _elevator[3]->addFloor(8);
}
void Elevator::on_ele4F9Button_clicked()
{
    _elevator[3]->addFloor(9);
}
void Elevator::on_ele4F10Button_clicked()
{
    _elevator[3]->addFloor(10);
}
void Elevator::on_ele4F11Button_clicked()
{
    _elevator[3]->addFloor(11);
}
void Elevator::on_ele4F12Button_clicked()
{
    _elevator[3]->addFloor(12);
}
void Elevator::on_ele4F13Button_clicked()
{
    _elevator[3]->addFloor(13);
}
void Elevator::on_ele4F14Button_clicked()
{
    _elevator[3]->addFloor(14);
}
void Elevator::on_ele4F15Button_clicked()
{
    _elevator[3]->addFloor(15);
}
void Elevator::on_ele4F16Button_clicked()
{
    _elevator[3]->addFloor(16);
}
void Elevator::on_ele4F17Button_clicked()
{
    _elevator[3]->addFloor(17);
}
void Elevator::on_ele4F18Button_clicked()
{
    _elevator[3]->addFloor(18);
}
void Elevator::on_ele4F19Button_clicked()
{
    _elevator[3]->addFloor(19);
}
void Elevator::on_ele4F20Button_clicked()
{
    _elevator[3]->addFloor(20);
}
void Elevator::on_ele4OpenButton_clicked()
{
    if((_elevator[3]->getStatus()==FREE)&&(_elevator[3]->getCloseDoor()))  //只有当前是空闲且关门才能开电梯
    {
        //开门动画
        int xLeft=ui->ele4LeftDoor->x();
        int xRight=ui->ele4RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele4LeftDoor->move(xLeft,ui->ele4RightDoor->y());
        ui->ele4RightDoor->move(xRight,ui->ele4RightDoor->y());
        _elevator[3]->setCloseDoor(false);
    }
}
void Elevator::on_ele4CloseButton_clicked()
{
    if(!_elevator[3]->getCloseDoor())   //如果当前电梯门是开的才能关门
    {
        //关门动画
        int xLeft=ui->ele4LeftDoor->x();
        int xRight=ui->ele4RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele4LeftDoor->move(xLeft,ui->ele4RightDoor->y());
        ui->ele4RightDoor->move(xRight,ui->ele4RightDoor->y());
        _elevator[3]->setCloseDoor(true);
    }
}

//5号电梯相关按钮点击函数
void Elevator::on_ele5F1Button_clicked()
{
    _elevator[4]->addFloor(1);
}
void Elevator::on_ele5F2Button_clicked()
{
    _elevator[4]->addFloor(2);
}
void Elevator::on_ele5F3Button_clicked()
{
    _elevator[4]->addFloor(3);
}
void Elevator::on_ele5F4Button_clicked()
{
    _elevator[4]->addFloor(4);
}
void Elevator::on_ele5F5Button_clicked()
{
    _elevator[4]->addFloor(5);
}
void Elevator::on_ele5F6Button_clicked()
{
    _elevator[4]->addFloor(6);
}
void Elevator::on_ele5F7Button_clicked()
{
    _elevator[4]->addFloor(7);
}
void Elevator::on_ele5F8Button_clicked()
{
    _elevator[4]->addFloor(8);
}
void Elevator::on_ele5F9Button_clicked()
{
    _elevator[4]->addFloor(9);
}
void Elevator::on_ele5F10Button_clicked()
{
    _elevator[4]->addFloor(10);
}
void Elevator::on_ele5F11Button_clicked()
{
    _elevator[4]->addFloor(11);
}
void Elevator::on_ele5F12Button_clicked()
{
    _elevator[4]->addFloor(12);
}
void Elevator::on_ele5F13Button_clicked()
{
    _elevator[4]->addFloor(13);
}
void Elevator::on_ele5F14Button_clicked()
{
    _elevator[4]->addFloor(14);
}
void Elevator::on_ele5F15Button_clicked()
{
    _elevator[4]->addFloor(15);
}
void Elevator::on_ele5F16Button_clicked()
{
    _elevator[4]->addFloor(16);
}
void Elevator::on_ele5F17Button_clicked()
{
    _elevator[4]->addFloor(17);
}
void Elevator::on_ele5F18Button_clicked()
{
    _elevator[4]->addFloor(18);
}
void Elevator::on_ele5F19Button_clicked()
{
    _elevator[4]->addFloor(19);
}
void Elevator::on_ele5F20Button_clicked()
{
    _elevator[4]->addFloor(20);
}
void Elevator::on_ele5OpenButton_clicked()
{
    if((_elevator[4]->getStatus()==FREE)&&(_elevator[4]->getCloseDoor()))  //只有当前是空闲且关门才能开电梯
    {
        //开门动画
        int xLeft=ui->ele5LeftDoor->x();
        int xRight=ui->ele5RightDoor->x();
        xLeft-=10;
        xRight+=10;
        ui->ele5LeftDoor->move(xLeft,ui->ele5RightDoor->y());
        ui->ele5RightDoor->move(xRight,ui->ele5RightDoor->y());
        _elevator[4]->setCloseDoor(false);
    }
}
void Elevator::on_ele5CloseButton_clicked()
{
    if(!_elevator[4]->getCloseDoor())   //如果当前电梯门是开的才能关门
    {
        //关门动画
        int xLeft=ui->ele5LeftDoor->x();
        int xRight=ui->ele5RightDoor->x();
        xLeft+=10;
        xRight-=10;
        ui->ele5LeftDoor->move(xLeft,ui->ele5RightDoor->y());
        ui->ele5RightDoor->move(xRight,ui->ele5RightDoor->y());
        _elevator[4]->setCloseDoor(true);
    }
}

//电梯外按钮点击函数
void Elevator::on_f1UpButton_clicked()
{
   _upWaitFloors[1]=true;
}

void Elevator::on_f2UpButton_clicked()
{
    _upWaitFloors[2]=true;
}

void Elevator::on_f2DownButton_clicked()
{
    _downWaitFloors[2]=true;
}

void Elevator::on_f3UpButton_clicked()
{
    _upWaitFloors[3]=true;
}

void Elevator::on_f3DownButton_clicked()
{
    _downWaitFloors[3]=true;
}

void Elevator::on_f4UpButton_clicked()
{
    _upWaitFloors[4]=true;
}

void Elevator::on_f4DownButton_clicked()
{
    _downWaitFloors[4]=true;
}

void Elevator::on_f5UpButton_clicked()
{
    _upWaitFloors[5]=true;
}

void Elevator::on_f5DownButton_clicked()
{
    _downWaitFloors[5]=true;
}

void Elevator::on_f6UpButton_clicked()
{
    _upWaitFloors[6]=true;
}

void Elevator::on_f6DownButton_clicked()
{
    _downWaitFloors[6]=true;
}

void Elevator::on_f7UpButton_clicked()
{
    _upWaitFloors[7]=true;
}

void Elevator::on_f7DownButton_clicked()
{
    _downWaitFloors[7]=true;
}

void Elevator::on_f8UpButton_clicked()
{
    _upWaitFloors[8]=true;
}

void Elevator::on_f8DownButton_clicked()
{
    _downWaitFloors[8]=true;
}

void Elevator::on_f9UpButton_clicked()
{
    _upWaitFloors[9]=true;
}

void Elevator::on_f9DownButton_clicked()
{
    _downWaitFloors[9]=true;
}

void Elevator::on_f10UpButton_clicked()
{
    _upWaitFloors[10]=true;
}

void Elevator::on_f10DownButton_clicked()
{
    _downWaitFloors[10]=true;
}

void Elevator::on_f11UpButton_clicked()
{
    _upWaitFloors[11]=true;
}

void Elevator::on_f11DownButton_clicked()
{
    _downWaitFloors[11]=true;
}

void Elevator::on_f12UpButton_clicked()
{
    _upWaitFloors[12]=true;
}

void Elevator::on_f12DownButton_clicked()
{
    _downWaitFloors[12]=true;
}

void Elevator::on_f13UpButton_clicked()
{
    _upWaitFloors[13]=true;
}

void Elevator::on_f13DownButton_clicked()
{
    _downWaitFloors[13]=true;
}

void Elevator::on_f14UpButton_clicked()
{
    _upWaitFloors[14]=true;
}

void Elevator::on_f14DownButton_clicked()
{
    _downWaitFloors[14]=true;
}

void Elevator::on_f15UpButton_clicked()
{
    _upWaitFloors[15]=true;
}

void Elevator::on_f15DownButton_clicked()
{
    _downWaitFloors[15]=true;
}

void Elevator::on_f16UpButton_clicked()
{
    _upWaitFloors[16]=true;
}

void Elevator::on_f16DownButton_clicked()
{
    _downWaitFloors[16]=true;
}

void Elevator::on_f17UpButton_clicked()
{
    _upWaitFloors[17]=true;
}

void Elevator::on_f17DownButton_clicked()
{
    _downWaitFloors[17]=true;
}

void Elevator::on_f18UpButton_clicked()
{
    _upWaitFloors[18]=true;
}

void Elevator::on_f18DownButton_clicked()
{
    _downWaitFloors[18]=true;
}

void Elevator::on_f19UpButton_clicked()
{
    _upWaitFloors[19]=true;
}

void Elevator::on_f19DownButton_clicked()
{
    _downWaitFloors[19]=true;
}

void Elevator::on_f20DownButton_clicked()
{
    _downWaitFloors[20]=true;
}
