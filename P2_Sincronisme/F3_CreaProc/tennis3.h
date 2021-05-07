//**************************************************************************
//
// Tennis3.h
//
// Aquest fitxer conté les llibrteries, constants i structs que comparteixen
// tennis3.c i pal_ord3.c
//
//**************************************************************************

//******************************* Llibreries********************************

// Generals de C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Threads
#include <pthread.h>

// Processos
#include <sys/wait.h>
#include <unistd.h>	

// Memoria Compartida
#include "memoria.h"

// Pantalla
#include "winsuport2.h"

//*************************** Limits de globals *****************************
#define MIN_FIL 7
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0
#define MIN_PAL_MAQ 1
#define MAX_PAL_MAQ 9
#define MAX_THREADS 2
#define NUM_ARGS_PROC 6
#define MAX_STRING 100

//****************************** Structs ************************************
typedef struct
{
	// Gestio de la partida
	int retard; // [ms]
	int tecla;
	int num_pilotes;

	// Posicio i velocitat de la maquina
	int fil_pal_maq[MAX_PAL_MAQ];
	int col_pal_maq[MAX_PAL_MAQ];
	float pVertical_pal_maq[MAX_PAL_MAQ];
	float v_pal_maq[MAX_PAL_MAQ];

} mem_compartida;
