
#include <Arduino.h>
#include <MicroCommon.h>



class MicroValue
{
public:
  enum Type : uint8_t {
    INT,
    FLOAT,
    ENUM
  };

private:
  Type type_;

  union {
    int32_t i;
    float   f;
  } value_;

  union {
    struct {
      int32_t min;
      int32_t max;
    } i;

    struct {
      float min;
      float max;
    } f;

    struct {
      const char **labels;
      int32_t count;
    } e;
  } meta_;

public:
  /* ---------- Constructors ---------- */

  static MicroValue Int(int32_t initial, int32_t min, int32_t max)
  {
    MicroValue v;
    v.type_ = INT;
    v.meta_.i.min = min;
    v.meta_.i.max = max;
    v.setInt(initial);
    return v;
  }

  static MicroValue Float(float initial, float min, float max)
  {
    MicroValue v;
    v.type_ = FLOAT;
    v.meta_.f.min = min;
    v.meta_.f.max = max;
    v.setFloat(initial);
    return v;
  }

  static MicroValue Enum(const char **labels, int32_t initial, int32_t count)
  {
    MicroValue v;
    v.type_ = ENUM;
    v.meta_.e.labels = labels;
    v.meta_.e.count  = count;
    v.setInt(initial);
    return v;
  }

  /* ---------- Type ---------- */

  Type getType() const { return type_; }

  char getTypeChar() const
  {
    switch (type_) {
      case INT:   return 'i';
      case FLOAT: return 'f';
      case ENUM:  return 'e';
      default:    return '?';
    }
  }

  /* ---------- Int API ---------- */

  void setInt(int32_t v)
  {
    switch (type_) {
      case INT:
        value_.i = Micro::clamp<int32_t>(v, meta_.i.min, meta_.i.max);
        break;

      case FLOAT:
        setFloat((float)v);
        break;

      case ENUM:
        value_.i = Micro::modulo(v, meta_.e.count);
        break;
    }
  }

  int32_t getInt() const
  {
    switch (type_) {
      case INT:   return value_.i;
      case FLOAT: return (int32_t)floor(value_.f);
      case ENUM:  return value_.i;
    }
    return 0;
  }

  /* ---------- Float API ---------- */

  void setFloat(float v)
  {
    switch (type_) {
      case FLOAT:
        value_.f = Micro::clamp<float>(v, meta_.f.min, meta_.f.max);
        break;

      case INT:
        setInt((int32_t)floor(v));
        break;

      case ENUM:
        setInt((int32_t)floor(v));
        break;
    }
  }

  float getFloat() const
  {
    switch (type_) {
      case FLOAT: return value_.f;
      case INT:   return (float)value_.i;
      case ENUM:  return (float)value_.i;
    }
    return 0.0f;
  }

  /* ---------- Mapping ---------- */

  void mapFloat(float in, float inMin, float inMax)
  {
    switch (type_) {
      case INT: {
        float mapped = Micro::map<float>(in, inMin, inMax,
                                         meta_.i.min, meta_.i.max);
        setFloat(mapped);
        break;
      }

      case FLOAT: {
        float mapped = Micro::map<float>(in, inMin, inMax,
                                         meta_.f.min, meta_.f.max);
        setFloat(mapped);
        break;
      }

      case ENUM: {
        float mapped = Micro::map<float>(in, inMin, inMax,
                                         0, meta_.e.count - 1);
        setFloat(mapped);
        break;
      }
    }
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMax)
  {
    switch (type_) {
      case INT:
        setInt(Micro::map<int32_t>(in, inMin, inMax,
                                   meta_.i.min, meta_.i.max));
        break;

      case FLOAT:
        setFloat((float)Micro::map<int32_t>(in, inMin, inMax,
                                            meta_.f.min, meta_.f.max));
        break;

      case ENUM:
        setInt(Micro::map<int32_t>(in, inMin, inMax,
                                   0, meta_.e.count - 1));
        break;
    }
  }

};




// ============================================================================================
const char* labels[3] = { "A", "B", "C" };
MicroValue vInt   = MicroValue::Int(5, 0, 127);
MicroValue vFloat = MicroValue::Float(0.5f, 0.0f, 1.0f);
MicroValue vEnum  = MicroValue::Enum(labels, 0, 3);

MicroValue vInt1   = MicroValue::Int(5, 0, 127);
MicroValue vFloat1 = MicroValue::Float(0.5f, 0.0f, 1.0f);
MicroValue vEnum1  = MicroValue::Enum(labels, 0, 3);

MicroValue *values[] = { &vInt, &vFloat, &vEnum, &vInt1, &vFloat1, &vEnum1 };
constexpr size_t VALUE_COUNT = sizeof(values) / sizeof(values[0]);

// ============================================================================================

// =================================================================================

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


BenchResult bench(MicroValue **values, size_t count)
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
  Serial.print("sizeof(MicroValue): "); Serial.println(sizeof(MicroValue));
  Serial.println();
}
