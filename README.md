# MicroSetting

Micro (small, simple and lightweight) library to manage settings adapted for menus that are browsed with encoders or keyboard input.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Todo

- [ ] Implement a change callback
- [ ] Read/write flags
- [ ] Rename rotate to rotateValue
- [ ] Rename setInt to setInteger
- [ ] Add setChar
- [ ] Swap 
- [ ] MicroSettingGroup needs to be 100% compatible with an array of integers, floats and chars
	- setIndex
	- rotateIndex, rotateValue
	- MicroSettingArrayIntegers
	- MicroSettingArrayChars
	- MicroSettingArrayFloats

## Documentation

### Accessors
- `const char * getName()`
- `const char * getLabel()`
- `int32_t getInt()`

### Control
- `virtual void setInt(int32_t i)`
- `void rotate(int amount)`

### Printing
- `void printValueTo(Print * printer)`
- `void printTo(Print * printer, char * valueSeparator)`

### Utilities
- `static int32_t signedIntModulo(int32_t value, int32_t modulo)`

### Constructors

#### None

- `MicroSetting(const char * name)`

#### Int

- `MicroSetting(const char * name, int count)`
- `MicroSetting(const char * name, int32_t min, int32_t count)`
- `MicroSetting(const char * name, int32_t min, int32_t count, int32_t initial)`

#### Enum
- `MicroSetting(const char * name, const char ** labels, int32_t count)`

### ESP32-only
- `virtual void putInPreferences(Preferences * preferences)`
- `virtual void getFromPreferences(Preferences * preferences)`
