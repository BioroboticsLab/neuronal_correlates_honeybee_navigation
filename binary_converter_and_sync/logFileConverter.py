import struct
import os  # For iterating through directories
from NcDjiCsvSync import NcDjiCsvSync

class DataConUmweltspaeher:
    telemetry_string_identifier = 'ABCDEFGHIJKLMNOPQR'
    telemetry_string_identifier_byte = b'ABCDEFGHIJKLMNOPQR'
    telemetry_string_identifier_byte_first4 = b'ABCD'
    fdc_identifier_byte = b'0123456789'

    lastValueCh0 = 0
    telemetryStr = ''

    #edit_consol_output = object

    def __init__(self, guiRoot):
        print('')


    def binToCsv(self, bvinary_file_path, csv_out_path, csv_out_path_fdc1004, csv_out_path_telemetry):
        #csv_out_path = "out_adc.csv"
        #csv_out_path_fdc1004 = "out_fdc1004.csv"
        #csv_out_path_telemetry = "out_telemetrey.csv"
        fr = open(bvinary_file_path, "rb")
        fw = open(csv_out_path,'a') # 'a' because for the ADC data every day one csv file should  be created
        fw_fdc = open(csv_out_path_fdc1004,'a') # 'a' because one day of fdc data should be in one csv-file
        fw_telemtry_csv = open(csv_out_path_telemetry,'a') # 'a' because one day of fdc data should be in one csv-file

        print('run')
        i=0
        channel_values = [0,0,0,0,0,0]
        ch = 0 # Channel index
        byte_i = 0 # Byte index
        j = 0
        line_i = 0 # Line index
        try:
            byte = 0
            while True:
                bytes = fr.read(4)
                try:
                    self.process_18_byte(bytes, fr, fw, fw_fdc, fw_telemtry_csv)
                except Exception as e:
                    print(e)
                    print('END')
                #print('byte: ' + str(int.from_bytes(byte, byteorder='big')) + '   raw: ' + str(byte))

                if not bytes:
                    break
        finally:
            fr.close()
            fw.close()
            fw_fdc.close()
            fw_telemtry_csv.close()

    def process_18_byte(self, bytes, fr, fw,  fw_fdc, fw_telemtry_csv):
        # If bytes are for the FDC
        #print('process_18_byte():', bytes)
        fdc1004_ch1 = 0
        fdc1004_ch2 = 0
        global fdc_identifier_byte
        global telemetry_string_identifier_byte
        if bytes[0:10] == self.fdc_identifier_byte:
            try:
                fdc1004_ch1 = struct.unpack('f', bytes[10:14])
            except:
                print('First float: error')
                print('First float:' + str(bytes[10:14]) + '   ' + str(len((bytes[10:14]))))
            try:
                fdc1004_ch2 = struct.unpack('f', bytes[14:18])
            except:
                print('Second float:' + str(bytes[14:18]) + '   ' + str(len((bytes[14:18]))))
            fw_fdc.write(str(fdc1004_ch1)[1:-2] + ',' + str(fdc1004_ch2)[1:-2] + '\r')
            #print(str(fdc1004_ch1)[1:-2] + ' ' + str(fdc1004_ch2)[1:-2])
        elif (bytes == self.telemetry_string_identifier_byte_first4):
            print('telemetry_string_identifier_byte_first4')
            # Check if the following bytes are telemetry_string_identifier_byte
            # TODO test
            last14_telemetry_string_identifier_byte = fr.read(14)
            if last14_telemetry_string_identifier_byte == self.telemetry_string_identifier_byte[4:18]:
                self.telemetryStr = self.process_telemetry_string(fr, fw_telemtry_csv)
                #print(telemetryStr)
        else:
            channel_values = [0,0]
            for ch in range(2):
                channel_values[ch] = bytes[0+2*ch]*256 + bytes[1+2*ch]
            # Ignore 0's in ch1 if it not more than one time
            if not channel_values[0]==0:
                fw.write(str(channel_values[0]) + ',' + str(channel_values[1]) + ',' + self.telemetryStr + '\r')
            elif self.lastValueCh0==0:
                fw.write(str(channel_values[0]) + ',' + str(channel_values[1]) + ',' + self.telemetryStr + '\r')
            self.lastValueCh0 = channel_values[0]
            self.telemetryStr = ''


    # Iterates threw the telemetry String and saves it to aCSV until the stop sequence is detected
    def process_telemetry_string(self, fr, fw_telemetry_csv):
        telemetry_string = ''
        while True:
            byte = fr.read(1)
            #print('new byte:' + str(byte) + '   telemetry_string: ' + str(telemetry_string))
            if not byte:
                break
            telemetry_string += str(chr(int.from_bytes(byte, byteorder='big')))
            global telemetry_string_identifier
            if self.telemetry_string_identifier in telemetry_string:
                break
        # Check if telemetry string is in the wrong format and if yes try to correct it
        print('telemetry_string raw:', telemetry_string)
        if len(telemetry_string.split(',')[1].split(':')) > 2:
            tel_str_list = telemetry_string.split(',')
            dt_temp = tel_str_list[1].split(':')
            dt_temp_new = dt_temp[3] + ':' + dt_temp[4] + ':' + dt_temp[5] + ',' + dt_temp[2] + '.' + dt_temp[1] + '.' + dt_temp[0]
            tel_str_list[1] = dt_temp_new
            telemetry_string = ''
            for elem in tel_str_list:
                telemetry_string += (elem + ',')
            # Delete last unneeded ,
            telemetry_string = telemetry_string[:-1]
            print('telemetry_string new:', telemetry_string)
        fw_telemetry_csv.write(telemetry_string[0:-18] + '\r')
        return telemetry_string[0:-18]

    # Iterates threw all files in a given directory and puts them into the processing functions
    def convert_and_analyse_directory(self, srcPath, destPath, doAnalyses, drone_path, time_shift_to_drone):

        adc_out_file_path = ''
        adc_synced_out_file_path = ''
        #old_date_from_name = 0 # Store to see if a new file has new date
        for subdir, dirs, files in os.walk(srcPath):
            i = 0
            for file in files:
                path_for_single_file = os.path.join(subdir, file)
                print(str(i) + '/' + str(len(files)) + ' current file: ' + str(path_for_single_file))
                date_from_name = file[0:6]
                #if (date_from_name != old_date_from_name) and old_date_from_name != 0:
                    #print('new Date') # TODO
                # str(file) + "_out_adc.csv": for each binary file one csv file
                # str(date_from_name) + "_out_adc.csv": one csv file for all binary files of one day
                adc_out_file_path = destPath + '/' + str(date_from_name) + "_out_adc.csv"
                adc_synced_out_file_path = destPath + '/' + str(date_from_name) + "_out_adc_synced.csv.txt"
                # TODO COMMENT IN!!!!
                self.binToCsv(path_for_single_file, adc_out_file_path, destPath + '/' + str(date_from_name) + "_out_fdc.csv", destPath + '/' + str(date_from_name) + "_out_telemetry.csv",)
                #old_date_from_name = date_from_name
                i += 1
            break  # Without the break it would also scan the subdirs
        # TODO After creating one csv-file create a csv file with braina nd drone data synced
        ncDjiCsvSync = NcDjiCsvSync()
        print('drone_path: ' + str(drone_path))
        print('time_shift_to_drone: ' + str(time_shift_to_drone))
        time_shift_to_drone = int(time_shift_to_drone)
        print('BEGIN - ncDjiCsvSync.synchronise_brain_dji')
        ncDjiCsvSync.synchronise_brain_dji(adc_out_file_path, drone_path, adc_synced_out_file_path, time_shift_to_drone)
        print('END - ncDjiCsvSync.synchronise_brain_dji')

    #binToCsv("D:/00000000.000")
