from tkinter import *
import serial
from os import *
from os.path import isfile,join

from buttons import *
from windows import *

'''
To switch windows see windows.py
Note to get the duck to dissapear you have to press 5.
For this to work you need to open the python UI first. 
'''


if __name__ == "__main__":
    root = Tk()
    root.geometry("800x600")
    root.resizable(False,False)
    root.title("Audio 'IPod' Interface")
    ser = serial.Serial('/dev/ttyACM0',timeout = 0.5)
    ser.flushInput()
    app = WindowManager(ser,root)
    root.mainloop()
