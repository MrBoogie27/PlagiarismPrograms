import subprocess


def run_hasher(args):
    result = run_binary_hasher(args.binary_name, args.file_name)
    print(result)


def run_binary_hasher(binary_name, file_name):
    result = subprocess.run([binary_name, file_name],
                            capture_output=True,
                            check=True,
                            encoding='utf-8')
    subtree_hashes = [int(subtree_hash) for subtree_hash in result.stdout.split('\n')[-2].split()]
    subtree_hashes.sort()
    return subtree_hashes
