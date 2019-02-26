import serial
from tkinter import *

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
        self.config(relief = SUNKEN,borderwidth = 0,command =self._on_Click,image = self.imagePath)
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
        self.root.frame.ser.write(bytes(self.buttonName+'|','utf-8'))#The bar symbol will be used for separating messages

class menuButton(betterButton):
    def __init__(self,parent,root,frame,buttonName,**options):
        self.frame = frame
        self.name = buttonName
        betterButton.__init__(self,parent,root,"menuButton",**options)
        self.text = Label(self.frame,text = "buttonName")
        self.config(text = self.name,compound = BOTTOM)
    def _on_Click(self):
        self.frame.ser.write(bytes(self.buttonName+'|','utf-8'))
        self.frame.switch(self.name)

class exitButton(menuButton):
    def _on_Click(self):
        self.frame.ser.write(bytes(self.buttonName+'|','utf-8'))
        self.frame.root.destroy()
        
        
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
