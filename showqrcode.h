#ifndef SHOWQRCODE_H
#define SHOWQRCODE_H

#include <QDialog>

namespace Ui {
class ShowQRCode;
}

class ShowQRCode : public QDialog
{
    Q_OBJECT

public:
    explicit ShowQRCode(QWidget *parent = 0);
    ~ShowQRCode();
    void setQRCode(QPixmap qrcode);

private:
    Ui::ShowQRCode *ui;
};

#endif // SHOWQRCODE_H
