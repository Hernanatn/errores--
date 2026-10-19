#include "Error.hpp"

#include <catch2/catch_all.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "errores--.hpp"

/****************************************************************
 *                      EJEMPLOS BÁSICOS                        *
 * ------------------------------------------------------------ *
 *   Pruebas de los ejemplos básico ofrecidos enREADME.md       *
 *   ---------------------------------------------------------- *
 *     Esta sección incluye pruebas para las funciones para     *
 *     casos de uso simples de las tres abstracciones           *
 *     principales: Error, Opcion<T>, Resultado<T>              *        
 ***************************************************************/
 #include "ejemplo_division.cpp"

TEST_CASE("División", "[resultado][division]") {
    int d = std::get<0>(dividir(10, 2)());
    int r = std::get<0>(res::Resultado<int>(5)());
    REQUIRE( d == r);
    int d0 = std::get<0>(dividir(10, 0)());
    int r0 = std::get<0>(res::Resultado<int>(0, err::Generico())());
    REQUIRE( d0 == r0);
}

/****************************************************************
 *                      EJEMPLO INTEGRAL                        *
 * ------------------------------------------------------------ *
 *   Pruebas del Ejemplo Integral ofrecido en README.md         *
 *   ---------------------------------------------------------- *
 *     Esta sección incluye pruebas para las funciones que      *
 *     simulan un servidor de eco, mostrando cómo manejar       *
 *     errores y respuestas en el contexto de sockets.          *
 ***************************************************************/

#include "ejemplo_integral.cpp"
// Prueba: inicializarServidor (casos válidos e inválidos)
TEST_CASE("inicializarServidor", "[servidor][inicializacion]") {
    // Prueba con un número de puerto válido
    auto [idSocket, error] = inicializarServidor(8080)();
    REQUIRE(*idSocket == *std::make_unique<int>(42));  // El ID del socket debe ser 42
    REQUIRE(!error);  
    
    // Prueba con un número de puerto inválido
    auto [idSocketInvalido, errorInvalido] = inicializarServidor(-1)();
    REQUIRE(idSocketInvalido == std::unique_ptr<int>(nullptr));  // El ID del socket debe ser 0
    REQUIRE(errorInvalido);  // Se espera un error
}

// Prueba: aceptarConexion (casos válidos e inválidos)
TEST_CASE("aceptarConexion", "[servidor][conexion]") {
    // Prueba con un socket de servidor válido
    auto [idClienteSocket, error] = aceptarConexion(std::make_unique<int>(42))();
    REQUIRE(*idClienteSocket == *std::make_unique<int>(24));  // El ID del socket del cliente debe ser 24
    REQUIRE(!error);  // No debe haber error

    // Prueba con un socket de servidor inválido
    auto [idClienteSocketInvalido, errorInvalido] = aceptarConexion(std::unique_ptr<int>(nullptr))();
    REQUIRE(idClienteSocketInvalido == std::unique_ptr<int>(nullptr));  // El socket del cliente es inválido
    REQUIRE(errorInvalido);  // Se espera un error
}

// Prueba: recibirMensaje (casos válidos e inválidos)
TEST_CASE("recibirMensaje", "[servidor][mensaje]") {
    // Prueba con un socket de cliente válido
    auto [mensaje, hayMensaje] = recibirMensaje(std::make_unique<int>(42))();
    REQUIRE(hayMensaje);  // Se debe recibir un mensaje
    REQUIRE(mensaje == "Mensaje de prueba");  // El mensaje simulado

    // Prueba con un socket de cliente inválido
    auto [mensajeVacio, hayMensajeVacio] = recibirMensaje(0)();
    REQUIRE(!hayMensajeVacio);  // No debe recibirse un mensaje
}

// Prueba: enviarMensaje (casos válidos e inválidos)
TEST_CASE("enviarMensaje", "[servidor][envio]") {
    // Prueba con un socket de cliente válido y mensaje
    auto error = enviarMensaje(std::make_unique<int>(42), "Hola cliente");
    REQUIRE(!error);  // El envío debe ser exitoso

    // Prueba con un socket de cliente inválido
    auto errorInvalido = enviarMensaje(0, "Hola cliente");
    REQUIRE(errorInvalido);  // Se espera un error

    // Prueba con un mensaje vacío
    auto errorMensajeVacio = enviarMensaje(std::make_unique<int>(42), "");
    REQUIRE(errorMensajeVacio);  // Se debe generar un error por mensaje vacío
}

/****************************************************************
 *                    PRUEBAS DE PUNTEROS                       *
 * ------------------------------------------------------------ *
 *   Pruebas para verificar el manejo correcto de punteros      *
 *   desnudos y inteligentes en Opcion<T> y Resultado<T>          *
 ***************************************************************/

// Estructura de prueba
struct DatosPrueba {
    int valor;
    explicit DatosPrueba(int v) : valor(v) {}
};

/****************************************************************
 *                    PRUEBAS DE OPCION                         *
 ****************************************************************/


TEST_CASE("Opcion con punteros desnudos", "[opcion][punteros]") {
    SECTION("Construcción y consumo de puntero válido") {
        DatosPrueba* datos = new DatosPrueba(42);
        opc::Opcion<DatosPrueba*> opcion(datos);
        
        REQUIRE(!opcion.estaVacia());
        
        auto [ptr, existe] = opcion();
        REQUIRE(existe);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
        
        delete ptr; // Limpieza manual necesaria después de consumir
    }

    SECTION("Opción vacía con puntero desnudo") {
        opc::Opcion<DatosPrueba*> opcionVacia;
        
        REQUIRE(opcionVacia.estaVacia());
        
        auto [ptr, existe] = opcionVacia();
        REQUIRE(!existe);
        REQUIRE(ptr == nullptr);
    }
}


TEST_CASE("Opcion con unique_ptr", "[opcion][smart_ptr]") {
    SECTION("Construcción y consumo de unique_ptr válido") {
        opc::Opcion<std::unique_ptr<DatosPrueba>> opcion(std::move(std::make_unique<DatosPrueba>(42)));
        
        REQUIRE(!opcion.estaVacia());
        
        auto [ptr, existe] = opcion();
        REQUIRE(existe);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
    }

    SECTION("Opción vacía con unique_ptr") {
        opc::Opcion<std::unique_ptr<DatosPrueba>> opcionVacia;
        
        REQUIRE(opcionVacia.estaVacia());
        
        auto [ptr, existe] = opcionVacia();
        REQUIRE(!existe);
        REQUIRE(ptr == nullptr);
    }
}

TEST_CASE("Opcion con shared_ptr", "[opcion][smart_ptr]") {
    SECTION("Construcción y consumo de shared_ptr válido") {
        auto datos = std::make_shared<DatosPrueba>(42);
        opc::Opcion<std::shared_ptr<DatosPrueba>> opcion(std::move(datos));
        
        REQUIRE(!opcion.estaVacia());
        
        auto [ptr, existe] = opcion();
        REQUIRE(existe);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
        REQUIRE(ptr.use_count() == 1); // Verificar que es el único propietario
    }

    SECTION("Opción vacía con shared_ptr") {
        opc::Opcion<std::shared_ptr<DatosPrueba>> opcionVacia{};
        
        REQUIRE(opcionVacia.estaVacia());
        
        auto [ptr, existe] = opcionVacia();
        REQUIRE(!existe);
        REQUIRE(ptr == nullptr);
    }
}

/****************************************************************
 *                  PRUEBAS DE RESULTADO                        *
 ****************************************************************/

TEST_CASE("Resultado con punteros desnudos", "[resultado][punteros]") {
    SECTION("Construcción y consumo exitoso") {
        DatosPrueba* datos = new DatosPrueba(42);
        res::Resultado<DatosPrueba*> resultado(datos);
        
        REQUIRE(!resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(!error);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
        
        delete ptr;
    }

    SECTION("Resultado con error") {
        res::Resultado<DatosPrueba*> resultado(nullptr, err::ERROR, "Error de prueba");
        
        REQUIRE(resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(error);
        REQUIRE(ptr == nullptr);
    }
}

TEST_CASE("Resultado con unique_ptr", "[resultado][smart_ptr]") {
    SECTION("Construcción y consumo exitoso") {
        auto datos = std::make_unique<DatosPrueba>(42);
        res::Resultado<std::unique_ptr<DatosPrueba>> resultado(std::move(datos));
        
        REQUIRE(!resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(!error);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
    }

    SECTION("Resultado con error") {
        res::Resultado<std::unique_ptr<DatosPrueba>> resultado(
            nullptr, 
            err::ERROR, 
            "Error de prueba"
        );
        
        REQUIRE(resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(error);
        REQUIRE(ptr == nullptr);
    }
}

TEST_CASE("Resultado con shared_ptr", "[resultado][smart_ptr]") {
    SECTION("Construcción y consumo exitoso") {
        auto datos = std::make_shared<DatosPrueba>(42);
        res::Resultado<std::shared_ptr<DatosPrueba>> resultado(std::move(datos));
        
        REQUIRE(!resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(!error);
        REQUIRE(ptr->valor == 42);
        REQUIRE(ptr != nullptr);
        REQUIRE(ptr.use_count() == 1);
    }

    SECTION("Resultado con error") {
        res::Resultado<std::shared_ptr<DatosPrueba>> resultado(
            nullptr, 
            err::ERROR, 
            "Error de prueba"
        );
        
        REQUIRE(resultado.Error());
        
        auto [ptr, error] = resultado();
        REQUIRE(error);
        REQUIRE(ptr == nullptr);
    }
}

int main(int argc, char* argv[]) {
    Catch::Session session; 
    int codigo = session.applyCommandLine(argc, argv);
    if (codigo == 0) {
        codigo = session.run();
    }
    // Devolver 1 si alguna prueba falló.
    return (codigo == 0) ? 0 : 1;
}

/****************************************************************
 *                  ARREGLOS PARA EL TALLER                     *
 * ------------------------------------------------------------ *
 *   Categoría del error, movimiento de Resultado<T*> y         *
 *   Resultado<unique_ptr>, la macro ERROR de <windows.h> y     *
 *   las definiciones de cabecera en dos unidades               *
 *   (otra_unidad.cpp).                                         *
 ***************************************************************/

int macroErrorDespuesDeIncluir();
err::Error errorDeOtraUnidad();

TEST_CASE("Error con categoría", "[error][categoria]") {
    err::Error sin = err::Generico("sin categoría");
    REQUIRE(sin.Categoria().empty());
    err::Error con = err::Generico("no existe el archivo", "no-encontrado");
    REQUIRE(con.Categoria() == "no-encontrado");
    REQUIRE(con.Codigo() == err::CodigoEstado::ERROR);
    REQUIRE(con.Mensaje().find("no existe el archivo") != std::string::npos);
    err::Error fatal(err::CodigoEstado::FATAL, "se rompió", "interno");
    REQUIRE(fatal.Categoria() == "interno");
    REQUIRE(fatal.Codigo() == err::CodigoEstado::FATAL);
    // Viaja dentro de un Resultado.
    auto [valor, error] = res::Resultado<int>(0, err::Generico("nada", "prohibido"))();
    REQUIRE(error.Categoria() == "prohibido");
}

TEST_CASE("La macro ERROR de <windows.h> y dos unidades", "[error][macro][odr]") {
    // otra_unidad.cpp definió ERROR antes de incluir: después la ve intacta.
    REQUIRE(macroErrorDespuesDeIncluir() == 0);
    err::Error e = errorDeOtraUnidad();
    REQUIRE(e.Codigo() == err::CodigoEstado::ERROR);
    REQUIRE(e.Categoria() == "otra-unidad");
}

static res::Resultado<int*> resultadoConNombre(int valor) {
    res::Resultado<int*> r(new int(valor));
    return r; // instancia el constructor de movimiento
}

TEST_CASE("Resultado con puntero desnudo se mueve", "[resultado][punteros][movimiento]") {
    res::Resultado<int*> r = resultadoConNombre(42);
    REQUIRE(static_cast<bool>(r));

    res::Resultado<int*> movido(std::move(r));
    auto [nada, errorMovido] = r();
    REQUIRE(nada == nullptr);
    REQUIRE(errorMovido.Mensaje().find("movido") != std::string::npos);

    res::Resultado<int*> otro(new int(7));
    otro = std::move(movido); // libera el 7 y toma el 42
    auto [p, error] = otro();
    REQUIRE_FALSE(error);
    REQUIRE(p != nullptr);
    REQUIRE(*p == 42);
    delete p;
}

TEST_CASE("Resultado con unique_ptr se mueve", "[resultado][smart_ptr][movimiento]") {
    res::Resultado<std::unique_ptr<int>> r(std::make_unique<int>(5));
    res::Resultado<std::unique_ptr<int>> movido(std::move(r));
    res::Resultado<std::unique_ptr<int>> destino;
    destino = std::move(movido);
    auto [p, error] = destino();
    REQUIRE_FALSE(error);
    REQUIRE(p != nullptr);
    REQUIRE(*p == 5);
}
