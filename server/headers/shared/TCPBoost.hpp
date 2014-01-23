#pragma once

#include "INetwork.hh"
#include "TCPBoostClient.hpp"

class	TCPBoost : public INetwork
{
private:
	void	startAccept()
	{
		TCPBoostClient* newTCPBoostClient = new TCPBoostClient(this->_ioService, this->_client, this->_updateIdClient, this->_datas);

		this->_acceptor.async_accept(newTCPBoostClient->socket(),
			boost::bind(&TCPBoost::handleAccept, this, newTCPBoostClient,
			boost::asio::placeholders::error));
	}

	void	handleAccept(TCPBoostClient* newTCPBoostClient, const boost::system::error_code& error)
	{
		static unsigned int id = 0;

		if (!error)
		{
			this->_client[++id] = newTCPBoostClient;
			this->_updateIdClient.push_back(std::pair<bool, unsigned int>(true, id));
			newTCPBoostClient->setMyId(id);
			newTCPBoostClient->start();
		}
		else
			delete newTCPBoostClient;
		this->startAccept();
	}

	TCPBoost(const TCPBoost& copy);
	TCPBoost&							operator=(const TCPBoost& other);
	std::map<unsigned int, TCPBoostClient*>				_client;
	std::list<std::pair<bool, unsigned int> >			_updateIdClient;
	boost::asio::io_service						_ioService;
	boost::asio::ip::tcp::acceptor					_acceptor;
	std::list<std::triple<unsigned int, char*, size_t> >		_datas;
public:
	TCPBoost(unsigned short port)
		: _acceptor(this->_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		this->startAccept();
	}
	virtual ~TCPBoost()
	{
	  for (std::map<unsigned int, TCPBoostClient*>::iterator it = this->_client.begin();
	       it != this->_client.end(); ++it)
	    {
	      delete it->second;
	    }
	  this->_client.clear();
	}

	void													update()
	{
		this->_ioService.poll();
	}

  void													send(unsigned int id, const char *data, size_t size)
	{
		std::map<unsigned int, TCPBoostClient*>::iterator	it;

		it = this->_client.find(id);
		if (it != this->_client.end())
		  it->second->write(data, size);
	}

	std::list<std::triple<unsigned int, char*, size_t> >&	receive()
	{
		return (this->_datas);
	}

	std::list<std::pair<bool, unsigned int> >&				updateIdClient()
	{
		return (this->_updateIdClient);
	}

	void													closeId(unsigned int id)
	{
		std::map<unsigned int, TCPBoostClient*>::iterator	it;

		it = this->_client.find(id);
		if (it != this->_client.end())
		  it->second->close();
	}

	std::string												getEndPointId(unsigned int id) const
	{
		std::map<unsigned int, TCPBoostClient*>::const_iterator	it;

		it = this->_client.find(id);
		return ((it != this->_client.end()) ? (it->second->getEndPoint()) : (""));
	}
};
