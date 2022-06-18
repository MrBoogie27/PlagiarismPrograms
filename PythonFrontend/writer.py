from common import compare_array
from compare import run_bear_trap, get_combine_sim, run_simian
from prepare_hash import run_binary_hasher
import psycopg2
import tempfile
from psycopg2 import sql


def get_connect(args):
    return psycopg2.connect(dbname=args.database, user=args.db_user,
                            password=args.password, host=args.host)


def insert_hash(row, update_data, binary_file):
    with tempfile.NamedTemporaryFile(mode="w+t", suffix='.cpp') as fp:
        fp.writelines(row[1])
        fp.seek(0)
        try:
            hashes = run_binary_hasher(binary_file, fp.name)
            update_data[row[0]] = hashes
        except Exception as e:
            print('error for {}: {}'.format(row[0], e))


def update_hashes(cursor, update_data, args):
    sql_command = open(args.sql_update, mode='r').read()
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(args.field)
    )

    for key, hashes in update_data.items():
        cursor.execute(stmt, (hashes, key))
        print("updated {}".format(key))


def writer_hasher(args):
    update_data = {}
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_command = open(args.sql_get, mode='r').read()
            stmt = sql.SQL(sql_command)
            cursor.execute(stmt)
            for row in cursor:
                insert_hash(row, update_data, args.binary_name)
            update_hashes(cursor, update_data, args)


def writer_similarity_v1(args):
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_command = open(args.sql_get, mode='r').read()
            stmt = sql.SQL(sql_command).format(
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field)
            )
            cursor.execute(stmt)
            all_compares = []
            for row in cursor:
                if not row[2] or not row[3]:
                    continue
                similarity = compare_array(row[2], row[3])
                all_compares.append({
                    'fst_id': row[0],
                    'snd_id': row[1],
                    'similarity': similarity
                })
            update_data_func(cursor, all_compares, args)


def writer_similarity_v3(args):
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_command = open(args.sql_get, mode='r').read()
            stmt = sql.SQL(sql_command).format(
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_prematch),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field_hash),
                sql.Identifier(args.field)
            )
            cursor.execute(stmt)
            all_compares = []
            for row in cursor:
                if not row[2] or not row[3]:
                    continue
                similarity = get_combine_sim(row[2], row[3], row[4])
                all_compares.append({
                    'fst_id': row[0],
                    'snd_id': row[1],
                    'similarity': similarity
                })
            update_data_func(cursor, all_compares, args)


def get_similarity(row, binary_path, update_data, method):
    with tempfile.NamedTemporaryFile(mode="w+t", suffix='.cpp') as first_file:
        first_file.writelines(row[2])
        first_file.seek(0)
        with tempfile.NamedTemporaryFile(mode="w+t", suffix='.cpp') as second_file:
            second_file.writelines(row[3])
            second_file.seek(0)
            try:
                similarity = method(binary_path, first_file.name, second_file.name)
                update_data.append(
                    {
                        'fst_id': row[0],
                        'snd_id': row[1],
                        'similarity': similarity
                    }
                )
            except Exception as e:
                print(e)
                print('error for {}'.format(row[0], row[1]))


def get_bear_trap_similarity(row, binary_path, update_data):
    get_similarity(row, binary_path, update_data, run_bear_trap)


def get_simian_similarity(row, jar_name, update_data):
    get_similarity(row, jar_name, update_data, run_simian)


def update_data_func(cursor, update_data, args):
    sql_command = open(args.sql_update, mode='r').read()
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(args.field)
    )
    for sim_info in update_data:
        cursor.execute(stmt, (sim_info['similarity'],
                              sim_info['fst_id'],
                              sim_info['snd_id'])
                       )
    print("Updated {} rows".format(len(update_data)))


def writer_bear_trap(args):
    update_data = []
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_template = open(args.sql_get, mode='r').read()
            stmt = sql.SQL(sql_template).format(
                sql.Identifier(args.field)
            )
            i = 0
            while True:
                cursor.execute(stmt)
                if cursor.rowcount == 0:
                    break
                print("Run iteration number {} for {} rows".format(i, cursor.rowcount))
                for row in cursor:
                    get_bear_trap_similarity(row, args.binary_name, update_data)
                update_data_func(cursor, update_data, args)
                print("Rows updated")
                i += 1


def writer_simian(args):
    update_data = []
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_template = open(args.sql_get, mode='r').read()
            stmt = sql.SQL(sql_template).format(
                sql.Identifier(args.field)
            )

            cursor.execute(stmt)
            for row in cursor:
                get_simian_similarity(row, args.jar_name, update_data)
            print(update_data)
            update_data_func(cursor, update_data, args)
            print("Rows updated")
