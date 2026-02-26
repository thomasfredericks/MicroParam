#include <Arduino.h>
#include <MicroParam.h>


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
