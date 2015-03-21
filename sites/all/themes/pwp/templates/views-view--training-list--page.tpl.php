<?php
$rows = $view->result;
?>
<?php if(count($rows) > 0): ?>
    <ul class="training-list">
        <?php foreach($rows as $k => $row): ?>
            <?php
            if (!empty($row->users_node_picture)) {
                if (is_numeric($row->users_node_picture)) {
                    $row->picture = file_load($row->users_node_picture);
                }
                if (!empty($row->picture->uri)) {
                    $filepath = $row->picture->uri;
                }
            } elseif (variable_get('user_picture_default', '')) {
                $filepath = variable_get('user_picture_default', '');
            }
            ?>
            <li class="<?php print $k%2 == 0 ? 'even' : 'odd' ?> <?php print count($rows) == ++$k ? 'last' : '' ?>">
                <div class="user-pic">
                    <?php $img = theme('image_style', array('style_name' => 'user_profile', 'path' => $filepath)); ?>
                    <a href="user/<?php print $row->node_uid ?>" target="_blank" title="<?php print $row->users_node_name ?>">
                        <?php print $img ?>
                    </a>
                    <a href="user/<?php print $row->node_uid ?>" target="_blank" title="<?php print $row->users_node_name ?>">
                        <?php print $row->users_node_name ?>
                    </a>
                </div>
                <div class="training-desc">
                    <h2><?php print l($row->node_title, 'training/'.$row->nid, array('attributes' => array('class' => array('title'), 'title' => $row->node_title))) ?></h2>
                    <?php if (count($row->field_body) > 0): ?>
                        <div class="desc">
                            <?php print $row->field_body[0]['rendered']['#markup'] ?>
                        </div>
                    <?php endif ?>
                </div>
                <div class="training-buy">
                    <?php if ($row->field_field_training_player_id[0]['raw']['value'] > 0): ?>
                        <div class="desc">
                            <?php print $row->field_field_training_player_id[0]['raw']['value'] ?>
                        </div>
                    <?php endif ?>
                </div>
                <div class="clear"></div>
            </li>
        <?php endforeach ?>
    </ul>
<?php endif ?>