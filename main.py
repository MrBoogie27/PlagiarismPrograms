#!/bin/python3
import argparse
from prepare_hash import run_hasher
from compare import compare_AST
from writer import writer_hasher, writer_similarity


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Plagiarism tool")
    parser.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    subparsers = parser.add_subparsers(title='Plagiarism modes',
                                       description='hasher and writer modes',
                                       help='hasher, compared')

    parser_hasher = subparsers.add_parser("hasher", help='get hashes subtrees')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser.set_defaults(func=run_hasher)

    parser_hasher = subparsers.add_parser("compare 2 AST", help='compare 2 AST')
    parser_hasher.add_argument("-f", "--first-file", required=True)
    parser_hasher.add_argument("-f", "--second-file", required=True)
    parser.set_defaults(func=compare_AST)

    parser_hasher = subparsers.add_parser("writer hashes", help='write hashes to db')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser.set_defaults(func=writer_hasher)

    parser_hasher = subparsers.add_parser("writer similarity", help='write similarity files to db')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser.set_defaults(func=writer_similarity)

    args = parser.parse_args()
    args.func(args)
