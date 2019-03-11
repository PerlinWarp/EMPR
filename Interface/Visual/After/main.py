# import tkinter
from tkinter import *


class Timer:
    def __init__(self, parent):
        #Serial Setup
        self.w = Canvas(root, width=200, height=100)
        self.w.pack()

        self.w.create_rectangle(10,10,100,20, fill="red")


        # variable storing time
        self.seconds = 0
        # label displaying time
        self.label = Label(parent, text="0 s", font="Arial 30", width=10)
        self.label.pack()
        # start the timer
        self.label.after(1000, self.refresh_label)

    def refresh_label(self):
        """ refresh the content of the label every second """
        # increment the time
        self.seconds += 1
        # display the new time
        self.label.configure(text="%i s" % self.seconds)
        # request tkinter to call self.refresh after 1s (the delay is given in ms)
        self.label.after(1000, self.refresh_label)

        self.w.delete("all")
        self.w.create_rectangle(10,10,100,20*self.seconds, fill="red")

    ##def update_volume(self):


if __name__ == "__main__":
    root = Tk()
    root.geometry("800x600")
    timer = Timer(root)
    root.mainloop()


