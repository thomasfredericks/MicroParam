#include <Arduino.h>
#include <MicroCommon.h>

// ============================================================================================

struct Value {
  enum Type : uint8_t { INT, FLOAT, ENUM } type;

  union {
    uint32_t i;
    float f;
    int32_t e;
  } value;

  union {
    struct {
      uint32_t min, max;
    } i;
    struct {
      float min, max;
    } f;
    struct {
      int32_t count;
    } e;
  } meta;
};

// ======================= Values =======================

Value values[] = {
    // Ints
    {Value::INT, {.i = 5}, {.i = {0, 127}}},
    {Value::INT, {.i = 10}, {.i = {0, 127}}},

    // Floats
    {Value::FLOAT, {.f = 0.5f}, {.f = {0.0f, 1.0f}}},
    {Value::FLOAT, {.f = 0.25f}, {.f = {0.0f, 1.0f}}},

    // Enums
    {Value::ENUM, {.e = 0}, {.e = {3}}},
    {Value::ENUM, {.e = 1}, {.e = {3}}},
};

constexpr size_t VALUE_COUNT = sizeof(values) / sizeof(values[0]);

// ============================================================================================

#define ITERATIONS 200000

struct BenchResult {
  uint32_t cycles;
  uint32_t heap;
  uint32_t stack;
};

volatile int32_t sink_i;
volatile float sink_f;

static inline uint32_t cycles() { return ESP.getCycleCount(); }

uint32_t freeHeap() { return ESP.getFreeHeap(); }

uint32_t stackHighWater() {
  return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t);
}

// ---------- Bench ----------

BenchResult bench(Value *values, size_t count) {
  BenchResult r;

  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    for (size_t v = 0; v < count; ++v) {
      Value &val = values[v];

      switch (val.type) {
      case Value::INT:
        val.value.i = Micro::clamp<uint32_t>(i, val.meta.i.min, val.meta.i.max);
        sink_i = val.value.i;
        val.value.f = (float)val.value.i * 0.001f;
        sink_f = val.value.f;
        break;

      case Value::FLOAT: {
        float f = (float)i * 0.001f;
        val.value.f = Micro::clamp<float>(f, val.meta.f.min, val.meta.f.max);
        sink_f = val.value.f;
        val.value.i = (uint32_t)floor(val.value.f);
        sink_i = val.value.i;
        break;
      }

      case Value::ENUM:
        val.value.e = Micro::modulo(i, val.meta.e.count);
        sink_i = val.value.e;
        val.value.f = (float)val.value.e;
        sink_f = val.value.f;
        break;
      }
    }
  }

  r.cycles = cycles() - start;

  uint32_t heapAfter = freeHeap();
  uint32_t stackAfter = stackHighWater();

  r.heap = freeHeap();
  r.stack = stackHighWater();

  return r;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  bench(values, VALUE_COUNT); // warm-up
}

void loop() {
  delay(5000);

  BenchResult r = bench(values, VALUE_COUNT);

  Serial.println(__FILE__);
  Serial.print("Cycles: ");
  Serial.println(r.cycles);
  Serial.print("Heap : ");
  Serial.println(r.heap);
  Serial.print("Stack : ");
  Serial.println(r.stack);
  Serial.println();
}
