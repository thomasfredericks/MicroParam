# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### MicroParam
Base class for parameter types. Represents a generic parameter with integer and float interfaces.

#### Constructors

```cpp
MicroParam myMicroParam(type);
```
Creates a parameter with a specific type.

- Parameters:
  - `type`: Character representing the parameter type (`char`)

### MicroParamInt
Integer parameter with minimum and maximum bounds.

#### Constructors

```cpp
MicroParamInt myMicroParamInt(initial, min, max);
```
Creates an integer parameter with an initial value and bounds.

- Parameters:
  - `initial`: Initial value (`int32_t`)
  - `min`: Minimum allowed value (`int32_t`)
  - `max`: Maximum allowed value (`int32_t`)

#### Method `setInt(i);`

```cpp
myMicroParamInt.setInt(i);
```
Sets the integer value, clamped to the minimum and maximum.

- Parameters:
  - `i`: Value to set (`int32_t`)

#### Method `getInt();`

```cpp
int32_t value = myMicroParamInt.getInt();
```
Gets the current integer value.

- Returns:
  - Current value (`int32_t`)

#### Method `setFloat(f);`

```cpp
myMicroParamInt.setFloat(f);
```
Sets the parameter from a float by flooring the value and clamping it.

- Parameters:
  - `f`: Float value to set (`float`)

#### Method `getFloat();`

```cpp
float value = myMicroParamInt.getFloat();
```
Gets the current value as a float.

- Returns:
  - Current value (`float`)

#### Method `mapFloat(in, inMin, inMax);`

```cpp
myMicroParamInt.mapFloat(in, inMin, inMax);
```
Maps a float input from a source range to the parameter's integer range.

- Parameters:
  - `in`: Input value (`float`)
  - `inMin`: Minimum of input range (`float`)
  - `inMax`: Maximum of input range (`float`)

#### Method `mapInt(in, inMin, inMax);`

```cpp
myMicroParamInt.mapInt(in, inMin, inMax);
```
Maps an integer input from a source range to the parameter's integer range.

- Parameters:
  - `in`: Input value (`int32_t`)
  - `inMin`: Minimum of input range (`int32_t`)
  - `inMax`: Maximum of input range (`int32_t`)

### MicroParamFloat
Floating-point parameter with minimum and maximum bounds.

#### Constructors

```cpp
MicroParamFloat myMicroParamFloat(initial, min, max);
```
Creates a float parameter with an initial value and bounds.

- Parameters:
  - `initial`: Initial value (`float`)
  - `min`: Minimum allowed value (`float`)
  - `max`: Maximum allowed value (`float`)

#### Method `setFloat(f);`

```cpp
myMicroParamFloat.setFloat(f);
```
Sets the float value, clamped to the minimum and maximum.

- Parameters:
  - `f`: Value to set (`float`)

#### Method `getFloat();`

```cpp
float value = myMicroParamFloat.getFloat();
```
Gets the current float value.

- Returns:
  - Current value (`float`)

#### Method `setInt(i);`

```cpp
myMicroParamFloat.setInt(i);
```
Sets the parameter from an integer by converting it to float and clamping.

- Parameters:
  - `i`: Integer value to set (`int32_t`)

#### Method `getInt();`

```cpp
int32_t value = myMicroParamFloat.getInt();
```
Gets the current value as an integer (floored).

- Returns:
  - Current value (`int32_t`)

#### Method `mapFloat(in, inMin, inMax);`

```cpp
myMicroParamFloat.mapFloat(in, inMin, inMax);
```
Maps a float input from a source range to the parameter's float range.

- Parameters:
  - `in`: Input value (`float`)
  - `inMin`: Minimum of input range (`float`)
  - `inMax`: Maximum of input range (`float`)

#### Method `mapInt(in, inMin, inMax);`

```cpp
myMicroParamFloat.mapInt(in, inMin, inMax);
```
Maps an integer input from a source range to the parameter's float range.

- Parameters:
  - `in`: Input value (`int32_t`)
  - `inMin`: Minimum of input range (`int32_t`)
  - `inMax`: Maximum of input range (`int32_t`)

### MicroParamBind
Type alias for `Micro::Bind<MicroParam *>`.

Example usage:
```cpp
MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };
```

### MicroParamBinder
Type alias for `Micro::Binder<MicroParam *>`.

Example usage:
```cpp
MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));
```
