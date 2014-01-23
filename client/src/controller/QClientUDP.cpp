#include "include/controller/QClientUDP.h"
#include "../headers-shared/shared/Exception.h"
#include "../headers-shared/shared/LibC.hh"
#include <iostream>

Babibel::Controller::QClientUDP::QClientUDP() : AClientUDP()
{
    QObject::connect(&this->_QSocket, SIGNAL(connected()), this, SLOT(connectOk()));
    QObject::connect(&this->_QSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(&this->_QSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::connect(&this->_QSocket, SIGNAL(readyRead()), this, SLOT(okToRead()));
}

Babibel::Controller::QClientUDP::~QClientUDP()
{
}

void Babibel::Controller::QClientUDP::bind()
{
	int ret = this->_QSocket.bind(QHostAddress::Any, 11234, QAbstractSocket::ReuseAddressHint);
    this->_event = true;
    this->_connected = true;
}

void Babibel::Controller::QClientUDP::connectOk()
{
    this->_connected = true;
    this->_event = true;
}

void Babibel::Controller::QClientUDP::disconnected()
{
    this->_connected = false;
    this->_disconnected = true;
    this->_event = true;
}

void Babibel::Controller::QClientUDP::error(QAbstractSocket::SocketError)
{
    //this->_connected = false;
    //this->_error = this->_QSocket.errorString().toStdString();
    //this->_event = true;
}

void Babibel::Controller::QClientUDP::okToRead()
{
    this->_readable = true;
    this->readSocket();
}

void Babibel::Controller::QClientUDP::setIp(const std::string &ip)
{
    this->_ip = ip;
    this->_QHostAddr.setAddress(ip.c_str());
}

void Babibel::Controller::QClientUDP::setPort(int port)
{
    this->_port = port;
}

void Babibel::Controller::QClientUDP::readSocket()
{
    int maxLen = 4096;
    std::pair<char *, int> line;

    while(this->_QSocket.hasPendingDatagrams())
    {
        line.first = new char[maxLen];
        LibC::memset(line.first, 0, maxLen);
		line.second = this->_QSocket.readDatagram(line.first, maxLen);
        if (this->_disconnected == true || line.second <= 0)
        {
            delete[] line.first;
            break;
        }
      //  std::cout << "READ" << line.second <<std::endl;
        this->_queue.push_back(line);
    }
}

void Babibel::Controller::QClientUDP::writeSocket(const char *data, int size)
{
    if (this->_sip == "")
        return;
	int ret = this->_QSocket.writeDatagram(data, size, QHostAddress(this->_sip.c_str()), this->_port);
 //   std::cout << "WRITE" << std::endl;

    if (ret == -1)
        std::cout << "FATAL RET on " << this->_sip << std::endl;
}

