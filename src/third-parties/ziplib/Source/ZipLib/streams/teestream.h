#pragma once
#include <ostream>
#include <cstdint>
// #include "streambuffs/tee_streambuff.h"

#include <streambuf>
#include <algorithm>

template <typename ELEM_TYPE, typename TRAITS_TYPE>
class tee_streambuf:
  public std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE>
{
  public:
    typedef std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE> base_type;
    typedef typename std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE>::traits_type traits_type;

    typedef typename base_type::char_type char_type;
    typedef typename base_type::int_type  int_type;
    typedef typename base_type::pos_type  pos_type;
    typedef typename base_type::off_type  off_type;

    tee_streambuf& bind(base_type* sb)
    {
      _sbCollection.push_back(sb);
      return *this;
    }

    tee_streambuf& bind(std::basic_ostream<ELEM_TYPE, TRAITS_TYPE>& stream)
    {
      _sbCollection.push_back(stream.rdbuf());
      return *this;
    }

  protected:
    int_type overflow(int_type c = traits_type::eof()) override
    {
      bool failed = false;
      
      for (auto* sb : _sbCollection)
      {
        if (sb->sputc(c) == traits_type::eof())
        {
          failed = true;
        }
      }

      return failed ? traits_type::eof() : c;
    }
  
    int sync() override
    {
      bool failed = false;

      for (auto* sb : _sbCollection)
      {
        if (sb->pubsync() == -1)
        {
          failed = true;
        }
      }

      return failed ? -1 : 0;
    }
  
  private:
    std::vector<base_type*> _sbCollection;
};

/**
 * \brief Basic teestream. Distributes the input data into every bound output stream.
 */
template <typename ELEM_TYPE, typename TRAITS_TYPE>
class basic_teestream
  : public std::basic_ostream<ELEM_TYPE, TRAITS_TYPE>
{
  public:
    basic_teestream()
      : std::basic_ostream<ELEM_TYPE, TRAITS_TYPE>(&_teeStreambuf)
    {

    }

    basic_teestream(basic_teestream<ELEM_TYPE, TRAITS_TYPE>&& other)
      : basic_teestream()
    {
      _teeStreambuf = std::move(other._teeStreambuf);
      this->swap(other);
    }

    basic_teestream& bind(std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE>* sb)
    {
      _teeStreambuf.bind(sb);
      return *this;
    }

    basic_teestream& bind(std::basic_ostream<ELEM_TYPE, TRAITS_TYPE>& stream)
    {
      _teeStreambuf.bind(stream);
      return *this;
    }

    basic_teestream move()
    {
      return std::move(*this);
    }

  private:
    basic_teestream(const basic_teestream&);
    basic_teestream& operator = (const basic_teestream&);

    tee_streambuf<ELEM_TYPE, TRAITS_TYPE> _teeStreambuf;
};

//////////////////////////////////////////////////////////////////////////

typedef basic_teestream<uint8_t, std::char_traits<uint8_t>>  byte_teestream;
typedef basic_teestream<char, std::char_traits<char>>        teestream;
typedef basic_teestream<wchar_t, std::char_traits<wchar_t>>  wteestream;
