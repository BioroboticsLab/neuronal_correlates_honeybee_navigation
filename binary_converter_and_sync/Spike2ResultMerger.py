# Helps to merge the sorted events from spike2 into the file(containing, brain data and drone logs)


class Spike2ResultsMerger:
    # Merges a spike2 file and a csv (containing, brain data and drone logs)
    @staticmethod
    def merge(spike2_file, synced_csv_file, merged_synced_csv_file, frequency):
        fr_spikes_csv = open(spike2_file, "r")
        spike_line = fr_spikes_csv.readline()
        while True:  # Go to first line with event
            spike_line = fr_spikes_csv.readline()
            if len(spike_line.split(',')) > 4:
                break
        fw = open(merged_synced_csv_file, 'w')
        fr_synced_csv = open(synced_csv_file, "r")
        synced_line = fr_synced_csv.readline()
        i = 1
        while synced_line:
            event = '0'  # 0 means no spike. All other numbers are the spike classes
            try:
                next_spike_ind = int(frequency * float(spike_line.split(',')[0]))  # Line index of the next spike
            except:
                next_spike_ind = 0
            if i == next_spike_ind and spike_line:
                event = spike_line.split(',')[2]
                spike_line = fr_spikes_csv.readline()
            i += 1
            synced_line = fr_synced_csv.readline()
            fw.write(str(event) + ',' + synced_line)
        fw.close()

    @staticmethod
    def remove_brain_and_add_timestamps(synced_csv_file, synced_timestamp_csv_file, frequency):
        fr_synced_csv = open(synced_csv_file, "r")
        fw = open(synced_timestamp_csv_file, 'w')
        synced_line = fr_synced_csv.readline()
        i = 1
        while synced_line:
            timestamp = i/frequency
            i += 1
            synced_line = fr_synced_csv.readline()
            if(len(synced_line) > 20):
                fw.write(str("%.6f" % timestamp) + ',' + synced_line)
        fw.close()

    @staticmethod
    def add_timestamps_det_rates_per_s(spike2_file, spike2_rates_file):
        fr_synced_csv = open(spike2_file, "r")
        fw = open(spike2_rates_file, 'w')
        # Ignore first 5 lines
        for i in range(6):
            spike_line = fr_synced_csv.readline()
        time_s_prev = spike_line.split(',')[0].split('.')[0]
        max_event_type = Spike2ResultsMerger.get_number_of_units(spike2_file) + 1
        print('max_event_type: ' + str(max_event_type))
        event_cnts = [0]*(max_event_type)
        while spike_line:
            print('spike_line: ' + str(spike_line))
            event_type = int(spike_line.split(',')[2])
            event_cnts[event_type] += 1
            time_s_current = spike_line.split(',')[0].split('.')[0]
            if (time_s_current != time_s_prev):
                event_cnts_str = spike_line.split(',')[0] + ','
                for i in range(max_event_type):
                    print('i: ' + str(i))
                    event_cnts_str = event_cnts_str + str(event_cnts[i]) + ','
                print('event_cnts_str: ' + event_cnts_str)
                fw.write(event_cnts_str[:-1] + '\r')
                #fw.write(spike_line.split(',')[0] + ',' + str(event_cnts[1]) + ',' + str(event_cnts[3]) + ',' + str(event_cnts[4]) + ',' + str(event_cnts[5]) + '\r')
                time_s_prev = time_s_current
                event_cnts = [0]*(max_event_type)
            spike_line = fr_synced_csv.readline()
            if len(spike_line) < 5:
                break
        fw.close()

    # Creates a file containing time in seconds from recording start and spike rate with selected bining
    def create_spike_rates_file_flex_bining(self, spike2_file, spike2_rates_file, bin_steps_per_s):
        fr_spike2_file = open(spike2_file, "r")
        fw = open(spike2_rates_file, 'w')
        # Ignore first 5 lines
        for i in range(6):
            spike_line = fr_spike2_file.readline()
        max_event_type = Spike2ResultsMerger.get_number_of_units(spike2_file) + 1
        event_cnts = [0]*(max_event_type)
        time_s_current = float(spike_line.split(',')[0])
        current_bin_time = 0.0
        while spike_line:
            while current_bin_time + bin_steps_per_s > time_s_current and spike_line:
                #print('YAY')
                event_type = int(spike_line.split(',')[2])
                event_cnts[event_type] += 1
                spike_line = fr_spike2_file.readline()
                if len(spike_line) < 5:
                    break
                time_s_current = float(spike_line.split(',')[0])
            event_cnts_str = ""
            for i in range(max_event_type):
                event_cnts_str = event_cnts_str + str(event_cnts[i]) + ','
            fw.write(str(current_bin_time) + ',' + event_cnts_str[:-1] + '\r')
            event_cnts = [0]*(max_event_type)
            current_bin_time += bin_steps_per_s
            current_bin_time = round(current_bin_time, 4)
            #print('current_bin_time: ' + str(current_bin_time))
            if len(spike_line) < 5: # TODO remove behind or  or current_bin_time > 10000
                break
        fw.close()

    @staticmethod
    def get_number_of_units(spike2_file):
        fr = open(spike2_file, 'r')
        # Ignore first 5 lines
        for i in range(6):
            spike_line = fr.readline()
        max_event_type = 0 # Index of unit
        while spike_line:
            current_event_type = int(spike_line.split(',')[2])
            if current_event_type > max_event_type:
                max_event_type = current_event_type
            spike_line = fr.readline()
            if len(spike_line) < 5:
                break
        return max_event_type

    def sync_drone_and_spike_data_flex_intervalls(self, spike2_file, synced_csv_file, spikes_drone_intervalls_csv, frequency, bin_steps_per_s):
        output_spike_rates = spike2_file.split('.')[0] + '_' + str(bin_steps_per_s) +'_bin_step_width'
        self.create_spike_rates_file_flex_bining(spike2_file, output_spike_rates, bin_steps_per_s)
        fr_synced_csv = open(synced_csv_file, "r")
        fr_spike_rates = open(output_spike_rates, "r")
        # Add Spike artes to dictionary
        spike_rates_dic = {}
        spike_rates_line = fr_spike_rates.readline()
        while(spike_rates_line):
            timestamp = round(float(spike_rates_line.split(',')[0]), 4)
            spike_rates_list = spike_rates_line.split(',')[1:-1]
            spike_rates_list.append(spike_rates_line.split(',')[-1])
            #print('spike_rates_list: ' + str(spike_rates_list))
            spike_rates_list = [int(x) for x in spike_rates_list]
            spike_rates_dic[timestamp] = spike_rates_list
            spike_rates_line = fr_spike_rates.readline()
            print('spike_rates_dic: ' + str(timestamp) + str(spike_rates_dic[timestamp]))
        fw = open(spikes_drone_intervalls_csv, 'w')
        synced_line = fr_synced_csv.readline()
        i = 1
        prev_timestamp = 0.0
        while synced_line:
            timestamp = round(float(i/int(frequency)), 2)
            i += 1 #bin_steps_per_s*10
            synced_line = fr_synced_csv.readline()
            if len(synced_line) > 20 and timestamp != prev_timestamp:
                print('frequency:', frequency)
                print('synced_line (len > 20):', synced_line)
                print('timestamp: ' + str(timestamp))
                print('bin_steps_per_s: ' + str(bin_steps_per_s) + '    timestamp: ' + str(timestamp))
                if timestamp in spike_rates_dic:
                    print('timestamp found: ' + str(timestamp))
                    fw.write(str(timestamp) + ',' + str(spike_rates_dic[timestamp])[1:-1] + ',' + synced_line)
                prev_timestamp = timestamp
        fw.close()
        #print('spike_rates_dic: ' + str(spike_rates_dic))

    def sync_drone_and_spike_data_1s_intervalls(self, spike2_file, synced_csv_file, spikes_drone_1s_intervalls_csv, frequency):
        output_spike_rates = spike2_file.split('.')[0] + '_rates_per_sec.txt'
        self.add_timestamps_det_rates_per_s(spike2_file, output_spike_rates)
        fr_synced_csv = open(synced_csv_file, "r")
        fr_spike_rates = open(output_spike_rates, "r")
        # Add Spike artes to dictionary
        spike_rates_dic = {}
        spike_rates_line = fr_spike_rates.readline()
        while(spike_rates_line):
            timestamp = int(spike_rates_line.split('.')[0])
            spike_rates_list = spike_rates_line.split(',')[1:-1]
            spike_rates_list.append(spike_rates_line.split(',')[-1])
            #print('spike_rates_list: ' + str(spike_rates_list))
            spike_rates_list = [int(x) for x in spike_rates_list]
            spike_rates_dic[timestamp] = spike_rates_list
            spike_rates_line = fr_spike_rates.readline()
            print('spike_rates_dic: ' + str(spike_rates_dic[timestamp]))
        fw = open(spikes_drone_1s_intervalls_csv, 'w')
        synced_line = fr_synced_csv.readline()
        i = 1
        prev_timestamp = 0
        while synced_line:
            #print('synced_line: ' + synced_line)
            #if len(synced_line=='') < 5:
                #break
            timestamp = int(i/int(frequency))
            i += 1
            synced_line = fr_synced_csv.readline()
            if(len(synced_line) > 20 and timestamp != prev_timestamp):
                if timestamp in spike_rates_dic:
                    fw.write(str(timestamp) + ',' + str(spike_rates_dic[timestamp])[1:-1] + ',' + synced_line)
                prev_timestamp = timestamp
        fw.close()





spike2ResultsMerger = Spike2ResultsMerger()
#spike2_file_test = 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc.csv_17khz_noGap_spikes.txt'
#spike2_file_test = 'C:/Users/Julian/Desktop/2018_NeuroCopter/20180802/02/synced/020818_sorrted.txt'

#synced_csv_file_test = 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc_synced.csv'
synced_csv_file_test = 'C:/Users/Julian/Desktop/2018_NeuroCopter/20180802/02/synced/020818_out_adc_synced.csv'

#merged_synced_csv_file_test = 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc_synced_spikes_merged.csv'
#synced_timestamp_csv_file = 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc_synced_timestamps_merged.csv'
#spike_rates = 'C:/Users/Julian/Desktop/ableiten_inga_07062018/feld_1256-1311-15min/brain_data_battery0/synced_for_correlation_analysis/070618_out_adc.csv_17khz_noGap_spikes_spike_rates.txt'
spikes_drone_1s_intervalls_csv_test = 'C:/Users/Julian/Desktop/2018_NeuroCopter/20180802/02/synced/020818_out_adc_36khz_noGap_spikes_spike_rates_1s_intervalls.txt'

#spike2ResultsMerger.merge(spike2_file_test, synced_csv_file_test, merged_synced_csv_file_test, 17000)
#spike2ResultsMerger.remove_brain_and_add_timestamps(synced_csv_file_test, synced_timestamp_csv_file, 17000)
#spike2ResultsMerger.add_timestamps_det_rates_per_s(spike2_file_test, spike_rates)
#spike2ResultsMerger.sync_drone_and_spike_data_1s_intervalls(spike2_file_test, synced_csv_file_test, spikes_drone_1s_intervalls_csv_test, 36000)

#spike2_file = 'C:/Users/Julian/Desktop/ableiten_ben_29082018/2/29082018_2.txt'
#spike2_rates_file = 'C:/Users/Julian/Desktop/ableiten_ben_29082018/2/29082018_2_out_flex_bined.txt'
#synced_csv_file = 'C:/Users/Julian/Desktop/ableiten_ben_29082018/2/2_290818_out_adc_synced.csv'
#spikes_drone_intervalls_csv = 'C:/Users/Julian/Desktop/ableiten_ben_29082018/2/2_290818_out_adc_binned_01s.csv'
#spike2ResultsMerger.create_spike_rates_file_flex_bining(spike2_file, spike2_rates_file, bin_steps_per_s=0.1)

#spike2ResultsMerger.sync_drone_and_spike_data_flex_intervalls(spike2_file, synced_csv_file, spikes_drone_intervalls_csv, 36000, 0.1)
