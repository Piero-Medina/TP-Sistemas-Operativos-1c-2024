#!/bin/bash

# Lista de directorios
directories=("cpu" "entradasalida" "kernel" "memoria")

# Función para limpiar archivos .log en un directorio
clean_logs() {
    local dir="$1"
    echo "Limpiando archivos .log en el directorio $dir..."
    find "$dir" -type f -name "*.log" -exec sh -c '> {}' \;
}

# Iterar sobre cada directorio y limpiar archivos .log
for current_directory in "${directories[@]}"; do
    if [ -d "$current_directory" ]; then
        clean_logs "$current_directory"
    else
        echo "El directorio $current_directory no existe."
    fi
done

echo "Limpieza de archivos .log completada en todos los directorios."
