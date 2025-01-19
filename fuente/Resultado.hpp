#ifndef RESULTADO_HPP
#define RESULTADO_HPP

#include <tuple>
#include <utility>

#include <conceptos.hpp>
#include "Error.hpp"

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
            explicit Resultado() noexcept
                requires utiles::genericos::sin_constructor_por_defecto<T> = delete;
            explicit Resultado() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
                requires utiles::genericos::con_constructor_por_defecto<T>;

            explicit Resultado(T data) noexcept;
            explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, err::Error error) noexcept;
            
            ~Resultado() noexcept;

            std::tuple<T, err::Error>Consumir(T porDefecto) noexcept
                requires utiles::genericos::sin_constructor_por_defecto<T>;
            std::tuple<T, err::Error>Consumir() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
                requires utiles::genericos::con_constructor_por_defecto<T>;

            operator bool() noexcept;
            std::tuple<T, err::Error> operator()(T porDefecto) noexcept
                requires utiles::genericos::sin_constructor_por_defecto<T>;
            std::tuple<T, err::Error> operator()() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
                requires utiles::genericos::con_constructor_por_defecto<T>;
    };

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    struct Resultado<T> : public ResultadoBase<T>{
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
    template <typename T>
    Resultado<T>::Resultado() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
    requires utiles::genericos::con_constructor_por_defecto<T> {
        this->resultado = T{};
        this->error = err::Exito();
    }
    
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
    std::tuple<T, err::Error> Resultado<T>::Consumir(T porDefecto) noexcept
        requires utiles::genericos::sin_constructor_por_defecto<T> {
        bool ok = !this->Error();
        return std::make_tuple(ok ? this->resultado : porDefecto, error);
    };

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::Consumir() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
        requires utiles::genericos::con_constructor_por_defecto<T> {
        bool ok = !this->Error();
        return std::make_tuple(ok ? this->resultado : T{}, error);
    }

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::operator()() noexcept(utiles::genericos::con_constructor_por_defecto<T>)
        requires utiles::genericos::con_constructor_por_defecto<T> {
            return Consumir();
        }

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::operator()(T porDefecto) noexcept
        requires utiles::genericos::sin_constructor_por_defecto<T> {
            return Consumir(porDefecto);
        }

    /*
     *  Especialización para Punteros Desnudos
     */

    template <typename T> requires utiles::genericos::puntero_desnudo<T>
    Resultado<T>::Resultado() noexcept{
        this->resultado = nullptr;
        this->error = err::Exito();
    }

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