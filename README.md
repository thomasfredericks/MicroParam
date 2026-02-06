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

## Structs and Classes

### `MicroParamInt`

| Member | Description | Returns |
|--------|-------------|---------|
| Constructor `MicroParamInt(v, min_, max_)` | Initializes a `MicroParamInt` with a value clamped to `[min_, max_]`. `v` (`int32_t`) is the initial value, `min_` (`int32_t`) is the minimum, `max_` (`int32_t`) is the maximum. | — |
| `set(v)` | Sets the value clamped to `[min, max]`. `v` (`int32_t`) is the new value. | — |
| `get()` | Retrieves the current value. | Current value (`int32_t`) |
| Assignment `= v` | Sets the value clamped to the range. `v` (`int32_t`) is the value to assign. | — |
| Conversion `operator int32_t()` | Converts to raw integer value. | Current value (`int32_t`) |
| Increment/decrement `++x, x++, --x, x--` | Modifies the value within the valid range. | — |


### `MicroParamFloat`

| Member | Description | Returns |
|--------|-------------|---------|
| Constructor `MicroParamFloat(v, min_, max_)` | Initializes a `MicroParamFloat` with a value clamped to `[min_, max_]`. `v` (`float`) is the initial value, `min_` (`float`) is the minimum, `max_` (`float`) is the maximum. | — |
| `set(v)` | Sets the value clamped to `[min, max]`. `v` (`float`) is the new value. | — |
| `get()` | Retrieves the current value. | Current value (`float`) |
| Assignment `= v` | Sets the value clamped to the range. `v` (`float`) is the value to assign. | — |
| Conversion `operator float()` | Converts to raw float value. | Current value (`float`) |



### `MicroParamEnum`

| Member | Description | Returns |
|--------|-------------|---------|
| Constructor `MicroParamEnum(v, count_, labels_)` | Initializes an enum value wrapped modulo `count_`. `v` (`int32_t`) is the initial value, `count_` (`int32_t`) is the number of enum items, `labels_` (`const char **`) is the array of string labels. | — |
| `set(v)` | Sets the value wrapped modulo `count`. `v` (`int32_t`) is the new value. | — |
| `get()` | Retrieves the current enum value. | Current value (`int32_t`) |
| `label()` | Gets the label string corresponding to the current value. | Label string (`const char *`) or `nullptr` if labels are null |
| Assignment `= v` | Sets the value modulo `count`. `v` (`int32_t`) is the value to assign. | — |
| Conversion `operator int32_t()` | Converts to raw integer value. | Current value (`int32_t`) |


### `MicroParamBind`

| Member | Description | Returns |
|--------|-------------|---------|
| Constructor `MicroParamBind(k, v)` | Creates a binding between a key and a variable of type `MicroParamInt`, `MicroParamFloat`, or `MicroParamEnum`. `k` (`const char *`) is the key string, `v` (`MicroParamInt &`, `MicroParamFloat &`, or `MicroParamEnum &`) is the variable to bind. | — |
| `getType()` | Retrieves the type of the bound variable. | Type (`MicroParamBind::Type`) |
| `getKey()` | Retrieves the key string. | Key (`const char *`) |
| `checkKey(k)` | Checks if the key matches the provided string. `k` (`const char *`) is the key string to check. | True if keys match (`bool`) |
| `setInt(v)` | Sets the value of the bound variable as an integer. `v` (`int32_t`) is the new value. | — |
| `getInt()` | Retrieves the bound variable as an integer. | Value as integer (`int32_t`) |
| `setFloat(v)` | Sets the value of the bound variable as a float. `v` (`float`) is the new value. | — |
| `getFloat()` | Retrieves the bound variable as a float. | Value as float (`float`) |


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
