

from tkinter import *
root=Tk()
b=Button(root,justify = LEFT,relief=SUNKEN,borderwidth =0)
photo=PhotoImage(file="play.gif")
b.config(image=photo,width="30",height="30")
b.pack(side=LEFT)
root.mainloop()
