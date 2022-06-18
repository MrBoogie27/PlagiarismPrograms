import pandas as pd
import PySimpleGUI as sg
from psycopg2 import sql
from writer import get_connect
import numpy as np
import matplotlib.pyplot as plt

WIDTH = 50
HEIGHT = 35
THRESHOLD = 0.5

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


def get_all_metrics(args):
    with get_connect(args) as conn:
        with conn.cursor() as cursor:
            sql_command = open(args.sql_get, mode='r').read()
            identifier = []
            for field in args.fields.split(','):
                identifier.append(sql.Identifier(field))
            identifier.append(sql.Identifier(args.manual_field))
            stmt = sql.SQL(sql_command).format(
                sql.SQL(',').join(identifier),
                sql.Identifier(args.manual_field)
            )
            cursor.execute(stmt)
            all_compares = []
            for row in cursor:
                all_compares.append(list(row))
            return all_compares


def get_stats(args):
    all_metrics = get_all_metrics(args)
    fields = args.fields.split(',')
    print_table(all_metrics, fields)
    print_graphs(all_metrics, fields)


def print_table(all_metrics, fields):
    data = {}
    common = {
        'TP': 'True Positive',
        'FP': 'False Positive',
        'FN': 'False Negative',
        'TN': 'True Negative'
    }
    euclidean_distances = {}
    for j in range(len(fields)):
        cnt = {
            'TP': 0,
            'FP': 0,
            'FN': 0,
            'TN': 0
        }
        ed = 0
        for row in all_metrics:
            current = row[j]
            target = row[-1]
            ed += (current - target) * (current - target)
            if current > THRESHOLD:
                if target > THRESHOLD:
                    cnt['FP'] += 1
                else:
                    cnt['TP'] += 1
            else:
                if target > THRESHOLD:
                    cnt['TN'] += 1
                else:
                    cnt['FN'] += 1
        old_keys = list(cnt.keys())
        for key in old_keys:
            new_key = common[key]
            cnt[new_key] = cnt[key] * 100.0 / len(all_metrics)
            del cnt[key]
        data[fields[j]] = cnt
        euclidean_distances[fields[j]] = round(ed / len(all_metrics), 3)

    df = pd.DataFrame(data).round(2)
    print(df)
    print(euclidean_distances)


def print_graphs(all_metrics, fields):
    print_histogram(all_metrics, fields)
    print_distribution(all_metrics, fields)


def print_histogram(all_metrics, fields):
    data = {}
    for j in range(len(fields)):
        cnt = {
            'Negative': 0,
            'Positive': 0,
        }
        for row in all_metrics:
            current = row[j]
            if current > THRESHOLD:
                cnt['Positive'] += 1
            else:
                cnt['Negative'] += 1

        for key in cnt.keys():
            cnt[key] = cnt[key] * 100.0 / len(all_metrics)
        data[fields[j]] = cnt

    df = pd.DataFrame(data).round(2)
    print(df)

    x = range(len(fields))

    rects1 = plt.bar(x=x, height=df.loc['Positive', :], width=0.4, alpha=0.8, color='green', label='Positive')
    rects2 = plt.bar(x=[i + 0.4 for i in x], height=df.loc['Negative', :], width=0.4, color='red', label='Negative')
    plt.ylim(0, 100)
    plt.ylabel("Percent")

    plt.xticks([index + 0.2 for index in x], fields)
    plt.xlabel("Metrics")
    plt.title('Распределение вердиктов')
    plt.legend()

    for rect in rects1:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width() / 2, height+1, str(height), ha="center", va="bottom")
    for rect in rects2:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width() / 2, height+1, str(height), ha="center", va="bottom")

    plt.show()


def print_distribution(all_metrics, fields):
    np_metrics = np.array(all_metrics).transpose()
    np_metrics = np.multiply(np_metrics, 100.0)
    # plt.hist(list(np_metrics)[:-1], histtype='step', bins=100, label=fields, density=True)
    # plt.ylabel("Percent")
    plt.xlabel("Value")
    plt.title('Плотности распределений метрик')

    for np_metrics_one in np_metrics[:-1]:
        pd.Series(np_metrics_one).plot.kde()
    plt.xlim(0, 100)
    plt.legend(fields)
    plt.show()
