#include <fstream>
#include "Thread.hpp"
#include "Server.hh"
#include "Util.hpp"

Server::Server(unsigned short port)
  : _network(port), _instructions(Server::ENUM_COUNT)
{
  this->_askQuit = false;
  this->_manager = NULL;
  this->_mutex.init("PROMPT");
  this->_instructions[Server::LIST] = &Server::list;
  this->_instructions[Server::CALL] = &Server::call;
  this->_instructions[Server::HANGUP] = &Server::hangUp;
  this->_instructions[Server::STATUSTEXT] = &Server::statusText;
  this->_instructions[Server::STATUS] = &Server::status;
  this->_instructions[Server::ACCEPT_CALL] = &Server::acceptCall;
  this->_instructions[Server::REJECT_CALL] = &Server::rejectCall;
  this->_instructions[Server::LOGIN] = &Server::login;
  this->_instructions[Server::CREATE_ACCOUNT] = &Server::createAccount;
  this->_instructions[Server::ADD_CONTACT] = &Server::addContact;
  this->_instructions[Server::REMOVE_CONTACT] = &Server::removeContact;
  this->_instructions[Server::BLOCK_CONTACT] = &Server::blockContact;
  this->_instructions[Server::CHAT] = &Server::chat;
  this->_instructions[Server::MYERROR] = &Server::error;
  this->_instructions[Server::HANDSHAKE] = &Server::handShake;
  this->_instructions[Server::PING] = &Server::ping;
}

Server::~Server()
{
  for (std::list<Client *>::iterator it = this->_clients.begin();
       it != this->_clients.end(); ++it)
      delete *it;
  this->_clients.clear();
  this->_mutex.destroy("PROMPT");
  delete this->_manager;
}

unsigned short	Server::getPort() const
{
  return (this->_network.getPort());
}

int				Server::start()
{
  Thread<Server, void, void *>	thInput(this, &Server::checkCommandPrompt, NULL);
  int				ret(0);

  this->_askQuit = false;
  std::cout << "Server is starting..." << std::endl;
  try
    {
      this->_network.init();
      this->_manager = new ManagerAccount();
      this->_manager->init();
    }
  catch (const Exception& ex)
    {
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cerr << "The server couldn't start." << std::endl;
      return (-1);
    }
  thInput.start();
  std::cout << "Server started." << std::endl;
  std::cout << PROMPT;
  while (!this->checkAllQuitAndClose())
    {
      try
	{
	  this->checkTimeOut();
	  this->_network.getModuleNetwork()->update();
	  this->updateCurrentClients();
	  if (!ret)
	    {
	      this->handleNetwork();
	      if (LibC::mSleep(10) == -1)
		throw Exception("Sleep has failed.");
	    }
	}
      catch (const Exception& ex)
	{
	  std::cerr << std::endl << "Error: " << ex.what() << std::endl;
	  this->quitServer();
	  ret = -1;
	}
    }
  if (!ret)
    thInput.join();
  std::cout << "Server stopped." << std::endl;
  return (ret);
}

bool				Server::checkAllQuitAndClose()
{
  if ((this->checkAskQuit()) && (this->_clients.size() == 0))
    return (true);
  return (false);
}

void		Server::checkCommandPrompt(void *)
{
  std::string	cmd;

  while (0xD4170)
    {
      if ((!std::getline(std::cin, cmd)) || ((cmd.size() == 8) && (cmd == "shutdown")))
	{
	  this->_mutex.lock("PROMPT");
	  this->_askQuit = true;
	  this->_mutex.unLock("PROMPT");
	  if (cmd.size() != 8)
	    std::cout << std::endl;
	  return;
	}
      if ((cmd.size() == 4) && (cmd == "help"))
	std::cout << "---" << std::endl <<"Commands:" << std::endl << "\t\thelp\t\tFor display all commands." << std::endl
		  << "\t\tshutdown\tFor stop the server," << std::endl << "---" << std::endl;
      else if (cmd.size() != 0)
	std::cout << "Command not found: " << cmd << " (You can enter \"help\" command)" << std::endl;
      std::cout << PROMPT;
    }
}

bool		Server::checkAskQuit()
{
  bool		tmp;

  this->_mutex.lock("PROMPT");
  tmp = this->_askQuit;
  this->_mutex.unLock("PROMPT");
  return (tmp);
}

void		Server::quitServer()
{
  this->_mutex.lock("PROMPT");
  this->_askQuit = true;
  this->_mutex.unLock("PROMPT");
}

void		Server::writePrompt(const std::string &msg)
{
  std::cout << std::endl << msg << std::endl << PROMPT << std::flush;
}

Client					*Server::findAccountConnected(const std::string& login)
{
  std::list<Client *>::iterator		it;

  for (it = this->_clients.begin();
       it != this->_clients.end(); ++it)
    if (((*it)->getAccount() != NULL) && ((*it)->getAccount()->getLogin() == login))
      break;
  if (it == this->_clients.end())
    return (NULL);
  return (*it);
}

Client					*Server::findAccountConnected(unsigned int uid)
{
  std::list<Client *>::iterator		it;

  for (it = this->_clients.begin();
       it != this->_clients.end(); ++it)
    if ((*it)->getUid() == uid)
      break;
  if (it == this->_clients.end())
    return (NULL);
  return (*it);
}

void						Server::updateCurrentClients()
{
  std::list<std::pair<bool, unsigned int> >	&tmpList = this->_network.getModuleNetwork()->updateIdClient();
  std::list<Client *>::iterator			it_bis;;
  unsigned int					size = 3;
  Client					*tmpClient;

  if (tmpList.empty())
    return;
  for (std::list<std::pair<bool, unsigned int> >::iterator it = tmpList.begin();
       it != tmpList.end(); ++it)
    {
      if ((*it).first)
	{
	  Packet	newPacket;
	  this->_clients.push_back(tmpClient = new Client((*it).second));
	  tmpClient->setIp(this->_network.getModuleNetwork()->getEndPointId((*it).second));
	  newPacket.setInstruction(Server::HANDSHAKE);
	  newPacket.setRequestUID(0);
	  try
	    {
	      this->_network.getModuleNetwork()->send((*it).second, newPacket.serialize(), newPacket.size());
	    }
	  catch (const Exception& ex)
	    {
	      this->_network.getModuleNetwork()->closeId((*it).second);
	      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
	      std::cout << PROMPT;
	    }
	}
      else
	{
	  for (it_bis = this->_clients.begin();
	       it_bis != this->_clients.end(); ++it_bis)
	    if ((*it_bis)->getUid() == (*it).second)
	      break;
	  if (it_bis != this->_clients.end())
	    {
	      if ((*it_bis)->getAccount())
		{
		  (*it_bis)->getAccount()->setConnected(false);
		  (*it_bis)->getAccount()->setStatusCall(Account::NONE);
		  if (!(*it_bis)->getAccount()->getConversation().empty())
		    {
		      size += (*it_bis)->getAccount()->getLogin().size() + 4;
		      for (std::list<Account *>::iterator it = (*it_bis)->getAccount()->getConversation().begin();
			   it != (*it_bis)->getAccount()->getConversation().end(); ++it)
			{
			  if (((*it_bis)->getAccount()->getConversation().size() == 1) && 
			      ((*it_bis)->getAccount()->findAccountConversation((*it)->getLogin()) != (*it_bis)->getAccount()->getConversation().end()))
			    {
			      Packet	newPacket;
			      newPacket.setInstruction(Server::HANGUP);
			      newPacket.updateData(size + 4);
			      newPacket.setFormat("cs");
			      newPacket.setRequestUID(0);
			      newPacket.appendToData<char>(0, 1);
			      newPacket.appendToData(1, (*it_bis)->getAccount()->getLogin().c_str());
			      try
				{
			      this->_network.getModuleNetwork()->send(this->findAccountConnected((*it)->getLogin())->getUid(), newPacket.serialize(), newPacket.size());
				}
			      catch (const Exception&)
				{
				  
				}
			    }
			}
		      this->_manager->hangUp((*it_bis)->getAccount()->getLogin());
		    }
		  for (std::list<Client *>::iterator it_tmp = this->_clients.begin();
		       it_tmp != this->_clients.end(); ++it_tmp)
		    if (((*it_tmp)->getAccount()) && (*it_tmp)->getAccount()->getFriends().find((*it_bis)->getAccount()->getLogin()) != (*it_tmp)->getAccount()->getFriends().end())
		      this->list((*it_tmp), NULL); 
		}
	      this->_clients.erase(it_bis);
	      this->writePrompt(std::string("Client disconnected id => " + Util::toString<unsigned int>((*it_bis)->getUid()) + " (" + (*it_bis)->getIp() + ")"));
	    }
	}
    }
  tmpList.clear();
}

void							Server::checkTimeOut()
{
  for (std::list<Client *>::iterator it = this->_clients.begin();
       it != this->_clients.end(); ++it)
    {
      //server quit
      if (this->checkAskQuit())
	this->_network.getModuleNetwork()->closeId((*it)->getUid());
      else
	{
	  //handshake
	  if ((!(*it)->hasHandShake()) && ((LibC::time(NULL) - (*it)->getConnectionTime()) > 10))
	    this->_network.getModuleNetwork()->closeId((*it)->getUid());
	  //ping
	  if ((LibC::time(NULL) - (*it)->getLastPingTime()) > 60)
	    this->_network.getModuleNetwork()->closeId((*it)->getUid());
	  //call time out
	  if (((*it)->getAccount()) && ((*it)->getAccount()->getStatusCall() == Account::ISWAITING) && ((LibC::time(NULL) - (*it)->getAccount()->getTimeOutCall()) > 30))
	    this->hangUp((*it), NULL);
	}
    }
}

void							Server::handleNetwork()
{
  std::list<std::triple<unsigned int, char*, size_t> >	&tmpList = this->_network.getModuleNetwork()->receive();
  Client						*clientTmp;
  Packet						*tmpPacket;

  if (tmpList.empty())
    return;
  for (std::list<std::triple<unsigned int, char*, size_t> >::iterator it = tmpList.begin();
       it != tmpList.end(); ++it)
    {
      if ((clientTmp = this->findAccountConnected((*it).first)))
	{
	  if (clientTmp->getPacket().feed((*it).second, (*it).third))
	    {
	      while ((tmpPacket = clientTmp->getPacket().getPacket()))
		{
		  if ((tmpPacket->getInstruction() < Server::ENUM_COUNT) && 
		      ((clientTmp->hasHandShake()) || ((!clientTmp->hasHandShake()) && (tmpPacket->getInstruction() == Server::HANDSHAKE)))) 
		    (this->*_instructions[tmpPacket->getInstruction()])(clientTmp, tmpPacket);
		  delete tmpPacket;
		}
	    }
	  else
	    {
	      this->_network.getModuleNetwork()->closeId((*it).first);
	      std::cerr << std::endl << "Error: Client invalid." << std::endl;
	      std::cout << PROMPT;
	    }
	}
    }
  tmpList.clear();
}

bool					Server::list(Client *client, Packet *packet)
{
  std::list<std::string>		friendsTmp;
  Account				*accountTmp;
  Packet				newPacket;
  unsigned int				size = 3;
  int					idRaw = 2;
  bool					success = true;
  std::string				msg;

  if (!client->getAccount())
    {
      msg = "List: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else
    {
      friendsTmp = client->getAccount()->getListFriends();
      size += 4;
      for (std::list<std::string>::iterator it = friendsTmp.begin();
	   it != friendsTmp.end(); ++it)
	{
	  if ((accountTmp = this->_manager->findAccount(*it)))
	    {
	      size += accountTmp->getLogin().size() + 4;
	      size += accountTmp->getState().size() + 4;
	      size += 3;
	    }
	}
    }
  newPacket.setInstruction(Server::LIST);
  newPacket.updateData(size + ((success) ? (7) : (4)));
  newPacket.setFormat((success) ? ("clssc") : ("cs"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  if (success)
    {
      newPacket.appendToData<short>(1, friendsTmp.size());
      for (std::list<std::string>::iterator it = friendsTmp.begin();
	   it != friendsTmp.end(); ++it)
	{
	  if ((accountTmp = this->_manager->findAccount(*it)))
	    {
	      newPacket.appendToData(idRaw++, accountTmp->getLogin().c_str());
	      newPacket.appendToData(idRaw++, accountTmp->getState().c_str());
	      if (!accountTmp->isBlocked(client->getAccount()->getLogin()))
		newPacket.appendToData<char>(idRaw++, (accountTmp->isConnected()) ? (accountTmp->getStatus()) : (Account::INVISIBLE));
	      else
		newPacket.appendToData<char>(idRaw++, Account::INVISIBLE);
	    }
	}
    }
  else
    {
      newPacket.appendToData(0, msg.c_str());
    }
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("LIST");
  return (true);
}

bool					Server::call(Client *client, Packet *packet)
{
  Packet				newPacket;
  Packet				toPacket;
  unsigned int				size = 3;
  std::string				msg;
  std::string				login;
  bool					success = true;
  Account				*actTmp;

  try
    {
      if (packet)
	login = packet->getStringInData(0);
    }
  catch (const Exception&)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Call: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!(actTmp = this->_manager->findAccount(login)))
    {
      msg = "Call: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!actTmp->isConnected())
    {
      msg = "Call: This account is not online.";
      success = false;
      size += msg.size() + 4;	  
    }
  else
    {
      try
	{
	  if (this->_manager->call(client->getAccount()->getLogin(), actTmp->getLogin()))
	    {
	      msg = client->getAccount()->getLogin();
	      size += msg.size() + 4;
	    }
	  else
	    {
	      msg = "Call: This account is not online."; 
	      success = false;
	      size += msg.size() + 4; 
	    }
	}
      catch (const Exception& ex)
	{
	  msg = "Call: " + std::string(ex.what());
	  success = false;
	  size += msg.size() + 4;	  
	}
    }
  newPacket.setInstruction(Server::CALL);
  newPacket.updateData(size + 9 + ((success) ? (client->getIp().size()) : (login.size())));
  newPacket.setFormat("css");
  newPacket.setRequestUID(0);
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  if (success)
    {
      this->writePrompt("SUCCESS");
      newPacket.appendToData(2, client->getIp().c_str());
    }
  else
    newPacket.appendToData(2, login.c_str());
  try
    {
      this->_network.getModuleNetwork()->send((success) ? (this->findAccountConnected(login)->getUid()) : (client->getUid()), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId((success) ? (this->findAccountConnected(login)->getUid()) : (client->getUid()));
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("CALL");
  return (success);
}

bool					Server::hangUp(Client *client, Packet *packet)
{
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  
  if (!client->getAccount())
    {
      msg = "HangUp: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (client->getAccount()->getConversation().empty())
    {
      msg = ((client->getAccount()->getStatusCall() == Account::ISWAITING) && ((LibC::time(NULL) - client->getAccount()->getTimeOutCall()) > 30)) ? ("Call time out.") : ("HangUp: You are not in call.");
      success = false;
      size += msg.size() + 4;
    }
  else
    {
      size += client->getAccount()->getLogin().size() + 4;
      for (std::list<Account *>::iterator it = client->getAccount()->getConversation().begin();
	   it != client->getAccount()->getConversation().end(); ++it)
	{
	  if (((*it)->findAccountConversation(client->getAccount()->getLogin()) != (*it)->getConversation().end()) &&
	  ((*it)->getConversation().size() == 1))
	    {
	      Packet				newPacket;
	      
	      newPacket.setInstruction(Server::HANGUP);
	      newPacket.updateData(size + 4);
	      newPacket.setFormat("cs");
	      newPacket.setRequestUID(0);
	      newPacket.appendToData<char>(0, 1);
	      newPacket.appendToData(1, client->getAccount()->getLogin().c_str());
	      try 
		{
	      this->_network.getModuleNetwork()->send(this->findAccountConnected((*it)->getLogin())->getUid(), newPacket.serialize(), newPacket.size());
		}
	      catch (const Exception&)
		{

		}
	    }
	}
      size = 3;
      msg = client->getAccount()->getConversation().front()->getLogin();
      size += msg.size() + 4;
    }
  Packet				newPacket;

  newPacket.setInstruction(Server::HANGUP);
  if (success)
    newPacket.updateData(size + 4);
  else
    newPacket.updateData(size + 9 + ((client->getAccount()->getConversation().empty()) ? (0) : (client->getAccount()->getConversation().front()->getLogin().size())));
  newPacket.setFormat((success) ? ("cs") : ("css"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  if (!success)
    newPacket.appendToData(2, (client->getAccount()->getConversation().empty()) ? ("") : (client->getAccount()->getConversation().front()->getLogin().c_str()));
  this->_manager->hangUp(client->getAccount()->getLogin());
  try
  {
    this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
  }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("HANGUP");
  return (true);
}

bool					Server::statusText(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  std::string				msg;
  std::string				tmpStates;
  bool					success = true;

  try
    {
      if (packet)
	tmpStates = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Status Text: You are not connected with an account.";
      success = false;
    }
  else if (!packet)
    msg = client->getAccount()->getState();
  else
    {
      client->getAccount()->setState(tmpStates);
      msg = client->getAccount()->getState();
    }
  size += msg.size() + 4;
  newPacket.setInstruction(Server::STATUSTEXT);
  newPacket.updateData(size + 4);
  newPacket.setFormat("cs");
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("STATUSTEXT");
  if ((success) && (packet))
    {
      for (std::list<Client *>::iterator it = this->_clients.begin();
	   it != this->_clients.end(); ++it)
	if (((*it)->getAccount()) && (*it)->getAccount()->getFriends().find(client->getAccount()->getLogin()) != (*it)->getAccount()->getFriends().end())
	  this->list((*it), NULL);
    }
  return (success);
}

bool					Server::status(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  char					status;

  if (!client->getAccount())
    {
      msg = "Status: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else
    {
      try
	{
	  if (packet)
	    status = packet->getIntTypeInData<char>(0);
	  if ((packet) && (!client->getAccount()->setStatus((Account::eStatus) status)))
	    {
	      msg = "Status: Status is invalid.";
	      success = false;
	  size += msg.size() + 4;
	    }
	  else
	    {
	      status = client->getAccount()->getStatus();
	      size += 3;
	    }
	}
      catch (const Exception& ex)
	{
	  this->_network.getModuleNetwork()->closeId(client->getUid());
	  std::cerr << std::endl << "Error: Client invalid." << std::endl;
	  std::cout << PROMPT;
	  return (false);
	}      
    }
  newPacket.setInstruction(Server::STATUS);
  newPacket.updateData(size + 4);
  newPacket.setFormat((!success) ? ("cs") : ("cc"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  if (!success)
    newPacket.appendToData(1, msg.c_str());
  else
    newPacket.appendToData<char>(1, status);
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("STATUS");
  if ((success) && (packet))
    {
      for (std::list<Client *>::iterator it = this->_clients.begin();
	   it != this->_clients.end(); ++it)
	if (((*it)->getAccount()) && (*it)->getAccount()->getFriends().find(client->getAccount()->getLogin()) != (*it)->getAccount()->getFriends().end())
	  this->list((*it), NULL);
    }
  return (success);
}

bool					Server::acceptCall(Client *client, Packet *packet)
{
  Packet				newPacket;
  Packet				toPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  Account				*actTmp;
  std::string				tmpLogin;

  try
    {
      if (packet)
	tmpLogin = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Accept call: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!(actTmp = this->_manager->findAccount(tmpLogin)))
    {
      msg = "Accept call: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!actTmp->isConnected())
    {
      msg = "Accept call: This account is not online.";
      success = false;
      size += msg.size() + 4;	  
    }
  else if (this->_manager->answerCall(client->getAccount()->getLogin(), actTmp->getLogin(), true))
    {
      msg = client->getAccount()->getLogin();
      size += msg.size() + 4;
    }
  else
    {
      msg = "Accept call: Account not found.";
      success = false;
      size += msg.size() + 4; 
    }
  newPacket.setInstruction(Server::ACCEPT_CALL);
  newPacket.updateData(size + 9 + ((success) ? (client->getIp().size()) : (tmpLogin.size())));
  newPacket.setFormat("css");
  if (!success)
    newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  else
    newPacket.setRequestUID(0);
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  if (success)
    newPacket.appendToData(2, client->getIp().c_str());
  else
    newPacket.appendToData(2, packet->getStringInData(0).c_str());
  try
    {
      this->_network.getModuleNetwork()->send((!success) ? (client->getUid()) : (this->findAccountConnected(tmpLogin)->getUid()), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId((!success) ? (client->getUid()) : (this->findAccountConnected(tmpLogin)->getUid()));
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("ACCEPTCALL");
  return (success);
}

bool					Server::rejectCall(Client *client, Packet *packet)
{
  Packet				newPacket;
  Packet				toPacket;
  unsigned int				size = 3;
  std::string				msg;
  std::string				tmpLogin;
  bool					success = true;
  Account				*actTmp;

  try
    {
      if (packet)
	tmpLogin = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Reject call: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!(actTmp = this->_manager->findAccount(tmpLogin)))
    {
      msg = "Accept call: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!actTmp->isConnected())
    {
      msg = "Reject call: This account is not online.";
      success = false;
      size += msg.size() + 4;	  
    }
  else if (this->_manager->answerCall(client->getAccount()->getLogin(), actTmp->getLogin(), false))
    {
      msg = client->getAccount()->getLogin();
      size += msg.size() + 4;
    }
  else
    {
      msg = "Reject call: Account not found.";
      success = false;
      size += msg.size() + 4; 
    }
  newPacket.setInstruction(Server::REJECT_CALL);
  newPacket.updateData(size + 9 + ((success) ? (client->getIp().size()) : (tmpLogin.size())));
  newPacket.setFormat("css");
  if (!success)
    newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  else
    newPacket.setRequestUID(0);
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  if (success)
    newPacket.appendToData(2, client->getIp().c_str());
  else
    newPacket.appendToData(2, packet->getStringInData(0).c_str());
  try
    {
      this->_network.getModuleNetwork()->send((!success) ? (client->getUid()) : (this->findAccountConnected(tmpLogin)->getUid()), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId((!success) ? (client->getUid()) : (this->findAccountConnected(tmpLogin)->getUid()));
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("REJECTCALL");
  return (success);
}

bool					Server::login(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  Account				*actTmp;
  std::string				msg;
  bool					success = true;
  std::string				tmpLogin;
  std::string				tmpPasswd;

  try
    {
      if (packet)
	{
	  tmpLogin = packet->getStringInData(0);
	  tmpPasswd = packet->getStringInData(1);
	}
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!(actTmp = this->_manager->login(tmpLogin, tmpPasswd)))
    {
      msg = "Login: Account or password is invalid.";
      success = false;
    }
  else
    {
      if (client->getAccount())
	{
	  msg = "Login: You are already connected with an account.";
	  success = false;
	}
      else if (actTmp->isConnected())
	{
	  msg = "Login: This account is already connected.";
	  success = false;
	}
      else
	{
	  msg = actTmp->getLogin();
	  actTmp->setConnected(true);
	  client->setAccount(actTmp);
	}
    }
  size += msg.size() + 4;
  newPacket.setInstruction(Server::LOGIN);
  newPacket.updateData(size + 4);
  newPacket.setFormat("cs");
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("LOGIN");
  if (success)
    {
      this->statusText(client, NULL);
      this->status(client, NULL);
      this->list(client, NULL);
      for (std::list<Client *>::iterator it = this->_clients.begin();
	   it != this->_clients.end(); ++it)
	if (((*it)->getAccount()) && (*it)->getAccount()->getFriends().find(client->getAccount()->getLogin()) != (*it)->getAccount()->getFriends().end())
	  this->list((*it), NULL);
    }
  return (success);
}

bool					Server::createAccount(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  Account				*actTmp;
  std::string				msg;
  bool					success = true;
  std::string				tmpLogin;
  std::string				tmpPasswd;

  try
    {
      if (packet)
	{
	  tmpLogin = packet->getStringInData(0);
	  tmpPasswd = packet->getStringInData(1);
	}
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (tmpLogin == "")
    {
      msg = "Account creation: Login is invalid..";
      success = false;
    }
  else if (!this->_manager->createAccount(tmpLogin, tmpPasswd))
    {
      msg = "Account creation: Account already exist.";
      success = false;
    }
  else if (!(actTmp = this->_manager->login(tmpLogin, tmpPasswd)))
    {
      msg = "Account creation: On error is occured.";
      success = false;
    }
  else
    {
      if (client->getAccount())
	{
	  msg = "Account creation: You are already connected with this account.";
	  success = false;
	}
      else if (actTmp->isConnected())
	{
	  msg = "Login: This account is already connected.";
	  success = false;
	}
      else
	{
	  msg = actTmp->getLogin();
	  actTmp->setConnected(true);
	  client->setAccount(actTmp);
	}
    }
  size += msg.size() + 4;
  newPacket.setInstruction(Server::CREATE_ACCOUNT);
  newPacket.updateData(size + 4);
  newPacket.setFormat("cs");
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("CREATE_ACCOUNT");
  if (success)
    {
      this->statusText(client, NULL);
      this->status(client, NULL);
      this->list(client, NULL);
      for (std::list<Client *>::iterator it = this->_clients.begin();
	   it != this->_clients.end(); ++it)
	if (((*it)->getAccount()) && (*it)->getAccount()->getFriends().find(client->getAccount()->getLogin()) != (*it)->getAccount()->getFriends().end())
	  this->list((*it), NULL);
    }
  return (success);
}

bool					Server::addContact(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  std::string				tmpLogin;

  try
    {
      if (packet)
	tmpLogin = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Adding contact: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!this->_manager->findAccount(tmpLogin))
    {
      msg = "Adding contact: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!client->getAccount()->addFriend(tmpLogin))
    {
      msg = "Adding contact: Account already added.";
      success = false;
      size += msg.size() + 4;
    }
  newPacket.setInstruction(Server::ADD_CONTACT);
  newPacket.updateData(size + ((success) ? (3) : (4)));
  newPacket.setFormat((success) ? ("c") : ("cs"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  if (!success)
    newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("ADDCONTACT");
  if (success)
    {
      this->list(client, NULL);
    }
  return (success);
}

bool					Server::removeContact(Client *client, Packet *packet)
{
  Packet				newPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  std::string				tmpLogin;
  Client				*tmpClient;
  bool					wasBlocked = false;

  try
    {
      if (packet)
	tmpLogin = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if ((client) && (client->getAccount()) && (client->getAccount()->isBlocked(tmpLogin)))
    wasBlocked = true;
  if (!client->getAccount())
    {
      msg = "Deleting contact: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!this->_manager->findAccount(tmpLogin))
    {
      msg = "Deleting contact: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!client->getAccount()->deleteFriends(tmpLogin))
    {
      msg = "Deleting contact: This account is not your friend.";
      success = false;
      size += msg.size() + 4;
    }
  newPacket.setInstruction(Server::REMOVE_CONTACT);
  newPacket.updateData(size + ((success) ? (3) : (4)));
  newPacket.setFormat((success) ? ("c") : ("cs"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  if (!success)
    newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("REMOVECONTACT");
  if (success)
    {
      this->list(client, NULL);
      if ((wasBlocked) && (tmpClient = this->findAccountConnected(tmpLogin)))
	{
	  this->list(tmpClient, NULL);
	}
    }
  return (success);
}

bool					Server::blockContact(Client *client, Packet *packet)
{
  Client				*tmpClient = NULL;
  Packet				newPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  std::string				tmpLogin;

  try
    {
      if (packet)
	tmpLogin = packet->getStringInData(0);
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Blocking contact: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!this->_manager->findAccount(tmpLogin))
    {
      msg = "Blocking contact: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!client->getAccount()->friendBlock(packet->getStringInData(0), (client->getAccount()->isBlocked(tmpLogin)) ? (false) : (true)))
    {
      msg = "Blocking contact: This account is not your friend.";
      success = false;
      size += msg.size() + 4;
    }
  newPacket.setInstruction(Server::BLOCK_CONTACT);
  newPacket.updateData(size + ((success) ? (3) : (4)));
  newPacket.setFormat((success) ? ("c") : ("cs"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  if (!success)
    newPacket.appendToData(1, msg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  //this->writePrompt("BLOCKCONTACT");
  if ((success) && (tmpClient = this->findAccountConnected(tmpLogin)))
    {
      this->list(tmpClient, NULL);
    }
  return (success);
}

bool					Server::chat(Client *client, Packet *packet)
{
  Packet				newPacket;
  Packet				toPacket;
  unsigned int				size = 3;
  std::string				msg;
  bool					success = true;
  Account				*actTmp;
  std::string				tmpLogin;
  std::string				tmpMsg;

  try
    {
      if (packet)
	{
	  tmpLogin = packet->getStringInData(0);
	  tmpMsg = packet->getStringInData(1);
	}
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: Client invalid." << std::endl;
      std::cout << PROMPT;
      return (false);
    }
  if (!client->getAccount())
    {
      msg = "Chat: You are not connected with an account.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!(actTmp = this->_manager->findAccount(tmpLogin)))
    {
      msg = "Chat: Account not found.";
      success = false;
      size += msg.size() + 4;
    }
  else if (!actTmp->isConnected())
    {
      msg = "Chat: This account is not online.";
      success = false;
      size += msg.size() + 4;	  
    }
  else
    {
      try
	{
	  if (this->_manager->chat(client->getAccount()->getLogin(), actTmp->getLogin()))
	    {	      
	      msg = tmpLogin;
	      size += msg.size() + 4;
	    }
	  else
	    {
	      msg = "Chat: This account is not online.";
	      success = false;
	      size += msg.size() + 4; 
	    }
	}
      catch (const Exception& ex)
	{
	  msg = "Chat: " + std::string(ex.what());
	  success = false;
	  size += msg.size() + 4;
	}
    }
  newPacket.setInstruction(Server::CHAT);
  newPacket.updateData(size + ((success) ? (5 + (tmpMsg.size() + 4)) : (4)));
  newPacket.setFormat((success) ? ("css") : ("cs"));
  newPacket.setRequestUID((packet) ? (packet->getRequestUID()) : (0));
  newPacket.appendToData<char>(0, (success) ? (1) : (0));
  newPacket.appendToData(1, msg.c_str());
  if (success)
    newPacket.appendToData(2, tmpMsg.c_str());
  try
    {
      this->_network.getModuleNetwork()->send(client->getUid(), newPacket.serialize(), newPacket.size());
    }
  catch (const Exception& ex)
    {
      this->_network.getModuleNetwork()->closeId(client->getUid());
      std::cerr << std::endl << "Error: " << ex.what() << std::endl;
      std::cout << PROMPT;
    }
  if (success)
    {
      size = 3;
      msg = client->getAccount()->getLogin();
      size += msg.size() + 4;
      toPacket.setInstruction(Server::CHAT);
      toPacket.updateData(size + 5 + (tmpMsg.size() + 4));
      toPacket.setFormat("css");
      toPacket.setRequestUID(0);
      toPacket.appendToData<char>(0, 1);
      toPacket.appendToData(1, msg.c_str());
      toPacket.appendToData(2, tmpMsg.c_str());
      try
	{
	  this->_network.getModuleNetwork()->send(this->findAccountConnected(tmpLogin)->getUid(), toPacket.serialize(), toPacket.size());
	}
      catch (const Exception& ex)
	{
	  this->_network.getModuleNetwork()->closeId(this->findAccountConnected(tmpLogin)->getUid());
	  std::cerr << std::endl << "Error: " << ex.what() << std::endl;
	  std::cout << PROMPT;
	}
    }
  //this->writePrompt("CHAT");
  return (success);
}

bool					Server::error(Client *, Packet *)
{
  return (true);
}

bool					Server::handShake(Client *client, Packet *)
{
  client->setHasHandShake(true);
  std::cout << std::endl << "Client connected id => " << client->getUid() << " (" << client->getIp() << ")" << std::endl << PROMPT << std::flush;
  //this->writePrompt("HANDSHAKE");
  return (true);
}

bool					Server::ping(Client *client, Packet *)
{
  client->setLastPingTime(LibC::time(NULL));
  //this->writePrompt("PING");
  return (true);
}
