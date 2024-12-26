#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
private slots:
    void on_loginBt_clicked();
    void on_logout_clicked();
    void on_send_clicked();
    void connectedToServer();
    void messageReceived(const QString &sender,const QString &text);
    void jsonReveived(const QJsonObject & docObj);
private:
    Ui::MainWindow *ui;
    chatClient* m_chatClient;
    void userJoined(const QString &user);
};
#endif // MAINWINDOW_H
