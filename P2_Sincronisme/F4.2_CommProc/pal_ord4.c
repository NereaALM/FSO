//*************************************************************************
//
// pal_ord4.c
//
// funcio per moure la paleta de l'ordinador autonomament, en funcio de la
// velocitat de la paleta (variable global v_pal_maq)
// index serà un enter que indicara l’ordre de creació de la paleta
// 		0 -> primera paleta (1)
//		1 -> segona paleta 	(2)
//		...
//		8 -> novena paleta 	(9)
//
//*************************************************************************

#include "tennis4.h"

///*************************************************************************
// 	VARIABLES GLOBALS
///*************************************************************************

// Parametres visuals
int nFil_taulell;
int nCol_taulell;
int long_pal;

// Indexos
int ind;
char ind_pantalla;

// Threads
pthread_t thread_consulta;

// Memoria compartida
int id_taulell;
int * p_taulell;
int id_mem;
mem_compartida * p_mem;

// Sincronisme
int id_sem;

///*************************************************************************
// 	FUNCIONS
///**************************************************************************

// Thread per
void * consulta_bustia(void * cap)
{
	char missatge;
	int long_miss; // [Bytes]

	char elem_darrere;

	do
	{
		long_miss = receiveM(p_mem->ids_busties[ind], missatge);

		elem_darrere = win_quincar(p_mem->fil_pal_maq[ind], p_mem->col_pal_maq[ind] + 1);
		
		if (elem_darrere == ' ')
			// Moure cap a darrere
		else if (elem_darrere == '.')
			// Eliminar procés
		else //sendM(win_quincar(fil_hipo, col_hipo), missatge, MAX_MISS);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	return 0;
}

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	//***************** VARIABLES LOCALS DEL PROCÉS *************************

	int fil_hipotetica;

	int thread_output;

	//************ INICIALITZACIONS & CONTROL D'ERRORS ***********************

	// Carregar parametres d'entrada del procés en variables

	// rang i: 			[0, 8]
	ind = atoi(ll_args[1]);
	// rang char_index: [1, 9]
	ind_pantalla = (char) ind + 1;

	nFil_taulell = atoi(ll_args[2]);
	nCol_taulell = atoi(ll_args[3]);
	long_pal = atoi(ll_args[4]);
	id_taulell = atoi(ll_args[5]);
	id_mem = atoi(ll_args[6]);
	id_sem = atoi(ll_args[7]);

	// Mapejar zona de memoria compartida
	p_taulell = map_mem(id_taulell);
	if (p_taulell == (int *) -1)
	{
		fprintf(stderr, "P %c: error en identificador de memoria del taulell\n", ind_pantalla);
		exit(0);
	}
	p_mem = map_mem(id_mem);
	if (p_mem == (mem_compartida *) -1)
	{
		fprintf(stderr, "P %c: error en identificador de memoria de l'struct\n", ind_pantalla);
		exit(0);
	}

	// Inicialitzem la bústia del procés
	waitS(id_sem);
	p_mem->ids_busties[ind] = ini_mis();
	signalS(id_sem);

	// Obtenir accés a la pantalla
	waitS(id_sem);
	win_set(p_taulell, nFil_taulell, nCol_taulell);
	signalS(id_sem);

	// Creacio de thread per espera a missatges
	pthread_create(&thread_consulta, NULL, consulta_bustia, NULL);

	//****************************** JOC ***********************************
	do
	{
		fil_hipotetica = p_mem->pVertical_pal_maq[ind] + p_mem->v_pal_maq[ind]; // posicio hipotetica de la paleta
		if (fil_hipotetica != p_mem->fil_pal_maq[ind])				 // si pos. hipotetica no coincideix amb pos. actual
		{
			if (p_mem->v_pal_maq[ind] > 0.0) // verificar moviment cap avall
			{
				waitS(id_sem);
				if (win_quincar(fil_hipotetica + long_pal - 1, p_mem->col_pal_maq[ind]) == ' ') // si no hi ha obstacle
				{
					win_escricar(p_mem->fil_pal_maq[ind], p_mem->col_pal_maq[ind], ' ', NO_INV); // esborra primer bloc
					p_mem->pVertical_pal_maq[ind] += p_mem->v_pal_maq[ind];
					p_mem->fil_pal_maq[ind] = p_mem->pVertical_pal_maq[ind];									// actualitza posicio
					win_escricar(p_mem->fil_pal_maq[ind] + long_pal - 1, p_mem->col_pal_maq[ind], '0' + ind_pantalla, INVERS); // impr. ultim bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					p_mem->v_pal_maq[ind] = -p_mem->v_pal_maq[ind];
				signalS(id_sem);
			}
			else // verificar moviment cap amunt
			{
				waitS(id_sem);
				if (win_quincar(fil_hipotetica, p_mem->col_pal_maq[ind]) == ' ') // si no hi ha obstacle
				{
					win_escricar(p_mem->fil_pal_maq[ind] + long_pal - 1, p_mem->col_pal_maq[ind], ' ', NO_INV); // esbo. ultim bloc
					p_mem->pVertical_pal_maq[ind] += p_mem->v_pal_maq[ind];
					p_mem->fil_pal_maq[ind] = p_mem->pVertical_pal_maq[ind];					 // actualitza posicio
					win_escricar(p_mem->fil_pal_maq[ind], p_mem->col_pal_maq[ind], '0' + ind_pantalla, INVERS); // impr. primer bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					p_mem->v_pal_maq[ind] = -p_mem->v_pal_maq[ind];
				signalS(id_sem);
			}
		}
		else
		{
			waitS(id_sem);
			p_mem->pVertical_pal_maq[ind] += p_mem->v_pal_maq[ind]; // actualitza posicio vertical real de la paleta
			signalS(id_sem);
		}

		win_retard(p_mem->retard);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	//***************************** FI DE JOC *******************************

	// Espera a thread de consulta de busties
	pthread_join(thread_consulta, (void *)(intptr_t) thread_output);

	// Eliminem la bústia del procés
	waitS(id_sem);
	elim_mis(p_mem->ids_busties[ind]);
	signalS(id_sem);

	return 0;
}
