<?php
print $anchors; ?>
<div <?php if ( !empty($message_classes)) { ?>class="<?php echo implode(' ', $message_classes); ?>" <?php } ?> id="privatemsg-mid-<?php print $mid; ?>">
  <div class="privatemsg-author-avatar">
    <?php print $author_picture; ?>
  </div>
  <div class="privatemsg-message-column">

        <div class="privatemsg-message-information">
            <span class="privatemsg-author-name"><?php print $author_name_link; ?></span>
            <?php if (isset($message_action_block)): ?>
                <span class="privatemsg-user-block">
                    <?php print $message_action_block ?>
                </span>
            <?php endif ?>

            <?php if (isset($message_action_delete)): ?>
                <span class="privatemsg-message-delete">
                    <?php print $message_action_delete ?>
                </span>
            <?php endif ?>

            <span class="privatemsg-message-date"><?php print $message_date; ?></span>

            <?php if (isset($new)): ?>
                <span class="new privatemsg-message-new"><?php print $new ?></span>
            <?php endif ?>
        </div>
    <div class="privatemsg-message-body">
      <?php print $message_body; ?>
    </div>
  </div>
  <div class="clearfix"></div>
</div>
