<?php

function helper_get_matches_list() {
    global $user;
    $uid = $user->uid;
    $type = 'match';
    $nodes = array();

    if(!in_array('pro', $user->roles)) {
        $nids = array();
        $query = "SELECT t.match_id FROM {teams} AS t
          JOIN {players} AS p ON p.team_id = t.id
          WHERE p.uid = $uid";
        $exec = db_query($query);
        while($res = $exec->fetchAssoc()) {
            $nids[] = $res['match_id'];
        }

        if(count($nids) > 0) {
            $query = new EntityFieldQuery();
            $entities = $query->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', $type)
                ->propertyCondition('nid', $nids, 'in')
                ->execute();
            if(count($entities) > 0) {
                $nodes = node_load_multiple(array_keys($entities['node']));
            }
        }
    } else {
        $query = new EntityFieldQuery();
        $entities = $query->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', $type)
            ->propertyCondition('uid', $uid)
            ->execute();
        if(count($entities) > 0) {
            $nodes = node_load_multiple(array_keys($entities['node']));
        }
    }
    if(count($nodes) > 0) {
        //krumo($nodes);
        $header = array(
            t('ID'),
            t('Pro'),
            t('Date'),
            t('State'),
        );
        foreach($nodes as $node) {
            if($node->field_state[$node->language][0]['value'] == 0) {
                $state = t('Planned');
            } elseif($node->field_state[$node->language][0]['value'] == 1) {
                $state = t('Active');
            } else {
                $state = t('Finished');
            }
            $rows[] = array(
                array(
                    'data' => l($node->nid, 'node/'.$node->nid),
                    'class' => array('id')
                ),
                array(
                    'data' => l($node->name, 'user/'.$node->uid, array('attributes' => array('class' => array('pro-user')))),
                    'class' => array('nicknames')
                ),
                array(
                    'data' => date('Y-m-d H:i', strtotime($node->field_date[$node->language][0]['value'])),
                    'class' => array('date')
                ),
                array(
                    'data' => $state,
                    'class' => array('state')
                ),
            );
        }

        return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('user-matches'))));
    } else {
        return t("You didn't play yet");
    }
}