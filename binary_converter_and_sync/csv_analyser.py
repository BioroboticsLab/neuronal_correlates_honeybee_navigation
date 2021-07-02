import plotly as py
import pandas as pd
import numpy as np

from datetime import datetime
from datetime import time as dt_tm
from datetime import date as dt_date

import plotly.plotly as py
import plotly.tools as plotly_tools
from plotly.graph_objs import *
from table_html import tableHtml
import webbrowser



import os
import tempfile
os.environ['MPLCONFIGDIR'] = tempfile.mkdtemp()
#import matplotlib.pyplot as plt
import matplotlib.pyplot as plt, mpld3

from scipy.stats import gaussian_kde

from IPython.display import HTML

html_string = '''
        <html>
            <head>
                <link rel="stylesheet" href="http://astrometrie.org/stylesheet_report.css">
                <style>body{ margin:0 100; background:whitesmoke; }</style>
            </head>
            <body>
                <h1>Umweltspäher Messprotokoll</h1>
'''

class Csv_analyser:



    # Constructor
    def __init__(self, new_output_filename):
        global edit_consol_output
        edit_consol_output = new_output_filename

    # Converts CSV columns to arrays
    def csv_columns_to_arrays(self, csv_file_path):
        df=pd.read_csv(csv_file_path, sep=',',header=None)
        return df

    # Returns the number of columns of a CSV-file
    def get_csv_num_columns(self, csv_file_path):
        fr_csv = open(csv_file_path, "r")
        num_columns = len(fr_csv.readline().split(','))
        fr_csv.close()
        return num_columns

    # Median Filter
    def median_filter(self, x, filterSize,  time):
        print('filterSize: ' + str(filterSize))
        print('x: ' + str(x))
        filtered_x = []
        filtered_time = []
        temp_med = []
        i=0
        for elem in x:
            temp_med.append(elem)
            if len(temp_med) == filterSize:
                filtered_x.append(np.median(temp_med))
                filtered_time.append(time[i])
                temp = 0
                i += 1
        print('filtered_x: ' + str(filtered_x))
        print('filtered_time: ' + str(filtered_time))
        return [filtered_x, filtered_time]


    # Plots a column of a pandas datafield
    def plot_xy(self, df_to_plot, x_index, y_index, x_label, y_label, title,  dest_path, median_filter = 0):
        time = df_to_plot[x_index]
        temperature = df_to_plot[y_index]
        if median_filter>1:
            #temperature = pd.rolling_median(temperature, median_filter)
            temperature = temperature.rolling(window=median_filter,center=False).median()
            time = time.rolling(window=median_filter,center=False).median()
            #filtered_data = self.median_filter(temperature, median_filter, time)
            #temperature = filtered_data[0]
            #time = filtered_data[1]
        plt.xlabel(x_label)
        plt.ylabel(y_label)
        fig = plt.figure(figsize=(10, 7))
        #fig.suptitle('Titel', fontsize=25)
        ax = fig.gca()
        ax.plot(self.timeListToFloatList(time), temperature)
        ax.set_title(title, fontsize=25)
        ax.set_xlabel(x_label, fontsize=18)
        ax.set_ylabel(y_label, fontsize=18)
        #mpld3.show(fig)
        mpld3.save_html(fig, dest_path)
        print('DONE')

        # Plot as JPG
        # Plots a column of a pandas datafield
    def plot_xy_jpg(self, df_to_plot, x_index, y_index, x_label, y_label, title,  dest_path):
        time = df_to_plot[x_index]
        temperature = df_to_plot[y_index]
        #plt.plot(self.reduce_data(time, 500), self.reduce_data(temperature, 500)) # orange
        plt.plot(self.timeListToFloatList(time), temperature)
        plt.xlabel(x_label)
        plt.ylabel(y_label)
        #fig = plt.figure()
        #fig.suptitle('Titel', fontsize=25)
        #ax = fig.gca()
        #ax.plot(time, temperature)
        #ax.set_title(title, fontsize=25)
        #ax.set_xlabel(x_label, fontsize=18)
        #ax.set_ylabel(y_label, fontsize=18)
        #mpld3.show(fig)
        #mpld3.save_html(fig, dest_path)
        plt.savefig(dest_path+ '.jpg')

    # Deletes data before ploting
    def reduce_data(self, x, aim_len):
        factor = int(len(x)/aim_len)
        x_new = x[::factor]
        return x_new

    # Time data list (hh:mm:ss) to float
    def timeListToFloatList(self, timeList):
        floatList = []
        for time in timeList:
            time_split = time.split(':')
            floatList.append(int(time_split[0]) + (1/60)*int(time_split[1]) + (1/3600)*int(time_split[2]))
        return floatList

    # Generates the HTML-Protocol using a Template
    def generate_html_protocoll(self, csv_path, out_path, protocol_name):
        dates = self.get_dates(csv_path)
        self.add_html_html_elem('h3', 'Bienenstocknummer: ' +self.get_hive_id(csv_path))
        self.add_html_html_elem('h3', 'Hardware UUID: ' +self.get_first_value(csv_path, 17)) # UUID of the STM32

        # Add GPS Coordinates if available
        gps_list = self.get_gps_as_string(csv_path)
        if gps_list[0] != 0:
            self.add_html_html_elem('h3', gps_list[1])
            self.add_html_html_elem('h3', gps_list[2])

        self.add_html_recording_period(dates)

        # Add Sensor Status

        self.add_html_html_elem('p', '&nbsp;')
        self.add_html_html_elem('p', '&nbsp;')
        self.add_html_html_elem('h2', 'Status der Sensoren: ')
        adc_test_results = self.test_data(csv_path, 30, 0.2,['_out_adc.csv','_out_adc.CSV'],6)
        print('adc_test_results: ' + str(adc_test_results))
        fdc_test_results = self.test_data(csv_path, 30, 0.2,['_out_fdc.csv','_out_fdc.CSV'],2) # Test FDC data
        telemetry_test_results = self.test_data(csv_path, 30, 0.5,['_out_telemetry.csv','_out_telemetry.CSV'],18)
        sensorStatusTable = tableHtml(["Sensor    ", "Status    ", "Bemerkung    "])
        staus_problem = '<font color="#FF0000">Problem</font>'
        staus_ok = '<font color="#088A08">OK</font>'
        for adc_ch in range(len(adc_test_results[1])):
            ch_status = staus_ok
            if adc_test_results[1][adc_ch] < int(0.8*adc_test_results[0]): # Mark ADC problem just if less than 80 percent of the files have a problem
                ch_status = staus_problem
            sensorStatusTable.add_line(['Feldsensor ' + str(adc_ch) , ch_status, str(adc_test_results[1][adc_ch]) + ' von ' + str(adc_test_results[0]) + ' Dateien OK'])
        for fdc_ch in range(len(fdc_test_results[1])):
            ch_status = staus_ok
            if fdc_test_results[1][fdc_ch] != fdc_test_results[0]:
                ch_status = staus_problem
            sensorStatusTable.add_line(['Aktivitaetssensor ' + str(fdc_ch) , ch_status, str(fdc_test_results[1][fdc_ch]) + ' von ' + str(fdc_test_results[0]) + ' Dateien OK'])
        telemetry_sensors_to_check = [0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0]
        telemetry_sensors_captions = ['Umweltspaeher-id', 'Uhrzeit', 'Datum', 'Innentemperatursensor', 'Luftfeuchtigkeit (Innen)', 'Waagezelle 0', 'Waagezelle 1', 'Waagezelle 2', 'Gps-Zeit', 'Gps-Lat', 'GPS-Lon', 'GPS-Status', 'GPS-Sats', 'GPS-Hoehe', 'CPU-Temperatur', 'Außentemperatur', 'Außenluftfeuchtigkeit', 'CPU-UUID']
        print('telemetry_test_results: ' + str(telemetry_test_results))
        for tel_sensor_index in range(len(telemetry_test_results[1])):
            if telemetry_sensors_to_check[tel_sensor_index]:
                ch_status = staus_ok
                if telemetry_test_results[1][tel_sensor_index] != telemetry_test_results[0]:
                    ch_status = staus_problem
                sensorStatusTable.add_line([telemetry_sensors_captions[tel_sensor_index] + ' (ID = ' + str(tel_sensor_index) + ')', ch_status, str(telemetry_test_results[1][tel_sensor_index]) + ' von ' + str(telemetry_test_results[0]) + ' Dateien OK'])

        sensorStatusTableHtmlStr = sensorStatusTable.get_html_table()
        global html_string
        html_string = html_string + sensorStatusTableHtmlStr


        """
        print('fdc_test_results: ' + str(fdc_test_results))
        self.add_html_html_elem('h3', 'Aktivitätssensor ')
        self.add_html_html_elem('h4', 'Es liegen Daten von ' + str(fdc_test_results[0]) + ' Tagen vor. Sensor 1 hat an ' + str(fdc_test_results[1][0]) + ' Tagen fehlerfrei funktioniert. Und Sensor 1 hat an ' + str(fdc_test_results[1][1]) + ' Tagen fehlerfrei funktioniert.')
        self.add_html_html_elem('h3', 'Feldmessungen ')
        
        print('adc_test_results: ' + str(adc_test_results))
        self.add_html_html_elem('h4', 'Es liegen ' + str(adc_test_results[0]) + ' Dateien vor.')
        for adc_ch in range(len(adc_test_results[1])):
            self.add_html_html_elem('h5', 'Sensor ' + str(adc_ch) + ': ' + str(adc_test_results[1][adc_ch]) + ' gute Messdateien.')
        self.add_html_html_elem('h3', 'Weitere Sensoren')
        
        print('telemetry_test_results: ' + str(telemetry_test_results))
        for adc_ch in range(len(telemetry_test_results[1])):
            self.add_html_html_elem('h5', 'Sensor ' + str(adc_ch) + ': ' + str(telemetry_test_results[1][adc_ch]) + ' gute Messdateien.')
"""


        telemtry_files = self.get_csv_files_by_suffix(csv_path, ['_telemetry.csv','_telemetry.CSV'])
        for telemtry_file in telemtry_files:
            print('Work on: ' + telemtry_file)
            csv_as_list = self.csv_columns_to_arrays(csv_path + telemtry_file).sort_values(1)
            time_temp_file = 'time_temp_' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  3, 'Zeit', 'Temperatur in C', 'Temperaturverlauf (Innen)', out_path + '/' + time_temp_file)
            hum_temp_file = 'hum_temp_' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  4, 'Zeit', 'Luftfeuchtigkeit', 'Luftfeuchtigkeitsverlauf', out_path+ '/' + hum_temp_file)
            scales_file = 'scale_values_' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  5, 'Zeit', 'Gewicht', 'Gewichtsverlauf', out_path+ '/' + scales_file) # TODO add the 2 others and calc the weight
            chip_temp_file = 'chip_temp_' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  14, 'Zeit', 'Temperatur in C', 'Innentemperatur (an der CPU)', out_path+ '/' + chip_temp_file, median_filter=0)
            time_temp_out_file = 'time_temp_out' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  15, 'Zeit', 'Temperatur in C', 'Temperaturverlauf (Außen)', out_path + '/' + time_temp_out_file)
            hum_temp_out_file = 'hum_temp_out' + telemtry_file[:6] + '.html'
            self.plot_xy(csv_as_list, 1,  16, 'Zeit', 'Luftfeuchtigkeit', 'Luftfeuchtigkeitsverlauf (Außen)', out_path+ '/' + hum_temp_out_file)
            self.add_html_html_elem('p', '&nbsp;')
            self.add_html_html_elem('p', '&nbsp;')
            self.add_html_subTitle('Messungen vom: ' + self.date_str_to_readable(telemtry_file[:6]))
            self.add_html_iframe(time_temp_file)
            self.add_html_iframe(hum_temp_file)
            self.add_html_iframe(scales_file)
            self.add_html_iframe(chip_temp_file)
            self.add_html_iframe(time_temp_out_file)
            self.add_html_iframe(hum_temp_out_file)
        self.finalise_html_report(out_path+ '/' + protocol_name  + '.html')
        webbrowser.open_new(out_path+ '/' + protocol_name  + '.html')
        print('END')

    # Tests if there are changes in a csv file
    def test_data(self, csv_path, values_to_check, quotient_of_changes, suffix, values_per_line):
        #try:
            print('test_data: csv_path: ' + str(csv_path))
            cnt_good_sensor_files = [0] * values_per_line # One element for each value in the csv file
            fdc_files = self.get_csv_files_by_suffix(csv_path, suffix)
            #values_to_check = 20
            #quotient_of_changes = 0.2 # Relative cnt of different values
            number_of_files = 0
            print('test_data-files: ' +  str(fdc_files))
            for fdc_file in fdc_files:
                number_of_files += 1
                fo = open(csv_path+fdc_file, "r")
                #val_first_0 = 0
                #val_first_1 = 0
                number_of_different_values_ch0 = 0
                number_of_different_values_ch1 = 0
                line_first_val = fo.readline()
                #values_per_line = len(line_first_val.split(','))
                vals_first = [0]*values_per_line
                number_of_different_values = [0] * values_per_line
                for i in range(values_to_check):
                    line_first_val = fo.readline()

                    for val_index in range(values_per_line):
                        try:
                            new_val = float(line_first_val.split(',')[val_index])
                        except:
                            new_val = 0
                        vals_first[val_index] += new_val
                        if (i>1):
                            if (round(vals_first[val_index]/(i+1), 8) != round(new_val, 8)):
                                number_of_different_values[val_index] += 1
                                print('vals_first[val_index]/(i+1): ' + str(vals_first[val_index]/(i+1)) + '     new_val: ' + str(new_val))
                        print('number_of_different_values: ' + str(number_of_different_values))
                for val_ind in range(values_per_line):
                    if (number_of_different_values[val_ind]/values_to_check > quotient_of_changes):
                        cnt_good_sensor_files[val_ind] += 1
                #if (number_of_different_values[1]/values_to_check > quotient_of_changes):
                    #cnt_good_sensor_files[1] += 1
            return [number_of_files, cnt_good_sensor_files]
        #except:
            #return [0,0,0]

    def add_html_iframe(self, filename):
        global html_string
        html_string = html_string + '''<iframe width="1100" height="700" frameborder="0" seamless="seamless" scrolling="no" \
        src="''' + filename + '''"></iframe>
                ''' #<p>Text.</p>

    # Adds a Caption to the HTML report
    def add_html_subTitle(self, subTitle):
        global html_string
        html_string = html_string + '<h2>' + subTitle + '</h2>'

    def add_html_html_elem(self, elem, value):
        global html_string
        html_string = html_string + '<' + elem + '>' + value + '</' + elem + '>'

    # Adds the recording period
    def add_html_recording_period(self, dates):
        self.add_html_subTitle('Messzeitraum: ' + self.date_str_to_readable(dates[0]) + ' bis ' + self.date_str_to_readable(dates[-1]))

    def date_str_to_readable(self, date_str):
        readable_date = begin_date = str(date_str[0:2]) + '.' + str(date_str[2:4]) + '.' + '20' + str(date_str[4:26])
        return readable_date

    def finalise_html_report(self, out_path):
        global html_string
        html_string = html_string + '''            </body>
        </html>'''
        f = open(out_path,'w')
        f.write(html_string)
        f.close()

    # Iterates threw a directory and returns a list containg the dates of the CSV-files (first 6 chars of each file)
    def get_dates(self, csv_folder):
        dates = []
        for subdir, dirs, files in os.walk(csv_folder):
            for file in files:
                if file[-4:]=='.csv' or file[-4:]=='.CSV':
                    dates.append(file[:6])
            break  # Without the break it would also scan the subdirs
        print('dates: ' + str(dates))
        return dates

    # Returns the Hive_id from a csv telemetry file
    def get_hive_id(self, csv_path):
        first_telemetry_file = self.get_csv_files_by_suffix(csv_path, ['_telemetry.csv','_telemetry.CSV'])[0]
        fr_csv = open(csv_path+first_telemetry_file, "r")
        hive_id = (fr_csv.readline().split(','))[0]
        fr_csv.close()
        return hive_id

    # Returns the Hive_id from a csv telemetry file
    def get_first_value(self, csv_path, column):
        first_telemetry_file = self.get_csv_files_by_suffix(csv_path, ['_telemetry.csv','_telemetry.CSV'])[0]
        fr_csv = open(csv_path+first_telemetry_file, "r")
        first_value = (fr_csv.readline().split(','))[column]
        fr_csv.close()
        return first_value

    # Return a nice (for humans) formated GPS String
    def get_gps_as_string(self, csv_path):
        first_telemetry_file = self.get_csv_files_by_suffix(csv_path, ['_telemetry.csv','_telemetry.CSV'])[0]
        fr_csv = open(csv_path+first_telemetry_file, "r")
        first_csv_line = (fr_csv.readline().split(','))
        gps_lat = first_csv_line[9]
        gps_lon = first_csv_line[10]
        gps_available = int(first_csv_line[11])
        fr_csv.close()
        if gps_available==1 or gps_available==2:
            return [1, 'Geographische Länge: ' + self.format_gps_coordinate(gps_lat), 'Geographische Breite: ' + self.format_gps_coordinate(gps_lon)]
        else:
            return [0]

    def format_gps_coordinate(self, coordinate_str):
        if coordinate_str[1] == '0':
            return coordinate_str[2:4]+' Grad  '+coordinate_str[4:-2] + ' Minuten (' + coordinate_str[-1] +')'
        else:
            return coordinate_str[1:3]+' Grad  '+coordinate_str[3:-2] + ' Minuten (' + coordinate_str[-1] +')'



    # Returns a list of all telemtry files in a given folder
    def get_csv_files_by_suffix(self, csv_folder, suffix):
        telemetry_files = []
        for subdir, dirs, files in os.walk(csv_folder):
            for file in files:
                if file[-len(suffix[0]):]==suffix[0] or file[-len(suffix[1]):]==suffix[1]:
                    telemetry_files.append(file)
            break  # Without the break it would also scan the subdirs
        print('found these files: ' + str(telemetry_files))
        return telemetry_files

    # Generates the Report folder and calls generate_html_protocoll
    def create_folder_and_start_report(self, csv_path):
        print('create_folder_and_start_report: csv_path: ' + str(csv_path))
        protocol_name = 'Protokoll_' + self.get_dates(csv_path)[0] + '_' + self.get_dates(csv_path)[-1]
        out_path = csv_path + protocol_name
        if not os.path.exists(out_path):
            os.makedirs(out_path)
        self.generate_html_protocoll(csv_path, out_path, protocol_name)


#csv_analyser = Csv_analyser('test_analyses')
#out_path = 'C:/Users/Julian/Desktop/Programming/Repos/Umweltspaeher_logfile_converter/test_3/Protokoll/'
#protocol_name = 'test_protokoll.html'
#csv_path = 'C:/Users/Julian/Desktop/Programming/Repos/Umweltspaeher_logfile_converter/test_3/'
#csv_analyser.create_folder_and_start_report(csv_path)

#csv_analyser.generate_html_protocoll(csv_path, out_path, protocol_name)
#csv_as_list = csv_analyser.csv_columns_to_arrays('C:/Users/Julian/Desktop/Programming/Repos/Umweltspaeher_logfile_converter/test_data_2/020318_out_telemetry.csv')
#csv_analyser.plot_xy(csv_as_list, 1,  3, 'Zeit', 'Temperatur in C', 'Temperaturverlauf', 'C:/Users/Julian/Desktop/Programming/Repos/Umweltspaeher_logfile_converter/test_data_2/time_temp.html')
#csv_analyser.plot_xy(csv_as_list, 1,  4, 'Zeit', 'Luftfeuchtigkeit', 'Luftfeuchtigkeit', 'C:/Users/Julian/Desktop/Programming/Repos/Umweltspaeher_logfile_converter/test_data_2/time_hum.html')
