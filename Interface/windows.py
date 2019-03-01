import serial
from tkinter import *
from tkinter.ttk import Combobox
from os import *
from os.path import isfile,join
from buttons import *
from PIL import Image, ImageTk,GifImagePlugin

class WindowManager(Frame):
    def __init__(self,ser,master = None,width=800,height=600):
        self.width = width
        self.height = height
        Frame.__init__(self,master,borderwidth = 0,relief = FLAT,background = "#BDC3C7",width = self.width,height = self.height)#outside frame containing all menus etc
        self.root = master
        self.ser = ser
        self.many_ducks = False
        self.width = self.winfo_width()
        self.height = self.winfo_height()
        #Change this to menu or play to switch between the windows
        self.currentScreen = "menu"
        #Defining all the windows for the menu buttons
        self.menus = {"play":PlayScreen(self),"menu":MainMenu(self),"settings":Settings(self),"browse":Browse(self),"BlueScreen":BlueScreen(self),"load":loadingScreen(self)}#initialize array of window contents
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
        self.serConnected = False
        self.widgets = {}
        self.init_widgets()      
    def init_widgets(self):
        pass     
              
    def hide_All(self):#hide all widgets on grid
        for widget in self.widgets:
            widget.grid_remove()#remove objects without destroying them
            
    def show_All(self):#map all widgets to grid
        for widget in self.widgets:
            widget.grid(row = i,column = j)


class PlaceWindow(Window):
    def hide_All(self):
        for widget in self.widgets:
            self.widgets[widget].place_forget()#remove objects without destroying them
    def show_All(self):
        self.widgets["background"].switch_to_duck()

class MainMenu(PlaceWindow):
    
    def animate_duck(self):
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until(b'|')
            print(d)
            if d.endswith(b"CONNECT|"):
                #Getting rid of the duck. 
                self.serConnected = True
                self.widgets["loading"].place_forget()
                self.widgets["duck"].place_forget()
                self.frame.ser.write(b"ACK|")
        if self.serConnected == False:
            self.widgets["duck"].inc_image()
            self.frame.after(40,self.animate_duck)#repeat every 40 ms
            
    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("menubkg",0,0),("plantpot",450,350),("mbed_logo",80,90)])
        self.widgets["duck"]  = transparencyLabel(self.frame, "duck","menubkg",700,30)
        self.widgets["loading"] = Label(self.frame, text = "Connecting...",foreground = "white",font= "Arial")
        self.widgets["button_Area"] = betterLabel(self.frame, "buttonBox")
        
        self.widgets["backButton"] = menuButton(self.frame,self,"play")
        self.widgets["menuRecord"] = menuButton(self.frame,self,"Record")
        self.widgets["pauseButton"] = menuButton(self.frame,self,"settings") 
        self.widgets["exitButton"] = exitButton(self.frame,self,"exit")
        

    def show_All(self):
        
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1 )
        self.widgets["duck"].place(x = 700,y =30)
        self.widgets["loading"].place(x=700,y = 120)
        self.widgets["button_Area"].place(x = 80,y =180 )
        
        self.widgets["backButton"].place(x=130,y =260)
        self.widgets["menuRecord"].place(x=130,y =330)
        self.widgets["pauseButton"].place(x=130,y =400)
        self.widgets["exitButton"].place(x=130,y =470)
        PlaceWindow.show_All(self)
        self.animate_duck()

class PlayScreen(PlaceWindow):
    # Non interface functions
    def pause(self):
        print("play")

    def play(self):
        print("pause")
    
    # Interface functions
    def redraw_Canvas(self):
        self.widgets["canvas"].create_rectangle(50,80,23,30,fill = "blue")
        
    def init_widgets(self):
        self.serConnected = False
        self.frame.grid(row =0,column =0,sticky = N+S+E+W)

        self.widgets["background"] = layeredLabel(self.frame,[("playbackground",0,0)])
        self.widgets["canvas"] = Canvas(self.frame,background ="black",width = 300,height = 243,highlightthickness=0)

        
        self.widgets["start"] = startButton(self.frame,self,"winstart")
        self.widgets["cross"] = hoverButton(self.frame,self,"cross",menu="BlueScreen")
        self.widgets["realplay"] = functionalButton(self.frame,self, "realplay", function = self.pause)
        self.widgets["realpause"] = functionalButton(self.frame,self, "realpause", function = self.play)

        #Parts of other buttons
        self.widgets["95menu"] = betterLabel(self.frame, "95menu")
        self.widgets["shutdown"] = hoverButton(self.frame,self, "shutdown", "menu")

        self.redraw_Canvas()

        
   
    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["canvas"].place(x=274 ,y=140)

        self.widgets["start"].place(x=1,y =576)
        self.widgets["cross"].place(x=563,y =67)
        
        self.widgets["realplay"].place(x=84,y =120)
        self.widgets["realpause"].place(x=116,y =120)
        PlaceWindow.show_All(self)

class Settings(PlaceWindow):
            
    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("menubkg",0,0),("settingsbkgd",200,100)])
        
        self.widgets["cancelButton"] = hoverButton(self.frame,self,"cancelbutton","menu")
        self.widgets["okButton"] = hoverButton(self.frame,self,"okbutton","menu")
        self.widgets["duckButton"] = duckButton(self.frame,self,"neverbutton","settings")
        self.widgets["testLabel"] = betterLabel(self.frame, "duck")
        comboBox_menus = ["Sandwich","Antistropic filtering","filet fish","steak"]
        self.widgets["volume"] = Scale(self.frame,orient = HORIZONTAL,length =148)
        for i in range(4):
            self.widgets["scale"+str(i)] = betterScale(self.frame,comboBox_menus[i],414,257)
        self.widgets["listBox"] = referenceComboBox(self.frame,self.widgets,comboBox_menus,[["scale",4]]) #4

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["okButton"].place(x=439,y=432)
        self.widgets["cancelButton"].place(x= 520,y = 432)
        self.widgets["duckButton"].place(x= 452,y = 186)
        self.widgets["listBox"].place(x=232,y=258) #232, 258
        self.widgets["volume"].place(x=232,y=176)
        PlaceWindow.show_All(self)

class BlueScreen(PlaceWindow):
            
    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("bluescreen",0,0)])
    
    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        PlaceWindow.show_All(self)

class Browse(PlaceWindow):
    
    def animate_duck(self):
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until(b'|')

            if d.endswith(b"CONNECT|"):
                self.serConnected = True
                print("5 was pressed")
                self.frame.ser.write(b"ACK|")
        if self.serConnected == False:
            self.widgets["duck"].inc_image()
            self.frame.after(40,self.animate_duck)#repeat every 40 ms
            
    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("menubkg",0,0),("plantpot",450,350),("mbed_logo",80,90)])
        self.widgets["duck"]  = transparencyLabel(self.frame, "duck","menubkg",700,30)
        self.widgets["loading"] = Label(self.frame, text = "Connecting...",foreground = "white",font= "Arial")
        self.widgets["button_Area"] = betterLabel(self.frame, "buttonBox")
        
        self.widgets["backButton"] = menuButton(self.frame,self,"play")
        self.widgets["browseButton"] = menuButton(self.frame,self,"browse")
        self.widgets["pauseButton"] = menuButton(self.frame,self,"settings") 
        self.widgets["exitButton"] = exitButton(self.frame,self,"exit")

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1 )
        self.widgets["duck"].place(x = 700,y =30)
        self.widgets["loading"].place(x=700,y = 120)
        self.widgets["button_Area"].place(x = 80,y =180 )
        
        self.widgets["backButton"].place(x=130,y =260)
        self.widgets["browseButton"].place(x=130,y =330)
        self.widgets["pauseButton"].place(x=130,y =400)
        self.widgets["exitButton"].place(x=130,y =470)
        PlaceWindow.show_All(self)
        self.animate_duck()
        
        

        

class loadingScreen(PlaceWindow):
    def return_to_menu(self,event):
        self.frame.switch("menu")
    def init_widgets(self):
        self.frame.bind("<Escape>",self.return_to_menu)
        self.frame.focus_set()
        self.widgets["background"] = layeredLabel(self.frame,[("menubkg",0,0),("connecting",150,500),("embedfs",250,150)])
        self.widgets["load_big"]  = transparencyLabel(self.frame, "load_big","menubkg",360,420)
    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["load_big"].place(x = 360,y =420)
        PlaceWindow.show_All(self)
        self.animate()
    def animate(self):
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until('|')
            if d == "CONNECT":
                self.serConnected == True
                self.frame.switch("play")
                self.frame.ser.write("ACK|")
        if self.serConnected == False:
            self.widgets["load_big"].inc_image()
            self.frame.after(70,self.animate)#repeat every 40 ms
        
