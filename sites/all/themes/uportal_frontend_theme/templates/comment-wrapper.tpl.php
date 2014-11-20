
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  
  <div class="clearfix comments-form-list-wrapper">
    <div class="uportal-shadow comments-form">
      <div class="form-title">What do you think about this video?</div>
      <ul class="emotions-list clearfix">
        <li class="smiley"><a href="#"><span></span></a></li>
        <li class="sad"><a href="#"><span></span></a></li>
        <li class="nothing"><a href="#"><span></span></a></li>
        <li class="laugh"><a href="#"><span></span></a></li>
        <li class="shocked"><a href="#"><span></span></a></li>
      </ul>
      <?php if ($content['comment_form']): ?>
        <?php
          //$content['comment_form']['author']['#weight'] = 1;
          print render($content['comment_form']);
        ?>
      <?php endif; ?>    </div>
    <div class="comments-list">
      <?php print render($content['comments']); ?>
    </div>
  </div>
  
</div>
