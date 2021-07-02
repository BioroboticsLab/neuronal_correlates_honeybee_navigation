class tableHtml:

    titles = []
    lines = []

    def __init__(self, titles):
        global edit_consol_output
        self.titles = titles

    def add_line(self, line):
        self.lines.append(line)

    def get_html_table(self):
        html_table = "<table border=\"2\" width=\"70\%\"><tr>"
        for title in self.titles:
            html_table = html_table + "<td><b>" + title + "</b></td>"
        html_table = html_table + "</tr>"
        for line in self.lines:
            html_table = html_table + "<tr>"
            for elem in line:
                html_table = html_table + "<td>" + elem + "</td>"
            html_table = html_table + "</tr>"
        html_table = html_table + "</table>"
        return html_table

