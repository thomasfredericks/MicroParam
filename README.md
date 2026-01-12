# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### `:MicroParam:MicroParamInt`
Implements the abstract `MicroParam` interface for integer parameters. Supports clamped assignment, integer and float access, and range mapping.

Creates an integer parameter with an initial value and inclusive bounds.
```cpp
MicroParamInt(initial, min, max)
```
**Parameters**:
- `initial`: Initial value of the parameter (`int32_t`)
- `min`: Minimum allowed value (`int32_t`)
- `max`: Maximum allowed value (`int32_t`)

Sets the parameter value from an integer, clamped to the configured range using `Micro::clamp<int32_t>()`.
```cpp
MicroParamInt.setInt(i)
```
**Parameters**:
- `i`: Value to assign (`int32_t`)

Retrieves the current value as an integer.
```cpp
MicroParamInt.getInt()
```
**Returns**:
- Current value (`int32_t`)

Sets the parameter value from a float (converted to integer with `floor()` and clamped).
```cpp
MicroParamInt.setFloat(f)
```
**Parameters**:
- `f`: Value to assign (`float`)

Retrieves the current value as a float.
```cpp
MicroParamInt.getFloat()
```
**Returns**:
- Current value (`float`)

Maps a float from an input range to the parameter's range.
```cpp
MicroParamInt.mapFloat(in, inMin, inMax)
```
**Parameters**:
- `in`: Input value (`float`)
- `inMin`: Input range minimum (`float`)
- `inMax`: Input range maximum (`float`)

Maps an integer from an input range to the parameter's range.
```cpp
MicroParamInt.mapInt(in, inMin, inMax)
```
**Parameters**:
- `in`: Input value (`int32_t`)
- `inMin`: Input range minimum (`int32_t`)
- `inMax`: Input range maximum (`int32_t`)

### `:MicroParam:MicroParamFloat`
Implements the abstract `MicroParam` interface for floating-point parameters. Supports clamped assignment, integer and float access, and range mapping.

Creates a float parameter with an initial value and inclusive bounds.
```cpp
MicroParamFloat(initial, min, max)
```
**Parameters**:
- `initial`: Initial value of the parameter (`float`)
- `min`: Minimum allowed value (`float`)
- `max`: Maximum allowed value (`float`)

Sets the parameter value from a float, clamped to the configured range using `Micro::clamp<float>()`.
```cpp
MicroParamFloat.setFloat(f)
```
**Parameters**:
- `f`: Value to assign (`float`)

Retrieves the current value as a float.
```cpp
MicroParamFloat.getFloat()
```
**Returns**:
- Current value (`float`)

Sets the parameter value from an integer (converted to float and clamped).
```cpp
MicroParamFloat.setInt(i)
```
**Parameters**:
- `i`: Value to assign (`int32_t`)

Retrieves the current value as an integer (using `floor()`).
```cpp
MicroParamFloat.getInt()
```
**Returns**:
- Current value (`int32_t`)

Maps a float from an input range to the parameter's range.
```cpp
MicroParamFloat.mapFloat(in, inMin, inMax)
```
**Parameters**:
- `in`: Input value (`float`)
- `inMin`: Input range minimum (`float`)
- `inMax`: Input range maximum (`float`)

Maps an integer from an input range to the parameter's range.
```cpp
MicroParamFloat.mapInt(in, inMin, inMax)
```
**Parameters**:
- `in`: Input value (`int32_t`)
- `inMin`: Input range minimum (`int32_t`)
- `inMax`: Input range maximum (`int32_t`)

### `MicroParamBind`
Typedef for `Micro::Bind<MicroParam *>`. Example use: `MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };`

### `MicroParamBinder`
Typedef for `Micro::Binder<MicroParam *>`. Example use: `MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));`
