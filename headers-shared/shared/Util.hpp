#pragma once

# include	<string>
# include	<sstream>

namespace	Util
{
  template<typename T>
  std::string	toString(T a)
  {
    std::ostringstream	oss;

    oss << a;
    return (oss.str());
  }

  template<typename T>
  T	fromString(const std::string& str)
  {
    std::istringstream	iss(str);
    T	val;

    iss >> val;
    return (val);
  }

  std::string&	replace(std::string&, const std::string&, const std::string&);
  bool		isEqual(const std::string&, const std::string&);
  std::string	getBetween(const std::string&, const std::string&, const std::string&);
  std::string	getNext(std::string&, const std::string&);
}
