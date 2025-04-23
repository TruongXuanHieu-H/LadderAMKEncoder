import matplotlib.pyplot as plt
import os
import sys
import re

def parse_file(filename, w_target, k_target):
    x = []  # Mảng chứa giá trị n
    y = []  # Mảng chứa giá trị time
    
    pattern = re.compile(r"\{(\d+) - (\d+) - (\d+)\} : (\d+) ns")
    
    with open(filename, 'r') as file:
        for line in file:
            match = pattern.match(line.strip())
            if match:
                n, w, k, time = map(int, match.groups())
                if w == w_target and k == k_target:
                    x.append(n)
                    y.append(time / 1000000)
    return x, y


# File name
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
filename1 = os.path.join(parent_dir, "report/BDD_report.txt")
filename2 = os.path.join(parent_dir, "report/card_report.txt")
filename3 = os.path.join(parent_dir, "report/reduced_report.txt")
filename4 = os.path.join(parent_dir, "report/scl_report.txt")
filename5 = os.path.join(parent_dir, "report/seq_report.txt")

# Read data
w_target = int(sys.argv[1])
k_target = int(sys.argv[2])
x1, y1 = parse_file(filename1, w_target, k_target)
x2, y2 = parse_file(filename2, w_target, k_target)
x3, y3 = parse_file(filename3, w_target, k_target)
x4, y4 = parse_file(filename4, w_target, k_target)
x5, y5 = parse_file(filename5, w_target, k_target)


# Plot the data
if len(x1) > 0:
    plt.plot(x1, y1, linestyle='-.', color='purple', label="BDD", linewidth=2)
if len(x2) > 0:
    plt.plot(x2, y2, linestyle=':', color='green', label="Card", linewidth=2)
if len(x3) > 0:
    plt.plot(x3, y3, linestyle=':', color='blue', label="Pairwise", linewidth=2)
if len(x4) > 0:
    plt.plot(x4, y4, linestyle='-', color='black', label="SCL", linewidth=2)
if len(x5) > 0:
    plt.plot(x5, y5, linestyle='--', color='orange', label="Seq", linewidth=2)
plt.legend()
plt.xlabel('#Number of variables (n)')
plt.ylabel('#Time (ms)')
plt.grid(True)
plt.tight_layout()
plt.show()
