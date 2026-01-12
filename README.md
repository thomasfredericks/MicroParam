# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

### Structs and Classes

### MicroParam
- Abstract base class representing a generic parameter that can be accessed, converted, and range-mapped as either an integer or a floating-point value.
- Members:
  - type_ – Internal identifier describing the parameter type (character)
- Constructors:
  - MicroParam(type) – Creates a parameter with a given type identifier (type is a character)
- Methods:
  - MicroParam::setInt(v) – Assigns the parameter value from an integer representation.
    - **Parameters:**
      - v – Value to assign (32-bit signed integer)
    - **Returns:** None (void)
  - MicroParam::getInt() – Retrieves the parameter value as an integer.
    - **Returns:** Current value (32-bit signed integer)
  - MicroParam::setFloat(v) – Assigns the parameter value from a floating-point representation.
    - **Parameters:**
      - v – Value to assign (float)
    - **Returns:** None (void)
  - MicroParam::getFloat() – Retrieves the parameter value as a floating-point number.
    - **Returns:** Current value (float)
  - MicroParam::mapFloat(in, inMin, inMax) – Maps a floating-point input from a source range into the parameter’s internal range.
    - **Parameters:**
      - in – Input value to map (float)
      - inMin – Lower bound of the input range (float)
      - inMax – Upper bound of the input range (float)
    - **Returns:** None (void)
  - MicroParam::mapInt(in, inMin, inMax) – Maps an integer input from a source range into the parameter’s internal range.
    - **Parameters:**
      - in – Input value to map (32-bit signed integer)
      - inMin – Lower bound of the input range (32-bit signed integer)
      - inMax – Upper bound of the input range (32-bit signed integer)
    - **Returns:** None (void)

### MicroParamInt
- Concrete implementation of `MicroParam` for integer-backed parameters with clamping and range mapping.
- Members:
  - value_ – Current stored value (32-bit signed integer)
  - min_ – Minimum allowed value (32-bit signed integer)
  - max_ – Maximum allowed value (32-bit signed integer)
- Constructors:
  - MicroParamInt(initial, min, max) – Creates an integer parameter with an initial value and inclusive bounds (all parameters are 32-bit signed integers)
- Methods:
  - MicroParamInt::setInt(i) – Sets the integer value, clamped to the configured range using `Micro::clamp<int32_t>()`.
    - **Parameters:**
      - i – Value to assign (32-bit signed integer)
    - **Returns:** None (void)
  - MicroParamInt::getInt() – Retrieves the stored integer value.
    - **Returns:** Current value (32-bit signed integer)
  - MicroParamInt::setFloat(f) – Sets the value from a float by flooring to an integer, then clamping.
    - **Parameters:**
      - f – Value to assign (float)
    - **Returns:** None (void)
  - MicroParamInt::getFloat() – Retrieves the stored value as a floating-point number.
    - **Returns:** Current value (float)
  - MicroParamInt::mapFloat(in, inMin, inMax) – Maps a floating-point input range into the integer parameter range using `Micro::map<float>()`.
    - **Parameters:**
      - in – Input value to map (float)
      - inMin – Lower bound of the input range (float)
      - inMax – Upper bound of the input range (float)
    - **Returns:** None (void)
  - MicroParamInt::mapInt(in, inMin, inMax) – Maps an integer input range into the integer parameter range using `Micro::map<int32_t>()`.
    - **Parameters:**
      - in – Input value to map (32-bit signed integer)
      - inMin – Lower bound of the input range (32-bit signed integer)
      - inMax – Upper bound of the input range (32-bit signed integer)
    - **Returns:** None (void)

### MicroParamFloat
- Concrete implementation of `MicroParam` for floating-point-backed parameters with clamping and range mapping.
- Members:
  - value_ – Current stored value (float)
  - min_ – Minimum allowed value (float)
  - max_ – Maximum allowed value (float)
- Constructors:
  - MicroParamFloat(initial, min, max) – Creates a floating-point parameter with an initial value and bounds (all parameters are floats)
- Methods:
  - MicroParamFloat::setFloat(f) – Sets the floating-point value, clamped to the configured range using `Micro::clamp<float>()`.
    - **Parameters:**
      - f – Value to assign (float)
    - **Returns:** None (void)
  - MicroParamFloat::getFloat() – Retrieves the stored floating-point value.
    - **Returns:** Current value (float)
  - MicroParamFloat::setInt(i) – Sets the value from an integer by converting to float and clamping.
    - **Parameters:**
      - i – Value to assign (32-bit signed integer)
    - **Returns:** None (void)
  - MicroParamFloat::getInt() – Retrieves the stored value as an integer by flooring.
    - **Returns:** Current value (32-bit signed integer)
  - MicroParamFloat::mapFloat(in, inMin, inMax) – Maps a floating-point input range into the float parameter range using `Micro::map<float>()`.
    - **Parameters:**
      - in – Input value to map (float)
      - inMin – Lower bound of the input range (float)
      - inMax – Upper bound of the input range (float)
    - **Returns:** None (void)
  - MicroParamFloat::mapInt(in, inMin, inMax) – Maps an integer input range into the float parameter range using `Micro::map<int32_t>()`.
    - **Parameters:**
      - in – Input value to map (32-bit signed integer)
      - inMin – Lower bound of the input range (32-bit signed integer)
      - inMax – Upper bound of the input range (32-bit signed integer)
    - **Returns:** None (void)

### MicroParamBind
- Type alias representing a binding between a key and a `MicroParam` pointer.
- **Template Parameter:** MicroParam* – Pointer to a parameter instance

### MicroParamBinder
- Type alias providing a binder utility for `MicroParam` pointers.
- **Template Parameter:** MicroParam* – Pointer to a parameter instance
