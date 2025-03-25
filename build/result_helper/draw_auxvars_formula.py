import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

# Define n(x)


def n(x):
    return 1000

# Define N(x)


def N(x):
    return n(x) - x + 1

# Define the seq expression


def seq(x):
    return N(x) * (x - 2)

# Define the BDD expression


def bdd(x):
    return N(x) * 2 * (x - 1)

# Define the product expression

def card(x):
    return N(x) * x

# Define the product expression


def product(x):
    return N(x) * 2 * np.sqrt(x)

# Define the duplex expression


def duplex(x):
    return 4 * np.ceil(n(x) / x) * (x - 1)

# Define the SCL expression


def scl(x):
    return (2 * np.ceil(n(x) / x) - 2) * (x - 2)


# Generate x values
x_values = np.linspace(2, 999, 998)

# Calculate y values using the expressions
seq_values = seq(x_values)
bdd_values = bdd(x_values)
card_values = card(x_values)
product_values = product(x_values)
duplex_values = duplex(x_values)
scl_values = scl(x_values)

# Custom formatter for y-axis


def custom_formatter(y, pos):
    return f'{y * 1e-4:.0f}'  # Format the y value


# Create the plot
# plt.figure(figsize=(10, 10))
plt.plot(x_values, seq_values, linestyle='--',
         label="Seq", color='orange', linewidth=2)
plt.plot(x_values, bdd_values, linestyle='-.',
         label="BDD", color='purple', linewidth=2)
plt.plot(x_values, card_values, linestyle=':',
         label="Card", color='green', linewidth=2)
plt.plot(x_values, product_values, linestyle='--',
         label="Product", color='red', linewidth=2)
plt.plot(x_values, duplex_values, linestyle='-.',
         label="Duplex", color='magenta', linewidth=2)
plt.plot(x_values, scl_values, linestyle='-',
         label="SCL", color='black', linewidth=2)

# Setting labels and grid
plt.xlabel('#Width')
plt.ylabel('#AuxVars')
plt.axhline(0, color='black', linewidth=0.5, ls='--')
plt.axvline(0, color='black', linewidth=0.5, ls='--')
plt.grid()

# Set y-axis limits
plt.ylim(0, 55000)

# Apply the custom formatter to the y-axis
plt.gca().yaxis.set_major_formatter(FuncFormatter(custom_formatter))
plt.text(y=1, x=0, s='e+04', va='bottom',
         ha='center', transform=plt.gca().transAxes)
plt.tight_layout()

plt.legend()
plt.show()
