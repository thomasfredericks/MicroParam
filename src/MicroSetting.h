
#include <Arduino.h>
#include <MicroTof.h>

#ifdef ESP32
#include <Preferences.h>
#endif

#include <Arduino.h>
#include <MicroTof.h>

class MicroSetting
{
protected:
  const char *name_;
  const char type_;

public:
  MicroSetting(const char *name, char type) : name_(name), type_(type) {}
  // virtual ~MicroSettingBase() = default;

  const char *getName() const { return name_; }
  virtual void setFloat(float f) = 0;
  virtual void setInt(int32_t i) = 0;
  virtual float getFloat() = 0;
  virtual int32_t getInt() = 0;
  virtual void rotate(int amount) = 0;
};

class MicroSettingInt : public MicroSetting
{
private:
  int32_t value_;
  int32_t min_;
  int32_t max_;

public:
  MicroSettingInt(const char *name, int32_t min, int32_t maxExclusive, int32_t initial)
      : MicroSetting(name, 'i'), min_(min), max_(maxExclusive)
  {
    setInt(initial);
  }

  void setInt(int32_t i) override
  {
    value_ = MicroTof::clampExclusive(i, min_, max_);
  }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  void rotate(int amount) override
  {
    setInt(value_ + amount);
  }

  int32_t getInt() override { return value_; }

  float getFloat() override { return (float)value_; }
};

class MicroSettingFloat : public MicroSetting
{
private:
  float value_;
  float min_;
  float max_;
  float step_;

public:
  MicroSettingFloat(const char *name, float min, float maxInclusive, float initial, float step)
      : MicroSetting(name, 'f'), min_(min), max_(maxInclusive), step_(step)
  {
    setFloat(initial);
  }

  MicroSettingFloat(const char *name, float min, float maxInclusive, float initial)
     : MicroSettingFloat(name, min, maxInclusive, initial, 0.1f) 
  {
  }

  void setFloat(float f) override
  {
    value_ = MicroTof::clampInclusivef(f, min_, max_);
  }

  void setInt(int32_t i) override
  {
    setFloat((float)i);
  }

  void rotate(int amount) override
  {
    setFloat(value_ + step_ * amount);
  }

  float getFloat() override { return value_; }

  int32_t getInt() override { return floor(value_); }
};

class MicroSettingEnum : public MicroSetting
{
private:
  int32_t value_;
  const char **labels_;
  int32_t count_;

public:
  MicroSettingEnum(const char *name, const char **labels, int32_t count)
      : MicroSetting(name, 'e'), labels_(labels), count_(count), value_(0) {}

  void setInt(int32_t i) override
  {
    value_ = MicroTof::wrapExclusive(i, 0, count_);
  }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  void rotate(int amount) override
  {
    setInt(value_ + amount);
  }

  int32_t getInt() override { return value_; }

  float getFloat() override { return (float)value_; }
};

class MicroSettingGroup
{

  size_t count_;
  size_t current_;
  const char *name_;
  MicroSetting **settings_ = 0;
  // MicroSetting::levels level_ = MicroSetting::KEY; //key or value

public:

  void rotateIndex(int amount)
  {
    current_ = MicroTof::wrapExclusive(current_ + amount, 0, count_);
  }

  MicroSetting * getCurrentSetting()
  {
    return settings_[current_];
  }

MicroSetting * getSettingAtIndex(int index) const {
    return (index >= 0 && index < count_) ? settings_[index] : nullptr;
}

  void setIndex(int index)
  {
    current_ = MicroTof::clampExclusive(index, 0, count_);
  }

  int getIndex()
  {
    return current_;
  }

  const char *getName()
  {
    return name_;
  }

  int getCount()
  {
    return count_;
  }
  /*
    void printEachTo(Print *printer, char *valueSeparator, char *settingSeparator)
    {
      for (int i = 0; i < count_; i++)
      {
        if (i && settingSeparator)
          printer->print(settingSeparator);

        settings_[i]->printTo(printer, valueSeparator);
      }
    }
  */
  MicroSettingGroup(const char *name, MicroSetting **settings, size_t count)
  {
    name_ = name;
    settings_ = settings;
    count_ = count;
    current_ = 0;
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



/*
#ifdef ESP32
public:
  virtual void putInPreferences(Preferences *preferences)
  {
    switch (type_)
    {
    case 'e':
    case 'i':
      preferences->putInt(name_, value_.i);
      break;
    }
  }

  virtual void getFromPreferences(Preferences *preferences)
  {

    switch (type_)
    {
    case 'e':
    case 'i':
      setInt(preferences->getInt(name_, value_.i));
      break;
    }
  }

#endif
*/
