
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
  char getType() { return type_; };
  virtual void rotate(int32_t amount)
  {
  }
  virtual void setFloat(float f)
  {
  }
  virtual void setInt(int32_t i)
  {
  }
  virtual float getFloat()
  {
    return 0;
  }
  virtual int32_t getInt()
  {
    return 0;
  }
  virtual int32_t getMicroSettingCount()
  {
    return 0; // returns 0 except for groups
  };
  virtual MicroSetting *getMicroSetting(int32_t index)
  {
    return nullptr; // returns nullptr except for groups
  }
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

  void setInt(int32_t i)
  {
    value_ = MicroTof::clampExclusive(i, min_, max_);
  }

  void rotate(int32_t amount) override
  {
    setInt(value_ + amount);
  }

  int32_t getInt() { return value_; }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

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

  void setFloat(float f)
  {
    value_ = MicroTof::clampInclusivef(f, min_, max_);
  }

  void rotate(int32_t amount) override
  {
    setFloat(value_ + step_ * amount);
  }

  float getFloat() { return value_; }

  void setInt(int32_t i) override
  {
    setFloat((float)i);
  }

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

  void rotate(int32_t amount) override
  {
    setInt(value_ + amount);
  }

  int32_t getInt() override { return value_; }

  void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  float getFloat() override { return (float)value_; }
};

class MicroSettingGroup : public MicroSetting
{

  size_t count_;
  size_t current_;
  const char *name_;
  MicroSetting **settings_ = 0;
  // MicroSetting::levels level_ = MicroSetting::KEY; //key or value

public:

 /*  void rotate(int32_t amount) override
  {
    current_ = MicroTof::wrapExclusive(current_ + amount, 0, count_);
  }

  void setInt(int32_t index) override
  {
    current_ = MicroTof::clampExclusive(index, 0, count_);
  }

  int32_t getInt() override
  {
    return current_;
  } */

  MicroSettingGroup(const char *name, MicroSetting **settings, size_t count) : MicroSetting(name, 'g')
  {
    settings_ = settings;
    count_ = count;
    current_ = 0;
  }

/*   void setFloat(float f) override
  {
    setInt((int32_t)floor(f));
  }

  float getFloat() override { return (float)current_; } */

  int32_t getMicroSettingCount() override { return count_; }

  MicroSetting *getMicroSetting(int32_t index) override
  {
    if (count_ == 0)
      return nullptr;
    return settings_[MicroTof::clampExclusive(index, 0, count_)];
  }

  /*
  MicroSetting *getCurrentSetting()
  {
    return settings_[current_];
  }

  MicroSetting *getSettingAtIndex(int index) const
  {
    return (index >= 0 && index < count_) ? settings_[index] : nullptr;
  }



  int getCount()
  {
    return count_;
  }
    */

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

MicroSetting *matchPath(MicroSetting *root, const char *path, char separator)
{
  if (!root || !path)
    return nullptr;

  // Skip leading '/'
  if (*path == separator)
    path++;

  MicroSetting *current = root;

  while (*path && current)
  {
    // Find end of current segment
    const char *segmentStart = path;
    const char *segmentEnd = path;

    while (*segmentEnd && *segmentEnd != separator)
      segmentEnd++;

    size_t segmentLen = segmentEnd - segmentStart;

    // Search children
    bool found = false;
    int count = current->getMicroSettingCount();
    if (current->getType() == 'g') // If a group
    {
      for (int i = 0; i < count; i++)
      {
        MicroSetting *child = current->getMicroSetting(i);
        const char *name = child->getName();

        // Exact match: length + content
        if (strncmp(name, segmentStart, segmentLen) == 0 &&
            name[segmentLen] == '\0')
        {
          current = child;
          found = true;
          break;
        }
      }
    }

    if (!found)
      return nullptr;

    // Move to next segment
    path = segmentEnd;
    if (*path == separator)
      path++;
  }

  return current;
}
