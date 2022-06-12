#!/bin/bash

export PYTHONPATH=/usr/local/bin/python3
cd /app/BearTrap
python3 ./main.py writer_bear_trap \
                      --binary-name ./bearTrap \
                      --database BearTrapDb \
                      --db-user postgres \
                      --password postgres \
                      --host localhost \
                      --field astmatchv2 \
                      --sql-get ./SqlTemplate/GetNotMatchedPairSourceTemplate.sql \
                      --sql-update ./SqlTemplate/UpdateBearTrapSimilarityTempalte.sql
