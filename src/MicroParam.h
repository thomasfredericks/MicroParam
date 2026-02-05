
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>


// ======================= Base types =======================
struct MicroParamByte {
    uint8_t value_;
    const uint8_t min_, max_;

    constexpr MicroParamByte(uint8_t v, uint8_t min=0, uint8_t max=255)
        : value_(v), min_(min), max_(max)
    {
        value_ = (v < min_) ? min_ : ((v > max_) ? max_ : v);
    }

    // Assignment
    MicroParamByte& operator=(uint8_t v) {
        value_ = (v < min_) ? min_ : ((v > max_) ? max_ : v);
        return *this;
    }

    // Conversion
    operator uint8_t() const { return value_; }

    // Explicit API
    void set(uint8_t v) { *this = v; }
    uint8_t get() const { return value_; }

    // Prefix increment: ++x
    MicroParamByte& operator++() {
        if (value_ < max_) value_++;
        return *this;
    }

    // Postfix increment: x++
    MicroParamByte operator++(int) {
        MicroParamByte temp = *this;
        ++(*this);
        return temp;
    }

    // Prefix decrement: --x
    MicroParamByte& operator--() {
        if (value_ > min_) value_--;
        return *this;
    }

    // Postfix decrement: x--
    MicroParamByte operator--(int) {
        MicroParamByte temp = *this;
        --(*this);
        return temp;
    }
};


struct MicroParamInt {
  int32_t value_;
  const int32_t min_, max_;

  // Constructor
  constexpr MicroParamInt(int32_t v, int32_t min, int32_t max)
    : value_(v), min_(min), max_(max)
  {
    value_ = microClamp<int32_t>(value_, min_, max_);
  }

  // Assignment from raw value → calls set()
  MicroParamInt& operator=(int32_t v) {
    value_ = microClamp<int32_t>(v, min_, max_);
    return *this;
  }

  // Conversion
  operator int32_t() const {
    return value_;
  }

   // Explicit API
  void set(int32_t v) { value_ = microClamp<int32_t>(v, min_, max_); }
  int32_t get() const { return value_; }
 
  // Prefix increment: ++x
  MicroParamInt& operator++() {
    set(value_ + 1);
    return *this;
  }

  // Postfix increment: x++
  MicroParamInt operator++(int) {
    MicroParamInt temp = *this;
    set(value_ + 1);
    return temp;
  }

  // Prefix decrement: --x
  MicroParamInt& operator--() {
    set(value_ - 1);
    return *this;
  }

  // Postfix decrement: x--
  MicroParamInt operator--(int) {
    MicroParamInt temp = *this;
    set(value_ - 1);
    return temp;
  }

};


struct MicroParamFloat {
  float value_;
  const float min_, max_;

  constexpr MicroParamFloat(float v, float min, float max)
    : value_(v), min_(min), max_(max)
  {
    value_ = microClamp<float>(value_, min_, max_);
  }

  MicroParamFloat& operator=(float v) {
    value_ = microClamp<float>(v, min_, max_);
    return *this;
  }

  operator float() const {
    return value_;
  }

  void set(float v) {
    value_ = microClamp<float>(v, min_, max_);
  }

  float get() const {
    return value_ ;
  }
};


struct MicroParamEnum {
  int32_t value_;
  const int32_t count_;
  const char **labels_;

  // Constructor
  constexpr MicroParamEnum(int32_t v, int32_t count_, const char **labels_)
    : value_(v), count_(count_), labels_(labels_)
  {
    value_ = microModulo(value_, count_);
  }

  // Assignment operator → sets the value with modulo
  MicroParamEnum& operator=(int32_t v) {
    value_ = microModulo(v, count_);
    return *this;
  }

  // Explicit set method
  void set(int32_t v) {
    value_ = microModulo(v, count_);
  }

  // Explicit get method
  int32_t get() const {
    return value_;
  }

  // Implicit conversion to int32_t
  operator int32_t() const {
    return value_;
  }

  // Get the label string
  const char* label() const {
    return labels_ ? labels_[value_] : nullptr;
  }
};


// ======================= MicroParamBind =======================
class MicroParamBind {
  private:
  enum Type : uint8_t { INT = 'i' , FLOAT = 'f', ENUM = 'e' } type_;
  const char *key_;
  const uint32_t keyHash_;
  

  union {
    MicroParamInt   *i;
    MicroParamFloat *f;
    MicroParamEnum  *e;
  } ptr_;

  public:
  // --------- Constructors ---------
  MicroParamBind(const char *k, MicroParamInt &v)
      : type_(INT), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.i = &v;
  }

  MicroParamBind(const char *k, MicroParamFloat &v)
      : type_(FLOAT), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.f = &v;
  }

  MicroParamBind(const char *k, MicroParamEnum &v)
      : type_(ENUM), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.e = &v;
  }

  Type getType() const {
    return type_;
  }

  const char *getKey() const {
    return key_;
  }

  bool matches(const char *k) const {
    return strcmp(key_, k) == 0;
  }

  bool matches(uint32_t hash) const {
    return keyHash_ == hash;
  }

  bool matches(uint32_t hash, const char *addr) const {
        if (hash != keyHash_) return false;
        return strcmp(key_, addr) == 0; 
    }

  // --------- Access ---------
  void setInt(int32_t v) {
    switch (type_) {
      case INT:   ptr_.i->set(v); break;
      case FLOAT: ptr_.f->set((float)v); break;
      case ENUM:  ptr_.e->set(v); break;
    }
  }

  int32_t getInt() const {
    switch (type_) {
      case INT:   return ptr_.i->get();
      case FLOAT: return (int32_t)ptr_.f->get();
      case ENUM:  return ptr_.e->get();
    }
    return 0;
  }

  void setFloat(float v) {
    switch (type_) {
      case FLOAT: ptr_.f->set(v); break;
      case INT:   ptr_.i->set((int32_t)floor(v)); break;
      case ENUM:  ptr_.e->set((int32_t)floor(v)); break;
    }
  }

  float getFloat() const {
    switch (type_) {
      case FLOAT: return ptr_.f->get();
      case INT:   return (float)ptr_.i->get();
      case ENUM:  return (float)ptr_.e->get();
    }
    return 0.0f;
  }
};


#endif // __MICRO_PARAM_H__
