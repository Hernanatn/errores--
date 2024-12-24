## `Errores--`

`Errores--` es una librería de C++ que ofrece contenedores genéricos + de errores, valores opcionales y resultados de operaciones. Consiste de tres abstracciones: `Error`, `Opcion` y `Resultado`. Esta librería proporciona una forma coherente y flexible de gestionar el flujo de control en situaciones donde es necesario manejar errores de manera explícita -evitando rupturas en el flujo de control a través de excepciones- y gestionar la presencia o ausencia de valores de forma segura.

`Errores--` está inspirada en el manejo de errores como valores de `Go` y en los contenedores Option<T> y Result<T> de `Rust`. La librería adscribe al paradigma ***Cero es Inicialización - ZII,** por sus siglas en inglés* y trata de imponer prácticas seguras con una semántica de propiedad de memoria clara.


La librería se presenta aún en versión alpha - distribución `v 0.1.0-alpha`, no está particularmente optimizada y puede presentar errores.

[![Hecho por Chaska](https://img.shields.io/badge/hecho_por-Ch'aska-303030.svg)](https://cajadeideas.ar)
[![Versión: Alpha v0.1.0](https://img.shields.io/badge/version-Alpha_v0.1.0-orange.svg)](https://github.com/hernanatn/github.com/hernanatn/aplicacion.go/releases/latest)
[![Verisón de C++: 17](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)](https://es.cppreference.com/w/cpp/17)
[![Licencia: CC BY-NC 4.0](https://img.shields.io/badge/Licencia-CC_BY--SA_4.0-lightgrey.svg)](LICENSE)


---

## Índice

1. [Errores--](#errores)
    - Descripción general
2. [Manejo de Memoria](#manejo-de-memoria)
   - Valores Directos
   - Punteros Inteligentes
   - Punteros Desnudos
     - Advertencia

3. [Abstracciones Principales](#abstracciones-principales)
   - [Error](#error)
     - Estructura
     - Métodos y Funciones Útiles
   - [Opcion<T>](#opciont)
     - Estructura
     - Métodos y Funciones Útiles
     - Restricciones y Notas
     - Ejemplo de Uso
   - [Resultado<T>](#resultadot)
     - Estructura
     - Métodos y Funciones Útiles
     - Restricciones y Notas

4. [Ejemplos](#ejemplos)
   - [Ejemplo Mínimo de Uso Idiomático](#ejemplo-mínimo-de-uso-idiomático)
   - [Otros ejemplos](#otros-ejemplos)
     - Ejemplo 1: Manejo de Errores
     - Ejemplo 2: Uso de `Opcion` (Valores Opcionales)
     - Ejemplo 3: Resultados de Operaciones con `Resultado`
   - [Ejemplo "Integral" - Servidor de Eco](#ejemplo-integral---servidor-de-eco)

---

## Manejo de Memoria

La librería impone reglas claras para garantizar la seguridad de la memoria:
Tanto `Opcion<T>` como `Resultado<T>` asumen propiedad de la memoria de sus miembros en tanto no sea *consumidos*. E imponen las siguientes restricciones de memoria:

- **Valores Directos**: `T` puede ser un valor directo como `int` o `std::string`.
- **Punteros Inteligentes**: `std::unique_ptr<T>` o `std::shared_ptr<T>` son soportados y respetan las reglas de propiedad.
- **Punteros Desnudos**: Si `T` es un puntero desnudo (`T*`), la memoria debe ser exclusiva de la instancia. Esta será liberada automáticamente al destruir la instancia, salvo que se consuma explícitamente (mediante una llamada a `.Consumir()` o aprovechando la sintaxis cómoda de la sobrecarga del operador `()` ).
  - **Advertencia**: No se debe utilizar punteros desnudos que apunten a memoria gestionada por `std::shared_ptr` u otras técnicas de manejo de memoria compartida.

## Abstracciones Principales

### **Error**

La clase `Error` encapsula información sobre errores mediante un código de estado y un mensaje descriptivo. Esta abstracción permite identificar claramente el estado de una operación (éxito, error o fatal), es extensible con la implementación de códigos especializados para casos concretos. Está expuesta através del espacio de nombre `err`.

#### Estructura:
```cpp
struct Error {
    CodigoEstado codigo;
    std::string mensaje;

    Error();
    explicit Error(CodigoEstado codigo, std::string mensaje = "ERROR");
    explicit Error(Error *e);

    CodigoEstado Codigo();
    std::string Mensaje();
    void agregarMensaje(std::string mensaje);

    operator bool();
    operator std::string() const;
    operator const char*() const;
    operator char*();

    friend std::ostream &operator<<(std::ostream &os, Error const &e);
};
```

#### Métodos y Funciones Útiles:
- **Constructor por defecto:**
  Inicializa un error genérico con el código `ERROR` y un mensaje predeterminado "ERROR".
- **Constructores explícitos:**
  Permiten inicializar un `Error` con un código y un mensaje o copiar desde otro error.
- **Métodos de acceso:**
  - `Codigo()`: Devuelve el código de estado.
  - `Mensaje()`: Devuelve el mensaje descriptivo.
  - `agregarMensaje(std::string mensaje)`: Añade un mensaje adicional al existente.
- **Sobrecarga de operadores:**
  - Conversión a `bool`, para verificar si el estado es exitoso.
  - Conversión a cadenas (`std::string`, `const char*`, `char*`).
  - Impresión directa con `std::ostream`.
- **Funciones estáticas:**
  - `Exito(std::string mensaje = "Exito")`: Retorna un error con código `EXITO`.
  - `Fatal(std::string mensaje = "Error Fatal")`: Retorna un error con código `FATAL`.
  - `Generico(std::string mensaje = "Error")`: Retorna un error con código `ERROR`.



### **Opcion<T>**

La clase `Opcion<T>` representa un contenedor para valores opcionales, similar a `std::optional`, pero con características adicionales como soporte para transferencia de propiedad. Está expuesta através del espacio de nombre `opc`.

#### Estructura:
```cpp
template <typename T>
struct Opcion {
private:
    T data;         // Almacena el valor, si existe.
    bool vacia;     // Indica si la instancia contiene un valor.

public:
    // Constructores
    Opcion() noexcept;               // Constructor por defecto (opción vacía).
    explicit Opcion(T data) noexcept; // Constructor con inicialización de valor.

    // Destructor
    ~Opcion();

    // Métodos
    bool estaVacia() const noexcept; // Indica si la opción está vacía.
    std::tuple<T, bool> Consumir() noexcept; // Obtiene y consume el valor, transfiriendo propiedad si aplica.

    // Operadores
    operator bool() noexcept;               // Conversión a booleano (true si tiene valor).
    std::tuple<T, bool> operator()() noexcept; // Consume el valor (igual que `Consumir()`).
};
```

#### Métodos y Funciones Útiles:
- **Constructor por defecto:**
  Inicializa una opción vacía.
  ```cpp
  Opcion<int> opcion; // Instancia sin valor.
  ```

- **Constructor explícito:**
  Inicializa la opción con un valor.
  ```cpp
  Opcion<int> opcion(42); // Instancia con el valor 42.
  ```

- **`bool estaVacia() const noexcept`:**
  Devuelve `true` si la opción no contiene un valor.
  ```cpp
  if (opcion.estaVacia()) {
      std::cout << "Sin valor\n";
  }
  ```

- **`std::tuple<T, bool> Consumir() noexcept`:**
  Devuelve el valor contenido (si existe) y un indicador booleano que señala si había un valor válido. Transfiere la propiedad del valor, dejando la opción vacía.
  ```cpp
  auto [valor, ok] = opcion.Consumir();
  if (ok) {
      std::cout << "Valor: " << valor << "\n";
  }
  ```

- **Operador `operator bool()`:**
  Conversión implícita a booleano. Útil para verificar si la opción contiene un valor.
  ```cpp
  if (opcion) {
      std::cout << "Opción tiene valor\n";
  }
  ```

- **Operador `operator()()`:**
  Sinónimo de `Consumir`. Permite consumir el valor usando sintaxis funcional.
  ```cpp
  auto [valor, ok] = opcion();
  ```

#### Restricciones y Notas

1. **Punteros desnudos:**
   - Si se utiliza un puntero desnudo (`T*`), debe ser propiedad exclusiva de la opción.
   - Al consumir el valor, el puntero se transfiere mediante `std::exchange`, dejando el puntero original como `nullptr`.

2. **Punteros inteligentes:**
   - Compatible con `std::unique_ptr` y `std::shared_ptr`.
   - La propiedad se transfiere mediante `std::move`.


#### Ejemplo de Uso

```cpp
#include "Opcion.hpp"
#include <iostream>

int main() {
    opc::Opcion<int> opcion(42); // Opción con valor.

    if (opcion) {
        auto [valor, ok] = opcion();
        if (ok) {
            std::cout << "El valor es: " << valor << std::endl;
        }
    } else {
        std::cout << "La opción está vacía." << std::endl;
    }

    return 0;
}
```

En este ejemplo, se ilustra cómo crear y consumir una opción. Al final, la opción queda vacía después de llamar a `Consumir`.


### **Resultado<T>**

La clase `Resultado` encapsula el resultado de una operación que puede fallar, combinando un valor de tipo `T` y un objeto `Error`. Está expuesta através del espacio de nombre `res`.

#### Estructura:
```cpp
template <typename T>
class Resultado {
    T valor;
    Error error;

public:
    Resultado(T valor);
    Resultado(T valor, CodigoEstado codigo, std::string mensaje);

    std::pair<T, Error> operator()();
};
```

#### Métodos y Funciones Útiles:
- **Constructores:**
  - `Resultado(T valor)`: Crea un resultado exitoso.
  - `Resultado(T valor, CodigoEstado codigo, std::string mensaje)`: Crea un resultado con un error asociado.
- **Sobrecarga del operador `()`:**
  Devuelve una tupla con el valor y el error:
  ```cpp
  auto [valor, error] = resultado();
  if (error) {
      // Manejo de errores.
  } else {
      // Uso del valor.
  }
  ```
  #### Restricciones y Notas

1. **Punteros desnudos:**
   - Si se utiliza un puntero desnudo (`T*`), debe ser propiedad exclusiva de la opción.
   - Al consumir el valor, el puntero se transfiere mediante `std::exchange`, dejando el puntero original como `nullptr`.

2. **Punteros inteligentes:**
   - Compatible con `std::unique_ptr` y `std::shared_ptr`.
   - La propiedad se transfiere mediante `std::move`.


## Ejemplos
#### Ejemplo Mínimo de Uso Idiomático

```cpp
#include "errores--.hpp"

res::Resultado<int> dividir(int a, int b) {
    if (b == 0) {
        return res::Resultado<int>(0, Error::ERROR, "No se puede dividir por cero");
    }
    return res::Resultado<int>(a / b);
}

int main() {
    res::Resultado<int> resultado = dividir(10, 2);
    auto [valor, error] = resultado();
    if (error) {
        std::cout << error << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valor << std::endl;
    }

    res::Resultado<int> resultadoError = dividir(10, 0);
    auto [valorError, e] = resultadoError();
    if (e) {
        std::cout << e << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valorError << std::endl;
    }
}
```

En este ejemplo, el flujo de control es claro y expresivo, evitando excepciones y utilizando la sobrecarga de operadores para simplificar la lógica de manejo de errores.


### Otros ejemplos

A continuación se presentan algunos ejemplos completos de uso de la librería `Errores--`, ilustrando cómo manejar errores, valores opcionales y resultados de operaciones.

#### Ejemplo 1: Manejo de Errores

En este ejemplo se muestra cómo se pueden utilizar los tipos de error para manejar distintas situaciones en el flujo de un programa.

```cpp
#include "errores--.hpp"

err::Error procesarArchivo(const std::string& nombreArchivo) {
    if (nombreArchivo.empty()) {
        // Si el nombre del archivo está vacío, se genera un error
        err::Error error = err::Generico("El nombre del archivo está vacío");
        return error;
    }

    // Supongamos que el archivo fue procesado exitosamente
    err::Error exito = Error::Exito("Operación exitosa. Archivo leído");
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

#### Ejemplo 2: Uso de `Opcion` (Valores Opcionales)

El siguiente ejemplo ilustra cómo se puede usar `Opcion` para manejar valores que pueden o no estar presentes de manera segura.

```cpp
#include "errores--.hpp"

opc::Opcion<int> obtenerValor(bool existe) {
    if (existe) {
        return Opcion<int>(42);  // Valor presente
    } else {
        return Opcion<int>();  // Opción vacía
    }
}

int main() {
    opc::Opcion<int> opcion = obtenerValor(true);  // Opción con valor
    auto [valor, ok] = opcion();
    if (ok) {
        std::cout << "Valor obtenido: " << valor << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }

    // Opción vacía
    opc::Opcion<int> opcionVacia = obtenerValor(false);  // Opción vacía
    auto [valorVacio, ok] = opcionVacia();
    if (ok) {
        std::cout << "Valor obtenido: " << valorVacio << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }
}
```

#### Ejemplo 3: Resultados de Operaciones con `Resultado`

En este ejemplo, se utiliza la clase `Resultado` para encapsular tanto el valor como el estado de error de una operación.

```cpp
#include "errores--.hpp"

res::Resultado<int> dividir(int a, int b) {
    if (b == 0) {
        // Error de división por cero
        return Resultado<int>(0, Error::ERROR, "No se puede dividir por cero");
    }
    return Resultado<int>(a / b);  // Operación exitosa
}

int main() {
    res::Resultado<int> resultado = dividir(10, 2);
    auto [valor, error] = resultado();
    if (error) {
        std::cout << error << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valor << std::endl;
    }

    // División por cero
    res::Resultado<int> resultadoError = dividir(10, 0);
    auto [valorError, e] = resultadoError();
    if (e) {
        std::cout << e << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valorError << std::endl;
    }
}
```

### Ejemplo "Integral" - Servidor de Eco

````cpp
#include "errores--.hpp"

#include <iostream>
#include <string>
#include <vector>


// Simula la inicialización de un socket servidor.
// Devuelve un `Resultado` con un identificador de socket o un error.
res::Resultado<int> inicializarServidor(int puerto) {
    if (puerto <= 0) {
        return res::Resultado<int>(0, CodigoEstado::ERROR, "Puerto inválido");
    }
    // Lógica simulada para inicializar el servidor.
    int socketID = 42; // ID del socket simulado.
    return res::Resultado<int>(socketID);
}

// Simula la aceptación de una nueva conexión.
// Devuelve un `Resultado` con el identificador del cliente conectado.
res::Resultado<int> aceptarConexion(int servidorSocket) {
    if (servidorSocket <= 0) {
        return res::Resultado<int>(0, err::CodigoEstado::ERROR, "Socket del servidor inválido");
    }
    // Lógica simulada para aceptar una conexión.
    int clienteSocket = 24; // ID del cliente simulado.
    return res::Resultado<int>(clienteSocket);
}

// Simula la recepción de un mensaje del cliente.
// Devuelve una `Opcion` con el mensaje recibido o vacío si no hay datos.
opc::Opcion<std::string> recibirMensaje(int clienteSocket) {
    if (clienteSocket <= 0) {
        return opc::Opcion<std::string>(); // Opción vacía.
    }
    // Lógica simulada para recibir un mensaje.
    return opc::Opcion<std::string>("Mensaje de prueba");
}

// Simula el envío de un mensaje al cliente.
// Devuelve un `Error` en caso de fallo.
er::Error enviarMensaje(int clienteSocket, const std::string& mensaje) {
    if (clienteSocket <= 0 || mensaje.empty()) {
        return err::Generico("Error al enviar el mensaje");
    }
    // Lógica simulada para enviar un mensaje.
    return err::Exito("Mensaje enviado correctamente");
}

// Simula el cierre de un socket.
void cerrarSocket(int socketID) {
    // Lógica simulada para cerrar el socket.
    std::cout << "Socket " << socketID << " cerrado." << std::endl;
}

// Implementación del servidor de eco.
void servidorEco(int puerto) {
    auto [servidorSocket, errorServidor] = inicializarServidor(puerto)();
    if (errorServidor) {
        std::cout << "Error al inicializar el servidor: " << errorServidor << std::endl;
        return;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << " (Socket ID: " << servidorSocket << ")" << std::endl;

    while (true) {
        auto [clienteSocket, errorConexion] = aceptarConexion(servidorSocket)();
        if (errorConexion) {
            std::cout << "Error al aceptar conexión: " << errorConexion << std::endl;
            continue;
        }

        std::cout << "Cliente conectado (Socket ID: " << clienteSocket << ")" << std::endl;

        while (true) {
            auto [mensaje, tieneMensaje] = recibirMensaje(clienteSocket)();
            if (!tieneMensaje) {
                std::cout << "El cliente desconectó." << std::endl;
                break;
            }

            std::cout << "Mensaje recibido: " << mensaje << std::endl;

            err::Error errorEnvio = enviarMensaje(clienteSocket, mensaje);
            if (errorEnvio) {
                std::cout << "Error al enviar el mensaje al cliente: " << errorEnvio << std::endl;
                break;
            }
        }

        cerrarSocket(clienteSocket);
    }

    cerrarSocket(servidorSocket);
}

int main() {
    servidorEco(8080);
    return 0;
}

```

