#ifndef RESULTADO_HPP
#define RESULTADO_HPP

#include <tuple>
#include <memory>
#include <utility>

#include "utiles/Genericos.hpp"
#include "error.hpp"

namespace res { //Declaración
        /**
    * @brief Estructura que representa el resultado de una operación.
    *
    * @tparam T Tipo del valor. Si la operación es exitosa, contiene el valor subyacente, sino un valor inicializado en cero del tipo T.
    *  T uede ser:
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
    class Resultado{
        private:
            T resultado;
            err::Error error;

        protected:

        public:
            Resultado() noexcept
            : Resultado{T{},err::EXITO, "Operación exitosa"} 
            {};

            explicit Resultado(T data) noexcept;
            explicit Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, err::Error error) noexcept;

            err::Error Error() const noexcept;
            std::tuple<T, err::Error>Consumir() noexcept;
            ~Resultado() noexcept;

            operator bool() noexcept;
            std::tuple<T, err::Error> operator()() noexcept;
    };
}

namespace res { // Implementación
    template<typename T>
    Resultado<T>::Resultado(T data, err::Error e) noexcept{
        
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            this->resultado = data;
        } else if constexpr (std::is_pointer<T>::value) {
            this->resultado = data;  
        } else if constexpr (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            this->resultado = std::move(data);
        } else {
            this->resultado = data;
        }
        this->error = e;
    }

    template<typename T>
    Resultado<T>::Resultado(T data) noexcept{
                
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            this->resultado = data;
        } else if constexpr (std::is_pointer<T>::value) {
            this->resultado = data;  
        } else if constexpr (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            this->resultado = std::move(data);
        } else {
            this->resultado = data;
        }
        this->error = err::Exito();
    }

    template<typename T>
    Resultado<T>::Resultado(T data, err::CodigoEstado codigo, std::string mensaje) noexcept{
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            this->resultado = data;
        } else if constexpr (std::is_pointer<T>::value) {
            this->resultado = data;  
        } else if constexpr (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            this->resultado = std::move(data);
        } else {
            this->resultado = data;
        }
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
    err::Error Resultado<T>::Error() const noexcept{
        return this->error;
    }

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::Consumir() noexcept{
        bool ok = !Error();
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            return std::make_tuple(ok ? this->resultado : T{}, error);
        } else if (std::is_pointer<T>::value) { 
            return std::make_tuple(ok ? std::exchange(this->resultado,nullptr) : T{}, error);
        } else if (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            return std::make_tuple(ok ? std::move(this->resultado) : nullptr, error);
        } else {
            return std::make_tuple(ok ? this->resultado : T{}, error);
        }

    }


    template<typename T>
    Resultado<T>::operator bool() noexcept { return !error; }

    template<typename T>
    std::tuple<T, err::Error> Resultado<T>::operator()() noexcept{
        return Consumir();
    }
}

#endif
