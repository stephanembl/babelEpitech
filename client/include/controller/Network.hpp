#pragma		once

#include	<iostream>
#include	<queue>
#include	"../headers-shared/shared/PacketFactory.h"
#include    "include/controller/QClientTCP.h"
#include    "include/controller/QClientUDP.h"

namespace	Babibel
{
  namespace Controller
  {
    class		Controller;
  }
}

namespace	Babibel
{
namespace Controller
{
class		Network
{
public:
    enum eInstruction
    {
        //MANDATORY
        LIST = 0,
        CALL,
        HANGUP,
        //FEATURES
        STATUSTEXT,
        STATUS,
        ACCEPT_CALL,
        REJECT_CALL,
        LOGIN,
        CREATE_ACCOUNT,
        ADD_CONTACT,
        REMOVE_CONTACT,
        BLOCK_CONTACT,
        CHAT,
        ERRORBABEL,
        HANDSHAKE,
        PING,
        AUDIO,
        ENUM_COUNT
    };

private:
    std::queue<Packet *>	_sendQueueUDP;
    std::queue<Packet *>	_sendQueueTCP;
    AClientUDP		*_sockUDP;
    AClientTCP		*_sockTCP;
    PacketFactory		_factoryTCP;
    PacketFactory		_factoryUDP;

    bool			_initTCP;
    bool			_initUDP;

    Controller		*_controller;
    int			_reqUID;
    int         _pingTimer;

    private:
      Network		&operator=(const Network &);
      Network(const Network &);

    public:
      Network(Controller *controller)
		  : _getPtrs(ENUM_COUNT)
      {
	_controller = controller;
    _initUDP = false;
    this->_sockUDP = new Babibel::Controller::QClientUDP;
    this->connectUDP("127.0.0.1", 11234);
	_sockTCP = NULL;
    _initTCP = false;
	_reqUID = 0;
    this->_getPtrs[LIST] = &Babibel::Controller::Network::getList;
    this->_getPtrs[CALL] = &Babibel::Controller::Network::getCall;
    this->_getPtrs[HANGUP] = &Babibel::Controller::Network::getHangup;
    this->_getPtrs[STATUSTEXT] = &Babibel::Controller::Network::getStatusText;
    this->_getPtrs[STATUS] = &Babibel::Controller::Network::getStatus;
    this->_getPtrs[ACCEPT_CALL] = &Babibel::Controller::Network::getAcceptCall;
    this->_getPtrs[REJECT_CALL] = &Babibel::Controller::Network::getRejectCall;
    this->_getPtrs[LOGIN] = &Babibel::Controller::Network::getLogin;
    this->_getPtrs[CREATE_ACCOUNT] = &Babibel::Controller::Network::getCreateAccount;
    this->_getPtrs[ADD_CONTACT] = &Babibel::Controller::Network::getAddContact;
    this->_getPtrs[REMOVE_CONTACT] = &Babibel::Controller::Network::getRemoveContact;
    this->_getPtrs[BLOCK_CONTACT] = &Babibel::Controller::Network::getBlockContact;
    this->_getPtrs[CHAT] = &Babibel::Controller::Network::getChat;
    this->_getPtrs[ERRORBABEL] = &Babibel::Controller::Network::getError;
    this->_getPtrs[HANDSHAKE] = &Babibel::Controller::Network::getHandshake;
    this->_getPtrs[PING] = &Babibel::Controller::Network::getPing;
      }

      ~Network() {}

      void	connectTCP(const std::string &ip, int port);
      void	connectUDP(const std::string &ip, int port);

      void	setInitTCP(bool b);
      bool	initTCP() const;
      bool initUDP() const;
      void setInitUDP(bool initUDP);
      void	handleNetworkUDP();
      void	handleNetworkTCP();

      void	handleNetwork()
      {
          if (_initUDP && this->_sockUDP->isConnected())
          {
              handleNetworkUDP();
          }
          if (_initTCP && this->_sockTCP->isConnected())
		  {
			  handleNetworkTCP();
			}
      }

      void	pushUDP(Packet *packet)
      {
          this->_sendQueueUDP.push(packet);
      }

	  void	pushTCP(Packet *packet)
	  {
		  this->_sendQueueTCP.push(packet);
	  }

	  int	getUID()
	  {
		  return _reqUID++;
	  }

      AClientTCP *sockTCP() const;
      AClientUDP *sockUDP() const;

      typedef			bool (Babibel::Controller::Network::*mGetPtr)(Packet *packet);
      std::vector<mGetPtr>	_getPtrs;


public:
      bool      getList(Packet *packet);
      bool      getCall(Packet *packet);
      bool      getHangup(Packet *packet);
      bool      getStatusText(Packet *packet);
      bool      getStatus(Packet *packet);
      bool      getAcceptCall(Packet *packet);
      bool      getRejectCall(Packet *packet);
      bool      getLogin(Packet *packet);
      bool      getCreateAccount(Packet *packet);
      bool      getAddContact(Packet *packet);
      bool      getRemoveContact(Packet *packet);
      bool      getBlockContact(Packet *packet);
      bool      getChat(Packet *packet);
      bool      getError(Packet *packet);
      bool      getHandshake(Packet *packet);
      bool      getPing(Packet *packet);

      bool      setCreateAccount(const std::string &, const std::string &);
      bool      setLogin(const std::string &, const std::string &);
      bool      setStatusText(const std::string &);
      bool      setStatus(eStatus);
      bool      setAddContact(const std::string &);
      bool      setRemoveContact(const std::string &);
      bool      setBlockContact(const std::string &);
      bool      setChat(const std::string &, const std::string &);
      bool      setAcceptCall(const std::string &);
      bool      setRejectCall(const std::string &);
      bool      setCall(const std::string &);
      bool      setHangup(const std::string &);


};
}
}
