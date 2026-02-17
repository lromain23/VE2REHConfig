#include "sitesdialog.h"
#include "ui_sitesdialog.h"
#include "mainwindow.h"
#include <qforeach.h>
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
        emit SendCommand(ButtonCmd.replace("%",SiteID,Qt::CaseInsensitive));
    } else {
        emit SendCommand(ButtonCmd);
    }
}


void SitesDialog::on_sendDTMF_clicked()
{
    auto dtmf_seq = ui->dtmfSeq->text();
    qDebug() << "Sending DTMF Seq : " << dtmf_seq;
    emit SendCommand(dtmf_seq);
}

