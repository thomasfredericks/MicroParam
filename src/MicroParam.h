
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
    value = Micro::clamp(value, min, max);
  }

  // Assignment from raw value → calls set()
  MicroInt& operator=(int32_t v) {
    value = Micro::clamp(v, min, max);
    return *this;
  }

  // Implicit conversion to int32_t → calls get()
  operator int32_t() const {
    return value;
  }

  // Optional explicit API
  void set(int32_t v) {
    value = Micro::clamp(v, min, max);
  }

  int32_t get() const {
    return value;
  }
};


struct MicroFloat {
  float value;
  const float min, max;

  constexpr MicroFloat(float v, float min_, float max_)
    : value(v), min(min_), max(max_)
  {
    value = Micro::clamp(value, min, max);
  }

  MicroFloat& operator=(float v) {
    value = Micro::clamp(v, min, max);
    return *this;
  }

  operator float() const {
    return value;
  }

  void set(float v) {
    value = Micro::clamp(v, min, max);
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
    value = Micro::modulo(value, count);
  }

  // Assignment operator → sets the value with modulo
  MicroEnum& operator=(int32_t v) {
    value = Micro::modulo(v, count);
    return *this;
  }

  // Explicit set method
  void set(int32_t v) {
    value = Micro::modulo(v, count);
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



// ======================= MicroParam =======================
struct MicroParam {
  enum Type : uint8_t { INT, FLOAT, ENUM } type;
  const char *key;

  union {
    MicroInt *i;
    MicroFloat *f;
    MicroEnum *e;
  } ptr;

  // --------- Constructors ---------
  static MicroParam bind(const char *k, MicroInt &v) {
    MicroParam p;
    p.type = INT;
    p.key = k;
    p.ptr.i = &v;
    return p;
  }

  static MicroParam bind(const char *k, MicroFloat &v) {
    MicroParam p;
    p.type = FLOAT;
    p.key = k;
    p.ptr.f = &v;
    return p;
  }

  static MicroParam bind(const char *k, MicroEnum &v) {
    MicroParam p;
    p.type = ENUM;
    p.key = k;
    p.ptr.e = &v;
    return p;
  }

  // --------- Access ---------
  void setInt(int32_t v) {
    switch (type) {
    case INT:
      ptr.i->set(v);
      break;
    case FLOAT:
      ptr.f->set((float)v);
      break;
    case ENUM:
      ptr.e->set(v);
      break;
    }
  }

  int32_t getInt() const {
    switch (type) {
    case INT:
      return ptr.i->value;
    case FLOAT:
      return (int32_t)ptr.f->value;
    case ENUM:
      return ptr.e->value;
    }
    return 0;
  }

  void setFloat(float v) {
    switch (type) {
    case FLOAT:
      ptr.f->set(v);
      break;
    case INT:
      ptr.i->set((int32_t)floor(v));
      break;
    case ENUM:
      ptr.e->set((int32_t)floor(v));
      break;
    }
  }

  float getFloat() const {
    switch (type) {
    case FLOAT:
      return ptr.f->value;
    case INT:
      return (float)ptr.i->value;
    case ENUM:
      return (float)ptr.e->value;
    }
    return 0.0f;
  }
};


#endif // __MICRO_PARAM_H__
