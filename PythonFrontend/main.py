#!/bin/python3
import argparse
from prepare_hash import run_hasher
from compare import compare_AST
from writer import writer_hasher, writer_similarity, writer_bear_trap


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Plagiarism tool")
    subparsers = parser.add_subparsers(title='Plagiarism modes',
                                       description='hasher and writer modes',
                                       help='hasher, compared')

    parser_hasher = subparsers.add_parser("hasher", help='get hashes subtrees')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser_hasher.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_hasher.set_defaults(func=run_hasher)

    parser_compare = subparsers.add_parser("compare2AST", help='compare 2 AST')
    parser_compare.add_argument("-f", "--first-file", required=True)
    parser_compare.add_argument("-s", "--second-file", required=True)
    parser_compare.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_compare.set_defaults(func=compare_AST)

    parser_writer_hasher = subparsers.add_parser("writer_hashes", help='write hashes to db')
    parser_writer_hasher.add_argument("-db", "--database", required=True)
    parser_writer_hasher.add_argument("-U", "--db_user", required=True)
    parser_writer_hasher.add_argument("--password", required=True)
    parser_writer_hasher.add_argument("--host", required=True)
    parser_writer_hasher.set_defaults(func=writer_hasher)

    parser_writer_similarity = subparsers.add_parser("writer_similarity", help='write similarity files to db')
    parser_writer_similarity.add_argument("-db", "--database", required=True)
    parser_writer_similarity.add_argument("-U", "--db_user", required=True)
    parser_writer_similarity.add_argument("--password", required=True)
    parser_writer_similarity.add_argument("--host", required=True)
    parser_writer_similarity.set_defaults(func=writer_similarity)

    parser_writer_bear_trap = subparsers.add_parser("writer_bear_trap",
                                                    help='write similarity programm to db by BearTrap program')
    parser_writer_bear_trap.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_writer_bear_trap.add_argument("-db", "--database", required=True)
    parser_writer_bear_trap.add_argument("-U", "--db-user", required=True)
    parser_writer_bear_trap.add_argument("--password", required=True)
    parser_writer_bear_trap.add_argument("--host", required=True)
    parser_writer_bear_trap.add_argument("--field", required=True)
    parser_writer_bear_trap.add_argument("--sql-get", required=True)
    parser_writer_bear_trap.add_argument("--sql-update", required=True)
    parser_writer_bear_trap.set_defaults(func=writer_bear_trap)

    args = parser.parse_args()
    args.func(args)
