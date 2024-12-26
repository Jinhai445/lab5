#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonValue>
#include <QJsonObject>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    m_chatClient = new chatClient(this);
    connect(m_chatClient,&chatClient::connected,this,&MainWindow::connectedToServer);
    // connect(m_chatClient,&chatClient::messageReceived,this,&MainWindow::messageReceived);
    connect(m_chatClient,&chatClient::jsonReceived,this,&MainWindow::jsonReveived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::userLeft(const QString &user)
{
    for(auto aItem: ui->userList->findItems(user,Qt::MatchExactly)){
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::userListReceived(const QStringList &list)
{
    qDebug()<<list;
    ui->userList->clear();
    ui->userList->addItems(list);
}

void MainWindow::on_loginBt_clicked()
{
    qDebug()<<ui->serverEdit->text();
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()),1967);

}

void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    ui->titleLabel->setText(QString("%1的聊天室").arg(ui->userName->text()));
    qDebug()<<ui->userName->text();
    m_chatClient->sendMessage(ui->userName->text(),"login");

}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->textEdit->append(QString("%1: %2").arg(sender).arg(text));
}



void MainWindow::jsonReveived(const QJsonObject &docObj)
{
    qDebug()<<"json"<<docObj;
    const QJsonValue typeVal= docObj.value("type");
    // qDebug()<<"判断"<<(typeVal.toString().compare("userlist",Qt::CaseInsensitive)== 0);
    if(typeVal.isNull()||!typeVal.isString())
        return;
    if(typeVal.toString().compare("message",Qt::CaseInsensitive)==0){
        const QJsonValue textVal= docObj.value("text");
        const QJsonValue senderVal = docObj.value("sender");
        if(senderVal.isNull()||!senderVal.isString())
            return;
        if(textVal.isNull()||!textVal.isString())
            return;
        const QString text=textVal.toString().trimmed();
        messageReceived(senderVal.toString(),textVal.toString());
        QJsonObject message;

    }else if(typeVal.toString().compare("newuser",Qt::CaseInsensitive)== 0){
        const QJsonValue usernameVal =docObj.value("username");
        if(usernameVal.isNull()||!usernameVal.isString())
            return;
        userJoined(usernameVal.toString());

    }else if(typeVal.toString().compare("userdisconnected",Qt::CaseInsensitive)== 0){
        const QJsonValue usernameVal =docObj.value("username");
        if(usernameVal.isNull()||!usernameVal.isString())
            return;
        userLeft(usernameVal.toString());

    }else if(typeVal.toString().compare("userlist",Qt::CaseInsensitive)== 0){
        const QJsonValue userlistVal =docObj.value("userlist");
        if(userlistVal.isNull()||!userlistVal.isArray())
            return;
        // qDebug()<<"12"<<userlistVal.toVariant().toStringList();
        userListReceived(userlistVal.toVariant().toStringList());
    }else{

    }

}

void MainWindow::userJoined(const QString& user)
{
    ui->userList->addItem(user);
}


void MainWindow::on_logout_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    m_chatClient->disconnectFromHost();
    for(auto aItem: ui->userList->findItems(ui->userName->text(),Qt::MatchExactly)){
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}


void MainWindow::on_send_clicked()
{
    if(!ui->edit->text().isEmpty()){
        m_chatClient->sendMessage(ui->edit->text());
        ui->edit->text().clear();
    }
}

