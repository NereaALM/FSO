///*************************************************************************
// 	DEFINICIONS
///*************************************************************************

#include "tennis3.h"

///*************************************************************************
// 	FUNCIONS
///**************************************************************************

// funcio per moure la paleta de l'ordinador autonomament, en funcio de la
// velocitat de la paleta (variable global v_pal_maq)
// index serà un enter que indicara l’ordre de creació de la paleta
// 		0 -> primera paleta (1)
//		1 -> segona paleta 	(2)
//		...
//		8 -> novena paleta 	(9)
void * mou_paleta_ordinador(void * index)
{
	int f_h;
	int i;
	char ind_pantalla;

	// rang i: 			[0, 8]
	i = (int) (intptr_t) index;
	// rang char_index: [1, 9]
	ind_pantalla = (char) (intptr_t) index + 1;

	do
	{
		// TO DO: wait

		f_h = pVertical_pal_maq[i] + v_pal_maq[i]; // posicio hipotetica de la paleta
		if (f_h != fil_pal_maq[i])				 // si pos. hipotetica no coincideix amb pos. actual
		{
			if (v_pal_maq[i] > 0.0) // verificar moviment cap avall
			{
				if (win_quincar(f_h + long_pal - 1, col_pal_maq[i]) == ' ') // si no hi ha obstacle
				{
					win_escricar(fil_pal_maq[i], col_pal_maq[i], ' ', NO_INV); // esborra primer bloc
					pVertical_pal_maq[i] += v_pal_maq[i];
					fil_pal_maq[i] = pVertical_pal_maq[i];									// actualitza posicio
					win_escricar(fil_pal_maq[i] + long_pal - 1, col_pal_maq[i], '0' + ind_pantalla, INVERS); // impr. ultim bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					v_pal_maq[i] = -v_pal_maq[i];
			}
			else // verificar moviment cap amunt
			{
				if (win_quincar(f_h, col_pal_maq[i]) == ' ') // si no hi ha obstacle
				{
					win_escricar(fil_pal_maq[i] + long_pal - 1, col_pal_maq[i], ' ', NO_INV); // esbo. ultim bloc
					pVertical_pal_maq[i] += v_pal_maq[i];
					fil_pal_maq[i] = pVertical_pal_maq[i];					 // actualitza posicio
					win_escricar(fil_pal_maq[i], col_pal_maq[i], '0' + ind_pantalla, INVERS); // impr. primer bloc
				}
				else // si hi ha obstacle, canvia el sentit del moviment
					v_pal_maq[i] = -v_pal_maq[i];
			}
		}
		else
			pVertical_pal_maq[i] += v_pal_maq[i]; // actualitza posicio vertical real de la paleta
		
		// TO DO: signal

		win_retard(retard);

	} while ((tecla != TEC_RETURN) && (num_pilotes > 0));

	return 0;
}

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	
}
