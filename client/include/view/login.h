#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

namespace Babibel
{
namespace View
{
class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    bool shouldNotExit() const;
    void setShouldNotExit(bool shouldNotExit);

private slots:
    void on_okButton_clicked();

    void on_createButton_clicked();

    void on_cancelButton_clicked();

signals:
    void debugLogin(const QString &login);

private:
    void closeEvent(QCloseEvent *);

private:
    bool _shouldNotExit;
public:
    Ui::Login *ui;
};
}
}

#endif // LOGIN_H
