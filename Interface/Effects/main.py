import tkinter as tk
from tkinter import filedialog
from effects import *
from PIL import Image
import matplotlib.pyplot as plt


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
        self.choose_file.grid(column=0, row=0)

        self.save = tk.Button(self)
        self.save["text"] = "Save"
        self.save["command"] = self.save_cmd
        self.save.grid(column=0, row=1)

        self.quit = tk.Button(self)
        self.quit["text"] = "Quit"
        self.quit["command"] = self.master.destroy
        self.quit.grid(column=0, row=3)

        self.filter_5 = tk.Button(self)
        self.filter_5["text"] = "Filter 1ms"
        self.filter_5["command"] = self.filter_1_cmd
        self.filter_5.grid(column=1, row=1)

        self.filter_10 = tk.Button(self)
        self.filter_10["text"] = "Filter 5ms"
        self.filter_10["command"] = self.filter_5_cmd
        self.filter_10.grid(column=1, row=2)

        self.filter_50 = tk.Button(self)
        self.filter_50["text"] = "Filter 10ms"
        self.filter_50["command"] = self.filter_10_cmd
        self.filter_50.grid(column=1, row=3)

        self.delay_5 = tk.Button(self)
        self.delay_5["text"] = "Delay 5ms"
        self.delay_5["command"] = self.delay_5_cmd
        self.delay_5.grid(column=2, row=1)

        self.delay_10 = tk.Button(self)
        self.delay_10["text"] = "Delay 10ms"
        self.delay_10["command"] = self.delay_10_cmd
        self.delay_10.grid(column=2, row=2)

        self.delay_50 = tk.Button(self)
        self.delay_50["text"] = "Delay 50ms"
        self.delay_50["command"] = self.delay_50_cmd
        self.delay_50.grid(column=2, row=3)

        self.mod_amp_10 = tk.Button(self)
        self.mod_amp_10["text"] = "Amp Mod 10ms"
        self.mod_amp_10["command"] = self.mod_amp_10_cmd
        self.mod_amp_10.grid(column=3, row=1)

        self.mod_amp_100 = tk.Button(self)
        self.mod_amp_100["text"] = "Amp Mod 100ms"
        self.mod_amp_100["command"] = self.mod_amp_100_cmd
        self.mod_amp_100.grid(column=3, row=2)

        self.mod_amp_1000 = tk.Button(self)
        self.mod_amp_1000["text"] = "Amp Mod 1000ms"
        self.mod_amp_1000["command"] = self.mod_amp_1000_cmd
        self.mod_amp_1000.grid(column=3, row=3)

        self.flange = tk.Button(self)
        self.flange["text"] = "Flange"
        self.flange["command"] = self.flange_cmd
        self.flange.grid(column=4, row=1)



    def save_cmd(self):
        self.console.save("applied_effects.wav")

    def filter_1_cmd(self):
        self.console.effectFilter(1)

    def filter_5_cmd(self):
        self.console.effectFilter(5)

    def filter_10_cmd(self):
        self.console.effectFilter(10)

    def delay_5_cmd(self):
        self.console.effectDelay(50)

    def delay_10_cmd(self):
        self.console.effectDelay(50)

    def delay_50_cmd(self):
        self.console.effectDelay(50)

    def mod_amp_10_cmd(self):
        self.console.effectModAmpl(10, 0.75)

    def mod_amp_100_cmd(self):
        self.console.effectModAmpl(100, 0.75)

    def mod_amp_1000_cmd(self):
        self.console.effectModAmpl(1000, 0.75)

    def flange_cmd(self):
        self.console.effectFlange(20, 10)

    def choose_file_cmd(self):
        self.selected_file_path = filedialog.askopenfilename(initialdir=".", title="Select file",
                                   filetypes=(("16-bit signed WAV files", "*.wav"),))
        self.console = EffectsConsole(self.selected_file_path)

    def show_cmd(self):
        if self.console is not None:
            wm = plt.get_current_fig_manager()
            wm.window.attributes('-type', 'dialog')
            plt.plot(self.console.samples[::self.console.msToSmpl(10)])
            plt.show()



if __name__ == "__main__":
    root = tk.Tk()
    # root.geometry("800x600")
    root.attributes('-type', 'dialog')

    app = Application(master=root)
    app.mainloop()




