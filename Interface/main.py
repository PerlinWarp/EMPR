from tkinter import *
import serial
import os



class Window(Frame):
    def __init__(self,master = None):
        Frame.__init__(self,master,borderwidth = 2,relief = 'groove',background = 'orange')
        self.root = master
        self.width = self.root.winfo_width()
        self.height = self.root.winfo_height()
        self.ser = serial.Serial('/dev/ttyACM0')
        self.ser.write(b'PCINT')
        self.init_Frames()

    def redraw_Canvas(self):
        self.canvas.create_rectangle(50,80,23,30,fill = "blue")

    def on_Resize(self,event):
        canvas.delete("all")
        self.width,self.height = event.width, event.height
        self.redraw_Canvas()

    def back(self):
        self.ser.write(b'CMD:BACK')
    def fwd(self):
        self.ser.write(b'CMD:FWD.')
    def pause(self):
        self.ser.write(b'CMD:PAUS')

    def init_Frames(self):
        self.grid(row =0,column =0,sticky = N+S+E+W)

        self.canvas = Canvas(self,width = 400,height = 280,highlightthickness=0)
        self.canvas.bind("<Configure>",self.on_Resize)
        self.canvas.grid(row = 0,column = 0, sticky = N+W+E)
        self.redraw_Canvas()

        self.buttonFrame = Frame(self,borderwidth = 2, relief = 'groove',background = 'white')
        self.buttonFrame.grid(row = 1,column = 0,sticky = S)

        self.backButton = Button(self.buttonFrame, text ="Back",command = self.back,relief = FLAT,pady = 20,padx = 20)
        self.backButton.grid(row = 0,column = 0)

        self.pauseButton = Button(self.buttonFrame, text ="Paus",command = self.pause,relief = FLAT,pady = 20,padx = 20)
        self.pauseButton.grid(row = 0,column = 1)

        self.fwdButton = Button(self.buttonFrame, text ="Frwd",command = self.fwd,relief = FLAT,pady = 20,padx = 20)
        self.fwdButton.grid(row = 0,column = 2)

        self.rowconfigure(0,weight = 1)
        self.columnconfigure(0,weight =1)



if __name__ == "__main__":
    root = Tk()
    root.geometry("400x300")
    root.title("Audio 'IPod' Interface")

    app = Window(root)
    root.mainloop()
