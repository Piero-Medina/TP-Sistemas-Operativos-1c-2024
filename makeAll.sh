#!/bin/bash

# Lista de directorios
directories=("utils" "cpu" "entradasalida" "kernel" "memoria")

# Iterar sobre cada directorio y ejecutar make
for current_directory in "${directories[@]}"; do
    if [ -d "$current_directory" ]; then
        echo "Limpiando en el directorio $current_directory..."
        (cd "$current_directory" && make clean)
    else
        echo "El directorio $current_directory no existe."
    fi
done

echo "Compilación completada."

# Iterar sobre cada directorio y ejecutar make
for current_directory in "${directories[@]}"; do
    if [ -d "$current_directory" ]; then
        echo "Limpiando en el directorio $current_directory..."
        (cd "$current_directory" && make)
    else
        echo "El directorio $current_directory no existe."
    fi
done

echo "Compilación completada."