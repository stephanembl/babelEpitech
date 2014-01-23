#ifndef ACLIENTTCP_H
#define ACLIENTTCP_H

#include <string>
#include <list>
#include <utility>

namespace Babibel
{
namespace Controller
{
class AClientTCP
{
public:
    AClientTCP();
    virtual ~AClientTCP();

    virtual void connect();
    virtual void setIp(const std::string &ip);
    virtual void setPort(int port);
    virtual void readSocket();
    virtual void writeSocket(const char *data, int size);
    virtual std::list<std::pair<char *, int> > getQueue();

    virtual bool isConnected() const;
    virtual bool hasBeenDisconnected();
    virtual bool isReadable() const;

    std::string error() const;

    bool event() const;
    void setEvent(bool event);

protected:
    std::string _ip;
    int _socket;
    int _port;
    std::list<std::pair<char *, int> > _queue;
    bool _connected;
    bool _readable;
    std::string    _error;
    bool    _event;
    bool    _disconnected;

};
}
}

#endif // ACLIENTTCP_H
