#include <Arduino.h>
#include <MicroCommon.h>


// ======================= Base types =======================
struct MicroParamInt {
  int32_t value;
  const int32_t min, max;

  // Constructor
  constexpr MicroParamInt(int32_t v, int32_t min_, int32_t max_)
    : value(v), min(min_), max(max_)
  {
    value = microClamp(value, min, max);
  }

  // Assignment from raw value → calls set()
  MicroParamInt& operator=(int32_t v) {
    value = microClamp(v, min, max);
    return *this;
  }

  // Implicit conversion to int32_t → calls get()
  operator int32_t() const {
    return value;
  }

  // Optional explicit API
  void set(int32_t v) {
    value = microClamp(v, min, max);
  }

  int32_t get() const {
    return value;
  }
};


struct MicroParamFloat {
  float value;
  const float min, max;

  constexpr MicroParamFloat(float v, float min_, float max_)
    : value(v), min(min_), max(max_)
  {
    value = microClamp(value, min, max);
  }

  MicroParamFloat& operator=(float v) {
    value = microClamp(v, min, max);
    return *this;
  }

  operator float() const {
    return value;
  }

  void set(float v) {
    value = microClamp(v, min, max);
  }

  float get() const {
    return value;
  }
};


struct MicroParamEnum {
  int32_t value;
  const int32_t count;
  const char **labels;

  // Constructor
  constexpr MicroParamEnum(int32_t v, int32_t count_, const char **labels_)
    : value(v), count(count_), labels(labels_)
  {
    value = microModulo(value, count);
  }

  // Assignment operator → sets the value with modulo
  MicroParamEnum& operator=(int32_t v) {
    value = microModulo(v, count);
    return *this;
  }

  // Explicit set method
  void set(int32_t v) {
    value = microModulo(v, count);
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
    MicroParamInt *i;
    MicroParamFloat *f;
    MicroParamEnum *e;
  } ptr;

  // --------- Constructors ---------
  static MicroParam bind(const char *k, MicroParamInt &v) {
    MicroParam p;
    p.type = INT;
    p.key = k;
    p.ptr.i = &v;
    return p;
  }

  static MicroParam bind(const char *k, MicroParamFloat &v) {
    MicroParam p;
    p.type = FLOAT;
    p.key = k;
    p.ptr.f = &v;
    return p;
  }

  static MicroParam bind(const char *k, MicroParamEnum &v) {
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

// ---------------------- Values ----------------------
const char* labels[3] = { "A", "B", "C" };
MicroParamInt i1{5, 0, 127};
MicroParamInt i2{10, 0, 127};
MicroParamFloat f1{0.5f, 0.0f, 1.0f};
MicroParamFloat f2{0.25f, 0.0f, 1.0f};
MicroParamEnum e1{0, 3, labels};
MicroParamEnum e2{0, 3, labels};


MicroParam params[] = {MicroParam::bind("i1", i1), MicroParam::bind("i2", i2),
                       MicroParam::bind("f1", f1), MicroParam::bind("f2", f2),
                       MicroParam::bind("e1", e1), MicroParam::bind("e2", e2)};

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
      MicroParam * param = &params[p];

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
