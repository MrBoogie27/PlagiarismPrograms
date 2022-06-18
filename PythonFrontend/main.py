#!/bin/python3
import argparse
from prepare_hash import run_hasher
from compare import compare_AST
from writer import writer_hasher, writer_similarity_v1, writer_similarity_v3, writer_bear_trap, writer_simian
from stats import update_manual, get_stats


if __name__ == '__main__':
    db_parser = argparse.ArgumentParser(description="Database options", add_help=False)
    db_parser.add_argument("-db", "--database", default="BearTrapDb")
    db_parser.add_argument("-U", "--db-user", default="postgres")
    db_parser.add_argument("--password", default="postgres")
    db_parser.add_argument("--host", default="localhost")

    update_metric_parser = argparse.ArgumentParser(description="Update metric options", add_help=False)
    update_metric_parser.add_argument("--field", required=True)
    update_metric_parser.add_argument("--sql-get", required=True)
    update_metric_parser.add_argument("--sql-update", required=True)

    parser = argparse.ArgumentParser(description="Plagiarism tool")
    subparsers = parser.add_subparsers(title='Plagiarism modes',
                                       description='hasher, write, stats and compare modes',
                                       help='hasher, compared, write similarity files to db, Collect statistics for db')

    parser_hasher = subparsers.add_parser("hasher", help='get hashes subtrees')
    parser_hasher.add_argument("-f", "--file-name", required=True)
    parser_hasher.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_hasher.set_defaults(func=run_hasher)

    parser_compare = subparsers.add_parser("compare2AST_hash", help='compare 2 AST by hash')
    parser_compare.add_argument("-f", "--first-file", required=True)
    parser_compare.add_argument("-s", "--second-file", required=True)
    parser_compare.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_compare.set_defaults(func=compare_AST)

    parser_writer_hasher = subparsers.add_parser("writer_hashes",
                                                 help='write hashes to db',
                                                 parents=[db_parser, update_metric_parser])
    parser_writer_hasher.add_argument("-b", "--binary-name", default="./hasher_AST_tool")
    parser_writer_hasher.set_defaults(func=writer_hasher)

    parser_writer_similarity_v1 = subparsers.add_parser("writer_similarity_v1",
                                                        help='write similarity files to db',
                                                        parents=[db_parser, update_metric_parser])
    parser_writer_similarity_v1.add_argument("--field-hash", default="hashes")
    parser_writer_similarity_v1.set_defaults(func=writer_similarity_v1)

    parser_writer_similarity_v3 = subparsers.add_parser("writer_similarity_v3",
                                                        help='write similarity files to db',
                                                        parents=[db_parser, update_metric_parser])
    parser_writer_similarity_v3.add_argument("--field-hash", default="hashes")
    parser_writer_similarity_v3.add_argument("--field-prematch", default="match_score")
    parser_writer_similarity_v3.set_defaults(func=writer_similarity_v3)

    parser_writer_bear_trap = subparsers.add_parser("writer_bear_trap",
                                                    help='write similarity programm to db by BearTrap program',
                                                    parents=[db_parser, update_metric_parser])
    parser_writer_bear_trap.add_argument("-b", "--binary-name", default="./bearTrap")
    parser_writer_bear_trap.set_defaults(func=writer_bear_trap)

    parser_writer_simian = subparsers.add_parser("writer_simian",
                                                 help='write similarity programm to db by simian program',
                                                 parents=[db_parser, update_metric_parser])
    parser_writer_simian.add_argument("--jar-name", required=True)
    parser_writer_simian.set_defaults(func=writer_simian)

    parser_update_manual = subparsers.add_parser("manual_similarity",
                                                 help='UI for manual get similarity',
                                                 parents=[db_parser])
    parser_update_manual.add_argument("--sql-get", required=True)
    parser_update_manual.add_argument("--sql-update", required=True)
    parser_update_manual.add_argument("--manual-field", default="manual_check")
    parser_update_manual.set_defaults(func=update_manual)

    parser_get_stats = subparsers.add_parser("get_stats",
                                             help='get metrics and graphs, where fields its common separated fields',
                                             parents=[db_parser])
    parser_get_stats.add_argument("--sql-get", required=True)
    parser_get_stats.add_argument("--fields", required=True)
    parser_get_stats.add_argument("--manual-field", default="manual_check")
    parser_get_stats.set_defaults(func=get_stats)

    args = parser.parse_args()
    args.func(args)
