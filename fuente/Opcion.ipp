#ifndef OPCION_IPP
#define OPCION_IPP

#ifndef OPCION_HPP
#error __FILE__ solo se debería incluír desde Opcion.hpp.
#endif 
namespace Opcion{
    template <typename T>
    Opcion<T>::Opcion() noexcept{
        this->data = T{};
        this->vacia = true;
    }

    template <typename T>
    Opcion<T>::Opcion(T data) noexcept{
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            this->data = data;
        } else if constexpr (std::is_pointer<T>::value) {
            this->data = data;  
        } else if constexpr (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            this->data = std::move(data);
        } else {
            this->data = data;
        }
        this->vacia = false;
    }
    template <typename T>
    bool Opcion<T>::estaVacia() const noexcept { return vacia; }

    template <typename T> Opcion<T>::~Opcion() {
        if constexpr (std::is_pointer<T>::value) {
            delete this->data;
        }
    }

    template<typename T>
    std::tuple<T, bool> Opcion<T>::Consumir() noexcept{
        bool ok = !this->estaVacia();
        if constexpr (!tiene_et<T>::value || !tiene_vt<T>::value) {
            return std::make_tuple(ok ? this->data : T{}, ok);
        } else if (std::is_pointer<T>::value) { 
            return std::make_tuple(ok ? std::exchange(this->data,nullptr) : T{}, ok);
        } else if (std::is_same<T, std::shared_ptr<typename T::element_type>>::value 
        || std::is_same<T, std::unique_ptr<typename T::element_type>>::value) {
            return std::make_tuple(ok ? std::move(this->data) : nullptr, ok);
        } else {
            return std::make_tuple(ok ? this->data : T{}, ok);
        }
    }

    template<typename T>
    std::tuple<T, bool> Opcion<T>::operator()() noexcept{
            return Consumir();
        }

    template<typename T>
    Opcion<T>::operator bool() noexcept { return !vacia; }

}
#endif