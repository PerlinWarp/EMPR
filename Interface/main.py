from tkinter import *
import serial
import os



class Window(Frame):
    def __init__(self,master = None):
        Frame.__init__(self,master)
        self.root = master
        self.ser = serial.Serial('/dev/ttyACM0')
        self.ser.write(b'PCINT')
        self.init_Frames()

    def redraw_Canvas(self,scale):
        print("no")

    def on_Resize(self,event):
        canvas.delete("all")
        w,h = event.width, event.height
        xy = 0,0,w-1,h-1
        self.redraw_Canvas(xy)

    def back(self):
        self.ser.write(b'CMD:BACK')
    def fwd(self):
        self.ser.write(b'CMD:FWD.')
    def pause(self):
        self.ser.write(b'CMD:PAUS')

    def init_Frames(self):
        self.frames = {}

        self.frames[0] = Frame(self, borderwidth = 2,relief = 'groove',background='orange')
        self.frames[0].grid(row=1,column =0,columnspan = 2)

        self.canvas = Canvas(self,width = self.root.winfo_width(),height = self.root.winfo_height() - 40,highlightthickness=0)
        self.canvas.bind("<Configure>",self.on_Resize)
        #self.canvas.pack(fill = BOTH, expand = 1)
        self.redraw_Canvas(self.root.winfo_width(),self.root.winfo_height())

        self.backButton = Button(self.frames[0], text ="Back",command = self.back,relief = FLAT,pady = 20,padx = 20)
        self.backButton.grid(row = 0,column = 0)

        self.pauseButton = Button(self.frames[0], text ="Paus",command = self.pause,relief = FLAT,pady = 20,padx = 20)
        self.pauseButton.grid(row = 0,column = 1)

        self.fwdButton = Button(self.frames[0], text ="Frwd",command = self.fwd,relief = FLAT,pady = 20,padx = 20)
        self.fwdButton.grid(row = 0,column = 2)



if __name__ == "__main__":
    root = Tk()
    root.geometry("400x300")
    root.title("Audio 'IPod' Interface")

    app = Window(root)
    root.mainloop()
