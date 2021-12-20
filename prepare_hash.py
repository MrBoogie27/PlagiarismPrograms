#!/bin/python3
import subprocess
import argparse


def run_hasher(args):
    print(args.file_name)
    result = subprocess.run([args.binary_name, args.file_name],
                            capture_output=True,
                            check=True,
                            encoding='utf-8')
    subtree_hashes = [int(subtree_hash) for subtree_hash in result.stdout.split('\n')[-2].split()]
    subtree_hashes.sort()
    print(subtree_hashes)
    return subtree_hashes


def compare_subtrees(fst_subtree_hashes, snd_subtree_hashes):
    i, j = 0, 0
    answer = 0
    while i < len(fst_subtree_hashes) and j < len(snd_subtree_hashes):
        if fst_subtree_hashes[i] == snd_subtree_hashes[j]:
            answer += 1
            i += 1
            j += 1
        elif fst_subtree_hashes[i] > snd_subtree_hashes[j]:
            j += 1
        else:
            i += 1

    return answer / max(len(fst_subtree_hashes), len(snd_subtree_hashes))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Plagiarism tool")
    subparsers = parser.add_subparsers(title='Plagiarism modes',
                                       description='hasher and writer modes',
                                       help='hasher, compared')

    parser_hasher = subparsers.add_parser("hasher", help='get hashes subtrees')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser_hasher.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser.set_defaults(func=run_hasher)

    args = parser.parse_args()
    fst = args.func(args)
    args.file_name = './Tests/example_duplicate.cpp'
    snd = args.func(args)
    print(compare_subtrees(fst, snd))
