
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroTof.h>

class MicroParam
{

private:
  char type_;

public:
  MicroParam(char type) : type_(type)
  {
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
    value_ = MicroTof::clamp<int32_t>(i, min_, max_);
  }

  int32_t getInt() const override { return value_; }

  void setFloat(float f) override
  {
    setInt(floor(f));
  }

  float getFloat() const override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    float mapped = MicroTof::map<float>(in, inMin, inMax, min_, max_);
    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(MicroTof::map<int32_t>(in, inMin, inMax, min_, max_));
  }

  MicroParamInt(int32_t initial, int32_t min, int32_t max)
      : MicroParam('i')
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
    value_ = MicroTof::clamp<float>(f, min_, max_);
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
    float mapped = MicroTof::map<float>(in, inMin, inMax, min_, max_);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(MicroTof::map<int32_t>(in, inMin, inMax, min_, max_));
  }

  MicroParamFloat( float initial, float min, float max)
      : MicroParam('f'), min_(min), max_(max)
  {
    setFloat(initial);
  }

};

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
    value_ = MicroTof::modulo(i, count_);
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
    float mapped = MicroTof::mapf(in, inMin, inMax, 0, count_ - 1);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, 0, count_ - 1));
  }
};
*/
#endif // __MICRO_PARAM_H__
