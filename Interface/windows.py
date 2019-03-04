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
        self.currentScreen = "browse"
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
        
        self.startBinding = self.frame.root.bind("<Button-1>",self.widgets["start"].check_focus,"+")
        self.widgets["start"].place(x=1,y =576)
        self.widgets["cross"].place(x=563,y =67)
        
        self.widgets["realplay"].place(x=84,y =120)
        self.widgets["realpause"].place(x=116,y =120)
        PlaceWindow.show_All(self)
    def hide_All(self):
        PlaceWindow.hide_All(self)
        self.frame.root.unbind("<Button-1>",self.startBinding)

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

class Browse(PlaceWindow):
    
    def add_directories(self,directoryTree,path):
        if len(path) == 1:
            if path[0][-1] =='d':#is a directory
                directoryTree[path[0][:-1]] = {}#empty folder = dictionary
            elif path[0][-1] =='f':
                directoryTree[path[0][:-1]] = path[0][:-1]
            return directoryTree
                
        p = path.pop(0) 
        if p in directoryTree:
            self.add_directories(directoryTree[p],path)
        else:
            directoryTree[p] = self.add_directories(directoryTree,path)
        return directoryTree
    def init_directories(self,directoryTree,path):
            if type(directoryTree) != type(dict()):
                item_type = "file"
                self.widgets[path] = browserButton(self.widgets["fileWindow"],self,directoryTree,item_type)
            else:
                item_type = "folder"
                for key in directoryTree.keys():
                    self.init_directories(directoryTree[key],path+"/"+key)
                self.widgets[path] = browserButton(self.widgets["fileWindow"],self,path.split('/')[-1],item_type)
    def place_directories(self,directoryTree,path):
        counter = 0
        for key in directoryTree.keys():
            self.bindings[path+"/"+key] = self.frame.root.bind("<Button-1>",self.widgets[path+"/"+key].check_focus,"+")
            self.widgets[path+"/"+key].place(x = 189+(78*(counter%5)),y=169+ (75*(counter//5)))
            counter+=1
        self.widgets["objects"].config(text = str(counter) + " Object(s)")
        self.widgets["dirEntry"].change_dir(self.path)
        
    def hide_directories(self,directoryTree,path):
        for key in directoryTree.keys():
            self.widgets[path+"/"+key].place_forget()
    def into_dir(self,new_path):
        self.widgets["folderName"].config(text = new_path.rpartition('/')[2])
        self.hide_directories(self.workingTree,self.path)
        self.path += "/"+new_path
        self.workingTree = self.workingTree[new_path]
        self.place_directories(self.workingTree,self.path)
        if self.path == "C:":
            self.widgets["backButton"].place_forget()
            self.hidden = True           
        elif self.hidden == True:
            self.widgets["backButton"].place(x = 149,y = 49)
            self.hidden = False
        
        
    def find_directory(self,directoryTree,path):
        if path in directoryTree.keys():
            return directoryTree[path]
        else:
            new_path = path.partition('/')
            if new_path[0] in directoryTree.keys():
                return self.find_directory(directoryTree[new_path[0]],new_path[2])
            else:
                return False
        
    def outof_dir(self):
        if self.path != "C:":
            self.hide_directories(self.workingTree,self.path)
            self.path = self.path.rpartition('/')[0]
            self.widgets["folderName"].config(text = self.path.rpartition('/')[2])
            self.workingTree = self.find_directory(self.directoryTree,self.path)#remove the C: as the base is self.directoryTree itself
            self.place_directories(self.workingTree,self.path)
        else:
            self.widgets["backButton"].place_forget()
            self.hidden = True


    def change_dir(self,new_path):
        self.hide_directories(self.workingTree,self.path)
        self.path = new_path
        self.workingTree = self.find_directory(self.directoryTree,new_path)
        self.place_directories(self.workingTree,self.path)
        if self.hidden == True:
            self.widgets["backButton"].place(x = 149,y = 49)
            self.hidden = False
        

    def init_widgets(self):
        self.frame.ser.write("B|")
        self.selectedFile = None
        directoryTree = {}
        self.hidden = True
        self.bindings = {}
        for p in ["rootd|","root/newd|","hellof|","root/hif|","root/hellof|","root/new/boyd|"]:
            path = p[:-1].split('/')
            directoryTree = self.add_directories(directoryTree,path)
        finished = True
        while(not finished):
            d = str(self.frame.ser.read_until("|"))
            if d == "|":
                break
            path = d[:-1].split('/')
            directoryTree = self.add_directories(directoryTree,path)
        self.directoryTree = {"C:":directoryTree}
        self.workingTree = directoryTree
        self.path = "C:"
        self.widgets["background"] = layeredLabel(self.frame,[("playbackground",0,0)])
        
        self.widgets["fileWindow"] = dragDropFrame(self.frame,self,relief = FLAT,width =618,height = 525,borderwidth =0)

        self.widgets["taskbar"] = layeredLabel(self.frame,[("taskbar",0,0)])
        self.widgets["start"] = startButton(self.frame,self,"winstart")
        
        self.widgets["fileWindowbg"] = callbackLayeredLabel(self.widgets["fileWindow"], [("filebrowser",0,0)])
        self.widgets["nanocross"] = hoverButtoninFrame(self.widgets["fileWindow"],self,"nanoCross","menu")
        self.widgets["backButton"] = functionalButton(self.widgets["fileWindow"],self,"filebrowserUp",function = self.outof_dir)
        self.widgets["deleteButton"] = functionalButton(self.widgets["fileWindow"],self,"filebrowserDelete",function = self.delete)
        self.widgets["folderName"] = Label(self.widgets["fileWindow"],text = "My Computer",font =("MS Reference Sans Serif bold",16),background = "white",foreground = "#0099FF")
        self.widgets["dirEntry"] = directoryEntry(self.widgets["fileWindow"],self,self.path)
        self.widgets["objects"] = Label(self.widgets["fileWindow"],text = "1 Object(s)",background = "#C0C0C0",font =("MS Reference Sans Serif bold",8),borderwidth = 0)

        self.widgets["rightclickmenu"] = betterLabel(self.frame,"rightclickmenu")
        self.widgets["rightclickmenu2"] = betterLabel(self.frame,"rightclickmenu2")
        self.widgets["rightclickmenu_file"] = betterLabel(self.frame,"rightclickmenu_file")
        self.widgets["new"] = functionalButton(self.frame,self, "new",self.rc2 )
        self.widgets["delete"] = functionalButton(self.frame,self, "delete",self.delete)
        self.widgets["rename"] = functionalButton(self.frame,self, "rename", lambda: None)
        self.widgets["open"] = functionalButton(self.frame,self, "open",lambda: None)
        self.widgets["newfolder"] = functionalButton(self.frame,self, "newfolder", lambda: None)
        self.widgets["wavesound"] = functionalButton(self.frame,self, "wavesound", lambda: None)
        
        self.init_directories(self.workingTree,self.path)
        
        self.widgets["95menu"] = betterLabel(self.frame, "95menu")
        self.widgets["shutdown"] = hoverButton(self.frame,self, "shutdown", "menu")
    def delete(self):#If recursive directory deletion is necessary uncomment lines
        self.hide_directories(self.workingTree,self.path)
        #self.recursiveDelete(self.workingTree[self.selectedFile])
        self.workingTree.pop(self.selectedFile,None)
        self.place_directories(self.workingTree,self.path)
        for i in ["rightclickmenu_file","delete","rename","open"]:
            self.widgets[i].place_forget()
        
    def recursiveDelete(directoryTree):
        if type(directoryTree) == type({}):
            for i in directoryTree.keys():
                recursiveDelete(directoryTree[i])
                directoryTree.pop(i,None)
                #self.ser.write("D,path|")
    def rc2(self):
        self.widgets["rightclickmenu2"].place(x=int(self.widgets["rightclickmenu"].place_info()["x"])+154,y =161+int(self.widgets["rightclickmenu"].place_info()["y"]))
        self.widgets["newfolder"].place(x=int(self.widgets["rightclickmenu2"].place_info()["x"])+2,y =2+int(self.widgets["rightclickmenu2"].place_info()["y"]))
        self.widgets["wavesound"].place(x=int(self.widgets["rightclickmenu2"].place_info()["x"])+2,y =22+int(self.widgets["rightclickmenu2"].place_info()["y"]))
        
    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["fileWindow"].place(x=80,y=18)
        self.widgets["taskbar"].place(x =0,y = 574)
        self.bindings["startBinding"] = self.frame.root.bind("<Button-1>",self.widgets["start"].check_focus,"+")
        self.bindings["rightclickmenuBinding"] = self.frame.root.bind("<Button-1>",self.widgets["fileWindow"].check_focus,"+") 
        self.widgets["fileWindowbg"].place(x=0,y=0)
        self.place_directories(self.workingTree,self.path)
        self.widgets["deleteButton"].place(x=443,y=49)
        self.widgets["nanocross"].place(x=596,y=6)
        self.widgets["start"].place(x=1,y =576)
        self.widgets["folderName"].place(x = 115, y = 133)
        self.widgets["dirEntry"].place(x = 77,y = 93)
        self.widgets["objects"].place(x=8,y=504)
    def hide_All(self):
        PlaceWindow.hide_All(self)
        for val in self.bindings.values():
            self.frame.root.unbind("<Button-1>",val)
