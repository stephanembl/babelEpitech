#pragma once

#include <iostream>
#include <cstring>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "LibC.hh"

#define maxLength 2048

class	TCPBoostClient
{
private:
	void	handleRead(const boost::system::error_code& error, size_t bytesTransferred)
	{
		if (!error)
		{
			char *tmp = new char[bytesTransferred + 1];

			LibC::memset(tmp, 0, bytesTransferred + 1);
			LibC::memcpy(tmp, this->_data, bytesTransferred);
			this->_datas.push_back(std::triple<unsigned int, char*, size_t>(this->_myId, tmp, bytesTransferred));
			LibC::memset(this->_data, 0, bytesTransferred);
			this->_socket.async_read_some(boost::asio::buffer(this->_data, maxLength),
				boost::bind(&TCPBoostClient::handleRead, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			// std::cout << "Id: " << this->_myId << " -> " << error.message() << std::endl;
			delete this;
		}
	}

	void	handleWrite(const boost::system::error_code& error)
	{
		if (error)
		{
 			// std::cout << "Id: " << this->_myId << " -> " << error.message() << std::endl;
			delete this;
		}
	}
	boost::asio::ip::tcp::socket				_socket;
	std::map<unsigned int, TCPBoostClient*>&		_client;
	std::list<std::pair<bool, unsigned int> >&		_updateIdClient;
	std::list<std::triple<unsigned int, char*, size_t> >&	_datas;
	unsigned int						_myId;
	char							_data[maxLength];
public:
	TCPBoostClient(boost::asio::io_service& ioService, std::map<unsigned int, TCPBoostClient*>& client,
		std::list<std::pair<bool, unsigned int> >& updateIdClient, std::list<std::triple<unsigned int, char*, size_t> >& datas)
		: _socket(ioService), _client(client), _updateIdClient(updateIdClient), _datas(datas)
	{
	}
	virtual ~TCPBoostClient()
	{
		this->_updateIdClient.push_back(std::pair<bool, unsigned int>(false, this->_myId));
		this->_client.erase(this->_myId);
	}

	boost::asio::ip::tcp::socket&	socket()
	{
		return _socket;
	}

	void							setMyId(unsigned int id)
	{
		this->_myId = id;
	}

	void							start()
	{
		LibC::memset(this->_data, 0, maxLength);
		this->_socket.async_read_some(boost::asio::buffer(this->_data, maxLength),
			boost::bind(&TCPBoostClient::handleRead, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		// std::cout << "CA SE CONNECTE ID = " << this->_myId << std::endl;
	}

	void							write(const char *data, size_t size)
	{
		boost::asio::async_write(this->_socket,
			boost::asio::buffer(data, size),
			boost::bind(&TCPBoostClient::handleWrite, this,
			boost::asio::placeholders::error));
	}

	void							close()
	{
		this->_socket.close();
	}

	std::string						getEndPoint()
	{
	  return (this->_socket.remote_endpoint().address().to_string());
	}
};
