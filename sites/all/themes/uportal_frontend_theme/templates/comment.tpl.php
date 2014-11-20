
<div class="uportal-shadow <?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  
  <div class="comment-text clearfix">
    <div class="emotion-time-wrapper">
      <div class="emotion"><div></div></div>
      <div class="time">
        <?php print format_interval(time()-$comment->created, 1).' ago' ?>
      </div>
    </div>
    <div class="comment-author-body">
      <div class="comment-author">
        <?php print $comment->name; ?>
      </div>
      <div class="comment-body">
        <?php print render($content['comment_body']); ?>
      </div>
    </div>
  </div>

</div>
