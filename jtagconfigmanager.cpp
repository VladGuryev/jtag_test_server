#include "jtagconfigmanager.h"

JtagConfigManager::JtagConfigManager(QObject *parent) : QObject(parent)
{
    configServer = new QTcpServer(this);
    connect(configServer, SIGNAL(newConnection()), this, SLOT(newConnectionHandler()));
    configServer->listen(QHostAddress::LocalHost, port_cc);
}


void JtagConfigManager::newConnectionHandler()
{
    qDebug() << "newConnectionHandler";
    while (configServer->hasPendingConnections()){
        m_socket = new QTcpSocket(this);
        m_socket = configServer->nextPendingConnection();
        connect(m_socket, SIGNAL(disconnected()), SLOT(disconnectedHandler()));
        connect(m_socket, SIGNAL(readyRead()), SLOT(readyReadHandler()));

        if(m_socket->state() == QAbstractSocket::ConnectedState){
            emit socketConnected();
            isSocketConnected = true;
            qDebug() << "socket connected to jtag after emit socketConnected()";
        } else {
            qDebug() << "connection of socket with jtag was not successfull";
        }
    }
}

/**
 * @brief ConfigTcpServer::sendCommand(QString adrSent, QString newValue)
 * метод отправл€ет запрос (его нужно сформировать) дл€ того чтобы плис вернула ответ,
 * вроде того что в такой то €чейке лежит некоторое число
 * ‘ункци€ отправл€ет строку, котора€ €вл€етс€ hex выражением, A = 10 и т.д.
 * @param
 */
void JtagConfigManager::sendCommand(const QString adress, const QString data)
{
    if(!isSocketConnected)
        return;
    QByteArray byteArrDataSent = dataFormaterIn(adress, data);

    if(m_socket->state() == QAbstractSocket::ConnectedState && m_socket != Q_NULLPTR){
       // qDebug() << "Qbytearray to the socket: " << byteArrDataSent;
        qint64 i = m_socket->write(byteArrDataSent);
        m_socket->waitForBytesWritten(10);
      //  qDebug() << "data  sent to the fpga: " << "i = " << i << " data : " << adrSent;
        if(data != ""){
//            QString setConfigAdress = "4002";
//            QString setConfigValue = "1";
//            QByteArray applyByteArray = dataFormaterIn(setConfigAdress, setConfigValue);
//            qDebug() << "applyByteArray: " << applyByteArray;
//            qint64 i = m_socket->write(applyByteArray);
//            m_socket->waitForBytesWritten(10);
        }
    } else {
      //  qDebug() << "connection between socket and jtag failed";
    }

}

void JtagConfigManager::disconnectedHandler()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}

QByteArray JtagConfigManager::dataFormaterIn(QString adress, QString data)
{
    QString endStr = " FFFFFFFFFFFFFFFF\r\n";
    QByteArray formatedData;

    if(data == ""){
        formatedData = (adress + endStr).toLatin1();
    } else {
        QString convertedValue = IntToHexConverter(data);
        formatedData = (adress + " " + convertedValue + "\r\n").toLatin1();
    }
    return formatedData;
}

QString JtagConfigManager::IntToHexConverter(QString data)
{
    bool ok;
    uint dec = data.toUInt(&ok, 10);
    data = QString("%1").arg(dec,0,16).toUpper();  //плис понимает только строки в hex виде. таким образом строку
    // приходитс€ конвертировать из int представлени€ в строку hex символов
    int count = data.size();
    QString convertedData;

    for(int i = 0; i < 16 - count; i++){   //дополн€ем нул€ми недостающие позиции в строке из 16 символов
        convertedData.append("0");
    }
    convertedData += data;

    return convertedData;
}

QPair<QString, QString> JtagConfigManager::dataFormaterOut(QByteArray dataReceived)
{
    QString stringizedData = QString::fromStdString(dataReceived.toStdString());
    QString receivedValue;
    QString receiverAdres;

    bool firstNotZeroSymbol = false;
    for(int i = 0; i < 4; i++){
        receiverAdres.append(stringizedData.at(i));
    }
    for(int i = 6; i < 21; i++){
        if(stringizedData.at(i) != "0" || firstNotZeroSymbol){
            firstNotZeroSymbol = true;
            receivedValue.append(stringizedData.at(i));
        }
    }
    if(receivedValue == "")
        receivedValue = "0";
    return qMakePair(receiverAdres, receivedValue);
}

void JtagConfigManager::readyReadHandler()
{
    // qDebug() << "readyReadHandler()";
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());
    if (socket != nullptr) {
        if (socket->bytesAvailable()) {
            QByteArray dataReceived = socket->readLine(photoPacketByteSize);
            emit this->rawDataReceived(dataReceived);
            emit this->parsedDataReceived(dataFormaterOut(dataReceived));
            qDebug() << "adress: " << dataFormaterOut(dataReceived).first << "received: " <<
                        dataFormaterOut(dataReceived).second;
        }
    }
}

JtagConfigManager::~JtagConfigManager()
{
    m_socket->disconnect();
    delete configServer;
}
