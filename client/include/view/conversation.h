#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class Conversation;
}

namespace Babibel
{
namespace View
{
class Conversation : public QWidget
{
    Q_OBJECT

    QString     _login;
public:
    explicit Conversation(QWidget *parent = 0);
    ~Conversation();

    void    setLogin(const QString &login);
    void    refresh();
    void    call();

signals:
    int closeConversation(const QString &login);

private slots:
    void                closeEvent(QCloseEvent *event);

    void on_sendButton_clicked();

    void on_lineEdit_returnPressed();

    void on_callButton_clicked();

    void acceptorreject(QAbstractButton *btn);

public:
    Ui::Conversation *ui;
};
}
}

#endif // CONVERSATION_H
