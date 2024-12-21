## `Errores--`

`Errores--` es una librería de C++ que facilita el manejo de errores, valores opcionales y resultados de operaciones mediante tres tipos principales: `Error`, `Opcion` y `Resultado`. Esta librería proporciona una forma coherente y flexible de gestionar el flujo de control en situaciones donde es necesario manejar errores de manera explícita -evitando rupturas en el flujo de control a través de excepciones- y gestionar la presencia o ausencia de valores de forma segura.

Está inspirada en el manejo de errores como valores de `Go` y en los contenedores Option<T> y Result<T> de `Rust`.

La librería se presenta aún en versión alpha - distribución `v 0.0.1-alpha`, no está particularmente optimizada y puede presentar errores.

[![Hecho por Chaska](https://img.shields.io/badge/hecho_por-Ch'aska-303030.svg)](https://cajadeideas.ar)
[![Versión: Beta v0.1](https://img.shields.io/badge/version-Alpha_v0.0.1-orange.svg)](https://github.com/hernanatn/github.com/hernanatn/aplicacion.go/releases/latest)
[![Verisón de C++: 17](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)](https://go.dev/doc/go1.22)
[![Licencia: CC BY-NC 4.0](https://img.shields.io/badge/Licencia-CC_BY--SA_4.0-lightgrey.svg)](LICENSE)


---

## Índice

1. [Errores--](#errores)
    - Descripción general
2. [Tipos Principales](#tipos-principales)
    - [Error](#error)
        - Descripción
        - Constructores
        - Funciones
        - Ejemplo de uso
    - [Opcion](#opcion)
        - Descripción
        - Constructores
        - Funciones
        - Ejemplo de uso
    - [Resultado](#resultado)
        - Descripción
        - Constructores
        - Funciones
        - Ejemplo de uso
3. [Ejemplos Completos](#ejemplos-completos)
    - [Ejemplo 1: Manejo de Errores](#ejemplo-1-manejo-de-errores)
    - [Ejemplo 2: Uso de `Opcion` (Valores Opcionales)](#ejemplo-2-uso-de-opcion-valores-opcionales)
    - [Ejemplo 3: Resultados de Operaciones con `Resultado`](#ejemplo-3-resultados-de-operaciones-con-resultado)
    - [Ejemplo 4: Integración de `Errores--` en un Proyecto Completo](#ejemplo-4-integracion-de-errores-en-un-proyecto-completo)
4. [Conclusiones](#conclusiones)

---

### Tipos Principales

#### `Error`

El tipo `Error` encapsula un código de estado y un mensaje, lo que permite representar el estado de una operación (ya sea exitosa o fallida).

- **`Error`**: Representa un error con un código de estado y un mensaje descriptivo.
- **`CodigoEstado`**: Enum que define los códigos de estado posibles para un error:
    - `FATAL`: Error fatal.
    - `ERROR`: Error genérico.
    - `EXITO`: Indica que la operación fue exitosa.

**Constructores**:
- `Error(CodigoEstado codigo, std::string mensaje = "ERROR")`: Inicializa un error con un código y un mensaje.
- `Error(Error *e)`:  **aún no implementado.**

**Funciones**:
- `CodigoEstado Codigo()`: Retorna el código de estado del error.
- `std::string Mensaje()`: Retorna el mensaje asociado con el error.
- `void agregarMensaje(std::string mensaje)`: Agrega un mensaje adicional al error.
- Sobrecarga de operadores para conversión a `bool`, `std::string`, y `const char*`.

**Ejemplo de uso**:
```cpp
Error e = Error::Generico();
std::cout << "Error: " << e << std::endl;
```

#### `Opcion`

El tipo `Opcion` representa un valor opcional de tipo `T`, el cual puede o no estar presente.

- **`Opcion<T>`**: Una estructura que puede almacenar un valor de tipo `T` o estar vacía.
- **Operador `Consumir()`**: Extrae el valor de la opción si está presente, de lo contrario devuelve un valor por defecto y un indicador de validez.

**Constructores**:
- `Opcion() noexcept`: Crea una opción vacía.
- `Opcion(T data) noexcept`: Crea una opción con un valor específico.

**Funciones**:
- `bool estaVacia() const noexcept`: Indica si la opción está vacía.
- `std::tuple<T, bool> Consumir() noexcept`: Extrae el valor de la opción y un indicador de si es válido.
- Sobrecarga de operador `()` para usar `Consumir()`.

**Ejemplo de uso**:
```cpp
Opcion<int> opcion = Opcion<int>(42);
auto [valor, valido] = opcion.Consumir();
if (valido) {
    std::cout << "Valor obtenido: " << valor << std::endl;
} else {
    std::cout << "No se obtuvo un valor válido." << std::endl;
}
```

#### `Resultado`

El tipo `Resultado` representa el resultado de una operación, que puede contener un valor de tipo `T` o un error asociado.

- **`Resultado<T>`**: Una estructura que contiene un valor de tipo `T` y un posible error de tipo `Error`.
- **Operador `Consumir()`**: Extrae el valor y el error asociados a la operación.

**Constructores**:
- `Resultado() noexcept`: Crea un resultado exitoso con un valor por defecto.
- `Resultado(T data) noexcept`: Crea un resultado con un valor específico.
- `Resultado(T data, Error::CodigoEstado codigo, std::string mensaje) noexcept`: Crea un resultado con un valor y un error específico.

**Funciones**:
- `Error::Error Error() const noexcept`: Retorna el error asociado con el resultado.
- `std::tuple<T, Error::Error> Consumir() noexcept`: Extrae el valor y el error.
- Sobrecarga de operador `()` para usar `Consumir()`.

**Ejemplo de uso**:
```cpp
Resultado<int> resultado = Resultado<int>(42);
auto [valor, error] = resultado.Consumir();
if (error) {
    std::cout << "Error: " << error << std::endl;
} else {
    std::cout << "Resultado de la operación: " << valor << std::endl;
}
```
## Ejemplos Completos

A continuación se presentan algunos ejemplos completos de uso de la librería `Errores--`, ilustrando cómo manejar errores, valores opcionales y resultados de operaciones.

### Ejemplo 1: Manejo de Errores

En este ejemplo se muestra cómo se pueden utilizar los tipos de error para manejar distintas situaciones en el flujo de un programa.

```cpp
#include "Errores.hpp"

Error::Error procesarArchivo(const std::string& nombreArchivo) {
    if (nombreArchivo.empty()) {
        // Si el nombre del archivo está vacío, se genera un error
        Error error = Error::Generico("El nombre del archivo está vacío");
        return error;
    }

    // Supongamos que el archivo fue procesado exitosamente
    Error exito = Error::Exito("Operación exitosa. Archivo leído");
    std::cout << "Operación exitosa: " << exito << std::endl;
    return exito;
}

int main() {
    e = procesarArchivo("");  // ERROR
    if (e) { // true
        std::cout << e << std::endl; //Hubo error, se imprime
    }
    e2 = procesarArchivo("archivo.txt");  //EXITO
    if (e2) { // false
        std::cout << e2 << std::endl; //No hubo error, no se imprime
    }
}
```

### Ejemplo 2: Uso de `Opcion` (Valores Opcionales)

El siguiente ejemplo ilustra cómo se puede usar `Opcion` para manejar valores que pueden o no estar presentes de manera segura.

```cpp
#include "Errores.hpp"

Opcion<int> obtenerValor(bool existe) {
    if (existe) {
        return Opcion<int>(42);  // Valor presente
    } else {
        return Opcion<int>();  // Opción vacía
    }
}

int main() {
    auto opcion = obtenerValor(true);  // Opción con valor
    auto [valor, ok] = opcion.Consumir();
    if (ok) {
        std::cout << "Valor obtenido: " << valor << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }

    // Opción vacía
    auto opcionVacia = obtenerValor(false);  // Opción vacía
    auto [valorVacio, ok] = opcionVacia.Consumir();
    if (ok) {
        std::cout << "Valor obtenido: " << valorVacio << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }
}
```

### Ejemplo 3: Resultados de Operaciones con `Resultado`

En este ejemplo, se utiliza la clase `Resultado` para encapsular tanto el valor como el estado de error de una operación.

```cpp
#include "Errores.hpp"

Resultado<int> dividir(int a, int b) {
    if (b == 0) {
        // Error de división por cero
        return Resultado<int>(0, Error::ERROR, "No se puede dividir por cero");
    }
    return Resultado<int>(a / b);  // Operación exitosa
}

int main() {
    auto resultado = dividir(10, 2);
    auto [valor, error] = resultado.Consumir();
    if (error) {
        std::cout << error << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valor << std::endl;
    }

    // División por cero
    auto resultadoError = dividir(10, 0);
    auto [valorError, e] = resultadoError.Consumir();
    if (e) {
        std::cout << e << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valorError << std::endl;
    }
}
```

### Ejemplo 4: Integración de `Errores--` en un Proyecto Completo

Este ejemplo muestra cómo se puede integrar toda la funcionalidad de `Errores--` para gestionar valores opcionales y resultados, así como manejar errores de manera coherente en un programa más grande.

```cpp
#include "Errores.hpp"

Resultado<int> procesarDatos(int valor) {
    if (valor < 0) {
        // Si el valor es negativo, se retorna un error
        return Resultado<int>(0, Error::ERROR, "Valor negativo no permitido");
    }
    return Resultado<int>(valor * 2);  // Operación exitosa
}

Opcion<std::string> obtenerMensaje(bool exito) {
    if (exito) {
        return Opcion<std::string>("Operación exitosa");
    } else {
        return Opcion<std::string>();  // Mensaje vacío
    }
}

int main() {
    // Procesar datos con un valor positivo
    auto resultado = procesarDatos(5);
    auto [valor, error] = resultado.Consumir();
    if (error) {
        std::cout << "Error: " << error << std::endl;
    } else {
        std::cout << "Resultado procesado: " << valor << std::endl;
    }

    // Obtener mensaje de éxito
    auto mensaje = obtenerMensaje(true);
    auto [msg, valido] = mensaje.Consumir();
    if (valido) {
        std::cout << "Mensaje: " << msg << std::endl;
    } else {
        std::cout << "No se obtuvo un mensaje." << std::endl;
    }

    // Procesar datos con un valor negativo
    auto resultadoNegativo = procesarDatos(-10);
    auto [valorNegativo, errorNegativo] = resultadoNegativo.Consumir();
    if (errorNegativo) {
        std::cout << "Error: " << errorNegativo << std::endl;
    } else {
        std::cout << "Resultado procesado: " << valorNegativo << std::endl;
    }
}
```

---
