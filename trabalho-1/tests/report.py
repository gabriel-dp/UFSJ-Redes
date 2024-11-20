import sys
import pandas as pd
import re
import seaborn as sns
import matplotlib.pyplot as plt

# Read the report file
with open(sys.argv[1], 'r') as file:
    report = file.read()

# Define regex patterns for extracting information
command_pattern = r"Command:\s+.*-f\s+(\S+)" 
time_pattern = r"Elapsed time \(seconds\)\s+=\s+([\d.]+)"
size_pattern = r"Size \(bytes\)\s+=\s+(\d+)"
speed_pattern = r"Speed \(bytes/second\)\s+=\s+([\d.]+)"
package_loss_pattern = r"Package Loss\s+=\s+([\d.]+)%\s+\((\d+)/(\d+)\)"

# Extract data using regex
commands = re.findall(command_pattern, report)
times = re.findall(time_pattern, report)
sizes = re.findall(size_pattern, report)
speeds = re.findall(speed_pattern, report)
package_losses = re.findall(package_loss_pattern, report)
if len(package_losses) < len(commands):
    # Fill missing values with zeros or placeholders
    missing_count = len(commands) - len(package_losses)
    package_losses += [(0.0, 0, 0)] * missing_count

# Parse Package Loss data
loss_percentages = [float(pl[0]) for pl in package_losses]
received_counts = [int(pl[1]) for pl in package_losses]
total_counts = [int(pl[2]) for pl in package_losses]

# Create a DataFrame
data = {
    "File": commands,
    "Elapsed Time (s)": list(map(float, times)),
    "Size (bytes)": list(map(int, sizes)),
    "Speed (bytes/s)": list(map(float, speeds)),
    "Package Loss (%)": loss_percentages,
    "Received Packets": received_counts,
    "Total Packets": total_counts
}
df = pd.DataFrame(data)
filtered_df = df[df["Size (bytes)"] >= 1048576]
# .groupby("File").agg(
#     {
#         "Elapsed Time (s)": "mean",
#         "Size (bytes)": "mean",
#         "Speed (bytes/s)": "mean",
#         "Package Loss (%)": "mean",
#         "Received Packets": "sum",
#         "Total Packets": "sum"
#     }
# ).reset_index()

# Display the DataFrame
print(filtered_df)


# Create a bar plot for elapsed time per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="File", y="Elapsed Time (s)", data=filtered_df)
plt.title("Elapsed Time per File")
plt.xlabel("File")
plt.ylabel("Elapsed Time (seconds)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a bar plot for file size per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="File", y="Size (bytes)", data=filtered_df)
plt.title("File Size per File")
plt.xlabel("File")
plt.ylabel("File Size (bytes)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a bar plot for speed per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="File", y="Speed (bytes/s)", data=filtered_df)
plt.title("Transfer Speed per File")
plt.xlabel("File")
plt.ylabel("Speed (bytes/second)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a bar plot for package loss per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="File", y="Package Loss (%)", data=filtered_df)
plt.title("Package Loss per File")
plt.xlabel("File")
plt.ylabel("Package Loss (%)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()