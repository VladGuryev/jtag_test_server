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
    argument = "\"C:/altera/14.1/qprogrammer/bin64/quartus_stp.exe\" -t \"D:/VGuryev/VVOD/build-vvod_slot-Desktop_Qt_5_10_1_MinGW_32bit-Debug/appdesktop/jtag_client.tcl\" 4 \"";
    quartusStpProcess->start(argument);

    if(quartusStpProcess->state() == QProcess::ProcessState::Running){
        qDebug() << "process running";
    }

    QTime dieTime = QTime::currentTime().addMSecs(2000);
    while(QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    manager.sendCommand("0004");


    return a.exec();
}
