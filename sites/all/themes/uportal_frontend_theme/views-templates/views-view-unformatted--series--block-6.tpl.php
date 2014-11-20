<?php

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;

//$images_view = views_get_view_result('series', 'block_4', 117);
$images_view = views_get_view_result('series', 'block_4', $raw_results[0]->nid);
$images_str = '<span class="type-icon icon"><span></span></span><span class="node-status"><span></span></span>';
$img_cnter = 0;
$category_nid = $raw_results[0]->node_field_data_field_topic_reference_nid;
$first_node_type = false;

$show_no_images = 4;
$images_cnter = 1;
foreach ($images_view as $image_obj) {
	if ($images_cnter>$show_no_images) {
		break;
	}
	$node_type = $image_obj->node_type;
	$img_raw = $image_obj->field_field_image[0]['raw'];
	$img_render = $image_obj->field_field_image[0]['rendered'];
	$img_video_render = $image_obj->field_field_video[0]['rendered'];
	if ( isset($img_raw['is_default']) && $img_raw['is_default'] && $node_type=='video' && $img_video_render ) {
		$img = render($img_video_render);
	} else {
		$img = render($img_render);
	}
	$images_str .= '<span class="img img-'.($img_cnter++).'">'.$img.'</span>';
	if (!$first_node_type) {
		$first_node_type = $node_type;
	}
	$images_cnter++;
}

//title string on hover
$title_str = '<span class="hover-title"><span class="wrapper">';
$title_str .= '<span class="category-title">'.$rendered_results[0]['title_1'].'</span>';
$title_str .= '<span class="series-title">'.$rendered_results[0]['title'].'</span>';
$title_str .= '</span></span>';

?>

<a class="series-list-item-wrapper clearfix category-class-nid-<?php print $category_nid ?>" href="/view/series/<?php print $raw_results[0]->nid ?>">
	
	<span class="series-images node-type-<?php print $first_node_type ?> clearfix">
		<?php print $images_str; ?>
		<?php print $title_str; ?>
	</span>
	
	<span class="series-info">
		<span class="label">Series:</span>
		<span class="title"><?php print $rendered_results[0]['title'] ?></span>
		<span class="desc"><?php print $rendered_results[0]['body'] ?></span>
	</span>
	
</a>