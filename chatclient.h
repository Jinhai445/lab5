#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
class chatClient : public QObject
{
    Q_OBJECT
public:
    explicit chatClient(QObject *parent = nullptr);

signals:
    void connected();
    void messageReceived(const QString &text);
    void jsonReceived(const QJsonObject & docObj);
private:
    QTcpSocket* m_clientSocket;
    QString curPriChatUser;
public:
    void disconnectFromHost();
    QString getCurPriChatUser() const;
    void setCurPriChatUser(const QString &newCurPriChatUser);
    QTcpSocket *clientSocket() const;
    void setClientSocket(QTcpSocket *newClientSocket);
    void sendJson(const QJsonObject &json);

public slots:
    void onReadyRead();
    void sendMessage(const QString &text,const QString &type  = "message",const QString &receiver="");
    void connectToServer(const QHostAddress &address,quint16 port);

};

#endif // CHATCLIENT_H
