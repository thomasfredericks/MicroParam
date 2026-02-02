
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>


// ======================= Base types =======================
struct MicroInt {
  int32_t value;
  const int32_t min, max;

  // Constructor
  constexpr MicroInt(int32_t v, int32_t min_, int32_t max_)
    : value(v), min(min_), max(max_)
  {
    value = microClamp<int32_t>(value, min, max);
  }

  // Assignment from raw value → calls set()
  MicroInt& operator=(int32_t v) {
    value = microClamp<int32_t>(v, min, max);
    return *this;
  }

  // Conversion
  operator int32_t() const {
    return value;
  }

   // Explicit API
  void set(int32_t v) { value = microClamp<int32_t>(v, min, max); }
  int32_t get() const { return value; }
 
  // Prefix increment: ++x
  MicroInt& operator++() {
    set(value + 1);
    return *this;
  }

  // Postfix increment: x++
  MicroInt operator++(int) {
    MicroInt temp = *this;
    set(value + 1);
    return temp;
  }

  // Prefix decrement: --x
  MicroInt& operator--() {
    set(value - 1);
    return *this;
  }

  // Postfix decrement: x--
  MicroInt operator--(int) {
    MicroInt temp = *this;
    set(value - 1);
    return temp;
  }

};


struct MicroFloat {
  float value;
  const float min, max;

  constexpr MicroFloat(float v, float min_, float max_)
    : value(v), min(min_), max(max_)
  {
    value = microClamp<float>(value, min, max);
  }

  MicroFloat& operator=(float v) {
    value = microClamp(v, min, max);
    return *this;
  }

  operator float() const {
    return value;
  }

  void set(float v) {
    value = microClamp<float>(v, min, max);
  }

  float get() const {
    return value;
  }
};


struct MicroEnum {
  int32_t value;
  const int32_t count;
  const char **labels;

  // Constructor
  constexpr MicroEnum(int32_t v, int32_t count_, const char **labels_)
    : value(v), count(count_), labels(labels_)
  {
    value = microModulo(value, count);
  }

  // Assignment operator → sets the value with modulo
  MicroEnum& operator=(int32_t v) {
    value = microModulo(v, count);
    return *this;
  }

  // Explicit set method
  void set(int32_t v) {
    value = microModulo(v, count);
  }

  // Explicit get method
  int32_t get() const {
    return value;
  }

  // Implicit conversion to int32_t
  operator int32_t() const {
    return value;
  }

  // Get the label string
  const char* label() const {
    return labels ? labels[value] : nullptr;
  }
};


// ======================= MicroBind =======================
class MicroBind {
  private:
  enum Type : uint8_t { INT = 'i' , FLOAT = 'f', ENUM = 'e' } type;
  const char *key;
  

  union {
    MicroInt   *i;
    MicroFloat *f;
    MicroEnum  *e;
  } ptr;

  public:
  // --------- Constructors ---------
  MicroBind(const char *k, MicroInt &v)
      : type(INT), key(k) {
    ptr.i = &v;
  }

  MicroBind(const char *k, MicroFloat &v)
      : type(FLOAT), key(k) {
    ptr.f = &v;
  }

  MicroBind(const char *k, MicroEnum &v)
      : type(ENUM), key(k) {
    ptr.e = &v;
  }

  Type getType() const {
    return type;
  }

  const char *getKey() const {
    return key;
  }

  bool checkKey(const char *k) const {
    return strcmp(key, k) == 0;
  }

  // --------- Access ---------
  void setInt(int32_t v) {
    switch (type) {
      case INT:   ptr.i->set(v); break;
      case FLOAT: ptr.f->set((float)v); break;
      case ENUM:  ptr.e->set(v); break;
    }
  }

  int32_t getInt() const {
    switch (type) {
      case INT:   return ptr.i->value;
      case FLOAT: return (int32_t)ptr.f->value;
      case ENUM:  return ptr.e->value;
    }
    return 0;
  }

  void setFloat(float v) {
    switch (type) {
      case FLOAT: ptr.f->set(v); break;
      case INT:   ptr.i->set((int32_t)floor(v)); break;
      case ENUM:  ptr.e->set((int32_t)floor(v)); break;
    }
  }

  float getFloat() const {
    switch (type) {
      case FLOAT: return ptr.f->value;
      case INT:   return (float)ptr.i->value;
      case ENUM:  return (float)ptr.e->value;
    }
    return 0.0f;
  }
};


#endif // __MICRO_PARAM_H__
