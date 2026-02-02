
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
    v->value_.i = microClamp<int32_t>(i, v->meta_.i.min, v->meta_.i.max);
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
    v->value_.f = microClamp<float>(f, v->meta_.f.min, v->meta_.f.max);
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
    v->value_.i = microModulo(i, v->meta_.e.count);
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
MicroValueNB i1   = MicroValueNB::Int(5, 0, 127);
MicroValueNB f1 = MicroValueNB::Float(0.5f, 0.0f, 1.0f);
MicroValueNB e1  = MicroValueNB::Enum(labels, 0, 3);
MicroValueNB i2   = MicroValueNB::Int(5, 0, 127);
MicroValueNB f2 = MicroValueNB::Float(0.5f, 0.0f, 1.0f);
MicroValueNB e2  = MicroValueNB::Enum(labels, 0, 3);

MicroValueNB *params[] = { &i1, &f1 , &e1, &i2, &f2 , &e2  };
constexpr size_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);

// =================================================================================

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
      MicroValueNB * param = params[p];

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
