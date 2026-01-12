# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### `MicroParam:MicroParamInt`
Implements the abstract `MicroParam` interface for integer parameters. Supports clamped assignment, integer and float access, and range mapping.
#### Constructors
- `MicroParamInt(initial, min, max)`: Creates an integer parameter with an initial value and inclusive bounds (all parameters are `int32_t`).
#### `MicroParamInt::setInt(i)`
Sets the parameter value from an integer, clamped to the configured range using `Micro::clamp<int32_t>()`.
**Parameters**:
- `i`: Value to assign (`int32_t`)
**Returns**:
- None (`void`)

#### `MicroParamInt::getInt()`
Retrieves the current value as an integer.
**Parameters**:
- None
**Returns**:
- Current value (`int32_t`)

#### `MicroParamInt::setFloat(f)`
Sets the value from a float by flooring to an integer and clamping.
**Parameters**:
- `f`: Value to assign (`float`)
**Returns**:
- None (`void`)

#### `MicroParamInt::getFloat()`
Retrieves the current value as a float.
**Parameters**:
- None
**Returns**:
- Current value (`float`)

#### `MicroParamInt::mapFloat(in, inMin, inMax)`
Maps a float input from a source range into the parameter’s integer range using `Micro::map<float>()`.
**Parameters**:
- `in`: Input value to map (`float`)
- `inMin`: Lower bound of the input range (`float`)
- `inMax`: Upper bound of the input range (`float`)
**Returns**:
- None (`void`)

#### `MicroParamInt::mapInt(in, inMin, inMax)`
Maps an integer input from a source range into the parameter’s integer range using `Micro::map<int32_t>()`.
**Parameters**:
- `in`: Input value to map (`int32_t`)
- `inMin`: Lower bound of the input range (`int32_t`)
- `inMax`: Upper bound of the input range (`int32_t`)
**Returns**:
- None (`void`)

### `MicroParam:MicroParamFloat`
Implements the abstract `MicroParam` interface for float parameters. Supports clamped assignment, integer and float access, and range mapping.
#### Constructors
- `MicroParamFloat(initial, min, max)`: Creates a float parameter with an initial value and bounds (all parameters are `float`).
#### `MicroParamFloat::setFloat(f)`
Sets the parameter value, clamped to the configured range using `Micro::clamp<float>()`.
**Parameters**:
- `f`: Value to assign (`float`)
**Returns**:
- None (`void`)

#### `MicroParamFloat::getFloat()`
Retrieves the current value as a float.
**Parameters**:
- None
**Returns**:
- Current value (`float`)

#### `MicroParamFloat::setInt(i)`
Sets the value from an integer by converting to float and clamping.
**Parameters**:
- `i`: Value to assign (`int32_t`)
**Returns**:
- None (`void`)

#### `MicroParamFloat::getInt()`
Retrieves the current value as an integer by flooring the stored float.
**Parameters**:
- None
**Returns**:
- Current value (`int32_t`)

#### `MicroParamFloat::mapFloat(in, inMin, inMax)`
Maps a float input from a source range into the parameter’s float range using `Micro::map<float>()`.
**Parameters**:
- `in`: Input value to map (`float`)
- `inMin`: Lower bound of the input range (`float`)
- `inMax`: Upper bound of the input range (`float`)
**Returns**:
- None (`void`)

#### `MicroParamFloat::mapInt(in, inMin, inMax)`
Maps an integer input from a source range into the parameter’s float range using `Micro::map<int32_t>()`.
**Parameters**:
- `in`: Input value to map (`int32_t`)
- `inMin`: Lower bound of the input range (`int32_t`)
- `inMax`: Upper bound of the input range (`int32_t`)
**Returns**:
- None (`void`)

### `MicroParamBind`
Binding type between a key and a `MicroParam` pointer, based on `Micro::Bind<MicroParam *>`.  
Example use: `MicroParamBind bindings[] = { {"/param1", &param1}, {"/param2", &param2} };`
- **Template Parameter:** `MicroParam*` – Pointer to a parameter instance

### `MicroParamBinder`
Binder utility for `MicroParam` pointers, based on `Micro::Binder<MicroParam *>`.  
Example use: `MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));`
- **Template Parameter:** `MicroParam*` – Pointer to a parameter instance
