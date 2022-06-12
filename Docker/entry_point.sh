#!/bin/bash

touch /var/log/cron.log
crontab ./cron_file
cron && tail -f /var/log/cron.log
