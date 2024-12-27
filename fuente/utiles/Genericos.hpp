
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