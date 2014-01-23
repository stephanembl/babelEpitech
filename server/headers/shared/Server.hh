#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "LibC.hh"
#include "MutexPool.h"
#include "Network.hh"
#include "ManagerAccount.hh"
#include "Client.hh"

#define				DEFAULT_PORT	11235
#define				PROMPT		"$>"
#define				ENDL		std::endl << PROMPT

class				Server
{
private:
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
	MYERROR,
	HANDSHAKE,
	PING,
	ENUM_COUNT
      };

  typedef			bool (Server::*instructions)(Client *client, Packet *packet);
  Network			_network;
  IManagerUser			*_manager;
  bool				_askQuit;
  MutexPool			_mutex;
  std::list<Client *>		_clients;
  std::vector<instructions>	_instructions;

  Server(const Server& copy);
  Server&		operator=(const Server& other);
  bool			checkAllQuitAndClose();
  void			checkCommandPrompt(void *);
  bool			checkAskQuit();
  void			quitServer();
  void			writePrompt(const std::string &msg);
  Client		*findAccountConnected(const std::string& login);
  Client		*findAccountConnected(unsigned int uid);
  void			updateCurrentClients();
  void			checkTimeOut();
  void			handleNetwork();
  bool			list(Client *client, Packet *packet);
  bool			call(Client *client, Packet *packet);
  bool			hangUp(Client *client, Packet *packet);
  bool			statusText(Client *client, Packet *packet);
  bool			status(Client *client, Packet *packet);
  bool			acceptCall(Client *client, Packet *packet);
  bool			rejectCall(Client *client, Packet *packet);
  bool			login(Client *client, Packet *packet);
  bool			createAccount(Client *client, Packet *packet);
  bool			addContact(Client *client, Packet *packet);
  bool			removeContact(Client *client, Packet *packet);
  bool			blockContact(Client *client, Packet *packet);
  bool			chat(Client *client, Packet *packet);
  bool			error(Client *client, Packet *packet);
  bool			handShake(Client *client, Packet *packet);
  bool			ping(Client *client, Packet *packet);

public:
  Server(unsigned short port = DEFAULT_PORT);
  virtual ~Server();
  unsigned short	getPort() const;
  int			start();
};
