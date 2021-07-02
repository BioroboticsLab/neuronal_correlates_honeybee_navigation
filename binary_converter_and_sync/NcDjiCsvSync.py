from droneLogAnalyser import DroneLogAnalyser
from BrainDataHelper import BrainDataHelper

# Neurocopter DJI Csv syncroniser
class NcDjiCsvSync:

    def synchronise_brain_dji(self, csv_brain_data_file, dji_csv_file_path, synced_csv_file, time_shift):
        sample_rate = int(self.get_sample_rate(csv_brain_data_file))
        fr_brain_data_csv = open(csv_brain_data_file, "r")
        while True:  # Go to sync point
            brain_data_line = fr_brain_data_csv.readline()
            if len(brain_data_line) > 30:
                print('synchronise_brain_dji: found sync point in brain data file')
                break
        brainDataHelper = BrainDataHelper(csv_brain_data_file)
        fw = open(synced_csv_file, 'w')
        drone_log_analyser = DroneLogAnalyser(dji_csv_file_path)
        drone_telemetry_index = 0
        i = 0
        brain_lines_until_telemetry = 1
        drone_telemetry_lines = []
        len_stm32_tel = 0
        while brain_data_line:
            #print('brain_data_line: ' + str(brain_data_line))
            if len(brain_data_line) > 30:
                # Remove STM32 Hardware ID and NULLs
                #print('brain_data_line old: ' + str(brain_data_line))
                brain_data_line = str(brain_data_line.split(',')[:-1])[1:-1].replace('\'', '')
                #print('brain_data_line new: ' + str(brain_data_line))
                time = brainDataHelper.calc_time(brain_data_line)
                date = brainDataHelper.calc_date(brain_data_line)
                time[0] += time_shift
                # Check if timeshift causes a day change
                if time[0] > 23:
                    time[0] -= 24
                    date[0] += 1  # TODO make month and year change...
                    print('WARNING: Day change. If month should change to wrong data is produced.')
                date_next_telemetry, time_next_telemetry, lines_between_sync, telemetry_duration = brainDataHelper.get_next_telemetry()
                if date_next_telemetry == []:  # End of file reached. leave last 10 seconds (because it can not be interpolated)
                    break
                drone_telemetry_lines = (drone_log_analyser.get_drone_telemetry_lines(date, time, telemetry_duration))
                if drone_telemetry_lines == []:
                    print('drone_telemetry_lines == []')
                    break
                else:
                    print('drone_telemetry_lines:', drone_telemetry_lines)
                drone_telemetry_index = 0
                brain_lines_until_telemetry = int(lines_between_sync/len(drone_telemetry_lines))+1  # Drone telemetry after this amount of lines
                i = 0
                len_stm32_tel = len(brain_data_line.split(','))-3  # For lines without stm32 telemetry data (to create empty cells)
            if drone_telemetry_lines != [] and (i % brain_lines_until_telemetry) == 0:
                if len(brain_data_line) > 30:
                    fw.write(brain_data_line[:-1] + drone_telemetry_lines[drone_telemetry_index])
                else:
                    fw.write(brain_data_line[:-1] + ','*len_stm32_tel + drone_telemetry_lines[drone_telemetry_index])
                #print('add drone_telemetry_line: ' + str(drone_telemetry_lines[drone_telemetry_index]))
                #print('i: ' + str(i) + '    i % brain_lines_until_telemetry: ' + str((i % brain_lines_until_telemetry)))
                #print('brain_lines_until_telemetry: ' +str(brain_lines_until_telemetry))
                #print('len(drone_telemetry_lines): ' + str(len(drone_telemetry_lines)))
                drone_telemetry_index += 1
            else:
                fw.write(brain_data_line[:-1] + '\r')
            brain_data_line = fr_brain_data_csv.readline()
            i += 1
        fw.close()

    def get_sample_rate(self, csv_brain_data_file):
        timestamp0 = []
        timestamp1 = []
        records_till_telemetry = 0
        fr_csv = open(csv_brain_data_file, "r")
        while True:
            line = fr_csv.readline()
            if len(line) > 30:
                timestamp0 = line.split(',')[3].split(':')
                break
        while True:
            line = fr_csv.readline()
            if len(line) > 30:
                timestamp1 = line.split(',')[3].split(':')
                break
            records_till_telemetry += 1
        print('timestamp0:', timestamp0)
        print('timestamp1:', timestamp1)
        # Activate if date and time are in wrong format
        shift = 0
        timestamp0_in_s = int(timestamp0[0+shift])*3600 + int(timestamp0[1+shift])*60 + int(timestamp0[2+shift])
        timestamp1_in_s = int(timestamp1[0+shift])*3600 + int(timestamp1[1+shift])*60 + int(timestamp1[2+shift])
        time_between_telemetry = timestamp1_in_s - timestamp0_in_s
        samples_per_second = float(records_till_telemetry)/float(time_between_telemetry)
        return samples_per_second

#ncDjiCsvSync = NcDjiCsvSync()
#ncDjiCsvSync.synchronise_brain_dji('C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc.csv', 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/drone/flights', 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc_synced.csv', 8)

#ncDjiCsvSync = NcDjiCsvSync()
#samples_per_second = ncDjiCsvSync.get_sample_rate('C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/one_csv_corrected3/070618_out_adc.csv')
#print('samples_per_second: ' + str(samples_per_second))


#dji_csv_file_path = 'Z:/2019_Neurocopter/16072019_ableiten/drone_data/CSVs'
#drone_log_analyser = DroneLogAnalyser(dji_csv_file_path)
#drone_telemetry_lines = (drone_log_analyser.get_drone_telemetry_lines([2019, 7, 16], [20, 53, 16], 1))
#print('drone_telemetry_lines:', drone_telemetry_lines)
