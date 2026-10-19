#ifndef ERROR_HPP
#define ERROR_HPP


#include <string>

// <windows.h> (vía <wingdi.h>) define la macro `ERROR` (= 0): si se incluye antes que este encabezado, rompe
// `CodigoEstado::ERROR`. Se guarda, se anula mientras dura el encabezado y se restaura al final, así que el código que
// incluyó <windows.h> sigue viendo la suya.
#pragma push_macro("ERROR")
#undef ERROR

namespace err { // Declaración
    enum CodigoEstado
    {
        FATAL = -2,
        ERROR = -1,
        EXITO = 0,
    };

    /**
     * @brief Tipo que representa un error con un código y un mensaje descriptivo.
     *
     * Este tipo se utiliza para encapsular un error mediante un código de estado
     * y un mensaje de error asociado. Los códigos de estado se definen mediante el
     * `enum CodigoEstado`, donde `ERROR` representa un estado de error y `EXITO` indica
     * una operación exitosa.
     *
     * El tipo proporciona mecanismos para obtener el código de estado, el mensaje
     * asociado, y también permite modificar el mensaje mediante la función `agregarMensaje`.
     *
     * Opcionalmente lleva una **categoría**: un texto corto que clasifica el error para quien lo atiende (p. ej.
     * `"no-encontrado"`, `"prohibido"`), sin tener que parsear el mensaje. Por omisión es vacía.
     * Además, sobrecarga varios operadores para permitir el uso conveniente del tipo en
     * expresiones booleanas y de conversión a tipos como `std::string` o `const char*`.
     *
     * @note El operador `<<` permite imprimir un objeto del tipo `Error` utilizando
     * flujos de salida estándar como `std::cout`.
     */
    struct Error{
        protected:
        CodigoEstado codigo;
        std::string mensaje;
        std::string categoria;
        public:
        Error() noexcept : Error{CodigoEstado::ERROR, "ERROR"} {};
        explicit Error(CodigoEstado codigo, std::string mensaje = "ERROR");
        explicit Error(CodigoEstado codigo, std::string mensaje, std::string categoria);
        explicit Error(Error *e); // <HACER/>
        CodigoEstado Codigo();
        std::string Mensaje();
        std::string Categoria() const;

        void agregarMensaje(std::string mensaje);

        operator bool();
        operator std::string() const;
        operator const char*() const;
        operator char*();

    
        // Sobrecarga del operador << para hacer que Error sea "imprimible" con cualquier "output stream", e.g., std::cout.
        friend std::ostream &operator<<(std::ostream &os, Error const &e){
            return os << e.mensaje;
        }
    };
}

namespace err { //Implementación
    inline Error::Error(CodigoEstado codigo, std::string mensaje){
        this->codigo = codigo;
        this->mensaje = "[" + std::to_string(codigo) + "] " + mensaje + "\n";
    };
    inline Error::Error(CodigoEstado codigo, std::string mensaje, std::string categoria) : Error(codigo, mensaje){
        this->categoria = std::move(categoria);
    };
    inline void Error::agregarMensaje(std::string mensaje){
        this->mensaje = this->mensaje.append(mensaje);
    };

    inline std::string Error::Mensaje(){
        return mensaje;
    };
    inline CodigoEstado Error::Codigo(){
        return codigo;
    };
    inline std::string Error::Categoria() const{
        return categoria;
    };

    inline Error::operator bool(){ return (this->codigo != CodigoEstado::EXITO); };

    inline Error::operator std::string() const  { return mensaje ;}
    inline Error::operator const char*() const  { return mensaje.c_str() ;}
    inline Error::operator char*() { return const_cast<char*>(mensaje.c_str()) ;}

    inline Error Exito(std::string mensaje ="Exito"){
        return Error(
            CodigoEstado::EXITO,
            mensaje
        );
    }
    inline Error Fatal(std::string mensaje ="Error Fatal"){
        return Error(
            CodigoEstado::FATAL,
            mensaje
        );
    }
    inline Error Generico(std::string mensaje ="Error"){
        return Error(
            CodigoEstado::ERROR,
            mensaje
        );
    }
    inline Error Generico(std::string mensaje, std::string categoria){
        return Error(
            CodigoEstado::ERROR,
            mensaje,
            categoria
        );
    }
}

#pragma pop_macro("ERROR")
#endif