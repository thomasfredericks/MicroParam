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
Represents an integer value constrained within a fixed range.

#### Constructors

```cpp
MicroParamInt myMicroParamInt(v, min_, max_);
```
Initializes a `MicroParamInt` with a value clamped to `[min_, max_]`.

- Parameters:
  - `v`: Initial value (`int32_t`)
  - `min_`: Minimum value (`int32_t`)
  - `max_`: Maximum value (`int32_t`)

#### Method `set(v);`
```cpp
myMicroParamInt.set(v);
```
Sets the value, clamped to `[min, max]`.

- Parameters:
  - `v`: New value (`int32_t`)

#### Method `get();`
```cpp
int32_t value = myMicroParamInt.get();
```
Retrieves the current value.

- Returns:
  - Current value (`int32_t`)

#### Operators
- Assignment from `int32_t`:
```cpp
myMicroParamInt = v;
```
Sets the value clamped to the range.

- Conversion to `int32_t`:
```cpp
int32_t value = myMicroParamInt;
```

- Increment/decrement operators:
  - Prefix `++x`, postfix `x++`, prefix `--x`, postfix `x--`  
  All modify the value within the valid range.

---

### `MicroParamFloat`
Represents a floating-point value constrained within a fixed range.

#### Constructors

```cpp
MicroParamFloat myMicroParamFloat(v, min_, max_);
```
Initializes a `MicroParamFloat` with a value clamped to `[min_, max_]`.

- Parameters:
  - `v`: Initial value (`float`)
  - `min_`: Minimum value (`float`)
  - `max_`: Maximum value (`float`)

#### Method `set(v);`
```cpp
myMicroParamFloat.set(v);
```
Sets the value, clamped to `[min, max]`.

- Parameters:
  - `v`: New value (`float`)

#### Method `get();`
```cpp
float value = myMicroParamFloat.get();
```
Retrieves the current value.

- Returns:
  - Current value (`float`)

#### Operators
- Assignment from `float`:
```cpp
myMicroParamFloat = v;
```
Sets the value clamped to the range.

- Conversion to `float`:
```cpp
float value = myMicroParamFloat;
```

---

### `MicroParamEnum`
Represents an enumerated value with labels, wrapped modulo the number of items.

#### Constructors

```cpp
MicroParamEnum myMicroParamEnum(v, count_, labels_);
```
Initializes an enum value wrapped modulo `count_`.

- Parameters:
  - `v`: Initial value (`int32_t`)
  - `count_`: Number of enum items (`int32_t`)
  - `labels_`: Array of string labels (`const char **`)

#### Method `set(v);`
```cpp
myMicroParamEnum.set(v);
```
Sets the value wrapped modulo `count`.

- Parameters:
  - `v`: New value (`int32_t`)

#### Method `get();`
```cpp
int32_t value = myMicroParamEnum.get();
```
Retrieves the current enum value.

- Returns:
  - Current value (`int32_t`)

#### Method `label();`
```cpp
const char* label = myMicroParamEnum.label();
```
Gets the label string corresponding to the current value.

- Returns:
  - Label string (`const char *`), or `nullptr` if labels are null

#### Operators
- Assignment from `int32_t`:
```cpp
myMicroParamEnum = v;
```
Sets the value modulo `count`.

- Conversion to `int32_t`:
```cpp
int32_t value = myMicroParamEnum;
```

---

### `MicroParamBind`
Binds a `MicroParamInt`, `MicroParamFloat`, or `MicroParamEnum` to a key for generic access.

#### Constructors

```cpp
MicroParamBind myMicroParamBind(k, v);
```
Creates a binding between a key and a variable of type `MicroParamInt`, `MicroParamFloat`, or `MicroParamEnum`.

- Parameters:
  - `k`: Key string (`const char *`)
  - `v`: Variable to bind (`MicroParamInt &`, `MicroParamFloat &`, or `MicroParamEnum &`)

#### Method `getType();`
```cpp
MicroParamBind::Type type = myMicroParamBind.getType();
```
Retrieves the type of the bound variable.

- Returns:
  - Type (`MicroParamBind::Type`)

#### Method `getKey();`
```cpp
const char* key = myMicroParamBind.getKey();
```
Retrieves the key string.

- Returns:
  - Key (`const char *`)

#### Method `checkKey(k);`
```cpp
bool match = myMicroParamBind.checkKey(k);
```
Checks if the key matches the provided string.

- Parameters:
  - `k`: Key string to check (`const char *`)
- Returns:
  - True if keys match (`bool`)

#### Method `setInt(v);`
```cpp
myMicroParamBind.setInt(v);
```
Sets the value of the bound variable as an integer.

- Parameters:
  - `v`: New value (`int32_t`)

#### Method `getInt();`
```cpp
int32_t value = myMicroParamBind.getInt();
```
Retrieves the bound variable as an integer.

- Returns:
  - Value as integer (`int32_t`)

#### Method `setFloat(v);`
```cpp
myMicroParamBind.setFloat(v);
```
Sets the value of the bound variable as a float.

- Parameters:
  - `v`: New value (`float`)

#### Method `getFloat();`
```cpp
float value = myMicroParamBind.getFloat();
```
Retrieves the bound variable as a float.

- Returns:
  - Value as float (`float`)



## Tests

To determine the ideal architecture, five models were tested:

- **`Direct`**: Direct manipulation of variables. This is the baseline for reading and writing values, but it does not test binding. Obviously, raw variables do not share a common interface.
- **`Virtual`**: Parameters are defined as classes with virtual functions that distinguish each data type. All parameters share a common interface.
- **`Pointer`**: A generic class uses custom function pointers for each data type. All parameters share a common interface.
- **`Unique`**: Unique classes are defined for each data type. A common interface is only accessed when binding.
- **`Overload`**: A variation of `Unique` with overloads for the `=` operator.

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

| Model     | External (M Cycles) | Internal (M Cycles) | Heap left |
|----------|---------------------|---------------------|-----------|
| Direct   | N/A                 | 9.435226            | 336408    |
| Overload | 37.338829           | 14.854615           | 336280    |
| Unique   | 49.98629            | 27.50039            | 336288    |
| Virtual  | 89.348789           | 36.339528           | 336288    |
| Pointer  | 107.20746           | 74.090209           | 336216    |

Note that the previous **real world** tests differ from ChatGPT approximations that were the following :

| Model | Relative Speed | Approx. Cost vs Direct | Notes |
|---------:|---------------:|-----------------------:|-------|
| Direct   | 100%           | 1.0×                   | Raw variable access |
| Unique   | ~98–100%       | ~1.0–1.02×             | Optimizes to direct access |
| Overload | ~97–99%        | ~1.01–1.03×            | Minor syntactic overhead |
| Pointer  | ~85–90%        | ~1.1–1.18×             | Function pointer indirection |
| Virtual  | ~70–75%        | ~1.3–1.4×              | Virtual dispatch cost |
