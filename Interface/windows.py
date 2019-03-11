import serial
from tkinter import *
from tkinter.ttk import Combobox
from os.path import isfile,join
from buttons import *
from PIL import Image, ImageTk,GifImagePlugin

sem = False

def scale(value):
    '''
    Scales the byte between 0 and 100.
    Input, value between +/- 1/2 * 2^16
    '''
    value = abs(value) / 2**15 # Scale between +/-0.5
    value = int(value*200)
    print("Height = ", value)
    return value


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
        self.menus = {"play":PlayScreen(self),"menu":MainMenu(self),"settings":Settings(self),"browsePlay":Browse_For_Play(self),"browseRecord":Browse_For_Record(self),"BlueScreen":BlueScreen(self),"load":loadingScreen(self),"record":RecordScreen(self), "TestingScreen":TestingScreen(self)}#initialize array of window contents
        self.menus[self.currentScreen].show_All()
    def switch(self,screen):
        if self.menus[self.currentScreen].serConnected == True:

            self.menus[self.currentScreen].hide_All()
            self.menus[screen].show_All()
            self.menus[screen].serConnected = True
            self.currentScreen = screen
        elif screen == "menu" or screen == "BlueScreen":
            self.menus[self.currentScreen].hide_All()
            self.menus[screen].show_All()
            self.currentScreen = screen
        else:
            self.menus[self.currentScreen].hide_All()
            self.menus["load"].show_All()
            self.currentScreen = "load"
            self.menus["load"].nextScreen = screen


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
            widget.grid(row = 0,column = 0)



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
        self.widgets["nanocross"] = hoverButton(self.frame,self,"nanoCross",menu="BlueScreen")
        self.widgets["testing_Cross"] = hoverButton(self.frame,self,"nanoCross",menu="TestingScreen")

        self.widgets["menuPlay"] = functionalButton(self.frame,self,"menuplay",lambda: self.frame.switch("browsePlay"))
        self.widgets["menuRecord"] = functionalButton(self.frame,self,"menuRecord",lambda: self.frame.switch("browseRecord"))
        self.widgets["pauseButton"] = menuButton(self.frame,self,"settings")
        self.widgets["exitButton"] = exitButton(self.frame,self,"exit")


    def show_All(self):

        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1 )
        self.widgets["duck"].place(x = 700,y =30)
        self.widgets["loading"].place(x=700,y = 120)
        self.widgets["button_Area"].place(x = 80,y =180 )

        self.widgets["testing_Cross"].place(x=780,y=0) #800x600
        self.widgets["nanocross"].place(x=309,y=183)
        self.widgets["menuPlay"].place(x=130,y =260)
        self.widgets["menuRecord"].place(x=130,y =330)
        self.widgets["pauseButton"].place(x=130,y =400)
        self.widgets["exitButton"].place(x=130,y =470)
        PlaceWindow.show_All(self)
        self.animate_duck()

class PlayScreen(PlaceWindow):
        # Non interface functions
    def pause(self):
        print("play")

    def playSong(self):
        print("play")


    # Interface functionsswitch
    def redraw_Canvas(self):
        self.widgets["canvas"].delete("all")
        yscale = self.canvas_height/max(self.song_data)
        xscale = self.canvas_width/len(self.song_data)
        startColor = (248,41,137)
        endColor = (23,11,81)
        c = 0
        for i in range(self.canvas_height//2,self.canvas_height):
            self.widgets["canvas"].create_line(0,i,self.canvas_width,i,fill = self.getGradient((0,0,0),(55,24,24),((c*2)/self.canvas_height)))
            c+= 1
        for i in range(len(self.song_data)):
            self.widgets["canvas"].create_rectangle(int(i*xscale),int((self.canvas_height-(yscale*self.song_data[i]))/2),int(((i+1)*xscale)-1),int((self.canvas_height+(yscale*self.song_data[i]))/2),fill = self.getGradient(startColor,endColor,i/len(self.song_data)))
    def getGradient(self,start,end,index):
        v = (   format(int(start[0]+((end[0]-start[0])*index)),'x'),
                format(int(start[1]+((end[1]-start[1])*index)),'x'),
                format(int(start[2]+((end[2]-start[2])*index)),'x'))
        return "#" + str((2-len(v[0]))*'0')+v[0] +str((2-len(v[1]))*'0')+ v[1] +str((2-len(v[2]))*'0')+ v[2]
    def playScreenSerInit(self,path):
        self.frame.ser.write(bytes("P:"+path +"|","utf-8"))
        self.song_data = []
        while(True):
            d = self.frame.ser.read_until("|")
            if d == "|":
                break
            self.song_data.append(d[:-1])
        self.redraw_Canvas()
        self.songLength = int(self.frame.ser.read_until("|")[:-1])#in seconds
        while(self.frame.ser.read_until("|") != "Play|"):
            pass
        self.timerScale = 295/self.songLength
        self.songCounter = 0
        self.play = True
        self.frame.after(1000,self.inc_timer)
    def inc_timer(self):
        self.songCounter += 1
        self.widgets["realtimer"].place(x = 190 +int(self.songCounter/self.timerScale),y = 119)
        if self.songLength != self.songCounter and self.play == True:
             self.frame.after(1000,self.inc_timer)
    def init_widgets(self):
        self.song_data = [234,5000,123,1341,452,425,245,2,625,3567,356,2456,425,4,45,56,3356,453,45,4545,245,356,56,7,56,56,4,45,34,34,3]
        self.serConnected = False
        self.frame.grid(row =0,column =0,sticky = N+S+E+W)

        self.widgets["background"] = layeredLabel(self.frame,[("playbackground",0,0)])
        self.canvas_height = 243
        self.canvas_width = 300
        self.widgets["canvas"] = Canvas(self.frame,background ="black",width = self.canvas_width,height = self.canvas_height,highlightthickness=0)


        self.widgets["start"] = startButton(self.frame,self,"winstart")
        self.widgets["cross"] = hoverButton(self.frame,self,"cross",menu="BlueScreen")
        self.widgets["realplay"] = switchableButton(self.frame,self, "realplay", function = self.pause)
        self.widgets["realpause"] = functionalButton(self.frame,self, "realpause", function = self.playSong)
        self.widgets["realstop"] = functionalButton(self.frame,self, "realstop", function = lambda:None)
        self.widgets["realtimer"] = sliderButton(self.frame,self, "realtimer", self.adjust_counter,190,498,"x")
        self.widgets["realvolume"] = volumeSlider(self.frame,self, "realvolume", lambda:None,296,352,"y")
        self.widgets["revereserooney"] = switchButton(self.frame,self,"reverse",function = self.reverse_play_button)
        #Parts of other buttons
        self.widgets["95menu"] = betterLabel(self.frame, "95menu")
        self.widgets["shutdown"] = hoverButton(self.frame,self, "shutdown", "menu")
        self.widgets["documents"] = hoverButton(self.frame,self, "documents", "browse")
        self.redraw_Canvas()

    def reverse_play_button(self):
        self.widgets["realplay"].switch_images()
        self.widgets["realplay"].config(image = self.widgets["realplay"].imagePath)
    def adjust_counter(self):
        self.songCounter = self.frame.root.winfo_pointerx() - 190

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["canvas"].place(x=274 ,y=140)

        self.startBinding = self.frame.root.bind("<Button-1>",self.widgets["start"].check_focus,"+")
        self.widgets["start"].place(x=1,y =576)
        self.widgets["cross"].place(x=563,y =67)

        self.widgets["realplay"].place(x=84,y =120)
        self.widgets["realpause"].place(x=116,y =120)
        self.widgets["realstop"].place(x= 147,y = 120)
        self.widgets["realtimer"].place(x=190,y =119)
        self.widgets["realvolume"].place(x=248,y =318)
        self.widgets["revereserooney"].place(x=243,y= 176)
        PlaceWindow.show_All(self)
    def hide_All(self):
        PlaceWindow.hide_All(self)
        self.frame.root.unbind("<Button-1>",self.startBinding)

class RecordScreen(PlayScreen):
    pass

class Settings(PlaceWindow):

    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("menubkg",0,0),("settingsbkgd",200,100)])

        self.widgets["cancelButton"] = hoverButton(self.frame,self,"cancelbutton","menu")
        self.widgets["okButton"] = functionalButton(self.frame,self,"okbutton",self.acceptSettings)
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
    def acceptSettings(self):
        options = ["volume","scale0","scale1","scale2","scale3"]
        for i in range(len(options)):
            g = self.widgets[options[i]].get()
            if  g != 0:
                g = "S:"+ str(i) +"," + str(g)+".|"
                self.frame.ser.write(bytes(g,"utf-8"))
        self.frame.switch("menu")

class BlueScreen(PlaceWindow):

    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("bluescreen",0,0)])

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        PlaceWindow.show_All(self)

        print(b"T|")
        self.frame.ser.write(b"T|")
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until('|')
            if d == "CONNECT":
                self.serConnected == True
                self.frame.switch("play")

class TestingScreen(PlaceWindow):
    '''
    For testing the differerent serial functions before we get file management to work
    On the string sent from the PC:
    first byte = F for files
    Second byte is one of: P(lay),C(opy),D(elete),A(djust Volume),R(everse)
    From the second 2 bytes to the null is the file directory.
    See main.c switchstatement for more details
    '''

    def test(self):
        print("Starting test")
        self.frame.ser.write(b"FCcopyme.txt|")
        print("FCdeleteme.wav|")
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until('|')
            print(d)

    def reverse(self):
        print("Starting test")
        self.frame.ser.write(b"FRa.wav|")
        print("FCdeleteme.wav|")
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until('|')
            print(d)


    def deleting(self):
        print("Starting deleting test")
        self.frame.ser.write(b"FDdelete.wav|")
        print("FDdelete.wav|")
        if self.frame.ser.in_waiting > 0:
            d = self.frame.ser.read_until(b'|')
            print(d)

    def A4(self):
        file = bytes([])
        print("Starting A4")
        self.frame.ser.write(b"A4|")
        while(self.frame.ser.in_waiting > 0):
            d = self.frame.ser.read_until('EndOfFile')
            file += d
            #print(d)
        print("Reached the end of the file")
        print(len(file))

        print("Writing to a file")
        #The b opens the file in binary mode to write hex directly.
        f = open('dataFile.wav','wb')
        f.write(file)
        f.close()
        print("Done ")

    def stream(self):
        print("Starting streaming")
        self.frame.ser.write(b"AS|")
        eof = False
        d = self.frame.ser.read(2)
        while (True and sem == True):
        #while(self.frame.ser.in_waiting > 0 and not str(d) == b''):
            d = self.frame.ser.read(2)
            val = int.from_bytes(d,"little",signed=True)
            print("Data:", d)
            print("Int:  ",str(int.from_bytes(d,"little",signed=True)))

            self.widgets["canvas"].delete("all")
            self.widgets["canvas"].create_rectangle(10,10,100,scale(val), fill="red")
            sem = False
        print("Reached the end of the file")

        #The b opens the file in binary mode to write hex directly.
        print("Done ")



    def init_widgets(self):
        self.widgets["background"] = layeredLabel(self.frame,[("win95loading",0,0)])
        self.widgets["okButton"] = functionalButton(self.frame,self,"okbutton",self.A4)
        self.widgets["test"] = functionalButton(self.frame,self,"neverbutton",self.stream)
        self.canvas_height = 243
        self.canvas_width = 300
        self.widgets["canvas"] = Canvas(self.frame,background ="black",width = self.canvas_width,height = self.canvas_height,highlightthickness=0)
        self.widgets["canvas"].create_rectangle(10,10,100,100, fill="red")

    def show_All(self):
        self.widgets["background"].place(x=0,y=0,relwidth = 1,relheight =1)
        self.widgets["okButton"].place(x=700,y=100)
        self.widgets["test"].place(x=700,y=125)
        self.widgets["canvas"].place(x=274 ,y=140)
        PlaceWindow.show_All(self)


class loadingScreen(PlaceWindow):
    def return_to_menu(self,event):
        self.frame.switch("menu")
    def init_widgets(self):
        self.nextScreen = "menu"
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
            d = self.frame.ser.read_until(b'|')
            if d.endswith(b"CONNECT|"):
                self.serConnected = True
                self.frame.ser.write(b"ACK|")
                self.frame.switch(self.nextScreen)
        if self.serConnected == False:
            self.widgets["load_big"].inc_image()
            self.job = self.frame.after(140,self.animate)#repeat every 40 ms

    def hide_All(self):
        PlaceWindow.hide_All(self)
        if self.job is not None:
            self.frame.after_cancel(self.job)
        self.job = None


class Browse_For_Play(PlaceWindow):
    '''
    While Idris works on the MBED file reading code
    I will impliment communication in changes in volume
    '''
    def add_directories(self,directoryTree,path):
        if len(path) == 1:
            print(path[0])
            if path[0][-1] =='d':#is a directory
                if path[0][:-1] not in directoryTree:
                    directoryTree[path[0][:-1]] = {}#empty folder = dictionary
            elif path[0][-1] =='f':
                directoryTree[path[0][:-1]] = path[0][:-1]
            return directoryTree

        p = path.pop(0)
        if p in directoryTree.keys():
            self.add_directories(directoryTree[p],path)
        else:
            directoryTree[p] = self.add_directories({},path)
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
        self.counter = 0
        for key in directoryTree.keys():
            self.bindings[path+"/"+key] = self.frame.root.bind("<Button-1>",self.widgets[path+"/"+key].check_focus,"+")
            self.widgets[path+"/"+key].place(x = 189+(78*(self.counter%5)),y=169+ (75*(self.counter//5)))
            self.counter+=1
        self.widgets["objects"].config(text = str(self.counter) + " Object(s)")
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
        self.selectedFile = None
        directoryTree = {}
        self.hidden = True
        self.bindings = {}

        #for p in ["rootd|","root/newd|","hellof|","root/hif|","root/hellof|","root/new/boyd|"]:
        #    path = p[:-1].split('/')
        #    directoryTree = self.add_directories(directoryTree,path)

        self.directoryTree = {"C:":directoryTree}
        self.workingTree = directoryTree
        self.path = "C:"
        self.widgets["background"] = layeredLabel(self.frame,[("playbackground",0,0)])

        self.widgets["fileWindow"] = dragDropFrame(self.frame,self,relief = FLAT,width =618,height = 525,borderwidth =0)

        self.widgets["taskbar"] = layeredLabel(self.frame,[("taskbar",0,0)])
        self.widgets["start"] = startButton(self.frame,self,"winstart")

        self.widgets["fileWindowbg"] = callbackLayeredLabel(self.widgets["fileWindow"], [("filebrowser",0,0)])
        self.widgets["nanocross"] = hoverButtoninFrame(self.widgets["fileWindow"],self,"nanoCross","play")
        self.widgets["backButton"] = functionalButton(self.widgets["fileWindow"],self,"filebrowserUp",function = self.outof_dir)
        self.widgets["deleteButton"] = functionalButton(self.widgets["fileWindow"],self,"filebrowserDelete",function = self.delete)
        self.widgets["copyButton"] = functionalButton(self.widgets["fileWindow"],self,"filebrowserCopy",function = self.copy)
        self.widgets["folderName"] = Label(self.widgets["fileWindow"],text = "My Computer",font =("MS Reference Sans Serif bold",16),background = "white",foreground = "#0099FF")
        self.widgets["dirEntry"] = directoryEntry(self.widgets["fileWindow"],self,self.path)
        self.widgets["objects"] = Label(self.widgets["fileWindow"],text = "1 Object(s)",background = "white",font =("MS Reference Sans Serif bold",8),borderwidth = 0)
        self.File_Name = StringVar()
        self.widgets["FileName"] = Label(self.widgets["fileWindow"],background = "white",font =("Arial bold",10),textvariable=self.File_Name,borderwidth = 0)
        self.Size_Bytes = StringVar()
        self.widgets["Size_Bytes"] = Label(self.widgets["fileWindow"],background = "white",font =("MS Reference Sans Serif bold",8),textvariable=self.Size_Bytes,borderwidth = 0)
        self.Song_Length = StringVar()
        self.widgets["SongLength"] = Label(self.widgets["fileWindow"],background = "white",font =("MS Reference Sans Serif bold",8),textvariable=self.Song_Length,borderwidth = 0)
        self.widgets["rightclickmenu"] = betterLabel(self.frame,"rightclickmenu")
        self.widgets["rightclickmenu2"] = betterLabel(self.frame,"rightclickmenu2")
        self.widgets["rightclickmenu_file"] = betterLabel(self.frame,"rightclickmenu_file")
        self.widgets["new"] = functionalButton(self.frame,self, "new",self.rc2 )
        self.widgets["delete"] = functionalButton(self.frame,self, "delete",self.delete)
        self.widgets["rename"] = functionalButton(self.frame,self, "rename", self.rename)
        self.widgets["open"] = functionalButton(self.frame,self, "open",self.opens)
        self.widgets["newfolder"] = functionalButton(self.frame,self, "newfolder", lambda: self.new("folder"))
        self.widgets["wavesound"] = functionalButton(self.frame,self, "wavesound", lambda: self.new("file"))

        self.init_directories(self.workingTree,self.path)

        self.widgets["95menu"] = betterLabel(self.frame, "95menu")
        self.widgets["shutdown"] = hoverButton(self.frame,self, "shutdown", "menu")
        self.widgets["documents"] = hoverButton(self.frame,self, "documents", "browse")
    def opens(self):
        self.frame.ser.write(bytes("P:"+self.path +"/" +self.selectedFile+"/"+'|',"utf-8"))
        self.frame.switch("play")
    def delete(self):#If recursive directory deletion is necessary uncomment lines
        self.hide_directories(self.workingTree,self.path)
        #self.recursiveDelete(self.workingTree[self.selectedFile])
        self.workingTree.pop(self.selectedFile,None)
        if len(self.path) == 2:#if is "C:"
            p = "/"
        else: #if is "C:/xxx/xx etc."
            p = self.path[2:]
        print("FD"+ p +self.selectedFile+'|')
        self.frame.ser.write(bytes("FD"+ p +self.selectedFile+'|',"utf-8"))
        self.place_directories(self.workingTree,self.path)
        for i in ["rightclickmenu_file","delete","rename","open"]:
            self.widgets[i].place_forget()
    def copy(self):
        self.hide_directories(self.workingTree,self.path)
        if len(self.path) == 2:#if is "C:"
            p = "/"
        else: #if is "C:/xxx/xx etc."
            p = self.path[2:]
        print("FC"+p +self.selectedFile+'|')
        self.frame.ser.write(bytes("FC"+p +self.selectedFile+'|',"utf-8"))
        filetype = self.widgets[self.path+"/"+self.selectedFile].filetype
        text = self.selectedFile + ".copy"
        if filetype == "folder":
            self.workingTree[text] = {}
        else:
            self.workingTree[text] = text
        self.widgets[self.path+"/"+text] = browserButton(self.widgets["fileWindow"],self,text,filetype)
        self.place_directories(self.workingTree,self.path)

    def rename(self):#TODO
        if type(self.workingTree[self.selectedFile]) == type({}):
            self.delete()
            self.new("folder")
        else:
            self.delete()
            self.new("file")

    def new(self,filetype):
        self.widgets["newFile"] = browserButtonNew(self.widgets["fileWindow"],self,filetype)
        self.widgets["newFile"].place(x = 189+(78*(self.counter%5)),y=169+ (75*(self.counter//5)))

        self.counter +=1
        self.widgets["objects"].config(text = str(self.counter) + " Object(s)")
    def newDone(self,text,filetype):
        self.widgets["newFile"].place_forget()
        del self.widgets["newFile"]
        self.widgets.pop("newFile",None)

        self.hide_directories(self.workingTree,self.path)
        if text in self.workingTree:
            text += "(1)"
        if filetype == "folder":
            self.frame.ser.write(bytes("Fd:"+self.path +"/" +text+"/"+'|',"utf-8"))
            self.workingTree[text] = {}
        else:
            self.frame.ser.write(bytes("Ff:"+self.path +"/" +text+"/"+'|',"utf-8"))
            self.workingTree[text] = text
        self.widgets[self.path+"/"+text] = browserButton(self.widgets["fileWindow"],self,text,filetype)
        self.place_directories(self.workingTree,self.path)

    def recursiveDelete(self,directoryTree):
        if type(directoryTree) == type({}):
            for i in directoryTree.keys():
                self.recursiveDelete(directoryTree[i])
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
        self.widgets["copyButton"].place(x=266,y=49)
        self.widgets["nanocross"].place(x=596,y=6)
        self.widgets["start"].place(x=1,y =576)
        self.widgets["folderName"].place(x = 115, y = 133)
        self.widgets["dirEntry"].place(x = 77,y = 93)
        self.widgets["objects"].place(x=8,y=504)
        self.widgets["FileName"].place(x = 23,y = 230)
        self.widgets["Size_Bytes"].place(x = 23,y = 250)
        self.widgets["SongLength"].place(x = 23, y = 270)
    def hide_All(self):
        self.hidden = True
        PlaceWindow.hide_All(self)
        self.frame.root.unbind("<Button-1>")
class Browse_For_Record(Browse_For_Play):
    def getDirs(self):
        self.frame.ser.write(b"B|")#Make this happen in place.
        while(True):
            d = self.frame.ser.read_until(b'|').decode("utf-8")
            if d == "ed|":
                break
            d =  d[1:]#will always start with a /, so remove
            #print(d)
            path = d[:-1].split('/')
            #print(path)
            if path[0] != 'd' and path[0] != 'null)d':
                self.workingTree = self.add_directories(self.workingTree,path)
        self.init_directories(self.workingTree,self.path)

    def show_All(self):
        self.getDirs()
        Browse_For_Play.show_All(self)
