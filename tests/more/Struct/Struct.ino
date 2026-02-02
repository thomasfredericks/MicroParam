#include <Arduino.h>
#include <MicroCommon.h>




// ======================= Base types =======================
struct MicroParamInt {
    int32_t value;
    int32_t min, max;
    void set(int32_t v) { value = microClamp(v, min, max); }
    int32_t get() { return value};
};

struct MicroParamFloat {
    float value;
    float min, max;
    void set(float v) { value = microClamp(v, min, max); }
    float get() { return value};
};

struct MicroParamEnum {
    int32_t value;
    int32_t count;
    char ** labels;
    void set(int32_t v) { value = microModulo(v, count); }
    int32_t get() { return value};
};

// ======================= MicroParam =======================
struct MicroParam {
    enum Type : uint8_t { INT, FLOAT, ENUM } type;
    const char *key;

    union {
        MicroParamInt   *i;
        MicroParamFloat *f;
        MicroParamEnum  *e;
    } ptr;

    // Overloaded bind constructors
    static MicroParam bind(const char *k, MicroParamInt &v) {
        MicroParam p;
        p.type = INT;
        p.key  = k;
        p.ptr.i = &v;
        return p;
    }

    static MicroParam bind(const char *k, MicroParamFloat &v) {
        MicroParam p;
        p.type = FLOAT;
        p.key  = k;
        p.ptr.f = &v;
        return p;
    }

    static MicroParam bind(const char *k, MicroParamEnum &v) {
        MicroParam p;
        p.type = ENUM;
        p.key  = k;
        p.ptr.e = &v;
        return p;
    }

    // Accessors
    void setInt(int32_t v) {
        switch(type) {
            case INT:   ptr.i->set(v); break;
            case FLOAT: ptr.f->set((float)v); break;
            case ENUM:  ptr.e->set(v); break;
        }
    }

    int32_t getInt() const {
        switch(type) {
            case INT:   return ptr.i->value;
            case FLOAT: return (int32_t)ptr.f->value;
            case ENUM:  return ptr.e->value;
        }
        return 0;
    }

    void setFloat(float v) {
        switch(type) {
            case FLOAT: ptr.f->set(v); break;
            case INT:   ptr.i->set((int32_t)floor(v)); break;
            case ENUM:  ptr.e->set((int32_t)floor(v)); break;
        }
    }

    float getFloat() const {
        switch(type) {
            case FLOAT: return ptr.f->value;
            case INT:   return (float)ptr.i->value;
            case ENUM:  return (float)ptr.e->value;
        }
        return 0.0f;
    }
};

// ---------------------- Values ----------------------
MicroParamInt   i1{5, 0, 127};
MicroParamInt   i2{10, 0, 127};
MicroParamFloat f1{0.5f, 0.0f, 1.0f};
MicroParamFloat f2{0.25f, 0.0f, 1.0f};
MicroParamEnum  e1{0, 3};
MicroParamEnum  e2{1, 3};

// Array of MicroParam bindings
MicroParam values[] = {
    MicroParam::bind("i1", i1),
    MicroParam::bind("i2", i2),
    MicroParam::bind("f1", f1),
    MicroParam::bind("f2", f2),
    MicroParam::bind("e1", e1),
    MicroParam::bind("e2", e2)
};

constexpr size_t VALUE_COUNT = sizeof(values)/sizeof(values[0]);


// ======================= Benchmark =======================

#define ITERATIONS 200000

volatile int32_t sink_i;
volatile float   sink_f;
volatile int32_t sinke_e;

struct BenchResult {
    uint32_t cycles;
    uint32_t heap;
    uint32_t stack;
};

static inline uint32_t cycles() { return ESP.getCycleCount(); }
uint32_t freeHeap() { return ESP.getFreeHeap(); }
uint32_t stackHighWater() { return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t); }



// ---------------------- Benchmark ----------------------
// READ FROM COMMON ARRAY AND ASSIGN INT (TO FLOAT OR ENUM)
uint32_t benchCommonArray() {


    uint32_t start = cycles();

    for(int i=0; i<ITERATIONS; ++i) {
        for(size_t v=0; v<VALUE_COUNT; ++v) {
            MicroParam &param = values[v];

            param.setInt(i);
            sink_i = param.getInt();

        }
    }

    return cycles() - start;
}

// READ FROM COMMON ARRAY AND ASSIGN INT (TO FLOAT OR ENUM)
uint32_t benchDirectUse() {


    uint32_t start = cycles();

    for(int i=0; i<ITERATIONS; ++i) {
      i1.set(i);
      sink_i = i1.

        MicroParamInt   i1{5, 0, 127};
MicroParamInt   i2{10, 0, 127};
MicroParamFloat f1{0.5f, 0.0f, 1.0f};
MicroParamFloat f2{0.25f, 0.0f, 1.0f};
MicroParamEnum  e1{0, 3};
MicroParamEnum  e2{1, 3};
    }

    return cycles() - start;
}

// ======================= Setup / Loop =======================
void setup() {
    Serial.begin(115200);
    delay(1000);

    bench(); // warm-up
}

void loop() {
    delay(5000);

    Serial.println(F(__FILE__));
    Serial.print(F("Common Array: ")); Serial.println(benchCommonArray());
    Serial.print(F("Direct Use: ")); Serial.println(benchDirectUse());
    Serial.print(F("Heap: ")); Serial.println(freeHeap());
    Serial.print(F("Stack: ")); Serial.println(stackHighWater());
    Serial.println();
}
