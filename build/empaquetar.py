import os; from sys import argv, path; argc : int = len(argv); RAIZ :str = "\\".join(os.path.dirname(__file__).split("\\")[:-1]); FUENTE : str = f"{RAIZ}\\fuente"; path.append(RAIZ); path.append(FUENTE); os.chdir(RAIZ);

def empaquetar(archivo : str, incluidos : set = None) -> str:
    print(f"Empaquetando: {archivo.split("\\")[-1]}")
    if not incluidos:
        incluidos : set[str] = set()
    buffer = ""
    with open(f"{archivo}", "r", encoding="utf-8") as h:
        for linea in h.readlines():
            buf : str = linea
            inc : set = incluidos
            if '#include "' in linea:
                archivo : str = f"{FUENTE}\\{linea.replace('#include "',"").replace('"',"").replace("/","\\").replace("\n","").strip()}"
                #print(f"<DEBUG/> {incluidos}/{archivo=}")
                
                if archivo not in incluidos:
                    buf, inc = empaquetar(archivo,incluidos)
                else: buf = "\n"

                incluidos.update(inc)
                incluidos.add(archivo)

            buffer += buf

    return (buffer + "\n", incluidos)
    
def main(argc : int, argv : list[str]) -> int :
    _ : str = argv.pop(0)
    
    with open(f"errores--.hpp", "w", encoding="utf-8") as hpp:
        contenido, incluidos = empaquetar(f"{FUENTE}\\errores--.hpp")
        hpp.write(contenido)
    print(f"¡Listo!: {RAIZ}\\errores--.hpp")





        


if __name__ == "__main__":
    main(argc, argv)