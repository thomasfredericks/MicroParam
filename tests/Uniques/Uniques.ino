#include <Arduino.h>
#include <MicroCommon.h>

#define ITERATIONS 200000

volatile int32_t sink_i;
volatile float   sink_f;

// ======================= Base types =======================
struct MicroInt {
    int32_t value;
    int32_t min, max;
    void set(int32_t v) { value = Micro::clamp(v, min, max); }
};

struct MicroFloat {
    float value;
    float min, max;
    void set(float v) { value = Micro::clamp(v, min, max); }
};

struct MicroEnum {
    int32_t value;
    int32_t count;
    void set(int32_t v) { value = Micro::modulo(v, count); }
};

// ======================= MicroParam =======================
struct MicroParam {
    enum Type : uint8_t { INT, FLOAT, ENUM } type;
    const char *key;

    union {
        MicroInt  *i;
        MicroFloat* f;
        MicroEnum *e;
    } ptr;

    // --------- Constructors ---------
   static MicroParam bind(const char *k, MicroInt &v) {
        MicroParam p;
        p.type = INT;
        p.key = k;
        p.ptr.i = &v;
        return p;
    }

    static MicroParam bind(const char *k, MicroFloat &v) {
        MicroParam p;
        p.type = FLOAT;
        p.key = k;
        p.ptr.f = &v;
        return p;
    }

    static MicroParam bind(const char *k, MicroEnum &v) {
        MicroParam p;
        p.type = ENUM;
        p.key = k;
        p.ptr.e = &v;
        return p;
    }

    // --------- Access ---------
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

// ======================= Benchmark =======================
struct BenchResult {
    uint32_t cycles;
    uint32_t heap;
    uint32_t stack;
};

static inline uint32_t cycles() { return ESP.getCycleCount(); }
uint32_t freeHeap() { return ESP.getFreeHeap(); }
uint32_t stackHighWater() { return uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t); }

// ---------------------- Values ----------------------
MicroInt   i1{5, 0, 127};
MicroInt   i2{10, 0, 127};
MicroFloat f1{0.5f, 0.0f, 1.0f};
MicroFloat f2{0.25f, 0.0f, 1.0f};
MicroEnum  e1{0, 3};
MicroEnum  e2{1, 3};

MicroParam params[] = {
    MicroParam::bind("i1", i1),
    MicroParam::bind("i2", i2),
    MicroParam::bind("f1", f1),
    MicroParam::bind("f2", f2),
    MicroParam::bind("e1", e1),
    MicroParam::bind("e2", e2)
};

constexpr size_t PARAM_COUNT = sizeof(params)/sizeof(params[0]);

// ---------------------- Benchmark ----------------------
BenchResult bench() {
    BenchResult r;

    uint32_t heapBefore  = freeHeap();
    uint32_t stackBefore = stackHighWater();
    uint32_t start = cycles();

    for(int i=0;i<ITERATIONS;++i) {
        for(size_t p=0;p<PARAM_COUNT;++p) {
            MicroParam &param = params[p];

            param.setInt(i);
            sink_i = param.getInt();

            param.setFloat((float)i*0.001f);
            sink_f = param.getFloat();
        }
    }

    r.cycles = cycles() - start;
    r.heap   = heapBefore - freeHeap();
    r.stack  = stackBefore - stackHighWater();
    return r;
}

// ======================= Setup / Loop =======================
void setup() {
    Serial.begin(115200);
    delay(1000);

    bench(); // warm-up
}

void loop() {
    delay(5000);

    BenchResult r = bench();

    Serial.println(F(__FILE__));
    Serial.print(F("Cycles: ")); Serial.println(r.cycles);
    Serial.print(F("Heap delta: ")); Serial.println(r.heap);
    Serial.print(F("Stack delta: ")); Serial.println(r.stack);
    Serial.println();
}
