#include "glacier_upload_script.h"

std::string get_glacier_upload_script();

std::string get_glacier_upload_script(const std::vector<std::string>& list, size_t chunk_size) {

	std::string file_list;
	for (const auto& item : list)
		file_list += "\"" + item + "\",";

	const auto key = std::make_tuple(file_list, chunk_size);
	static std::map<decltype(key), std::string> cache;
	
	if (cache[key].empty()) {
		cache[key] =
			"file_list = [" + file_list + "]\n" +
			"s3_glacier_chunk_size = " + std::to_string(chunk_size) + "\n" +
			get_glacier_upload_script();
	}

	return cache[key];
}
std::string get_glacier_upload_script() {
		return R"(
import os
import sys
import json
import glob
import subprocess
import hashlib

s3_glacier_vault_name = "LTR-S3-GLACIER-VAULT"

def getChunkSHA256Hashes(fileName) :
	f = open(fileName, 'rb')
	raw = f.read(s3_glacier_chunk_size)
	f.close()
	return hashlib.sha256(raw).digest()

def computeSHA256TreeHash() :
	rawBytes = []
	rawBytes.append(getChunkSHA256Hashes(file_list[0]))
	rawBytes.append(getChunkSHA256Hashes(file_list[1]))
	rawBytes.append(getChunkSHA256Hashes(file_list[2]))
	output = rawBytes[:]

	while len(output) > 1:
		outputLength = len(output)
		newOutputLength = int(outputLength / 2)
		if outputLength % 2 == 1 :
			newOutputLength = int(newOutputLength + 1)
		newOutput = [''] * newOutputLength
		j = 0
		for i in range(0, newOutputLength * 2, 2) :
			if outputLength - i > 1:
				sha256obj = hashlib.sha256()
				sha256obj.update(output[i])
				sha256obj.update(output[i + 1])

				newOutput[j] = sha256obj.digest()
			else:
				newOutput[j] = output[i]
			j = j + 1
		output = newOutput[:]
	return output[0]


print("-Start upload data  into AWS S3 Glacier Vault {0}".format(s3_glacier_vault_name))

command = "aws glacier initiate-multipart-upload --account-id - --part-size {0} --vault-name {1}".format(s3_glacier_chunk_size, s3_glacier_vault_name)
result = subprocess.run(command, shell = True, stdout = subprocess.PIPE)
if 0 != result.returncode:
	print("[ERROR]: Failed to run: " + command)
	os.sys.exit(-1)

print(str(result.stdout))
json_data = json.loads(result.stdout.decode('ascii'))
uploadId = json_data["uploadId"]
print("Multi-part upload id: {0}".format(uploadId))

start_bytes = end_bytes = 0
for item in file_list :

	start_bytes = end_bytes
	end_bytes += os.path.getsize(item)
	command = "aws glacier upload-multipart-part --upload-id {0} --body {1} --range 'bytes {2}-{3}/*' --account-id - --vault-name {4}".format(uploadId, item, start_bytes, end_bytes - 1, s3_glacier_vault_name)
	result = subprocess.run(command, shell = True)
	print("uploaded item " + item)
	if 0 != result.returncode:
		print("[ERROR]: Failed to run: " + command)
		os.sys.exit(-1)

tree_hash = computeSHA256TreeHash().hex()
print(tree_hash)
command = "aws glacier complete-multipart-upload --archive-size {0} --upload-id {1} --account-id - --checksum {2} --vault-name {3}".format(end_bytes, uploadId, tree_hash, s3_glacier_vault_name)
result = subprocess.run(command, shell = True)
if 0 != result.returncode:
	print("[ERROR]: Failed to run: " + command)
	os.sys.exit(-1)
)";
}
