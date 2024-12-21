#ifndef RESULTADO_HPP
#define RESULTADO_HPP
#include "utiles/Genericos.hpp"
#include "Error.hpp"
#include <tuple>
#include <memory>
#include <utility>

namespace Resultado {
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
            Error::Error error;

        protected:

        public:
            Resultado() noexcept
            : Resultado{T{},Error::Exito()} 
            {};

            explicit Resultado(T data) noexcept;
            explicit Resultado(T data, Error::CodigoEstado codigo, std::string mensaje) noexcept;
            explicit Resultado(T data, Error::Error error) noexcept;

            Error::Error Error() const noexcept;
            std::tuple<T, Error::Error>Consumir() noexcept;
            ~Resultado() noexcept;

            operator bool() noexcept;
            std::tuple<T, Error::Error> operator()() noexcept;
    };
}

#include "Resultado.ipp"

#endif