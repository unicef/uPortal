<?php

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;

?>

<?php
  foreach ($rows as $id => $row):
    $rendered_result = $rendered_results[$id];
    $raw_result = $raw_results[$id];
    
    $last_comment_time = $rendered_result['last_comment_timestamp'];
    if (!$rendered_result['comment_count']) {
      $last_comment_time = 'N/A';
    }
    
    $last_download_time = $rendered_result['utime'];
    if (!$last_download_time) {
      $last_download_time = 'N/A';
    }
    
    $download_count = $rendered_result['count'];
    if (!$download_count) {
      $download_count = 0;
    }
?>

  <div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?>>
    
    <div class="category created clearfix">
      <div class="category-values">
        <label>Created on:</label><span><?php print $rendered_result['created'] ?></span>
        <label>Last updated:</label><span><?php print $rendered_result['changed'] ?></span>
      </div>
    </div><div class="category views clearfix">
      <div class="category-values">
        <label>Last View:</label><span><?php print $rendered_result['timestamp'] ?></span>
        <label>Views Today:</label><span><?php print $rendered_result['daycount'] ?></span>
        <label>Total Views:</label><span><?php print $rendered_result['totalcount'] ?></span>
      </div>
    </div>
    <div class="category comments clearfix">
      <div class="category-values">
        <label>Last Comment:</label><span><?php print $last_comment_time ?></span>
        <label>Total Comments:</label><span><?php print $rendered_result['comment_count'] ?></span>
      </div>
    </div>
    <div class="category downloads clearfix">
      <div class="category-values">
        <label>Last Download:</label><span><?php print $last_download_time ?></span>
        <label>Total Downloads:</label><span><?php print $download_count ?></span>
      </div>
    </div>

  </div>

<?php endforeach; ?>
