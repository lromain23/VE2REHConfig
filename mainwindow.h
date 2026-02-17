#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <qlineedit.h>
#include <qnetworkreply.h>

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
//    void sendDTMF(QString cmd);

private slots:
    void on_pushButton_clicked();
//    void onWebFinished(QNetworkReply* reply);
    void onReplyFinished();
    void onPostFinished();
    void save_key(QString key, QString value);
    void read_key(QString key, QLineEdit *widget);

public slots:
    void sendDTMF(QString cmd);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QNetworkAccessManager> manager;
    static QString USERNAME_KEY;
    static QString PASSWORD_KEY;

};
#endif // MAINWINDOW_H
