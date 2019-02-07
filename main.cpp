#include <QCoreApplication>
#include <QProcess>
#include <QTime>
#include "jtagconfigmanager.h"

struct ControlConstantRegisters{
    QString SET_ZERO = "0094";
    QString USER_SET_POSITION = "0095";
    QString CURRENT_POSITION = "0096";
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JtagConfigManager manager(&a);
    manager.runServer();

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

    ControlConstantRegisters registers;

    //requests to engine
    manager.sendCommand(registers.CURRENT_POSITION);
    manager.sendCommand(registers.SET_ZERO, "1");
    delay(5000);
    manager.sendCommand(registers.USER_SET_POSITION, "1000");
    delay(5000);
    manager.sendCommand(registers.CURRENT_POSITION);


    return a.exec();
}
