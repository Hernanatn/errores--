#Pruebas
La librería incluye una suite (aún en progreso) de pruebas implementada con Catch2, que verifica el comportamiento correcto de todas las abstracciones y sus especializaciones.

Compilar y correr las pruebas:
1. Descargar e instalar CMake;
2. Clonar el repositorio de Catch2;
3. Crear un archivo CMakelists.txt en la raíz del proyecto con la siguiente información:
```cmake
cmake_minimum_required(VERSION 3.14)
project(errores--)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

add_library(errores-- INTERFACE) 
target_include_directories(errores-- INTERFACE fuente)

set(CATCH2_DIR "[[ DIRECTORIO EN EL QUE SE CLONÓ CATCH2 ]]")
add_subdirectory(${CATCH2_DIR} ${CMAKE_BINARY_DIR}/catch2-build)



enable_testing()
add_executable(correr_pruebas pruebas/pruebas.cpp)
target_include_directories(correr_pruebas PRIVATE ${CATCH2_DIR}/src)
target_link_libraries(correr_pruebas PRIVATE errores-- Catch2::Catch2WithMain)

set(CMAKE_VERBOSE_MAKEFILE ON)
include(CTest)
```
4. correr el script [`pruebas.ps1`](/pruebas/compilar_pruebas.ps1) desde la raíz del proyecto.