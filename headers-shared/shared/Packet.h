#pragma once

#include	<sstream>
#include	<map>
#include	<stdexcept>
#include	<cstring>
#include	<iostream>

#include    "LibC.hh"

class Packet
{
  unsigned int			_magicNumber;
  unsigned short       		_requestUID;
  unsigned char			_instruction;
  unsigned int			_dataSize;
  char		       		*_data;
  char                  *_serialization;
  std::string			_fmt;
  std::string			_lfmt;
  short                 _listsize;
  unsigned int			_actualDataSize;

  std::string			getStringInData(int *) const;
  void				consumeFormat(int *, int *);

 public:
  enum				eHeaderIndex
  {
    MAGIC_NUMBER_INDEX = 0,
    REQUEST_UID_INDEX = 3,
    INSTRUCTION_INDEX = 5,
    DATA_SIZE_INDEX = 6,
  };

  static const int	HEADER_SIZE = 9;
  static const unsigned int	MAGIC_NUMBER = 0xDA170;

  Packet(int uid = -1);
  Packet(const Packet&);
  virtual				~Packet();

  void				setMagicNumber(unsigned int);
  void				setRequestUID(unsigned short);
  void				setInstruction(unsigned char);
  void				setDataSize(unsigned int);
  void				setFormat(const std::string &);
  void				setData(char *);

  unsigned int			getMagicNumber() const;
  unsigned short		getRequestUID() const;
  unsigned char 		getInstruction() const;
  unsigned int			getDataSize() const;
  std::string			getFormat() const;
  char				*getData() const;

  void				show();

  void				updateData(unsigned int);
  unsigned int			size() const;
  unsigned int			recvsize() const;
  char				*serialize();
  void				deserialize(char *);
  void				header(char *);
  std::string			getStringInData(int);
  std::string			getFormatInData();

  static				unsigned short getUniqueIdentifier()
  {
    static	unsigned short uid = 0;
    return (++uid);
  }

    void					appendToData(short id, char *data, int size)
    {
        union				uConvert
        {
          short			data;
          char			tab[2];
        }					convert;

        convert.data = size;
        LibC::memcpy(this->_data + this->_actualDataSize, &id, 2);
        LibC::memcpy(this->_data + this->_actualDataSize + 2, convert.tab, 2);
        LibC::memcpy(this->_data + this->_actualDataSize + 4, data, size);
        this->_actualDataSize += 4 + size;
    }

  template<typename T>
    void					appendToData(short id, T data)
    {
      union				uConvert
      {
	T				data;
	char				tab[sizeof(T)];
      }					convert;

      convert.data = data;
      LibC::memcpy(this->_data + this->_actualDataSize, &id, 2);
      LibC::memcpy(this->_data + this->_actualDataSize + 2, convert.tab, sizeof(T));
      this->_actualDataSize += 2 + sizeof(T);
    }

  template<typename T>
    T						getIntTypeInData(int index)
    {
      int					curSize = (this->_fmt.size()) ? (this->_fmt.size() + 2) : (0);
      int					curIdx = 0;
      union				uConvert
      {
	T				data;
	char			tab[sizeof(T)];
      }					convert;

      convert.data = 0;
      while (curIdx < index)
	{
	  try
	    {
	      this->consumeFormat(&curIdx, &curSize);
	    }
	  catch (std::exception &e)
	    {
	      throw e;
	      return (convert.data);
	    }
	}
      if (curIdx == index && this->_fmt.size())
	{
	  if (this->_lfmt.size())
	    {
	      if (this->_lfmt[((curIdx  - this->_fmt.find(this->_lfmt)) % this->_lfmt.size())] == 'l')
		{
		  convert.tab[0] = this->_data[curSize + 2];
		  convert.tab[1] = this->_data[curSize + 2 + 1];
		}
	      if (this->_lfmt[((curIdx  - this->_fmt.find(this->_lfmt)) % this->_lfmt.size())] == 'c')
		{
		  convert.tab[0] = this->_data[curSize + 2];
		}
	    }
	  else
	    {
	      if (this->_fmt[curIdx] == 'l')
		{
		  convert.tab[0] = this->_data[curSize + 2];
		  convert.tab[1] = this->_data[curSize + 2 + 1];
		}
	      if (this->_fmt[curIdx] == 'c')
		{
		  convert.tab[0] = this->_data[curSize + 2];
		}
	    }
	  this->_lfmt = "";
	}
      return (convert.data);
    }
};

template<>
void					Packet::appendToData(short id, const std::string& data);

template<>
void					Packet::appendToData(short id, const char *data);
