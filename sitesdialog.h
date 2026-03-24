#ifndef SITESDIALOG_H
#define SITESDIALOG_H

#include <QDialog>

namespace Ui {
class SitesDialog;
}

class SitesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SitesDialog(QWidget *parent = nullptr);
    ~SitesDialog();
    QByteArray SiteData;

private slots:
    void CommandButton_clicked();
    void buildUIfromJSON(void);
    void on_sendDTMF_clicked();

private:
    Ui::SitesDialog *ui;
    QList<QPushButton*> allButtons;
    void loadJsonFromFile(void);
    void loadJsonFromURL(void);

signals:
    void SendCommand(QString command, bool enable);
    void JsonDataReady(void);
};

#endif // SITESDIALOG_H
