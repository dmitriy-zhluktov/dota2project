<?php

function helper_get_pro_list() {
    //$perpage = 20;

    $query = db_select('users_roles', 'ur')
        ->fields('ur', array('uid'))
        ->fields('u', array('name'));
    $query->join('users', 'u', 'ur.uid = u.uid');
    $query->condition('ur.rid', 4);
    //$query = $query->extend('PagerDefault')->limit($perpage);
    $result = $query->execute();

    while($row = $result->fetchObject()) {
        $played = helper_get_pro_matches($row->uid);
        $win = helper_get_pro_win_rating($row->uid);
        $rows[] = array(
            'data' => array(
                array(
                    'data' => l($row->name, 'user/'.$row->uid, array('attributes' => array('class' => array('pro-user')))),
                    'class' => array('pro-id')
                ),
                array(
                    'data' => $played,
                    'class' => array('pro-match-amount')
                ),
                array(
                    'data' => ($win == 0)? (0).'%' : ($win/$played*100).'%',
                    'class' => array('pro-win-rating')
                ),
                array(
                    'data' => '<span class="stars">'.helper_get_pro_avg_rating($row->uid).'</span>',
                    'class' => array('pro-win-rating')
                ),

            ),
            'class' => array('pro-'.$row->uid)
        );
    }
    if(count($rows) > 0) {
        $header = array(
            t('Name'),
            t('Matches played'),
            t('Win rating'),
            t('Avg match rating')
        );
        $html = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('pro-list'))));
    } else {
        $html = t('There is no PRO-players register yet.');
    }

    return $html;
}

function helper_get_pro_matches($uid) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'match')
        ->propertyCondition('uid', $uid)
        ->fieldCondition('field_state', 'value', 2);
    $result = $query->execute();

    $count = 0;

    if (isset($result['node'])) {
        $count = count($result['node']);
        //krumo($result);
    }
    return $count;
}

function helper_get_pro_win_rating($uid) {
    $query = "SELECT COUNT(n.nid) FROM {node} AS n
        JOIN {teams} AS t ON t.match_id = n.nid
        JOIN {field_data_field_profi_command} AS p ON p.entity_id = n.nid
        JOIN {field_data_field_state} AS st ON st.entity_id = n.nid
        WHERE n.uid = $uid AND t.id = p.field_profi_command_value AND st.field_state_value = 2 AND t.winner = 1";

    $count = db_query($query)->fetchField();

    return $count;
}

function helper_get_pro_avg_rating($uid) {
    $query = "SELECT COUNT(n.nid) as count, SUM(r.field_rating_value) as rating FROM {node} AS n
        JOIN {field_data_field_rating} AS r ON r.entity_id = n.nid
        JOIN {field_data_field_state} AS st ON st.entity_id = n.nid
        WHERE n.uid = $uid AND st.field_state_value = 2 AND r.field_rating_value != 0.00";

    $result = db_query($query)->fetchAssoc();
    $output = ($result['count'] == 0)? 0 : $result['rating']/$result['count'];

    return $output;
}