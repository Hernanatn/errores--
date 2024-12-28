## `Errores--`

`Errores--` is a C++ library that provides generic abstractions for error handling, optional values, and operation results. The library implements three fundamental abstractions: `Error`, `Opcion<T>`, and `Resultado<T>`. This library provides a consistent and flexible way to manage control flow in situations where it is necessary to handle errors explicitly -avoiding breaks in the control flow through exceptions- and manage the presence or absence of values safely. `Errores--` is inspired by the error handling as values of `Go` and the containers `Option<T>` and `Result<T>` of `Rust`. The library adheres to the **Zero Is Initialization - ZII** paradigm and focuses on enforcing safe practices with clear memory ownership semantics.

[![Made by Chaska](https://img.shields.io/badge/made_by-Chaska-303030.svg)](https://cajadeideas.ar)
[![Version: Alpha v0.2.0](https://img.shields.io/badge/version-Alpha_v0.2.0-orange.svg)](https://github.com/hernanatn/github.com/hernanatn/aplicacion.go/releases/latest)
[![C++ Version: 20](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus)](https://isocpp.org/std/the-standard)
![Packaging](https://img.shields.io/github/actions/workflow/status/Hernanatn/errores--/empaquetar.yml?label=bundle)
[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC_BY--NC_4.0-lightgrey.svg)](LICENSE)

1. [Introduction](#errores--)
    - [Usage](#usage)
2. [New in v0.2.0-alpha](#new-in-v020-alpha)
3. [Memory Management](#memory-management)
   - [Supported Data Types](#supported-data-types)
   - [Memory Restrictions](#memory-restrictions)
4. [Main Abstractions](#main-abstractions)
   - [Error](#error)
   - [Opcion\<T>](#opciont)
   - [Resultado\<T>](#resultadot)
5. [Documentation](#documentation)
6. [Requirements](#requirements)
7. [Tests](#tests)
8. [Project Status](#project-status)
9. [Examples](#examples)
   - [Example of Idiomatic Usage](#example-of-idiomatic-usage)
   - [Handling Direct Values](#handling-direct-values)
   - [Handling Smart Pointers](#handling-smart-pointers)
   - [Error Handling](#error-handling)
10. [License](#license)

### Usage

The library can be included in a project by downloading and `#including` the header file
[`errores--.hpp`](/errores--.hpp)

```cpp
#include "errores--.hpp"
```

## New in v0.2.0-alpha

This version introduces significant improvements in memory management and type safety:

  - Implementation of template specializations for `Opcion<T>` and `Resultado<T>` that safely handle direct values, raw pointers, and smart pointers.
  - Use of C++20 concepts for more expressive and secure type restrictions.
  - Complete test suite implemented with Catch2.
  - Improved move semantics for non-copyable types.
  - Expanded documentation with usage examples.

## Memory Management

The library implements strict rules to ensure memory safety. Both `Opcion<T>` and `Resultado<T>` assume ownership of their members until they are *consumed*. Memory restrictions are applied according to the data type:

### Supported Data Types

1.  **Direct Values**

      - Fundamental types (`int`, `double`, etc.)
      - Standard library types (`std::string`, etc.)
      - User-defined types that support default construction

2.  **Smart Pointers**

      - `std::unique_ptr<T>`
      - `std::shared_ptr<T>`

3.  **Raw Pointers**

      - Must be owned exclusively by the instance
      - Are automatically released when the instance is destroyed
      - Ownership is transferred when consuming the value

### Memory Restrictions

  - Raw pointers must be owned exclusively by the instance.
  - The use of raw pointers pointing to memory managed by `std::shared_ptr` is not allowed.
  - Ownership transfer is done using `std::move` for smart pointers.
  - Raw pointers are transferred using `std::exchange`.

## Main Abstractions

### Error

The `Error` class encapsulates information about errors using a status code and a descriptive message. This abstraction allows to clearly identify the status of an operation (success, error or fatal) and is extensible for specific cases. [Full reference](/documentation/Error.md).

#### Methods

  - `CodigoEstado Codigo()`: Returns the status code of the error.
  - `std::string Mensaje()`: Returns the error message.
  - `void agregarMensaje(std::string mensaje)`: Adds additional text to the message.
  - `operator bool()`: Returns true if there is an error (status is not SUCCESS).
  - `operator std::string()`: Converts the error to its string representation.
  - `operator const char*()`: Converts the message to a C-style string.
  - `operator char*()`: Converts the message to a modifiable C-style string.
  - `operator<<`: Allows printing the error to output streams.

#### Utility Functions

```cpp
namespace err {
    inline Error Exito(std::string mensaje = "Exito");
    inline Error Fatal(std::string mensaje = "Error Fatal");
    inline Error Generico(std::string mensaje = "Error");
}
```

### Opcion<T>

`Opcion<T>` is a generic container that represents an optional value that may be present or absent. It implements specializations to handle different data types safely. [Full reference](/documentation/Opcion.md).

#### Methods

  - `bool estaVacia() const noexcept`: Indicates if the option is empty.
  - `std::tuple<T, bool> Consumir() noexcept`: Returns a tuple with the value (if it exists) and a success indicator.
  - `operator bool()`: Returns true if the option contains a value.
  - `operator()()`: Alias for Consumir().

#### Specializations

1.  **Direct Values**

      - Handles value types (int, std::string, etc.)
      - Initialization to zero when empty

2.  **Raw Pointers** (T\*)

      - Assumes exclusive ownership of the pointed memory
      - Automatically frees the memory if it is not consumed
      - Copy constructor and operator deleted
      - Move semantics using std::exchange

3.  **Smart Pointers** (std::unique\_ptr\<T\>, std::shared\_ptr\<T\>)

      - Move semantics for ownership transfer
      - nullptr when empty
      - Copy constructor and operator deleted

### Resultado<T>

`Resultado<T>` encapsulates the result of an operation that can be successful or fail, combining a value of type `T` with an `Error`. [Full reference](/documentation/Resultado.md)

#### Methods

  - `err::Error Error() const noexcept`: Returns the error status.
  - `std::tuple<T, err::Error> Consumir() noexcept`: Returns a tuple with the value and the error.
  - `operator bool()`: Returns true if the operation was successful.
  - `operator()()`: Alias for Consumir().

#### Specializations

1.  **Direct Values**

      - Returns value initialized to zero in case of error
      - Copies the value in case of success

2.  **Raw Pointers** (T\*)

      - Assumes exclusive ownership of the pointed memory
      - Returns nullptr in case of error
      - Frees memory if not consumed
      - Move semantics using std::exchange

3.  **Smart Pointers** (std::unique\_ptr\<T\>, std::shared\_ptr\<T\>)

      - Move semantics for ownership transfer
      - Returns nullptr in case of error

## Documentation

The reference for the abstractions, a more detailed explanation of memory management with `errores--` and more examples can be found in [/documentation](/documentation)

## Requirements

  - C++20 compatible compiler
  - CMake and Catch2 (to run the tests)

## Tests

The library includes a (still in progress) test suite implemented with Catch2, which verifies the correct behavior of all the abstractions and their specializations. You can run the provided file ([/tests/run\_tests.exe](/tests/tests.exe)) or [compile the tests from scratch](/documentation/Tests.md)

## Project Status

This version (v0.2.0-alpha) represents a significant advance in terms of security and functionality, but is still considered to be in alpha state. The API may change in future versions.

## Examples

### Example of Idiomatic Usage

```cpp
#include "errores--.hpp"

// Function that can fail returning a Resultado
res::Resultado<float> dividir(float a, float b) {
    if (b == 0.0f) {
        return res::Resultado<float>(0.0f, err::ERROR, "División por cero"); // Returns the error with a message
    }
    return res::Resultado<float>(a / b);
}

// Function that can return an optional value
opc::Opcion<int> obtenerConfiguracion(const std::string& clave) {
    if (configuraciones.count(clave) == 0) {
        return opc::Opcion<int>(); // Return an empty option
    }
    return opc::Opcion<int>(configuraciones[clave]);
}

// Combined use of the abstractions
void ejemplo() {
    // Use of Resultado
    auto [resultado, error] = dividir(10.0f, 2.0f)(); 
    if (error) {
        std::cout << "Error dividing: " << error.Mensaje();
        return; // Early handling of the error
    }
    
    // Use of Opcion
    auto opcion = obtenerConfiguracion("max_conexiones");
    auto [valor, ok] = opcion();
    if (!ok) {
        // Handle the absence of a value
        valor = 10; // Default value
    }
    
    // Continue with normal logic
    std::cout << "Result: " << resultado << ", Setting: " << valor << std::endl;
}
```

### Handling Direct Values

```cpp
// Example with direct values
opc::Opcion<int> optInt(42);
auto [valor, ok] = optInt();
if (ok) {
    std::cout << "Value: " << valor << "\n";
}
```

### Handling Smart Pointers

```cpp
// Example with unique_ptr
auto ptr = std::make_unique<MiClase>(42);
res::Resultado<std::unique_ptr<MiClase>> resultado(std::move(ptr));
auto [valor, error] = resultado();
if (!error) {
    valor->procesar();
}
```

### Error Handling

```cpp
// Example of a function that returns a Resultado
res::Resultado<float> dividir(float a, float b) {
    if (b == 0) {
        return res::Resultado<float>(0, err::Error(err::ERROR, "División por cero"));
    }
    return res::Resultado<float>(a / b);
}

auto [resultado, error] = dividir(10.0f, 2.0f)(); 
if (error) {
    //handle error, save state or return early
}
//continue
```

## License

This project is licensed under CC BY-SA 4.0. See the LICENSE file for more details.