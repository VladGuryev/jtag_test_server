#include "jtagconfigmanager.h"

JtagConfigManager::JtagConfigManager(QObject *parent) : QObject(parent)
{
    connect(&configServer, SIGNAL(newConnection()), this, SLOT(newConnectionHandler()));
    configServer.listen(QHostAddress::LocalHost, configPort);
    connect(this, SIGNAL(dataRecieved(QByteArray)), this, SLOT(dataBuffered(QByteArray)));
}


void JtagConfigManager::newConnectionHandler()
{
    qDebug() << "newConnectionHandler";
    while (configServer.hasPendingConnections()){
        QTcpSocket *socket = new QTcpSocket(this);
        socket = configServer.nextPendingConnection();
        connect(socket, SIGNAL(disconnected()), SLOT(disconnectedHandler()));
        connect(socket, SIGNAL(readyRead()), SLOT(readyReadHandler()));

        m_currentSocket = socket;

        if(socket->state() == QAbstractSocket::ConnectedState){
            emit socketConnected();
            isSocketConnected = true;
            qDebug() << "socket connected to jtag after emit socketConnected()";
        } else {
            qDebug() << "connection of socket with jtag was not successfull";
        }
    }
}

//метод дл€ записи нового значени€ в плис

/**
 * @brief ConfigTcpServer::onSocketConnectionWriteHandler
 * //send a packet отправл€ем команду на считывание по таймеру из HighTemp
 * метод отправл€ет запрос (его нужно сформировать) дл€ того чтобы плис вернула ответ, вроде того что в такой то €чейке
 * лежит некоторое число
 * @param dataSent
 */
void JtagConfigManager::sendCommand(QString adrSent, QString newValue)
{
    if(!isSocketConnected)
        return;
    QTcpSocket* socket = m_currentSocket; //qDebug() << "socket->readBufferSize() = " << socket->readBufferSize();
    QString endStr = " FFFFFFFFFFFFFFFF\r\n";
    QByteArray byteArrDataSent;

    if(newValue == ""){
        byteArrDataSent = (adrSent + endStr).toLatin1();
    } else {
        byteArrDataSent = (adrSent + " " + newValue + "\r\n").toLatin1();
    }

    if(socket->state() == QAbstractSocket::ConnectedState && m_currentSocket != Q_NULLPTR){

        //QString str = "0001 FFFFFFFFFFFFFFFF\r\n";
        //QByteArray dataSent2 = str.toLatin1();

        qDebug() << "Qbytearray to the socket: " << byteArrDataSent;
        qint64 i = socket->write(byteArrDataSent);
        socket->waitForBytesWritten(150);

        qDebug() << "data  sent to the fpga: " << "i = " << i << " data : " << adrSent;
    } else {
        qDebug() << "connection between socket and jtag failed";
    }
}

JtagConfigManager::~JtagConfigManager()
{

}


void JtagConfigManager::disconnectedHandler()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}

void JtagConfigManager::dataBuffered(QByteArray dataFromSocket)
{
    this->dataBuffer = dataFromSocket;
}

void JtagConfigManager::readyReadHandler()
{
    qDebug() << "readyReadHandler()";
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());
    if (socket != nullptr) {
        if (socket->bytesAvailable()) {
           //emit this->dataRecieved(socket->readLine(photoPacketByteSize));
           qDebug() << socket->readLine(photoPacketByteSize);
        }
    }
}
