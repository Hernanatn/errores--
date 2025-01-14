## `Errores--`

`Errores--` es una librería de C++ que provee abstracciones genéricas para el manejo de errores, valores opcionales y resultados de operaciones. La librería implementa tres abstracciones fundamentales: `Error`, `Opcion<T>` y `Resultado<T>`. Esta librería proporciona una forma coherente y flexible de gestionar el flujo de control en situaciones donde es necesario manejar errores de manera explícita -evitando rupturas en el flujo de control a través de excepciones- y gestionar la presencia o ausencia de valores de forma segura.

`Errores--` está inspirada en el manejo de errores como valores de `Go` y en los contenedores `Option<T>` y `Result<T>` de `Rust`. La librería adscribe al paradigma ***Cero es Inicialización - ZII** (Zero Is Initialization)* y se enfoca en imponer prácticas seguras con una semántica clara de propiedad de memoria.

<img src="https://img.shields.io/badge/hecho_por-Ch'aska-253545?style=for-the-badge" alt="hecho_por_Chaska" height="25px"/> <img src="https://img.shields.io/badge/C%2B%2B_20-4549BF?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++" height="25px"/> <a href=https://github.com/hernanatn/github.com/hernanatn/errores--/releases/latest><img src="https://img.shields.io/badge/Versión-0.5.0--beta-orange?style=for-the-badge" alt="version" height="25px"/></a> <img src="https://img.shields.io/badge/Licencia-CC_BY--NC--ND_4.0-lightgrey?style=for-the-badge" alt="licencia" height="25px"/>


![Empaquetado](https://img.shields.io/github/actions/workflow/status/Hernanatn/errores--/empaquetar.yml?label=bundle)
![Pruebas](https://img.shields.io/github/actions/workflow/status/Hernanatn/errores--/pruebas.yml?label=tests)

<details>

<summary><h3>Índice </h3></summary>    


1. [Introducción](#errores--)
    - [Uso](#uso)
2. [Novedades en v0.5.0-beta](#novedades-en-v050-beta)
3. [Manejo de Memoria](#manejo-de-memoria)
   - [Tipos de Datos Soportados](#tipos-de-datos-soportados)
   - [Restricciones de Memoria](#restricciones-de-memoria)
4. [Abstracciones Principales](#abstracciones-principales)
   - [Error](#error)
   - [Opcion\<T>](#opciont)
   - [Resultado\<T>](#resultadot)
5. [Documentación](#documentación)
6. [Requisitos](#requisitos)
7. [Pruebas](#pruebas)
8. [Estado del Proyecto](#estado-del-proyecto)
9. [Ejemplos](#ejemplos)
   - [Ejemplo de Uso Idiomático](#ejemplo-de-uso-idiomático)
   - [Manejo de Valores Directos](#manejo-de-valores-directos)
   - [Manejo de Punteros Inteligentes](#manejo-de-punteros-inteligentes)
   - [Manejo de Errores](#manejo-de-errores)
10. [Licencia](#licencia)


</details>

### Uso
La librería se puede incluir en un proyecto descargando e `#incluyendo` el archivo de encabezado [`errores--.hpp`](/errores--.hpp)

```cpp
#include "errores--.hpp"
```

<details>
<summary><h2>Novedades en v0.5.0-beta</h2></summary>
    
## Novedades en v0.5.0-beta

Esta versión introduce manejo diferenciado para atender casos en que el tipo subyacente no provee constructor por defecto:

- Implementación de especializaciones de plantillas para `Opcion<T>` y `Resultado<T>` que que manejan casos en que `T` no provee constructor por defecto;
- Tanto ``Opcion<T>`` como ``Resultado<T>`` ahora inicializan a `nullptr` cuando `T` es un puntero desnudo; 
- Agrega método útil `valorO(T porDefecto)` en la especialización de `Opcion<T>` para valores "simples"; y
- Más pruebas.

</details>

## Manejo de Memoria

La librería implementa reglas estrictas para garantizar la seguridad de la memoria. Tanto `Opcion<T>` como `Resultado<T>` asumen la propiedad de sus miembros hasta que sean *consumidos*. Las restricciones de memoria se aplican según el tipo de dato:

### Tipos de Datos Soportados

1. **Valores Directos**
   - Tipos fundamentales (`int`, `double`, etc.)
   - Tipos de la biblioteca estándar (`std::string`, etc.)
   - Tipos definidos por el usuario que soporten construcción por defecto

2. **Punteros Inteligentes**
   - `std::unique_ptr<T>`
   - `std::shared_ptr<T>`

3. **Punteros Desnudos**
   - Deben ser propiedad exclusiva de la instancia
   - Se liberan automáticamente al destruir la instancia
   - La propiedad se transfiere al consumir el valor

### Restricciones de Memoria

- Los punteros desnudos deben ser propiedad exclusiva de la instancia
- No se permite el uso de punteros desnudos que apunten a memoria gestionada por `std::shared_ptr`
- La transferencia de propiedad se realiza mediante `std::move` para punteros inteligentes
- Los punteros desnudos se transfieren mediante `std::exchange`

## Abstracciones Principales

### Error

La clase `Error` encapsula información sobre errores mediante un código de estado y un mensaje descriptivo. Esta abstracción permite identificar claramente el estado de una operación (éxito, error o fatal) y es extensible para casos específicos. [Referencia completa](/documentación/Error.md).

#### Métodos
- `CodigoEstado Codigo()`: Devuelve el código de estado del error
- `std::string Mensaje()`: Devuelve el mensaje de error
- `void agregarMensaje(std::string mensaje)`: Agrega texto adicional al mensaje
- `operator bool()`: Devuelve verdadero si hay un error (estado no es EXITO)
- `operator std::string()`: Convierte el error a su representación en cadena
- `operator const char*()`: Convierte el mensaje a cadena estilo C
- `operator char*()`: Convierte el mensaje a cadena estilo C modificable
- `operator<<`: Permite imprimir el error en flujos de salida

#### Funciones Útiles
```cpp
namespace err {
    inline Error Exito(std::string mensaje = "Exito");
    inline Error Fatal(std::string mensaje = "Error Fatal");
    inline Error Generico(std::string mensaje = "Error");
}
```

### Opcion<T>

`Opcion<T>` es un contenedor genérico que representa un valor opcional que puede estar presente o ausente. Implementa especializaciones para manejar diferentes tipos de datos de manera segura. [Referencia completa](/documentación/Opcion.md).

#### Métodos
- `bool estaVacia() const noexcept`: Indica si la opción está vacía
- `std::tuple<T, bool> Consumir() noexcept`: Devuelve una tupla con el valor (o en su defecto `T{}` / `nullptr`) y un indicador de si la Opción está vacía.  *Para valores directos que proveen constructor por defecto, o para punteros*.
- `std::tuple<T, bool> Consumir(T porDefecto) noexcept`: Devuelve una tupla con el valor (si existe, sino valor por defect) y un indicador de éxito. *Para valores directos que no proveen constructor por defecto*
- `operator bool()`: Devuelve verdadero si la opción contiene un valor
- `operator()()`: Alias para Consumir()

#### Especializaciones
1. **Valores Directos**
   - Retorna valor inicializado a cero si está vacía y si el tipo ofrece constructor por defecto, o acepta un argumento `T porDefecto` en caso contrario
   - Constructor por defecto eliminado para tipos que no lo proveen.

2. **Punteros Desnudos** (T*)
   - nullptr cuando está vacía
   - Asume propiedad exclusiva de la memoria apuntada
   - Libera la memoria automáticamente si no es consumida
   - Constructor y operador de copia eliminados
   - Semántica de movimiento mediante std::exchange

3. **Punteros Inteligentes** (std::unique_ptr<T>, std::shared_ptr<T>)
   - nullptr cuando está vacía
   - Constructor y operador de copia eliminados
   - Semántica de movimiento para transferencia de propiedad


### Resultado<T>

`Resultado<T>` encapsula el resultado de una operación que puede ser exitosa o fallar, combinando un valor de tipo `T` con un `Error`. [Referencia completa](/documentación/Resultado.md)

#### Métodos
- `err::Error Error() const noexcept`: Devuelve el estado del error
- `std::tuple<T, err::Error> Consumir() noexcept`: Devuelve una tupla con el valor (o en su defecto `T{}` / `nullptr`) y el error. *Para valores directos que proveen constructor por defecto. O punteros*.
- `std::tuple<T, err::Error> Consumir(T porDefecto) noexcept`: Devuelve una tupla con el valor (si existe, de lo contrario `porDefecto`) y el error *Para valores directos que no proveen constructor por defecto*.
- `operator bool()`: Devuelve verdadero si la operación fue exitosa
- `operator()()`: Alias para Consumir()

#### Especializaciones
1. **Valores Directos**
   - Retorna valor inicializado a cero en caso de error si el tipo ofrece constructor por defecto, o acepta un argumento `T porDefecto` en caso contrario
   - Constructor por defecto eliminado para tipos que no lo proveen.

2. **Punteros Desnudos** (T*)
   - nullptr en caso de error
   - Asume propiedad exclusiva de la memoria apuntada
   - Libera memoria si no es consumida
   - Constructor y operador de copia eliminados
   - Semántica de movimiento mediante std::exchange

3. **Punteros Inteligentes** (std::unique_ptr<T>, std::shared_ptr<T>)
   - nullptr en caso de error
   - Constructor y operador de copia eliminados
   - Semántica de movimiento para transferencia de propiedad

## Documentación
La referencia para las abstracciones, una explicación más acabada del manejo de memoria con `errores--` y más ejemplos pueden encontrarse en [/documentación](/documentación)

## Requisitos

- Compilador compatible con C++20
- CMake y Catch2 (para ejecutar las pruebas)

## Pruebas
La librería incluye una suite (aún en progreso) de pruebas implementada con Catch2, que verifica el comportamiento correcto de todas las abstracciones y sus especializaciones.

Se puede correr el archivo provisto ( [/pruebas/correr_pruebas.exe](/pruebas/pruebas.exe) ) o [compilar las pruebas desde cero](/documentación/Pruebas.md)


## Estado del Proyecto

Esta versión (v0.5.0-beta) representa un avance significativo en términos de seguridad y funcionalidad, pero aún se considera en estado beta. La API puede sufrir cambios (menores) en futuras versiones.

<details>
<summary><h2>Ejemplos</h2></summaryy>
    

## Ejemplos
### Ejemplo de Uso Idiomático

```cpp
#include "errores--.hpp"

// Función que puede fallar retornando un Resultado
res::Resultado<float> dividir(float a, float b) {
    if (b == 0.0f) {
        return res::Resultado<float>(0.0f, err::ERROR, "División por cero");
    }
    return res::Resultado<float>(a / b);
}

// Función que puede retornar un valor opcional
opc::Opcion<std::string> obtenerEntorno(const std::string& clave) {
    if (configuraciones.count(clave) == 0 || !ENTORNO) {
        return opc::Opcion<std::string>();  // Retorna opción vacía
    }
    return opc::Opcion<std::string>(configuraciones[clave]);
}

// otra Función que puede retornar un valor opcional
opc::Opcion<int> obtenerConfiguracion(const std::string& clave) {
    if (configuraciones.count(clave) == 0) {
        return opc::Opcion<int>();  // Retorna opción vacía
    }
    return opc::Opcion<int>(configuraciones[clave]);
}

// Uso combinado de las abstracciones
void ejemplo() {
    // Uso de Resultado
    auto [resultado, error] = dividir(10.0f, 2.0f)();
    if (error) {
        std::cout << "Error al dividir: " << error.Mensaje();
        return;  // Manejo temprano del error
    }
    
    // Uso de Opcion
    auto opcion = obtenerConfiguracion("max_conexiones");
    auto [valor, ok] = opcion();
    if (!ok) {
        // Manejar ausencia de valor
        valor = 10;  // Valor por defecto
    }

    auto opcionEntorno = obtenerEntorno("PRODUCCION");
    std::string entorno = opcionEntorno.valorO("DESARROLLO");
    
    // Continuar con la lógica normal
    std::cout << "Resultado: " << resultado << ", Configuración: " << valor << std::endl;
}
```

### Manejo de Valores Directos

```cpp
// Ejemplo con valores directos
opc::Opcion<int> optInt(42);
auto [valor, ok] = optInt();
if (ok) {
    std::cout << "Valor: " << valor << "\n"; // Valor: 42
}

opc::Opcion<int> optInt();
int x = optInt.valorO(5)
std::cout << "X: " << x << "\n"; // X: 5
```

### Manejo de Punteros Inteligentes

```cpp
// Ejemplo con unique_ptr
auto ptr = std::make_unique<MiClase>(42);
res::Resultado<std::unique_ptr<MiClase>> resultado(std::move(ptr));
auto [valor, error] = resultado();
if (!error) {
    valor->procesar();
}
```

### Manejo de Errores

```cpp
// Ejemplo de función que retorna un Resultado
res::Resultado<float> dividir(float a, float b) {
    if (b == 0) {
        return res::Resultado<float>(0, err::Error(err::ERROR, "División por cero"));
    }
    return res::Resultado<float>(a / b);
}

auto [resultado, error] = dividir(10.0f, 2.0f)();
if (error) {
    //manejar error, salvar el estado o retornar temprano
}
//continuar
```

</details>

## Licencia
Este proyecto está licenciado bajo CC BY-SA 4.0. Ver el archivo [LICENSE](LICENSE) para más detalles.
