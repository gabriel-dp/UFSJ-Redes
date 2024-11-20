import subprocess
from datetime import datetime

ip = "127.0.0.1"
port = "8000"
executable = "./bin/udp_client"
iterations = 50

input_files = []
for i in range(1, 11):
    file_name = f"{str(i).zfill(2)}mb"
    input_files.append(f"{file_name}.bin")
    input_files.append(f"{file_name}.md5")

current_time = datetime.now()
formatted_time = current_time.strftime("%Y-%m-%d-%H-%M-%S")
output_file = f"results_{formatted_time}.txt"

with open(output_file, "w") as f:
    f.write("Execution Report\n\n")
    for input_file in input_files:
        for i in range(iterations):
            cmd = f"{executable} -i {ip} -p {port} -f {input_file}"
            print(f"Running #{i + 1}: {cmd}")
            try:
                result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
                f.write(f"Command: {cmd}\nResult:\n{result.stdout.strip()}\n\n")
            except Exception as e:
                f.write(f"Command: {cmd}\nError: {e}\n\n")
            
            if input_file.find("md5") != -1:
                break

print(f"All tests completed. Results saved to {output_file}")