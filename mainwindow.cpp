#include "mainwindow.h"
#include "messagesearch.h"
#include "ui_mainwindow.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    m_chatClient = new chatClient(this);

    connect(m_chatClient,&chatClient::connected,this,&MainWindow::connectedToServer);
    connect(m_chatClient,&chatClient::jsonReceived,this,&MainWindow::jsonReveived);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::removeItem()
{
    QListWidgetItem *item = ui->userList->currentItem();
    if (item) {
        // 删除选中的项
        delete item;
    } else {
        // 如果没有选中项，弹出警告框
        QMessageBox::warning(this, "警告", "请先选择一个项进行删除！");
    }
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

//登录
void MainWindow::on_loginBt_clicked()
{
    if(ui->userName->text().trimmed().isEmpty()){
        QMessageBox::warning(this, "ERROR","用户名为空");
        return;
    }
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()),1967);

}
//连接
void MainWindow::connectedToServer()
{

    QJsonObject message;
    message["type"]="login";
    message["userName"] = ui->userName->text().trimmed();
    message["password"] = ui->pwd->text().trimmed();
    QDataStream serverStream(m_chatClient->clientSocket());
    serverStream.setVersion(QDataStream::Qt_5_12);
    serverStream<<QJsonDocument(message).toJson();

}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->publicChatShow->append(QString("%1: %2").arg(sender).arg(text));
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

    }else if(typeVal.toString().compare("isSend",Qt::CaseInsensitive)==0){
        const QJsonValue textVal= docObj.value("text");
        if(textVal.isNull()||!textVal.isString())
            return;
        if(textVal.toString() =="ok"){
            ui->publicChatShow->append(QString("%1(me): %2").arg(ui->userName->text()).arg(ui->MessageEdit->text()));
        }
        if(textVal.toString() =="Words prohibited"){
            QMessageBox::warning(this,"ERROR","你已被禁言");
        }
        ui->MessageEdit->setText("");

    }
    else if(typeVal.toString().compare("newuser",Qt::CaseInsensitive)== 0){
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

        userListReceived(userlistVal.toVariant().toStringList());
    }else if(typeVal.toString().compare("privatemessage",Qt::CaseInsensitive)==0){
        const QJsonValue textVal= docObj.value("text");
        const QJsonValue senderVal = docObj.value("sender");
        if(senderVal.isNull()||!senderVal.isString())
            return;
        if(textVal.isNull()||!textVal.isString())
            return;
        if(ui->stackedWidget->currentWidget()==ui->PriChatPage && ui->PriChatName->text() == senderVal.toString()){
            ui->PriChatShow->append(QString("%1: %2").arg(senderVal.toString()).arg(textVal.toString()));
        }else{
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setWindowTitle("提示");
            msgBox->setText(QString("%1发来一条新消息").arg(senderVal.toString()));
            msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            // 修改 "确定" 按钮的文本为 "去私聊"
            msgBox->button(QMessageBox::Ok)->setText("去私聊");
            msgBox->button(QMessageBox::Cancel)->setText("取消");
            // 显示消息框
            int ret = msgBox->exec();
            qDebug()<<(ret==QMessageBox::Ok);
            // 根据用户选择的按钮进行不同的操作
            if (ret == QMessageBox::Ok) {
                toPriChat(senderVal.toString());
            } else if (ret == QMessageBox::Cancel) {
                qDebug() << "用户点击了取消";
            }

            delete msgBox;  // 删除动态分配的 msgBox 对象
        }

        // ui->PriChatShow->append(decodedString);
    }else if(typeVal.toString().compare("initPri",Qt::CaseInsensitive)== 0){
        const QJsonValue userlistVal = docObj.value("list");
        if(userlistVal.isNull()||!userlistVal.isArray())
            return;
        QStringList list = userlistVal.toVariant().toStringList();
        ui->PriChatShow->clear();
        for(QString str : list){
            ui->PriChatShow->append(str);
        }
    }else if(typeVal.toString().compare("error",Qt::CaseInsensitive)== 0){
        const QJsonValue errorVal = docObj.value("text");
        if(errorVal.isNull()||!errorVal.isString())
            return;
        QMessageBox::warning(this, "ERROR", errorVal.toString());
    }else if(typeVal.toString().compare("login",Qt::CaseInsensitive)== 0){
        QJsonValue textVal = docObj.value("text");
        if(textVal.isNull()||!textVal.isString())
            return;
        if(textVal == "Login successful!"){
            toPublicChat();
            ui->titleLabel->setText(QString("%1的聊天室").arg(ui->userName->text()));
        }else{
            m_chatClient->disconnectFromHost();
            QMessageBox::warning(this, "ERROR",textVal.toString());
        }
    }else if(typeVal.toString().compare("initPublic",Qt::CaseInsensitive)== 0){
        const QJsonValue listVal = docObj.value("list");
        if(listVal.isNull()||!listVal.isArray())
            return;
        QStringList list = listVal.toVariant().toStringList();
        ui->publicChatShow->clear();
        for(QString str : list){
            ui->publicChatShow->append(str);
        }
    }else if(typeVal.toString().compare("search",Qt::CaseInsensitive)== 0){
        const QJsonValue listVal = docObj.value("list");
        if(listVal.isNull()||!listVal.isArray())
            return;
        ui->searchList->clear();
        for(QString str:listVal.toVariant().toStringList()){
            ui->searchList->addItem(str);
        }

        // toSearchPage();

    }else if(typeVal.toString().compare("ban",Qt::CaseInsensitive)== 0){
        QJsonValue textVal = docObj.value("text");
        if(textVal.isNull()||!textVal.isString())
            return;
        QMessageBox::information(this,"提示",textVal.toString());

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
    if(!ui->MessageEdit->text().isEmpty()){
        m_chatClient->sendMessage(ui->MessageEdit->text());

    }
}



//双击进入私聊，并初始化信息
void MainWindow::on_userList_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug()<<item->text();
    m_chatClient->setCurPriChatUser(item->text());
    if(item->text()!= ui->userName->text()+"*"){
        toPriChat(item->text());
    }

}
void MainWindow::toPriChat(const QString& chatName){
    ui->stackedWidget->setCurrentWidget(ui->PriChatPage);
    ui->PriChatName->setText(chatName);
    m_chatClient->sendMessage("","initPri",ui->PriChatName->text());
}

void MainWindow::toSearchPage(const QString &userList)
{
    ui->stackedWidget->setCurrentWidget(ui->searchPage);
    for(QString user : userList){
        ui->users->addItem(user);
    }
}
//进入群聊并初始化信息
void MainWindow::toPublicChat(){
    ui->stackedWidget->setCurrentWidget(ui->publicChatPage);
    QJsonObject message;
    message["type"]="initPublic";
    m_chatClient->sendJson(message);
}
//私聊发送信息
void MainWindow::on_PriSendBt_clicked()
{
    if(!ui->priMessage->text().isEmpty()){
        ui->PriChatShow->append(QString("%1(me): %2").arg(ui->userName->text()).arg(ui->priMessage->text()));
        m_chatClient->sendMessage(ui->priMessage->text(),"privateMessage",m_chatClient->getCurPriChatUser());
        ui->priMessage->setText("");
    }

}

//退出私聊
void MainWindow::on_quitPriChat_clicked()
{
    toPublicChat();
}





void MainWindow::on_noWord_clicked()
{
    if(ui->userName->text()!="admin"){
        QMessageBox::warning(this,"ERROR","无权限");
        return;
    }else{
        QJsonObject message;
        message["type"]="ban";
        message["isBanned"]="true";
        if(ui->userList->currentItem()==nullptr){
            QMessageBox::warning(this,"ERROR","请选择用户");
        }
        if(ui->userList->currentItem()->text()== "admin*"){
            return;
        }
        message["user"] = ui->userList->currentItem()->text();
        m_chatClient->sendJson(message);
    }
}


void MainWindow::on_search_clicked()
{

    ui->stackedWidget->setCurrentWidget(ui->searchPage);
    ui->searchList->clear();
    ui->users->clear();
    ui->type->setCurrentIndex(0);
    ui->startTime->setDateTime(QDateTime::currentDateTime());
    ui->endTime->setDateTime(QDateTime::currentDateTime());
    ui->keyWord->clear();
    int itemCount = ui->userList->count();
    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->userList->item(i);
        QString itemText = item->text();
        if(itemText==ui->userName->text()+"*"){
            itemText = ui->userName->text();
        }
        ui->users->addItem(itemText);
    }
}


void MainWindow::on_type_currentTextChanged(const QString &arg1)
{
    if(arg1=="群聊"){
        ui->users->clear();
        int itemCount =ui->userList->count();
        // 遍历 QListWidget 中的所有项
        for (int i = 0; i < itemCount; ++i) {
            // 获取每个 QListWidgetItem
            QListWidgetItem* item = ui->userList->item(i);
            // 获取项的文本

            QString itemText = item->text();
            if(itemText==ui->userName->text()+"*"){
                itemText = ui->userName->text();
            }
             ui->users->addItem(itemText);
        }
    }else if(arg1=="私聊"){
        ui->users->clear();
        int itemCount =ui->userList->count();
        // 遍历 QListWidget 中的所有项
        for (int i = 0; i < itemCount; ++i) {
            // 获取每个 QListWidgetItem
            QListWidgetItem* item = ui->userList->item(i);

            QString itemText = item->text();
            if(itemText!= ui->userName->text()+"*"){
                ui->users->addItem(itemText);
            }
        }
    }
}


void MainWindow::on_searchBt_clicked()
{
    QJsonObject message;
    message["type"]="search";
    message["searchType"]= ui->type->currentText();
    message["user"] = ui->users->currentText();
    message["keyWord"] = ui->keyWord->text();
    message["start"] = ui->startTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    message["end"] = ui->endTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    m_chatClient->sendJson(message);
}


void MainWindow::on_quitSearch_clicked()
{
    toPublicChat();
}


void MainWindow::on_noBan_clicked()
{
    if(ui->userName->text()!="admin"){
        QMessageBox::warning(this,"ERROR","无权限");
        return;
    }else{
        QJsonObject message;
        message["type"]="ban";
        message["isBanned"]="false";
        if(ui->userList->currentItem()==nullptr){
            QMessageBox::warning(this,"ERROR","请选择用户");
        }
        if(ui->userList->currentItem()->text()== "administrator*"){
            return;
        }
        message["user"] = ui->userList->currentItem()->text();
        m_chatClient->sendJson(message);
    }
}

