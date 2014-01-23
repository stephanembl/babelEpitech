#ifndef QCLIENTUDP_H
#define QCLIENTUDP_H

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QUdpSocket>
#include "AClientUDP.h"

namespace Babibel
{
namespace Controller
{
class QClientUDP : public QObject, public AClientUDP
{
Q_OBJECT
public:
    QClientUDP();
    ~QClientUDP();

    void bind();
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
    QUdpSocket _QSocket;
    QHostAddress _QHostAddr;
};
}
}

#endif // QCLIENTUDP_H
