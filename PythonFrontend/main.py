#!/bin/python3
import argparse
from prepare_hash import run_hasher
from compare import compare_AST
from writer import writer_hasher, writer_similarity, writer_bear_trap
from stats import update_manual


if __name__ == '__main__':
    db_parser = argparse.ArgumentParser(description="Database options", add_help=False)
    db_parser.add_argument("-db", "--database", required=True)
    db_parser.add_argument("-U", "--db-user", required=True)
    db_parser.add_argument("--password", required=True)
    db_parser.add_argument("--host", required=True)

    parser = argparse.ArgumentParser(description="Plagiarism tool")
    subparsers = parser.add_subparsers(title='Plagiarism modes',
                                       description='hasher, write, stats and compare modes',
                                       help='hasher, compared, write similarity files to db, Collect statistics for db')

    parser_hasher = subparsers.add_parser("hasher", help='get hashes subtrees')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser_hasher.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_hasher.set_defaults(func=run_hasher)

    parser_compare = subparsers.add_parser("compare2AST", help='compare 2 AST')
    parser_compare.add_argument("-f", "--first-file", required=True)
    parser_compare.add_argument("-s", "--second-file", required=True)
    parser_compare.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_compare.set_defaults(func=compare_AST)

    parser_writer_hasher = subparsers.add_parser("writer_hashes",
                                                 help='write hashes to db',
                                                 parents=[db_parser])
    parser_writer_hasher.set_defaults(func=writer_hasher)

    parser_writer_similarity = subparsers.add_parser("writer_similarity",
                                                     help='write similarity files to db',
                                                     parents=[db_parser])
    parser_writer_similarity.set_defaults(func=writer_similarity)

    parser_writer_bear_trap = subparsers.add_parser("writer_bear_trap",
                                                    help='write similarity programm to db by BearTrap program',
                                                    parents=[db_parser])
    parser_writer_bear_trap.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_writer_bear_trap.add_argument("--field", required=True)
    parser_writer_bear_trap.add_argument("--sql-get", required=True)
    parser_writer_bear_trap.add_argument("--sql-update", required=True)
    parser_writer_bear_trap.set_defaults(func=writer_bear_trap)

    parser_update_manual = subparsers.add_parser("manual_similarity",
                                                 help='UI for manual get similarity',
                                                 parents=[db_parser])
    parser_update_manual.add_argument("--sql-get", required=True)
    parser_update_manual.set_defaults(func=update_manual)

    args = parser.parse_args()
    args.func(args)
