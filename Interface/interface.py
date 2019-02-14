from tkinter import *
import serial
import os
#NOTE: Errors will occur if serial module is installed as well as 
# pyserial. Make sure to pip3 uninstall serial first before
# pip3 install serial
     
def back():
    ser.write(b'CMD:BACK')
def fwd():
    ser.write(b'CMD:FWD.')
def pause():
    ser.write(b'CMD:PAUS')

    
def redraw_Canvas();

def on_Resize(self,event):
    canvas.delete("all")
    w,h = event.width, event.height
    

def initFrames(root):
    frames = {}
    frames[0] = Frame(root,borderwidth = 2,relief = 'flat',background='green')
    frames[0].grid(row =0,column = 0,sticky = N+S+E+W)

    frames[1] = Frame(root, borderwidth = 2,relief = 'groove',background='orange')
    frames[1].grid(row=1,column =0,columnspan = 2)

    canvas = Canvas(frames[0],width = 200,height = 100,highlightthickness=0)
    canvas.bind("<Configure>",on_Resize)
    canvas.pack(fill = BOTH, expand = 1)
    backButton = Button(frames[1], text ="Back",command = back,relief = FLAT,pady = 20,padx = 20)
    backButton.grid(row = 0,column = 0)
    fwdButton = Button(frames[1], text ="Frwd",command = fwd,relief = FLAT,pady = 20,padx = 20)
    fwdButton.grid(row = 0,column = 1)
    return frames

if __name__ == "__main__":
    root = Tk()
    root.geometry("400x300")
    ser = serial.Serial('/dev/ttyACM0')
    ser.write(b'PCINT')
    root.title("Audio 'IPod' Interface")
    frames = initFrames(root)
