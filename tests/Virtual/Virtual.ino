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
    value_ = microClamp<int32_t>(i, min_, max_);
  }

  int32_t getInt() const override { return value_; }

  void setFloat(float f) override
  {
    setInt(floor(f));
  }

  float getFloat() const override { return (float)value_; }

  void mapFloat(float in, float inMin, float inMax) override
  {
    float mapped = microMap<float>(in, inMin, inMax, min_, max_);
    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(microMap<int32_t>(in, inMin, inMax, min_, max_));
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
    value_ = microClamp<float>(f, min_, max_);
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
    float mapped = microMap<float>(in, inMin, inMax, min_, max_);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
  {
    setInt(microMap<int32_t>(in, inMin, inMax, min_, max_));
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
    value_ = microModulo(i, count_);
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
    float mapped = microMap<float>(in, inMin, inMax, 0, count_ - 1);

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, 0, count_ - 1));
  }
};

// ============================================================================================

const char* labels[3] = { "A", "B", "C" };
MicroParamInt   i1(5, 0, 127);
MicroParamFloat f1(0.5f, 0.0f, 1.0f);
MicroParamEnum e1(labels, 0, 3);

MicroParamInt   i2(5, 0, 127);
MicroParamFloat f2(0.5f, 0.0f, 1.0f);
MicroParamEnum e2(labels, 0, 3);


MicroParam *params[] = { &i1, &i2, &f1 ,&f2, &e1, &e2 };
constexpr size_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);

// ======================= Benchmark =======================
#define ITERATIONS 200000

volatile int32_t sink_i;
volatile float sink_f;


static inline uint32_t cycles() { return ESP.getCycleCount(); }
uint32_t freeHeap() { return ESP.getFreeHeap(); }
uint32_t stackHighWater() {
  return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t);
}

// ---------------------- Benchmark ----------------------
// READ FROM COMMON ARRAY AND ASSIGN INT (TO FLOAT OR ENUM)
uint32_t benchCommonArray() {


  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    for (size_t p = 0; p < PARAM_COUNT; ++p) {
      MicroParam * param = params[p];

      param->setInt(i);
      sink_i = param->getInt();

    }
  }


  return cycles() - start;
}

// READ FASTEST POSSIBLE FROM VARIABLE
uint32_t benchDirectUse() {

  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    float f = i;
   
     i1.setInt(i);
    sink_i = i1.getInt();

    i2.setInt(i);
    sink_i = i2.getInt();

    f1.setFloat(f);
    sink_f = f1.getFloat();

    f2.setFloat(f);
    sink_f = f2.getFloat();

    e1.setInt(i);
    sink_i = e1.getInt();

    e2.setInt(i);
    sink_i = e2.getInt(); 
    
  }


  return cycles() - start;
}

// ======================= Setup / Loop =======================
void setup() {
  Serial.begin(115200);
  delay(1000);

}

void loop() {
  delay(5000);

  Serial.println(F(__FILE__));
  Serial.print(F("Common Array Cycles: "));
  Serial.println(benchCommonArray());
  Serial.print(F("Direct Use: ")); 
  Serial.println(benchDirectUse());
  Serial.print(F("Heap : "));
  Serial.println(freeHeap());
  Serial.print(F("Stack : "));
  Serial.println(stackHighWater());
  Serial.println();
}
