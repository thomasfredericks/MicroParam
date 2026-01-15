#include <Arduino.h>
#include <MicroCommon.h>


// ============================================================================================

// ======================================================
// Param declaration macros (ONE param per call)
// ======================================================

// ---------- INT ----------
#define PARAM_INT(name, min_, max_)        \
  uint32_t name;                           \
  const uint32_t name##_MIN = (min_);      \
  const uint32_t name##_MAX = (max_);

// ---------- FLOAT ----------
#define PARAM_FLOAT(name, min_, max_)      \
  float name;                              \
  const float name##_MIN = (min_);         \
  const float name##_MAX = (max_);

// ---------- ENUM ----------
#define PARAM_ENUM(name, count_, labels_)  \
  int32_t name;                            \
  const int32_t name##_COUNT = (count_);   \
  const char **name##_LABELS = (labels_);

// ======================================================
// Param set macros (hot-path safe)
// ======================================================

#define PARAM_SET_INT(name, v) \
  do { \
    (name) = Micro::clamp<uint32_t>((v), name##_MIN, name##_MAX); \
  } while (0)

#define PARAM_SET_FLOAT(name, v) \
  do { \
    (name) = Micro::clamp<float>((v), name##_MIN, name##_MAX); \
  } while (0)

#define PARAM_SET_ENUM(name, v) \
  do { \
    (name) = Micro::modulo((v), name##_COUNT); \
  } while (0)


// Ints
const char* labels[3] = { "A", "B", "C" };

PARAM_INT  (vInt1,   0, 127)
PARAM_INT  (vInt2,   0, 127)

PARAM_FLOAT(vFloat1, 0.0f, 1.0f)
PARAM_FLOAT(vFloat2, 0.0f, 1.0f)

PARAM_ENUM (vEnum1,  3, labels)
PARAM_ENUM (vEnum2,  3, labels)

// Number of values per type
constexpr size_t VALUE_INT = 2;
constexpr size_t VALUE_FLOAT = 2;
constexpr size_t VALUE_ENUM = 2;





// ---------------------- Benchmark ----------------------


static inline uint32_t cycles() { return ESP.getCycleCount(); }
uint32_t freeHeap() { return ESP.getFreeHeap(); }
uint32_t stackHighWater() {
  return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t);
}

#define ITERATIONS 200000

volatile int32_t sink_i;
volatile float sink_f;

/* // READ FROM COMMON ARRAY AND ASSIGN INT (TO FLOAT OR ENUM)
uint32_t benchCommonArray() {


  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    for (size_t p = 0; p < PARAM_COUNT; ++p) {
      MicroParam &param = params[p];

      param.setInt(i);
      sink_i = param.getInt();

    }
  }


  return cycles() - start;
}
 */
// READ FASTEST POSSIBLE FROM VARIABLE
uint32_t benchDirectUse() {

  uint32_t start = cycles();

  for (int i = 0; i < ITERATIONS; ++i) {
    float f = i;


    PARAM_SET_INT(vInt1,i);
    sink_i = vInt1;

    PARAM_SET_INT(vInt2,i);
    sink_i = vInt2;

    PARAM_SET_FLOAT(vFloat1,i);
    sink_f = vFloat2;

    PARAM_SET_FLOAT(vFloat2,i);
    sink_f = vFloat2;

    PARAM_SET_ENUM(vEnum1,i);
    sink_i = vEnum1;

    PARAM_SET_ENUM(vEnum2,i);
    sink_i = vEnum2;

   
  
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
/*   Serial.print(F("Common Array Cycles: "));
  Serial.println(benchCommonArray()); */
  Serial.print(F("Direct Use: ")); 
  Serial.println(benchDirectUse());
  Serial.print(F("Heap : "));
  Serial.println(freeHeap());
  Serial.print(F("Stack : "));
  Serial.println(stackHighWater());
  Serial.println();
}
