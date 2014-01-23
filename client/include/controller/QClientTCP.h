#ifndef QCLIENTTCP_H
#define QCLIENTTCP_H

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QTcpSocket>
#include "AClientTCP.h"

namespace Babibel
{
namespace Controller
{
class QClientTCP : public QObject, public AClientTCP
{
Q_OBJECT
public:
    QClientTCP();
    ~QClientTCP();

    void connect();
    void setIp(const std::string &ip);
    void setPort(int port);
    void readSocket();
    void writeSocket(const char *data, int size);

private slots :
    void connectOk();
    void disconnected();
    void okToRead();
    void error(QAbstractSocket::SocketError);
private:
    QTcpSocket _QSocket;
};
}
}
#endif // QCLIENTTCP_H
