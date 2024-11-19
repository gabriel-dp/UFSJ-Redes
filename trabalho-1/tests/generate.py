import os
import subprocess

def generate_file(file_name, size_mb):
    size_bytes = size_mb * 1024 * 1024

    with open(file_name, "wb") as f:
        chunk_size = 1024 * 1024
        full_chunks = size_bytes // chunk_size 
        remaining_bytes = size_bytes % chunk_size 

        for _ in range(full_chunks):
            f.write(os.urandom(chunk_size))
        
        if remaining_bytes > 0:
            f.write(os.urandom(remaining_bytes))

    print(f"File '{file_name}' of size {str(size_mb).zfill(2)} MB created.")

def generate_hash(file_name, output_file):
    try:
        result = subprocess.run(f"md5sum {file_name}", shell=True, capture_output=True, text=True)
        with open(output_file, "w") as out_f:
            hash_value = result.stdout.split()[0]
            base_name = file_name.split("/")[-1]
            out_f.write(f"{hash_value} {base_name}\n")
    except:
        pass


for i in range(1, 11):
    file_name = str(i).zfill(2)
    binary = f"server/{file_name}mb.bin"
    hash = f"server/{file_name}mb.md5"

    generate_file(binary, i) 
    generate_hash(binary, hash)
