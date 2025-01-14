#pragma once
#include "errores--.hpp"

res::Resultado<int> dividir(int a, int b) {
    if (b == 0) {
        return res::Resultado<int>(0, err::ERROR, "No se puede dividir por cero");
    }
    return res::Resultado<int>(a / b);
}