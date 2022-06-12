FROM hnagamin/llvm-10.0

WORKDIR /app/BearTrap

COPY ./Docker/install.sh ./
RUN ./install.sh

COPY ./Docker/run.sh ./
COPY ./Docker/cron_file ./
COPY ./Docker/entry_point.sh /etc/entry_point.sh
COPY ./PythonFrontend ./
COPY ./cmake-build-release/bearTrap ./

ENTRYPOINT ["/etc/entry_point.sh"]
