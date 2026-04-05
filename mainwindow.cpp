#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtNetwork>
#include <QNetworkCookieJar>
#include "sitesdialog.h"
#include <QDebug>
#include <qtkeychain/keychain.h>

QString MainWindow::USERNAME_KEY = "VE2REHConfig/PASSWORD";
QString MainWindow::PASSWORD_KEY = "VE2REHConfig/USERNAME";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    read_key(MainWindow::USERNAME_KEY,ui->userName);
    read_key(MainWindow::PASSWORD_KEY,ui->password);
    manager = std::make_unique<QNetworkAccessManager>(this);
    QNetworkCookieJar *cookieJar = new QNetworkCookieJar(this);
    manager->setCookieJar(cookieJar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendDTMF(QString cmd, bool enable) {
    if ( manager && enable) {
        QUrlQuery postData;
        postData.addQueryItem("dtmf_regen",QString("Submit"));
        postData.addQueryItem("dtmf_r", cmd);
        // Post request
        QUrl url("http://irlp.ve2reh.net:15426/dtmf/index.php");
        QNetworkRequest postRequest(url);

        postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *postReply = manager->post(postRequest, postData.query(QUrl::FullyEncoded).toUtf8());
        // Connect to post reply finished signal
        connect(postReply, &QNetworkReply::finished, this, &MainWindow::onPostFinished);
    }
    qDebug() << "Sending Command to web : " << cmd << " Enable : " << enable;
}

void MainWindow::onPostFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "POST failed:" << reply->errorString();
        } else {
            qDebug() << "POST successful, response:";
            qDebug() << reply->readAll();
        }
        reply->deleteLater();
    }

}
void MainWindow::on_pushButton_clicked()
{
    auto userName = ui->userName->text();
    auto password = ui->password->text();

    connect(manager.get(), &QNetworkAccessManager::authenticationRequired,
            this,
            [userName,password](QNetworkReply *reply, QAuthenticator *authenticator)
            {
                Q_UNUSED(reply);
                authenticator->setUser(userName);
                authenticator->setPassword(password);
            });

    QNetworkRequest request(QUrl("http://irlp.ve2reh.net:15426/dtmf/index.php"));

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, &MainWindow::onReplyFinished);

}
void MainWindow::save_key(QString key, QString value)
{
    auto *job = new QKeychain::WritePasswordJob("VE2REHConfig");

    job->setKey(key);
    job->setTextData(value);
    connect(job, &QKeychain::Job::finished,
            [job]() {
                if (job->error()) {
                    qDebug() << "Error:" << job->errorString();
                }
                job->deleteLater();
            });

    job->start();
}

void MainWindow::read_key(QString key, QLineEdit *widget)
{
    auto *job = new QKeychain::ReadPasswordJob("VE2REHConfig", this);
    job->setKey(key);
    connect(job, &QKeychain::Job::finished,
            this, [this, job, widget]() {

                if (job->error()) {
                    qDebug() << "Keychain error:" << job->errorString();
                } else {
                    widget->setText(job->textData());
                }
                job->deleteLater();
            });
    job->start();
}

void MainWindow::onReplyFinished() {
    SitesDialog *sitesDlg;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
    } else {
        qDebug() << "Page content:";
        qDebug() << reply->readAll();
        if ( ui->saveLogin->isChecked() ) {
          save_key(MainWindow::USERNAME_KEY,ui->userName->text());
          save_key(MainWindow::PASSWORD_KEY,ui->password->text());
        }
        sitesDlg = new SitesDialog(this);
        sitesDlg->setAttribute(Qt::WA_DeleteOnClose);
        connect(sitesDlg,&SitesDialog::SendCommand,this,&MainWindow::sendDTMF);
        this->hide();
        sitesDlg->exec();
    }
    reply->deleteLater();
    qApp->quit();
}

