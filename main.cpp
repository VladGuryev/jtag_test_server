#include <QCoreApplication>
#include <QProcess>
#include <QTime>
#include "jtagconfigmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JtagConfigManager manager(&a);

    QProcess* quartusStpProcess = new QProcess(&a);
    QString argument;
    argument = "\"C:/altera/14.1/qprogrammer/bin64/quartus_stp.exe\" -t \"D:/VGuryev/VVOD/build-vvod_slot-Desktop_Qt_5_10_1_MinGW_32bit-Debug/appdesktop/jtag_client.tcl\" 1 \"";
    quartusStpProcess->start(argument);

    if(quartusStpProcess->state() == QProcess::ProcessState::Running){
        qDebug() << "process running";
    }

    auto delay = [](int msec){QTime dieTime = QTime::currentTime().addMSecs(msec);
        while(QTime::currentTime() < dieTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }};
    delay(1500);


    int i = 4;
    while(i>0){
        qDebug() << "READING:";
//        manager.sendCommand("0004");
//        manager.sendCommand("000e");
//        manager.sendCommand("0003");
//        manager.sendCommand("000d");
        //manager.sendCommand("0001");
          manager.sendCommand("0095");
//        qDebug() << "WRITING PROCESS";
//        manager.sendCommand("4001", "111");
//        delay(500);
//        qDebug() << "----------------------\n";
//        qDebug() << "READING:";
//        manager.sendCommand("0001");
//        qDebug() << "WRITING PROCESS";
//        manager.sendCommand("4001", "222");
        delay(2000);
//        qDebug() << "----------------------\n";

        i--;
    }



    return a.exec();
}
