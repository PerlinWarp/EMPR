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
    ser = 0
    #ser = self.ser = serial.Serial('/dev/ttyACM0',921600)
    app = WindowManager(ser,root)
    root.mainloop()
