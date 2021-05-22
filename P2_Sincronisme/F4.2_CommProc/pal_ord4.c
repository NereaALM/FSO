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

// Quan es rep un xoc pots efectuar un moviment cap enrere, transmetre el xoc
// o sortir de taulell. Aquesta funció efectua aquestes accions.
// Per mirar el que hi ha darrere necessita consultar el sentit horitzontal
void rebre_xoc(int sentit_hori)
{
	// Elements de darrere de la paleta actual
	char elem_darrere[long_pal];
	int elem_repetit;
	int hiha_elemD;
	int index_elem;

	// Bustia de element de darrere amb que xoquem
	int id_bustia;

	// Iteradors
	int i;
	int j;

	waitS(id_sem);

	// Avaluar tipus de xoc mirant elements de darrere
	hiha_elemD = 0;
	for (i = 0; i < long_pal && hiha_elemD != 1; i++)
	{
		elem_darrere[i] = win_quincar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind] + sentit_hori);

		// Xoc amb taulell
		if (p_mem->col_pal_maq[ind] + sentit_hori >= nCol_taulell - 1)
			hiha_elemD = 1;
		
		// Xoc amb paleta
		else if ((int) (elem_darrere[i] - '0') >= MIN_PAL_MAQ &&
				(int) (elem_darrere[i] - '0') <= p_mem->num_pal_maq)
			hiha_elemD = 2;
	}

	// Actuar segons el xoc
	switch (hiha_elemD)
	{
	// CASOS FINALS
	// Moure paleta cap a darrere
	case 0:
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
		// Esborrar paleta
		for (i = 0; i < long_pal; i++)
			win_escricar(p_mem->fil_pal_maq[ind] + i, p_mem->col_pal_maq[ind], ' ', NO_INV);
		// Acaba procés
		p_mem->pal_es_viva[ind] = 0;
		p_mem->num_pal_vives--;

		signalS(id_sem);
		
		break;

	// CAS RECURSIU
	case 2:
		
		signalS(id_sem);

		// Transmets xoc a totes les paletes que hi hagi darrera
		for (i = 0; i < long_pal; i++)
		{
			// Em busco a mi mateix en els elements visistats per evitar reenviar missatges
			elem_repetit = 0;
			for (j = i - 1; j >= 0 && !elem_repetit; j--)
				if (elem_darrere[i] == elem_darrere[j])
					elem_repetit = 1;
			
			// Si no hem enviat el missatje ja i 
			// l'element és dins el rang d'indexos possibles de paletes, enviem el missatge
			if (!elem_repetit &&
				(int) (elem_darrere[i] - '0') >= MIN_PAL_MAQ &&
				(int) (elem_darrere[i] - '0') <= p_mem->num_pal_maq)
			{
				index_elem = (int) (elem_darrere[i] - '0')  - 1;
				id_bustia = p_mem->ids_busties[index_elem];

				sendM(id_bustia, &sentit_hori, LONG_MISS);

				waitS(id_sem);
				p_mem->miss_pendents[index_elem]++;
				signalS(id_sem);
			}
		}
		break;
	
	default:
		break;
	}
}

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	//***************** VARIABLES LOCALS DEL PROCÉS *************************

	int fil_hipo;

	int sentit_hori;

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

	//****************************** JOC ***********************************
	do
	{
		// Rebre missatge
		if (p_mem->miss_pendents[ind] > 0)
		{
			receiveM(p_mem->ids_busties[ind], &sentit_hori);

			rebre_xoc(sentit_hori);

			waitS(id_sem);
			p_mem->miss_pendents[ind]--;
			signalS(id_sem);
		}

		if (p_mem->pal_es_viva[ind])
		{
			// Moure paleta
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
		}

	} while (p_mem->tecla != TEC_RETURN && p_mem->num_pilotes > 0  && p_mem->pal_es_viva[ind]);

	//***************************** FI DE JOC *******************************

	// Eliminem la bústia del procés
	elim_mis(p_mem->ids_busties[ind]);

	return 0;
}
