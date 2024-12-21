
#ifndef GENERICOS_H
#define GENERICOS_H
#include <type_traits>

template <typename T, typename = void>
struct tiene_et : std::false_type {};

template <typename T>
struct tiene_et<T, std::void_t<typename T::element_type>> : std::true_type {};

template <typename T, typename = void>
struct tiene_vt : std::false_type {};

template <typename T>
struct tiene_vt<T, std::void_t<typename T::value_type>> : std::true_type {};
#endif