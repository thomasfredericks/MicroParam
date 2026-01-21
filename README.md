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

## Tests

To determine the ideal architecture, four scenarios were tested:

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

### Results

| Test     | External (M Cycles) | Internal (M Cycles) | Heap left |
|----------|---------------------|---------------------|-----------|
| Direct   | N/A                 | 9.435226            | 336408    |
| Overload | 37.338829           | 14.854615           | 336280    |
| Unique   | 49.98629            | 27.50039            | 336288    |
| Virtual  | 89.348789           | 36.339528           | 336288    |
| Pointer  | 107.20746           | 74.090209           | 336216    |

Note that the previous real world tests differ from ChatGPT approximations that were the following :

| Test | Relative Speed | Approx. Cost vs Direct | Notes |
|---------:|---------------:|-----------------------:|-------|
| Direct   | 100%           | 1.0×                   | Raw variable access |
| Unique   | ~98–100%       | ~1.0–1.02×             | Optimizes to direct access |
| Overload | ~97–99%        | ~1.01–1.03×            | Minor syntactic overhead |
| Pointer  | ~85–90%        | ~1.1–1.18×             | Function pointer indirection |
| Virtual  | ~70–75%        | ~1.3–1.4×              | Virtual dispatch cost |
