#include	<cstring>
#include	<cstdio>

#include	"../headers-shared/shared/Packet.h"
#include    "../headers-shared/shared/LibC.hh"

Packet::Packet(int uid)
{
  this->_magicNumber = Packet::MAGIC_NUMBER;
  this->_requestUID = (uid == -1) ? Packet::getUniqueIdentifier() : uid;
  this->_instruction = 0;
  this->_dataSize = 0;
  this->_data = 0;
  this->_serialization = 0;
  this->_actualDataSize = 0;
  this->_listsize = -1;
}

Packet::Packet(const Packet& packet)
{
  this->_actualDataSize = packet._actualDataSize;
  this->_dataSize = packet._dataSize;
  if (this->_dataSize > 0)
    {
      this->_data = new char[this->_dataSize];
      LibC::memcpy(this->_data, packet._data, packet._dataSize);
    }
  else
    this->_data = 0;
  this->_instruction = packet._instruction;
  if (packet._serialization != 0)
    {
      this->_serialization = new char[packet.size()];
      LibC::memcpy(this->_serialization, packet._serialization, packet.size());
    }
  else
    this->_serialization = 0;
  this->_magicNumber = packet._magicNumber;
  this->_requestUID = packet._requestUID;
}

Packet::~Packet()
{
    if (this->_serialization != 0)
        delete[] this->_serialization;
    if (this->_data)
        delete[] this->_data;
}

void		Packet::setMagicNumber(unsigned int magicNumber)
{
  this->_magicNumber = magicNumber;
}

void		Packet::setRequestUID(unsigned short requestUID)
{
  this->_requestUID = requestUID;
}

void		Packet::setInstruction(unsigned char instruction)
{
  this->_instruction = instruction;
}

void		Packet::setDataSize(unsigned int dataSize)
{
  this->_dataSize = dataSize;
}

void		Packet::setFormat(const std::string & data)
{
  this->_fmt = data;
  union				uConvert
    {
      short			data;
      char			tab[2];
    }					convert;

    convert.data = data.size();
    LibC::memcpy(this->_data + this->_actualDataSize, convert.tab, 2);
    LibC::memcpy(this->_data + this->_actualDataSize + 2, data.c_str(), data.size());
    this->_actualDataSize += 2 + data.size();
}

void		Packet::setData(char *data)
{
  this->_data = data;
}

unsigned int		Packet::getMagicNumber() const
{
  return (this->_magicNumber);
}

unsigned short		Packet::getRequestUID() const
{
  return (this->_requestUID);
}

unsigned char		Packet::getInstruction() const
{
  return (this->_instruction);
}

unsigned int			Packet::getDataSize() const
{
  return (this->_dataSize);
}

std::string		Packet::getFormat() const
{
  return (this->_fmt);
}

char		*Packet::getData() const
{
  return (this->_data);
}

void		Packet::updateData(unsigned int size)
{
  char	*tmp;

  if (this->_data == 0)
    {
      this->_data = size == 0 ? 0 : new char[size];
      this->_dataSize = size;
      return;
    }
  tmp = size == 0 ? 0 : new char[size];
  if (size != 0)
    {
      LibC::memcpy(tmp, this->_data, this->_dataSize);
      delete[] this->_data;
    }
  this->_data = tmp;
  this->_dataSize = size;
}

unsigned int	Packet::size() const
{
  return (Packet::HEADER_SIZE + this->_dataSize);
}

char		*Packet::serialize()
{
  char	*tmp;
  union				uConvert
  {
    short			data;
    char			tab[2];
  }				convert;

  convert.data = 0;
  if (this->_serialization != 0)
    delete[] this->_serialization;
  this->_serialization = new char[Packet::HEADER_SIZE + this->_dataSize];
  tmp = reinterpret_cast<char *>(&this->_magicNumber);
  this->_serialization[0] = tmp[0];
  this->_serialization[1] = tmp[1];
  this->_serialization[2] = tmp[2];
  tmp = reinterpret_cast<char *>(&this->_requestUID);
  this->_serialization[3] = tmp[0];
  this->_serialization[4] = tmp[1];
  tmp = reinterpret_cast<char *>(&this->_instruction);
  this->_serialization[5] = tmp[0];
  tmp = reinterpret_cast<char *>(&this->_dataSize);
  this->_serialization[6] = tmp[0];
  this->_serialization[7] = tmp[1];
  this->_serialization[8] = tmp[2];
  if (this->_data != 0)
    LibC::memcpy(this->_serialization + Packet::HEADER_SIZE + convert.data, this->_data, this->_dataSize);
  return (this->_serialization);
}

void				Packet::deserialize(char *packet)
{
  this->header(packet);
  this->_data = new char[this->_dataSize];
  LibC::memcpy(this->_data, packet + Packet::HEADER_SIZE , this->_dataSize);
  if (this->_dataSize >= 2)
  {
    this->getFormatInData();
  }
}

void				Packet::header(char *packet)
{
  union				uConvert
  {
    unsigned int		value;
    char			tab[3];
  }					convert;

  convert.value = 0;
  this->_magicNumber = convert.value;

  convert.tab[0] = *reinterpret_cast<const char *>(packet + Packet::MAGIC_NUMBER_INDEX);
  convert.tab[1] = *reinterpret_cast<const char *>(packet + Packet::MAGIC_NUMBER_INDEX + 1);
  convert.tab[2] = *reinterpret_cast<const char *>(packet + Packet::MAGIC_NUMBER_INDEX + 2);
  this->_magicNumber = convert.value;
  this->_requestUID = *reinterpret_cast<const unsigned short *>(packet + Packet::REQUEST_UID_INDEX);
  this->_instruction = *reinterpret_cast<const unsigned char *>(packet + Packet::INSTRUCTION_INDEX);
  convert.value = 0;
  convert.tab[0] = *reinterpret_cast<const char *>(packet + Packet::DATA_SIZE_INDEX);
  convert.tab[1] = *reinterpret_cast<const char *>(packet + Packet::DATA_SIZE_INDEX + 1);
  convert.tab[2] = *reinterpret_cast<const char *>(packet + Packet::DATA_SIZE_INDEX + 2);
  this->_dataSize = convert.value;
}

void				Packet::consumeFormat(int *curIdx, int *curSize)
{
    if (this->_fmt.size() && *curIdx < (int)(this->_fmt.size() + ((this->_lfmt.size()) ? (this->_lfmt.size() * this->_listsize) : (0))))
    {
        if (*curIdx < (int)this->_fmt.size() && this->_fmt[*curIdx] == 'l')
        {
            this->_listsize = getIntTypeInData<short>(*curIdx);
            *curIdx += 1;
            *curSize += 4;
            this->_lfmt = this->_fmt;
            this->_lfmt.erase(0,*curIdx);
        }
        else
        {
            if (this->_lfmt.size())
            {
	      if (this->_lfmt[((*curIdx - this->_fmt.find(this->_lfmt)) % this->_lfmt.size())] == 's')
                {
                    *curIdx += 1;
		    this->getStringInData(curSize);
                }
                else if (this->_lfmt[((*curIdx - this->_fmt.find(this->_lfmt)) % this->_lfmt.size())] == 'c')
                {
                    *curIdx += 1;
                    *curSize += 3;
                }
                else
                    *curIdx += 1;
            }
            else
            {
                if (this->_fmt[*curIdx] == 's')
                {
                    *curIdx += 1;
                    this->getStringInData(curSize);
                }
                else if (this->_fmt[*curIdx] == 'c')
                {
                    *curIdx += 1;
                    *curSize += 3;
                }
                else
                    *curIdx += 1;
            }
        }
    }
    else
        *curIdx += 1;
}

std::string			Packet::getStringInData(int *currSize) const
{
  std::string		str;
  union			uConvert
  {
    short		size;
    char		tab[2];
  }				convert;

  if (*currSize + 4 <= (int)this->getDataSize())
    {
      convert.tab[0] = this->_data[*currSize + 2];
      convert.tab[1] = this->_data[*currSize + 3];
    }
  else
    throw std::out_of_range("Error : not found");
  if (*currSize + 4 + convert.size <= (int)this->getDataSize())
    {
      char	*res;

      res = new char[convert.size + 1];
      res[convert.size] = 0;
      LibC::memcpy(res, this->_data + *currSize + 4, convert.size);
      str = std::string(res);
      delete[] res;
      *currSize += 4 + convert.size;
    }
  return (str);
}

std::string			Packet::getFormatInData()
{
  std::string		str;
  union			uConvert
  {
    short		size;
    char		tab[2];
  }				convert;

  if (2 <= (int)this->getDataSize())
    {
      convert.tab[0] = this->_data[0];
      convert.tab[1] = this->_data[1];
    }
  else
    throw std::out_of_range("Error : not found");
  if (2 + convert.size <= (int)this->getDataSize())
    {
      char	*res;

      res = new char[convert.size + 1];
      res[convert.size] = 0;
      LibC::memcpy(res, this->_data + 2, convert.size);
      str = std::string(res);
      delete[] res;
    }
  this->_fmt = str;
  return (str);
}


std::string				Packet::getStringInData(int index)
{
  int					curSize = (this->_fmt.size()) ? (this->_fmt.size() + 2) : (0);
    int					curIdx = 0;

    while (curIdx < index)
    {
        try
        {
            this->consumeFormat(&curIdx, &curSize);
        }
        catch (std::exception &e)
        {
            throw e;
            return ("");
        }
    }
    if (curIdx == index && this->_fmt.size())
    {
        if (this->_lfmt.size())
        {
            if (this->_lfmt[(curIdx - this->_fmt.find(this->_lfmt)) % this->_lfmt.size() ] == 's')
	      {
		this->_lfmt = "";
                return (this->getStringInData(&curSize));
	      }
	}
        else
        {
            if (this->_fmt[curIdx] == 's')
	      {
		this->_lfmt = "";
                return (this->getStringInData(&curSize));
	      }
	}
	this->_lfmt = "";
    }
    return ("");
}

void				Packet::show()
{
}


template<>
void					Packet::appendToData(short id, const std::string& data)
{
  union				uConvert
  {
    short			data;
    char			tab[2];
  }					convert;

  convert.data = data.size();
  LibC::memcpy(this->_data + this->_actualDataSize, &id, 2);
  LibC::memcpy(this->_data + this->_actualDataSize + 2, convert.tab, 2);
  LibC::memcpy(this->_data + this->_actualDataSize + 4, data.c_str(), data.size());
  this->_actualDataSize += 4 + data.size();
}

template<>
void					Packet::appendToData(short id, const char *str)
{
  union				uConvert
  {
    short			data;
    char			tab[2];
  }					convert;
  std::string			data;

  data = std::string(str);
  convert.data = data.size();
  LibC::memcpy(this->_data + this->_actualDataSize, &id, 2);
  LibC::memcpy(this->_data + this->_actualDataSize + 2, convert.tab, 2);
  LibC::memcpy(this->_data + this->_actualDataSize + 4, data.c_str(), data.size());
  this->_actualDataSize += 4 + data.size();
}
