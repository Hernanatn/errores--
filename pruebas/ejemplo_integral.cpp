#pragma once
#include "errores--.hpp"
#include <iostream>
#include <memory>
#include <utility>

// Simula la inicialización de un socket servidor.
// Devuelve un `Resultado` con un identificador de socket o un error.
res::Resultado<std::unique_ptr<int>> inicializarServidor(int puerto) {
    if (puerto <= 0) {
        return res::Resultado<std::unique_ptr<int>>(std::unique_ptr<int>(nullptr), err::CodigoEstado::ERROR, "Puerto inválido");
    }
    // Lógica simulada para inicializar el servidor.
    std::unique_ptr<int> socketID = std::make_unique<int>(42); // ID del socket simulado.
    return res::Resultado<std::unique_ptr<int>>(std::move(socketID));
}

// Simula la aceptación de una nueva conexión.
// Devuelve un `Resultado` con el identificador del cliente conectado.
res::Resultado<std::unique_ptr<int>> aceptarConexion(std::unique_ptr<int> servidorSocket) {
    if (servidorSocket <= 0) {
        return res::Resultado<std::unique_ptr<int>>(std::unique_ptr<int>(nullptr), err::CodigoEstado::ERROR, "Socket del servidor inválido");
    }
    // Lógica simulada para aceptar una conexión.
    std::unique_ptr<int> clienteSocket = std::make_unique<int>(24); // ID del cliente simulado.
    return res::Resultado<std::unique_ptr<int>>(std::move(clienteSocket));
}

// Simula la recepción de un mensaje del cliente.
// Devuelve una `Opcion` con el mensaje recibido o vacío si no hay datos.
opc::Opcion<std::string> recibirMensaje(std::unique_ptr<int> clienteSocket) {
    if (clienteSocket <= 0) {
        return opc::Opcion<std::string>(); // Opción vacía.
    }
    // Lógica simulada para recibir un mensaje.
    return opc::Opcion<std::string>("Mensaje de prueba");
}

// Simula el envío de un mensaje al cliente.
// Devuelve un `Error` en caso de fallo.
err::Error enviarMensaje(std::unique_ptr<int> clienteSocket, const std::string& mensaje) {
    if (clienteSocket <= 0 || mensaje.empty()) {
        return err::Generico("Error al enviar el mensaje");
    }
    // Lógica simulada para enviar un mensaje.
    return err::Exito("Mensaje enviado correctamente");
}

// Simula el cierre de un socket.
void cerrarSocket(std::unique_ptr<int> socketID) {
    // Lógica simulada para cerrar el socket.
    std::cout << "Socket " << *socketID << " cerrado." << std::endl;
}


// Implementación del servidor de eco.
void servidorEco(int puerto) {
    auto [servidorSocket, errorServidor] = inicializarServidor(puerto)();
    if (errorServidor) {
        std::cout << "Error al inicializar el servidor: " << errorServidor << std::endl;
        return;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << " (Socket ID: " << *servidorSocket << ")" << std::endl;

    while (true) {
        auto [clienteSocket, errorConexion] = aceptarConexion(std::move(servidorSocket))();
        if (errorConexion) {
            std::cout << "Error al aceptar conexión: " << errorConexion << std::endl;
            continue;
        }

        std::cout << "Cliente conectado (Socket ID: " << *clienteSocket << ")" << std::endl;

        while (true) {
            auto [mensaje, tieneMensaje] = recibirMensaje(std::move(clienteSocket))();
            if (!tieneMensaje) {
                std::cout << "El cliente desconectó." << std::endl;
                break;
            }

            std::cout << "Mensaje recibido: " << mensaje << std::endl;

            err::Error errorEnvio = enviarMensaje(std::move(clienteSocket), mensaje);
            if (errorEnvio) {
                std::cout << "Error al enviar el mensaje al cliente: " << errorEnvio << std::endl;
                break;
            }
        }

        cerrarSocket(std::move(clienteSocket));
    }
    cerrarSocket(std::move(servidorSocket));
}