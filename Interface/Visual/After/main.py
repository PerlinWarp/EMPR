# import tkinter
from tkinter import *
import serial

def scale(value):
    '''
    Scales the byte between 0 and 100. 
    Input, value between +/- 1/2 * 2^16
    '''
    value = abs(value) / 2**15 # Scale between +/-0.5
    value = int(value*200) 
    print("Height = ", value)
    return 190 - value


class Timer:
    def __init__(self, parent):
        #Serial Setup
        self.ser = serial.Serial('/dev/ttyACM0',timeout = 0.5)
        self.ser.flushInput()
        print("serial initialized")
        self.ser.write(b"AS|")

        self.w = Canvas(root, width=200, height=200)
        self.w.pack()

        self.w.create_rectangle(10,10,100,20, fill="red")


        # variable storing time
        self.seconds = 0
        self.song_done = False
        self.zeros = 0
        # label displaying time
        self.label = Label(root, text="0 s", font="Arial 30", width=10)
        self.label.pack(side=BOTTOM)
        # start the timer
        self.label.after(1, self.refresh_label)

    def refresh_label(self):
        if(self.zeros > 10):
            self.song_done = True
        """ refresh the content of the label every second """
        # increment the time
        self.seconds += 1
        # display the new time
        if(self.song_done):
            self.label.configure(text="No Song")
        else:
            print(self.zeros)
            self.label.configure(text="%i s" % self.seconds)
        # request tkinter to call self.refresh after 1s (the delay is given in ms)
        self.label.after(1, self.refresh_label)

        self.w.delete("all")
        ## Read from serial
        d = self.ser.read(2)
        print("Ser = ", d)
        val = int.from_bytes(d,"little",signed=True)
        if(val == 0):
            self.zeros += 1
        print("Data:", d)
        print("Int:  ",str(int.from_bytes(d,"little",signed=True)))
        vol = scale(val)
        if(vol > 100):
            colour = "green"
        else:
            colour = "red"
        self.w.create_rectangle(5,200,100,scale(val), fill=colour)

    ##def update_volume(self):


if __name__ == "__main__":
    root = Tk()
    root.geometry("800x600")
    timer = Timer(root)
    root.mainloop()


