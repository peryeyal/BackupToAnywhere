import subprocess

s3_glacier_vault_name = "LTR-S3-GLACIER-VAULT"

print("Get AWS Glacier vault list from AWS:")
command = "aws glacier list-vaults  --account-id -"
result = subprocess.run(command, shell=True)
if 0 != result.returncode:
    print("[ERROR]: Failed to run: " + command)
    os.sys.exit(-1)

print("Get upload list for '{0}' AWS Glacier vault".format(s3_glacier_vault_name))
command = "aws glacier list-multipart-uploads --account-id - --vault-name {0}".format(s3_glacier_vault_name)
result = subprocess.run(command, shell=True)
if 0 != result.returncode:
    print("[ERROR]: Failed to run: " + command)
    os.sys.exit(-1)

print("Get list of active uploads for '{0}' AWS Glacier vault".format(s3_glacier_vault_name))
command = "aws glacier list-parts --account-id - --vault-name {0} --upload-id 8w5soAC3kkD4T0lM7zVIiImt8w-YpKwDy_vEgDurKdFpQB1tLJY6YWEdcKleDkouMR4kHiPr6555i5DPjfqjMykUmWq6".format(s3_glacier_vault_name)
result = subprocess.run(command, shell=True)
if 0 != result.returncode:
    print("[ERROR]: Failed to run: " + command)
    os.sys.exit(-1)

