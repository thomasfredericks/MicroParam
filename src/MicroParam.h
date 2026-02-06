
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>

namespace MicroParam
{
  enum Type
  {
    INT = 'i',
    FLOAT = 'f',
    ENUM = 'e',
    STRING = 's',
    BYTE = 'y',
    BLOB = 'b'
  };
}

// ======================= Base types =======================
struct MicroParamByte
{
  uint8_t value_;
  const uint8_t min_, max_;

  MicroParamByte(uint8_t v, uint8_t min = 0, uint8_t max = 255)
      : value_(v), min_(min), max_(max)
  {
    value_ = (v < min_) ? min_ : ((v > max_) ? max_ : v);
  }

  // Assignment
  MicroParamByte &operator=(uint8_t v)
  {
    value_ = (v < min_) ? min_ : ((v > max_) ? max_ : v);
    return *this;
  }

  // Conversion
  operator uint8_t() const { return value_; }

  // Explicit API
  void set(uint8_t v) { *this = v; }
  uint8_t get() const { return value_; }

  // Prefix increment: ++x
  MicroParamByte &operator++()
  {
    if (value_ < max_)
      value_++;
    return *this;
  }

  // Postfix increment: x++
  MicroParamByte operator++(int)
  {
    MicroParamByte temp = *this;
    ++(*this);
    return temp;
  }

  // Prefix decrement: --x
  MicroParamByte &operator--()
  {
    if (value_ > min_)
      value_--;
    return *this;
  }

  // Postfix decrement: x--
  MicroParamByte operator--(int)
  {
    MicroParamByte temp = *this;
    --(*this);
    return temp;
  }
};

struct MicroParamInt
{
  int32_t value_;
  const int32_t min_, max_;

  // Constructor
  MicroParamInt(int32_t v, int32_t min, int32_t max)
      : min_(min), max_(max)
  {
    value_ = microClamp<int32_t>(value_, min_, max_);
  }

  // Assignment from raw value → calls set()
  MicroParamInt &operator=(int32_t v)
  {
    value_ = microClamp<int32_t>(v, min_, max_);
    return *this;
  }

  // Conversion
  operator int32_t() const
  {
    return value_;
  }

  // Explicit API
  void set(int32_t v) { value_ = microClamp<int32_t>(v, min_, max_); }
  int32_t get() const { return value_; }

  // Prefix increment: ++x
  MicroParamInt &operator++()
  {
    set(value_ + 1);
    return *this;
  }

  // Postfix increment: x++
  MicroParamInt operator++(int)
  {
    MicroParamInt temp = *this;
    set(value_ + 1);
    return temp;
  }

  // Prefix decrement: --x
  MicroParamInt &operator--()
  {
    set(value_ - 1);
    return *this;
  }

  // Postfix decrement: x--
  MicroParamInt operator--(int)
  {
    MicroParamInt temp = *this;
    set(value_ - 1);
    return temp;
  }
};

struct MicroParamFloat
{
  float value_;
  const float min_, max_;

  MicroParamFloat(float v, float min, float max)
      : value_(v), min_(min), max_(max)
  {
    value_ = microClamp<float>(value_, min_, max_);
  }

  MicroParamFloat &operator=(float v)
  {
    value_ = microClamp<float>(v, min_, max_);
    return *this;
  }

  operator float() const
  {
    return value_;
  }

  void set(float v)
  {
    value_ = microClamp<float>(v, min_, max_);
  }

  float get() const
  {
    return value_;
  }
};

struct MicroParamEnum
{
  int32_t value_;
  const int32_t count_;
  const char **labels_;

  // Constructor
  MicroParamEnum(int32_t v, int32_t count_, const char **labels_)
      : count_(count_), labels_(labels_)
  {
    value_ = microModulo(value_, count_);
  }

  // Assignment operator → sets the value with modulo
  MicroParamEnum &operator=(int32_t v)
  {
    value_ = microModulo(v, count_);
    return *this;
  }

  // Explicit set method
  void set(int32_t v)
  {
    value_ = microModulo(v, count_);
  }

  // Explicit get method
  int32_t get() const
  {
    return value_;
  }

  // Implicit conversion to int32_t
  operator int32_t() const
  {
    return value_;
  }

  // Get the label string
  const char *label() const
  {
    return labels_ ? labels_[value_] : nullptr;
  }
};

// COMPLEX TYPES

struct MicroParamBlob
{
  uint8_t *data_;
  uint32_t capacity_;
  uint32_t length_;

  MicroParamBlob(uint8_t *buffer, uint32_t capacity, uint32_t length = 0)
      : data_(buffer), capacity_(capacity), length_(length)
  {
  }


  const uint8_t *data() const { return data_; }

  uint32_t length() const { return length_; }
  uint32_t capacity() const { return capacity_; }

 bool set(const uint8_t* buffer, uint32_t length)
{
    if (length > capacity_)
        return false;

    memcpy(data_, buffer, length);
    length_ = length;

    return true;
}


  // ---- array operators ----
  uint8_t &operator[](uint32_t index)
  {
    return data_[index];
  }

  const uint8_t &operator[](uint32_t index) const
  {
    return data_[index];
  }
};

struct MicroParamString
{
  char *data_;
  uint32_t capacity_;
  uint32_t length_;

  MicroParamString(char *buffer, uint32_t capacity)
      : data_(buffer), capacity_(capacity), length_(0)
  {
    if (capacity_ == 0 || data_ == nullptr)
      return;

    length_ = strnlen(data_, capacity_ - 1);
    data_[length_] = '\0';
  }

  const char *get() const { return data_; }

  uint32_t length() const { return length_; }
  uint32_t capacity() const { return capacity_; }

  bool set(const char *s)
  {
    if (capacity_ == 0 || data_ == nullptr || s == nullptr)
      return false;

    size_t new_len = strnlen(s, capacity_ - 1);
    if (new_len >= capacity_)
      return false;

    memcpy(data_, s, new_len);
    data_[new_len] = '\0';
    length_ = new_len;
    return true;
  }

  char *data() { return data_; }
};



#endif // __MICRO_PARAM_H__
