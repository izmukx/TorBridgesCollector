#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFile>
#include <QIODevice>
#include <QFileDialog>
#include <QSettings>
#include <QSystemTrayIcon>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_bridgesType_currentIndexChanged(QString type);
    void on_proxyType_currentIndexChanged(QString type);
    void on_torType_currentIndexChanged(QString type);
    void on_chooseTorrc_clicked();
    void on_proxyPort_valueChanged(int port);
    void on_ipv6Support_toggled(bool option);
    void on_modTorrc_toggled(bool option);
    void on_modBridgesPath_toggled(bool option);
    void on_chooseBridgesPath_clicked();
    void on_useProxy_toggled(bool option);

public slots:
    void on_showIconInTray_toggled(bool option);
    void on_closeToTray_toggled(bool option);

signals:
    void setTrayIcon(int action);

private:
    Ui::SettingsDialog *ui;
    QString bridgesType;
    QString proxyType;
    QString torrcFilePath;
    int proxyPort;
    bool ipv6Support;
    bool useProxy;
    bool modTorrc;
};

#endif // SETTINGSDIALOG_H
