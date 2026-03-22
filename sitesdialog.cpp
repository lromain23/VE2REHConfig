#include "sitesdialog.h"
#include "ui_sitesdialog.h"
#include "mainwindow.h"
#include <qforeach.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPushButton>


SitesDialog::SitesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SitesDialog)
{
    ui->setupUi(this);
    allButtons=findChildren<QPushButton*>();
    for (auto &button : allButtons) {
        if ( button != ui->sendDTMF ) {
          connect(button,&QPushButton::clicked,this,&SitesDialog::CommandButton_clicked);
        }
    }
    loadJsonFromFile();
}

SitesDialog::~SitesDialog()
{
    delete ui;
}

void SitesDialog::CommandButton_clicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QString ButtonCmd = button->property("Command").toString();
    QWidget *pw = button->parentWidget();
    QString SiteID = QString();
    bool done=false;
    while (pw && !done) {
        if (pw->property("SiteID").isValid()) {
            SiteID = pw->property("SiteID").toString();
            done=true;
        }
        pw = pw->parentWidget();
    }
    qDebug() << "SiteID" << SiteID << " Command : " << ButtonCmd;
    if ( ButtonCmd.contains("%") ) {
        emit SendCommand(ButtonCmd.replace("%",SiteID,Qt::CaseInsensitive),ui->enable->isChecked());
    } else {
        emit SendCommand(ButtonCmd,ui->enable->isChecked());
    }
}


void SitesDialog::on_sendDTMF_clicked()
{
    auto dtmf_seq = ui->dtmfSeq->text();
    qDebug() << "Sending DTMF Seq : " << dtmf_seq;
    emit SendCommand(dtmf_seq,ui->enable->isChecked());
}

void SitesDialog::loadJsonFromFile()
{
    QString path =
        QCoreApplication::applicationDirPath()
        + "/ve2rehcfg.json";

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open file:" << path;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc =
        QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON parse error:"
                 << error.errorString();
        return;
    }

    if (!doc.isObject())
    {
        qDebug() << "Invalid JSON root";
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray sites = root["sites"].toArray();
    for (const QJsonValue &siteValue : sites) {
        QJsonObject site = siteValue.toObject();
        QString nom = site["name"].toString();
        QString id  = site["id"].toString();
        qDebug() << "Site:" << nom << "ID:" << id;
        QWidget *site_tab = new QWidget();
        site_tab->setProperty("SiteID",id);
        QVBoxLayout *layout = new QVBoxLayout(site_tab);
    //    layout->addWidget(new QLabel(nom));
        std::list<QGroupBox*> groupbox_list;
        QJsonObject functions = site["functions"].toObject();
        for (auto [key,value] : functions.toVariantMap().asKeyValueRange()) {
            qDebug () << "Category : " << key.toStdString();
            QJsonArray categoryCommands = value.toJsonArray();
            QGroupBox *groupBox = new QGroupBox(site_tab);
            groupBox->setTitle(key);
  //          layout->addWidget(groupBox);
            groupbox_list.push_front(groupBox);
            QHBoxLayout *groupLayout = new QHBoxLayout(groupBox);
            for (const QJsonValue &catCmd  : categoryCommands) {
                QJsonObject dtmfCmd = catCmd.toObject();
                QString cmdName = dtmfCmd["name"].toString();
                QString cmdDTMF = dtmfCmd["command"].toString();
                qDebug() << "Cmd:"<<cmdName<<" DTMF:"<< cmdDTMF;
                QPushButton *button = new QPushButton(cmdName,this);
                button->setProperty("Command",cmdDTMF);
                groupLayout->addWidget(button);
                connect(button,&QPushButton::clicked,this,&SitesDialog::CommandButton_clicked);
            }
        }
        for (auto const &gb : groupbox_list) {
            layout->addWidget(gb);
        }
        ui->SitesTab->addTab(site_tab, nom);

    }
}

