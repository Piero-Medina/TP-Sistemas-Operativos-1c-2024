#!/bin/bash

# Variables de colores ANSI con Estilo Negrita (1)
RED="\e[1;31m"
GREEN="\e[1;32m"
BLUE="\e[1;34m"
PURPLE="\e[1;35m"
RESET="\e[0m"  # Restablecer los colores a los valores predeterminados

# Guardar el directorio de trabajo actual
WORK_DIR="$(pwd)"
echo -e "Directorio de Trabajo: $WORK_DIR"

# Compilar el proyecto
echo -e "${BLUE}Limpiando Módulo${RESET}"
make clean

echo -e "${BLUE}Compilando Módulo${RESET}"
make 

# Definir los valores de las variables para cada terminal
# Terminal 1
NOMBRE1="FS"
CONFIG1="config/prueba_fs/FS.config"

# Terminal 2
NOMBRE2="TECLADO"
CONFIG2="config/prueba_fs/TECLADO.config"

# Terminal 3
NOMBRE3="MONITOR"
CONFIG3="config/prueba_fs/MONITOR.config"

# Inicia una sesión de tmux llamada 'valgrind-sesion-fs'
tmux new-session -d -s valgrind-sesion-fs

# Configura la primera ventana para ejecutar Valgrind para IO FS
tmux send-keys -t valgrind-sesion-fs:0 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE1}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-fs:0 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE1 $CONFIG1" C-m

# Crea una segunda ventana en la sesión 'valgrind-sesion-fs' y configura para IO TECLADO
tmux new-window -t valgrind-sesion-fs:1
tmux send-keys -t valgrind-sesion-fs:1 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE2}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-fs:1 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE2 $CONFIG2" C-m

# Crea una tercera ventana en la sesión 'valgrind-sesion-fs' y configura para IO MONITOR
tmux new-window -t valgrind-sesion-fs:2
tmux send-keys -t valgrind-sesion-fs:2 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE3}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-fs:2 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE3 $CONFIG3" C-m

# Adjunta y muestra la sesión de tmux
tmux attach -t valgrind-sesion-fs
