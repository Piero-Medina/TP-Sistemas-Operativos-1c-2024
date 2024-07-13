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
NOMBRE1="GENERICA"
CONFIG1="config/prueba_salvation/GENERICA.config"

# Terminal 2
NOMBRE2="ESPERA"
CONFIG2="config/prueba_salvation/ESPERA.config"

# Terminal 3
NOMBRE3="MONITOR"
CONFIG3="config/prueba_salvation/MONITOR.config"

# Terminal 4
NOMBRE4="SLP1"
CONFIG4="config/prueba_salvation/SLP1.config"

# Terminal 5
NOMBRE5="TECLADO"
CONFIG5="config/prueba_io/TECLADO.config"

# Inicia una sesión de tmux llamada 'valgrind-sesion-salvation'
tmux new-session -d -s valgrind-sesion-salvation

# Configura la primera ventana para ejecutar Valgrind para IO GENERICA
tmux send-keys -t valgrind-sesion-salvation:0 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE1}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-salvation:0 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE1 $CONFIG1" C-m

# Crea una segunda ventana en la sesión 'valgrind-sesion-salvation' y configura para IO ESPERA
tmux new-window -t valgrind-sesion-salvation:1
tmux send-keys -t valgrind-sesion-salvation:1 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE2}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-salvation:1 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE2 $CONFIG2" C-m

# Crea una tercera ventana en la sesión 'valgrind-sesion-salvation' y configura para IO MONITOR
tmux new-window -t valgrind-sesion-salvation:2
tmux send-keys -t valgrind-sesion-salvation:2 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE3}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-salvation:2 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE3 $CONFIG3" C-m

# Crea una cuarta ventana en la sesión 'valgrind-sesion-salvation' y configura para IO SLP1
tmux new-window -t valgrind-sesion-salvation:3
tmux send-keys -t valgrind-sesion-salvation:3 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE4}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-salvation:3 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE4 $CONFIG4" C-m

# Crea una quinta ventana en la sesión 'valgrind-sesion-salvation' y configura para IO TECLADO
tmux new-window -t valgrind-sesion-salvation:4
tmux send-keys -t valgrind-sesion-salvation:4 "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE5}${RESET}'" C-m
tmux send-keys -t valgrind-sesion-salvation:4 "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE5 $CONFIG5" C-m

# Adjunta y muestra la sesión de tmux
tmux attach -t valgrind-sesion-salvation
