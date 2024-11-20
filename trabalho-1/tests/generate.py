import os
import subprocess

DIRECTORY = 'tests/server'

def generate_file(file_name, size_mb):
    size_bytes = size_mb * 1024 * 1024

    # Creates a file of exactly N megabytes
    with open(file_name, "wb") as f:
        chunk_size = 1024 * 1024
        full_chunks = size_bytes // chunk_size 
        remaining_bytes = size_bytes % chunk_size 

        for _ in range(full_chunks):
            f.write(os.urandom(chunk_size))
        
        if remaining_bytes > 0:
            f.write(os.urandom(remaining_bytes))

    print(f"Data file '{file_name}' of size {size_mb} MB created")


def generate_hash(file_name, output_file):
    try:
        result = subprocess.run(f"md5sum {file_name}", shell=True, capture_output=True, text=True)
        with open(output_file, "w") as hash_file:
            hash_value = result.stdout.split()[0] # Extract only the hash value from output
            base_name = file_name.split("/")[-1] # Extract only the file name
            hash_file.write(f"{hash_value} {base_name}\n")
        print(f"Hash file for '{file_name}' created")
    except:
        print(f"Failed to create hash file for '{file_name}'")


for i in range(10):
    file_name = str(i + 1).zfill(2)
    binary = f"{DIRECTORY}/{file_name}mb.bin"
    hash = f"{DIRECTORY}/{file_name}mb.md5"

    generate_file(binary, i + 1) 
    generate_hash(binary, hash)

print(f"Files generated in {DIRECTORY}")
