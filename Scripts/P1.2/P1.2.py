#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Practica 1 de FSO-2020/21
# Autors: M.Àngels Moncusí Nerea López
# Data: 12 mar 2021
# Versio: v2

#___________________________________________IMPORTS____________________________________________

# imports tipics/generics
import os   # os.path, os.stat, os.remove ...
import subprocess
from sys import stderr
from stat import filemode

# imports pel GUI
from tkinter import *   # GUI
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox	# per a mostrar missatges a l’usuari
# https://docs.python.org/3.9/library/tkinter.messagebox.html

# imports auxilliars/secundaris
import gzip				 # per si el tar esta comprimit 
from datetime import date
from datetime import datetime
from datetime import timedelta
import stat
import tarfile

#___________________________________________FUNCIONS____________________________________________

def tracta_exepcio(error):
	exception_type = type(error).__name__
	print("\n",exception_type, file=sys.stderr)
	print(error, file=sys.stderr)
	
def llista_dir():
	global lboxP, lboxS
	global quefaig
	quefaig.set("llistant el directori en curs				   ")
	llista_dirPy()
	llista_dirSh()
	quefaig.set("llistant el directori en curs				   ")

def llista_dirPy():
	global lboxP
	global quefaig
	quefaig.set("llistant el directori en curs en python ")
	elements=os.listdir('./')
	lboxP.delete(0,END)
	for element in elements:
		lboxP.insert(END,element)

def llista_dirSh():
	global lboxS
	global quefaig
	quefaig.set("llistant el directori en curs en shell   ")
	lboxS.delete(0,END)
	out=subprocess.check_output(["ls","./"], universal_newlines=True)
	for elem in out.splitlines():
	   lboxS.insert(END,elem)

# TO DO_______________________________________________________________________________________________

# 1. quins usuaris tenen un nom massa curt

# Funció auxiliar per demanar la mida en cas necessari
def escollir_mida(mida = -1):
	if mida == -1:		
		escollir_mida = messagebox.askyesno('Escollir mida', 'Vols escollir la mida mínima dels noms?')
		
		if escollir_mida:
			mida = simpledialog.askinteger('Mida minima','Quina mida mínima de nom vols?')
			if not mida:
				mida = 4
		else:
			mida = 4
			
		quefaig.set("Buscant els noms d'usuaris amb menys de "+str(mida)+" caràcters   ")
	
	return mida
	
def noms_curts():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters ")
	
	# Lectura de paràmetres
	mida = escollir_mida()
	
	# Crida de funcions
	noms_curtsPy(mida)
	noms_curtsSh(mida)

def noms_curtsPy(mida = -1):
	global lboxP
	global quefaig
	quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters en python")
	
	# Lectura de paràmetres
	mida = escollir_mida(mida)
 	
	# Lectura de fitxer /etc/passwd
	passwd_file = open('/etc/passwd', 'r')
	flines_list = passwd_file.readlines()
	passwd_file.close()
	
	# Evaluació de longituds de noms
	for line in flines_list:
		user = line.split(':')[0]
		if len(user) < mida:
			lboxP.insert(END, user)

def noms_curtsSh(mida = -1):
	global lboxS
	global quefaig
	quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters en shell")
	
	# Lectura de paràmetres
	mida = escollir_mida(mida)
	
	# Lectura de fitxer /etc/passwd	
	args = ["cut", "-d:", "-f1", "/etc/passwd"]
	cut = subprocess.Popen(args, stdout=subprocess.PIPE)
	users = str(cut.communicate())[2:][:-10]
	
	# Evaluació de longituds de noms
	for user in users.split('\\'):
		user = user[1:]
		if len(user) < mida:
			lboxS.insert(END, user)


# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers)
def sudoers():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant quins usuaris poden executar amb permisos elevats ")
	sudoersPy()
	sudoersSh()

def sudoersPy():
	global lboxP
	global quefaig
	quefaig.set("Buscant quins usuaris poden executar amb permisos elevats ")
	
	# Lectura de fitxer /etc/group
	passwd_file = open('/etc/group', 'r')
	flines_list = passwd_file.readlines()
	passwd_file.close()
	
	# Cerca de sudoers
	for line in flines_list:
		if re.search('^sudo:.*$', line):
			lboxP.insert(END, line.split(':')[3][:-1])
	
def sudoersSh():
	global lboxS
	global quefaig
	quefaig.set("Buscant quins usuaris poden executar amb permisos elevats ")
	
	# echo -e "$(grep '^sudo:.*$' /etc/group | cut -d: -f4)"
	args = ['grep', '^sudo:.*$', '/etc/group']
	grep = subprocess.Popen(args, stdout=subprocess.PIPE)
	output = (str(grep.communicate())[3:][:-10]).split(':')[3]
	
	lboxS.insert(END, output)
	
	
# 3. quins usuaris fa massa temps que van canviar la seva contrasenya

# Funció per controlar execució en sudo i inicialitzar els dies sense canvis
def init_massa_temps(dies = -1):
	
	# Controls de permis d'execució sudo
	if dies != -2 and os.getuid() != 0:
		messagebox.showinfo(message="S'ha d'executar com a sudo per fer aquesta funció", title="Avis")
		dies = -2
	
	# Inicialització de dies
	elif dies == -1:
		is_def_lim = messagebox.askyesno("Limit de temps",
		"Vols definir el limit de temps sense canviar la contrasenya o deixar el temps per defecte (300 dies)?")
		if is_def_lim:
			dies = simpledialog.askinteger('Limit de temps','Quin limit de temps sense canviar la contrasenya vols?')
			if not dies:
				dies = 300
		else:
			dies = 300
		quefaig.set("Buscant quins usuaris fa més de " + str(dies) + " dies que van canviar la seva contrasenya")
		
	return dies

def massa_temps():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant quins usuaris fa massa temps que van canviar la seva contrasenya ")
	
	dies = init_massa_temps()
	
	massa_tempsPy(dies)
	massa_tempsSh(dies)

def massa_tempsPy(dies = -1):
	global lboxP
	global quefaig
	quefaig.set("Buscant quins usuaris fa massa temps que van canviar la seva contrasenya ")
	
	dies = init_massa_temps(dies)
	
	# Control d'execució com a sudo
	if dies >= 0:
		
		# Data límit en format Y-M-D
		limit_date = date.today()
		limit_date = limit_date - timedelta(days = dies)
		# Data inici Unix en format Y-M-D
		unix_ini_date = date(1970, 1, 1)
		
		# Lectura de fitxer /etc/shadow
		shadow_file = open('/etc/shadow', 'r')
		flines_list = shadow_file.readlines()
		shadow_file.close()
		
		# Impressió de usuaris amb contrasenya antiga
		for line in flines_list:
			# Num de dies des del 1970 fins l'ultim canvi de password
			user_date = line.split(':')[2]
			# Data de ultim canvi
			user_date = unix_ini_date + timedelta(days = int(user_date))
			
			if user_date < limit_date:
				lboxP.insert(END, line.split(':')[0])
	
def massa_tempsSh(dies = -1):
	global lboxS
	global quefaig
	quefaig.set("Buscant quins usuaris fa massa temps que van canviar la seva contrasenya ")
	
	dies = init_massa_temps(dies)
	
	if dies >= 0:
		
		# Data límit en format Y-M-D
		args = ['date', '+%Y-%m-%d']
		date_proc = subprocess.Popen(args, stdout=subprocess.PIPE)
		today = str(date_proc.communicate()[0])[2:][:-3]
		today = datetime.strptime(today, '%Y-%m-%d')
		limit_date = today - timedelta(days = dies)
		
		# Data inici Unix en format Y-M-D
		unix_ini_date = datetime(1970, 1, 1)
		
		# Lectura de fitxer /etc/shadow
		args = ['cat', '/etc/shadow']
		cat = subprocess.Popen(args, stdout=subprocess.PIPE)
		file_content = str(cat.communicate()[0])[2:][:-3].split('\\')
		
		# Impressió de usuaris amb contrasenya antiga
		for line in file_content:
			line = line.split(':')
			user = line[0]
			user_days = line[2]
			user_date = unix_ini_date + timedelta(days = int(user_days))
			
			if user_date < limit_date:
				lboxS.insert(END, user)
			

# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others)
def exec_others():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant quins ftxers tenen el permís d’execució per a els altres usuaris (others) ")
	exec_othersPy()
	exec_othersSh()
	
def exec_othersPy():
	global lboxP
	global quefaig
	quefaig.set("Buscant quins fitxers tenen el permís d’execució per a els altres usuaris (others) ")
	
	home_dir = os.path.expanduser("~")
	
	for current_dir, dir_list, file_list in os.walk(home_dir):
		for file in file_list:
		
			file_path = os.path.join(current_dir, file)
			if os.path.isfile(file_path):
			
				perms = os.stat(file_path)
				if perms.st_mode & stat.S_IXOTH:
					lboxP.insert(END, file)
	
def exec_othersSh():
	global lboxS
	global quefaig
	quefaig.set("Buscant quins fitxers tenen el permís d’execució per a els altres usuaris (others) ")
	
	home_dir = os.path.expanduser("~")
	
	# Obtenir paths
	args = ['ls', '-l', '-R', str(home_dir)]
	ls = subprocess.Popen(args, stdout=subprocess.PIPE)
	paths = str(ls.communicate()[0])[1:][:-3].split('\\')
	
	# Mirar si bit x de other està activat
	for path in paths:
		path = path[1:]
		if path.startswith("-"):
			path = path.split(" ")
			if path[0][9] == 'x':
				lboxS.insert(END, path[-1])
	

# 5. quins ftxers tenen el bit SETUID activat
def setuid_actiu():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant quins fitxers tenen el bit SETUID activat")
	setuid_actiuPy()
	setuid_actiuSh()
	
def setuid_actiuPy():
	global lboxP
	global quefaig
	quefaig.set("Buscant quins ftxers tenen el bit SETUID activat")
	
	home_dir = os.path.expanduser("~")
	
	for current_dir, dir_list, file_list in os.walk(home_dir):
		for file in file_list:
			
			file_path = os.path.join(current_dir, file)
			if os.path.isfile(file_path):
			
				perms = os.stat(file_path)
				if perms.st_mode & stat.S_ISUID:
					lboxP.insert(END, file)
	
def setuid_actiuSh():
	global lboxS
	global quefaig
	quefaig.set("Buscant quins ftxers tenen el bit SETUID activat")
	
	home_dir = os.path.expanduser("~")
	
	# Obtenir paths
	args = ['ls', '-l', '-R', str(home_dir)]
	ls = subprocess.Popen(args, stdout=subprocess.PIPE)
	paths = str(ls.communicate()[0])[1:][:-3].split('\\')
	
	# Mirar si bit s de user està activat
	for path in paths:
		path = path[1:]
		if path.startswith("-"):
			path = path.split(" ")
			if path[0][3] == 's':
				print(path[-1])
				lboxS.insert(END, path[-1])
				

# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat
def permis_exec_a_tar():
	global lboxP, lboxS
	global quefaig
	quefaig.set("Buscant quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat")
	permis_exec_a_tarPy()
	permis_exec_a_tarSh()
	
def permis_exec_a_tarPy():
	global lboxP
	global quefaig
	quefaig.set("Buscant quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat")
	
	home_dir = os.path.expanduser("~")
	
	for current_dir, dir_list, file_list in os.walk(home_dir):
		for file in file_list:
			if file.endswith('.tar') or file.endswith('.tgz'):
				
				file_path = os.path.join(current_dir, file)
				if os.path.isfile(file_path):
					
					with tarfile.open(file_path, 'r') as tar:
						
						file_printed = False
						for member in tar.getmembers():
							if not file_printed and (member.mode & stat.S_IXUSR or member.mode & stat.S_IXGRP or member.mode & stat.S_IXOTH):
								lboxP.insert(END, file)
								file_printed = True

def permis_exec_a_tarSh():
	global lboxS
	global quefaig
	quefaig.set("Buscant quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat")
	
# End TO DO_______________________________________________________________________________________________

def netejar():
	global lboxP, lboxS
	global quefaig
	quefaig.set("netejant la informació trobada")
	lboxP.delete(0,END)
	lboxS.delete(0,END)

def netejarPy():
	global lboxP, lboxS
	global quefaig
	quefaig.set("netejant la informació trobada de Python")
	lboxP.delete(0,END)
	
def netejarSh():
	global lboxP, lboxS
	global quefaig
	quefaig.set("netejant la informació trobada de Shell")
	lboxS.delete(0,END)

def tancaGUI():
	guiroot.quit()

def acabar():
	tancaGUI()


#_________________________________________PRINCIPAL_______________________________________________

#### tkinter GUI
# No cal mes informacio, pero si teniu curiositat,
# sense entar en molts detalls tcl/tk:
# https://likegeeks.com/es/ejemplos-de-la-gui-de-python/

#### inici del programa a executar en l'entorn python 3

guiroot= Tk()
guiroot.title("Comprovar seguretat bàsica del sistema")
guiroot.minsize(500,400)
### cerca= StringVar()  # variable usada en els QUERIES

quefaig = StringVar()
quefaig.set("No se encara que fer encara, escull una opció")

# layout del GUI
frameBotons= Frame(guiroot)
Button (frameBotons, text='llista_dir', command=llista_dir).pack(anchor=W,side=LEFT)
Button (frameBotons, text='noms curts', command=noms_curts).pack(anchor=W,side=LEFT)
Button (frameBotons, text='sudoers', command=sudoers).pack(side=LEFT)
Button (frameBotons, text='massa temps', command=massa_temps).pack(side=LEFT)
Button (frameBotons, text='exec others', command=exec_others).pack(side=LEFT)
Button (frameBotons, text='setuid actiu', command=setuid_actiu).pack(side=LEFT)
Button (frameBotons, text='permis exec a tar', command=permis_exec_a_tar).pack(anchor=E,side=LEFT)
Button (frameBotons, text='netejar', command=netejar).pack(anchor=W,side=LEFT)
frameBotons.pack(expand=True,fill=BOTH,padx=10,pady=5)

frameque= Frame(guiroot)
Label (frameque,text='Què faig:',font=("Arial Bold",14)).pack(anchor=W,side=LEFT)
Label (frameque,textvariable=quefaig, font=("Arial",14)).pack(anchor=W, side=LEFT)
frameque.pack(side=TOP,fill=BOTH,padx=10,pady=10)

frameEvents= Frame(guiroot)
frameEvents.pack(side=TOP, expand=True, fill=BOTH,padx=20,pady=5)

frameEventsPy= Frame(frameEvents)
frameBotonsPy= Frame(frameEvents)
frameEventsPy.pack(side=LEFT,expand=True,fill=BOTH)
frameBotonsPy.pack(side=LEFT,expand=True,fill=BOTH,padx=5)

Label (frameEventsPy,text='Informació trobada Python:',font=("Arial Bold",14)).pack(anchor=W,side=TOP)
scrollist = Scrollbar(frameEventsPy,orient=VERTICAL)
lboxP = Listbox(frameEventsPy,yscrollcommand=scrollist.set,selectmode=SINGLE,exportselection=False)
scrollist.config(command=lboxP.yview)
scrollist.pack(side=RIGHT,fill=Y)
lboxP.pack(side=LEFT,expand=True,fill=BOTH)

Label  (frameBotonsPy, text='Només Python:',font=("Arial Bold",14)).pack(anchor=W,side=TOP,pady=20)
Button (frameBotonsPy, text='llista dir', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='noms curts', command=noms_curtsPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='sudoers', command=sudoersPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='massa temps', command=massa_tempsPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='exec others', command=exec_othersPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='setuid actiu', command=setuid_actiuPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='exec a tar', command=permis_exec_a_tarPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='netejar', command=netejarPy).pack(anchor=W,side=TOP)

frameEventsSh= Frame(frameEvents)
frameBotonsSh= Frame(frameEvents)
frameEventsSh.pack(side=LEFT,expand=True,fill=BOTH)
frameBotonsSh.pack(side=LEFT,expand=True,fill=BOTH,padx=5)

Label (frameEventsSh,text='Informació trobada Shell:',font=("Arial Bold",14)).pack(anchor=W,side=TOP)
scrollist = Scrollbar(frameEventsSh,orient=VERTICAL)
lboxS= Listbox(frameEventsSh,yscrollcommand=scrollist.set,selectmode=SINGLE,exportselection=False)
scrollist.config(command=lboxS.yview)
scrollist.pack(side=RIGHT,fill=Y)
lboxS.pack(side=LEFT,expand=True,fill=BOTH)
frameEventsSh.pack(expand=True,fill=BOTH)

Label  (frameBotonsSh, text='Només Shell :',font=("Arial Bold",14)).pack(anchor=W,side=TOP,pady=20)
Button (frameBotonsSh, text='llista dir', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='noms curts', command=noms_curtsSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='sudoers', command=sudoersSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='massa temps', command=massa_tempsSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='exec others', command=exec_othersSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='setuid actiu', command=setuid_actiuSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='exec a tar', command=permis_exec_a_tarSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='netejar', command=netejarSh).pack(anchor=W,side=TOP)

frameSortir= Frame(guiroot)
frameSortir.pack(side=TOP, expand=True, fill=BOTH,padx=20,pady=5)

Button (frameSortir, text='Sortir',command=acabar).pack(side=BOTTOM,anchor=W)

comprova= messagebox.askyesno("Benvingut","Vols comprovar alguns aspectes de la seguretat del sistema?")

if comprova:
	guiroot.mainloop()
else:
	acabar()
