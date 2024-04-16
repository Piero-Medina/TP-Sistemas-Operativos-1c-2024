#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>
#include <stdlib.h> 
typedef enum {
    NEW,
    READY,
    EXECUTE,
    BLOCKED,
    EXIT
}estado_pcb;

typedef struct {
    uint32_t PC;   // Program Counter, indica la próxima instrucción a ejecutar
    uint8_t AX;    // Registro Numérico de propósito general
    uint8_t BX;    // Registro Numérico de propósito general
    uint8_t CX;    // Registro Numérico de propósito general
    uint8_t DX;    // Registro Numérico de propósito general
    uint32_t EAX;  // Registro Numérico de propósito general
    uint32_t EBX;  // Registro Numérico de propósito general
    uint32_t ECX;  // Registro Numérico de propósito general
    uint32_t EDX;  // Registro Numérico de propósito general
    uint32_t SI;   // Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string
    uint32_t DI;   // Contiene la dirección lógica de memoria de destino a donde se va a copiar un string
}registros_cpu;

typedef struct {
    int pid;
    uint32_t program_counter;
    int quantum;
    registros_cpu* registros;
    estado_pcb estado;
}t_PCB;

// crea y retorna un puntero a t_PCB 
t_PCB* crear_PCB(int pid, int quantum, estado_pcb estado);
void liberar_PCB(t_PCB* pcb);

// crea y retorna un puntero a registros_cpu 
registros_cpu* crear_registros_cpu();
void liberar_registros_cpu(registros_cpu* registros);

#endif