def compare_array(fst_subtree_hashes, snd_subtree_hashes):
    i, j = 0, 0
    answer = 0
    while i < len(fst_subtree_hashes) and j < len(snd_subtree_hashes):
        if fst_subtree_hashes[i] == snd_subtree_hashes[j]:
            answer += 1
            i += 1
            j += 1
        elif fst_subtree_hashes[i] > snd_subtree_hashes[j]:
            j += 1
        else:
            i += 1

    return answer / max(len(fst_subtree_hashes), len(snd_subtree_hashes))
