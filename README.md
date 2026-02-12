# MicroParam

Micro (small, simple, and lightweight) library to manage parameters.

> [!WARNING]
> This project is currently a work in progress.
> The API may change between releases.

## Design Goals and Philosophy

### Fast

Reading and writing parameter values should be very fast—ideally as fast as direct manipulation of variables without a parameter library. Therefore, using this library should not impact the speed of internal code. An integer or a float in code should be replaceable by a parameter without any performance drawback.

### Lightweight

Parameters should not carry string names internally for several reasons:
- **Memory efficiency** – By omitting names, each parameter uses less memory.
- **Controlled exposure** – Names are only needed when parameters are exposed externally (e.g., via bindings). This allows selective exposure of parameters, keeping internal data private and clean.

Names should only be assigned **when binding parameters**, because that is the only time they are needed—for external interfaces, UI exposure, or network interaction. Internal operations must remain fast and lightweight, without the overhead of storing or searching names.

## MicroParam types

All `MicroParam` inherit the base class `MicroPram`

| Type   | Constructor | Description |
|--------|-------------|-------------|
| `MicroParamByte` | `MicroParamByte(uint8_t v, uint8_t min, uint8_t max)` | Stores a `uint8_t` with an initial value `v` constrained between `min` and `max`. |
| `MicroParamInt` | `MicroParamInt(int32_t initial, int32_t min, int32_t max)` | Stores a signed 32-bit integer with a `initial` value constrained between `min` and `max`. Supports integer and float assignment with automatic clamping. |
| `MicroParamFloat` | `MicroParamFloat(float initial, float min, float max)` | Stores a floating point value with a `initial` value constrained between `min` and `max`. Supports assignment from integers and floats with automatic clamping. |
| `MicroParamEnum` | `MicroParamEnum(const char **labels, int32_t value, int32_t count)` | Stores an enumerated value represented by an index into a list of string labels. Values wrap using modulo within the available label count. |
| `MicroParamBlob` | `MicroParamBlob(uint8_t *buffer, uint32_t capacity, uint32_t length)` | Accesses `uint8_t` data from a user-provided buffer with a length limited by a maximum capacity. |
| `MicroParamString` | `MicroParamString(char *buffer, uint32_t capacity)` | Accesses a null-terminated string from a user-provided buffer limited by fixed maximum capacity. |

## Common methods to all MicroParam

| Method | Return type | Description | Supported by |
|--------|-------------|-------------|-------------|
| `getType()` | `MicroParam::Type` | Returns the parameter type. | All |
| `setInt(int32_t v)` | `void` | Sets the parameter value from an integer. | int, float, enum | 
| `getInt() const` | `int32_t` | Returns the parameter value as an integer. Conversion depends on the concrete parameter type. | int, float, enum | 
| `setFloat(float v)` | `void` | Sets the parameter value from a float. Conversion and clamping depend on the concrete parameter type. | int, float, enum | 
| `getFloat() const` | `float` | Returns the parameter value as a float. Conversion depends on the concrete parameter type. | int, float, enum | 
| `setString(const char *s)` | `void` | Sets the parameter value from a string when supported. Default implementation does nothing. | enum, string | 
| `getString() const` | `const char *` | Returns the parameter value as a string when supported. Default implementation returns `nullptr`. | enum, string | 
| `mapFloat(float in, float inMin, float inMax)` | `void` | Maps a float input range to the parameter range and assigns the mapped value. | int, float, enum | 
| `mapInt(int32_t in, int32_t inMin, int32_t inMax)` | `void` | Maps an integer input range to the parameter range and assigns the mapped value. | int, float, enum | 

## MicroParam fast methods

> [!NOTE]  
> These methods can only be accessed directly on the class and not on the parent `MicroParam` class.

For `MicroParamByte`, `MicroParamInt`, `MicroParamFloat` and `MicroParamEnum`:

| Fast Method | Description |
|-------------|-------------|
| `=` | Assigns a value directly to the parameter with automatic conversion and clamping or wrapping depending on the type. Enables fast syntax such as `param = value;` or `value = param`. |

For `MicroParamBlob`:

| Fast Method | Description |
|-------------|-------------|
| `[index]` | Direct indexed access to the internal buffer for reading or writing individual bytes. No bounds checking. |

For `MicroParamString`:

| Fast Method | Description |
|-------------|-------------|
| `set(cstring)` | Copies a C-string into the internal buffer up to capacity. |
| `get()` | Returns a pointer to the internal null-terminated string buffer. |



## Tests

To determine the ideal architecture, five models were tested:

- **`Direct`**: Direct manipulation of variables. This is the baseline for reading and writing values, but it does not test binding. Obviously, raw variables do not share a common interface.
- **`Virtual`**: Parameters are defined as classes with virtual functions that distinguish each data type. All parameters share a common interface.
- **`Pointer`**: A generic class uses custom function pointers for each data type. All parameters share a common interface.
- **`Unique`**: Unique classes are defined for each data type. A common interface is only accessed when binding.
- **`Overload`**: A variation of `Unique` with overloads for the `=` operator.
- **`VirtualOverload`**: A variation of `Virtual` with overloads for the `=` and conversion operators of the derived class (not the base class).

Two types of tests were conducted:
- The **internal test** involved reading and changing the value of a parameter by accessing it directly.
- The **external test** involved reading and changing the value of a parameter by accessing it through a binding object that grouped parameters.
- A total of **200,000 iterations** were performed for each test.
- Each test involved **six parameters**: two integers, two floats, and two enums.

Here was how most of the models created the parameters:
```cpp
// ---------------------- Values ----------------------
const char* labels[3] = { "A", "B", "C" };
MicroParamInt i1{5, 0, 127};
MicroParamInt i2{10, 0, 127};
MicroParamFloat f1{0.5f, 0.0f, 1.0f};
MicroParamFloat f2{0.25f, 0.0f, 1.0f};
MicroParamEnum e1{0, 3, labels};
MicroParamEnum e2{0, 3, labels};


MicroParam params[] = {MicroParam::bind("i1", i1), MicroParam::bind("i2", i2),
                       MicroParam::bind("f1", f1), MicroParam::bind("f2", f2),
                       MicroParam::bind("e1", e1), MicroParam::bind("e2", e2)};

constexpr size_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);
```

The test code can be found in the `tests` folder.

### Results

**Real world** tests :

| Model           | External (M Cycles) | Internal (M Cycles) | Heap left |
|-----------------|---------------------|---------------------|-----------|
| Direct          | N/A                 | 9.435226            | 336408    |
| Overload        | 37.338829           | 14.854615           | 336280    |
| VirtualOverload | 83.314171           | 14.854669           | 336288    |
| Unique          | 49.98629            | 27.50039            | 336288    |
| Virtual         | 89.348789           | 36.339528           | 336288    |
| Pointer         | 107.20746           | 74.090209           | 336216    |

The **VirtualOverload** model was selected as it gives excellent internal speed and allows for the best balance between polymorphic flexibility and overall performance, without a meaningful increase in memory usage.

Note that the previous **real world** tests differ from **ChatGPT approximations** that were the following :

| Model | Relative Speed | Approx. Cost vs Direct | Notes |
|---------:|---------------:|-----------------------:|-------|
| Direct   | 100%           | 1.0×                   | Raw variable access |
| Unique   | ~98–100%       | ~1.0–1.02×             | Optimizes to direct access |
| Overload | ~97–99%        | ~1.01–1.03×            | Minor syntactic overhead |
| Pointer  | ~85–90%        | ~1.1–1.18×             | Function pointer indirection |
| Virtual  | ~70–75%        | ~1.3–1.4×              | Virtual dispatch cost |
