# Opcion<T>

### Descripción General
`Opcion<T>` es un contenedor genérico que representa un valor opcional que puede estar presente o ausente. Implementa especializaciones para manejar diferentes tipos de datos de manera segura, siguiendo el paradigma de inicialización cero (ZII).


## Opcion<T>

### Descripción General
`Opcion<T>` es un contenedor genérico que representa un valor opcional que puede estar presente o ausente. Implementa especializaciones para manejar diferentes tipos de datos de manera segura, siguiendo el paradigma de inicialización cero (ZII).

### Definición del Tipo Base
```cpp
namespace opc {
    template<typename T>
    class OpcionBase {
    protected:
        bool vacia;
    public:
        OpcionBase() noexcept : vacia(true) {}
        virtual ~OpcionBase() noexcept = default;
        bool estaVacia() const noexcept { return vacia; }
        operator bool() noexcept { return !vacia; }
    };
}
```

### Implementación Principal
```cpp
namespace opc {
    template<typename T>
    struct Opcion : public OpcionBase<T> {
    private:
        T data;
        using OpcionBase<T>::vacia;
        
    public:
        Opcion() noexcept;
        explicit Opcion(T data) noexcept;
        
        std::tuple<T, bool> Consumir() noexcept;
        operator bool() noexcept;
        std::tuple<T, bool> operator()() noexcept;
    };
}
```

### Especialización para Punteros Desnudos
```cpp
namespace opc {
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    struct Opcion<T> : public OpcionBase<T> {
    private:
        T data;
        using OpcionBase<T>::vacia;
        
    public:
        explicit Opcion() noexcept;
        explicit Opcion(T data) noexcept;
        
        Opcion(const Opcion<T>&) = delete;
        Opcion operator=(const Opcion<T>&) = delete;
        
        Opcion(const Opcion<T>&& otro) noexcept;
        Opcion& operator=(const Opcion<T>&& otro) noexcept;
        
        ~Opcion() noexcept;
        std::tuple<T, bool> Consumir() noexcept;
        std::tuple<T, bool> operator()() noexcept;
    };
}
```

### Especialización para Punteros Inteligentes
```cpp
namespace opc {
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    struct Opcion<T> : public OpcionBase<typename T::element_type> {
    private:
        T data;
        using OpcionBase<typename T::element_type>::vacia;
        
    public:
        explicit Opcion() noexcept;
        explicit Opcion(T data) noexcept;
        
        Opcion(const Opcion<T>&) = delete;
        Opcion operator=(const Opcion<T>&) = delete;
        
        Opcion(const Opcion<T>&& otro) noexcept;
        Opcion& operator=(const Opcion<T>&& otro) noexcept;
        
        ~Opcion() noexcept = default;
        std::tuple<T, bool> Consumir() noexcept;
        std::tuple<T, bool> operator()() noexcept;
    };
}
```

### Métodos
- `bool estaVacia() const noexcept`: Indica si la opción está vacía
- `std::tuple<T, bool> Consumir() noexcept`: Devuelve una tupla con el valor (si existe) y un indicador de éxito
- `operator bool()`: Devuelve verdadero si la opción contiene un valor
- `operator()()`: Alias para Consumir()

### Especializaciones
1. **Valores Directos**
   - Maneja tipos por valor (int, std::string, etc.)
   - Inicialización a cero cuando está vacío

2. **Punteros Desnudos** (T*)
   - Asume propiedad exclusiva de la memoria apuntada
   - Libera la memoria automáticamente si no es consumida
   - Constructor y operador de copia eliminados
   - Semántica de movimiento mediante std::exchange

3. **Punteros Inteligentes** (std::unique_ptr<T>, std::shared_ptr<T>)
   - Semántica de movimiento para transferencia de propiedad
   - nullptr cuando está vacío
   - Constructor y operador de copia eliminados

### Ejemplo de Uso Idiomático
```cpp
// Función que puede devolver un valor opcional
opc::Opcion<int> obtenerConfiguracion(const std::string& clave) {
    if (configuraciones.count(clave) == 0) {
        return opc::Opcion<int>();  // Retorna opción vacía
    }
    return opc::Opcion<int>(configuraciones[clave]);
}

// Uso
auto opcion = obtenerConfiguracion("max_conexiones");
auto [valor, ok] = opcion();
if (ok) {
    std::cout << "Configuración cargada: " << valor << std::endl;
} else {
    // Usar valor por defecto o manejar ausencia
    valor = 10;  // valor por defecto
}
```
