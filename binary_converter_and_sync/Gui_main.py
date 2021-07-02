# -*- coding: cp1252 -*-
### Bird_Attack_Analyser_Gui_v13 ###
### by JP ###
### Version Info:
### v01: first version
### v02: Renamend to Bird_Attack_Analyser_Gui_v02

from tkinter import *
import tkinter, tkinter.constants, tkinter.filedialog
from tkinter import ttk

from logFileConverter import DataConUmweltspaeher
from Spike2ResultMerger import Spike2ResultsMerger
import os  # For iterating through directories
import webbrowser


class Converter_Gui:
    conv_instance = object
    guiRoot = object
    edit_kml_path = object
    edit_dest_path = object
    edit_drone_path = object
    edit_consol_output = object
    createPdfReport = object
    pb = object
    edit_synced_csv = object
    edit_sorted_spikes = object
    edit_spike_frequ = object
    edit_bin_step_width = object
    

    def __init__(self, new_guiRoot, new_conv_instance):
        global guiRoot
        guiRoot = new_guiRoot
        global conv_instance
        conv_instance = new_conv_instance
        global edit_kml_path
        edit_kml_path = Text(guiRoot, height=1, width=120)
        global edit_dest_path
        edit_dest_path = Text(guiRoot, height=1, width=120)
        global edit_time_shift_to_drone
        edit_time_shift_to_drone = Text(guiRoot, height=1, width=120)
        global edit_drone_path
        edit_drone_path = Text(guiRoot, height=1, width=120)
        global edit_synced_csv
        edit_synced_csv = Text(guiRoot, height=1, width=120)
        global edit_sorted_spikes
        edit_sorted_spikes = Text(guiRoot, height=1, width=120)
        global edit_spike_frequ
        edit_spike_frequ = Text(guiRoot, height=1, width=120)
        global edit_bin_step_width
        edit_bin_step_width = Text(guiRoot, height=1, width=120)
        setup_gui()

# Gets Progress in percent and displays it
def set_progress(progress):
    global pb
    pb["value"] = progress
    global guiRoot
    pb.pack()
    mainloop()


def callback_select_src():
    path = tkinter.filedialog.askdirectory()
    global edit_kml_path
    edit_kml_path.delete('@0,0', END)
    edit_kml_path.insert(END, path)
    mainloop()
    print(path)


def callback_select_src_file():
    path = tkinter.filedialog.askopenfilename()
    global edit_kml_path
    edit_kml_path.delete('@0,0', END)
    edit_kml_path.insert(END, path)
    mainloop()
    print(path)


def callback_select_dest():
    path = tkinter.filedialog.askdirectory()
    global edit_dest_path
    edit_dest_path.delete('@0,0', END)
    edit_dest_path.insert(END, path)
    mainloop()
    print(path)

def callback_select_drone_path():
    path = tkinter.filedialog.askdirectory()
    global edit_kml_path
    edit_drone_path.delete('@0,0', END)
    edit_drone_path.insert(END, path)
    mainloop()
    print(path)

def callback_select_synced_csv():
    path = tkinter.filedialog.askopenfilename()
    global edit_kml_path
    edit_synced_csv.delete('@0,0', END)
    edit_synced_csv.insert(END, path)
    mainloop()
    print(path)

def callback_select_sorted_spikes():
    path = tkinter.filedialog.askopenfilename()
    global edit_kml_path
    edit_sorted_spikes.delete('@0,0', END)
    edit_sorted_spikes.insert(END, path)
    mainloop()
    print(path)


def callback_start():
    # xmlCsvConf.print_to_consol('TEST')
    srcPath = edit_kml_path.get("@0,0", END)
    srcPath = srcPath[:-1].replace("/", "\\")
    destPath = edit_dest_path.get("@0,0", END)
    destPath = destPath[:-1].replace("/", "\\")

    drone_path = edit_drone_path.get("@0,0", END)
    drone_path = drone_path[:-1].replace("/", "\\")
    time_shift_to_drone =edit_time_shift_to_drone.get("@0,0", END)

    conv_instance.convert_and_analyse_directory(srcPath, destPath, True, drone_path, time_shift_to_drone)


def callback_help():
    webbrowser.open_new(r"https://docs.google.com/document/d/1W783tdo17F6Kw3oUdrvv6jfucJQHIXh2GhsQVguE3xM/edit?usp=sharing")

def callback_start_sorted_spikes_merging():
    # xmlCsvConf.print_to_consol('TEST')
    spike2_file = edit_sorted_spikes.get("@0,0", END)
    spike2_file = spike2_file[:-1].replace("/", "\\")
    synced_csv_file = edit_synced_csv.get("@0,0", END)
    synced_csv_file = synced_csv_file[:-1].replace("/", "\\")
    spike_frequ = edit_spike_frequ.get("@0,0", END)
    bin_step_width = float(edit_bin_step_width.get("@0,0", END))
    outFile = synced_csv_file + "_spike_rates_" + str(bin_step_width) + "s_intervalls.txt"

    spike2ResultsMerger = Spike2ResultsMerger()
    #spike2ResultsMerger.sync_drone_and_spike_data_1s_intervalls(spike2_file, synced_csv_file, outFile, spike_frequ)
    spike2ResultsMerger.sync_drone_and_spike_data_flex_intervalls(spike2_file, synced_csv_file, outFile, spike_frequ, bin_step_width)
    #conv_instance.convert_and_analyse_directory(srcPath, destPath, createPdfReport.get(), drone_path, time_shift_to_drone)

def setup_gui():
    guiRoot.title("Neurocopter Binary Converter and Analyser - by JP")
    label_up3 = Label(guiRoot, height=1, width=150)
    label_up3.pack()
    button_help = Button(guiRoot, text='Hilfe', width=25, command=callback_help)
    button_help.pack()
    label_up = Label(guiRoot, height=1, width=150)
    label_up.pack()
    edit_kml_path.insert(END, 'Quellordner (muss Binaer-Dateien enthalten)')
    edit_kml_path.pack()
    button_select_folder = Button(guiRoot, text='Quellordner auswaehlen', width=25, command=callback_select_src)
    button_select_folder.pack()
    #button_select_file = Button(guiRoot, text='Quelldatei auswaehlen', width=25, command=callback_select_src_file)
    #button_select_file.pack()
    label_up2 = Label(guiRoot, height=1, width=150)
    label_up2.pack()
    edit_dest_path.insert(END, 'Zielordner fr CSV-Dateien')
    edit_dest_path.pack()
    button_select_folder = Button(guiRoot, text='Zielordner auswaehlen', width=25, command=callback_select_dest)
    button_select_folder.pack()
    label_up3 = Label(guiRoot, height=1, width=150)
    label_up3.pack()
    edit_drone_path.insert(END, 'Quellordner (muss CSV Dateien von DatCon enthalten)')
    edit_drone_path.pack()
    button_select_folder = Button(guiRoot, text='Drone Logs auswaehlen', width=25, command=callback_select_drone_path)
    button_select_folder.pack()
    label_up3 = Label(guiRoot, height=1, width=150)
    label_up3.pack()
    label_time_shift_desc = Label(guiRoot, height=1, width=150, text='Zeitverschiebung STM32 zur Drohne in Stunden')
    label_time_shift_desc.pack()
    edit_time_shift_to_drone.insert(END, '8')
    edit_time_shift_to_drone.pack()
    label_center_up = Label(guiRoot, height=1, width=120)
    label_center_up.pack()
    button_run = Button(guiRoot, text='Start', width=25, command=callback_start)
    button_run.pack()
    label_center = Label(guiRoot, height=3, width=120)
    label_center.pack()
    # For Spike2 results merger
    #label_center_up = Label(guiRoot, height=2, width=120)

    edit_synced_csv.insert(END, 'Synchronised CSV-file')
    edit_synced_csv.pack()
    button_synced_csv = Button(guiRoot, text='Select Synchronised CSV-file', width=25, command=callback_select_synced_csv)
    button_synced_csv.pack()
    label_seperator = Label(guiRoot, height=1, width=120)
    label_seperator.pack()
    edit_sorted_spikes.insert(END, 'Sorted Spikes TXT-file from Spike2')
    edit_sorted_spikes.pack()
    button_sorted_spikes = Button(guiRoot, text='Select  TXT-file from Spike2', width=25, command=callback_select_sorted_spikes)
    button_sorted_spikes.pack()
    label_seperator1 = Label(guiRoot, height=1, width=120)
    label_seperator1.pack()
    label_spike_frequ = Label(guiRoot, height=1, width=150, text='Frequency used in spike2')
    label_spike_frequ.pack()
    edit_spike_frequ.insert(END, '36000')
    edit_spike_frequ.pack()
    label_seperator2 = Label(guiRoot, height=1, width=120)
    label_seperator2.pack()
    label_bin_step_width = Label(guiRoot, height=1, width=150, text='Bining step width in s')
    label_bin_step_width.pack()
    edit_bin_step_width.insert(END, '0.1')
    edit_bin_step_width.pack()
    label_seperator2 = Label(guiRoot, height=1, width=120)
    label_seperator2.pack()
    button_run_sorted_spikes_merging = Button(guiRoot, text='Merge Sorted Spikes', width=25, command=callback_start_sorted_spikes_merging)
    button_run_sorted_spikes_merging.pack()


    #edit_consol_output.insert(END, 'Meldungen')
    #edit_consol_outputScroll = Scrollbar(guiRoot)
    #edit_consol_outputScroll.pack(side=RIGHT, fill=Y)
    #edit_consol_output.pack()
    #edit_consol_outputScroll.config(command=edit_consol_output.yview)
    #edit_consol_output.config(yscrollcommand=edit_consol_outputScroll.set)
    ##global pb
    #pb = ttk.Progressbar(guiRoot, orient="horizontal", length=200, mode="determinate")
    #pb.pack()
    label_down = Label(guiRoot, height=1, width=150)
    label_down.pack()
    guiRoot.mainloop()




guiRoot = Tk()
#edit_consol_output = Text(guiRoot, height=25, width=120)
dataConUmweltspaeher = DataConUmweltspaeher(guiRoot)
cGui = Converter_Gui(guiRoot, dataConUmweltspaeher)




#dataConUmweltspaeher.convert_and_analyse_directory("D:/", "D:/", True)
