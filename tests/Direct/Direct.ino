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
const char* labels[3] = { "A", "B", "C" };
int32_t vEnum[2] = {0, 1};
const int32_t vEnumCount[2] = {3, 3};

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
    vInt[0] = Micro::clamp<uint32_t>(i,vIntMin[0],vIntMax[0]);
    sink_i = vInt[0];

    vInt[1] = Micro::clamp<uint32_t>(i,vIntMin[1],vIntMax[1]);
    sink_i = vInt[1];

    vFloat[0] = Micro::clamp<float>(i,vFloatMin[0],vFloatMax[0]);
    sink_f = vFloat[0];

    vFloat[1] = Micro::clamp<float>(i,vFloatMin[1],vFloatMax[1]);
    sink_f = vFloat[1];

    vEnum[0] =  Micro::modulo(i, vEnumCount[0]);
    sink_i = vEnum[0];

    vEnum[1] = Micro::modulo(i, vEnumCount[1]);
    sink_i = vEnum[1];
  
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
