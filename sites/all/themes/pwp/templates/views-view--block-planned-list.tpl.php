<?php
$rows = $view->result;
?>
<?php if(count($rows) > 0): ?>
    <?php
        $header = array(
            t('ID'),
            t('Time to start'),
            t('PRO-player'),
            t('Description'),
            t('Free slots')
        );
        $tBody = array();
        foreach($rows as $k => $row) {
            $yesImage = theme('image', array(
                'path' => drupal_get_path('theme', 'pwp').'/images/checked.png',
                'alt' => t('Free slot is available'),
                'title' => t('Free slot is available'),
                'width' => 24,
                'height' => 24
            ));
            $noImage = theme('image', array(
                'path' => drupal_get_path('theme', 'pwp').'/images/unchecked-x.png',
                'alt' => t('Free slot is not available'),
                'title' => t('Free slot is not available'),
                'width' => 24,
                'height' => 24
            ));
            $last = ($row->field_field_training_player_id[0]['raw']['value'] == 0) ? '<span class="yesno">'.$yesImage.'</span>': '<span class="yesno">'.$noImage.'</span>';
            $tBody[] = array(
                'data' => array(
                    l(t($row->node_title), 'training/'.$row->nid),
                    match_get_planned_timing(strtotime($row->field_data_field_date_field_date_value), $row->node_uid),
                    l($row->users_node_name, 'user/'.$row->node_uid, array('attributes' => array('class' => array('pro-user')))),
                    (count($row->field_body) > 0)? $row->field_body[0]['rendered']['#markup'] : '',
                    $last
                ),
                'class' => array('row-'.$k)
            );
        }

        print theme('table', array('header' => $header, 'rows' => $tBody, 'attributes' => array('class' => array('planned-matches'))));
    ?>
<?php else: ?>
    <div class="center"><?php print t('No planned trainings at this moment') ?></div>
<?php endif ?>