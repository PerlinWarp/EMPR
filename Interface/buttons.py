import serial
from tkinter import *
from PIL import Image, ImageTk,GifImagePlugin
class betterButton(Button):
    def __init__(self,parent,root,buttonName,**options):
        Button.__init__(self,parent,**options)
        self.root = root
        self.imagePath = ImageTk.PhotoImage(file ="resources/"+buttonName+".gif")
        self.imagePathPressed = PhotoImage(file ="resources/"+buttonName+"_pressed.gif")
        self.buttonName = buttonName
        self.config(relief = SUNKEN,borderwidth = 0,command =self._on_Click,image = self.imagePath)
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
    def __init__(self,parent,root,buttonName,menu,**options):
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
        self.image = Image.open("resources/"+imageLoc+".gif")
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
                    temp.putpixel((x, y), (px[0], px[1], px[2], min(255,765-sum(px[:3]))))
            self.images.append(ImageTk.PhotoImage(Image.alpha_composite(background,temp)))

class layeredLabel(Label):
    def __init__(self,frame,images):
        self.load_images(images)
        Label.__init__(self,frame,image = self.image,borderwidth =0)
    def load_images(self,images):
        self.image = Image.open("resources/"+images[0][0]+".png").convert("RGBA")
        for i in range(1,len(images)):
            temp = Image.open("resources/"+images[i][0]+".png").convert("RGBA")
            self.image.paste(temp,(images[i][1],images[i][2]),temp)
        self.image = ImageTk.PhotoImage(self.image)
        
