from common import compare_array
from prepare_hash import run_binary_hasher
import psycopg2
import tempfile
from psycopg2 import sql


TABLE_NAME='runs'
COLUMNS=('id', 'content', 'ASTHash')

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
    PROBLEM_ID=3
    COUNT_LIMIT = 100
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


def writer_similarity(args):
    pass

