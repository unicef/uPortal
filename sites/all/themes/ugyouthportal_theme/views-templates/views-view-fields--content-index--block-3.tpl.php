<?php

global $ugyouthportal_data;

$page_type = $ugyouthportal_data['current-index-filter-values']['page-type'];
$path = ($page_type=='all-lesson-plans') ? 'lesson-plan' : 'series';
$images_view = ($page_type=='all-lesson-plans') ? 'block_7' : 'block_4';

//make image
$lesson_plan_nid = $row->nid;
$images_results = views_get_view_result('content_index', $images_view, $lesson_plan_nid);
$images = array();
$img_str = '';
foreach ($images_results as $image_result) {
	if (isset($image_result->field_field_image[0]['raw']['is_default']) && $image_result->field_field_image[0]['raw']['is_default'] && isset($image_result->field_field_video[0]['rendered']['#item']['thumbnail'])) {
		$image = drupal_render($image_result->field_field_video[0]['rendered']);
	} else {
		$image = drupal_render($image_result->field_field_image[0]['rendered']);
	}
	$images[] = $image;
}
for ($i=0;$i<4;$i++) {
	$img = isset($images[$i]) ? $images[$i] : '';
	$img_str .= '<span class="small-img">'.$img.'</span>';
}

$title = $row->node_title;
$nid = $row->nid;
$body = $fields['body']->content;
$node_language = $row->node_language;

//$ugyouthportal_data['dsm_fields'] = array ( 'images' => $images_results, 'lesson-plan-nid'=>$lesson_plan_nid, );

$link_html = "
	<span class=\"index-item-wrapper clearfix series-plan-wrapper\">
		<span class=\"img\">$img_str</span>
		<span class=\"txt\">
			<span class=\"title\">$title</span>
			<span class=\"body\">$body</span>
		</span>
		<span class=\"bottom-border\"></span>
	</span>
";

$link_path = drupal_get_path_alias('view/'.$path.'/'.$nid, $node_language);
$link = '<a href="/'.$link_path.'" title="'.$title.'">'.$link_html.'</a>';

echo $link;
