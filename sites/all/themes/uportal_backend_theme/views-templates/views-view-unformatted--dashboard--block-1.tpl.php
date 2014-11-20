<?php

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$category_nid = $view->args[0];
$no_results = count($view_results);

$types = array(
  'video' => array( 'no'=>0, 'name'=>'Videos', 'type_name'=>0, 'raw_no'=>0, ),
  'audio' => array( 'no'=>0, 'name'=>'Audio', 'type_name'=>0, 'raw_no'=>0, ),
  'document' => array( 'no'=>0, 'name'=>'Documents', 'type_name'=>0, 'raw_no'=>0, ),
  'series' => array( 'no'=>0, 'name'=>'Series', 'type_name'=>0, 'raw_no'=>0, ),
);

for ($i=0; $i<$no_results; $i++) {
  $types[$view_results[$i]->node_type]['no'] = $rendered_results[$i]['type'];
  $types[$view_results[$i]->node_type]['raw_no'] = $view_results[$i]->node_type_1;
  $types[$view_results[$i]->node_type]['type_name'] = $view_results[$i]->node_type;
}

?>

<?php
   foreach ($types as $machine_name=>$type):
    if ($machine_name=='series') {
      $type_link = uportal_backend_content_listing_build_path(array (
        'category_nid' => $category_nid,
        'link_type' => 'type',
        'content_section_type' => 'series',
      ));
    } else {
      $type_link = uportal_backend_content_listing_build_path(array (
        'category_nid' => $category_nid,
        'type_machine_name' => $machine_name,
        'link_type' => 'type',
      ));
    }
?>

<div class="type-numbers clearfix <?php print $machine_name ?>">
  <div class="type-link result">
    <a href="<?php print $type_link['path'] ?>"><?php print $type['name'].":" ?></a>
  </div>
  <div class="numbers result"><?php print $type['no'] ?></div>
  <div class="bar-wrapper result" data-number="<?php print $type['raw_no'] ?>">
      <div class="graph"></div>
  </div>
</div>

<?php endforeach; ?>
