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
pthread_t thread_bustia;

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

// Thread per consultar si hi ha missatges nous
void * consulta_bustia(void * cap)
{
	// Missatge rebut
	int sentit_hori; // 1: E -> D; -1: E <- D
	int long_miss;

	// Moviment de paletes
	char elem_darrere[long_pal];
	int elem_repetit;
	int hiha_elemD;
	int id_bustia;

	// Iteradors
	int i;
	int j;

	do
	{
		// Rebre missatge i veure si es correcte
		long_miss = receiveM(p_mem->ids_busties[ind], &sentit_hori);

		if (long_miss == LONG_MISS && sentit_hori != 0)
		{

			// Avaluar tipus de xoc
			hiha_elemD = 0;
			waitS(id_sem);
			for (i = 0; i < long_pal && hiha_elemD != 1; i++)
			{
				elem_darrere[i] = win_quincar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind] + sentit_hori);

				// Taulell
				if (elem_darrere[i] == '+')
					hiha_elemD = 1;
				// Paleta
				else if ((int) (elem_darrere[i] - '0') >= MIN_PAL_MAQ &&
						 (int) (elem_darrere[i] - '0') <= MAX_PAL_MAQ)
					hiha_elemD = 2;
			}
			signalS(id_sem);

			// Actuar segons el xoc
			switch (hiha_elemD)
			{
			// CASOS FINALS
			// Moure paleta cap a darrere
			case 0:
				waitS(id_sem);
				for (i = 0; i < long_pal; i++)
				{
					// Esborrar pos actual
					win_escricar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind], ' ', NO_INV);
					// Escriure nova posicio
					win_escricar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind] + sentit_hori, '0' + ind_pantalla, INVERS);
				}
				p_mem->col_pal_maq[ind]+=sentit_hori;
				signalS(id_sem);
				break;
			// Esborrar paleta de pantalla i eliminar procés
			case 1:
				waitS(id_sem);
				// Esborrar paleta
				for (i = 0; i < long_pal; i++)
					win_escricar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind], ' ', NO_INV);
				// Acaba procés
				p_mem->pal_es_viva[ind] = 0;

				signalS(id_sem);
				break;

			// CAS RECURSIU
			case 2:
				// Transmets xoc a totes les paletes que hi hagi darrera
				for (i = 0; i < long_pal; i++)
				{
					// Em busco a mi mateix en els elements visistats per evitar reenviar missatges
					elem_repetit = 0;
					for (j = i; j > 0 && !elem_repetit; j--)
						if (elem_darrere[i] == elem_darrere[j])
							elem_repetit = 1;
					
					// Si no hem enviat el missatje ja i 
					// l'element és dins el rang d'indexos possibles de paletes, enviem el missatge
					if (!elem_repetit &&
						(int) (elem_darrere[i] - '0') >= MIN_PAL_MAQ &&
						(int) (elem_darrere[i] - '0') <= MAX_PAL_MAQ)
					{
						waitS(id_sem);
						id_bustia = p_mem->ids_busties[(int) (elem_darrere[i] - '0')  - 1];
						signalS(id_sem);

						sendM(id_bustia, &sentit_hori, LONG_MISS);
					}
				}
				break;
			
			default:
				break;
			}
		}

	} while (p_mem->tecla != TEC_RETURN && p_mem->num_pilotes > 0  && p_mem->pal_es_viva[ind] == 1);

	return 0;
}

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	//***************** VARIABLES LOCALS DEL PROCÉS *************************

	int fil_hipo;

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

	// Obtenir accés a la pantalla
	waitS(id_sem);
	win_set(p_taulell, nFil_taulell, nCol_taulell);
	signalS(id_sem);

	// Inicialitzem la bústia del procés
	p_mem->ids_busties[ind] = ini_mis();

	// Creacio de thread per espera a missatges
	pthread_create(&thread_bustia, NULL, consulta_bustia, NULL);

	//****************************** JOC ***********************************
	do
	{
		fil_hipo = p_mem->pVertical_pal_maq[ind] + p_mem->v_pal_maq[ind]; // posicio hipotetica de la paleta
		if (fil_hipo != p_mem->fil_pal_maq[ind])				 // si pos. hipotetica no coincideix amb pos. actual
		{
			if (p_mem->v_pal_maq[ind] > 0.0) // verificar moviment cap avall
			{
				waitS(id_sem);
				if (win_quincar(fil_hipo + long_pal - 1, p_mem->col_pal_maq[ind]) == ' ') // si no hi ha obstacle
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
				if (win_quincar(fil_hipo, p_mem->col_pal_maq[ind]) == ' ') // si no hi ha obstacle
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

	} while (p_mem->tecla != TEC_RETURN && p_mem->num_pilotes > 0  && p_mem->pal_es_viva[ind] == 1);

	//***************************** FI DE JOC *******************************

	// Espera a thread de consulta de busties
	pthread_join(thread_bustia, (void *)(intptr_t) thread_output);

	// Eliminem la bústia del procés
	elim_mis(p_mem->ids_busties[ind]);

	return 0;
}
