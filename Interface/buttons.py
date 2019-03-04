import serial
from tkinter import *
from tkinter.ttk import Combobox,Style
from PIL import Image, ImageTk,GifImagePlugin,ImageFont,ImageDraw
from random import randint

class betterButton(Button):
    def __init__(self,parent,root,buttonName,**options):
        Button.__init__(self,parent,**options)
        self.root = root
        self.imagePath = ImageTk.PhotoImage(file ="resources/"+buttonName+".gif")
        self.imagePathPressed = PhotoImage(file ="resources/"+buttonName+"_pressed.gif")
        self.buttonName = buttonName
        self.config(relief = FLAT,borderwidth = 0,highlightthickness=0,command =self._on_Click,image = self.imagePath)
        self.bind('<ButtonPress-1>',self._on_pressed)
        self.bind('<ButtonRelease-1>',self._on_release)
    def _on_pressed(self,event):
        self.config(image =self.imagePathPressed)
    def _on_release(self,event):
        self.config(image = self.imagePath)
    def _on_Click(self):
        pass

class serialButton(betterButton):
    def _on_Click(self):
        betterButton._on_Click(self)
        self.root.frame.ser.write(bytes(self.buttonName+'|','utf-8'))

class menuButton(betterButton):
    def __init__(self,parent,root,buttonName,**options):
        self.frame = parent
        self.name = buttonName
        betterButton.__init__(self,parent,root,"menu"+buttonName,**options,borderwidth = 0)
    def _on_Click(self):
        self.frame.ser.write(bytes(self.buttonName+'|','utf-8'))
        self.frame.switch(self.name)

class hoverButton(betterButton):
    def __init__(self,parent,root,buttonName,menu=None,**options):
        self.frame = parent
        self.menu = menu
        betterButton.__init__(self,parent,root,buttonName,**options)
        self.hoverPath = PhotoImage(file ="resources/"+buttonName+"_hover.gif")
        self.bind("<Enter>",self.on_enter)
        self.bind("<Leave>",self.on_leave)
    def on_enter(self,event):
        self.config(image = self.hoverPath)
    def on_leave(self,event):
        self.config(image = self.imagePath)
    def _on_Click(self):
        self.frame.switch(self.menu)

class hoverButtoninFrame(hoverButton):
    def _on_Click(self):
        self.frame.frame.switch(self.menu)

# Hover buttons that need to run some function when pressed. 
class functionalButton(hoverButton):
    def __init__(self,parent,root,buttonName,function,**options):
        hoverButton.__init__(self,parent,root,buttonName,menu=None,**options)
        self.function = function

    def _on_Click(self):
        self.function()
class startButton(hoverButton):
    def __init__(self,parent,root,buttonName,menu=None,**options):
        hoverButton.__init__(self,parent,root,buttonName,menu=None,**options)
        self.menu_open = False
    def place(self,**args):
        self.menu_open = False
        hoverButton.place(self,args)
    def _on_Click(self):
        self.focus()
        if(not self.menu_open):
            self.root.widgets["95menu"].place(x=0,y=260)
            self.root.widgets["shutdown"].place(x=24,y=537)
            self.root.widgets["documents"].place(x=24,y=237)
        else:
            self.root.widgets["95menu"].place_forget()
            self.root.widgets["shutdown"].place_forget()
            self.root.widgets["documents"].place_forget()
        self.menu_open = not self.menu_open
    def check_focus(self,event):
        if event.widget != self and event.widget != self.root.widgets["95menu"] and event.widget != self.root.widgets["shutdown"]:
            self.menu_open = False
            self.root.widgets["95menu"].place_forget()
            self.root.widgets["shutdown"].place_forget()

class duckButton(hoverButton):
    def _on_Click(self):
        self.frame.many_ducks = not self.frame.many_ducks

class exitButton(menuButton):
    def _on_Click(self):
        self.frame.ser.write(bytes(self.buttonName+'|','utf-8'))
        self.frame.root.destroy()
        
        
class pauseButton(serialButton):
    def __init__(self,parent,root,buttonName,**options):
        serialButton.__init__(self,parent,root,buttonName,**options)
        self.imagePathPlay = PhotoImage(file = "resources/play.gif")
        self.imagePathPlayPressed = PhotoImage(file ="resources/play_pressed.gif")
        self.buttonNamePlay = "play"
    def _on_Click(self):
        (self.imagePath,self.imagePathPlay) = (self.imagePathPlay,self.imagePath)
        (self.imagePathPressed,self.imagePathPlayPressed) = (self.imagePathPlayPressed,self.imagePathPressed)
        (self.buttonName,self.buttonNamePlay) = (self.buttonNamePlay,self.buttonName)
        self.config(image = self.imagePath)
        serialButton._on_Click(self)

class betterLabel(Label):
    def __init__(self,frame,imageLoc,**options):
        self.image = PhotoImage(file = "resources/"+imageLoc+".gif")
        Label.__init__(self,frame,image =self.image,borderwidth = 0,**options)
        
class size_rotate_Label(Label):
    def __init__(self,frame,imageLoc,width,height,angle,**options):
        self.image = Image.open("resources/"+imageLoc+".gif").convert("RGBA")
        self.image = self.image.rotate(angle)
        self.image = self.image.resize((width,height),Image.NEAREST)
        self.image = ImageTk.PhotoImage(self.image)
        Label.__init__(self,frame,image =self.image,borderwidth = 0,**options)
        
class animatedLabel(Label):
    def __init__(self,frame,imageLoc,**options):
        self.images = []
        self.load_images(imageLoc)
        self.imageCount = 0
        Label.__init__(self,frame,image = self.images[self.imageCount],borderwidth =0,**options)
        
    def load_images(self,imageLoc):
        animation = Image.open("resources/"+imageLoc+".gif")
        for i in range(animation.n_frames):
            animation.seek(i)
            self.images.append(ImageTk.PhotoImage(animation))
        self.image_number = animation.n_frames
        
    def switch_image(self,index):
        self.imageCount = index
        self.config(image = self.images[self.imageCount])
        
    def inc_image(self):
        self.imageCount = (self.imageCount +1) % self.image_number
        self.config(image = self.images[self.imageCount])

class transparencyLabel(animatedLabel):
    def __init__(self,frame,ani,bkd,x,y,**options):
        self.imageCount = 0
        self.images = []
        self.load_images(ani,bkd,x,y)
        Label.__init__(self,frame,image = self.images[self.imageCount],borderwidth =0,**options)
    def load_images(self,ani,bkd,x,y):
        animation = Image.open("resources/"+ani+".gif")
        width,height = animation.size
        background = Image.open("resources/"+bkd+".png").convert("RGBA")
        background = background.crop((x,y,x+width,y+height))
        self.image_number = animation.n_frames
        for i in range(animation.n_frames):
            animation.seek(i)
            temp = animation.convert("RGBA")
            for j,px in enumerate(temp.getdata()):
                    y = j // width
                    x = j % width
                    temp.putpixel((x, y), (px[0], px[1], px[2], min(255,765-sum(px[:3]),px[3])))
            self.images.append(ImageTk.PhotoImage(Image.alpha_composite(background,temp)))

class layeredLabel(Label):
    def __init__(self,frame,images):
        self.frame = frame
        self.load_images(images)
        Label.__init__(self,frame,image = self.image,borderwidth =0)
    def load_images(self,images):
        self.image = Image.open("resources/"+images[0][0]+".png").convert("RGBA")
        for i in range(1,len(images)):
            temp = Image.open("resources/"+images[i][0]+".png").convert("RGBA")
            self.image.paste(temp,(images[i][1],images[i][2]),temp)
        self.duckImage = self.image
        self.image = ImageTk.PhotoImage(self.image)
        if images[0][0] == "menubkg" or images[0][0] == "bluescreen" or images[0][0] == "playbackground": 
            duck = Image.open("resources/duck.gif").convert("RGBA")
            for i in range(15):
                temp = duck.rotate(randint(0,360))
                temp = temp.resize((randint(1,200),randint(1,300)),Image.NEAREST)
                self.duckImage.paste(temp,(randint(1,800),randint(1,600)),temp)
            self.duckImage = ImageTk.PhotoImage(self.duckImage)
    def switch_to_duck(self):
        if self.frame.many_ducks == True:
            self.config(image = self.duckImage)
        else:
            self.config(image = self.image)

class dragDropFrame(Frame):
    def __init__(self,frame,window,**options):
        Frame.__init__(self,frame,**options)
        self.frame = frame
        self.window = window
        self.move = False
        self.bind("<ButtonPress-1>",self.pickup,"+")
        self.bind("<ButtonRelease-1>",self.putdown,"+")
        self.bind("<B1-Motion>",self.drag,"+")
        self.bind("<Button-3>",self._on_Right_Click)
    def place(self,**kw):
        Frame.place(self,kw)
        self.x = int(self.place_info()["x"])
        self.y = int(self.place_info()["y"])
    def pickup(self,event):
        if event.y < 23:
            self.x = int(self.place_info()["x"])
            self.y = int(self.place_info()["y"])
            self.relx = event.x
            self.rely = event.y
            self.actx = self.frame.root.winfo_pointerx() - self.x
            self.acty = self.frame.root.winfo_pointery() - self.y
            self.move = True
    def drag(self,event):
        if self.move == True:
            self.x =  self.frame.root.winfo_pointerx() - self.actx
            self.y =  self.frame.root.winfo_pointery() - self.acty
            self.place(x= min(max(5-self.relx,self.x),795-self.relx),y= min(max(5-self.rely,self.y),574-self.rely))
    def putdown(self,event):
        if self.move == True:
            self.place(x= min(max(5-self.relx,self.x),795-self.relx),y= min(max(5-self.rely,self.y),574-self.rely))
            self.move = False
    def _on_Right_Click(self,event):
        if event.y > 120:
            
            self.window.widgets["rightclickmenu"].place(x=event.x+self.x,y=event.y+self.y)
            self.window.widgets["new"].place(x=event.x+self.x+3,y=event.y+self.y+163)
            for widget in ["rightclickmenu2","newfolder","wavesound","rightclickmenu_file","delete","rename","open"]:
                self.window.widgets[widget].place_forget()

                
    def check_focus(self,event):
        for widget in ["rightclickmenu","new"]:
            if event.widget == self.window.widgets[widget]:
                return
        self.window.widgets["rightclickmenu"].place_forget()
        self.window.widgets["new"].place_forget()
        self.window.widgets["rightclickmenu2"].place_forget()
        self.window.widgets["newfolder"].place_forget()
        self.window.widgets["wavesound"].place_forget()
        self.window.rcmenu2 = False
                    
class callbackLayeredLabel(layeredLabel):
    def __init__(self,frame,images):
        layeredLabel.__init__(self,frame,images)
        self.bind("<ButtonPress-1>",self.frame.pickup,"+")
        self.bind("<ButtonRelease-1>",self.frame.putdown,"+")
        self.bind("<B1-Motion>",self.frame.drag,"+")
        self.bind("<Button-3>",self.frame._on_Right_Click,"+")
        
        
class betterListBox(Listbox):
    def __init__(self,master,options):
        self.master = master
        Listbox.__init__(self,master)
        for option in options:
            self.insert(END, option)
            
class betterComboBox(Combobox):
    def __init__(self,frame,options,stringvar = None,width = 20,height = 21):
        self.text_font = ('Microsoft Sans Serif','10')
        combostyle = Style()
        combostyle.theme_create('combostyle', parent='alt',
                         settings = {   "TCombobox":     {"configure": {"selectbackground": "#F7A5D2",
                                        "fieldbackground" : "#F7A5D2",
                                        "background"      : "#C0C0C0",
                                        "selectforeground"      : "black",
                                        "bordercolor"      : "#C0C0C0"}}})
        combostyle.theme_use('combostyle')
        Combobox.__init__(self,frame,values=options,textvariable = stringvar,width = width,height = height,state = "readonly",font = self.text_font)
        frame.root.option_add('*TCombobox*Listbox.font', self.text_font)
        self.bind("<<ComboboxSelected>>",self.do)
    def do(self,event):
        pass
class referenceComboBox(betterComboBox):
    def __init__(self,frame,widgets,options,thingsToShow):#things to show is formated as so [[label,number],[]]
        self.thingsToShow = thingsToShow
        self.current_value = StringVar()
        self.widgets = widgets
        betterComboBox.__init__(self,frame,options,self.current_value)
    def do(self,event):
        for i in range(len(self.thingsToShow)):
            for j in range(self.thingsToShow[i][1]):
                widget = self.widgets[self.thingsToShow[i][0]+str(j)]
                if widget.value == self.current_value.get():
                    widget.place(x=widget.x,y=widget.y)
                else:
                    widget.place_forget()    
class betterScale(Scale):
    def __init__(self,frame,value,xVal=400,yVal=400,**options):
        self.value = value
        self.x = xVal
        self.y = yVal
        Scale.__init__(self,frame,from_=0,to=128,orient = HORIZONTAL,length =148,**options)
                 
class browserButton():#78 by 75, where 
    def __init__(self,frame,window,text,filetype):
        self.frame = frame
        self.window = window
        self.filetype = filetype
        self.im = PhotoImage(file ="resources/"+filetype+".gif")
        self.im_pressed = PhotoImage(file ="resources/"+filetype+"_pressed.gif")
        self.path = text
        self.get_text(text)
        self.image = Label(frame,borderwidth = 0,image = self.im)
        self.text  = Label(frame,borderwidth = 0,image = self.text_image)

        self.image_binding  = self.image.bind('<ButtonPress-1>',self._on_pressed,"+")
        self.text_binding  =  self.text.bind('<ButtonPress-1>',self._on_pressed,"+")

        self.image_binding2  = self.image.bind('<Double-Button-1>',self._open,"+")
        self.text_binding2  =  self.text.bind('<Double-Button-1>',self._open,"+")

        self.relx = self.rely =  0
        self.clicked = False
    def get_text(self,text):
        line = ["",""]
        i = j = 0
        text = text.split()
        text = [" " + t for t in text]
        while j < len(text):
            if len(line[i]) + len(text[j]) < 10:
                line[i]+=text[j]
            elif i == 1 or j == len(text)-1:
                line[i]+= text[j][:10-3-len(line[i])] + "..."
                break
            else:
                i+=1
                j-=1
            j+=1
        self.windowSize = (4*max(len(line[0]),len(line[1]))) + 10
        self.text_image           = Image.new("RGB",(self.windowSize,25),(255,255,255))
        self.text_image_sel       = Image.new("RGB",(self.windowSize,25),(0,0,128))
        self.text_image_unclicked = Image.new("RGB",(self.windowSize,25),(255,255,255))
        

        border_colours_sel       = [(0,0,128),(255,255,127)]
        border_colours_unclicked = [(0,0,0),(255,255,255)]
        
        for i in range(self.text_image.width):#draw a border around the image
            self.text_image_sel.putpixel((i,0),border_colours_sel[i%2])
            self.text_image_sel.putpixel((i,self.text_image.height-1),border_colours_sel[i%2])
            
            self.text_image_unclicked.putpixel((i,0),border_colours_unclicked[i%2])
            self.text_image_unclicked.putpixel((i,self.text_image.height-1),border_colours_unclicked[i%2])
            
        for i in range(self.text_image.height):
            self.text_image_sel.putpixel((0,i),border_colours_sel[i%2])
            self.text_image_sel.putpixel((self.text_image.width-1,i),border_colours_sel[i%2])
            
            self.text_image_unclicked.putpixel((0,i),border_colours_unclicked[i%2])
            self.text_image_unclicked.putpixel((self.text_image.width-1,i),border_colours_unclicked[i%2])
        fnt = ImageFont.truetype("resources/micross.ttf",9)
        lineLength = 2+(4*(len(line[0])))
        lineLength2 = 2+(4*(len(line[1])))
        textAdjust = (self.windowSize - lineLength) / 2
        textAdjust2 = (self.windowSize - lineLength2) / 2
        draw = ImageDraw.Draw(self.text_image)
        draw.text((textAdjust,2),line[0],font = fnt ,fill = (0,0,0))
        draw.text((textAdjust2,15),line[1],font = fnt,fill = (0,0,0))
                       
        draw = ImageDraw.Draw(self.text_image_sel)                       
        draw.text((textAdjust,2),line[0],font = fnt,fill = (255,255,255))
        draw.text((textAdjust2,15),line[1],font = fnt,fill = (255,255,255))

        draw = ImageDraw.Draw(self.text_image_unclicked)                       
        draw.text((textAdjust,2),line[0],font = fnt,fill = (0,0,0))
        draw.text((textAdjust2,15),line[1],font = fnt,fill = (0,0,0))

        self.text_image = ImageTk.PhotoImage(self.text_image)
        self.text_image_sel = ImageTk.PhotoImage(self.text_image_sel)
        self.text_image_unclicked = ImageTk.PhotoImage(self.text_image_unclicked)
                       
    def place(self,**kwargs):
        self.relx = kwargs['x']
        self.rely = kwargs['y']

        self.image.bind('<ButtonPress-1>',self._on_pressed,"+")
        self.text.bind('<ButtonPress-1>',self._on_pressed,"+")

        self.image.bind('<Double-Button-1>',self._open)
        self.text.bind('<Double-Button-1>',self._open)
        
        self.image.bind('<Button-3>',self._on_Right_Click)
        self.text.bind('<Button-3>',self._on_Right_Click)
        
        self.image.place(x = self.relx+22,y = self.rely+7 )
        self.text.place(x = self.relx+12+((50-self.windowSize) /2),y = self.rely+44 )
        
    def _on_Right_Click(self,event):
        self._on_pressed(event)
        self.window.widgets["rightclickmenu_file"].place(x=event.x+self.relx+103,y=event.y+self.rely+26)
        self.window.widgets["delete"].place(x=event.x+self.relx+105,y=event.y+self.rely+156)
        self.window.widgets["rename"].place(x=event.x+self.relx+105,y=event.y+self.rely+174)
        self.window.widgets["open"].place(x=event.x+self.relx+105,y=event.y+self.rely+28)
        for widget in ["rightclickmenu2","new","rightclickmenu","newfolder","wavesound"]:
            self.window.widgets[widget].place_forget()

        
    def place_forget(self):
        self.image.unbind('<ButtonPress-1>')
        self.text.unbind('<ButtonPress-1>')

        self.image.unbind('<Double-Button-1>')
        self.text.unbind('<Double-Button-1>')

        self.image.unbind('<Button-3>')
        self.text.unbind('<Button-3>')
        
        self.image.place_forget()
        self.text.place_forget()
        
    def place_info(self,**kwargs):
        outDict = {'x':self.relx,'y':self.rely,'relx':0,'rely':0,'width':78,'height':75,'in':self.frame}
        return outDict.fromkeys(set(outDict.keys())&set(kwargs.keys()))
    
    def _on_pressed(self,event):
        if self.clicked:
            self.image.config(image = self.im)
            self.text.config(image = self.text_image)
            self.clicked = False
        else:
            self.image.config(image = self.im_pressed)
            self.text.config(image = self.text_image_sel)
            self.window.selectedFile = self.path
            
    def _open(self,event):
        if(self.filetype == "folder"):
            self.frame.window.into_dir(self.path)
        else:
            pass
    def check_focus(self,event):
        if event.widget != self.image and event.widget != self.text:
            self.clicked = True
            self._on_pressed(event)
            
        f = ["rightclickmenu_file","delete","rename","open"]
        for widget in f:
            if event.widget == self.window.widgets[widget]:
                return
        for widget in f:
            self.window.widgets[widget].place_forget()

        
class directoryEntry(Entry):
    def __init__(self,frame,root,directory):
        Entry.__init__(self,frame,width = 80,borderwidth = 0)
        self.frame = frame
        self.root = root
        self.insert(0,directory)
        self.bind('<Return>',self._on_Enter)
    def _on_Enter(self,event):
        if self.root.find_directory(self.root.directoryTree,self.get()) != False:
            self.root.change_dir(self.get())
        else:
            self.change_dir(self.root.path)
    def change_dir(self,new_dir):
        self.delete(0,len(self.get()))
        self.insert(0,new_dir)

        
