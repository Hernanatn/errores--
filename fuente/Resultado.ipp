#ifndef RESULTADO_IPP
#define RESULTADO_IPP

#ifndef RESULTADO_HPP
#error __FILE__ solo se debería incluír desde Resultado.hpp.
#endif 

namespace Resultado{
    template<typename T>
    Resultado<T>::Resultado(T data, Error::Error e) noexcept{
        
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
        this->error = Error::Exito();
    }

    template<typename T>
    Resultado<T>::Resultado(T data, Error::CodigoEstado codigo, std::string mensaje) noexcept{
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
        Error::Error e(codigo,mensaje);
        this->error = e;
    }

    template<typename T>
    Resultado<T>::~Resultado() noexcept{
        if constexpr (std::is_pointer<T>::value) {
            delete resultado; 
        }
    }

    template<typename T>
    Error::Error Resultado<T>::Error() const noexcept{
        return this->error;
    }

    template<typename T>
    std::tuple<T, Error::Error> Resultado<T>::Consumir() noexcept{
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
    std::tuple<T, Error::Error> Resultado<T>::operator()() noexcept{
        return Consumir();
    }
}

#endif