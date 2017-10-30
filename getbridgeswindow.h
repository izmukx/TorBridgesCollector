#ifndef GETBRIDGESWINDOW_H
#define GETBRIDGESWINDOW_H

#include <QtWidgets>
#include <QDialog>
#include <QMainWindow>

namespace Ui {
class GetBridgesWindow;
}

class GetBridgesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GetBridgesWindow(QWidget *parent = 0);
    ~GetBridgesWindow();
    void showRetrievedBridges(QString bridges, QPixmap qrcode, QString message);

private slots:
    void on_copyToClipboard_clicked();
    void on_showQRCode_clicked();

private:
    Ui::GetBridgesWindow *ui;
    QPixmap BridgesQRCode;

};
#endif // GETBRIDGESWINDOW_H
