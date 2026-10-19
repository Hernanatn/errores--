// Una segunda unidad de traducción para las pruebas:
// - incluye los encabezados otra vez: si una definición de cabecera no fuera `inline`, el enlace fallaría por
//   definición múltiple;
// - define antes la macro `ERROR` como lo hace <wingdi.h> (vía <windows.h>): los encabezados tienen que compilar igual
//   y devolverle la macro intacta a quien los incluyó.

#ifdef _WIN32
#include <windows.h>
#else
#define ERROR 0
#endif

#include "errores--.hpp"

int macroErrorDespuesDeIncluir() {
    return ERROR;
}

err::Error errorDeOtraUnidad() {
    return err::Generico("desde otra unidad", "otra-unidad");
}
