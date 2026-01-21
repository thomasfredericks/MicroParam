# MicroParam

Micro (small, simple and lightweight) library to manage parameters.

> [!WARNING] 
> The project is currently highly a work in progress. 
> The API might change between releases.

## Design Goals and Philosophy 

### Fast

Reading and writing of parameter values should be done very fast. This should ideally be as fast as direct manipulation of variables without the parameter library. Therefore, the use of this library should not impact the speed of internal code. An integer or a float in code could be replaced by a parameter without any drawback.

### Lightweight

Parameters should not carry string names internally for several reasons:
- Memory Efficiency – By omitting names, each parameter uses less memory. 
- Controlled Exposure – Names are only needed when parameters are exposed externally (e.g., via bindings). This allows you to selectively expose only the parameters you want, keeping internal data private and clean.

Names should only be assigned **when binding parameters** because that is the only time they are needed—for external interfaces, UI exposure, or network interaction. Internal operations must remain fast and lightweight, without the overhead of storing or searching names.


## Tests

The determine the ideal architecture, 4 scenarios where tested:

- `Direct`: Direct manipulation of variables. This is the baseline for reading and accessing variables but it does not test binding. Obvisouly, raw variables do not share a common interface.
- `Virtual`: Parameters are defined as classes with virtual functions that distinguish each data type. All parameters share a common interface.
- `Pointer`: A generic class uses custom function pointers for each data type. All parameters share a common interface.
- `Uniqe`: There are unique classes for each data type. Only when binding can a common interface be accessed.
- `Overload`: A variation of `Unique` but with overloads for the `=` operator.


Two types of test where conducted :
- The internal test involved reading and changing the value of a paramater by accessing the paramater directly.
- The external test involved reading and changing the value of a paramater by accessing the paramater through a binding object that grouped them.
- A total of 200000 iterations was done for each.

### Results

| Test | External (M Cycles) | Internal (M Cycles) | Heap left | 
| --- | --- | --- | --- |
| Direct   | N/A                 | 9.435226 | 336408    |
| Overload | 37.338829           | 14.854615| 336280    |
| Unique   | 49.98629            | 27.50039 | 336288    |
| Virtual  | 89.348789           | 36.339528| 336288    |
| Pointer  | 107.20746           | 74.090209| 336216    |

