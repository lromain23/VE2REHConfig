#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtNetwork>
#include <QNetworkCookieJar>
#include "sitesdialog.h"
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <qtkeychain/keychain.h>

QString MainWindow::USERNAME_KEY = "VE2REHConfig/USERNAME";
QString MainWindow::PASSWORD_KEY = "VE2REHConfig/PASSWORD";

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
    // Cancel any pending network requests before destruction
    if (manager) {
        // Reset the manager to clean up resources properly
        manager.reset();
    }
    
    // Ensure UI is deleted properly
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

    // Store credentials for later use
    m_username = userName;
    m_password = password;

    // Set up authentication for future requests
    connect(manager.get(), &QNetworkAccessManager::authenticationRequired,
            this,
            [this](QNetworkReply *reply, QAuthenticator *authenticator)
            {
                Q_UNUSED(reply);
                authenticator->setUser(m_username);
                authenticator->setPassword(m_password);
            });

    QNetworkRequest request(QUrl("http://irlp.ve2reh.net:15426/dtmf/index.php"));

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, &MainWindow::onReplyFinished);
    // Don't call deleteLater() here as it's managed by Qt's object hierarchy
}
void MainWindow::save_key(QString key, QString value)
{
    auto *job = new QKeychain::WritePasswordJob("VE2REHConfig", this);

    job->setKey(key);
    job->setTextData(value);

    connect(job, &QKeychain::WritePasswordJob::finished,
            this, [job]() {

                if (job->error()) {
                    qDebug() << "Failed to save password:"
                             << job->errorString();
                } else {
                    qDebug() << "Password saved successfully";
                }
            });

    job->start();
}
void MainWindow::read_key(QString key, QLineEdit *widget)
{
#ifdef Q_OS_WIN

    auto *job = new QKeychain::ReadPasswordJob("VE2REHConfig", this);

    job->setKey(key);

    connect(job, &QKeychain::ReadPasswordJob::finished,
            this, [job, widget]() {

                if (job->error()) {
                    qDebug() << "Failed to read key:"
                             << job->errorString();
                    return;
                }

                widget->setText(job->textData());
            });

    job->start();

#else

    QSettings settings;
    QString value = settings.value(key).toString();
    widget->setText(value);

#endif
}

void MainWindow::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    
    // Check if reply is valid before proceeding
    if (!reply) {
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
    } else {
        qDebug() << "Page content:";
        qDebug() << reply->readAll();
        if ( ui->saveLogin->isChecked() ) {
          save_key(MainWindow::USERNAME_KEY,ui->userName->text());
          save_key(MainWindow::PASSWORD_KEY,ui->password->text());
        }
        
        // Create and show the dialog properly
        SitesDialog *sitesDlg = new SitesDialog(this);
        sitesDlg->setAttribute(Qt::WA_DeleteOnClose);
        connect(sitesDlg,&SitesDialog::SendCommand,this,&MainWindow::sendDTMF);
        connect(sitesDlg, &QDialog::finished, this, &MainWindow::onSitesDialogClosed); // Connect to dialog finished signal
        this->hide();
        sitesDlg->exec();
    }
    
    // Clean up the reply properly
    reply->deleteLater();
}

void MainWindow::onSitesDialogClosed() {
    // When the SitesDialog closes, quit the application
    QApplication::quit();
}
