# `Opcion<T>`

La estructura `Opcion<T>` representa un tipo de opción que puede contener un valor de tipo `T`
o **estar vacía** (sin valor).

#### Tipos aceptados:
`Opcion<T>` puede almacenar:
- Un **valor directo** (por ejemplo, `int`, `std::string`, etc.).
- Un **puntero inteligente** (`std::unique_ptr<T>` o `std::shared_ptr<T>`).
- Un **puntero desnudo** a **memoria no compartida** (`T*`), en cuyo caso se asume que la memoria
  es propiedad exclusiva de la estructura y será liberada al destruir la instancia.

#### Restricciones importantes:
- Si `T` es un **puntero desnudo** (`T*`), se asume que la memoria a la que apunta es
  propiedad exclusiva de esta instancia, **no debe ser compartida**.
- **No se debe pasar un puntero desnudo** que apunte a memoria gestionada por `std::shared_ptr`
  o cualquier otra técnica de manejo de memoria compartida, ya que podría resultar en un
  comportamiento indefinido (por ejemplo, doble liberación de memoria).

#### Funciones y Operadores:
- **`Opcion()`**: Constructor por defecto que inicializa la opción vacía.
- **`Opcion(T data)`**: Constructor que inicializa la opción con el valor proporcionado.
- **`estaVacia()`**: Indica si la opción está vacía (sin valor). Devuelve `true` si está vacía, `false` si contiene un valor.
- **`operator()`**: "Eleva" el valor contenido en la opción a una tupla `std::tuple<T, bool>`. Si la opción contiene un valor, la tupla incluye ese valor y `true`; si está vacía, incluye un valor por defecto (inicializado a cero) y `false`.
- **`operator bool()`**: Convierte la opción a un valor booleano. Devuelve `true` si la opción contiene un valor válido, `false` si está vacía.

#### Recomendaciones:
- Si se utiliza un puntero desnudo, se recomienda envolverlo en un puntero inteligente
  (por ejemplo, `std::unique_ptr`) para mejorar la seguridad y claridad del código.

---

### Ejemplo de uso de `Opcion<T>`

```cpp
#include <iostream>
#include <memory>

int main() {
    // Ejemplo 1: Opción con un valor
    Opcion<int> opcionConValor(42);  // Instanciación con un valor específico
    std::cout << "Opción con valor: " << opcionConValor.Data() << std::endl;  // Uso de Data()

    // Ejemplo 2: Opción vacía
    Opcion<int> opcionVacia;  // Instanciación por defecto (vacía)
    std::cout << "Opción vacía? " << (opcionVacia ? "Sí" : "No") << std::endl;  // Uso del operador bool

    // Ejemplo 3: Lifting con operator()
    auto [data, ok] = opcionConValor();  // Usando la tupla devuelta por operator()
    std::cout << "Valor elevado: " << data << ", Ok: " << (ok ? "Sí" : "No") << std::endl; // Desempaquetado de la tupla

    // Ejemplo 4: Destructor (liberación de recursos)
    // No es necesario llamar explícitamente al destructor, se llamará automáticamente cuando las instancias
    // salgan de ámbito, liberando la memoria si se usa puntero desnudo.
}
