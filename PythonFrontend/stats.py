import PySimpleGUI as sg
from psycopg2 import sql
from writer import get_connect

WIDTH = 50
HEIGHT = 35


def get_db_data(args, cursor, number):
    sql_command = open(args.sql_get, mode='r').read()
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(args.manual_field)
    )
    cursor.execute(stmt, (number, ))
    all_compares = []
    for row in cursor:
        all_compares.append(row)
    return all_compares


def update_db(args, cursor, key1, key2, verdict):
    sql_command = open(args.sql_update, mode='r').read()
    stmt = sql.SQL(sql_command).format(
        sql.Identifier(args.manual_field)
    )
    cursor.execute(stmt, (verdict, key1, key2))


def update_manual(args):
    layout = [
        [
            sg.Column([[sg.InputText(key='first_label', disabled=True, size=(WIDTH, None))],
                       [sg.Output(key='first_output', size=(WIDTH, HEIGHT), font=('Helvetica', 16))]]),
            sg.Column([[sg.InputText(key='second_label', disabled=True, size=(WIDTH, None))],
                       [sg.Output(key='second_output', size=(WIDTH, HEIGHT), font=('Helvetica', 16))]])
        ],
        [sg.Button('Cheater'), sg.Button('Different'), sg.Button('Skip')],
        [sg.Cancel()]
    ]
    window = sg.Window('Program Compare', layout, finalize=True)
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            for number in range(2, 100):
                for key1, key2, value1, value2 in get_db_data(args, cursor, str(number)):
                    window['first_label'].update(key1)
                    window['second_label'].update(key2)
                    window['first_output'].update(value1)
                    window['second_output'].update(value2)

                    while True:
                        event, values = window.read()
                        if event in (None, 'Exit', 'Cancel'):
                            window.close()
                            return
                        elif event == 'Cheater':
                            update_db(args, cursor, key1, key2, True)
                            break
                        elif event == 'Different':
                            update_db(args, cursor, key1, key2, False)
                            break
                        elif event == 'Skip':
                            break
    # window.close()
