#ifndef JTAGCONFIGMANAGER_H
#define JTAGCONFIGMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class JtagConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit JtagConfigManager(QObject *parent = nullptr);
    ~JtagConfigManager();

    void sendCommand(QString dataSent, QString newValue = "");
    QByteArray dataBuffer;
    bool isSocketConnected = false;
signals:
    void socketConnected();
    void dataRecieved(QByteArray dataFromSocket);

private slots:
    void newConnectionHandler();
    void readyReadHandler();
    void disconnectedHandler();
    void dataBuffered(QByteArray dataFromSocket);

private:
    quint16 configPort = 2541;
    QTcpServer configServer;
    QTcpSocket* m_currentSocket;

    static const int photoPacketByteSize = 26;
};

#endif // JTAGCONFIGMANAGER_H
