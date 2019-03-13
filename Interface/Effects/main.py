import tkinter as tk
from tkinter import filedialog
from effects import *
from PIL import Image
class Application(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.master = master

        self.pack()
        self.create_widgets()

        self.selected_file_path = None
        self.console = None

    def create_widgets(self):
        self.choose_file = tk.Button(self)
        self.choose_file["text"] = "Choose File"
        self.choose_file["command"] = self.choose_file_cmd
        self.choose_file.pack(side="left")

        self.filter = tk.Button(self)
        self.filter["text"] = "Filter"
        self.filter["command"] = self.filter_cmd
        self.filter.pack(side="right")

        self.delay = tk.Button(self)
        self.delay["text"] = "Delay"
        self.delay["command"] = self.delay_cmd
        self.delay.pack(side="right")

        self.flange = tk.Button(self)
        self.flange["text"] = "Flange"
        self.flange["command"] = self.flange_cmd
        self.flange.pack(side="right")

        self.save = tk.Button(self)
        self.save["text"] = "Save"
        self.save["command"] = self.save_cmd
        self.save.pack(side="bottom")

        self.quit = tk.Button(self, text="QUIT", fg="red",
                              command=self.master.destroy)
        self.quit.pack(side="bottom")

    def save_cmd(self):
        self.console.save("applied_effects.wav")

    def filter_cmd(self):
        self.console.effectFilter(5)

    def delay_cmd(self):
        self.console.effectDelay(50)

    def flange_cmd(self):
        self.console.effectFlange(20, 10)

    def choose_file_cmd(self):
        self.selected_file_path = filedialog.askopenfilename(initialdir=".", title="Select file",
                                   filetypes=(("16-bit signed WAV files", "*.wav"),))

        self.console = EffectsConsole(self.selected_file_path)

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("800x600")
    root.attributes('-type', 'dialog')

    app = Application(master=root)
    app.mainloop()




