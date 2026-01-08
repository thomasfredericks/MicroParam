
#include <Arduino.h>
#include <MicroTof.h>

#ifdef ESP32
#include <Preferences.h>
#endif



// ===================================================
class MicroSetting {
public:

protected:
  const char * name_;
  int32_t int_min_ = 0;
  int32_t int_range_ = 1;
  int32_t int_value_ = 0;
  char type_ = 'n'; // none
  const char ** labels_ = 0;

public:

  const char * getName() {
    return name_;
  }

  static int32_t signedIntModulo(int32_t value, int32_t modulo) {
    while ( value < 0 ) value += modulo;
    return (value % modulo);
  }

  const char * getLabel() {
    if (labels_) return labels_[int_value_];
    return "";
  }



  void printValueTo(Print * printer)  {
    switch (type_) {
    case 'e':
      printer->print(labels_[int_value_]);
      break;
    case 'i' :
      printer->print(int_value_);
      break;
    }
  }

  void printTo(Print * printer, char * valueSeparator)  {
    printer->print(name_);
    if (valueSeparator) printer->print(valueSeparator);
    printValueTo(printer);
  }

  virtual void setInt(int32_t i) {
    int_value_ = MicroSetting::signedIntModulo(i - int_min_, int_range_ ) + int_min_;
  }

  void rotate(int amount)  {
    setInt(int_value_ + amount);
  }


  int32_t getInt() {
    return int_value_;
  }

  MicroSetting(const char * name) {
    name_ = name;
    type_ = 'n'; // none/nill
  }

  MicroSetting(const char * name, int count) {
    name_ = name;
    type_ = 'i';
    int_min_ = 0;
    int_range_ = count;
    int_value_ = int_min_;
  }

  MicroSetting(const char * name, int32_t min, int32_t count) {
    name_ = name;
    type_ = 'i';
    int_min_ = min;
    int_range_ = count;
    int_value_ = int_min_;
  }

  MicroSetting(const char * name, int32_t min, int32_t count, int32_t initial) {
    name_ = name;
    type_ = 'i';
    int_min_ = min;
    int_range_ = count;
    setInt(initial);
  }

  MicroSetting(const char * name, const char ** labels, int32_t count) {
    name_ = name;
    type_ = 'e'; //enum
    labels_ = labels;
    int_min_ = 0;
    int_range_ = count;
    int_value_ = int_value_;
  }

#ifdef ESP32
public:



  virtual void putInPreferences(Preferences * preferences) {
    switch (type_) {
    case 'e':
    case 'i' :
      preferences->putInt(name_, int_value_);
      break;
    }
  }


  virtual void getFromPreferences(Preferences * preferences) {

    switch (type_) {
    case 'e':
    case 'i' :
      setInt(preferences->getInt(name_, int_value_));
      break;
    }
  }


#endif
};


class MicroSettingGroup {

  size_t count_;
  size_t current_;
  const char * name_ ;
  MicroSetting ** settings_ = 0;
  //MicroSetting::levels level_ = MicroSetting::KEY; //key or value

public:

  void rotate(int amount) {
    /*
    if ( level_ == MicroSetting::KEY) current_ = MicroSetting::signedIntModulo(current_ + amount, count_);
    else settings_[current_]->rotate(amount);
    */
    settings_[current_]->rotate(amount);
  }

  void rotateIndex(int amount) {
    current_ = MicroSetting::signedIntModulo(current_ + amount, count_);
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

 
  MicroSetting * getCurrentSetting() {
    return settings_[current_];
  }

  MicroSetting * getSettingAtIndex(int index) {
    return settings_[index];
  }

  void setIndex(int index) {
    current_ = constrain(index, 0, count_);
  }

  int getIndex() {
    return current_;
  }

  const char * getName() {
    return name_;
  }

  int getCount() {
    return count_;
  }


  void printEachTo(Print * printer, char * valueSeparator, char * settingSeparator)  {
    for (int i = 0; i < count_; i++) {
      if ( i && settingSeparator) printer->print(settingSeparator);

      settings_[i]->printTo(printer, valueSeparator);
    }
  }

  MicroSettingGroup(const char * name, MicroSetting ** settings, size_t count) {
    name_ = name;
    settings_ = settings;
    count_ = count;
    current_ = 0;
  }


#ifdef ESP32

  void putEachInPreferences(Preferences * preferences) {
    for (int i = 0; i < count_; i++) {
      LOG("Putting", i, settings_[i]->getName() );
      settings_[i]->putInPreferences(preferences);
    }
  }

  void getEachFromPreferences(Preferences * preferences) {
    for (int i = 0; i < count_; i++) {
      LOG("Getting", i, settings_[i]->getName() );
      settings_[i]->getFromPreferences(preferences);
    }
  }

#endif
};


