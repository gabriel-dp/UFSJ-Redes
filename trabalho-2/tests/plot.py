import matplotlib.pyplot as plt
import pandas as pd
import sys

SERVIDORES = {
    1: "Iterativo",
    2: "Multi-thread",
    3: "Multi-thread com fila",
    4: "Concorrente"
}

# Load the uploaded CSV file to examine its content
file_path = sys.argv[1]
data = pd.read_csv(file_path, delimiter=';')

# Group data by execution and plot total requests for each number of users
plt.figure(figsize=(10, 6))

# Plotting each execution as a separate line
for exec_num in data['server'].unique():
    exec_data = data[data['server'] == exec_num]
    plt.plot(exec_data['users'], exec_data['total'] - exec_data['fails'], marker='o', label=f'Servidor {SERVIDORES[exec_num]}')

# Customizing the plot
plt.title('Total de Requisições com Sucesso vs Número de Usuários por Servidor', fontsize=14)
plt.xlabel('Número de Usuários', fontsize=12)
plt.ylabel('Total de Requisições', fontsize=12)
plt.legend()
plt.grid(True)
plt.tight_layout()

# Display the plot
plt.show()

# Plotting Average Response Time vs. Number of Users for each execution
plt.figure(figsize=(10, 6))

# Plotting each execution as a separate line
for exec_num in data['server'].unique():
    exec_data = data[data['server'] == exec_num]
    plt.plot(exec_data['users'], exec_data['avg_time'], marker='o', label=f'Servidor {SERVIDORES[exec_num]}')

# Customizing the plot
plt.title('Tempo Médio de Resposta vs Número de Usuários por Servidor', fontsize=14)
plt.xlabel('Número de Usuários', fontsize=12)
plt.ylabel('Tempo Médio de Resposta (ms)', fontsize=12)
plt.legend()
plt.grid(True)
plt.tight_layout()

# Display the plot
plt.show()

# Plotting Average Response Time vs. Number of Users for each execution
plt.figure(figsize=(10, 6))

# Plotting each execution as a separate line
for exec_num in data['server'].unique():
    exec_data = data[data['server'] == exec_num]
    plt.plot(exec_data['users'], exec_data['fails'], marker='o', label=f'Servidor {SERVIDORES[exec_num]}')

# Customizing the plot
plt.title('Quantidade de falhas vs Número de Usuários por Servidor', fontsize=14)
plt.xlabel('Número de Usuários', fontsize=12)
plt.ylabel('Quantidade de falhas', fontsize=12)
plt.legend()
plt.grid(True)
plt.tight_layout()

# Display the plot
plt.show()



