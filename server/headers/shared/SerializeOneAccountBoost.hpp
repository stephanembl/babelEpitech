#pragma once

#include <iomanip>
#include <iostream>
#include <fstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "Account.hh"

class AccountTmp
{
public:
	std::string					_login;
	std::string					_hash;
	std::string					_state;
	Account::eStatus				_status;
	std::map<std::string, bool>			_friends;
};

class SerializeOneAccountBoost
{
public:
	SerializeOneAccountBoost() {}
	virtual ~SerializeOneAccountBoost() {}
	void save(Account& account, const char* filename)
	{
		AccountTmp tmp;
		tmp._login = account.getLogin();
		tmp._hash = account.getHash();
		tmp._state = account.getState();
		tmp._status = account.getStatus();
		tmp._friends = account.getFriends();
		std::ofstream ofs(filename);
		assert(ofs.good());
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(tmp);
	}

	void restore(Account *account, const char* filename)
	{
		AccountTmp tmp;
		std::ifstream ifs(filename);
		assert(ifs.good());
		boost::archive::xml_iarchive ia(ifs);
		ia >> BOOST_SERIALIZATION_NVP(tmp);
		account->rename(tmp._login);
		account->changePasswd(tmp._hash);
		account->setState(tmp._state);
		account->setStatus(tmp._status);
		std::map<std::string, bool> friends = tmp._friends;
		for (std::map<std::string, bool>::const_iterator it = friends.begin(); it != friends.end(); ++it)
		{
			account->addFriend(it->first);
			account->friendBlock(it->first, it->second);
		}
	}
};

namespace boost
{
	namespace serialization
	{
		template<class Archive>
		void serialize(Archive & ar, AccountTmp & a, const unsigned int)
		{
			ar & BOOST_SERIALIZATION_NVP(a._login);
			ar & BOOST_SERIALIZATION_NVP(a._hash);
			ar & BOOST_SERIALIZATION_NVP(a._state);
			ar & BOOST_SERIALIZATION_NVP(a._status);
			ar & BOOST_SERIALIZATION_NVP(a._friends);
		}
	}
}
