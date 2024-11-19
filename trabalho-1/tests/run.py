import subprocess
from datetime import datetime

current_time = datetime.now()
formatted_time = current_time.strftime("%Y-%m-%d-%H-%M-%S")

ip = "127.0.0.1"
port = "8001"

executable = "./bin/udp_client"
flags = f"-i {ip} -p {port}"
input_files = []
for i in range(1, 11):
    file_name = f"{str(i).zfill(2)}mb"
    input_files.append(f"{file_name}.bin")
    input_files.append(f"{file_name}.md5")
output_file = f"results_{formatted_time}.txt"

with open(output_file, "w") as f:
    f.write("Execution Report\n\n")
    for input_file in input_files:
        cmd = f"{executable} {flags} -f {input_file}"
        print(f"Running: {cmd}")
        try:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
            f.write(f"Command: {cmd}\nResult:\n{result.stdout.strip()}\n\n")
        except Exception as e:
            f.write(f"Command: {cmd}\nError: {e}\n\n")

print(f"All tests completed. Results saved to {output_file}")