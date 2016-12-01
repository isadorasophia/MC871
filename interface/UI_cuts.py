from Tkinter import *
import tkMessageBox

from PIL import ImageTk, Image
import os

import subprocess

exe = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/prog/planar_cut"
model_name = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/data/input/brain.scn"

output = "/home/bonnibel/graduation/6thsemester/mo815/libmc920/data/output/cuts.ppm"

def change():
    try:
        res = subprocess.check_call([exe, model_name, output, str(w1.get()), \
                                    str(w2.get()), str(w3.get()), str(w4.get()), \
                                    str(w5.get()), str(w6.get())])
    except subprocess.CalledProcessError:
        tkMessageBox.showinfo("An error occured!", "Please, check your inputs.")

    # get new image
    displayImage = ImageTk.PhotoImage(Image.open(output))
    panel.configure(image = displayImage)
    panel.image = displayImage

root = Tk()
root.title("Planar Cut Simulator")

# set image
img = ImageTk.PhotoImage(Image.open(output))
panel = Label(root, image = img)

# set scale buttons
l2 = Label(root, text="Vector X:", width = 0)
l2.grid(row = 1, column = 0, padx = 0, pady = 23)
w1 = Scale(root, from_=1, to=100, orient=HORIZONTAL)
w1.set(10)
w1.grid(row = 1, column = 1, columnspan = 6, padx = 10, pady = 5, sticky = W+E+N+S)

l2 = Label(root, text="Vector Y:", width = 0)
l2.grid(row = 2, column = 0, padx = 0, pady = 18)
w2 = Scale(root, from_=1, to=100, orient=HORIZONTAL)
w2.set(10)
w2.grid(row = 2, column = 1, columnspan = 6, padx = 10, sticky = W+E+N+S)

l2 = Label(root, text="Vector Z:", width = 0)
l2.grid(row = 3, column = 0, padx = 0, pady = 18)
w3 = Scale(root, from_=1, to=100, orient=HORIZONTAL)
w3.set(0)
w3.grid(row = 3, column = 1, columnspan = 6, padx = 10, sticky = W+E+N+S)

# set scale buttons
l2 = Label(root, text="Point X:", width = 0)
l2.grid(row = 4, column = 0, padx = 0, pady = 23)
w4 = Scale(root, from_=0, to=157, orient=HORIZONTAL)
w4.set(78)
w4.grid(row = 4, column = 1, columnspan = 6, padx = 10, pady = 5, sticky = W+E+N+S)

l2 = Label(root, text="Point Y:", width = 0)
l2.grid(row = 5, column = 0, padx = 0, pady = 18)
w5 = Scale(root, from_=0, to=255, orient=HORIZONTAL)
w5.set(127)
w5.grid(row = 5, column = 1, columnspan = 6, padx = 10, sticky = W+E+N+S)

l2 = Label(root, text="Point Z:", width = 0)
l2.grid(row = 6, column = 0, padx = 0, pady = 18)
w6 = Scale(root, from_=0, to=255, orient=HORIZONTAL)
w6.set(127)
w6.grid(row = 6, column = 1, columnspan = 6, padx = 10, sticky = W+E+N+S)

panel.grid(row = 7, columnspan = 6, sticky = W+E+N+S)

# set main button
button = Button(root, text='Apply!', command = change)
button.grid(row = 8, column = 2, columnspan = 2, pady = 12)

root.mainloop()