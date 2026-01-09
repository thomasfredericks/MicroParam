
#include <Arduino.h>
#include <MicroTof.h>

#ifdef ESP32
#include <Preferences.h>
#endif

#include <Arduino.h>
#include <MicroTof.h>
/*
#define MICRO_SETTING_ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

#define MICRO_SETTING_GROUP(var, ...)             \
  MicroSetting *var##_children[] = {__VA_ARGS__}; \
  MicroSettingGroup var(var##_children, MICRO_SETTING_ARRAY_COUNT(var##_children))
*/

class MicroSetting; // forward declaration

class MicroSettingGroup
{
  MicroTof::GrowableArray<MicroSetting *> children_{4};

public:
  MicroSettingGroup()
  {
  }

  // Add a child setting (value or group)
  void add(MicroSetting *child)
  {
    if (child)
    {
      children_.add(child);
    }
  }

  // Number of children
  size_t getCount() const
  {
    return children_.getCount();
  }

  // Access a child by index
  MicroSetting *get(size_t index)
  {
    if (children_.getCount() == 0)
      return nullptr;
    index = MicroTof::clamp(index, 0, children_.getCount());
    return children_[index];
  }
};

class MicroSetting
{
protected:
  const char *name_;
  const char type_;

public:
  MicroSetting(MicroSettingGroup &group, const char *name, char type)
      : name_(name), type_(type)
  {
    group.add(this);
  }

  const char *getName() const { return name_; }
  char getType() { return type_; };

  virtual void setFloat(float f) = 0;
  virtual void setInt(int32_t i) = 0;
  virtual float getFloat() = 0;
  virtual int32_t getInt() = 0;
  virtual void mapFloat(float in, float inMin, float inMax) = 0;
  virtual void mapInt(int32_t in, int32_t inMin, int32_t inMax) = 0;

  /*   virtual int32_t getChildCount()
    {
      return 0; // returns 0 except for groups
    };
    virtual MicroSetting *getChild(int32_t index)
    {
      return nullptr; // returns nullptr except for groups
    } */
};

class MicroSettingInt : public MicroSetting
{
private:
  int32_t value_;
  int32_t min_;
  int32_t max_;

public:
  MicroSettingInt(MicroSettingGroup &group, const char *name, int32_t min, int32_t maxInclusive, int32_t initial)
      : MicroSetting(group, name, 'i'), min_(min), max_(maxInclusive)
  {
    setInt(initial);
  }

  void setInt(int32_t i) override
  {
    value_ = MicroTof::clamp(i, min_, max_);
  }

  int32_t getInt() { return value_; }

  void setFloat(float f) override
  {
    setInt(floor(f));
  }

  float getFloat() override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = MicroTof::mapf(in, inMin, inMax, min_, max_ );

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, min_, max_));
  }

};

class MicroSettingFloat : public MicroSetting
{
private:
  float value_;
  float min_;
  float max_;
  float step_;

public:
  MicroSettingFloat(MicroSettingGroup &group, const char *name, float min, float maxInclusive, float initial, float step)
      : MicroSetting(group, name, 'f'), min_(min), max_(maxInclusive), step_(step)
  {
    setFloat(initial);
  }

  MicroSettingFloat(MicroSettingGroup &group, const char *name, float min, float maxInclusive, float initial)
      : MicroSettingFloat(group, name, min, maxInclusive, initial, 0.1f)
  {
  }

  void setFloat(float f) override
  {
    value_ = MicroTof::clampf(f, min_, max_);
  }

  float getFloat() { return value_; }

  void setInt(int32_t i) override
  {
    setFloat((float)i);
  }

  int32_t getInt() override { return floor(value_); }
  
    void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = MicroTof::mapf(in, inMin, inMax, min_, max_ );

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, min_, max_));
  }
  
};

class MicroSettingEnum : public MicroSetting
{
private:
  int32_t value_;
  const char **labels_;
  int32_t count_;

public:
  MicroSettingEnum(MicroSettingGroup &group, const char *name, const char **labels, int32_t count)
      : MicroSetting(group, name, 'e'), labels_(labels), count_(count), value_(0) {}

  void setInt(int32_t i) override
  {
    value_ = MicroTof::modulo(i, count_);
  }

  int32_t getInt() override { return value_; }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  float getFloat() override { return (float)value_; }

    void mapFloat(float in, float inMin, float inMax) override
  {
    // Scale to integer range
    float mapped = MicroTof::mapf(in, inMin, inMax, 0, count_ - 1 );

    setFloat(mapped);
  }

  void mapInt(int32_t in, int32_t inMin, int32_t inMaxExclusive) override
  {
    setInt(map(in, inMin, inMaxExclusive, 0, count_ - 1));
  }
};

/*
#ifdef ESP32

  void putEachInPreferences(Preferences *preferences)
  {
    for (int i = 0; i < count_; i++)
    {
      // LOG("Putting", i, settings_[i]->getName() );
      settings_[i]->putInPreferences(preferences);
    }
  }

  void getEachFromPreferences(Preferences *preferences)
  {
    for (int i = 0; i < count_; i++)
    {
      // LOG("Getting", i, settings_[i]->getName() );
      settings_[i]->getFromPreferences(preferences);
    }
  }

#endif
};
*/
