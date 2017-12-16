#pragma once
#include <istream>
// #include "streambuffs/sub_streambuf.h"

#include <streambuf>
#include <istream>
#include <cstdint>

template <typename ELEM_TYPE, typename TRAITS_TYPE>
class sub_streambuf :
  public std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE>
{
  public:
    typedef std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE> base_type;
    typedef typename std::basic_streambuf<ELEM_TYPE, TRAITS_TYPE>::traits_type traits_type;

    typedef typename base_type::char_type char_type;
    typedef typename base_type::int_type  int_type;
    typedef typename base_type::pos_type  pos_type;
    typedef typename base_type::off_type  off_type;

    sub_streambuf()
      : _inputStream(nullptr)
      , _startPosition(0)
      , _currentPosition(0)
      , _endPosition(0)
    {

    }

    sub_streambuf(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset, size_t length)
      : sub_streambuf()
    {
      init(input, startOffset, length);
    }

    void init(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset, size_t length)
    {
      _inputStream = &input;
      _startPosition = startOffset;
      _currentPosition = startOffset;
      _endPosition = startOffset + static_cast<pos_type>(length);
      _internalBuffer = new ELEM_TYPE[INTERNAL_BUFFER_SIZE];

      // set stream buffer
      ELEM_TYPE* endOfOutputBuffer = _internalBuffer + INTERNAL_BUFFER_SIZE;
      this->setg(endOfOutputBuffer, endOfOutputBuffer, endOfOutputBuffer);
    }

    bool is_init() const
    {
      return (_inputStream != nullptr && _internalBuffer != nullptr);
    }

    virtual ~sub_streambuf()
    {
      if (_internalBuffer != nullptr)
      {
        delete[] _internalBuffer;
      }
    }

  protected:
    int_type underflow() override
    {
      // buffer exhausted
      if (this->gptr() >= this->egptr())
      {
        ELEM_TYPE* base = _internalBuffer;

        _inputStream->seekg(_currentPosition, std::ios::beg);
        _inputStream->read(_internalBuffer, std::min(static_cast<size_t>(INTERNAL_BUFFER_SIZE), static_cast<size_t>(_endPosition - _currentPosition)));
        size_t n = static_cast<size_t>(_inputStream->gcount());

        _currentPosition += n;

        if (n == 0)
        {
          return traits_type::eof();
        }

        // set buffer pointers
        this->setg(base, base, base + n);
      }

      return traits_type::to_int_type(*this->gptr());
    }
    
  private:
    enum : size_t
    {
      INTERNAL_BUFFER_SIZE = 1 << 15
    };

    ELEM_TYPE* _internalBuffer;

    std::basic_istream<ELEM_TYPE, TRAITS_TYPE>* _inputStream;
    pos_type _startPosition;
    pos_type _currentPosition;
    pos_type _endPosition;
};

/**
 * \brief Basic input substream. Creates a virtual stream over an existing input stream.
 *        The substream starts at the position 0 and continues until EOF or the specified length.
 */
template <typename ELEM_TYPE, typename TRAITS_TYPE>
class basic_isubstream
  : public std::basic_istream<ELEM_TYPE, TRAITS_TYPE>
{
  public:
    typedef typename std::basic_istream<ELEM_TYPE, TRAITS_TYPE>::pos_type pos_type;

    basic_isubstream()
      : std::basic_istream<ELEM_TYPE, TRAITS_TYPE>(&_subStreambuf)
    {

    }

    basic_isubstream(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset = 0)
      : std::basic_istream<ELEM_TYPE, TRAITS_TYPE>(&_subStreambuf)
      , _subStreambuf(input, startOffset, static_cast<size_t>(-1))
    {

    }

    basic_isubstream(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset, size_t length)
      : std::basic_istream<ELEM_TYPE, TRAITS_TYPE>(&_subStreambuf)
      , _subStreambuf(input, startOffset, length)
    {

    }

    void init(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset = 0)
    {
      _subStreambuf.init(input, startOffset, static_cast<size_t>(-1));
    }

    void init(std::basic_istream<ELEM_TYPE, TRAITS_TYPE>& input, pos_type startOffset, size_t length)
    {
      _subStreambuf.init(input, startOffset, length);
    }

    bool is_init() const
    {
      return _subStreambuf.is_init();
    }

  private:
    sub_streambuf<ELEM_TYPE, TRAITS_TYPE> _subStreambuf;
};

//////////////////////////////////////////////////////////////////////////

typedef basic_isubstream<uint8_t, std::char_traits<uint8_t>>  byte_isubstream;
typedef basic_isubstream<char, std::char_traits<char>>        isubstream;
typedef basic_isubstream<wchar_t, std::char_traits<wchar_t>>  wisubstream;
