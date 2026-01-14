
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>

class MicroParam
{

protected:
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

  virtual void printDefinition(Stream & stream, char seperator) = 0;
  virtual void printValue(Stream & stream) = 0;
  
};

class MicroParamInt : public MicroParam
{
private:
  int32_t value_;
  const int32_t min_;
  const int32_t max_;

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

  void printValue(Stream & stream) override {
      stream.print(value_);
  }

  void printDefinition(Stream & stream, char seperator ) override {
    stream.print(min_);
    stream.print(seperator);
    stream.print(max_);
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
  const float min_;
  const float max_;

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

 void printValue(Stream & stream) override {
      stream.print(value_);
  }

  void printDefinition(Stream & stream, char seperator ) override {
    stream.print(min_);
    stream.print(seperator);
    stream.print(max_);

  }

  MicroParamFloat( float initial, float min, float max)
      : MicroParam('f'), min_(min), max_(max)
  {
    setFloat(initial);
  }

};



class MicroParamEnum : public MicroParam
{
private:
  int32_t value_;
  const char **labels_;
  int32_t count_;

public:
  MicroParamEnum( const char **labels, int32_t value, int32_t count)
      : MicroParam('e'), labels_(labels), count_(count) {
        setInt(value);
      }

  void setInt(int32_t i) override
  {
    value_ = Micro::modulo(i, count_);
  }

  int32_t getInt() const override { return value_; }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  float getFloat() const override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = Micro::map<float>(in, inMin, inMax, 0, count_ - 1);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, 0, count_ - 1));
  }
};


// Example use MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };
typedef Micro::Bind<MicroParam *> MicroParamBind;

// Example use MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));
typedef Micro::Binder<MicroParam *> MicroParamBinder;

/* class MicroParamBinder : public Micro::Binder<MicroParam*> {
public:
    using Micro::Binder<MicroParam*>::Binder;
    void printConfig(size_t index, Stream * stream) {
            sttream->print(binder.getKey(i));
            stream->pprint(' ');
            getPointer(i)->printConfig(stream);
            stream->println();
    }
}; */



#endif // __MICRO_PARAM_H__
