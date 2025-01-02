#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QTextEdit>
#include "chatclient.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public :
    void userLeft(const QString &user);
    void userListReceived(const QStringList & list);
    void toPriChat(const QString &chatName);
    void toSearchPage(const QString& userList);
    void toPublicChat();
    void removeItem();
private slots:
    void on_loginBt_clicked();
    void on_logout_clicked();
    void on_send_clicked();
    void connectedToServer();
    void messageReceived(const QString &sender,const QString &text);
    void jsonReveived(const QJsonObject & docObj);

    void on_userList_itemDoubleClicked(QListWidgetItem *item);

    void on_PriSendBt_clicked();

    void on_quitPriChat_clicked();


    void on_noWord_clicked();

    void on_search_clicked();

    void on_type_currentTextChanged(const QString &arg1);

    void on_searchBt_clicked();

    void on_quitSearch_clicked();

    void on_noBan_clicked();

private:
    Ui::MainWindow *ui;
    chatClient* m_chatClient;
    void userJoined(const QString &user);
};
#endif // MAINWINDOW_H
