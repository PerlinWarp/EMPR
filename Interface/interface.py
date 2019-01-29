from tkinter import *
import serial
import os

class Window(Frame):
    def __init__(self,ser,master=None):
        Frame.__init__(self,master)
        self.master = master
        self.init_window()
        
    def init_window(self):
        self.master.title("Audio 'IPod' Interface")
        self.pack(fill = BOTH,expand =1)
        backButton = Button(self, text ="Back",command = self.back,relief = FLAT,pady = 20,padx = 20)
        backButton.grid(row =0,column =0,pady = 10,padx = 25, sticky = S )
        backImg = PhotoImage(file = os.getcwd() +"/resources/duck.gif")
        backButton.pack()
        self.pack(side = BOTTOM)
       # backButton.config(image=backImg)
    def back(self):
        ser.write(b'CMD:BACK')
    def fwd(self):
        ser.write(b'CMD:fwd')
    def pause(self):
        ser.write(b'CMD:pause')

#use tkinter and pyserial
#def getInstruction(ser):
#    ser.write(b'INPUT')

if __name__ == "__main__":
    root = Tk()
    root.geometry("400x300")
    ser = serial.Serial('/dev/ttyACM0')
    app = Window(ser,root)
    root.mainloop()
