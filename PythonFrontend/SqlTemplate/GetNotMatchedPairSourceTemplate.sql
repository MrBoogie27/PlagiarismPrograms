SELECT runs_1.id, runs_2.id, runs_1.normalized_text, runs_2.normalized_text
FROM
    text_matches as matches
        INNER JOIN
    runs as runs_1
    ON matches.first_runs_id = runs_1.id
        INNER JOIN
    runs as runs_2
    ON matches.second_runs_id = runs_2.id
WHERE matches.{} is NULL and matches.first_runs_id != matches.second_runs_id
LIMIT 10000