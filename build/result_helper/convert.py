# Read the contents of ladder.txt
with open('ladder_report.txt', 'r') as file:
    lines = file.readlines()

# Convert nanoseconds to milliseconds
converted_data = []
for line in lines:
    # Split the line into parts
    parts = line.strip().split(': ')
    # Get the value in nanoseconds and convert to milliseconds
    ns_value = int(parts[1].replace(' ns', ''))
    ms_value = ns_value / 1_000_000  # Convert to milliseconds
    # Append the converted value to the list, rounded to 2 decimal places
    converted_data.append(f"{parts[0]}: {ms_value:.2f} ms")

# Print the converted data
for entry in converted_data:
    print(entry)
