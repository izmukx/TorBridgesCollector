#ifndef ENTERCAPTCHA_H
#define ENTERCAPTCHA_H

#include <QtWidgets>
#include <QDialog>
#include <QSettings>
#include <QtNetwork>
#include <QMainWindow>
#include <QTextEdit>

namespace Ui {
class EnterCaptcha;
}

class EnterCaptcha : public QDialog
{
    Q_OBJECT

public:
    explicit EnterCaptcha(QWidget *parent = 0);
    ~EnterCaptcha();
    void requestCaptcha();
    void requestBridges(QString captchaInput, QString captchaToken);

signals:
    void captchaLoadFinished(bool finished);
    void bridgesDataLoaded(QDateTime time);

public slots:
    void bridgesLoaded();

private slots:
    void on_submitCaptcha_accepted();
    void dataLoaded();

private:
    void captchaLoaded();
    Ui::EnterCaptcha *ui;
    QString enterCaptchaToken;
    QNetworkReply *requestLoaded;
    QByteArray requestedData;
    QNetworkAccessManager networkAccess;
    void setProxy();
};

#endif // ENTERCAPTCHA_H
