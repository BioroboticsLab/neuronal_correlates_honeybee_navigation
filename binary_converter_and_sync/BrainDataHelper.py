class BrainDataHelper:

    csv_brain_data_file = ''
    fr_brain_data_csv = object
    last_telemetry_time_in_s = 0

    def __init__(self, csv_brain_data_file):
        self.csv_brain_data_file = csv_brain_data_file
        # Go to sync point
        self.fr_brain_data_csv = open(csv_brain_data_file, "r")
        while True:  # Go to sync point
            brain_data_line = self.fr_brain_data_csv.readline()
            if len(brain_data_line) > 30:
                time = self.calc_time(brain_data_line)
                self.last_telemetry_time_in_s = time[0]*3600 + time[1]*60 + time[2]  # Save the time in s
                break

    # Returns the next line containing telemetry and number of lines between
    def get_next_telemetry(self):
        lines_between_sync = 0
        brain_data_line = self.fr_brain_data_csv.readline()
        while brain_data_line:  # Go to sync point
            brain_data_line = self.fr_brain_data_csv.readline()
            if len(brain_data_line) > 30:
                time = self.calc_time(brain_data_line)
                date = self.calc_date(brain_data_line)
                telemetry_duration = self.__get_telemetry_duration(brain_data_line)
                return [date, time, lines_between_sync, telemetry_duration]
            lines_between_sync += 1
        return [[],[],[],[]]  # At EOF

    # Gets aq new telemetry line and determines the duration between this telemetry line and the old one
    def __get_telemetry_duration(self, brain_data_line):
        time = self.calc_time(brain_data_line)
        time_in_s = time[0]*3600 + time[1]*60 + time[2]
        duration = time_in_s - self.last_telemetry_time_in_s
        self.last_telemetry_time_in_s = time_in_s
        return duration

    @staticmethod
    def calc_date(brain_data_line):
        date = brain_data_line.split(',')[4].split('.')
        date = date[::-1]
        date = [int(x) for x in date]
        return date

    @staticmethod
    def calc_time(brain_data_line):
        time = brain_data_line.split(',')[3].split(':')
        time = [int(x) for x in time]
        return time
