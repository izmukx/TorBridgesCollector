#include "entercaptcha.h"
#include "ui_entercaptcha.h"

#include "getbridgeswindow.h"

#include <QtWidgets>
#include <QDialog>
#include <QSettings>
#include <QMainWindow>
#include <QtNetwork>
#include <QTextEdit>

EnterCaptcha::EnterCaptcha(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterCaptcha),
    requestLoaded(Q_NULLPTR)
{
    ui->setupUi(this);
}

void EnterCaptcha::setProxy()
{
    QNetworkProxy proxy;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    if (settings.value("useProxy").toBool()) {
        if (settings.value("proxyType").toString() == "socks5") {
            proxy = QNetworkProxy::Socks5Proxy;
        } else if (settings.value("proxyType").toString() == "http") {
            proxy = QNetworkProxy::HttpProxy;
        }
        proxy.setHostName("127.0.0.1");
        proxy.setPort(settings.value("proxyPort").toInt());
        proxy.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0");
        QNetworkProxy::setApplicationProxy(proxy);
    } else if (!settings.value("useProxy").toBool()) {
        proxy = QNetworkProxy::DefaultProxy;
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void EnterCaptcha::requestCaptcha()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QString bridgesType;
    if (!settings.contains("bridgesType")) {
        bridgesType = "obfs4";
    } else {
        bridgesType = settings.value("bridgesType").toString();
    }
    setProxy();
    requestedData.clear();
    QUrl requestbridgesUrl(QStringLiteral("https://bridges.torproject.org/bridges"));
    QUrlQuery requestbridgesUrlParams;
    requestbridgesUrlParams.addQueryItem(QString("transport"), bridgesType);
    if (settings.value("ipv6Support").toBool()) {
        requestbridgesUrlParams.addQueryItem(QString("ipv6"), QString("yes"));
    }
    requestbridgesUrl.setQuery(requestbridgesUrlParams);
    QNetworkRequest request(requestbridgesUrl);
    requestLoaded = networkAccess.get(request);
    connect(requestLoaded, &QNetworkReply::finished, this, &EnterCaptcha::captchaLoaded);
    connect(requestLoaded, &QIODevice::readyRead, this, &EnterCaptcha::dataLoaded);
}

void EnterCaptcha::requestBridges(QString captchaInput, QString captchaToken)
{
    ui->submitCaptcha->setEnabled(false);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    QString bridgesType;
    if (!settings.contains("bridgesType")) {
        bridgesType = "obfs4";
    } else {
        bridgesType = settings.value("bridgesType").toString();
    }
    setProxy();
    requestedData.clear();
    QUrl requestbridgesUrl(QStringLiteral("https://bridges.torproject.org/bridges"));
    QUrlQuery requestbridgesUrlParams;
    requestbridgesUrlParams.addQueryItem(QString("transport"), bridgesType);
    if (settings.value("ipv6Support").toBool()) {
        requestbridgesUrlParams.addQueryItem(QString("ipv6"), QString("yes"));
    }
    requestbridgesUrl.setQuery(requestbridgesUrlParams);
    QUrlQuery postData;
    postData.addQueryItem("captcha_challenge_field", captchaToken);
    postData.addQueryItem("captcha_response_field", captchaInput);
    QNetworkRequest request(requestbridgesUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    requestLoaded = networkAccess.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(requestLoaded, &QNetworkReply::finished, this, &EnterCaptcha::bridgesLoaded);
    connect(requestLoaded, &QIODevice::readyRead, this, &EnterCaptcha::dataLoaded);
}

void EnterCaptcha::dataLoaded()
{
    requestedData.append(requestLoaded->readAll());
}

void EnterCaptcha::on_submitCaptcha_accepted()
{
    if (ui->captchaImage->text() != "Failed loading captcha.") {
        requestBridges(ui->captchaText->text(), enterCaptchaToken);
    } else {
        accept();
    }
}

void EnterCaptcha::captchaLoaded()
{
    QRegularExpression parseCaptcha("<div class=\\\"container-narrow\\\" id=\\\"captcha-submission-container\\\">\\n  <div class=\\\"container-fluid container-fluid-inner-5\\\">\\n    <div class=\\\"box\\\" id=\\\"captcha-box\\\">\\n      <img alt=\\\"Your browser is not displaying images properly.\\\" src=\\\"data:image/jpeg;base64,([^<]*)\" />\\n\\n");
    QRegularExpression parseFormId("<input type=\\\"hidden\\\"\\n                     form=\\\"captchaSubmission\\\"\\n                     name=\\\"captcha_challenge_field\\\"\\n                     id=\\\"captcha_challenge_field\\\"\\n                     value=\\\"([^<]*)\\\"></input>\\n");
    QPixmap captchaImage;
    QRegularExpressionMatch captchaReceived = parseCaptcha.match(requestedData);
    if (captchaReceived.hasMatch()) {
        captchaImage.loadFromData(QByteArray::fromBase64(captchaReceived.captured(1).toLatin1()));
        show();
        ui->captchaImage->setPixmap(captchaImage.scaledToWidth(330, Qt::SmoothTransformation));
        enterCaptchaToken = parseFormId.match(requestedData).captured(1);
    } else {
        show();
        ui->captchaImage->setText("Failed loading captcha.");
    }
    emit captchaLoadFinished(true);
}

void EnterCaptcha::bridgesLoaded()
{
    QRegularExpression parseBridgeLines("<div class=\\\"bridge-lines\\\" id=\\\"bridgelines\\\">\\n([^\"]*)<br />\\n          </div>\\n");
    QRegularExpression parseBridgesQRCode("<img title=\\\"QRCode for your bridge lines from BridgeDB\\\"\\n                     src=\\\"data:image/jpeg;base64,([^<]*)\\\"\\n                     alt=\\\"\\\" />\\n");
    QRegularExpression parseNoBridges("There currently aren't any bridges available...\\n");
    QRegularExpression parseCaptcha("<div class=\\\"container-narrow\\\" id=\\\"captcha-submission-container\\\">\\n  <div class=\\\"container-fluid container-fluid-inner-5\\\">\\n    <div class=\\\"box\\\" id=\\\"captcha-box\\\">\\n      <img alt=\\\"Your browser is not displaying images properly.\\\" src=\\\"data:image/jpeg;base64,([^<]*)\" />\\n\\n");
    QString wrongCaptcha = "\n<html>\n    <head>\n        <meta http-equiv=\"refresh\" content=\"0;URL=/bridges?transport=obfs4\">\n    </head>\n    <body bgcolor=\"#FFFFFF\" text=\"#000000\">\n    <a href=\"/bridges?transport=obfs4\">click here</a>\n    </body>\n</html>\n";
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TorBridgesCollector", "settings");
    GetBridgesWindow *bridgesWindow = new GetBridgesWindow(this);
    QString bridgesOutput;
    QString torrcModOutput;
    QRegularExpressionMatch bridgesMatch = parseBridgeLines.match(requestedData);
    if (bridgesMatch.hasMatch()) {
        QPixmap bridgesQRCode;
        bridgesQRCode.loadFromData(QByteArray::fromBase64(parseBridgesQRCode.match(requestedData).captured(1).toLatin1()));
        QList<QString> bridgeLines = bridgesMatch.captured(1).split("<br />");
        bridgesOutput = bridgeLines.join("\n");
        QDateTime retrievalTime = QDateTime::currentDateTime();
        settings.setValue("lastRetrievalTime", retrievalTime);
        emit bridgesDataLoaded(retrievalTime);
        if (settings.contains("bridgesPath") && settings.contains("bridgesFilePath")) {
            QFile bridgesFile(settings.value("bridgesFilePath").toString());
            QString bridgesFileData;
            if (bridgesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                bridgesFileData = bridgesFile.readAll();
                bridgesFile.close();
            }
            if (bridgesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream bridgesFileWrite(&bridgesFile);
                bridgesFileWrite << bridgesFileData << "#Retrieved on " + retrievalTime.toString().toUtf8() + ". Bridges type - " + settings.value("bridgesType").toString().toUtf8() + "\n\n" + bridgesOutput.toUtf8() + "\n\n\n";
                bridgesFile.close();
            }
        }
        foreach (QString bridgeLine, bridgeLines) {
            bridgeLines[bridgeLines.indexOf(bridgeLine)].replace("\n", "").prepend("Bridge ");
        }
        if (settings.contains("modTorrc") && settings.contains("torrcFilePath")) {
            QFile torrcFile(settings.value("torrcFilePath").toString());
            QString torrcFileData;
            if (torrcFile.exists()) {
                if (torrcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    torrcFileData = torrcFile.readAll();
                    torrcFile.close();
                }
                if (torrcFileData.contains("\nDataDirectory")) {
                    QString useBridges;
                    QString clientTransportLine;
                    if (settings.value("torType").toString() == "daemon") {
                        QRegularExpression bridgeLine("Bridge .*\n");
                        QRegularExpression bridgeClientTransportLine("\nClientTransportPlugin.*\n");
                        if (settings.value("bridgesType").toString() == "fte") {
                            clientTransportLine = "\nClientTransportPlugin fte exec /usr/bin/fteproxy --managed\n";
                        } else {
                            if (settings.value("bridgesType").toString() != "normal") {
                                clientTransportLine = "\nClientTransportPlugin obfs2,obfs3,obfs4,scramblesuit exec /usr/bin/obfs4proxy\n";
                            }
                        }
                        if (!torrcFileData.contains("\nUseBridges 1")) {
                            useBridges = "\nUseBridges 1";
                        }
                        torrcFileData.replace(bridgeLine, "");
                        torrcFileData.replace(bridgeClientTransportLine, "");
                    }
                    QString torrcFileOutput = torrcFileData + useBridges + "\n" + bridgeLines.join("\n") + clientTransportLine;
                    if (torrcFile.open(QIODevice::WriteOnly | QIODevice::Text) && torrcFile.permissions() & QFileDevice::WriteUser) {
                        QTextStream torrcFileWrite(&torrcFile);
                        torrcFileWrite << torrcFileOutput;
                        torrcFile.close();
                    } else {
#ifndef __unix__
                        torrcModOutput = "Failed to modify " + settings.value("torrcFilePath").toString() + " torrc file. Perhaps you should try running Tor Bridges Collector as administrator?";
#else
                        if (QFile("/usr/bin/pkexec").exists() && QFile("/usr/bin/cp").exists()) {
                            QFile tempTorrc(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/.temptorrc");
                            if (tempTorrc.open(QIODevice::WriteOnly | QIODevice::Text)) {
                                QTextStream outTempTorrc(&tempTorrc);
                                outTempTorrc << torrcFileOutput;
                                tempTorrc.close();
                                system("pkexec cp " + tempTorrc.fileName().toUtf8() + " " + settings.value("torrcFilePath").toString().toUtf8());
                            }
                            tempTorrc.remove();
                        } else {
                            torrcModOutput = "Failed to modify " + settings.value("torrcFilePath").toString() + " torrc file. Perhaps you should try running Tor Bridges Collector as administrator?";
                        }
#endif
                    }
                }
            }
        }
        bridgesWindow->showRetrievedBridges(bridgesOutput, bridgesQRCode.scaledToWidth(400, Qt::FastTransformation), torrcModOutput);
        accept();
    } else if (parseNoBridges.match(requestedData).hasMatch()) {
        ui->errorMessage->setText("Currently no bridges of specified type are available.");
        ui->captchaImage->setText("Loading captcha...");
        requestCaptcha();
    } else if (parseCaptcha.match(requestedData).hasMatch() || requestedData == wrongCaptcha) {
        ui->errorMessage->setText("Wrong captcha. Please try again.");
        ui->captchaImage->setText("Loading captcha...");
        requestCaptcha();
    } else {
        ui->errorMessage->setText("Could not get bridges.");
        ui->captchaImage->setText("Loading captcha...");
        requestCaptcha();
    }
    ui->captchaText->clear();
    ui->submitCaptcha->setEnabled(true);
}

EnterCaptcha::~EnterCaptcha()
{
    delete ui;
}
