from tkinter import *
import serial
import os
from os import listdir
from os.path import isfile,join

class betterButton(Button):
    def __init__(self,parent,root,buttonName,**options):
        Button.__init__(self,parent,**options)
        self.root = root
        self.imagePath = self.root.photos[buttonName+".gif"]
        if buttonName+"_pressed.gif" in self.root.photos:
            self.imagePathPressed = self.root.photos[buttonName+"_pressed.gif"]
        else:
            self.imagePathPressed = self.imagePath
        self.buttonName = buttonName
        self.width = 30
        self.height = 30
        self.config(width = self.width,height = self.height,relief = SUNKEN,borderwidth = 0,command =self._on_Click,image = self.imagePath)
        self.bind('<ButtonPress-1>',self._on_pressed)
        self.bind('<ButtonRelease-1>',self._on_release)
    def _on_pressed(self,event):
        self.config(image =self.imagePathPressed)
    def _on_release(self,event):
        self.config(image = self.imagePath)
    def _on_Click(self):
        pass
        #print(self.buttonName + " pressed.\n")

class serialButton(betterButton):
    def _on_Click(self):
        betterButton._on_Click(self)
        #self.ser.write(bytes(self.buttonName+'|'))#The bar symbol will be used for separating messages
        print(self.buttonName+'|')

class pauseButton(serialButton):
    def __init__(self,parent,root,buttonName,**options):
        serialButton.__init__(self,parent,root,buttonName,**options)
        self.imagePathPlay = self.root.photos["play.gif"]
        self.imagePathPlayPressed = self.root.photos["play_pressed.gif"]
        self.buttonNamePlay = "play"
    def _on_Click(self):
        (self.imagePath,self.imagePathPlay) = (self.imagePathPlay,self.imagePath)
        (self.imagePathPressed,self.imagePathPlayPressed) = (self.imagePathPlayPressed,self.imagePathPressed)
        (self.buttonName,self.buttonNamePlay) = (self.buttonNamePlay,self.buttonName)
        self.config(image = self.imagePath)
        serialButton._on_Click(self)

class Window(Frame):
    def __init__(self,master = None):
        Frame.__init__(self,master,borderwidth = 2,relief = 'groove',background = 'orange')
        self.root = master
        self.width = self.root.winfo_width()
        self.height = self.root.winfo_height()
        #self.ser = serial.Serial('/dev/ttyACM0')
        #self.ser.write(b'PCINT')
        self.load_images()
        self.init_Frames()

    def redraw_Canvas(self):
        self.canvas.create_rectangle(50,80,23,30,fill = "blue")

    def on_Resize(self,event):
        canvas.delete("all")
        self.width,self.height = event.width, event.height
        self.redraw_Canvas()

    def init_Frames(self):
        self.grid(row =0,column =0,sticky = N+S+E+W)

        self.canvas = Canvas(self,width = 396,height = 228+30,highlightthickness=0)
        self.canvas.grid(row = 0,column = 0, sticky = N+W+E)
        self.redraw_Canvas()

        self.buttonFrame = Frame(self,borderwidth = 2, relief = 'groove',background = 'white')
        self.buttonFrame.grid(row = 1,column = 0,sticky = S)
        
        self.backButton = serialButton(self.buttonFrame,self,"left")
        self.backButton.grid(row = 0,column = 0)

        self.pauseButton = pauseButton(self.buttonFrame,self,"pause")
        self.pauseButton.grid(row = 0,column = 1)
        
        self.fwdButton = serialButton(self.buttonFrame,self,"right")
        self.fwdButton.grid(row = 0,column = 2)

        self.rowconfigure(0,weight = 1)
        self.columnconfigure(0,weight =1)
        
    def load_images(self):
        self.photos = {}
        files = [f for f in listdir('.') if isfile(join('.', f))]# get all files in path
        for filename in files:
            if filename[-3:] == 'gif':
                self.photos[filename] = PhotoImage(file=filename)



if __name__ == "__main__":
    root = Tk()
    root.geometry("400x300")
    root.resizable(False,False)
    root.title("Audio 'IPod' Interface")
    app = Window(root)
    root.mainloop()
