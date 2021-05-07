//*************************************************************************
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

#include "tennis3.h"

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	//***************** VARIABLES LOCALS DEL PROCÉS *************************

	int fil_hipotetica;

	// Parametres visuals
	int nFil_taulell;
	int nCol_taulell;
	int long_pal;
	
	// Indexos
	int i;
	char ind_pantalla;

	// Memoria compartida
	int id_taulell;
	int * p_taulell;
	int id_mem;
	mem_compartida * p_mem;

	//************************** INICIALITZACIONS ****************************

	// Carregar parametres d'entrada del procés en variables
	
	// rang i: 			[0, 8]
	i = atoi(ll_args[0]);
	// rang char_index: [1, 9]
	ind_pantalla = (char) i + 1;

	nFil_taulell = atoi(ll_args[1]);
	nCol_taulell = atoi(ll_args[2]);
	long_pal = atoi(ll_args[3]);
	id_taulell = atoi(ll_args[4]);
	id_mem = atoi(ll_args[5]);

	// Mapejar zona de memoria compartida
	p_taulell = map_mem(id_taulell);
	p_mem = map_mem(id_mem);

	// Obtenir accés a la pantalla
	win_set(p_taulell, nFil_taulell, nCol_taulell);

	//****************************** JOC ***********************************

	do
	{
		fil_hipotetica = p_mem->pVertical_pal_maq[i] + p_mem->v_pal_maq[i]; // posicio hipotetica de la paleta
		if (fil_hipotetica != p_mem->fil_pal_maq[i])				 // si pos. hipotetica no coincideix amb pos. actual
		{
			if (p_mem->v_pal_maq[i] > 0.0) // verificar moviment cap avall
			{
				if (win_quincar(fil_hipotetica + long_pal - 1, p_mem->col_pal_maq[i]) == ' ') // si no hi ha obstacle
				{
					win_escricar(p_mem->fil_pal_maq[i], p_mem->col_pal_maq[i], ' ', NO_INV); // esborra primer bloc
					p_mem->pVertical_pal_maq[i] += p_mem->v_pal_maq[i];
					p_mem->fil_pal_maq[i] = p_mem->pVertical_pal_maq[i];									// actualitza posicio
					win_escricar(p_mem->fil_pal_maq[i] + long_pal - 1, p_mem->col_pal_maq[i], '0' + ind_pantalla, INVERS); // impr. ultim bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					p_mem->v_pal_maq[i] = -p_mem->v_pal_maq[i];
			}
			else // verificar moviment cap amunt
			{
				if (win_quincar(fil_hipotetica, p_mem->col_pal_maq[i]) == ' ') // si no hi ha obstacle
				{
					win_escricar(p_mem->fil_pal_maq[i] + long_pal - 1, p_mem->col_pal_maq[i], ' ', NO_INV); // esbo. ultim bloc
					p_mem->pVertical_pal_maq[i] += p_mem->v_pal_maq[i];
					p_mem->fil_pal_maq[i] = p_mem->pVertical_pal_maq[i];					 // actualitza posicio
					win_escricar(p_mem->fil_pal_maq[i], p_mem->col_pal_maq[i], '0' + ind_pantalla, INVERS); // impr. primer bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					p_mem->v_pal_maq[i] = -p_mem->v_pal_maq[i];
			}
		}
		else
			p_mem->pVertical_pal_maq[i] += p_mem->v_pal_maq[i]; // actualitza posicio vertical real de la paleta

		win_retard(p_mem->retard);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	return 0;	
}
