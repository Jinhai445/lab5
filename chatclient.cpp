#include "chatclient.h"
#include <QJsonDocument>
#include <QDataStream>
#include <QJsonObject>
chatClient::chatClient(QObject *parent): QObject(parent)
{
    m_clientSocket = new QTcpSocket(this);
    // &chatClient::connected
    connect(m_clientSocket,&QTcpSocket::connected,this,[](){
        qDebug()<<"连接成功";
    });
    connect(m_clientSocket,&QTcpSocket::connected,this,&chatClient::connected);
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&chatClient::onReadyRead);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, []() {
        qDebug() << "Disconnected from server!";
    });
}

QTcpSocket *chatClient::clientSocket() const
{
    return m_clientSocket;
}

void chatClient::setClientSocket(QTcpSocket *newClientSocket)
{
    m_clientSocket = newClientSocket;
}

void chatClient::sendJson(const QJsonObject &json)
{
    if(clientSocket()->state()!= QAbstractSocket::ConnectedState)
        return;
    const QByteArray jsonData=QJsonDocument(json).toJson(QJsonDocument::Compact);
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream<<jsonData;
}

QString chatClient::getCurPriChatUser() const
{
    return curPriChatUser;
}

void chatClient::setCurPriChatUser(const QString &newCurPriChatUser)
{
    curPriChatUser = newCurPriChatUser;
}

void chatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void chatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);// start an infinite loop
    for(;;){
        socketStream.startTransaction();
        socketStream>>jsonData;
        if(socketStream.commitTransaction()){
            // emit messageReceived(QString::fromUtf8(jsonData));
            QJsonParseError parseError;
            const QJsonDocument jsonDoc =QJsonDocument::fromJson(jsonData,&parseError);
            if(parseError.error == QJsonParseError::NoError){
                if(jsonDoc.isObject()){
                    qDebug()<<jsonDoc.object();
                    emit jsonReceived(jsonDoc.object());// parse the
                }
            }
        } else {
            break;
        }

    }
}

void chatClient::sendMessage(const QString &text, const QString &type,const QString &receiver)
{
    if(m_clientSocket->state()!= QAbstractSocket::ConnectedState)
        return;

    if(!type.isEmpty()){
        //create a ODataStream operating on the socket
        QDataStream serverStream(m_clientSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);
        //Create the JSoN we want to send
        QJsonObject message;
        message["type"]=type;
        message["text"]= text;
        if(!receiver.isEmpty()){
            message["receiver"] = receiver;
        }
        // send the JSON using QDatastream
        serverStream<<QJsonDocument(message).toJson();
    }
}

void chatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address,port);
}
