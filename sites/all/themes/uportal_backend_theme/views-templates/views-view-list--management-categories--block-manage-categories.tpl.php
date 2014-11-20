<?php

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = count($raw_results);

//get all categories in their right order
$categories_view = views_get_view_result('management_categories', 'block_1');
$category_results = array();
foreach ($categories_view as $category) {
  $category_nid = $category->nid;
  
  //category link
  $category_link_args = uportal_backend_content_listing_build_path(array (
		'category_nid' => $category_nid,
		'link_type' => 'category',
	));
  $category_path = $category_link_args['path'];
  
  //category icon
  $category_icon = '';
  $category_icon_name = '';
  if (count($category->field_field_category_icon) && isset($category->field_field_category_icon[0]['rendered'])) {
    $category_icon = render($category->field_field_category_icon[0]['rendered']);
    $category_icon_name = $category->field_field_category_icon[0]['raw']['filename'];
  }
  
  //category body
  $category_body = '';
  if (count($category->field_body) && isset($category->field_body[0]['rendered'])) {
    $category_body = render($category->field_body[0]['rendered']);
  }
  
  //category results
  $category_results['category-'.$category_nid] = array (
    'category-title' => $category->node_title,
    'category-icon' => $category_icon,
    'category-icon-name' => $category_icon_name,
    'category-nid' => $category_nid,
    'category-desc' => $category_body,
    'category-path' => $category_path,
		'published_count' => 0,
		'draft_count' => 0,
		'total_count' => 0,
    'real_count' => 0,
    'count' => 0,
  );
}

//get count per category
$content_types = array('video'=>1, 'audio'=>1, 'document'=>1,);
foreach ($raw_results as $raw_result) {
  $category_nid = $raw_result->node_field_data_field_topic_reference_nid;
	if (!isset($category_results['category-'.$category_nid])) {
		continue;
	}
	$node_status = intval($raw_result->node_status);
  $category_results['category-'.$category_nid]['real_count']++;
  if (isset($content_types[$raw_result->node_type])) {
    $category_results['category-'.$category_nid]['count']++;
    $category_results['category-'.$category_nid]['total_count']++;
		if ($node_status) {
			$category_results['category-'.$category_nid]['published_count']++;
		} else {
			$category_results['category-'.$category_nid]['draft_count']++;
		}
  }
}

?>

<div class="metadata-listing-results-header-wrapper">
  <div class="metadata-listing-page-results-header clearfix centred-strip">
    <ul class="manage-categories-list-header clearfix ">
      <li class="label menu-order-label">Menu Order</li>
      <li class="label category-title-label">Category Name</li>
      <li class="label icon-label">Icon</li>
      <li class="label description-label">Description</li>
      <li class="label no-items-label">Items (P/D) <div class="hover">No. of Items (Published/Drafts)</div></li>
      <li class="label links-label">Tools</li>
    </ul>
  </div>
</div>

<div class="centred-strip">
<ul class="manage-categories-list">

<?php
  $cnter = 1;
  foreach ($category_results as $category_result) {
		$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata-category-row.tpl.php';
		$row_vars = array (
			'category_nid' => $category_result['category-nid'],
      'icon_name' => $category_result['category-icon-name'],
      'cnter' => $cnter,
      'category_path' => $category_result['category-path'],
      'category_title' => $category_result['category-title'],
      'category_icon' => $category_result['category-icon'],
      'category_desc' => $category_result['category-desc'],
      'category_count' => $category_result['count'],
      'category_real_count' => $category_result['real_count'],
      'category_published_count' => $category_result['published_count'],
      'category_draft_count' => $category_result['draft_count'],
      'category_total_count' => $category_result['total_count'],
		);
		print _uportal_backend_load_view($template_path_row, $row_vars);
    $cnter++;
  }
?>

</ul>
</div>

<div id="categories-queue-operations" class="clearfix centred-strip">
  <div class="icon"></div>
  <div class="desc">
    <div class="unsaved">* There are unsaved changes. Click SAVE below, to save your changes.</div>
    <div class="saving">Saving order of categories ...</div>
    <div class="saved">Order of categories saved.</div>
  </div>
</div>

<div id="save-cancel-categories-btns-wrapper" class="clearfix centred-strip">
	<a href="#" class="btn save-btn">Save Menu Order</a>
	<a href="#" class="cancel btn light-btn">Cancel</a>
</div>
