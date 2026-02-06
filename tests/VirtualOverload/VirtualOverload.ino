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

  char getType()
  {
    return type_;
  }
  virtual void setInt(int32_t v) = 0;
  virtual int32_t getInt() const = 0;

  virtual void setFloat(float v) = 0;
  virtual float getFloat() const = 0;

  virtual void mapFloat(float in, float inMin, float inMax) = 0;
  virtual void mapInt(int32_t in, int32_t inMin, int32_t inMax) = 0;
};


class MicroParamInt  : public MicroParam
{
private:
    int32_t value_;
    const int32_t min_;
    const int32_t max_;

public:
    // Assignment operators
    MicroParamInt& operator=(int32_t v) { value_ = microClamp<int32_t>(v, min_, max_); return *this; }
    MicroParamInt& operator=(int v)       { value_ = microClamp<int32_t>((int32_t)v, min_, max_); return *this; }
    MicroParamInt& operator=(float v)     { value_ = microClamp<int32_t>((int32_t)floor(v), min_, max_); return *this; }
    MicroParamInt& operator=(const MicroParamInt& other) { value_ = other.value_; return *this; }

    // Implicit conversion
    operator int32_t() const { return value_; }
    operator float() const { return (float)value_; }

    // Virtual API (still works if needed)
    void setInt(int32_t i) override { value_ = microClamp<int32_t>(i, min_, max_); }
    int32_t getInt() const override { return value_; }

    void setFloat(float f) override { value_ = microClamp<int32_t>((int32_t)floor(f), min_, max_); }
    float getFloat() const override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microClamp<int32_t>((int32_t)floor(microMap<float>(in, inMin, inMax, min_, max_)), min_, max_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
    {
        value_ = microClamp<int32_t>(microMap<int32_t>(in, inMin, inMax, min_, max_), min_, max_);
    }

    MicroParamInt(int32_t initial, int32_t min, int32_t max)
        : MicroParam('i'), min_(min), max_(max)
    {
        value_ = microClamp<int32_t>(initial, min_, max_);
    }
};

class MicroParamFloat  : public MicroParam
{
private:
    float value_;
    const float min_;
    const float max_;

public:
    // Assignment operators
    MicroParamFloat& operator=(float v)     { value_ = microClamp<float>(v, min_, max_); return *this; }
    MicroParamFloat& operator=(int32_t v)   { value_ = microClamp<float>((float)v, min_, max_); return *this; }
    MicroParamFloat& operator=(int v)       { value_ = microClamp<float>((float)v, min_, max_); return *this; }
    MicroParamFloat& operator=(const MicroParamFloat& other) { value_ = other.value_; return *this; }

    // Implicit conversion
    operator float() const { return value_; }
   operator int32_t() const { return (int32_t)floor(value_); }

    // Virtual API
    void setFloat(float f) override { value_ = microClamp<float>(f, min_, max_); }
    float getFloat() const override { return value_; }

    void setInt(int32_t i) override { value_ = microClamp<float>((float)i, min_, max_); }
    int32_t getInt() const override { return (int32_t)floor(value_); }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microClamp<float>(microMap<float>(in, inMin, inMax, min_, max_), min_, max_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
    {
        value_ = microClamp<float>((float)microMap<int32_t>(in, inMin, inMax, (int32_t)min_, (int32_t)max_), min_, max_);
    }

    MicroParamFloat(float initial, float min, float max)
        : MicroParam('f'), min_(min), max_(max)
    {
        value_ = microClamp<float>(initial, min_, max_);
    }
};
class MicroParamEnum  : public MicroParam
{
private:
    int32_t value_;
    const char **labels_;
    int32_t count_;

public:
    // Assignment operators
    MicroParamEnum& operator=(int32_t v) { value_ = microModulo(v, count_); return *this; }
  MicroParamEnum& operator=(int v)     { value_ = microModulo((int32_t)v, count_); return *this; }
  MicroParamEnum& operator=(float v)   { value_ = microModulo((int32_t)floor(v), count_); return *this; }
   MicroParamEnum& operator=(const MicroParamEnum& other) { value_ = other.value_; return *this; }

    // Implicit conversion
    operator int32_t() const { return value_; }
   operator float() const { return (float)value_; }

    // Virtual API
    void setInt(int32_t i) override { value_ = microModulo(i, count_); }
    int32_t getInt() const override { return value_; }

    void setFloat(float f) override { value_ = microModulo((int32_t)floor(f), count_); }
    float getFloat() const override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microModulo((int32_t)floor(microMap<float>(in, inMin, inMax, 0, count_-1)), count_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
    {
        value_ = map(in, inMin, inMaxExclusive, 0, count_-1);
    }

    MicroParamEnum(const char **labels, int32_t value, int32_t count)
        : MicroParam('e'), labels_(labels), count_(count)
    {
        value_ = microModulo(value, count_);
    }
};



// ============================================================================================

const char *labels[3] = {"A", "B", "C"};
MicroParamInt i1(5, 0, 127);
MicroParamFloat f1(0.5f, 0.0f, 1.0f);
MicroParamEnum e1(labels, 0, 3);

MicroParamInt i2(5, 0, 127);
MicroParamFloat f2(0.5f, 0.0f, 1.0f);
MicroParamEnum e2(labels, 0, 3);

MicroParam *params[] = {&i1, &i2, &f1, &f2, &e1, &e2};
constexpr size_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);

// ======================= Benchmark =======================
#define ITERATIONS 200000

volatile int32_t sink_i;
volatile float sink_f;

static inline uint32_t cycles() { return ESP.getCycleCount(); }
uint32_t freeHeap() { return ESP.getFreeHeap(); }
uint32_t stackHighWater()
{
  return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t);
}

// ---------------------- Benchmark ----------------------
// READ FROM COMMON ARRAY AND ASSIGN INT (TO FLOAT OR ENUM)
uint32_t benchCommonArray()
{

  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i)
  {
    for (size_t p = 0; p < PARAM_COUNT; ++p)
    {
      MicroParam *param = params[p];

      param->setInt(i);
      sink_i = param->getInt();
    }
  }

  return cycles() - start;
}

// READ FASTEST POSSIBLE FROM VARIABLE
uint32_t benchDirectUse()
{

  uint32_t start = cycles();

    for (int i = 0; i < ITERATIONS; ++i) {
    float f = i;
   
     i1 = i; 
    sink_i = i1;

    i2 = i;
    sink_i = i2;

    f1 = f;
    sink_f = f1;

    f2 = f;
    sink_f = f2;

    e1 = i;
    sink_i = e1;

    e2 = 1;
    sink_i = e2; 
    
  }

  return cycles() - start;
}

// ======================= Setup / Loop =======================
void setup()
{
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
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
