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

// ============================================================================================

const char* labels[3] = { "A", "B", "C" };
MicroParamInt   pInt(5, 0, 127);
MicroParamFloat pFloat(0.5f, 0.0f, 1.0f);
MicroParamEnum pEnum(labels, 0, 3);

MicroParamInt   pInt1(5, 0, 127);
MicroParamFloat pFloat1(0.5f, 0.0f, 1.0f);
MicroParamEnum pEnum1(labels, 0, 3);


MicroParam *values[] = { &pInt, &pFloat, &pEnum ,&pInt1, &pFloat1, &pEnum1 };
constexpr size_t VALUE_COUNT = sizeof(values) / sizeof(values[0]);

// ============================================================================================

#define ITERATIONS 200000

struct BenchResult {
  uint32_t cycles;
  uint32_t heap;
  uint32_t stack;
};

volatile int32_t sink_i;
volatile float   sink_f;

static inline uint32_t cycles()
{
  return ESP.getCycleCount();
}

uint32_t freeHeap()
{
  return ESP.getFreeHeap();
}

uint32_t stackHighWater()
{
  return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t);
}


// ---------- Bench ----------


BenchResult bench(MicroParam **values, size_t count)
{
  BenchResult r;

  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    for (size_t v = 0; v < count; ++v) {
      values[v]->setInt(i);
      sink_i = values[v]->getInt();

      values[v]->setFloat((float)i * 0.001f);
      sink_f = values[v]->getFloat();
    }
  }

  r.cycles = cycles() - start;

  uint32_t heapAfter  = freeHeap();
  uint32_t stackAfter = stackHighWater();

  r.heap  = freeHeap();
  r.stack = stackHighWater();

  return r;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  bench(values, VALUE_COUNT); // warm-up
}

void loop()
{
  delay(5000);

  BenchResult r = bench(values, VALUE_COUNT);

  Serial.println(__FILE__);
  Serial.print("Cycles: "); Serial.println(r.cycles);
  Serial.print("Heap : "); Serial.println(r.heap);
  Serial.print("Stack : "); Serial.println(r.stack);
  Serial.print("sizeof(MicroParamInt): "); Serial.println(sizeof(MicroParamInt));
  Serial.println();
}
