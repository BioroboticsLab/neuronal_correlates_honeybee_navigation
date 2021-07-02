import os  # For iterating through directories
import datetime

class DroneLogAnalyser:
    droneLogDir = ''
    buffer_filled = False # Is current_fr_csv buffered
    current_fr_csv = object # Last used filereader (so the current line is saved for buffering)

    def __init__(self, droneLogDir):
        self.drone_log_dir = droneLogDir

    # Returns the dji log file containing the the given time
    # date: [yyyy,mm,dd] # time: [hh,mm,ss]
    def find_file(self, date, time):
        # While dateTime > :read the first line of every csv file in the folder
        #old_date_from_name = 0 # Store to see if a new file has new date
        log_with_datetime = ''
        for subdir, dirs, files in os.walk(self.drone_log_dir):
            i = 0
            for file in files:
                path_for_single_file = os.path.join(subdir, file)
                #print('current file: ' + str(path_for_single_file))
                fr_csv = open(path_for_single_file, "r")
                fr_csv.readline() # Skip first line because it contains column names
                #line = fr_csv.readline()
                #first_line = fr_csv.readline().split(',')
                first_line = self.read_and_get_first_gps(fr_csv)
                if first_line == '':
                    i = i
                    #print('File does not contain GPS datetime: ' + str(path_for_single_file))
                else:
                    fr_csv.close()
                    dji_gps_date_time_stamp = first_line.split(',')[47]
                    #print('dji_gps_date_time_stamp: ' + str(dji_gps_date_time_stamp))
                    if self.is_earlier(dji_gps_date_time_stamp, time, date):
                        log_with_datetime = path_for_single_file
                        #print('dji_gps_date_time_stamp: ' + str(dji_gps_date_time_stamp))
                i += 1
            break  # Without the break it would also scan the subdirs
        return log_with_datetime

    # Checks if a dji_gps_date_time_stamp is smaller than date and time
    def is_earlier(self, dji_gps_date_time_stamp, time, date):
        dji_gps_date = dji_gps_date_time_stamp.split('T')[0]
        dji_gps_time = dji_gps_date_time_stamp.split('T')[1][:-1]
        dji_gps_date = dji_gps_date.split('-')
        dji_gps_time = dji_gps_time.split(':')
        #print('dji_gps_date: ' + str(dji_gps_date))
        #print('dji_gps_time: ' + str(dji_gps_time))
        #print('dji_gps_date[0]' + str(dji_gps_date[0]))
        #print('dji_gps_date[1]' + str(dji_gps_date[1]))
        #print('dji_gps_date[2]' + str(dji_gps_date[2]))
        # datetime.date(2011, 1, 1) < datetime.date(2011, 1, 2)
        if datetime.date(int(dji_gps_date[0]), int(dji_gps_date[1]), int(dji_gps_date[2])) > datetime.date(date[0], date[1], date[2]):
            return False
        if datetime.time(int(dji_gps_time[0]), int(dji_gps_time[1]), int(dji_gps_time[2])) > datetime.time(time[0], time[1],time[2]):
            return False
        return True

    # Returns the first line containing gps data
    # If there is no gps data it returns a ''
    def read_and_get_first_gps(self, fr_csv):
        line = fr_csv.readline()
        while 'T' not in line.split(',')[47]:
            line = fr_csv.readline()
            #print('line.split(',')[47]: ' + str(line.split(',')[47]))
            if not line:
                #print('BREAK')
                return ''
        #print('read_and_get_first_gps: line: ' + str(line))
        return line

    # LEGACY - Returns all the telemetry lines of that second (gps time)
    def get_drone_telemetry_lines_of_1s(self, date, time, buffered=True):
        print('date: ' + str(date) + '   time: ' + str(time))
        #try:
        drone_telemetry_lines_of_1s = []
        if buffered and self.buffer_filled:
            fr_csv = self.current_fr_csv
        else:
            #print('NOT BUFFERED')
            current_file = self.find_file(date, time)
            fr_csv = open(current_file, "r")
            self.read_and_get_first_gps(fr_csv)
        while True:
            current_line = fr_csv.readline()
            is_line_in_time = False
            dji_gps_date_time_stamp = current_line.split(',')[47]
            if dji_gps_date_time_stamp == '':
                i = 1
            else:
                #print('dji_gps_date_time_stamp current_line: ' + str(current_line))
                dji_gps_date = dji_gps_date_time_stamp.split('T')[0]
                dji_gps_time = dji_gps_date_time_stamp.split('T')[1][:-1]
                dji_gps_date = dji_gps_date.split('-')
                dji_gps_time = dji_gps_time.split(':')
                if datetime.time(int(dji_gps_time[0]), int(dji_gps_time[1]), int(dji_gps_time[2])) == datetime.time(time[0], time[1],time[2]):
                    drone_telemetry_lines_of_1s.append(current_line)
                    is_line_in_time = True
                    #print('drone_telemetry_lines_of_1s.append(current_line)')
                if len(drone_telemetry_lines_of_1s) > 0 and is_line_in_time == False:
                    break
        if len(drone_telemetry_lines_of_1s) == 0 and buffered:
            return self.get_drone_telemetry_lines_of_1s(date, time, False)
        self.current_fr_csv = fr_csv
        self.buffer_filled = True
        return drone_telemetry_lines_of_1s

    # Returns all the telemetry lines of a given timeframe (gps time)
    # duration: duration + time = endtime in seconds
    def get_drone_telemetry_lines(self, date, time, duration, buffered=True):
        print('date: ' + str(date) + '   time: ' + str(time))
        #try:
        drone_telemetry_lines = []
        if buffered and self.buffer_filled:
            fr_csv = self.current_fr_csv
        else:
            #print('NOT BUFFERED')
            current_file = self.find_file(date, time)
            fr_csv = open(current_file, "r")
            self.read_and_get_first_gps(fr_csv)
        while True:
            current_line = fr_csv.readline()
            if not current_line:
                break
            is_line_in_time = False
            dji_gps_date_time_stamp = current_line.split(',')[47]
            if dji_gps_date_time_stamp=='':
                i=1
            else:
                #print('dji_gps_date_time_stamp current_line: ' + str(current_line))
                dji_gps_date = dji_gps_date_time_stamp.split('T')[0]
                dji_gps_time = dji_gps_date_time_stamp.split('T')[1][:-1]
                dji_gps_date = dji_gps_date.split('-')
                dji_gps_time = dji_gps_time.split(':')
                dji_gps_time_object = datetime.time(int(dji_gps_time[0]), int(dji_gps_time[1]), int(dji_gps_time[2]))
                time_object = datetime.time(time[0], time[1],time[2])
                dji_gps_time_seconds = int(dji_gps_time[0])*3600 + int(dji_gps_time[1])*60 + int(dji_gps_time[2])
                time_seconds = int(time[0])*3600 + int(time[1])*60 + int(time[2])
                if (dji_gps_time_seconds >= time_seconds) and (dji_gps_time_seconds <= time_seconds+duration):
                    drone_telemetry_lines.append(current_line) #.split(',')[47]
                    is_line_in_time = True
                    #print('drone_telemetry_lines_of_1s.append(current_line)')
                if len(drone_telemetry_lines) > 0 and is_line_in_time == False:
                    break
        if len(drone_telemetry_lines) == 0 and buffered:
            return self.get_drone_telemetry_lines(date, time, duration, False)
        self.current_fr_csv = fr_csv
        self.buffer_filled = True
        return drone_telemetry_lines


#dji_csv_file_path = 'Z:/2018_neuroCopter/Experiments/ableiten_inga_07062018/feld_1256-1311-15min/drone/flights'
#date_to_find = [2018, 6, 7]
#time_to_find = [18, 53, 50]
#time_to_find_2 = [19, 2, 30]
#time_to_find_3 = [18, 55, 59]
#dji_csv_file_path = 'Z:/2019_Neurocopter/16072019_ableiten/drone_data/CSVs'
#date_to_find = [2019, 7, 16]
#time_to_find = [20, 53, 17]
#drone_log_analyser = DroneLogAnalyser(dji_csv_file_path)
#print('drone_log_analyser.find_file:', drone_log_analyser.find_file(date_to_find, time_to_find))
#print(drone_log_analyser.get_drone_telemetry_lines_of_1s(date_to_find, time_to_find))
#print(len(drone_log_analyser.get_drone_telemetry_lines(date_to_find, time_to_find_2, 12)))
#print(drone_log_analyser.get_drone_telemetry_lines_of_1s(date_to_find, time_to_find_3))
