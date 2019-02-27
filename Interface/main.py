from tkinter import *
import serial
from os import *
from os.path import isfile,join

from buttons import *
from windows import *

if __name__ == "__main__":
    root = Tk()
    root.geometry("800x600")
    root.resizable(False,False)
    root.title("Audio 'IPod' Interface")
    #ser = serial.Serial('/dev/ttyACM0',timeout = 0.5)
    #ser.flushInput()
    ser = 0
    app = WindowManager(ser,root)
    root.mainloop()
