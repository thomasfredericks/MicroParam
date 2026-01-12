# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Structs and Classes

### `MicroParam:MicroParamInt`
Implements the abstract `MicroParam` interface with integer storage. Supports clamped assignment, integer and float access, and range mapping.
#### Constructors
- `MicroParamInt(initial, min, max)`: Creates an integer parameter with an initial value and inclusive bounds (all parameters are 32-bit signed integers)
#### Methods
##### `MicroParamInt::setInt(i)`
Sets the parameter value from an integer, clamped to the configured range using `Micro::clamp<int32_t>()`.
###### Parameters
- `i`: Value to assign (`int32_t`)
###### Returns
- None (`void`)

##### `MicroParamInt::getInt()`
Retrieves the current value as an integer.
###### Returns
- Current value (`int32_t`)

##### `MicroParamInt::setFloat(f)`
Sets the value from a float by flooring to an integer and clamping.
###### Parameters
- `f`: Value to assign (`float`)
###### Returns
- None (`void`)

##### `MicroParamInt::getFloat()`
Retrieves the current value as a float.
###### Returns
- Current value (`float`)

##### `MicroParamInt::mapFloat(in, inMin, inMax)`
Maps a float input from a source range into the parameter’s integer range using `Micro::map<float>()`.
###### Parameters
- `in`: Input value to map (`float`)
- `inMin`: Lower bound of the input range (`float`)
- `inMax`: Upper bound of the input range (`float`)
###### Returns
- None (`void`)

##### `MicroParamInt::mapInt(in, inMin, inMax)`
Maps an integer input from a source range into the parameter’s integer range using `Micro::map<int32_t>()`.
###### Parameters
- `in`: Input value to map (`int32_t`)
- `inMin`: Lower bound of the input range (`int32_t`)
- `inMax`: Upper bound of the input range (`int32_t`)
###### Returns
- None (`void`)

### `MicroParam:MicroParamFloat`
Implements the abstract `MicroParam` interface with float storage. Supports clamped assignment, integer and float access, and range mapping.
#### Constructors
- `MicroParamFloat(initial, mi
