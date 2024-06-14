#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h> 

#include <string.h>
#include <stdint.h>

// enum usado para los getters y setter en los registros 
typedef enum {
    AX,
    BX,
    CX,
    DX,
    EAX,
    EBX,
    ECX,
    EDX,
    SI,
    DI,
    PC 
}e_registro;

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
    uint32_t pid;
    uint32_t program_counter;
    uint32_t quantum;
    registros_cpu* registros;
    estado_pcb estado;
}t_PCB;

/////////////////////////////// Adminitracion de Estructuras ///////////////////////////////////

 t_PCB* crear_PCB(uint32_t pid, uint32_t quantum, estado_pcb estado);
 void liberar_PCB(t_PCB* pcb);

 registros_cpu* crear_registros_cpu();
 void liberar_registros_cpu(registros_cpu* registros);

 char* estado_to_string(t_PCB* pcb);

 // - devuelve el tamanio en bytes segun el nombre del registro
 size_t obtener_tamano_registro(t_PCB* pcb, const char* nombre_registro);

/////////////////////////////// Setter y Getter registros_cpu ///////////////////////////////////

 e_registro obtener_registro_por_nombre(char *nombre);
 void set_registro(t_PCB *pcb, uint32_t valor, e_registro registro);
 uint32_t get_registro(t_PCB *pcb, e_registro registro);

/////////////////////////////// imprimir t_PCB /////////////////////////////////////////////////
 
 void imprimir_estado_pcb(estado_pcb estado);
 void imprimir_registros_cpu(registros_cpu* registros);
 void imprimir_t_PCB(t_PCB* pcb);

////////////////////////////////////////////////////////////////////////////////////////////////
#endif