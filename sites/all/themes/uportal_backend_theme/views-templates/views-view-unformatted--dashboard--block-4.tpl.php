<?php

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);
$no_results_to_show = 5;
$no_results_print = 'Displaying '.$no_results_to_show.' of '.number_format($no_results, 0).' total items';
$cnter = 0;

$all_drafts_link_args = uportal_backend_content_listing_build_path(array (
  'status' => 0,
));
$all_drafts_link = '<a class="btn" href="'.$all_drafts_link_args['path'].'"><span class="icon"><span></span></span><span class="text">View all</span></a>';

?>

<div class="draft-content-header header-wrapper clearfix">
  <div class="header-title drafts-title">
    <?php print $no_results_print; ?>
  </div>
  <div class="header-link">
    <?php print $all_drafts_link; ?>
  </div>
</div>

<div class="draft-content-wrapper">
  
<?php foreach ($rows as $id => $row): ?>
  <div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?>>
    <?php print $row; ?>
  </div>
<?php
    if (++$cnter==$no_results_to_show) break;
  endforeach;
?>

</div>