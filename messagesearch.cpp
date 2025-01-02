#include "messagesearch.h"
#include "ui_messagesearch.h"
#include <QJsonObject>
// MessageSearch::MessageSearch(QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::MessageSearch)
// {
//     ui->setupUi(this);
// }


// 默认构造函数
MessageSearch::MessageSearch(QWidget *parent)
    : QWidget(parent), ui(new Ui::MessageSearch)
{
    ui->setupUi(this);
    ui->type->setCurrentIndex(0);
}

// 带参数的构造函数
MessageSearch::MessageSearch(const QStringList &userList, const QString &userName, chatClient* m_chatClient, QWidget *parent)
    : QWidget(parent), ui(new Ui::MessageSearch), userList(userList), userName(userName),m_chatClient(m_chatClient)
{
    ui->setupUi(this);


}
MessageSearch::~MessageSearch()
{
    delete ui;
}

void MessageSearch::on_type_currentTextChanged(const QString &arg1)
{

    qDebug()<<ui->startTime;
}


void MessageSearch::on_search_clicked()
{
    QJsonObject message;
    message["type"]="search";
    message["searchType"]= ui->type->currentText();
    message["sender"] = ui->senders->currentText();
    message["start"] = ui->startTime->time().toString();
    message["end"] = ui->endTime->time().toString();
    m_chatClient->sendJson(message);
}

