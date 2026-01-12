# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### `MicroParamInt`
Implements MicroParam for integer values.

```cpp
MicroParamInt myMicroParamInt(initial, min, max);
```
Constructs a MicroParamInt with an initial value and range.

Parameters:
- `initial`:  Initial value (`int32_t`)
- `min`:  Minimum allowed value (`int32_t`)
- `max`:  Maximum allowed value (`int32_t`)

#### `myMicroParamInt.setInt(i);`
Sets the integer value, clamped to the min and max range.

Parameters:
- `i`:  New integer value (`int32_t`)

#### `myMicroParamInt.getInt();`
Gets the current integer value.

Returns:
- Current integer value (`int32_t`)

#### `myMicroParamInt.setFloat(f);`
Sets the value using a float, converted to integer and clamped.

Parameters:
- `f`:  New float value (`float`)

#### `myMicroParamInt.getFloat();`
Gets the current value as a float.

Returns:
- Current value (`float`)

#### `myMicroParamInt.mapFloat(in, inMin, inMax);`
Maps a float input from one range to the internal integer range and sets it.

Parameters:
- `in`:  Input value (`float`)
- `inMin`:  Minimum of input range (`float`)
- `inMax`:  Maximum of input range (`float`)

#### `myMicroParamInt.mapInt(in, inMin, inMax);`
Maps an integer input from one range to the internal integer range and sets it.

Parameters:
- `in`:  Input value (`int32_t`)
- `inMin`:  Minimum of input range (`int32_t`)
- `inMax`:  Maximum of input range (`int32_t`)

### `MicroParamFloat`
Implements MicroParam for floating-point values.

```cpp
MicroParamFloat myMicroParamFloat(initial, min, max);
```
Constructs a MicroParamFloat with an initial value and range.

Parameters:
- `initial`:  Initial value (`float`)
- `min`:  Minimum allowed value (`float`)
- `max`:  Maximum allowed value (`float`)

#### `myMicroParamFloat.setFloat(f);`
Sets the float value, clamped to the min and max range.

Parameters:
- `f`:  New float value (`float`)

#### `myMicroParamFloat.getFloat();`
Gets the current float value.

Returns:
- Current value (`float`)

#### `myMicroParamFloat.setInt(i);`
Sets the value using an integer, converted to float and clamped.

Parameters:
- `i`:  New integer value (`int32_t`)

#### `myMicroParamFloat.getInt();`
Gets the current value as an integer (floored).

Returns:
- Current value (`int32_t`)

#### `myMicroParamFloat.mapFloat(in, inMin, inMax);`
Maps a float input from one range to the internal float range and sets it.

Parameters:
- `in`:  Input value (`float`)
- `inMin`:  Minimum of input range (`float`)
- `inMax`:  Maximum of input range (`float`)

#### `myMicroParamFloat.mapInt(in, inMin, inMax);`
Maps an integer input from one range to the internal float range and sets it.

Parameters:
- `in`:  Input value (`int32_t`)
- `inMin`:  Minimum of input range (`int32_t`)
- `inMax`:  Maximum of input range (`int32_t`)

## Typedefs

### `MicroParamBind`
Type alias for `Micro::Bind<MicroParam *>`.
- Example use: `MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };`

### `MicroParamBinder`
Type alias for `Micro::Binder<MicroParam *>`.
- Example use: `MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));`
