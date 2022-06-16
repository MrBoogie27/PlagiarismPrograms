import PySimpleGUI as sg
from writer import get_connect

WIDTH = 80
HEIGHT = 35


def get_db_data(cursor, sql_file):
    return [
        (1, 2, 3, 4),
        (5, 6, 7, 8)
    ]


def update_db(cursor, verdict):
    sg.Popup("Catch {}".format(verdict))


def update_manual(args):
    layout = [
        [
            sg.Column([[sg.InputText(key='first_label', disabled=True, size=(WIDTH, None))],
                       [sg.Output(key='first_output', size=(WIDTH, HEIGHT))]]),
            sg.Column([[sg.InputText(key='second_label', disabled=True, size=(WIDTH, None))],
                       [sg.Output(key='second_output', size=(WIDTH, HEIGHT))]])
        ],
        [sg.Button('Cheater'), sg.Button('Different')],
        [sg.Cancel()]
    ]
    window = sg.Window('Program Compare', layout, finalize=True)
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            for key1, key2, value1, value2 in get_db_data(cursor, args):
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
                        update_db(cursor, 1)
                        break
                    elif event == 'Different':
                        update_db(cursor, 0)
                        break
    window.close()
