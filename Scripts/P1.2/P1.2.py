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
import gzip                 # per si el tar esta comprimit 

#___________________________________________FUNCIONS____________________________________________

def tracta_exepcio(error):
    exception_type = type(error).__name__
    print("\n",exception_type, file=sys.stderr)
    print(error, file=sys.stderr)
    
def llista_dir():
    global lboxP, lboxS
    global quefaig
    quefaig.set("llistant el directori en curs                   ")
    llista_dirPy()
    llista_dirSh()
    quefaig.set("llistant el directori en curs                   ")

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
def noms_curts():
    global lboxP, lboxS
    global quefaig
    quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters ")
    noms_curtsPy()
    noms_curtsSh()

def noms_curtsPy():
    global lboxP
    global quefaig
    quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters en python")
    
    mida = simpledialog.askinteger('Mida minima','Quina mida mínima de nom vols?')
    quefaig.set("Buscant els noms d'usuaris amb menys de "+str(mida)+" caràcters   ")

def noms_curtsSh():
    global lboxS
    global quefaig
    quefaig.set("Buscant els noms d'usuaris amb menys d'un cert numero de caràcters en shell")
    
    mida = simpledialog.askinteger('Mida minima','Quina mida mínima de nom vols?')
    quefaig.set("Buscant els noms d'usuaris amb menys de "+str(mida)+" caràcters	")

# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers)
#def sudoers():
#def sudoersPy():
#def sudoersSh():

# 3. quins usuaris fa massa temps que van canviar la seva contrasenya
#def massa_temps():
#def massa_tempsPy():
#def massa_tempsSh():

# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others)
#def exec_others():
#def exec_othersPy():
#def exec_othersSh():

# 5. quins ftxers tenen el bit SETUID activat
#def setuid_actiu():
#def setuid_actiuPy():
#def setuid_actiuSh():

# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat
#def permis_exec_a_tar():
#def permis_exec_a_tarPy():
#def permis_exec_a_tarSh():

# End TO DO_______________________________________________________________________________________________

def netejar():
    global lboxP, lboxS
    global quefaig
    quefaig.set("netejant la informació trobada")
    lboxP.delete(0,END)
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
# TO DO: canvi de comanda
frameBotons= Frame(guiroot)
Button (frameBotons, text='llista_dir', command=llista_dir).pack(anchor=W,side=LEFT)
Button (frameBotons, text='noms curts', command=noms_curts).pack(anchor=W,side=LEFT)
Button (frameBotons, text='sudoers', command=llista_dir).pack(side=LEFT)
Button (frameBotons, text='massa temps', command=llista_dir).pack(side=LEFT)
Button (frameBotons, text='exec others', command=llista_dir).pack(side=LEFT)
Button (frameBotons, text='setuid actiu', command=llista_dir).pack(side=LEFT)
Button (frameBotons, text='permis exec a tar', command=llista_dir).pack(anchor=E,side=LEFT)
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

# TO DO: canvi de comanda
Label  (frameBotonsPy, text='Només Python:',font=("Arial Bold",14)).pack(anchor=W,side=TOP,pady=20)
Button (frameBotonsPy, text='llista dir', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='noms curts', command=noms_curtsPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='sudoers', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='massa temps', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='exec others', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='setuid actiu', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='exec a tar', command=llista_dirPy).pack(anchor=W,side=TOP)
Button (frameBotonsPy, text='netejar', command=netejar).pack(anchor=W,side=TOP)

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

# TO DO: canvi de comanda
Label  (frameBotonsSh, text='Només Shell :',font=("Arial Bold",14)).pack(anchor=W,side=TOP,pady=20)
Button (frameBotonsSh, text='llista dir', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='noms curts', command=noms_curtsSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='sudoers', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='massa temps', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='exec others', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='setuid actiu', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='exec a tar', command=llista_dirSh).pack(anchor=W,side=TOP)
Button (frameBotonsSh, text='netejar', command=netejar).pack(anchor=W,side=TOP)

frameSortir= Frame(guiroot)
frameSortir.pack(side=TOP, expand=True, fill=BOTH,padx=20,pady=5)

Button (frameSortir, text='Sortir',command=acabar).pack(side=BOTTOM,anchor=W)

comprova= messagebox.askyesno("Benvingut","Vols comprovar alguns aspectes de la seguretat del sistema?")

if comprova:
    guiroot.mainloop()
else:
    acabar()
