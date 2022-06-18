SELECT DISTINCT ON (runs_1.id) runs_1.id, runs_1.content
FROM
    text_matches as matches
        INNER JOIN
    runs as runs_1
    ON matches.first_runs_id = runs_1.id
WHERE matches.manual_check is not NULL
