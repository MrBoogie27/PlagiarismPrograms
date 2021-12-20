from common import compare_array
from prepare_hash import run_binary_hasher
import psycopg2
import tempfile
from psycopg2 import sql


TABLE_NAME='runs'
TABLE_TEXT_MATCHES= 'text_matches'
COLUMNS=('id', 'content', 'ASTHash')
COLUMNS_MATCH=('first_runs_id', 'second_runs_id', 'match_AST_v1')
PROBLEM_ID=3
COUNT_LIMIT = 100

def get_connect(args):
    return psycopg2.connect(dbname=args.database, user=args.db_user,
                            password=args.password, host=args.host)

def insert_hash(row, update_data):
    with tempfile.NamedTemporaryFile(mode="w+t", suffix='.cpp') as fp:
        fp.writelines(row[1])
        fp.seek(0)
        try:
            hashes = run_binary_hasher("./hasher_AST_tool", fp.name)
            update_data[row[0]] = hashes
        except Exception as e:
            print('error for {}'.format(row[0]))

def all_update(cursor, update_data):
    sql_command = """UPDATE {}
               SET {} = %s
               WHERE {} = %s"""
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(TABLE_NAME),
        sql.Identifier(COLUMNS[2]),
        sql.Identifier(COLUMNS[0])
    )
    # print(update_data)
    for key, hashes in update_data.items():
        cursor.execute(stmt, (hashes, key))
        print("updated {}".format(key))

def writer_hasher(args):
    update_data = {}
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            stmt = sql.SQL('SELECT {} FROM {} where problems_id = %s LIMIT %s').format(
                sql.SQL(',').join(map(sql.Identifier, COLUMNS)),
                sql.Identifier(TABLE_NAME)
            )
            cursor.execute(stmt, (PROBLEM_ID, COUNT_LIMIT))
            for row in cursor:
                if row[2] is None:
                    insert_hash(row, update_data)
            all_update(cursor, update_data)

def update_compared(cursor, all_compares):
    sql_command = """UPDATE {}
                   SET {} = %s
                   WHERE {} = %s and {} = %s"""
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(TABLE_TEXT_MATCHES),
        sql.Identifier(COLUMNS_MATCH[2]),
        sql.Identifier(COLUMNS_MATCH[0]),
        sql.Identifier(COLUMNS_MATCH[1])
    )
    for fst_key, snd_key, comparison in all_compares:
        cursor.execute(stmt, (comparison, fst_key, snd_key))
        print("updated comparison for {} and {}".format(fst_key, snd_key))

def writer_similarity(args):
    RUN_TABLE = 'runs'
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            stmt = sql.SQL("""SELECT first_runs_id,
                                     second_runs_id,
                                     fst_run."ASTHash" as fst_hash,
                                     snd_run."ASTHash" as snd_hash
                              FROM
                                {}
                                JOIN {} as fst_run
                                ON {}.first_runs_id = fst_run.id
                                JOIN {} as snd_run
                                ON {}.second_runs_id = snd_run.id
                              WHERE {}.problems_id = %s
                                    and "match_AST_v1" IS NULL
                                    and fst_run."ASTHash" IS NOT NULL
                                    and snd_run."ASTHash" IS NOT NULL
                              LIMIT %s""").format(
                sql.Identifier(TABLE_TEXT_MATCHES),
                sql.Identifier(RUN_TABLE),
                sql.Identifier(TABLE_TEXT_MATCHES),
                sql.Identifier(RUN_TABLE),
                sql.Identifier(TABLE_TEXT_MATCHES),
                sql.Identifier(TABLE_TEXT_MATCHES)
            )
            cursor.execute(stmt, (PROBLEM_ID, COUNT_LIMIT))
            all_compares = []
            for row in cursor:
                compared = compare_array(row[2], row[3])
                all_compares.append((row[0], row[1], compared))
            update_compared(cursor, all_compares)