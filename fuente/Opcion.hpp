#ifndef OPCION_HPP
#define OPCION_HPP

#include <tuple>
#include <utility>

#include "utiles/Genericos.hpp"
 
namespace opc { // Declaración
    // [DEBUG] GITHUB WORKFLOWS
    // [DEBUG] GITHUB v2
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

