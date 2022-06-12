#!/bin/bash

apt-get update && apt-get -y install cron llvm python3 python3-pip
pip install -U pip && pip install psycopg2-binary
