import os 
import sys 
import json 
import glob 
import subprocess
import hashlib

s3_glacier_vault_name = "LTR-S3-GLACIER-VAULT"

file_list = glob.glob(os.path.join(sys.argv[1], "**"))

def getChunkSHA256Hashes(fileName):
    f = open(fileName, 'rb')
    raw = f.read(os.path.getsize(fileName))
    f.close()
    return hashlib.sha256(raw).digest()

def computeSHA256TreeHash():
    rawBytes = []

    for file in file_list:
        rawBytes.append(getChunkSHA256Hashes(file))
    output = rawBytes[:]

    while len(output) > 1:
        outputLength = len(output)
        newOutputLength = int(outputLength / 2)
        if outputLength %2 == 1:
            newOutputLength = int(newOutputLength + 1)
        newOutput = [''] * newOutputLength
        j = 0
        for i in range(0, newOutputLength*2, 2):
            if outputLength - i > 1:
                sha256obj = hashlib.sha256()
                sha256obj.update(output[i])
                sha256obj.update(output[i+1])

                newOutput[j] = sha256obj.digest()
            else:
                newOutput[j] = output[i]
            j = j + 1
        output = newOutput[:]
    return output[0]

print("-Start upload data  into AWS S3 Glacier Vault ({0})".format(s3_glacier_vault_name))

command = "aws glacier initiate-multipart-upload --account-id - --part-size {0} --vault-name {1}".format(os.path.getsize(file_list[0]), s3_glacier_vault_name)
result = subprocess.run(command, shell=True, stdout=subprocess.PIPE)
if 0 != result.returncode:
    print("[ERROR]: Failed to run: " + command)
    os.sys.exit(-1)

json_data = json.loads(result.stdout.decode('ascii'))
uploadId = json_data["uploadId"]
print("Multi-part upload id: {0}".format(uploadId))

start_bytes = end_bytes = 0
for item in file_list:

    start_bytes = end_bytes
    end_bytes += os.path.getsize(item)
    print("Uploading item: {0}".format(item))
    command = "aws glacier upload-multipart-part --upload-id {0} --body {1} --range 'bytes {2}-{3}/*' --account-id - --vault-name {4}".format(uploadId, item, start_bytes, end_bytes-1, s3_glacier_vault_name)
    result = subprocess.run(command, shell=True)
    if 0 != result.returncode:
        print("[ERROR]: Failed to run: " + command)
        os.sys.exit(-1)

tree_hash = computeSHA256TreeHash().hex()
command = "aws glacier complete-multipart-upload --archive-size {0} --upload-id {1} --account-id - --checksum {2} --vault-name {3}".format(end_bytes, uploadId, tree_hash, s3_glacier_vault_name)
result = subprocess.run(command, shell=True)
if 0 != result.returncode:
    print("[ERROR]: Failed to run: " + command)
    os.sys.exit(-1)

print("Upload finished with hash: {0}".format(tree_hash))
