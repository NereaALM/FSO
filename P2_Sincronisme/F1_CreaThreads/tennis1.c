//**************************************************************************
//
//	Tennis1.c
//
//	Programa inicial d'exemple per a les practiques 2 i 3 de ISO.
//	Es tracta del joc del tennis: es dibuixa un camp de joc rectangular
//	amb una porteria a cada costat, una paleta per l'usuari, una paleta
//	per l'ordinador i una pilota que va rebotant per tot arreu; l'usuari
//	disposa de dues tecles per controlar la seva paleta, mentre que l'or-
//	dinador mou la seva automaticament (amunt i avall). Evidentment, es
//	tracta d'intentar col.locar la pilota a la porteria de l'ordinador
//	(porteria de la dreta), abans que l'ordinador aconseguixi col.locar
//	la pilota dins la porteria de l'usuari (porteria de l'esquerra).
//
//	Arguments del programa:
//	per controlar la posicio de tots els elements del joc, cal indicar
//	el nom d'un fitxer de text que contindra la seguent informacio:
//	nFil_taulell nCol_taulell mida_porteria long_pal
//	fil_pilota_R col_pilota_R v_fil_pilota_R v_col_pilota_R
//	fil_pal_maq col_pal_maq po_vf
//
//	on 'nFil_taulell', 'nCol_taulell' son les dimensions del taulell de joc,
//	 'mida_porteria'
//	es la mida de les dues porteries, 'long_pal' es la longitud de les dues
//	paletes; 'fil_pilota_R', 'col_pilota_R' es la posicio inicial
//	(fila,columna) de la pilota, mentre que 'v_fil_pilota_R',
//	'v_col_pilota_R' es la velocitat inicial;
//	finalment, 'fil_pal_maq', 'col_pal_maq' indicara la posicio del primer
//	caracter de la paleta de l'ordinador, mentre que la seva velocitat
//	vertica ve determinada pel parametre 'po_fv'.
//
//	A mes, es podra afegir un segon argument opcional per indicar el
//	retard de moviment de la pilota i la paleta de l'ordinador (en ms);
//	el valor d'aquest parametre per defecte es 100 (1 decima de segon).
//
//	Compilar i executar:
//	El programa invoca les funcions definides en 'winsuport.o', les
//	quals proporcionen una interficie senzilla per a crear una finestra
//	de text on es poden imprimir caracters en posicions especifiques de
//	la pantalla (basada en CURSES); per tant, el programa necessita ser
//	compilat amb la llibreria 'curses':
//
//	$ gcc tennis1.c winsuport.o -o tennis1 -lcurses
//	$ tennis1 fit_param [retard]
//
//	Codis de retorn:
//	El programa retorna algun dels seguents codis al SO:
//	0	==>	funcionament normal
//	1	==>	numero d'arguments incorrecte
//	2	==>	fitxer no accessible
//	3	==>	dimensions del taulell incorrectes
//	4	==>	parametres de la pilota incorrectes
//	5	==>	parametres d'alguna de les paletes incorrectes
//	6	==>	no s'ha pogut crear el camp de joc (no pot iniciar CURSES)
///*************************************************************************

///*************************************************************************
// 	DEFINICIONS
///*************************************************************************

// Llibreries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "winsuport.h"
#include <pthread.h>

// Limits de globals
#define MIN_FIL 7
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0
#define MIN_PAL_MAQ 1
#define MAX_PAL_MAQ 9
#define MAX_THREADS 11

// Variables globals

// Mides del camp i elements del joc
int nFil_taulell;
int nCol_taulell;
int mida_porteria;
int long_pal;

// Posicio usuari
int fil_pal_usu;
int col_pal_usu;

// Posicio i velocitat maquina
int fil_pal_maq[MAX_PAL_MAQ];
int col_pal_maq[MAX_PAL_MAQ];
float pVertical_pal_maq[MAX_PAL_MAQ];
float v_pal_maq[MAX_PAL_MAQ];

// Posicio i velocitat pilota
int fil_pilota;
int col_pilota;
float fil_pilota_R;
float col_pilota_R;
float v_fil_pilota_R;
float v_col_pilota_R;

// Retard del joc en ms
int retard;

// Gestio de la partida
// TO DO (opcional): inicialitzar tecla a 0 si C no ho fa
int tecla;
int cont;

// Threads
pthread_t taula_threads[MAX_THREADS];

///*************************************************************************
// 	FUNCIONS
//**************************************************************************

// funcio per realitzar la carrega dels parametres de joc emmagatzemats
// dins un fitxer de text, el nom del qual es passa per referencia en
// 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio
// enviant un missatge per la sortida d'error i retornant el codi per-
// tinent al SO (segons comentaris del principi del programa).
// Input:
// - nom_fitxer amb camp de joc
// Output:
// - nombre de paletes de la maquina
int carrega_parametres(const char * nom_fit)
{
	FILE *fit;
	int num_pal_maq;

	// Obrir fitxer
	fit = fopen(nom_fit, "rt"); 
	if (fit == NULL)
	{
		fprintf(stderr, "No s'ha pogut obrir el fitxer \'%s\'\n", nom_fit);
		exit(2);
	}

	// Llegir mides d'elements de joc
	if (!feof(fit))
		fscanf(fit, "%d %d %d %d\n", &nFil_taulell, &nCol_taulell, &mida_porteria, &long_pal);
	if ((nFil_taulell < MIN_FIL) || (nFil_taulell > MAX_FIL) ||
		(nCol_taulell < MIN_COL) || (nCol_taulell > MAX_COL) || (mida_porteria < 0) ||
		(mida_porteria > nFil_taulell - 3) || (long_pal < MIN_PAL) || (long_pal > nFil_taulell - 3))
	{
		fprintf(stderr, "Error: dimensions del camp de joc incorrectes:\n");
		fprintf(stderr, "\t%d =< nFil_taulell (%d) =< %d\n", MIN_FIL, nFil_taulell, MAX_FIL);
		fprintf(stderr, "\t%d =< nCol_taulell (%d) =< %d\n", MIN_COL, nCol_taulell, MAX_COL);
		fprintf(stderr, "\t0 =< mida_porteria (%d) =< nFil_taulell-3 (%d)\n", mida_porteria, (nFil_taulell - 3));
		fprintf(stderr, "\t%d =< long_pal (%d) =< nFil_taulell-3 (%d)\n", MIN_PAL, long_pal, (nFil_taulell - 3));
		fclose(fit);
		exit(3);
	}

	// Parametres de la pilota
	if (!feof(fit))
		fscanf(fit, "%d %d %f %f\n", &fil_pilota, &col_pilota, &v_fil_pilota_R, &v_col_pilota_R);
	if ((fil_pilota < 1) || (fil_pilota > nFil_taulell - 3) ||
		(col_pilota < 1) || (col_pilota > nCol_taulell - 2) ||
		(v_fil_pilota_R < MIN_VEL) || (v_fil_pilota_R > MAX_VEL) ||
		(v_col_pilota_R < MIN_VEL) || (v_col_pilota_R > MAX_VEL))
	{
		fprintf(stderr, "Error: parametre pilota incorrectes:\n");
		fprintf(stderr, "\t1 =< fil_pilota (%d) =< nFil_taulell-3 (%d)\n", fil_pilota, (nFil_taulell - 3));
		fprintf(stderr, "\t1 =< col_pilota (%d) =< nCol_taulell-2 (%d)\n", col_pilota, (nCol_taulell - 2));
		fprintf(stderr, "\t%.1f =< v_fil_pilota_R (%.1f) =< %.1f\n", MIN_VEL, v_fil_pilota_R, MAX_VEL);
		fprintf(stderr, "\t%.1f =< v_col_pilota_R (%.1f) =< %.1f\n", MIN_VEL, v_col_pilota_R, MAX_VEL);
		fclose(fit);
		exit(4);
	}

	// Parametres paleta ordinador
	num_pal_maq = 0;
	while (!feof(fit))
	{
		fscanf(fit, "%d %d %f\n", &fil_pal_maq[num_pal_maq], &col_pal_maq[num_pal_maq], &v_pal_maq[num_pal_maq]);
		if ((fil_pal_maq[num_pal_maq] < 1) || (fil_pal_maq[num_pal_maq] + long_pal > nFil_taulell - 2) ||
			(col_pal_maq[num_pal_maq] < 5) || (col_pal_maq[num_pal_maq] > nCol_taulell - 2) ||
			(v_pal_maq[num_pal_maq] < MIN_VEL) || (v_pal_maq[num_pal_maq] > MAX_VEL))
		{
			fprintf(stderr, "Error: parametres paleta ordinador incorrectes:\n");
			fprintf(stderr, "\t1 =< fil_pal_maq[num_pal_maq] (%d) =< nFil_taulell-long_pal-3 (%d)\n", fil_pal_maq[num_pal_maq],
					(nFil_taulell - long_pal - 3));
			fprintf(stderr, "\t5 =< col_pal_maq[num_pal_maq] (%d) =< nCol_taulell-2 (%d)\n", col_pal_maq[num_pal_maq],
					(nCol_taulell - 2));
			fprintf(stderr, "\t%.1f =< v_pal_maq[num_pal_maq] (%.1f) =< %.1f\n", MIN_VEL, v_pal_maq[num_pal_maq], MAX_VEL);
			fclose(fit);
			exit(5);
		}
		num_pal_maq++;
	}

	// Nombre de paletes valid
	if (num_pal_maq < MIN_PAL_MAQ || num_pal_maq > MAX_PAL_MAQ)
	{
		fprintf(stderr, "Error: nombre de paletes fora de rang\n");
		exit(6);
	}

	fclose(fit);

	// Fitxer carregat: tot OK!

	return num_pal_maq;
}

// funcio per inicialitar les variables i visualitzar l'estat inicial del joc
// Parametres d'entrada:
// - numero de paletes de la maquina a inicialitzar
int inicialitza_joc(int num_pal_maq)
{
	int i;
	int i_port;
	int f_port;
	int retwin;
	char index_pantalla;
	char strin[51];

	// Taulell
	retwin = win_ini(&nFil_taulell, &nCol_taulell, '+', INVERS); 
	if (retwin < 0) // si no pot crear l'entorn de joc amb les curses
	{
		fprintf(stderr, "Error en la creacio del taulell de joc:\t");
		switch (retwin)
		{
		case -1:
			fprintf(stderr, "camp de joc ja creat!\n");
			break;
		case -2:
			fprintf(stderr, "no s'ha pogut inicialitzar l'entorn de curses!\n");
			break;
		case -3:
			fprintf(stderr, "les mides del camp demanades son massa grans!\n");
			break;
		case -4:
			fprintf(stderr, "no s'ha pogut crear la finestra!\n");
			break;
		}
		return (retwin);
	}

	// Porteries
	i_port = nFil_taulell / 2 - mida_porteria / 2; 
	if (nFil_taulell % 2 == 0)
		i_port--;
	if (i_port == 0)
		i_port = 1;
	f_port = i_port + mida_porteria - 1;
	for (i = i_port; i <= f_port; i++)
	{
		win_escricar(i, 0, ' ', NO_INV);
		win_escricar(i, nCol_taulell - 1, ' ', NO_INV);
	}

	// Paleta usuari
	fil_pal_usu = nFil_taulell / 2;
	col_pal_usu = 3;
	if (fil_pal_usu + long_pal >= nFil_taulell - 3)
		fil_pal_usu = 1;
	for (i = 0; i < long_pal; i++)
		win_escricar(fil_pal_usu + i, col_pal_usu, '0', INVERS); // dibuixar paleta inicialment

	// Paletes ordinador
	for(int n_paleta = 0; n_paleta < num_pal_maq; n_paleta++)
	{
		for (i = 0; i < long_pal; i++)
		{
			index_pantalla = n_paleta + 1;
			win_escricar(fil_pal_maq[n_paleta] + i, col_pal_maq[n_paleta], '0' + index_pantalla, INVERS);
		}
		pVertical_pal_maq[n_paleta] = fil_pal_maq[n_paleta]; // fixar valor real paleta ordinador
	}

	// Pilota
	fil_pilota_R = fil_pilota;
	col_pilota_R = col_pilota;						   // fixar valor real posicio pilota
	win_escricar(fil_pilota, col_pilota, '.', INVERS); // dibuix inicial pilota

	// Instruccions
	sprintf(strin, "Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir.", 
			TEC_AMUNT, TEC_AVALL);
	win_escristr(strin);

	return (0);
}

// funcio per moure la pilota; retorna un valor amb alguna d'aquestes
// possibilitats:
//	-1 ==> la pilota no ha sortit del taulell
//	 0 ==> la pilota ha sortit per la porteria esquerra
//	>0 ==> la pilota ha sortit per la porteria dreta
// cap no conte informacio
void * moure_pilota(void * cap)
{
	int f_h;
	int c_h;
	char rh;
	char rv;
	char rd;

	do
	{
		f_h = fil_pilota_R + v_fil_pilota_R; // posicio hipotetica de la pilota
		c_h = col_pilota_R + v_col_pilota_R;
		cont = -1; // inicialment suposem que la pilota no surt
		rh = rv = rd = ' ';
		if ((f_h != fil_pilota) || (c_h != col_pilota))
		{						   // si posicio hipotetica no coincideix amb la pos. actual
			if (f_h != fil_pilota) // provar rebot vertical
			{
				rv = win_quincar(f_h, col_pilota); // veure si hi ha algun obstacle
				if (rv != ' ')					   // si no hi ha res
				{
					v_fil_pilota_R = -v_fil_pilota_R;	 // canvia velocitat vertical
					f_h = fil_pilota_R + v_fil_pilota_R; // actualitza posicio hipotetica
				}
			}
			if (c_h != col_pilota) // provar rebot horitzontal
			{
				rh = win_quincar(fil_pilota, c_h); // veure si hi ha algun obstacle
				if (rh != ' ')					   // si no hi ha res
				{
					v_col_pilota_R = -v_col_pilota_R;	 // canvia velocitat horitzontal
					c_h = col_pilota_R + v_col_pilota_R; // actualitza posicio hipotetica
				}
			}
			if ((f_h != fil_pilota) && (c_h != col_pilota)) // provar rebot diagonal
			{
				rd = win_quincar(f_h, c_h);
				if (rd != ' ') // si no hi ha obstacle
				{
					v_fil_pilota_R = -v_fil_pilota_R;
					v_col_pilota_R = -v_col_pilota_R; // canvia velocitats
					f_h = fil_pilota_R + v_fil_pilota_R;
					c_h = col_pilota_R + v_col_pilota_R; // actualitza posicio entera
				}
			}
			if (win_quincar(f_h, c_h) == ' ')					   // verificar posicio definitiva
			{													   // si no hi ha obstacle
				win_escricar(fil_pilota, col_pilota, ' ', NO_INV); // esborra pilota
				fil_pilota_R += v_fil_pilota_R;
				col_pilota_R += v_col_pilota_R;
				fil_pilota = f_h;
				col_pilota = c_h;									   // actualitza posicio actual
				if ((col_pilota > 0) && (col_pilota <= nCol_taulell))  // si no surt
					win_escricar(fil_pilota, col_pilota, '.', INVERS); // imprimeix pilota
				else
					cont = col_pilota; // codi de finalitzacio de partida
			}
		}
		else
		{
			fil_pilota_R += v_fil_pilota_R;
			col_pilota_R += v_col_pilota_R;
		}

	} while ((tecla != TEC_RETURN) && (cont == -1));

	return 0;
}

// funcio per moure la paleta de l'usuari en funcio de la tecla premuda
// cap no conte informacio
void * mou_paleta_usuari(void * cap)
{
	do
	{
		tecla = win_gettec();
		if (tecla != 0)
		{
			if ((tecla == TEC_AVALL) && (win_quincar(fil_pal_usu + long_pal, col_pal_usu) == ' '))
			{
				win_escricar(fil_pal_usu, col_pal_usu, ' ', NO_INV);				// esborra primer bloc
				fil_pal_usu++;														// actualitza posicio
				win_escricar(fil_pal_usu + long_pal - 1, col_pal_usu, '0', INVERS); // impri. ultim bloc
			}
			if ((tecla == TEC_AMUNT) && (win_quincar(fil_pal_usu - 1, col_pal_usu) == ' '))
			{
				win_escricar(fil_pal_usu + long_pal - 1, col_pal_usu, ' ', NO_INV); // esborra ultim bloc
				fil_pal_usu--;														// actualitza posicio
				win_escricar(fil_pal_usu, col_pal_usu, '0', INVERS);				// imprimeix primer bloc
			}
		}
	} while ((tecla != TEC_RETURN) && (cont == -1));

	return 0;
}

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
	// char rh,rv,rd;

	// rang i: 			[0, 8]
	i = (int) (intptr_t) index;
	// rang char_index: [1, 9]
	ind_pantalla = (char) (intptr_t) index + 1;

	do
	{
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

	} while ((tecla != TEC_RETURN) && (cont == -1));

	return 0;
}

//**************************************************************************
// 	PRINCIPAL
//**************************************************************************

int main(int n_args, const char *ll_args[])
{
	int num_pal_maq;
	
	int thread_output;

	time_t t_inicial;
	time_t t_actual;
	int t_partida_s;
	int t_partida_min;

	char strin[100];

	//************* INICIALITZACIONS & CONTROL D'ERRORS ********************

	// Nombre de parametres valid
	if ((n_args != 2) && (n_args != 3))
	{
		fprintf(stderr, "Comanda: tennis1 fit_param num_paletes [retard]\n");
		exit(1);
	}

	// Carrega de parametres en base a fitxer de camp donat
	num_pal_maq = carrega_parametres(ll_args[1]);

	// Assignacio de temps de retard
	if (n_args == 3)
		retard = atoi(ll_args[2]);
	else
		retard = 100;

	// Creacio del taulell de joc
	if (inicialitza_joc(num_pal_maq) != 0)
		exit(4);

	// Inicialitzacio de variables de threads
	thread_output = -1;

	//****************************** JOC ***********************************

	// Creacio de threads
	pthread_create(&taula_threads[10], NULL, moure_pilota, NULL);
	pthread_create(&taula_threads[9], NULL, mou_paleta_usuari, NULL);
	for (int i = 0; i < num_pal_maq; i++)
		pthread_create(&taula_threads[i], NULL, mou_paleta_ordinador, (void *)(intptr_t) i);

	// Temps de la partida
	time(&t_inicial);
	do
	{
		time(&t_actual);
		t_partida_s = difftime(t_actual, t_inicial);
		t_partida_min = t_partida_s / 60;
		t_partida_s = t_partida_s % 60;

		sprintf(strin, "Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir. Temps: %i:%i", 
			TEC_AMUNT, TEC_AVALL, t_partida_min, t_partida_s);
		win_escristr(strin);

		win_retard(retard);

	} while ((tecla != TEC_RETURN) && (cont == -1));

	// Espera a threads
	pthread_join(taula_threads[10], (void *)(intptr_t) thread_output);
	pthread_join(taula_threads[9], (void *)(intptr_t) thread_output);
	for (int i = 0; i < num_pal_maq; i++)
		pthread_join(taula_threads[i], (void *)(intptr_t) thread_output);

	win_fi();

	//***************************** FI DE JOC *******************************
	if (tecla == TEC_RETURN)
		printf("S'ha aturat el joc amb la tecla RETURN!\n");
	else
	{
		if (cont == 0)
			printf("Ha guanyat l'ordinador!\n");
		else
			printf("Ha guanyat l'usuari!\n");
	}

	return 0;
}