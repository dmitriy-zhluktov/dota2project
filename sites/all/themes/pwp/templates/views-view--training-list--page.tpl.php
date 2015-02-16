<?php
$rows = $view->result;
?>
<?php if(count($rows) > 0): ?>
    <ul class="training-list">
        <?php foreach($rows as $k => $row): ?>
            <?php $pro = user_load($row->node_uid) ?>
            <li class="<?php print $k%2 == 0 ? 'even' : 'odd' ?> <?php print count($rows) == ++$k ? 'last' : '' ?>">
                <div class="user-pic">
                    <?php $img = theme('image_style', array('style_name' => 'user_profile', 'path' => $pro->picture->uri)); ?>
                    <a href="user/<?php print $pro->uid ?>" target="_blank" title="<?php print $pro->name ?>">
                        <?php print $img ?>
                    </a>
                </div>
                <div class="training-desc">
                    <h2><?php print $row->node_title ?></h2>
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