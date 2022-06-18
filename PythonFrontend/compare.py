import re
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
                            check=False,
                            encoding='utf-8')
    answer = float(result.stdout.split()[1])
    return answer


def get_answer_siminian(stdout):
    pattern = re.compile(r'Found (\d+) duplicate lines in \d+ blocks in 2 files')
    for line in stdout.split('\n'):
        answer = pattern.findall(line)
        if answer:
            return float(answer[0])
    return 0


def run_simian(jar_path, first_file, second_file):
    # java -jar bin/simian-2.5.10.jar
    # -threshold=1 -language=cpp
    # ../PlagiarismPrograms/Tests/RealTest1.cpp ../PlagiarismPrograms/Tests/RealTest2.cpp
    result = subprocess.run(['java',
                             '-jar', jar_path,
                             '-threshold=2', '-language=cpp',
                             first_file, second_file],
                            capture_output=True,
                            check=True,
                            encoding='utf-8')
    first_count_rows = sum(1 for line in open(first_file))
    second_count_rows = sum(1 for line in open(second_file))
    common_rows = get_answer_siminian(result.stdout)
    answer = common_rows / max(first_count_rows, second_count_rows)
    return answer
