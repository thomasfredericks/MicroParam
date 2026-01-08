
#include <Arduino.h>
#include <MicroTof.h>

#ifdef ESP32
#include <Preferences.h>
#endif

// ===================================================
class MicroSetting
{
public:
protected:
  const char *name_;
  union Value
  {
    int32_t i;
    float f;
  };
  Value min_;
  Value max_exclusive_;
  Value value_;
  float stepf_;
  char type_ = 'n'; // none
  const char **labels_ = 0;

public:
  const char *getName()
  {
    return name_;
  }

  const char *getLabel()
  {
    if (labels_)
      return labels_[value_.i];
    return "";
  }

  void printValueTo(Print *printer)
  {
    switch (type_)
    {
    case 'e':
      printer->print(labels_[value_.i]);
      break;
    case 'i':
      printer->print(value_.i);
      break;
    case 'f':
      printer->print(value_.f);
      break;
    }
  }

  void printTo(Print *printer, char *valueSeparator)
  {
    printer->print(name_);
    if (valueSeparator)
      printer->print(valueSeparator);
    printValueTo(printer);
  }

   void setFloat(float f)
  {
    switch (type_)
    {
    case 'e':
    case 'i':
      value_.i = MicroTof::wrap(floor(f), min_.i, max_exclusive_.i);
      break;
    case 'f':
      value_.f = MicroTof::wrapf(f, min_.f, max_exclusive_.f);
      break;
    }
  }

   void setInt(int32_t i)
  {
    switch (type_)
    {
    case 'e':
    case 'i':
      value_.i = MicroTof::wrap(i, min_.i, max_exclusive_.i);
      break;
    case 'f':
      value_.f = MicroTof::wrapf(i, min_.f, max_exclusive_.f);
      break;
    }
  }

  void rotate(int amount)
  {
    switch (type_)
    {
    case 'e':
    case 'i':
      setInt(value_.i + amount);
      break;
    case 'f':
      setFloat(value_.f + ((float)amount * stepf_));
      break;
    }
  }

  int32_t getInt()
  {
    return value_.i;
  }

  // None
  MicroSetting(const char *name) : name_(name), type_('n')
  {
  }

  // Int
  MicroSetting(const char *name,
               int32_t min,
               int32_t max,
               int32_t initial)
      : name_(name), type_('i')
  {
    min_.i = min;
    max_exclusive_.i = max;
    setInt(initial);
  }
/*
  MicroSetting(const char *name, int32_t max)
      : MicroSetting(name, 0, max, 0)
  {
  }

  MicroSetting(const char *name, int32_t min, int32_t max)
      : MicroSetting(name, min, max, min)
  {
  }
*/
  // Enum
  MicroSetting(const char *name, const char **labels, int32_t count)
      : name_(name), type_('e'), labels_(labels)
  {
    value_.i = min_.i = 0;
    max_exclusive_.i = count;
  }

  // Float
  MicroSetting(const char *name,
               float min,
               float max,
               float initial,
               float step)
      : name_(name), type_('f'), stepf_(step)
  {
    min_.f = min;
    max_exclusive_.f = max;
    setFloat(initial);
  }
/*
  // Constructor with only max (min = 0, initial = 0)
  MicroSetting(const char *name, float max)
      : MicroSetting(name, 0.0f, max, 0.0f, 0.1f)
  {
  }

  // Constructor with min and max (initial = min)
  MicroSetting(const char *name, float min, float max)
      : MicroSetting(name, min, max, min, 0.1f)
  {
  }
*/

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
};

class MicroSettingGroup
{

  size_t count_;
  size_t current_;
  const char *name_;
  MicroSetting **settings_ = 0;
  // MicroSetting::levels level_ = MicroSetting::KEY; //key or value

public:
  void rotate(int amount)
  {
    /*
    if ( level_ == MicroSetting::KEY) current_ = MicroSetting::signedIntModulo(current_ + amount, count_);
    else settings_[current_]->rotate(amount);
    */
    settings_[current_]->rotate(amount);
  }

  void rotateIndex(int amount)
  {
    current_ = MicroTof::wrap(current_ + amount, 0, count_);
  }
  /*
    void rotateValue(int amount) {
      settings_[current_]->rotate(amount);
    }

    void toggleLevel() {
      if ( level_ == MicroSetting::KEY) level_ = MicroSetting::VALUE ;
      else level_ = MicroSetting::KEY;
    }

    MicroSetting::levels getLevel() {
      return level_;
    }

    void setLevel(MicroSetting::levels level) {
      level_ = level;
    }
    */

  MicroSetting *getCurrentSetting()
  {
    return settings_[current_];
  }

  MicroSetting *getSettingAtIndex(int index)
  {
    return settings_[index];
  }

  void setIndex(int index)
  {
    current_ = constrain(index, 0, count_);
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

  void printEachTo(Print *printer, char *valueSeparator, char *settingSeparator)
  {
    for (int i = 0; i < count_; i++)
    {
      if (i && settingSeparator)
        printer->print(settingSeparator);

      settings_[i]->printTo(printer, valueSeparator);
    }
  }

  MicroSettingGroup(const char *name, MicroSetting **settings, size_t count)
  {
    name_ = name;
    settings_ = settings;
    count_ = count;
    current_ = 0;
  }

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
