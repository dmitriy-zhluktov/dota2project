<div class="user-wrapper clearfix">
    <div class="user-picture-wrap">
        <?php if ($user_profile['user_picture']): ?>
            <?php print render($user_profile['user_picture']); ?>
        <?php endif; ?>
        <?php if($user_profile['self']): ?>
            <div class="pic-form">
                <div class="hide-left">
                    <?php print render($user_profile['picform']); ?>
                </div>
                <span id="change-pic"><?php print t('Change picture') ?></span>
            </div>
        <?php else: ?>
            <div class="pic-form">
                <?php print $user_profile['msglink']; ?>
            </div>
        <?php endif ?>
        <div class="steam-profile">
            <?php
                $steam = false;
                if(count($user_profile['hybridauth_identities']['#rows']) > 0) {
                    foreach($user_profile['hybridauth_identities']['#rows'] as $hybrid) {
                        if($hybrid[0] === 'Steam') {
                            $steam = $hybrid[1];
                            $view = '';
                            $steam = preg_replace('/<a(.+?)>.+?<\/a>/i',"<a$1>$view</a>",$steam);
                        }
                    }
                }
            ?>
            <?php if($steam): ?>
                <div class="steam-identity">
                    <?php print $steam ?>
                </div>
            <?php else: ?>
                <div class="steam-identity">
                    <a href="#" title="<?php print t('No Steam account associated yet.'); ?>"></a>
                </div>
            <?php endif ?>

        </div>
    </div>
    <div class="user-profile-level">
        <h1><?php print $user_profile['name'] ?></h1><span class="info-icon">i</span>
        <?php if($user_profile['description']): ?>
            <div class="show-info hidden"><?php print $user_profile['description'] ?></div>
        <?php endif ?>

        <span class="<?php print $user_profile['is_online']? 'online': 'offline' ?>">•</span>

        <div class="<?php print $user_profile['pro']? 'pro ':''?>profile-level">

        </div>
        <div class="member-for">
            <?php print t('Member for:')?><span> <?php print $user_profile['member'] ?></span>
        </div>
    </div>
    <div class="user-ratings">
        <div class="match-box all">
            <span><?php print t('Matches'); ?></span>
            <div class="all-matches">
                <?php print $user_profile['match']['total'] ?>
            </div>
        </div>
        <div class="separator-horizontal float-left">-</div>
        <div class="float-left">
            <div class="match-box wins">
                <span><?php print t('Wins'); ?></span>
                <div class="wins">
                    <?php print $user_profile['match']['win'] ?>
                </div>
            </div>

            <div class="match-box lost">
                <span><?php print t('Losts'); ?></span>
                <div class="lost">
                    <?php print $user_profile['match']['lost'] ?>
                </div>
            </div>
            <div class="clear"></div>

            <div class="win-rating clearfix">
                <span class="winrate-percents"><?php print $user_profile['match']['percents'] ?></span> <?php print t('winrate'); ?>
            </div>
        </div>
    </div>
    <div class="user-ratings">
        <span class="title"><?php print t('Rating'); ?></span>
        <div class="rating-box-big">
            <div class="rating" style="width: <?php print $user_profile['avg_rating']; ?>%;"></div>
        </div>

        <span class="title margin-top10"><?php print t('Time played'); ?></span>
        <div class="time-played">
            <?php print $user_profile['time_played']; ?>
        </div>
    </div>
    <?php if ($user_profile['summary']): ?>
        <?php print render($user_profile['summary']); ?>
    <?php endif; ?>
    <?php if($user_profile['ban'] && $user_profile['self']): ?>
        <div class="hidden">
            <div id="ban-above-mask">
                <h2><?php print t('You have been banned!'); ?></h2>
                <div class="expiration"><?php print $user_profile['ban']['expiration']?></div>
                <div class="description">
                    <div class="grey"><?php print t('Ban reason: ') ?></div>
                    <div class="plain"><?php print $user_profile['ban']['description']; ?></div>
                </div>
            </div>
        </div>
        <script type="text/javascript">
            jQuery.colorbox({inline:true, open:true, href:"#ban-above-mask" });
        </script>
    <?php endif ?>
</div>
<?php if(isset($user_profile['vods']) && $user_profile['vods'] != null): ?>
    <div class="user-vods-wrapper">
        <div class="vods-button">
            <button type="button" id="show-vods-list"><?php print t('VODs'); ?></button>
        </div>
        <div class="clear"></div>
        <div class="vods-list-wrapper hide-left">
            <div class="hidden">
                <div id="vod-player">
                    <iframe width="560" height="315" src="" frameborder="0" allowfullscreen></iframe>
                </div>
            </div>
            <ul class="horizontalcarousel jcarousel-skin-default">
                <?php foreach($user_profile['vods'] as $vod) : ?>
                    <li>
                        <img src="<?php print $vod['thumb'] ?>" title="<?php print $vod['title'] ?>" alt="<?php print $vod['title'] ?>" width="150" height="90"/>
                        <span class="play-button" data-video="<?php print $vod['source'] ?>"></span>
                        <span class="vod-title"><?php print $vod['title'] ?></span>
                        <div class="red-line"></div>
                        <div class="vod-info clearfix">
                            <span class="float-left"><?php print $vod['date'] ?></span>
                            <span class="float-right"><?php print $vod['views'] ?></span>
                        </div>
                    </li>
                <?php endforeach; ?>
            </ul>
            <?php jcarousel_add('horizontalcarousel'); ?>
        </div>
    </div>
<?php endif; ?>