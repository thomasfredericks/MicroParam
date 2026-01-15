#include <Arduino.h>
#include <MicroCommon.h>

// ============================================================================================

// Ints
uint32_t vInt[2] = {5, 10};
const uint32_t vIntMin[2] = {0, 0};
const uint32_t vIntMax[2] = {127, 127};

// Floats
float vFloat[2] = {0.5f, 0.25f};
const float vFloatMin[2] = {0.0f, 0.0f};
const float vFloatMax[2] = {1.0f, 1.0f};

// Enums (as int32_t)
int32_t vEnum[2] = {0, 1};
const int32_t vEnumCount[2] = {3, 3};

// Number of values per type
constexpr size_t VALUE_INT = 2;
constexpr size_t VALUE_FLOAT = 2;
constexpr size_t VALUE_ENUM = 2;

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

BenchResult bench() {
  BenchResult r;

  uint32_t heapBefore = freeHeap();
  uint32_t stackBefore = stackHighWater();
  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    // ----- Ints -----
    for (size_t v = 0; v < VALUE_INT; ++v) {
      vInt[v] = Micro::clamp<uint32_t>(i, vIntMin[v], vIntMax[v]);
      sink_i = vInt[v];

      vFloat[v] = (float)vInt[v] * 0.001f;
      sink_f = vFloat[v];
    }

    // ----- Floats -----
    for (size_t v = 0; v < VALUE_FLOAT; ++v) {
      float f = (float)i * 0.001f;
      vFloat[v] = Micro::clamp<float>(f, vFloatMin[v], vFloatMax[v]);
      sink_f = vFloat[v];

      vInt[v] = (uint32_t)floor(vFloat[v]);
      sink_i = vInt[v];
    }

    // ----- Enums -----
    for (size_t v = 0; v < VALUE_ENUM; ++v) {
      vEnum[v] = Micro::modulo(i, vEnumCount[v]);
      sink_i = vEnum[v];

      vFloat[v] = (float)vEnum[v];
      sink_f = vFloat[v];
    }
  }

  uint32_t heapAfter = freeHeap();
  uint32_t stackAfter = stackHighWater();

  r.cycles = cycles() - start;
  r.heap = freeHeap();
  r.stack = stackHighWater();

  return r;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  bench(); // warm-up
}

void loop() {
  delay(5000);

  BenchResult r = bench();

  Serial.println(__FILE__);
  Serial.print("Cycles: ");
  Serial.println(r.cycles);
  Serial.print("Heap : ");
  Serial.println(r.heap);
  Serial.print("Stack : ");
  Serial.println(r.stack);
  Serial.println();
}
