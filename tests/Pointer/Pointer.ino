
#include <Arduino.h>
#include <MicroCommon.h>

class MicroValueNB
{
public:
  enum Type : uint8_t { INT, FLOAT, ENUM };

private:
  Type type_;

  union {
    int32_t i;
    float   f;
  } value_;

  union {
    struct { int32_t min, max; } i;
    struct { float min, max; } f;
    struct { const char **labels; int32_t count; } e;
  } meta_;

  // ---- Function pointers (hot path) ----
  void     (*setInt_)(MicroValueNB *, int32_t);
  int32_t  (*getInt_)(const MicroValueNB *);
  void     (*setFloat_)(MicroValueNB *, float);
  float    (*getFloat_)(const MicroValueNB *);

public:
  /* ---------- Int implementation ---------- */

  static void setInt_Int(MicroValueNB *v, int32_t i)
  {
    v->value_.i = Micro::clamp<int32_t>(i, v->meta_.i.min, v->meta_.i.max);
  }

  static int32_t getInt_Int(const MicroValueNB *v)
  {
    return v->value_.i;
  }

  static void setFloat_Int(MicroValueNB *v, float f)
  {
    setInt_Int(v, (int32_t)floor(f));
  }

  static float getFloat_Int(const MicroValueNB *v)
  {
    return (float)v->value_.i;
  }

  /* ---------- Float implementation ---------- */

  static void setFloat_Float(MicroValueNB *v, float f)
  {
    v->value_.f = Micro::clamp<float>(f, v->meta_.f.min, v->meta_.f.max);
  }

  static float getFloat_Float(const MicroValueNB *v)
  {
    return v->value_.f;
  }

  static void setInt_Float(MicroValueNB *v, int32_t i)
  {
    setFloat_Float(v, (float)i);
  }

  static int32_t getInt_Float(const MicroValueNB *v)
  {
    return (int32_t)floor(v->value_.f);
  }

  /* ---------- Enum implementation ---------- */

  static void setInt_Enum(MicroValueNB *v, int32_t i)
  {
    v->value_.i = Micro::modulo(i, v->meta_.e.count);
  }

  static int32_t getInt_Enum(const MicroValueNB *v)
  {
    return v->value_.i;
  }

  static void setFloat_Enum(MicroValueNB *v, float f)
  {
    setInt_Enum(v, (int32_t)floor(f));
  }

  static float getFloat_Enum(const MicroValueNB *v)
  {
    return (float)v->value_.i;
  }

  /* ---------- Constructors ---------- */

  static MicroValueNB Int(int32_t initial, int32_t min, int32_t max)
  {
    MicroValueNB v;
    v.type_ = INT;
    v.meta_.i = { min, max };

    v.setInt_   = setInt_Int;
    v.getInt_   = getInt_Int;
    v.setFloat_ = setFloat_Int;
    v.getFloat_ = getFloat_Int;

    v.setInt(initial);
    return v;
  }

  static MicroValueNB Float(float initial, float min, float max)
  {
    MicroValueNB v;
    v.type_ = FLOAT;
    v.meta_.f = { min, max };

    v.setInt_   = setInt_Float;
    v.getInt_   = getInt_Float;
    v.setFloat_ = setFloat_Float;
    v.getFloat_ = getFloat_Float;

    v.setFloat(initial);
    return v;
  }

  static MicroValueNB Enum(const char **labels, int32_t initial, int32_t count)
  {
    MicroValueNB v;
    v.type_ = ENUM;
    v.meta_.e = { labels, count };

    v.setInt_   = setInt_Enum;
    v.getInt_   = getInt_Enum;
    v.setFloat_ = setFloat_Enum;
    v.getFloat_ = getFloat_Enum;

    v.setInt(initial);
    return v;
  }

  /* ---------- Public API (NO BRANCH) ---------- */

  inline void setInt(int32_t i)       { setInt_(this, i); }
  inline int32_t getInt() const       { return getInt_(this); }
  inline void setFloat(float f)       { setFloat_(this, f); }
  inline float getFloat() const       { return getFloat_(this); }
};

// =================================================================================

const char* labels[3] = { "A", "B", "C" };
MicroValueNB vInt   = MicroValueNB::Int(5, 0, 127);
MicroValueNB vFloat = MicroValueNB::Float(0.5f, 0.0f, 1.0f);
MicroValueNB vEnum  = MicroValueNB::Enum(labels, 0, 3);
MicroValueNB vInt1   = MicroValueNB::Int(5, 0, 127);
MicroValueNB vFloat1 = MicroValueNB::Float(0.5f, 0.0f, 1.0f);
MicroValueNB vEnum1  = MicroValueNB::Enum(labels, 0, 3);

MicroValueNB *values[] = { &vInt, &vFloat , &vEnum, &vInt1, &vFloat1 , &vEnum1  };
constexpr size_t VALUE_COUNT = sizeof(values) / sizeof(values[0]);

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


BenchResult bench(MicroValueNB **values, size_t count)
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
  Serial.print("sizeof(MicroValueNB): "); Serial.println(sizeof(MicroValueNB));
  Serial.println();
}