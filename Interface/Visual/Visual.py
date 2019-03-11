# def stream(self):
#     print("Starting streaming")
#     self.frame.ser.write(b"AS|")
#     eof = False
#     d = self.frame.ser.read(2)
#     #while(self.frame.ser.in_waiting > 0 and not str(d) == b''):
#         d = self.frame.ser.read(2)
#         val = int.from_bytes(d,"little",signed=True)
#         print("Data:", d)
#         print("Int:  ",str(int.from_bytes(d,"little",signed=True)))

#         self.widgets["canvas"].delete("all")
#         self.widgets["canvas"].create_rectangle(10,10,100,scale(val), fill="red")
#     print("Reached the end of the file")

#     #The b opens the file in binary mode to write hex directly.
#     print("Done ")

import serial

# Opening the Serial Interface
ser = serial.Serial('/dev/ttyACM0',timeout = 0.5)
ser.flushInput()
print("serial initialized")

while(True):
    d = ser.read(2)
    print("d = ", d)
    val = int.from_bytes(d,"little",signed=True)
    print("Val = ", val)
