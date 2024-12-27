# Resultado<T>

### Descripción General
`Resultado<T>` encapsula tanto un valor de tipo T como un objeto Error, representando el resultado de una operación que puede fallar. Al igual que `Opcion<T>`, proporciona implementaciones especializadas para diferentes tipos de punteros y sigue la semántica ZII.

### Definición del Tipo Base
```cpp
namespace res {
    template<typename T>
    struct ResultadoBase {
    protected:
        err::Error error;
    public:
        ResultadoBase() noexcept: error(err::EXITO) {};
        virtual ~ResultadoBase() noexcept = default;
        
        err::Error Error() const noexcept { return error; }
        operator bool() noexcept { return !error; }
    };
}
```

### Implementación Principal
```cpp
namespace res {
    template<typename T>
    struct Resultado : public ResultadoBase<T> {
    private:
        T resultado;
        using ResultadoBase<T>::error;
        
    public:
        Resultado() noexcept;
        explicit Resultado(T data) noexcept;
        explicit Resultado(T data, err::Error error) noexcept;
        explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
        
        std::tuple<T, err::Error> Consumir() noexcept;
        ~Resultado() noexcept;
        
        operator bool() noexcept;
        std::tuple<T, err::Error> operator()() noexcept;
    };
}
```

### Especialización para Punteros Desnudos
```cpp
namespace res {
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    struct Resultado<T> : public ResultadoBase<T> {
    private:
        T resultado;
        using ResultadoBase<T>::error;
        
    public:
        Resultado() noexcept;
        explicit Resultado(T data) noexcept;
        
        Resultado(const Resultado<T>&) = delete;
        Resultado operator=(const Resultado<T>&) = delete;
        
        Resultado(const Resultado<T>&& otro) noexcept;
        Resultado& operator=(const Resultado<T>&& otro) noexcept;
        
        explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
        explicit Resultado(T data, err::Error error) noexcept;
        
        std::tuple<T, err::Error> Consumir() noexcept;
        ~Resultado() noexcept;
        
        operator bool() noexcept;
        std::tuple<T, err::Error> operator()() noexcept;
    };
}
```

### Especialización para Punteros Inteligentes
```cpp
namespace res {
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    struct Resultado<T> : public ResultadoBase<typename T::element_type> {
    private:
        T resultado;
        using ResultadoBase<typename T::element_type>::error;
        
    public:
        Resultado() noexcept;
        explicit Resultado(T data) noexcept;
        
        Resultado(const Resultado<T>&) = delete;
        Resultado operator=(const Resultado<T>&) = delete;
        
        Resultado(const Resultado<T>&& otro) noexcept;
        Resultado& operator=(const Resultado<T>&& otro) noexcept;
        
        explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
        explicit Resultado(T data, err::Error error) noexcept;
        
        std::tuple<T, err::Error> Consumir() noexcept;
        ~Resultado() noexcept = default;
        
        operator bool() noexcept;
        std::tuple<T, err::Error> operator()() noexcept;
    };
}
```

### Métodos
- `err::Error Error() const noexcept`: Devuelve el estado del error
- `std::tuple<T, err::Error> Consumir() noexcept`: Devuelve una tupla con el valor y el error
- `operator bool()`: Devuelve verdadero si la operación fue exitosa
- `operator()()`: Alias para Consumir()

### Especializaciones
1. **Valores Directos**
   - Retorna valor inicializado a cero en caso de error
   - Copia el valor en caso de éxito

2. **Punteros Desnudos** (T*)
   - Asume propiedad exclusiva de la memoria apuntada
   - Retorna nullptr en caso de error
   - Libera memoria si no es consumida
   - Semántica de movimiento mediante std::exchange

3. **Punteros Inteligentes** (std::unique_ptr<T>, std::shared_ptr<T>)
   - Semántica de movimiento para transferencia de propiedad
   - Retorna nullptr en caso de error

### Ejemplo
```cpp
// Función que puede fallar con resultado
res::Resultado<float> dividir(float a, float b) {
    if (b == 0.0f) {
        return res::Resultado<float>(0.0f, err::ERROR, "División por cero");
    }
    return res::Resultado<float>(a / b);
}

// Uso
auto [resultado, error] = dividir(10.0f, 2.0f)();
if (error) {
    std::cout << "Error al dividir: " << error.Mensaje();
    return error;  // Propagar el error
}
// Continuar con el resultado
float cociente = resultado;
```