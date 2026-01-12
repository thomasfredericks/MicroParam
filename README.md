# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### `MicroParam:MicroParamInt`
Implements the abstract `MicroParam` interface for integer parameters. Supports clamped assignment, integer and float access, and range mapping.

```cpp
MicroParamInt myMicroParamInt(initial, min, max)
```
Creates an integer parameter with an initial value and inclusive bounds.
**Parameters**:
- `initial`: Initial value of the parameter (`int32_t`)
- `min`: Minimum allowed value (`int32_t`)
- `max`: Maximum allowed value (`int32_t`)

```cpp
myMicroParamInt.setInt(i)
```
Sets the parameter value from an integer, clamped to the configured range using `Micro::clamp<int32_t>()`.
**Parameters**:
- `i`: Value to assign (`int32_t`)

```cpp
myMicroParamInt.getInt()
```
Retrieves the current value as an integer.
**Returns**:
- Current value (`int32_t`)

```cpp
myMicroParamInt.setFloat(f)
```
Sets the parameter value from a float (converted to integer with `floor()` and clamped).
**Parameters**:
- `f`: Value to assign (`float`)

```cpp
myMicroParamInt.getFloat()
```
Retrieves the current value as a float.
**Returns**:
- Current value (`float`)

```cpp
myMicroParamInt.mapFloat(in, inMin, inMax)
```
Maps a float from an input range to the parameter's range.
**Parameters**:
- `in`: Input value (`float`)
- `inMin`: Input range minimum (`float`)
- `inMax`: Input range maximum (`float`)

```cpp
myMicroParamInt.mapInt(in, inMin, inMax)
```
Maps an integer from an input range to the parameter's range.
**Parameters**:
- `in`: Input value (`int32_t`)
- `inMin`: Input range minimum (`int32_t`)
- `inMax`: Input range maximum (`int32_t`)

### `MicroParam:MicroParamFloat`
Implements the abstract `MicroParam` interface for floating-point parameters. Supports clamped assignment, integer and float access, and range mapping.

```cpp
MicroParamFloat myMicroParamFloat(initial, min, max)
```
Creates a float parameter with an initial value and inclusive bounds.
**Parameters**:
- `initial`: Initial value of the parameter (`float`)
- `min`: Minimum allowed value (`float`)
- `max`: Maximum allowed value (`float`)

```cpp
myMicroParamFloat.setFloat(f)
```
Sets the parameter value from a float, clamped to the configured range using `Micro::clamp<float>()`.
**Parameters**:
- `f`: Value to assign (`float`)

```cpp
myMicroParamFloat.getFloat()
```
Retrieves the current value as a float.
**Returns**:
- Current value (`float`)

```cpp
myMicroParamFloat.setInt(i)
```
Sets the parameter value from an integer (converted to float and clamped).
**Parameters**:
- `i`: Value to assign (`int32_t`)

```cpp
myMicroParamFloat.getInt()
```
Retrieves the current value as an integer (using `floor()`).
**Returns**:
- Current value (`int32_t`)

```cpp
myMicroParamFloat.mapFloat(in, inMin, inMax)
```
Maps a float from an input range to the parameter's range.
**Parameters**:
- `in`: Input value (`float`)
- `inMin`: Input range minimum (`float`)
- `inMax`: Input range maximum (`float`)

```cpp
myMicroParamFloat.mapInt(in, inMin, inMax)
```
Maps an integer from an input range to the parameter's range.
**Parameters**:
- `in`: Input value (`int32_t`)
- `inMin`: Input range minimum (`int32_t`)
- `inMax`: Input range maximum (`int32_t`)

### `MicroParamBind`
Typedef for `Micro::Bind<MicroParam *>`.  
Example use: `MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };`

### `MicroParamBinder`
Typedef for `Micro::Binder<MicroParam *>`.  
Example use: `MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));`
