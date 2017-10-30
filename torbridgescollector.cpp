#include "torbridgescollector.h"
#include "settingsdialog.h"
#include "aboutpage.h"
#include "entercaptcha.h"
#include "ui_torbridgescollector.h"

TorBridgesCollector::TorBridgesCollector(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TorBridgesCollector)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    ui->setupUi(this);
    if (!settings.contains("retrieve_interval")) {
        settings.setValue("retrieve_interval", 86400);
    }
    if (!settings.contains("next_retrieval") || !settings.value("next_retrieval").toDateTime().toUTC().isValid()) {
        settings.setValue("next_retrieval", QDateTime::currentDateTimeUtc().addSecs(settings.value("retrieve_interval").toInt()));
    }
    if (!settings.contains("bridgesType")) {
        settings.setValue("bridgesType", "obfs4");
    }
    if (!settings.contains("proxyType")) {
        settings.setValue("proxyType", "socks5");
    }
    if (!settings.contains("proxyPort")) {
        settings.setValue("proxyPort", 9050);
    }
    if (!settings.contains("ipv6Support")) {
        settings.setValue("ipv6Support", false);
    }
    if (!settings.contains("useProxy")) {
        settings.setValue("useProxy", false);
    }
    if (!settings.contains("modTorrc")) {
        settings.setValue("modTorrc", false);
    }
    if (!settings.contains("torType")) {
        settings.setValue("torType", "daemon");
    }
    if (!settings.contains("bridgesPath")) {
        settings.setValue("bridgesPath", false);
    }
    if (!settings.contains("showInSystemTray")) {
        settings.setValue("showInSystemTray", true);
    }
    if (!settings.contains("closeToSystemTray")) {
        settings.setValue("closeToSystemTray", true);
    }
#ifndef QT_NO_SYSTEMTRAYICON
    appTray = new QSystemTrayIcon(this);
    appTray->setIcon(QIcon(":/images/status/normal.png"));
    windowShow = new QAction(tr("Show"), this);
    connect(windowShow, &QAction::triggered, this, &QWidget::showNormal);
    windowHide = new QAction(tr("Hide"), this);
    connect(windowHide, &QAction::triggered, this, &QWidget::hide);
    windowQuit = new QAction(tr("Exit"), this);
    connect(windowQuit, &QAction::triggered, this, &QApplication::quit);
    appTrayMenu = new QMenu(this);
    connect(windowQuit, &QAction::triggered, this, &QApplication::quit);
    appTrayMenu->addAction(windowShow);
    appTrayMenu->addAction(windowHide);
    appTrayMenu->addAction(windowQuit);
    appTray->setContextMenu(appTrayMenu);
    connect(appTray, &QSystemTrayIcon::messageClicked, this, &TorBridgesCollector::on_retrieveNowButton_clicked);
    connect(appTray, &QSystemTrayIcon::activated, this, &TorBridgesCollector::appTrayActivated);
    if (QSystemTrayIcon::isSystemTrayAvailable() && settings.value("showInSystemTray").toBool()) {
        appTray->show();
        QApplication::setQuitOnLastWindowClosed(!settings.value("closeToSystemTray").toBool());
    }
#endif
    lastTimeValue = settings.value("retrieve_interval").toInt();
    ui->setRetrieveTime->setText(getTimeFormat(settings.value("retrieve_interval").toInt()));
    ui->retrieveCountDownBar->setMaximum(settings.value("retrieve_interval").toInt());
    ui->retrieveCountDownBar->setValue(settings.value("retrieve_interval").toInt());
    QTimer *nextCheck = new QTimer(this);
    connect(nextCheck, SIGNAL(timeout()), this, SLOT(until_next_retrieve_timer()));
    nextCheck->start(1000);
    until_next_retrieve_timer();
    if (settings.contains("lastRetrievalTime")) {
        ui->lastRetrievalTime->setText("Last retrieval time: " + settings.value("lastRetrievalTime").toDateTime().toString());
    }
}

QString TorBridgesCollector::getTimeFormat(int timesecs)
{
    QStringList timecells;
    timecells << QString::number(timesecs/(24*3600)) << QString::number((timesecs%(24*3600))/3600) << QString::number((timesecs/60)%60) << QString::number(timesecs%60);
    for (int i(0); i < timecells.count(); i++) {
        if (timecells[i].size() == 0) {
            timecells[i] = "00";
        } else if (timecells[i].size() == 1) {
            timecells[i].prepend("0");
        } else if (timecells[i].size() > 2) {
            timecells[i].resize(2);
        }
    }
    return timecells.join(":");
}

void TorBridgesCollector::setBridgesButton(bool active)
{
    if (active) {
        ui->retrieveNowButton->setEnabled(true);
        ui->retrieveNowButton->setText("Retrieve now");
    } else {
        ui->retrieveNowButton->setEnabled(false);
        ui->retrieveNowButton->setText("Loading...");
    }
}

void TorBridgesCollector::updateLastRetrievalTime(QDateTime time)
{
    ui->lastRetrievalTime->setText("Last retrieval time: " + time.toString());
}

void TorBridgesCollector::on_setRetrieveTime_returnPressed()
{
    getTime(ui->setRetrieveTime->text());
    TorRetrievedBridges = false;
}

void TorBridgesCollector::on_setRetrieveTimeButton_clicked()
{
    getTime(ui->setRetrieveTime->text());
    TorRetrievedBridges = false;
}

void TorBridgesCollector::on_retrieveNowButton_clicked()
{
    appTray->setIcon(QIcon(":/images/status/normal.png"));
    EnterCaptcha *captchaWindow = new EnterCaptcha(this);
    setBridgesButton(false);
    connect(captchaWindow, SIGNAL(captchaLoadFinished(bool)), this, SLOT(setBridgesButton(bool)));
    connect(captchaWindow, SIGNAL(bridgesDataLoaded(QDateTime)), this, SLOT(updateLastRetrievalTime(QDateTime)));
    captchaWindow->requestCaptcha();
}

void TorBridgesCollector::on_aboutButton_clicked()
{
    AboutPage *aboutWindow = new AboutPage(this);
    aboutWindow->show();
}

void TorBridgesCollector::on_settingsButton_clicked()
{
    SettingsDialog *settingsWindow = new SettingsDialog(this);
    connect(settingsWindow, SIGNAL(setTrayIcon(int)), this, SLOT(setTrayIcon(int)));
    settingsWindow->show();
}

void TorBridgesCollector::on_exitButton_clicked()
{
    QApplication::quit();
}

void TorBridgesCollector::appTrayActivated(QSystemTrayIcon::ActivationReason trayReason)
{
    switch (trayReason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (!TorBridgesCollector::isHidden()) {
            hide();
        } else {
            show();
        }
        break;
    default:
        ;
    }
}

void TorBridgesCollector::getTime(QString timeraw)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QStringList text_time = timeraw.split(":");
    if (text_time.count() == 4) {
        int value = ((text_time[0].toInt()*24*3600) + (text_time[1].toInt()*3600) + (text_time[2].toInt()*60) + text_time[3].toInt());
        if (getTimeFormat(value) != timeraw) {
            timeraw = getTimeFormat(value);
            ui->setRetrieveTime->setText(timeraw);
        }
        QString time_correct = getTimeFormat(value);
        if (value < 31536000 && value > 9) {
            if (timeraw != time_correct) {
                ui->setRetrieveTime->setText(time_correct);
            }
            lastTimeValue = value;
            settings.setValue("next_retrieval", QDateTime::currentDateTimeUtc().addSecs(value));
            settings.setValue("retrieve_interval", value);
            ui->retrieveCountDownBar->setMaximum(value);
            ui->retrieveCountDownBar->setValue(value);
        } else if (value == 0) {
            ui->retrieveCountDownBar->setMaximum(value);
            ui->retrieveCountDownBar->setValue(value);
            ui->retrieveHoursCountDown->setText("none");
        } else {
            ui->retrieveCountDownBar->setMaximum(lastTimeValue);
            ui->retrieveCountDownBar->setValue(lastTimeValue);
            ui->setRetrieveTime->setText(getTimeFormat(lastTimeValue));
        }
    } else {
        ui->setRetrieveTime->setText(getTimeFormat(lastTimeValue));
    }
}

void TorBridgesCollector::setTrayIcon(int action)
{
#ifndef QT_NO_SYSTEMTRAYICON
    if (action == 0) {
        QApplication::setQuitOnLastWindowClosed(true);
        appTray->setVisible(false);
    } else if (action == 1) {
        appTray->setVisible(true);
    } else {
        setTrayIcon(int(!appTray->isVisible()));
    }
#endif
}

void TorBridgesCollector::until_next_retrieve_timer()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QDateTime lastRetrieval = settings.value("next_retrieval").toDateTime().toUTC();
    int remaining_time = QDateTime::currentDateTimeUtc().secsTo(lastRetrieval);
    if (settings.value("retrieve_interval").toInt() > 0) {
        if (remaining_time <= 0) {
            while (remaining_time <= 0) {
                if (!TorRetrievedBridges) {
                    appTray->setIcon(QIcon(":/images/status/alert.png"));
                    appTray->showMessage("Time to get new bridges", "Click to retrieve.", QIcon(":/images/status/alert.png"), 5000);
                    TorRetrievedBridges = true;
                }
                lastRetrieval = settings.value("next_retrieval").toDateTime().toUTC().addSecs(settings.value("retrieve_interval").toInt());
                settings.setValue("next_retrieval", lastRetrieval);
                remaining_time = QDateTime::currentDateTimeUtc().secsTo(lastRetrieval);
            }
        } else if (remaining_time > (settings.value("retrieve_interval").toInt() - 5) && TorRetrievedBridges) {
            appTray->setIcon(QIcon(":/images/status/alert.png"));
        } else {
            TorRetrievedBridges = false;
            appTray->setIcon(QIcon(":/images/status/normal.png"));
        }
        ui->retrieveCountDownBar->setValue(remaining_time);
        ui->retrieveHoursCountDown->setText(getTimeFormat(remaining_time));
    } else {
        appTray->setIcon(QIcon(":/images/status/inactive.png"));
        ui->retrieveHoursCountDown->setText("none");
    }
}

TorBridgesCollector::~TorBridgesCollector()
{
    delete ui;
}
