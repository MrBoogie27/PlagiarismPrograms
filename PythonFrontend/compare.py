import subprocess

from prepare_hash import run_binary_hasher
from common import compare_array


def compare_AST(args):
    result = run_compare(args.binary_name, args.first_file, args.second_file)
    print(result)


def run_compare(binary_name, first_file, second_file):
    result_first = run_binary_hasher(binary_name, first_file)
    result_second = run_binary_hasher(binary_name, second_file)
    return compare_array(result_first, result_second)


def run_bear_trap(binary_name, file_name, second_file):
    result = subprocess.run([binary_name, file_name, second_file],
                            capture_output=True,
                            check=True,
                            encoding='utf-8')
    answer = float(result.stdout.split()[1])
    return answer
