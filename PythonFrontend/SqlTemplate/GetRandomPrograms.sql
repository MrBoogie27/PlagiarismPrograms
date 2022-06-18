SELECT runs_1.id, runs_2.id, runs_1.content, runs_2.content
FROM
    text_matches as matches TABLESAMPLE SYSTEM (30)
        INNER JOIN
    runs as runs_1
    ON matches.first_runs_id = runs_1.id
        INNER JOIN
    runs as runs_2
    ON matches.second_runs_id = runs_2.id
WHERE {} is NULL and match_score > 0.6 and runs_1.problems_id = %s
LIMIT 10
