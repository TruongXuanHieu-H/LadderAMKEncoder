import matplotlib.pyplot as plt
import os

# Function to read data from the file


def read_data(filename, step):
    x = []
    y = []
    with open(filename, 'r') as file:
        for line in file:
            # Split the line into two parts
            parts = line.split(': ')
            if len(parts) == 2:
                # Extract the number and time
                number = int(parts[0])
                timens = int(parts[1].replace(' ns', '').strip())
                timems = round(timens / 1000000, 2)
                if number % step == 0:
                    x.append(number)
                    y.append(timems)
    return x, y


# File name
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
filename1 = os.path.join(parent_dir, "report/reduced_report.txt")
filename2 = os.path.join(parent_dir, "report/seq_report.txt")
filename3 = os.path.join(parent_dir, "report/BDD_report.txt")
filename4 = os.path.join(parent_dir, "report/card_report.txt")
filename5 = os.path.join(parent_dir, "report/product_report.txt")
filename6 = os.path.join(parent_dir, "report/duplex_report.txt")
filename7 = os.path.join(parent_dir, "report/scl_report.txt")

# Read data
x1, y1 = read_data(filename1, 5)
x2, y2 = read_data(filename2, 5)
x3, y3 = read_data(filename3, 5)
x4, y4 = read_data(filename4, 5)
x5, y5 = read_data(filename5, 5)
x6, y6 = read_data(filename6, 5)
x7, y7 = read_data(filename7, 5)


# Plot the data
plt.plot(x1, y1, linestyle=':', color='blue', label="Pairwise", linewidth=2)
plt.plot(x2, y2, linestyle='--', color='orange', label="Seq", linewidth=2)
plt.plot(x3, y3, linestyle='-.', color='purple', label="BDD", linewidth=2)
plt.plot(x4, y4, linestyle=':', color='green', label="Card", linewidth=2)
plt.plot(x5, y5, linestyle='--', color='red', label="Product", linewidth=2)
plt.plot(x6, y6, linestyle='-.', color='magenta', label="Duplex", linewidth=2)
plt.plot(x7, y7, linestyle='-', color='black', label="SCL", linewidth=2)
plt.legend()
plt.xlabel('#Width')
plt.ylabel('#Time (ms)')
plt.grid(True)
plt.tight_layout()
plt.show()
