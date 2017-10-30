#ifndef TORBRIDGESCOLLECTOR_H
#define TORBRIDGESCOLLECTOR_H

#include <QMainWindow>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>

namespace Ui {
class TorBridgesCollector;
}

class TorBridgesCollector : public QMainWindow
{
    Q_OBJECT

public:
    explicit TorBridgesCollector(QWidget *parent = 0);
    ~TorBridgesCollector();

public slots:
    void updateLastRetrievalTime(QDateTime time);
    void setTrayIcon(int action);

private slots:
    void on_setRetrieveTime_returnPressed();
    void on_setRetrieveTimeButton_clicked();
    void on_retrieveNowButton_clicked();
    void on_aboutButton_clicked();
    void on_settingsButton_clicked();
    void on_exitButton_clicked();
    void until_next_retrieve_timer();
    void setBridgesButton(bool active);
    void appTrayActivated(QSystemTrayIcon::ActivationReason trayReason);

private:
    Ui::TorBridgesCollector *ui;
    QTime setBridgeRetrieval;
    QTime bridgeRetrievalTime;
    QTimer nextBridgeRetrieval;
    QString getTimeFormat(int timesecs);
    QSettings settings;
    QString torrrcPath;
    void getTime(QString timeraw);
    QStringList bridgesList;
    int bridgesType;
    int timeValue;
    int lastTimeValue;
    bool TorRetrievedBridges = false;
    QAction *windowShow;
    QAction *windowHide;
    QAction *windowQuit;
    QSystemTrayIcon *appTray;
    QMenu *appTrayMenu;
};

#endif // TORBRIDGESCOLLECTOR_H
