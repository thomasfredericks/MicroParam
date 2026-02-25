
#ifndef __MICRO_PARAM_H__
#define __MICRO_PARAM_H__

#include <Arduino.h>
#include <MicroCommon.h>

// ======================= CORE =======================

// ------------------ Base class ------------------

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
    virtual void setInt(int32_t v) {};
    virtual int32_t getInt() const { return 0; };

    virtual void setFloat(float v) {};
    virtual float getFloat() const { return 0.0f; };
    virtual void mapFloat(float in, float inMin, float inMax) {};
    virtual void mapInt(int32_t in, int32_t inMin, int32_t inMax) {};

    virtual void setString(const char *s) {};
    virtual const char *getString() const { return nullptr; };
    virtual void setBlob(const uint8_t *data, size_t length) {};
    virtual size_t getBlob(uint8_t **data) const { return 0; }
};

// ======================= SIMPLE TYPES =======================

// ------------------ MicroParamByte ------------------

class MicroParamByte : public MicroParam
{
private:
    uint8_t value_;
    const uint8_t min_;
    const uint8_t max_;

public:
    MicroParamByte(uint8_t v, uint8_t min, uint8_t max)
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

// ------------------ MicroParamInt ------------------

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

    // -------- Constructor ----------
    MicroParamInt(int32_t initial, int32_t min, int32_t max)
        : MicroParam(MicroParam::Type::INT), min_(min), max_(max)
    {
        value_ = microClamp<int32_t>(initial, min_, max_);
    }
};

// ------------------ MicroParamFloat ------------------

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

// ------------------ MicroParamEnum ------------------

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

    void setString(const char *s) override
    {
        for (int32_t i = 0; i < count_; ++i)
        {
            if (strcmp(s, labels_[i]) == 0)
            {
                value_ = i;
                return;
            }
        }
    }

    const char *getString() const override
    {
        return labels_[value_];
    }

    MicroParamEnum(const char **labels, int32_t value, int32_t count)
        : MicroParam(MicroParam::Type::ENUM), labels_(labels), count_(count)
    {
        value_ = microModulo(value, count_);
    }
};

// ======================= COMPLEX TYPES =======================

// ------------------ MicroParamBlob ------------------

class MicroParamBlob : public MicroParam
{
private:
    uint8_t *data_;
    size_t capacity_;
    size_t length_;

public:
    MicroParamBlob(uint8_t *buffer, size_t capacity, size_t length)
        : MicroParam(MicroParam::Type::BLOB), data_(buffer), capacity_(capacity), length_(length) {}

    void setBlob(const uint8_t *data, size_t length) override
    {
        if (length > capacity_)
            return;
        memcpy(data_, data, length);
        length_ = length;
    }

    size_t getBlob(uint8_t **data) const override
    {
        *data = data_;
        return length_;
    }

    // FAST METHODS

    size_t get(uint8_t **blobData) const
    {
        *blobData = data_;
        return length_;
    }
    // size_t getLength() const { return length_; }
    // size_t getCapacity() const { return capacity_; }

    bool set(const uint8_t *buffer, size_t length)
    {
        if (length > capacity_)
            return false;
        memcpy(data_, buffer, length);
        length_ = length;
        return true;
    }

    // Array operator
    uint8_t &operator[](size_t index) { return data_[index]; }
    const uint8_t &operator[](size_t index) const { return data_[index]; }
};

// ------------------ MicroParamString ------------------

class MicroParamString : public MicroParam
{
private:
    char *str_;
    const size_t capacity_;
    size_t length_;

public:
    MicroParamString(char *buffer, size_t capacity)
        : MicroParam(MicroParam::Type::STRING), str_(buffer), capacity_(capacity)
    {
        // if (capacity_ == 0 || str_ == nullptr) return;
        length_ = strnlen(str_, capacity_ - 1);
        str_[capacity_ - 1] = '\0';
    }

    void setString(const char *s) override { set(s); }
    const char *getString() const override
    {
        return str_;
    }

    // FAST METHODS

    // size_t getLength() const { return length_; }
    // size_t getCapacity() const { return capacity_; }

    const char *get() const { return str_; }
    bool set(const char *s)
    {
        size_t i = 0;
        while (i < capacity_ - 1 && s[i] != '\0')
        {
            str_[i] = s[i];
            ++i;
        }

        if (s[i] != '\0') // source string was too long
            return false;

        str_[i] = '\0';
        length_ = i;
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
        set(other.get());
        return *this;
    }

    // Implicit conversion to const char*
    operator const char *() const { return str_; }
};

#endif // __MICRO_PARAM_H__
