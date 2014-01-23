#pragma once

#include <boost/filesystem.hpp>
#include "ISerialize.h"
#include "SerializeOneAccountBoost.hpp"

class SerializeAccountBoost : public ISerialize
{
private:
	std::string		_path;
	std::string		_extension;

	SerializeAccountBoost(const SerializeAccountBoost& copy);
	SerializeAccountBoost&	operator=(const SerializeAccountBoost& other);
public:
	SerializeAccountBoost() : _path("accounts"), _extension(".xml") {}
	virtual ~SerializeAccountBoost() {}

	bool save(std::map<std::string, Account *>& accounts)
	{
		boost::filesystem::path path(boost::filesystem::current_path() / this->_path);
		boost::filesystem::remove_all(path);
		boost::filesystem::create_directory(path);
		if (!boost::filesystem::is_directory(path))
			return (false);
		SerializeOneAccountBoost s;
		std::map<std::string, Account *>& tmp = accounts;
		for (std::map<std::string, Account *>::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
			s.save(*it->second, std::string(path.string() + "/" + it->first + this->_extension).c_str());
		return (true);
	}

	bool restore(std::map<std::string, Account *>& accounts)
	{
		boost::filesystem::path path(boost::filesystem::current_path() / this->_path);
		SerializeOneAccountBoost s;

		if (!boost::filesystem::is_directory(path))
			return (false);
		for (boost::filesystem::recursive_directory_iterator end, dir(path); dir != end; ++dir)
		{
			const boost::filesystem::path &p = dir->path();
			if (boost::filesystem::is_directory(p))
			{
				dir.no_push();
				continue;
			}
			if (boost::filesystem::is_regular_file(p) && p.extension() == this->_extension)
			{
				Account *tmp = new Account("", "");
				s.restore(tmp, p.string().c_str());
				accounts[tmp->getLogin()] = tmp;
			}
		}
		return (true);
	}
};
