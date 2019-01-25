#ifndef JTAGCONFIGMANAGER_H
#define JTAGCONFIGMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVariant>

class JtagConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit JtagConfigManager(QObject *parent = nullptr);
    ~JtagConfigManager();

    void sendCommand(QString adress, QString data = "");

signals:
    void socketConnected();
    void dataReceived(QByteArray dataFromSocket);

private slots:
    void newConnectionHandler();
    void readyReadHandler();
    void disconnectedHandler();

private:
    bool isSocketConnected = false;
    quint16 configPort = 2541;
    QTcpServer configServer;
    QTcpSocket* m_socket;
    static const int photoPacketByteSize = 26;
    QByteArray dataFormaterIn(QString adress, QString data);
    QString IntToHexConverter(QString data);
    QPair<QString, QString> dataFormaterOut(QByteArray dataReceived);
};

#endif // JTAGCONFIGMANAGER_H
