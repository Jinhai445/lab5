#ifndef MESSAGESEARCH_H
#define MESSAGESEARCH_H

#include <QWidget>
#include <chatClient.h>

namespace Ui {
class MessageSearch;
}

class MessageSearch : public QWidget
{
    Q_OBJECT

public:
    explicit MessageSearch(QWidget *parent = nullptr);
    ~MessageSearch();
    explicit MessageSearch(const QStringList &userList, const QString &userName,  chatClient* m_chatClient,QWidget *parent = nullptr);
private slots:
    void on_type_currentTextChanged(const QString &arg1);

    void on_search_clicked();

private:
    Ui::MessageSearch *ui;
    QStringList userList;
    QString userName;
    chatClient* m_chatClient;
};

#endif // MESSAGESEARCH_H
