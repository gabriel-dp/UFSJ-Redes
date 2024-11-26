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
    "Arquivo": commands,
    "Tempo decorrido (s)": list(map(float, times)),
    "Tamanho (bytes)": list(map(int, sizes)),
    "Velocidade (bytes/s)": list(map(float, speeds)),
    "Perda de pacotes (%)": loss_percentages,
    "Pacotes recebidos": received_counts,
    "Pacotes totais": total_counts
}
df = pd.DataFrame(data)
filtered_df = df[df["Tamanho (bytes)"] >= 1048576] # Remove hash files

# Display the DataFrame
print(filtered_df)


# Create a line plot for elapsed time per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="Arquivo", y="Tempo decorrido (s)", data=filtered_df, marker='o')
plt.title("Tempo médio decorrido por arquivo")
plt.xlabel("Arquivo")
plt.ylabel("Tempo decorrido (segundos)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a line plot for file size per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="Arquivo", y="Tamanho (bytes)", data=filtered_df, marker='o')
plt.title("Tamanho dos Arquivos")
max_bytes = filtered_df["Tamanho (bytes)"].max()
y_ticks = range(0, int(max_bytes // (1024 * 1024)) + 1)  # Ticks in MB
plt.yticks([y * 1024 * 1024 for y in y_ticks], [y for y in y_ticks])
plt.xlabel("Arquivo")
plt.ylabel("Tamanho do arquivo (MB)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a line plot for speed per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="Arquivo", y="Velocidade (bytes/s)", data=filtered_df, marker='o')
plt.title("Velocidade média de Transferência por Arquivo")
plt.xlabel("Arquivo")
plt.ylabel("Vecolidade (bytes/segundo)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

# Create a line plot for package loss per command
plt.figure(figsize=(10, 6))
sns.lineplot(x="Arquivo", y="Perda de pacotes (%)", data=filtered_df, marker='o')
plt.title("Perda média de pacotes por Arquivo")
plt.xlabel("Arquivo")
plt.ylabel("Perda de pacotes (%)")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()