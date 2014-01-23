#include "include/controller/AClientTCP.h"

Babibel::Controller::AClientTCP::AClientTCP()
{
    this->_connected = false;
    this->_readable = false;
    this->_disconnected = false;
}

Babibel::Controller::AClientTCP::~AClientTCP()
{
}

void Babibel::Controller::AClientTCP::connect()
{
}

void Babibel::Controller::AClientTCP::setIp(const std::string &ip)
{
    this->_ip = ip;
}

void Babibel::Controller::AClientTCP::setPort(int)
{
}

void Babibel::Controller::AClientTCP::readSocket()
{
}

void Babibel::Controller::AClientTCP::writeSocket(const char *data, int size)
{
    (void)data;
    (void)size;
}

std::list<std::pair<char *, int> > Babibel::Controller::AClientTCP::getQueue()
{
    std::list<std::pair<char *, int> > retQueue;

    this->_readable = false;
    retQueue = this->_queue;
    this->_queue.clear();
    return (retQueue);
}

bool Babibel::Controller::AClientTCP::isConnected() const
{
    return (this->_connected);
}

bool Babibel::Controller::AClientTCP::hasBeenDisconnected()
{
    if (this->_disconnected == true)
    {
        this->_disconnected = false;
        return (true);
    }
    return (false);
}


bool Babibel::Controller::AClientTCP::isReadable() const
{
    return (this->_readable);
}

std::string Babibel::Controller::AClientTCP::error() const
{
    return _error;
}
bool Babibel::Controller::AClientTCP::event() const
{
    return _event;
}

void Babibel::Controller::AClientTCP::setEvent(bool event)
{
    _event = event;
}


