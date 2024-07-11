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

# Ejecutar Valgrind en terminales separadas con los argumentos especificados
xterm -hold -e "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE1}${RESET}';
               valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE1 $CONFIG1; exec bash" &

xterm -hold -e "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE2}${RESET}';
               valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE2 $CONFIG2; exec bash" &

xterm -hold -e "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE3}${RESET}';
               valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE3 $CONFIG3; exec bash" &

xterm -hold -e "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE4}${RESET}';
               valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE4 $CONFIG4; exec bash" &

xterm -hold -e "echo -e '${GREEN}Ejecutando Valgrind para IO ${NOMBRE5}${RESET}';
               valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/entradasalida $NOMBRE5 $CONFIG5; exec bash" &