#ifndef ERRORES_HPP
#define ERRORES_HPP

#ifndef GENERICOS_H
#define GENERICOS_H
#include <type_traits>
#include <memory>


namespace utiles {
    namespace genericos {
    template <typename T, typename = void>
    struct tiene_et : std::false_type {};

    template <typename T>
    struct tiene_et<T, std::void_t<typename T::element_type>> : std::true_type {};

    template <typename T, typename = void>
    struct tiene_vt : std::false_type {};

    template <typename T>
    struct tiene_vt<T, std::void_t<typename T::value_type>> : std::true_type {};

    template<typename T>
    struct es_solo_movible {
    private:
        static constexpr bool es_movible = 
            std::is_move_constructible_v<T> && 
            std::is_move_assignable_v<T>;
        
        static constexpr bool es_copiable = 
            std::is_copy_constructible_v<T> && 
            std::is_copy_assignable_v<T>;
        
    public:
        static constexpr bool value = es_movible && !es_copiable;
    };

    template<typename T>
    concept puntero_inteligente = 
        std::is_same_v<T, std::shared_ptr<typename T::element_type>> ||
        std::is_same_v<T, std::unique_ptr<typename T::element_type>>;

    template<typename  T>
    concept puntero_desnudo = std::is_pointer<T>::value;

    template<typename  T>
    concept puntero =
        puntero_desnudo<T> ||
        puntero_inteligente<T>;

    }
}

#endif
#ifndef ERROR_HPP
#define ERROR_HPP


#include <string>


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
        public:
        Error() noexcept : Error{CodigoEstado::ERROR, "ERROR"} {};
        explicit Error(CodigoEstado codigo, std::string mensaje = "ERROR");
        explicit Error(Error *e); // <HACER/>
        CodigoEstado Codigo();
        std::string Mensaje();

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
    inline void Error::agregarMensaje(std::string mensaje){
        this->mensaje = this->mensaje.append(mensaje);
    };

    inline std::string Error::Mensaje(){
        return mensaje;
    };
    inline CodigoEstado Error::Codigo(){
        return codigo;
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
}


#endif

#ifndef OPCION_HPP
#define OPCION_HPP

#include <tuple>
#include <utility>


 
namespace opc { // Declaración
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

    /**
    * @brief Estructura que representa un tipo de opción, que puede contener un
    * valor de tipo T o estar vacía (sin valor).
    *
    * Esta estructura es útil para representar un valor opcional, como los tipos de
    * opción en otros lenguajes, permitiendo que un valor de tipo `T` pueda estar
    * presente o ausente explícitamente.
    *
    * @tparam T Tipo del valor almacenado. Puede ser:
    * - Un valor directo (ej. `int`, `std::string`, etc.).
    * - Un puntero inteligente (`std::unique_ptr<T>` o `std::shared_ptr<T>`).
    * - Un puntero desnudo a memoria no compartida (ej. `T*`), en cuyo caso se
    * asume que la memoria es propiedad exclusiva de la estructura y será liberada
    * al destruir la instancia si no es cedida al `Consumir`la.
    *
    * **Restricciones importantes**:
    * - Si `T` es un puntero desnudo (`T*`), se asume que la memoria a la que
    * apunta es propiedad exclusiva de esta instancia, **no debe ser compartida**.
    * - No se debe pasar un puntero desnudo que apunte a memoria gestionada por
    * `std::shared_ptr` o cualquier otra técnica de manejo de memoria compartida,
    * ya que podría resultar en un comportamiento indefinido (por ejemplo, doble
    * liberación de memoria).
    *
    * **Operadores**:
    * - El operador `operator()` "eleva" el valor contenido en una instancia de
    * `Opcion<T>` a una tupla que contiene el valor si está presente, o un valor
    * por defecto (inicialización a cero) si no lo está. Este operador devuelve una
    * tupla de tipo `std::tuple<T, bool>`, donde el segundo valor (`bool`) indica
    * si la instancia contenía un valor válido.
    *
    * **Notas**:
    * - Si se utiliza un puntero desnudo, se recomienda envolverlo en un puntero
    * inteligente (por ejemplo, `std::unique_ptr`) para mejorar la seguridad y
    * claridad del código.
    */
    template <typename T> 
    struct Opcion : public OpcionBase<T>{
        private:
        T data;
        using OpcionBase<T>::vacia;  

        public:
        Opcion() noexcept;
        explicit Opcion(T data) noexcept;

        /**
        * @brief Consumir "eleva" el valor de la opción y la "consume" - transfiere la propiedad de la data subyacente si es un puntero.
        *
        * Este operador devuelve una tupla `std::tuple<T, bool>`. Si la opción
        * contiene un valor, la tupla contiene ese valor y `true`. Si la opción está
        * vacía, la tupla contiene un valor por defecto de tipo `T` (inicializado a
        * cero) y `false`.
        *
        * Si el valor almacenado en la opción es un puntero (ya sea un puntero desnudo, 
        * un `std::shared_ptr` o un `std::unique_ptr`), el operador "consume" la opción y "libera"
        * el recurso, transfiriendo la propiedad del puntero al llamador. Para los punteros
        * inteligentes (`std::shared_ptr` o `std::unique_ptr`), la transferencia de propiedad
        * se realiza mediante un `std::move()`. Para punteros desnudos, se asigna `nullptr` a la opción
        * para evitar el doble manejo del recurso.
        *
        * @return Una tupla que contiene el valor de la opción (o un valor por
        * defecto) y un indicador de si la opción contenía un valor válido (`true` o
        * `false`).
        */
        std::tuple<T, bool> Consumir() noexcept;

        operator bool() noexcept;

        /**
        * @brief Operador de llamada "Consume" la Opcion.
        * @return Una tupla que contiene el valor de la opción (o un valor por
        * defecto) y un indicador de si la opción contenía un valor válido (`true` o
        * `false`).
        */
        std::tuple<T, bool> operator()() noexcept;
    };


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
        operator bool() noexcept;
        std::tuple<T, bool> Consumir() noexcept;
        std::tuple<T, bool> operator()() noexcept;
    };

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

        ~Opcion() noexcept{};
        operator bool() noexcept;
        std::tuple<T, bool> Consumir() noexcept;
        std::tuple<T, bool> operator()() noexcept;
    };
    
}

namespace opc{ // Implementación
    template <typename T>
    Opcion<T>::Opcion() noexcept{
        this->data = T{};
        this->vacia = true;
    }

    template <typename T>
    Opcion<T>::Opcion(T data) noexcept{
        this->data = data;
        this->vacia = false;
    }

    template<typename T>
    std::tuple<T, bool> Opcion<T>::Consumir() noexcept{
        bool ok = !this->estaVacia();
        return std::make_tuple(ok ? this->data : T{}, ok);
    }

    template<typename T>
    std::tuple<T, bool> Opcion<T>::operator()() noexcept{
            return Consumir();
        }



    /*
     *  Especialización para Punteros Desnudos
     */

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Opcion<T>::Opcion() noexcept{
        this->data = T{};
        this->vacia = true;
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Opcion<T>::Opcion(T data) noexcept{
        this->data = data;
        this->vacia = false;
    }

    // Contructor std::move
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Opcion<T>::Opcion(const Opcion<T>&& otro) noexcept{
        this->data = std::exchange(otro->data, nullptr);
        this->vacia = std::exchange(otro->vacia, true);
    }

    // Asignación std::move
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Opcion<T>& Opcion<T>::operator=(const Opcion<T>&& otro) noexcept{
        if (this != &otro){
            this->data = std::exchange(otro->data, nullptr);
            this->vacia = std::exchange(otro->vacia, true);
        }
        return *this;
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Opcion<T>::~Opcion() noexcept{
            delete this->data;
    }


    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    std::tuple<T, bool> Opcion<T>::Consumir() noexcept{
        bool ok = !this->estaVacia();
        this->vacia = true;
        return std::make_tuple(ok ? std::exchange(this->data,nullptr) : nullptr, ok);
    }
    
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    std::tuple<T, bool> Opcion<T>::operator()() noexcept{
        return Consumir();
    }

    /*
     *  Especialización para Punteros Inteligentes
     */

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Opcion<T>::Opcion() noexcept{
        this->data = nullptr;
        this->vacia = true;
    }

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Opcion<T>::Opcion(T data) noexcept{
        this->data = std::move(data);
        this->vacia = false;
    }

    // Contructor std::move
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Opcion<T>::Opcion(const Opcion<T>&& otro) noexcept{
        this->data = std::move(std::exchange(otro->data, nullptr));
        this->vacia = std::exchange(otro->vacia, true);
    }

    // Asignación std::move
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Opcion<T>& Opcion<T>::operator=(const Opcion<T>&& otro) noexcept{
        if (this != &otro){
            this->data = std::move(std::exchange(otro->data, nullptr));
            this->vacia = std::exchange(otro->vacia, true);
        }
        return *this;
    }



    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    std::tuple<T, bool> Opcion<T>::Consumir() noexcept{
        bool ok = !this->estaVacia();
        this->vacia = true;
        return std::make_tuple(ok ? std::move(this->data) : nullptr, ok);

    }

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    std::tuple<T, bool> Opcion<T>::operator()() noexcept{
            return Consumir();
        }

}
#endif


#ifndef RESULTADO_HPP
#define RESULTADO_HPP

#include <tuple>
#include <utility>




namespace res { //Declaración
    template<typename T>
    struct ResultadoBase{
        protected:
            err::Error error;
        public:
            ResultadoBase() noexcept: error(err::EXITO) {};
            virtual ~ResultadoBase() noexcept = default ;

            err::Error Error() const noexcept {return this->error;};
            operator bool() noexcept {return !error;};
    };
    
    /**
    * @brief Estructura que representa el resultado de una operación.
    *
    * @tparam T Tipo del valor. Si la operación es exitosa, contiene el valor subyacente, sino un valor inicializado en cero del tipo T.
    *  T puede ser:
    * - Un valor directo (ej. `int`, `std::string`, etc.).
    * - Un puntero inteligente (`std::unique_ptr<T>` o `std::shared_ptr<T>`).
    * - Un puntero desnudo a memoria no compartida (ej. `T*`), en cuyo caso se
    * asume que la memoria es propiedad exclusiva de la estructura y será liberada
    * al destruir la instancia si no es cedida al `Consumir`la.
    *
    * **Restricciones importantes**:
    * - Si `T` es un puntero desnudo (`T*`), se asume que la memoria a la que
    * apunta es propiedad exclusiva de esta instancia, **no debe ser compartida**.
    * - No se debe pasar un puntero desnudo que apunte a memoria gestionada por
    * `std::shared_ptr` o cualquier otra técnica de manejo de memoria compartida,
    * ya que podría resultar en un comportamiento indefinido (por ejemplo, doble
    * liberación de memoria).
    *
    * **Advertencia**: Los punteros (inteligentes o desnudos) deben ser únicos en propiedad. 
    * La llamada a `operator()` transfiere la propiedad del valor a la parte que llama, liberando
    * el recurso de la instancia original.
    *
    * **Operadores**:
    * - `operator()`: Devuelve una tupla `std::tuple<T, bool>`, donde el segundo valor indica
    *   si el resultado es válido (`true`) o no (`false`).
    */
    template<typename T>
    struct Resultado : public ResultadoBase<T>{
        private:
            T resultado;
            using ResultadoBase<T>::error;

        public:
            Resultado() noexcept : resultado(T{}), ResultadoBase<typename T::element_type>::error(err::Exito()) {};

            explicit Resultado(T data) noexcept;
            explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, err::Error error) noexcept;

            std::tuple<T, err::Error>Consumir() noexcept;
            ~Resultado() noexcept;

            operator bool() noexcept;
            std::tuple<T, err::Error> operator()() noexcept;
    };

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    struct Resultado<T> : public ResultadoBase<T>{
        private:
            T resultado;
            using ResultadoBase<T>::error;

        public:
            Resultado() noexcept : resultado(nullptr), ResultadoBase<typename T::element_type>::error(err::Exito()) {};

            explicit Resultado(T data) noexcept;
            
            Resultado(const Resultado<T>&) = delete;
            Resultado operator=(const Resultado<T>&) = delete;

            Resultado(const Resultado<T>&& otro) noexcept;
            Resultado& operator=(const Resultado<T>&& otro) noexcept;
            
            explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, err::Error error) noexcept;

            std::tuple<T, err::Error>Consumir() noexcept;
            ~Resultado() noexcept;

            operator bool() noexcept;
            std::tuple<T, err::Error> operator()() noexcept;
    };

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    struct Resultado<T> : public ResultadoBase<typename T::element_type>{
        private:
            T resultado;
            using ResultadoBase<typename T::element_type>::error;

        public:
            Resultado() noexcept : resultado(nullptr), ResultadoBase<typename T::element_type>::error(err::Exito()) {};

            explicit Resultado(T data) noexcept;
            
            Resultado(const Resultado<T>&) = delete;
            Resultado operator=(const Resultado<T>&) = delete;

            Resultado(const Resultado<T>&& otro) noexcept;
            Resultado& operator=(const Resultado<T>&& otro) noexcept;
            
            explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, err::Error error) noexcept;

            std::tuple<T, err::Error>Consumir() noexcept;
            ~Resultado() noexcept {};

            operator bool() noexcept;
            std::tuple<T, err::Error> operator()() noexcept;
    };
}

namespace res { // Implementación
    template<typename T>
    Resultado<T>::Resultado(T data) noexcept{
        this->resultado = data;        
        this->error = err::Exito();
    }

    template<typename T>
    Resultado<T>::Resultado(T data, err::Error e) noexcept{
        this->resultado = data;
        this->error = e;
    }

    template<typename T>
    Resultado<T>::Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept{
        this->resultado = data;
        err::Error e(codigo,mensaje);
        this->error = e;
    }

    template<typename T>
    Resultado<T>::~Resultado() noexcept{
        if constexpr (std::is_pointer<T>::value) {
            delete resultado; 
        }
    }

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::Consumir() noexcept{
        bool ok = !this->Error();
        return std::make_tuple(ok ? this->resultado : T{}, error);
    }
    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::operator()() noexcept{
        return Consumir();
    }

    /*
     *  Especialización para Punteros Desnudos
     */

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::Resultado(const Resultado<T>&& otro) noexcept{
        this->resultado = std::exchange(otro->resultado, nullptr);
        this->error = std::exchange(otro.error, err::Generico("Resultado movido."));
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>& Resultado<T>::operator=(const Resultado<T>&& otro) noexcept{
        if (this != &otro){
            this->resultado = std::exchange(otro->resultado, nullptr);
            this->error = std::exchange(otro.error, err::Generico("Resultado movido."));
        }
        return *this;
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::Resultado(T data) noexcept{
        this->resultado = data;        
        this->error = err::Exito();
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::Resultado(T data, err::Error e) noexcept{
        this->resultado = data;
        this->error = e;
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept{
        this->resultado = data;
        err::Error e(codigo,mensaje);
        this->error = e;
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::~Resultado() noexcept{
        delete resultado; 
    }

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    std::tuple<T, err::Error> Resultado<T>::Consumir() noexcept{
        bool ok = !this->Error();
        return std::make_tuple(ok ? std::exchange(this->resultado,nullptr) : nullptr, error);

    }
    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    std::tuple<T, err::Error> Resultado<T>::operator()() noexcept{
        return Consumir();
    }


    /*
     *  Especialización para Punteros Inteligentes
     */

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Resultado<T>::Resultado(T data) noexcept{
        this->resultado = std::move(data);
        this->error = err::Exito();
    }

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Resultado<T>::Resultado(T data, err::Error e) noexcept{
        this->resultado = std::move(data);
        this->error = e;
    }
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Resultado<T>::Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept{
        this->resultado = std::move(data);
        err::Error e(codigo,mensaje);
        this->error = e;
    }
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Resultado<T>::Resultado(const Resultado<T>&& otro) noexcept{
        this->resultado = std::move(std::exchange(otro->resultado, nullptr));
        this->error = std::exchange(otro.error, err::Generico("Resultado movido."));
    }

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    Resultado<T>& Resultado<T>::operator=(const Resultado<T>&& otro) noexcept{
        if (this != &otro){
            this->resultado = std::move(std::exchange(otro->resultado, nullptr));
            this->error = std::exchange(otro.error, err::Generico("Resultado movido."));
        }
        return *this;
    }

    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    std::tuple<T, err::Error> Resultado<T>::Consumir() noexcept{
        bool ok = !this->Error();
        return std::make_tuple(ok ? std::move(this->resultado) : nullptr, std::exchange(error,err::Generico("Resultado movido.")));
       
    }
    template <typename T> requires utiles::genericos::puntero_inteligente<T>
    std::tuple<T, err::Error> Resultado<T>::operator()() noexcept{
        return Consumir();
    }
}

#endif

#endif
