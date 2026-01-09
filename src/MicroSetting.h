
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
  MicroTof::ResizableArray<MicroSetting *> children_{4};

public:
  // Constructor: optionally pass parent
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
  MicroSetting * get(size_t index)
  {
    if (children_.getCount() == 0)
      return nullptr;
    index = MicroTof::clampExclusive(index, 0, children_.getCount());
    return children_[index];
  }
};

class MicroSetting
{
protected:
  const char *name_;
  const char type_;

public:
  MicroSetting(MicroSettingGroup & group, const char *name, char type)
      : name_(name), type_(type)
  {
    group.add(this);
  }

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
  MicroSettingInt(MicroSettingGroup &group, const char *name, int32_t min, int32_t maxExclusive, int32_t initial)
      : MicroSetting(group, name, 'i'), min_(min), max_(maxExclusive)
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
  MicroSettingFloat(MicroSettingGroup &group, const char *name, float min, float maxInclusive, float initial, float step)
      : MicroSetting(group, name, 'f'), min_(min), max_(maxInclusive), step_(step)
  {
    setFloat(initial);
  }

  MicroSettingFloat(MicroSettingGroup &group, const char *name, float min, float maxInclusive, float initial)
      : MicroSettingFloat(group, name, min, maxInclusive, initial, 0.1f)
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
  MicroSettingEnum(MicroSettingGroup &group, const char *name, const char **labels, int32_t count)
      : MicroSetting(group, name, 'e'), labels_(labels), count_(count), value_(0) {}

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

/*
MicroSetting *matchPath(MicroSetting *root, const char *path, char separator)
{
    if (!root || !path) return nullptr;

    MicroSetting *current = root;

    while (*path && current)
    {
        // Find end of current segment
        const char *segmentEnd = strchr(path, separator);
        size_t segmentLen = segmentEnd ? (size_t)(segmentEnd - path) : strlen(path);

        // Search children
        bool found = false;
        int count = current->getChildCount();

        for (int i = 0; i < count; i++)
        {
            MicroSetting *child = current->getChild(i);
            const char *name = child->getName();

            // Match including the leading '/'
            if (strncmp(name, path, segmentLen) == 0 && name[segmentLen] == '\0')
            {
                current = child;
                found = true;
                break;
            }
        }

        if (!found) return nullptr;

        // Move to next segment
        path += segmentLen;
        if (*path == separator) path++;
    }

    return current;
} */
