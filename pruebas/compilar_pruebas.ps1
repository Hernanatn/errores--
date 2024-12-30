param(
    [switch]$correr = $false
)
cmake -S . -B compilar;
cmake --build compilar;
Remove-Item -Path "pruebas/pruebas.exe";
Move-Item -Path "compilar/correr_pruebas.exe" -Destination "pruebas/pruebas.exe";

if($correr){
pruebas/pruebas.exe -s
}