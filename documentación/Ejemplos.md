
# Ejemplos

#### Ejemplo 1: Manejo de Errores

En este ejemplo se muestra cómo se pueden utilizar los tipos de error para manejar distintas situaciones en el flujo de un programa.

```cpp
#include "errores--.hpp"

err::Error procesarArchivo(const std::string& nombreArchivo) {
    if (nombreArchivo.empty()) {
        // Si el nombre del archivo está vacío, se genera un error
        err::Error error = err::Generico("El nombre del archivo está vacío");
        return error;
    }


    err::Error exito = Error::Exito("Operación exitosa. Archivo leído");
    std::cout << "Operación exitosa: " << exito << std::endl;
    return exito;
}

int main() {
    err::Error e = procesarArchivo("");  
    if (e) { 
        std::cout << e << std::endl; //Hubo error, se imprime
    }
    e2 = procesarArchivo("archivo.txt");  //EXITO
    if (e2) { // false
        std::cout << e2 << std::endl; //No hubo error, no se imprime
    }
}
```

#### Ejemplo 2: Uso de `Opcion` (Valores Opcionales)

El siguiente ejemplo ilustra cómo se puede usar `Opcion` para manejar valores que pueden o no estar presentes de manera segura.

```cpp
#include "errores--.hpp"

opc::Opcion<int> obtenerValor(bool existe) {
    if (existe) {
        return Opcion<int>(42);  // Valor presente
    } else {
        return Opcion<int>();  // Opción vacía
    }
}

int main() {
    opc::Opcion<int> opcion = obtenerValor(true);  // Opción con valor
    auto [valor, ok] = opcion();
    if (ok) {
        std::cout << "Valor obtenido: " << valor << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }

    // Opción vacía
    opc::Opcion<int> opcionVacia = obtenerValor(false);  // Opción vacía
    auto [valorVacio, ok] = opcionVacia();
    if (ok) {
        std::cout << "Valor obtenido: " << valorVacio << std::endl;
    } else {
        std::cout << "No se obtuvo un valor válido." << std::endl;
    }
}
```

#### Ejemplo 3: Resultados de Operaciones con `Resultado`

En este ejemplo, se utiliza la clase `Resultado` para encapsular tanto el valor como el estado de error de una operación.

```cpp
#include "errores--.hpp"

res::Resultado<int> dividir(int a, int b) {
    if (b == 0) {
        // Error de división por cero
        return Resultado<int>(0, Error::ERROR, "No se puede dividir por cero");
    }
    return Resultado<int>(a / b);  // Operación exitosa
}

int main() {
    res::Resultado<int> resultado = dividir(10, 2);
    auto [valor, error] = resultado();
    if (error) {
        std::cout << error << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valor << std::endl;
    }

    // División por cero
    res::Resultado<int> resultadoError = dividir(10, 0);
    auto [valorError, e] = resultadoError();
    if (e) {
        std::cout << e << std::endl;
    } else {
        std::cout << "Resultado de la división: " << valorError << std::endl;
    }
}
```

### Ejemplo "Integral" - Servidor de Eco

````cpp
#include "errores--.hpp"

#include <iostream>
#include <string>
#include <vector>


// Simula la inicialización de un socket servidor.
// Devuelve un `Resultado` con un identificador de socket o un error.
res::Resultado<int> inicializarServidor(int puerto) {
    if (puerto <= 0) {
        return res::Resultado<int>(0, CodigoEstado::ERROR, "Puerto inválido");
    }
    // Lógica simulada para inicializar el servidor.
    int socketID = 42; // ID del socket simulado.
    return res::Resultado<int>(socketID);
}

// Simula la aceptación de una nueva conexión.
// Devuelve un `Resultado` con el identificador del cliente conectado.
res::Resultado<int> aceptarConexion(int servidorSocket) {
    if (servidorSocket <= 0) {
        return res::Resultado<int>(0, err::CodigoEstado::ERROR, "Socket del servidor inválido");
    }
    // Lógica simulada para aceptar una conexión.
    int clienteSocket = 24; // ID del cliente simulado.
    return res::Resultado<int>(clienteSocket);
}

// Simula la recepción de un mensaje del cliente.
// Devuelve una `Opcion` con el mensaje recibido o vacío si no hay datos.
opc::Opcion<std::string> recibirMensaje(int clienteSocket) {
    if (clienteSocket <= 0) {
        return opc::Opcion<std::string>(); // Opción vacía.
    }
    // Lógica simulada para recibir un mensaje.
    return opc::Opcion<std::string>("Mensaje de prueba");
}

// Simula el envío de un mensaje al cliente.
// Devuelve un `Error` en caso de fallo.
err::Error enviarMensaje(int clienteSocket, const std::string& mensaje) {
    if (clienteSocket <= 0 || mensaje.empty()) {
        return err::Generico("Error al enviar el mensaje");
    }
    // Lógica simulada para enviar un mensaje.
    return err::Exito("Mensaje enviado correctamente");
}

// Simula el cierre de un socket.
void cerrarSocket(int socketID) {
    // Lógica simulada para cerrar el socket.
    std::cout << "Socket " << socketID << " cerrado." << std::endl;
}

// Implementación del servidor de eco.
void servidorEco(int puerto) {
    auto [servidorSocket, errorServidor] = inicializarServidor(puerto)();
    if (errorServidor) {
        std::cout << "Error al inicializar el servidor: " << errorServidor << std::endl;
        return;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << " (Socket ID: " << servidorSocket << ")" << std::endl;

    while (true) {
        auto [clienteSocket, errorConexion] = aceptarConexion(servidorSocket)();
        if (errorConexion) {
            std::cout << "Error al aceptar conexión: " << errorConexion << std::endl;
            continue;
        }

        std::cout << "Cliente conectado (Socket ID: " << clienteSocket << ")" << std::endl;

        while (true) {
            auto [mensaje, tieneMensaje] = recibirMensaje(clienteSocket)();
            if (!tieneMensaje) {
                std::cout << "El cliente desconectó." << std::endl;
                break;
            }

            std::cout << "Mensaje recibido: " << mensaje << std::endl;

            err::Error errorEnvio = enviarMensaje(clienteSocket, mensaje);
            if (errorEnvio) {
                std::cout << "Error al enviar el mensaje al cliente: " << errorEnvio << std::endl;
                break;
            }
        }

        cerrarSocket(clienteSocket);
    }

    cerrarSocket(servidorSocket);
}

int main() {
    servidorEco(8080);
    return 0;
}

```

