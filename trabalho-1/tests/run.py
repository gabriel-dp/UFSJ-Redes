import sys
import subprocess
from datetime import datetime

# Constants
EXECUTABLE = sys.argv[1]
IP = sys.argv[2]
PORT = sys.argv[3]
ITERATIONS = int(sys.argv[4])

# Define input files
input_files = []
for i in range(1, 11):
    file_name = f"{str(i).zfill(2)}mb"
    input_files.append(f"{file_name}.bin") # Data
    input_files.append(f"{file_name}.md5") # Hash

formatted_time = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
output_file = f"results_{formatted_time}.txt"

with open(output_file, "w") as f:
    f.write("Execution Report\n\n")
    
    for input_file in input_files:
        for i in range(ITERATIONS):
            cmd = f"{EXECUTABLE} -i {IP} -p {PORT} -f {input_file}"
            print(f"Running #{i + 1}: {cmd}")
            
            try:
                result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
                f.write(f"Command: {cmd}\nResult:\n{result.stdout.strip()}\n\n")
            except Exception as e:
                f.write(f"Command: {cmd}\nError: {e}\n\n")
            
            # Hash files run just one time
            if input_file.find("md5") != -1:
                break

print(f"All tests completed. Results saved to {output_file}")