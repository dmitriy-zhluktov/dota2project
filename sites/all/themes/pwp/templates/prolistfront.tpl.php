<?php
    $users = $variables['users'];
?>
<div class="pro-list">
    <ul class="horizontalcarousel jcarousel-skin-default">
        <?php foreach($users as $user) : ?>
            <li>
                <?php
                $uri = file_build_uri('pictures/user-default.png');
                if(isset($user->picture) && $user->picture != NULL) {
                    $uri = $user->picture->uri;
                }
                $pic = theme('image_style', array('path' => $uri, 'style_name' => 'user_profile')); ?>
                <?php print $pic ?>
                <div class="pro-name">
                    <?php print l($user->name, 'user/'.$user->uid, array('attributes' => array('class' => array('prolist-name'), 'title' => $user->name))) ?>
                </div>
            </li>
        <?php endforeach; ?>
    </ul>
    <?php jcarousel_add('horizontalcarousel'); ?>
</div>