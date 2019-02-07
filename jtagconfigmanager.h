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
    void sendCommand(const QString adress, const QString data = "");
    void setPort(quint16 new_port);
    void runServer();
signals:
    void socketConnected();
    /**
     * @brief rawDataReceived
     * @param dataFromSocket - возвращает сырой набор байтов hex
     */
    void rawDataReceived(QByteArray dataFromSocket);
    /**
     * @brief parsedDataReceived
     * @param adrValue - (first, last) строчное представление hex-числа
     * возвращает распарсенную пару адреса и значения в hex в стринге
     */
    void parsedDataReceived(QPair<QString, QString> adrValue);

private slots:
    void newConnectionHandler();
    void readyReadHandler();
    void disconnectedHandler();

private:
    quint16 port_cc = 2541;
    QTcpServer* configServer;
    QTcpSocket* m_socket;
    bool isSocketConnected = false;
    static const int photoPacketByteSize = 26;

private:
    QByteArray dataFormaterIn(QString adress, QString data);
    QPair<QString, QString> dataFormaterOut(QByteArray dataReceived);
    QString IntToHexConverter(QString data);
};

#endif // JTAGCONFIGMANAGER_H
