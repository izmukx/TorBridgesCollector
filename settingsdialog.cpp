#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    ui->setupUi(this);
    ui->bridgesType->setCurrentText(settings.value("bridgesType").toString());
    ui->proxyType->setCurrentText(settings.value("proxyType").toString());
    if (!settings.contains("torrcFilePath")) {
        ui->displayTorrcPath->setText("Torrc file path..");
    } else if (settings.value("modTorrc").toBool() && settings.contains("torrcFilePath")) {
        ui->displayTorrcPath->setText(settings.value("torrcFilePath").toString());
    }
    if (!settings.contains("bridgesPath") || !settings.value("bridgesPath").toBool()) {
        ui->displayBridgesPath->setText("%user%/tor_bridges.txt");
    } else {
        ui->displayBridgesPath->setText(settings.value("bridgesFilePath").toString());
    }
#ifdef QT_NO_SYSTEMTRAYICON
    ui->showInTray->setEnabled(false);
    ui->closeToTray->setEnabled(false);
#endif
    if (!settings.value("showInSystemTray").toBool()) {
        ui->closeToTray->setEnabled(false);
    }
    ui->showIconInTray->setChecked(settings.value("showInSystemTray").toBool());
    ui->closeToTray->setChecked(settings.value("closeToSystemTray").toBool());
    ui->chooseTorrc->setEnabled(settings.value("modTorrc").toBool());
    ui->chooseBridgesPath->setEnabled(settings.value("bridgesPath").toBool());
    ui->displayBridgesPath->setEnabled(settings.value("bridgesPath").toBool());
    ui->chooseTorrc->setEnabled(settings.value("modTorrc").toBool());
    ui->displayTorrcPath->setEnabled(settings.value("modTorrc").toBool());
    ui->torType->setEnabled(settings.value("modTorrc").toBool());
    ui->proxyPort->setValue(settings.value("proxyPort").toInt());
    ui->modBridgesPath->setChecked(settings.value("bridgesPath").toBool());
    ui->ipv6Support->setChecked(settings.value("ipv6Support").toBool());
    ui->useProxy->setChecked(settings.value("useProxy").toBool());
    ui->modTorrc->setChecked(settings.value("modTorrc").toBool());
    ui->proxyPort->setEnabled(settings.value("useProxy").toBool());
    ui->proxyType->setEnabled(settings.value("useProxy").toBool());
}

void SettingsDialog::on_bridgesType_currentIndexChanged(QString type)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("bridgesType", type);
}

void SettingsDialog::on_proxyType_currentIndexChanged(QString type)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("proxyType", type);
}

void SettingsDialog::on_chooseTorrc_clicked()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QString path = QFileDialog::getOpenFileName(this, tr("Open torrc file"), "", tr("All Files (*)"));
    QFile torrcfile(path);
    if (torrcfile.exists()) {
        ui->displayTorrcPath->setText(path);
        settings.setValue("torrcFilePath", path);
    }
}

void SettingsDialog::on_chooseBridgesPath_clicked()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QString path = QFileDialog::getSaveFileName(this, tr("Save bridges in"), "", tr("All Files (*)"));
    QFile bridgespathfile(path);
    if (bridgespathfile.open(QIODevice::WriteOnly | QIODevice::Text) && bridgespathfile.exists()) {
        ui->displayBridgesPath->setText(path);
        settings.setValue("bridgesFilePath", path);
    }
}

void SettingsDialog::on_modBridgesPath_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    ui->displayBridgesPath->setEnabled(option);
    ui->chooseBridgesPath->setEnabled(option);
    if (!option) {
        settings.remove("bridgesPath");
        settings.remove("bridgesFilePath");
        ui->displayBridgesPath->setText("%user%/tor_bridges.txt");
    } else {
        settings.setValue("bridgesPath", option);
        settings.setValue("bridgesFilePath", QString(QDir::homePath() + "/tor_bridges.txt"));
        ui->displayBridgesPath->setText(settings.value("bridgesFilePath").toString());
    }
}

void SettingsDialog::on_showIconInTray_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("showInSystemTray", option);
    ui->closeToTray->setEnabled(option);
    if (option) {
        QApplication::setQuitOnLastWindowClosed(!settings.value("closeToSystemTray").toBool());
    }
    emit setTrayIcon(int(option));
}

void SettingsDialog::on_closeToTray_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("closeToSystemTray", option);
    if (QSystemTrayIcon::isSystemTrayAvailable() && settings.value("showInSystemTray").toBool()) {
        QApplication::setQuitOnLastWindowClosed(!option);
    }
}

void SettingsDialog::on_proxyPort_valueChanged(int port)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("proxyPort", port);
}

void SettingsDialog::on_ipv6Support_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("ipv6Support", option);
}

void SettingsDialog::on_torType_currentIndexChanged(QString type)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("torType", type);
}

void SettingsDialog::on_modTorrc_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("modTorrc", option);
    ui->displayTorrcPath->setEnabled(option);
    ui->chooseTorrc->setEnabled(option);
    ui->torType->setEnabled(option);
    if (!option) {
        settings.remove("torrcFilePath");
        settings.remove("torType");
        ui->displayTorrcPath->setText("Torrc file path..");
        ui->torType->setCurrentText("daemon");
    }
}

void SettingsDialog::on_useProxy_toggled(bool option)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    settings.setValue("useProxy", option);
    ui->proxyPort->setEnabled(option);
    ui->proxyType->setEnabled(option);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
