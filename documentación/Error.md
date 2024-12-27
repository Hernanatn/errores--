# Error

### Descripción General
La clase `Error` encapsula información sobre errores mediante un código de estado y un mensaje descriptivo. Esta abstracción permite identificar claramente el estado de una operación y manejar errores de forma explícita, evitando el uso de excepciones.

### Definición del Tipo
```cpp
namespace err {
    enum CodigoEstado {
        FATAL = -2,  // Error irrecuperable
        ERROR = -1,  // Error recuperable
        EXITO = 0    // Operación exitosa
    };

    struct Error {
        private:
            CodigoEstado codigo;
            std::string mensaje;
        
        public:
            Error() noexcept : Error{CodigoEstado::ERROR, "ERROR"} {};
            explicit Error(CodigoEstado codigo, std::string mensaje = "ERROR");
            
            CodigoEstado Codigo();
            std::string Mensaje();
            void agregarMensaje(std::string mensaje);
            
            operator bool();
            operator std::string() const;
            operator const char*() const;
            operator char*();
            
            friend std::ostream &operator<<(std::ostream &os, Error const &e);
    };
}
```

### Métodos
- `CodigoEstado Codigo()`: Devuelve el código de estado del error
- `std::string Mensaje()`: Devuelve el mensaje de error
- `void agregarMensaje(std::string mensaje)`: Agrega texto adicional al mensaje
- `operator bool()`: Devuelve verdadero si hay un error (estado no es EXITO)
- `operator std::string()`: Convierte el error a su representación en cadena
- `operator const char*()`: Convierte el mensaje a cadena estilo C
- `operator char*()`: Convierte el mensaje a cadena estilo C modificable
- `operator<<`: Permite imprimir el error en flujos de salida

### Funciones Utilitarias
```cpp
namespace err {
    inline Error Exito(std::string mensaje = "Exito");
    inline Error Fatal(std::string mensaje = "Error Fatal");
    inline Error Generico(std::string mensaje = "Error");
}
```

### Ejemplo de Uso Idiomático
```cpp
// Función que puede fallar
err::Error validarEntrada(const std::string& entrada) {
    if (entrada.empty()) {
        return err::Generico("Entrada vacía");
    }
    if (entrada.length() < 3) {
        return err::Error(err::ERROR, "Entrada demasiado corta");
    }
    return err::Exito("Validación exitosa");
}

// Uso
auto error = validarEntrada(entrada);
if (error) {
    std::cout << error.Mensaje();
    return error;  // Propagar el error
}
// Continuar con la ejecución normal
```

