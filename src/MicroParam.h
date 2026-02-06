
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>

// ======================= Base class =======================
class MicroParam
{
public:
    enum Type
    {
        INT = 'i',
        FLOAT = 'f',
        ENUM = 'e',
        STRING = 's',
        BYTE = 'y',
        BLOB = 'b'
    };

protected:
    const MicroParam::Type type_;

public:
    MicroParam(MicroParam::Type type) : type_(type)
    {
    }

    MicroParam::Type getType()
    {
        return type_;
    }
    virtual void setInt(int32_t v) = 0;
    virtual int32_t getInt() const = 0;

    virtual void setFloat(float v) = 0;
    virtual float getFloat() const = 0;

    virtual void mapFloat(float in, float inMin, float inMax) = 0;
    virtual void mapInt(int32_t in, int32_t inMin, int32_t inMax) = 0;
};

// ======================= Simple types =======================

class MicroParamByte : public MicroParam
{
private:
    uint8_t value_;
    const uint8_t min_;
    const uint8_t max_;

public:
    MicroParamByte(uint8_t v, uint8_t min = 0, uint8_t max = 255)
        : MicroParam(MicroParam::Type::BYTE), value_(v), min_(min), max_(max)
    {
        if (value_ < min_)
            value_ = min_;
        if (value_ > max_)
            value_ = max_;
    }

    // Assignment operators
    MicroParamByte &operator=(uint8_t v)
    {
        value_ = (v < min_) ? min_ : ((v > max_) ? max_ : v);
        return *this;
    }

    MicroParamByte &operator=(int v)
    {
        return *this = (uint8_t)v;
    }

    // Implicit conversion
    operator uint8_t() const { return value_; }

   
    // Explicit API
   // void set(uint8_t v) { *this = v; }
   // uint8_t get() const { return value_; }
};

class MicroParamInt : public MicroParam
{
private:
    int32_t value_;
    const int32_t min_;
    const int32_t max_;

public:
    // Assignment operators
    MicroParamInt &operator=(int32_t v)
    {
        value_ = microClamp<int32_t>(v, min_, max_);
        return *this;
    }
    MicroParamInt &operator=(int v)
    {
        value_ = microClamp<int32_t>((int32_t)v, min_, max_);
        return *this;
    }
    MicroParamInt &operator=(float v)
    {
        value_ = microClamp<int32_t>((int32_t)floor(v), min_, max_);
        return *this;
    }
    MicroParamInt &operator=(const MicroParamInt &other)
    {
        value_ = other.value_;
        return *this;
    }

    // Implicit conversion
    operator int32_t() const { return value_; }
    operator float() const { return (float)value_; }

    // Virtual API (still works if needed)
    void setInt(int32_t i) override { value_ = microClamp<int32_t>(i, min_, max_); }
    int32_t getInt() const override { return value_; }

    void setFloat(float f) override { value_ = microClamp<int32_t>((int32_t)floor(f), min_, max_); }
    float getFloat() const override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microClamp<int32_t>((int32_t)floor(microMap<float>(in, inMin, inMax, min_, max_)), min_, max_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
    {
        value_ = microClamp<int32_t>(microMap<int32_t>(in, inMin, inMax, min_, max_), min_, max_);
    }

    MicroParamInt(int32_t initial, int32_t min, int32_t max)
        : MicroParam(MicroParam::Type::INT), min_(min), max_(max)
    {
        value_ = microClamp<int32_t>(initial, min_, max_);
    }
};

class MicroParamFloat : public MicroParam
{
private:
    float value_;
    const float min_;
    const float max_;

public:
    // Assignment operators
    MicroParamFloat &operator=(float v)
    {
        value_ = microClamp<float>(v, min_, max_);
        return *this;
    }
    MicroParamFloat &operator=(int32_t v)
    {
        value_ = microClamp<float>((float)v, min_, max_);
        return *this;
    }
    MicroParamFloat &operator=(int v)
    {
        value_ = microClamp<float>((float)v, min_, max_);
        return *this;
    }
    MicroParamFloat &operator=(const MicroParamFloat &other)
    {
        value_ = other.value_;
        return *this;
    }

    // Implicit conversion
    operator float() const { return value_; }
    operator int32_t() const { return (int32_t)floor(value_); }

    // Virtual API
    void setFloat(float f) override { value_ = microClamp<float>(f, min_, max_); }
    float getFloat() const override { return value_; }

    void setInt(int32_t i) override { value_ = microClamp<float>((float)i, min_, max_); }
    int32_t getInt() const override { return (int32_t)floor(value_); }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microClamp<float>(microMap<float>(in, inMin, inMax, min_, max_), min_, max_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMax) override
    {
        value_ = microClamp<float>((float)microMap<int32_t>(in, inMin, inMax, (int32_t)min_, (int32_t)max_), min_, max_);
    }

    MicroParamFloat(float initial, float min, float max)
        : MicroParam(MicroParam::Type::FLOAT), min_(min), max_(max)
    {
        value_ = microClamp<float>(initial, min_, max_);
    }
};

// ---------------

class MicroParamEnum : public MicroParam
{
private:
    int32_t value_;
    const char **labels_;
    int32_t count_;

public:
    // Assignment operators
    MicroParamEnum &operator=(int32_t v)
    {
        value_ = microModulo(v, count_);
        return *this;
    }
    MicroParamEnum &operator=(int v)
    {
        value_ = microModulo((int32_t)v, count_);
        return *this;
    }
    MicroParamEnum &operator=(float v)
    {
        value_ = microModulo((int32_t)floor(v), count_);
        return *this;
    }
    MicroParamEnum &operator=(const MicroParamEnum &other)
    {
        value_ = other.value_;
        return *this;
    }

    // Implicit conversion
    operator int32_t() const { return value_; }
    operator float() const { return (float)value_; }

    // Virtual API
    void setInt(int32_t i) override { value_ = microModulo(i, count_); }
    int32_t getInt() const override { return value_; }

    void setFloat(float f) override { value_ = microModulo((int32_t)floor(f), count_); }
    float getFloat() const override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
    {
        value_ = microModulo((int32_t)floor(microMap<float>(in, inMin, inMax, 0, count_ - 1)), count_);
    }

    void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
    {
        value_ = map(in, inMin, inMaxExclusive, 0, count_ - 1);
    }

    MicroParamEnum(const char **labels, int32_t value, int32_t count)
        : MicroParam(MicroParam::Type::ENUM), labels_(labels), count_(count)
    {
        value_ = microModulo(value, count_);
    }
};

// ======================= Complex types =======================

class MicroParamBlob : public MicroParam
{
private:
    uint8_t *data_;
    uint32_t capacity_;
    uint32_t length_;

public:
    MicroParamBlob(uint8_t *buffer, uint32_t capacity, uint32_t length = 0)
        : MicroParam(MicroParam::Type::BLOB), data_(buffer), capacity_(capacity), length_(length) {}

    //const uint8_t *data() const { return data_; }
    uint8_t *get() const { return data_; }
    uint32_t getLength() const { return length_; }
    uint32_t getCapacity() const { return capacity_; }

    bool set(const uint8_t *buffer, uint32_t length)
    {
        if (length > capacity_)
            return false;
        memcpy(data_, buffer, length);
        length_ = length;
        return true;
    }

    // Array operator
    uint8_t &operator[](uint32_t index) { return data_[index]; }
    const uint8_t &operator[](uint32_t index) const { return data_[index]; }
};

class MicroParamString : public MicroParam
{
private:
    char *data_;
    uint32_t capacity_;
    uint32_t length_;

public:
    MicroParamString(char *buffer, uint32_t capacity)
        : MicroParam(MicroParam::Type::STRING), data_(buffer), capacity_(capacity), length_(0)
    {
        if (capacity_ == 0 || data_ == nullptr)
            return;
        length_ = strnlen(data_, capacity_ - 1);
        data_[length_] = '\0';
    }

    //const char *get() const { return data_; }
   char * get() const { return data_; }
    uint32_t getLength() const { return length_; }
    uint32_t getCapacity() const { return capacity_; }

    bool set(const char *s)
    {
        size_t lengthNew = strlen(s);
        if ( lengthNew >= capacity_-1 ) return false;
        length_ = lengthNew;
        memcpy(data_, s, length_);
        data_[length_] = '\0';
        return true;
    }

    // Assignment operator
    MicroParamString &operator=(const char *s)
    {
        set(s);
        return *this;
    }

    MicroParamString &operator=(const MicroParamString &other)
    {
        set(other.data_);
        return *this;
    }

    // Implicit conversion to const char*
    operator const char *() const { return data_; }
};

// ======================= MicroParamBindKey =======================

class MicroParamBindKey
{
private:
    const char *key_;
    const uint32_t keyHash_;
    MicroParam &param_;

public:
    // --------- Constructors ---------
    MicroParamBindKey(const char *k, MicroParam &param)
        : key_(k), keyHash_(microHashFnv1a(k)), param_(param)
    {
    }

    const char *getKey() const
    {
        return key_;
    }

    bool matches(const char *k) const
    {
        return strcmp(key_, k) == 0;
    }

    bool matches(uint32_t hash) const
    {
        return keyHash_ == hash;
    }

    bool matches(uint32_t hash, const char *addr) const
    {
        if (hash != keyHash_)
            return false;
        return strcmp(key_, addr) == 0;
    }

    MicroParam &  getParam()
    {
        return param_;
    }
};

#endif // __MICRO_PARAM_H__
