//**************************************************************************
//
//		Tennis3.c
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
//	$ gcc tennis3.c winsuport.o -o tennis3 -lcurses
//	$ tennis3 fit_param [retard]
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
//**************************************************************************

//**************************************************************************
// 	DEFINICIONS
//**************************************************************************

// Llibreries
#include <stdio.h>
#include <stdlib.h>
#include "winsuport.h"

// Limits de globals
#define MIN_FIL 7
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0

// ************************** Variables globals ******************************

// Parametres de Joc
int nFil_taulell;
int nCol_taulell;
int mida_porteria;
int long_pal;

// Posicio usuari
int fil_pal_usu;
int col_pal_usu;

// Posicio i velocitat maquina
int fil_pal_maq;
int col_pal_maq;
float pVertical_pal_maq;
float v_pal_maq;

// Posicio i velocitat pilota
int fil_pilota;
int col_pilota;
float fil_pilota_R;
float col_pilota_R;
float v_fil_pilota_R;
float v_col_pilota_R;

// Retard del joc en ms
int retard;

//**************************************************************************
// 	FUNCIONS
//**************************************************************************

// funcio per realitzar la carrega dels parametres de joc emmagatzemats
// dins un fitxer de text, el nom del qual es passa per referencia en
// 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio
// enviant un missatge per la sortida d'error i retornant el codi per-
// tinent al SO (segons comentaris del principi del programa).
void carrega_parametres(const char *nom_fit)
{
	FILE *fit;

	fit = fopen(nom_fit, "rt"); // intenta obrir fitxer
	if (fit == NULL)
	{
		fprintf(stderr, "No s'ha pogut obrir el fitxer \'%s\'\n", nom_fit);
		exit(2);
	}

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

	if (!feof(fit))
		fscanf(fit, "%d %d %f\n", &fil_pal_maq, &col_pal_maq, &v_pal_maq);
	if ((fil_pal_maq < 1) || (fil_pal_maq + long_pal > nFil_taulell - 2) ||
		(col_pal_maq < 5) || (col_pal_maq > nCol_taulell - 2) ||
		(v_pal_maq < MIN_VEL) || (v_pal_maq > MAX_VEL))
	{
		fprintf(stderr, "Error: parametres paleta ordinador incorrectes:\n");
		fprintf(stderr, "\t1 =< fil_pal_maq (%d) =< nFil_taulell-long_pal-3 (%d)\n", fil_pal_maq,
				(nFil_taulell - long_pal - 3));
		fprintf(stderr, "\t5 =< col_pal_maq (%d) =< nCol_taulell-2 (%d)\n", col_pal_maq,
				(nCol_taulell - 2));
		fprintf(stderr, "\t%.1f =< v_pal_maq (%.1f) =< %.1f\n", MIN_VEL, v_pal_maq, MAX_VEL);
		fclose(fit);
		exit(5);
	}
	fclose(fit); // fitxer carregat: tot OK!
}

// funcio per inicialitar les variables i visualitzar l'estat inicial del joc
int inicialitza_joc(void)
{
	int i, i_port, f_port, retwin;
	char strin[51];

	retwin = win_ini(&nFil_taulell, &nCol_taulell, '+', INVERS); // intenta crear taulell

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

	i_port = nFil_taulell / 2 - mida_porteria / 2; // crea els forats de la porteria
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

	fil_pal_usu = nFil_taulell / 2;
	col_pal_usu = 3; // inicialitzar pos. paletes
	if (fil_pal_usu + long_pal >= nFil_taulell - 3)
		fil_pal_usu = 1;
	for (i = 0; i < long_pal; i++) // dibuixar paleta inicialment
	{
		win_escricar(fil_pal_usu + i, col_pal_usu, '0', INVERS);
		win_escricar(fil_pal_maq + i, col_pal_maq, '1', INVERS);
	}
	pVertical_pal_maq = fil_pal_maq; // fixar valor real paleta ordinador

	fil_pilota_R = fil_pilota;
	col_pilota_R = col_pilota;						   // fixar valor real posicio pilota
	win_escricar(fil_pilota, col_pilota, '.', INVERS); // dibuix inicial pilota

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
int moure_pilota(void)
{
	int f_h, c_h, result;
	char rh, rv, rd;

	f_h = fil_pilota_R + v_fil_pilota_R; // posicio hipotetica de la pilota
	c_h = col_pilota_R + v_col_pilota_R;
	result = -1; // inicialment suposem que la pilota no surt
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
				result = col_pilota; // codi de finalitzacio de partida
		}
	}
	else
	{
		fil_pilota_R += v_fil_pilota_R;
		col_pilota_R += v_col_pilota_R;
	}
	return (result);
}

// funcio per moure la paleta de l'usuari en funcio de la tecla premuda
void mou_paleta_usuari(int tecla)
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

// funcio per moure la paleta de l'ordinador autonomament, en funcio de la
// velocitat de la paleta (variable global v_pal_maq)
void mou_paleta_ordinador(void)
{
	int f_h;
	// char rh,rv,rd;

	f_h = pVertical_pal_maq + v_pal_maq; // posicio hipotetica de la paleta
	if (f_h != fil_pal_maq)				 // si pos. hipotetica no coincideix amb pos. actual
	{
		if (v_pal_maq > 0.0) // verificar moviment cap avall
		{
			if (win_quincar(f_h + long_pal - 1, col_pal_maq) == ' ') // si no hi ha obstacle
			{
				win_escricar(fil_pal_maq, col_pal_maq, ' ', NO_INV); // esborra primer bloc
				pVertical_pal_maq += v_pal_maq;
				fil_pal_maq = pVertical_pal_maq;									// actualitza posicio
				win_escricar(fil_pal_maq + long_pal - 1, col_pal_maq, '1', INVERS); // impr. ultim bloc
			}
			else // si hi ha obstacle, canvia el sentit del moviment
				v_pal_maq = -v_pal_maq;
		}
		else // verificar moviment cap amunt
		{
			if (win_quincar(f_h, col_pal_maq) == ' ') // si no hi ha obstacle
			{
				win_escricar(fil_pal_maq + long_pal - 1, col_pal_maq, ' ', NO_INV); // esbo. ultim bloc
				pVertical_pal_maq += v_pal_maq;
				fil_pal_maq = pVertical_pal_maq;					 // actualitza posicio
				win_escricar(fil_pal_maq, col_pal_maq, '1', INVERS); // impr. primer bloc
			}
			else // si hi ha obstacle, canvia el sentit del moviment
				v_pal_maq = -v_pal_maq;
		}
	}
	else
		pVertical_pal_maq += v_pal_maq; // actualitza posicio vertical real de la paleta
}

//**************************************************************************
// 	PRINCIPAL
//**************************************************************************

int main(int n_args, const char *ll_args[])
{
	int tec;
	int cont;

	//************* INICIALITZACIONS & CONTROL D'ERRORS ********************

	// Lectura de parametres
	if ((n_args != 2) && (n_args != 3))
	{
		fprintf(stderr, "Comanda: tennis3 fit_param [retard]\n");
		exit(1);
	}
	carrega_parametres(ll_args[1]);

	// Assignacio de temps de retard
	if (n_args == 3)
		retard = atoi(ll_args[2]);
	else
		retard = 100;

	// Crea taulell
	if (inicialitza_joc() != 0)
		exit(4);

	//****************************** JOC ***********************************
	do
	{
		tec = win_gettec();

		if (tec != 0)
			mou_paleta_usuari(tec);
		mou_paleta_ordinador();
		cont = moure_pilota();

		win_retard(retard);

	} while ((tec != TEC_RETURN) && (cont == -1));

	//***************************** FI DE JOC *******************************

	win_fi();

	if (tec == TEC_RETURN)
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
