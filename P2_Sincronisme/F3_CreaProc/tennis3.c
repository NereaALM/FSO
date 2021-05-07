//**************************************************************************
//
//	Tennis3.c
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
//	El programa invoca les funcions definides en 'winsuport2.o', les
//	quals proporcionen una interficie senzilla per a crear una finestra
//	de text on es poden imprimir caracters en posicions especifiques de
//	la pantalla (basada en CURSES); per tant, el programa necessita ser
//	compilat amb la llibreria 'curses':
//
//	$ gcc tennis3.c winsuport2.o -o tennis3 -lcurses
//  $ gcc pal_ord3.c winsuport2.o -o pal_ord3 -lcurses
//	$ tennis3 fit_param num_pilotes [retard]
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
//
//**************************************************************************

#include "tennis3.h"

///*************************************************************************
// 	VARIABLES GLOBALS
///*************************************************************************

// Parametres visuals
int nFil_taulell;
int nCol_taulell;
int mida_porteria;
int long_pal;

// Gestio de la partida
int gols_usuari;
int gols_maquina;

// Posicio i velocitat pilota
int fil_pilota;
int col_pilota;
float fil_pilota_R;
float col_pilota_R;
float v_fil_pilota_R;
float v_col_pilota_R;

// Posicio usuari
int fil_pal_usu;
int col_pal_usu;

// Threads
pthread_t taula_threads[MAX_THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Memoria compartida
int id_taulell;
int * p_taulell;
int id_mem;
mem_compartida * p_mem;
mem_compartida mem_comp;

// Processos
pid_t pid_pal_maq[MAX_PAL_MAQ];

///*************************************************************************
// 	FUNCIONS
///**************************************************************************

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
		fscanf(fit, "%d %d %f\n", &mem_comp.fil_pal_maq[num_pal_maq], &mem_comp.col_pal_maq[num_pal_maq], &mem_comp.v_pal_maq[num_pal_maq]);
		if ((mem_comp.fil_pal_maq[num_pal_maq] < 1) || (mem_comp.fil_pal_maq[num_pal_maq] + long_pal > nFil_taulell - 2) ||
			(mem_comp.col_pal_maq[num_pal_maq] < 5) || (mem_comp.col_pal_maq[num_pal_maq] > nCol_taulell - 2) ||
			(mem_comp.v_pal_maq[num_pal_maq] < MIN_VEL) || (mem_comp.v_pal_maq[num_pal_maq] > MAX_VEL))
		{
			fprintf(stderr, "Error: parametres paleta ordinador incorrectes:\n");
			fprintf(stderr, "\t1 =< fil_pal_maq[num_pal_maq] (%d) =< nFil_taulell-long_pal-3 (%d)\n", mem_comp.fil_pal_maq[num_pal_maq],
					(nFil_taulell - long_pal - 3));
			fprintf(stderr, "\t5 =< col_pal_maq[num_pal_maq] (%d) =< nCol_taulell-2 (%d)\n", mem_comp.col_pal_maq[num_pal_maq],
					(nCol_taulell - 2));
			fprintf(stderr, "\t%.1f =< mem_comp.v_pal_maq[num_pal_maq] (%.1f) =< %.1f\n", MIN_VEL, mem_comp.v_pal_maq[num_pal_maq], MAX_VEL);
			fclose(fit);
			exit(5);
		}
		num_pal_maq++;
	}

	// Nombre de paletes valid
	if (num_pal_maq < MIN_PAL_MAQ || num_pal_maq > MAX_PAL_MAQ)
	{
		fprintf(stderr, "Error: nombre de paletes fora de rang\n");
		exit(5);
	}

	fclose(fit);

	// Fitxer carregat: tot OK!

	return num_pal_maq;
}

// funcio per inicialitar les variables i visualitzar l'estat inicial del joc
// Parametres d'entrada:
// - numero de paletes de la maquina a inicialitzar
// Parametres de Sortida:
// - != 0 => error
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

		return retwin;
	}

	id_taulell = ini_mem(retwin);
	p_taulell = map_mem(id_taulell);
	win_set(p_taulell, nFil_taulell, nCol_taulell);

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
			win_escricar(mem_comp.fil_pal_maq[n_paleta] + i, mem_comp.col_pal_maq[n_paleta], '0' + index_pantalla, INVERS);
		}
		mem_comp.pVertical_pal_maq[n_paleta] = mem_comp.fil_pal_maq[n_paleta]; // fixar valor real paleta ordinador
	}

	// Pilota
	fil_pilota_R = fil_pilota;
	col_pilota_R = col_pilota;						   // fixar valor real posicio pilota
	win_escricar(fil_pilota, col_pilota, '.', INVERS); // dibuix inicial pilota

	// Instruccions
	sprintf(strin, "Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir.", 
			TEC_AMUNT, TEC_AVALL);
	win_escristr(strin);
	
	return 0;
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
		pthread_mutex_lock(&mutex);

		f_h = fil_pilota_R + v_fil_pilota_R; // posicio hipotetica de la pilota
		c_h = col_pilota_R + v_col_pilota_R;
		rh = rv = rd = ' ';
		if ((f_h != fil_pilota) || (c_h != col_pilota))
		{						   // si posicio hipotetica no coincideix amb la pos. actual
			// provar rebot vertical
			if (f_h != fil_pilota) 
			{
				rv = win_quincar(f_h, col_pilota); // veure si hi ha algun obstacle
				if (rv != ' ')					   // si no hi ha res
				{
					v_fil_pilota_R = -v_fil_pilota_R;	 // canvia velocitat vertical
					f_h = fil_pilota_R + v_fil_pilota_R; // actualitza posicio hipotetica
				}
			}
			// provar rebot horitzontal
			if (c_h != col_pilota) 
			{
				rh = win_quincar(fil_pilota, c_h); // veure si hi ha algun obstacle
				if (rh != ' ')					   // si no hi ha res
				{
					v_col_pilota_R = -v_col_pilota_R;	 // canvia velocitat horitzontal
					c_h = col_pilota_R + v_col_pilota_R; // actualitza posicio hipotetica
				}
			}
			// provar rebot diagonal
			if ((f_h != fil_pilota) && (c_h != col_pilota)) 
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
			// verificar posicio definitiva
			if (win_quincar(f_h, c_h) == ' ')					   
			{													   // si no hi ha obstacle
				win_escricar(fil_pilota, col_pilota, ' ', NO_INV); // esborra pilota
				fil_pilota_R += v_fil_pilota_R;
				col_pilota_R += v_col_pilota_R;
				fil_pilota = f_h;
				col_pilota = c_h;									   // actualitza posicio actual
				if ((col_pilota > 0) && (col_pilota <= nCol_taulell))  // si no surt
					win_escricar(fil_pilota, col_pilota, '.', INVERS); // imprimeix pilota			
				else if (col_pilota <= 0)
				{
					gols_maquina++;
					p_mem->num_pilotes--;

					// Inicialitzar pilota a porteria usuari
					fil_pilota = fil_pal_usu + long_pal / 2;
					col_pilota = col_pal_usu + 1;
					fil_pilota_R = fil_pilota;
					col_pilota_R = col_pilota;
					win_escricar(fil_pilota, col_pilota, '.', INVERS);
				}
				else if (col_pilota > nCol_taulell)
				{
					gols_usuari++;
					p_mem->num_pilotes--;

					// Inicialitzar pilota a porteria maquina
					fil_pilota = p_mem->fil_pal_maq[0] + long_pal / 2;
					col_pilota = p_mem->col_pal_maq[0] - 1;
					fil_pilota_R = fil_pilota;
					col_pilota_R = col_pilota;
					win_escricar(fil_pilota, col_pilota, '.', INVERS);
				}
			}
		}
		else
		{
			fil_pilota_R += v_fil_pilota_R;
			col_pilota_R += v_col_pilota_R;
		}

		pthread_mutex_unlock(&mutex);

		win_retard(p_mem->retard);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	return 0;
}

// funcio per moure la paleta de l'usuari en funcio de la tecla premuda
// cap no conte informacio
void * mou_paleta_usuari(void * cap)
{
	do
	{
		pthread_mutex_lock(&mutex);

		p_mem->tecla = win_gettec();
		if (p_mem->tecla != 0)
		{
			if ((p_mem->tecla == TEC_AVALL) && (win_quincar(fil_pal_usu + long_pal, col_pal_usu) == ' '))
			{
				win_escricar(fil_pal_usu, col_pal_usu, ' ', NO_INV);				// esborra primer bloc
				fil_pal_usu++;														// actualitza posicio
				win_escricar(fil_pal_usu + long_pal - 1, col_pal_usu, '0', INVERS); // impri. ultim bloc
			}
			if ((p_mem->tecla == TEC_AMUNT) && (win_quincar(fil_pal_usu - 1, col_pal_usu) == ' '))
			{
				win_escricar(fil_pal_usu + long_pal - 1, col_pal_usu, ' ', NO_INV); // esborra ultim bloc
				fil_pal_usu--;														// actualitza posicio
				win_escricar(fil_pal_usu, col_pal_usu, '0', INVERS);				// imprimeix primer bloc
			}
		}

		pthread_mutex_unlock(&mutex);

		win_retard(p_mem->retard);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	return 0;
}

///**************************************************************************
// 	PRINCIPAL
///**************************************************************************

int main(int n_args, const char *ll_args[])
{
	//********************** VARIABLES LOCALS *******************************

	char strin[MAX_STRING];

	time_t t_inicial;
	time_t t_actual;
	int t_partida_s;
	int t_partida_min;

	int thread_output;

	int num_pal_maq;
	int i;
	char args_proc[NUM_ARGS_PROC][MAX_STRING];

	//************* INICIALITZACIONS & CONTROL D'ERRORS ********************

	// Nombre de parametres valid
	if ((n_args != 3) && (n_args != 4))
	{
		fprintf(stderr, "Comanda: tennis3 fit_param num_pilotes [retard]\n");
		exit(1);
	}

	// Carrega de parametres en base a fitxer de camp donat
	num_pal_maq = carrega_parametres(ll_args[1]);

	// Nombre de pilotes de la partida
	mem_comp.num_pilotes = atoi(ll_args[2]);
	if (mem_comp.num_pilotes < 1)
	{
		fprintf(stderr, "Parametre incorrecte: Ha d'haver minim una pilota\n");
		exit(1);
	}

	// Assignacio de temps de retard
	if (n_args == 4)
		mem_comp.retard = atoi(ll_args[3]);
	else
		mem_comp.retard = 100;

	// Creacio del taulell de joc
	if (inicialitza_joc(num_pal_maq) != 0)
		exit(6);

	// Inicialització de zona de memoria per processos
	id_mem = ini_mem(sizeof(mem_compartida));
	p_mem = map_mem(id_mem);
	p_mem->retard = mem_comp.retard;
	p_mem->num_pilotes = mem_comp.num_pilotes;
	for (i = 0; i < MAX_PAL_MAQ; i++)
	{
		p_mem->fil_pal_maq[i] = mem_comp.fil_pal_maq[i];
		p_mem->col_pal_maq[i] = mem_comp.col_pal_maq[i];
		p_mem->pVertical_pal_maq[i] = mem_comp.pVertical_pal_maq[i];
		p_mem->v_pal_maq[i] = mem_comp.v_pal_maq[i];
	}

	// Inicialitzacio de variables de threads
	thread_output = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_unlock(&mutex);

	// Preparació d'arguments per inicialitzar processos
	sprintf(args_proc[1], "%i", nFil_taulell);
	sprintf(args_proc[2], "%i", nCol_taulell);
	sprintf(args_proc[3], "%i", long_pal);
	sprintf(args_proc[4], "%i", id_taulell);
	sprintf(args_proc[5], "%i", id_mem);

	//****************************** JOC ***********************************

	// Creació de processos
	for (i = 0; i < num_pal_maq; i++)
	{
		pid_pal_maq[i] = fork();
		if (pid_pal_maq[i] == (pid_t) 0)
		{
			sprintf(args_proc[0], "%i", i);
			execlp(	args_proc[0], args_proc[1], args_proc[2], args_proc[3],
					args_proc[4], args_proc[5], (char *) 0);
			exit(0);
		}
	}

	// Creacio de threads
	pthread_create(&taula_threads[0], NULL, moure_pilota, NULL);
	pthread_create(&taula_threads[1], NULL, mou_paleta_usuari, NULL);

	// Temps de la partida
	time(&t_inicial);
	do
	{
		time(&t_actual);
		t_partida_s = difftime(t_actual, t_inicial);
		t_partida_min = t_partida_s / 60;
		t_partida_s = t_partida_s % 60;

		sprintf(strin, "Marcadors: %i:%i Pilotes: %i Temps: %i:%i", 
				gols_maquina, gols_usuari, p_mem->num_pilotes, t_partida_min, t_partida_s);

		pthread_mutex_lock(&mutex);
		win_escristr(strin);
		win_update();
		pthread_mutex_unlock(&mutex);

		win_retard(p_mem->retard);

	} while ((p_mem->tecla != TEC_RETURN) && (p_mem->num_pilotes > 0));

	//***************************** FI DE JOC *******************************

	// Espera a threads
	pthread_join(taula_threads[0], (void *)(intptr_t) thread_output);
	pthread_join(taula_threads[1], (void *)(intptr_t) thread_output);
	pthread_mutex_destroy(&mutex);

	// Espera a processos
	for (i = 0; i < num_pal_maq; i++)
		waitpid(pid_pal_maq[i], 0, 0);

	// Alliberar recursos
	elim_mem(id_taulell);
	elim_mem(id_mem);

	win_fi();
	
	// Mostra resultat per pantalla
	if (p_mem->tecla == TEC_RETURN)
		printf("S'ha aturat el joc amb la tecla RETURN!\n");
	else
	{
		if (gols_maquina > gols_usuari)
			printf("Ha guanyat l'ordinador!\n");
		else if (gols_maquina < gols_usuari)
			printf("Ha guanyat l'usuari!\n");
		else printf("Hi ha hagut un empat!\n");
	}

	return 0;
}