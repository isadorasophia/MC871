from Tkinter import *
import tkMessageBox

from PIL import ImageTk, Image
import os

import subprocess

exe         = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/prog/BC"
model_name  = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/data/brain.scn"
labels_name = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/data/brain_labels.scn"

output    = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/prog/brain.ppm"

def change():
    try:
        res = subprocess.check_call([exe, model_name, labels_name, output, cur_cut.get(), e.get(), \
             str(w1.get()), str(w2.get())])
    except subprocess.CalledProcessError:
        tkMessageBox.showinfo("An error occured!", "Please, check your inputs.")

    # get new image
    displayImage = ImageTk.PhotoImage(Image.open(output))
    panel.configure(image = displayImage)
    panel.image = displayImage

root = Tk()
root.geometry('{}x{}'.format(300, 500))

# set image
img = ImageTk.PhotoImage(Image.open(output))
panel = Label(root, image = img)

# choose origin
l1 = Label(root, text="Valor de origem:", width = 12)
l1.grid(row = 0, column = 0, padx = 4)

e = Entry(root, width = 22)
e.insert(END, "OP")
e.grid(row = 0, column = 1, columnspan = 2, pady = 10, padx = 5, sticky = W)

# choose cut
CUTS = [
        ("Axial", "AXIAL"),
        ("Coronal", "CORONAL"),
        ("Sagital", "SAGITAL")
       ]

cur_cut = StringVar()
counter = 0

for t, mode in CUTS:
    cut_b = Radiobutton(root, text = t, indicatoron = 0, value = mode, variable = cur_cut)
    cut_b.grid(row = 1, column = counter, sticky = W+E+N+S, pady = 5)

    counter += 1

# set scale buttons
l2 = Label(root, text="Brightness:", width = 8)
l2.grid(row = 2, column = 0, padx = 4, pady = 23)
w1 = Scale(root, from_=0, to=100, orient=HORIZONTAL)
w1.set(60)
w1.grid(row = 2, column = 1, columnspan = 2, padx = 10, pady = 5, sticky = W+E+N+S)

l2 = Label(root, text="Contrast:", width = 8)
l2.grid(row = 3, column = 0, padx = 4, pady = 18)
w2 = Scale(root, from_=0, to=100, orient=HORIZONTAL)
w2.set(30)
w2.grid(row = 3, column = 1, columnspan = 2, padx = 10, sticky = W+E+N+S)

# set main button
panel.grid(row = 4, columnspan = 3, sticky = W+E+N+S)

button = Button(root, text='Apply!', command = change)
button.grid(row = 5, column = 1, pady = 12)

root.mainloop()