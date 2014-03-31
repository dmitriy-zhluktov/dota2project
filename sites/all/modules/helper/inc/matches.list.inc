<?php

function helper_get_matches_list() {
    /*global $user;
    $uid = $user->uid;

    $query = "SELECT t.match_id, st.field_state_value AS state FROM {teams} AS t
        JOIN {players} AS p ON p.team_id = t.id
        JOIN {field_data_field_state} AS st ON st.entity_id = t.match_id
        WHERE p.uid = $uid";
    $res = db_query($query)->fetchAssoc();
    krumo($res);*/
    return t('Not implemented yet');
}