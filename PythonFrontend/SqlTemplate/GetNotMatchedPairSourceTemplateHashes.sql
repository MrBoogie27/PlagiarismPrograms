SELECT runs_1.id, runs_2.id, runs_1.{}, runs_2.{}
FROM
    text_matches as matches
        INNER JOIN
    runs as runs_1
    ON matches.first_runs_id = runs_1.id
        INNER JOIN
    runs as runs_2
    ON matches.second_runs_id = runs_2.id
WHERE runs_1.{} is NOT NULL and runs_2.{} is NOT NULL and matches.{} IS NULL
