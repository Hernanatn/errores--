## Seguridad y Manejo de Memoria

La librería implementa una gestión de memoria estricta y segura mediante reglas claras de propiedad. Tanto `Opcion<T>` como `Resultado<T>` asumen la propiedad exclusiva de sus miembros hasta que estos sean *consumidos*. Este diseño previene fugas de memoria y accesos a memoria liberada.

### Tipos de Datos y Reglas de Propiedad

1. **Valores Directos**
   - Tipos fundamentales (`int`, `double`, etc.)
   - Tipos de la biblioteca estándar (`std::string`, etc.)
   - Tipos definidos por el usuario que soporten construcción por defecto
   - Se copian al construir y consumir las instancias

2. **Punteros Inteligentes**
   - Compatible con `std::unique_ptr<T>` y `std::shared_ptr<T>`
   - **Importante**: Deben ser movidos mediante `std::move` al construir las instancias
   - La transferencia de propiedad al consumir se realiza automáticamente mediante `std::move`
   - Ejemplo:
     ```cpp
     auto ptr = std::make_unique<MiClase>(42);
     opc::Opcion<std::unique_ptr<MiClase>> opcion(std::move(ptr)); // ptr queda nullptr
     ```

3. **Punteros Desnudos**
   - **Recomendación**: Se prefiere el uso de punteros inteligentes sobre punteros desnudos
   - Por defecto, se asume propiedad exclusiva del puntero
   - La memoria se libera automáticamente al destruir la instancia si no ha sido consumida
   - Pueden "compartirse" temporalmente si se garantiza que la instancia será consumida antes de salir del scope
   - **Advertencia**: El desarrollador es responsable de asegurar que el puntero sea consumido antes de cualquier liberación manual de memoria

### Gestión Segura de Punteros Desnudos

Aunque se recomienda usar punteros inteligentes, hay situaciones donde los punteros desnudos son necesarios. En estos casos:

```cpp
// Ejemplo de uso seguro de punteros desnudos
MiClase* ptr = new MiClase(42);
opc::Opcion<MiClase*> opcion(ptr);

// Caso 1: Uso temporal del puntero
if (!opcion.estaVacia()) {
    // Operaciones seguras con ptr mientras opcion exista
    ptr->operacion();
}
// Consumir antes de que opcion salga del scope
auto [valor, existe] = opcion();
// Ahora somos responsables de la memoria
delete valor;

// Caso 2: Transferencia de propiedad
MiClase* ptr2 = new MiClase(42);
{
    opc::Opcion<MiClase*> opcion2(ptr2);
    // La memoria será liberada automáticamente al salir del scope
    // si no se consume la opción
}

// Caso 3: NO HACER - Riesgo de doble liberación
MiClase* ptr3 = new MiClase(42);
{
    opc::Opcion<MiClase*> opcion3(ptr3);
    delete ptr3;  // ¡ERROR! opcion3 intentará liberar ptr3 nuevamente
}
```

### Transferencia Segura de Propiedad

La librería garantiza una transferencia segura de propiedad mediante el método `Consumir()` o el operador `()`:

```cpp
// Con punteros inteligentes
auto ptr = std::make_unique<MiClase>(42);
res::Resultado<std::unique_ptr<MiClase>> resultado(std::move(ptr));
auto [valor, error] = resultado();  // valor toma propiedad del puntero

// Con punteros desnudos
MiClase* ptr = new MiClase(42);
opc::Opcion<MiClase*> opcion(ptr);
auto [valor, existe] = opcion();  // opcion ya no es dueña del puntero
if (existe) {
    delete valor;  // Ahora somos responsables de la memoria
}
```

### Restricciones y Advertencias

1. **Punteros Desnudos**:
   - No usar con memoria gestionada por `std::shared_ptr` u otros mecanismos de memoria compartida
   - Garantizar que la instancia sea consumida antes de cualquier liberación manual
   - Evitar compartir el puntero fuera del scope de la instancia contenedora

2. **Punteros Inteligentes**:
   - Siempre usar `std::move` al construir instancias
   - No acceder al puntero original después de moverlo
   - No almacenar referencias al contenido del puntero fuera del scope de la instancia

3. **Valores**:
   - Asegurar que los tipos soporten construcción por copia si no son punteros
   - Considerar el costo de las copias al trabajar con objetos grandes
