#include <QCoreApplication>
#include <QProcess>
#include <QTime>
#include "jtagconfigmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JtagConfigManager manager;

    QProcess* quartusStpProcess = new QProcess;
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
    delay(2000);

    while(1){
        manager.sendCommand("0004");
        manager.sendCommand("000e");
        manager.sendCommand("0003");
        manager.sendCommand("000d");
        manager.sendCommand("0001");
        delay(100);
        qDebug() << "-----------------------------------------------";
    }



    return a.exec();
}
