import serial
from tkinter import *
from os import *
from os.path import isfile,join
from buttons import *
from PIL import Image, ImageTk
class WindowManager(Frame):
    def __init__(self,ser,master = None,width=800,height=600):
        self.width = width
        self.height = height
        Frame.__init__(self,master,borderwidth = 0,relief = FLAT,background = "#BDC3C7",width = self.width,height = self.height)#outside frame containing all menus etc
        self.root = master
        self.ser = ser
        self.width = self.winfo_width()
        self.height = self.winfo_height()
        self.currentScreen = "menu"
        self.menus = {"play":PlayScreen(self),"menu":MainMenu(self)}#initialize array of window contents
        self.menus[self.currentScreen].show_All()
    def switch(self,screen):
        self.menus[self.currentScreen].hide_All()
        self.menus[screen].show_All()
        self.currentScreen = screen
        


class Window():
#To Inherit, change load_image_set, and init_frames
    def __init__(self,master = None,width = 800,height = 600):
        self.frame = master
        self.width = width
        self.height = height
        self.load_image_set()
        self.load_images()
        self.widgets = {}
        self.init_widgets()

    def load_image_set(self):#create a dictionary specifying all 
        pass

    def init_widgets(self):
        pass
        
    def load_images(self):
        self.photos = {}
        files = [f for f in listdir('resources') if isfile(join('resources', f))]# get all files in path
        for filename in files:
            if filename.endswith(".gif") and filename[:-4] in self.imageSet:
                self.photos[filename] = PhotoImage(file="resources/"+filename)
            elif filename.endswith("_pressed.gif") and filename[:-12] in self.imageSet:
                self.photos[filename] = PhotoImage(file = "resources/"+filename)
              
    def hide_All(self):#hide all widgets on grid
        for widget in self.widgets:
            widget.grid_remove()#remove objects without destroying them
            
    def show_All(self):#map all widgets to grid
        for widget in self.widgets:
            widget.grid(row = i,column = j)


class PlayScreen(Window):
    def load_image_set(self):
        self.imageSet = ["left","right","play","pause"]
        
    def redraw_Canvas(self):
        self.widgets["canvas"].create_rectangle(50,80,23,30,fill = "blue")
        
    def init_widgets(self):
        self.serConnected = False
        self.frame.grid(row =0,column =0,sticky = N+S+E+W)

        self.widgets["canvas"] = Canvas(self.frame,width = 396,height = 228+30,highlightthickness=0)
        self.redraw_Canvas()

        self.widgets["buttonFrame"] = Frame(self.frame,borderwidth = 2, relief = 'groove',background = 'white')
        
        self.widgets["backButton"] = serialButton(self.widgets["buttonFrame"],self,"left")
        self.widgets["pauseButton"] = pauseButton(self.widgets["buttonFrame"],self,"pause") 
        self.widgets["fwdButton"] = serialButton(self.widgets["buttonFrame"],self,"right")
   
    def show_All(self):
        self.widgets["canvas"].grid(row = 0,column = 0, sticky = N+W+E)
        self.widgets["buttonFrame"].grid(row = 1,column = 0,sticky = S)
        
        self.widgets["backButton"].grid(row = 0,column = 0)
        self.widgets["pauseButton"].grid(row = 0,column = 1)
        self.widgets["fwdButton"].grid(row = 0,column = 2)
        
        self.frame.rowconfigure(0,weight = 1)
        self.frame.columnconfigure(0,weight =1)

class MainMenu(Window):
    
    def animate_duck(self):
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until(b'|')
            if d.endswith(b"CONNECT|"):
                self.serConnected = True
                self.widgets["loading"].place_forget()
                self.widgets["duck"].place_forget()
                self.frame.ser.write(b"ACK|")
        if self.serConnected == False:
            self.widgets["duck"].config(image = self.photos["duck"+str(self.fr)+".gif"])
            self.fr= (self.fr+1) % 12
            self.frame.after(40,self.animate_duck)#repeat every 40 ms
            
    def load_image_set(self):
        self.imageSet = ["pc98ico","menubkg","plantpot","mbed_logo","duck","menuButton"]
    def init_widgets(self):
        self.widgets["background"]  = Label(self.frame, image=self.photos["menubkg.gif"])
        self.widgets["duck"]  = Label(self.frame, image=self.photos["duck.gif"])
        self.widgets["loading"] = Label(self.frame, text = "Connecting...",foreground = "white",font= "Arial")
        self.widgets["button_Area"] = Frame(self.frame,height = 350,width = 250,borderwidth = 5,relief = SUNKEN,highlightbackground="green", background = "#ECF0F1")
        
        self.widgets["backButton"] = menuButton(self.widgets["button_Area"],self,self.frame,"play")
        self.widgets["browseButton"] = menuButton(self.widgets["button_Area"],self,self.frame,"Browse")
        self.widgets["pauseButton"] = menuButton(self.widgets["button_Area"],self,self.frame,"Settings") 
        self.widgets["exitButton"] = exitButton(self.widgets["button_Area"],self,self.frame,"Exit")

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1 )
        self.widgets["duck"].place(x = 700,y =30)
        self.widgets["loading"].place(x=700,y = 120)
        self.widgets["button_Area"].place(x = 80,y =180 )
        
        self.widgets["backButton"].place(x=40,y =70)
        self.widgets["browseButton"].place(x=40,y =140)
        self.widgets["pauseButton"].place(x=40,y =210)
        self.widgets["exitButton"].place(x=40,y =280)
        self.animate_duck()
        
    def hide_All(self):
        for widget in self.widgets:
            widget.place_forget()#remove objects without destroying them
            
    def load_images(self):
        self.fr = 0
        self.serConnected = False
        Window.load_images(self)
        for i in range(0,12):
            self.photos["duck"+str(i)+".gif"] = PhotoImage(file = "resources/duck.gif",format= "gif -index "+str(i))
        self.photos["menubkg.gif"] = Image.open("resources/menubkg.png").convert("RGBA")
        self.photos["plantpot.gif"] = Image.open("resources/plantpot.png").convert("RGBA")
        self.photos["mbed_logo.gif"] = Image.open("resources/mbed_logo.png").convert("RGBA")
        self.photos["menubkg.gif"].paste(self.photos["mbed_logo.gif"],(80,90),self.photos["mbed_logo.gif"])
        self.photos["menubkg.gif"].paste(self.photos["plantpot.gif"],(450,350),self.photos["plantpot.gif"])
        
        self.photos["menubkg.gif"] = ImageTk.PhotoImage(self.photos["menubkg.gif"])

        
        
        
        
