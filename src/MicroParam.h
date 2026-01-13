
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>

class MicroParam
{

private:
  char type_;

public:
  MicroParam(char type) : type_(type)
  {
  }
  
  char getType() { 
    return type_ ;
  }
  virtual void setInt(int32_t v) = 0;
  virtual int32_t getInt() const = 0;

  virtual void setFloat(float v) = 0;
  virtual float getFloat() const = 0;

  virtual void mapFloat(float in, float inMin, float inMax) = 0;
  virtual void mapInt(int32_t in, int32_t inMin, int32_t inMax) = 0;
};

class MicroParamInt : public MicroParam
{
private:
  int32_t value_;
  int32_t min_;
  int32_t max_;

public:
  void setInt(int32_t i) override
  {
    value_ = Micro::clamp<int32_t>(i, min_, max_);
  }

  int32_t getInt() const override { return value_; }

  void setFloat(float f) override
  {
    setInt(floor(f));
  }

  float getFloat() const override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    float mapped = Micro::map<float>(in, inMin, inMax, min_, max_);
    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(Micro::map<int32_t>(in, inMin, inMax, min_, max_));
  }

  MicroParamInt(int32_t initial, int32_t min, int32_t max)
      : MicroParam('i'), min_(min), max_(max)
  {
    setInt(initial);
  }
};


class MicroParamFloat : public MicroParam
{
private:
  float value_;
  float min_;
  float max_;

public:
  void setFloat(float f) override
  {
    value_ = Micro::clamp<float>(f, min_, max_);
  }

  float getFloat() const override { return value_; }

  void setInt(int32_t i) override
  {
    setFloat((float)i);
  }

  int32_t getInt() const override { return floor(value_); }

  void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = Micro::map<float>(in, inMin, inMax, min_, max_);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(Micro::map<int32_t>(in, inMin, inMax, min_, max_));
  }

  MicroParamFloat( float initial, float min, float max)
      : MicroParam('f'), min_(min), max_(max)
  {
    setFloat(initial);
  }

};

/* class MicroParamBindInt : public MicroParam
{
    int32_t & value_; // reference to the actual int
    int32_t min_ = 0;
    int32_t max_ = 0;

public:
    // constructor: type 'i' for int, key not stored in base
    MicroParamBindInt(int32_t &value, int32_t min, int32_t max)
        : MicroParam('i'), value_(value), min_(min), max_(max)
    {
        setInt(value); // clamp initial value
    }

    // ===============================
    // MicroParam int interface
    void setInt(int32_t i) override
  {
    value_ = Micro::clamp<int32_t>(i, min_, max_);
  }

  int32_t getInt() const override { return value_; }

  void setFloat(float f) override
  {
    setInt(floor(f));
  }

  float getFloat() const override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    float mapped = Micro::map<float>(in, inMin, inMax, min_, max_);
    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(Micro::map<int32_t>(in, inMin, inMax, min_, max_));
  }
}; */

/*
class MicroParamEnum : public MicroParam
{
private:
  int32_t value_;
  const char **labels_;
  int32_t count_;

public:
  MicroParamEnum( const char **labels, int32_t count)
      : MicroParam('e'), labels_(labels), count_(count), value_(0) {}

  void setInt(int32_t i) override
  {
    value_ = Micro::modulo(i, count_);
  }

  int32_t getInt() override { return value_; }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  float getFloat() override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = Micro::mapf(in, inMin, inMax, 0, count_ - 1);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, 0, count_ - 1));
  }
};

*/
// Example use MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };
typedef Micro::Bind<MicroParam *> MicroParamBind;

// Example use MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));
typedef Micro::Binder<MicroParam *> MicroParamBinder;

/* class Bindings
{

    size_t count_ = 0;
    MicroParamBind * pairs_;

public:
    KeyParamCollection(MicroParamBind * pairs, size_t count)
        : pairs_(pairs), count_(count)
    {

    }

    // get pointer to the stored value by key
    MicroParam * get(const char *key)
    {
        for (size_t i = 0; i < count_; ++i)
        {
            if (strcmp(pairs_[i].key_, key) == 0)
                return pairs_[i].pointer_;
        }
        return nullptr;
    }

    // get pointer by index
    MicroParam * get(size_t index)
    {
        if (index >= count_)
            return nullptr;
        return pairs_[index].pointer_;
    }

    size_t getCount() const { return count_; }
}; */

#endif // __MICRO_PARAM_H__
