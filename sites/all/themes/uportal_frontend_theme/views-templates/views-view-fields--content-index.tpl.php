<?php

//dsm($fields);

//global $ugyouthportal_data;
//$ugyouthportal_data['dsm_fields'] = get_defined_vars();

$content_type = $row->node_type;

$img = $fields['field_image']->content;
if (
	isset($row->field_field_image[0]['raw']['is_default'])
	&& $row->field_field_image[0]['raw']['is_default']
	&& $content_type=='video'
	&& isset($fields['field_video']->content)
	&& $fields['field_video']->content
) {
	$img = $fields['field_video']->content;
}

$title = $row->node_title;
$nid = $row->nid;
$body = $fields['body']->content;
$node_language = $row->node_language;

$link_html = "
	<span class=\"index-item-wrapper clearfix type-$content_type\">
		<span class=\"img\">$img<span class=\"icon\"></span></span>
		<span class=\"txt\">
			<span class=\"title\">$title</span>
			<span class=\"body\">$body</span>
		</span>
		<span class=\"bottom-border\"></span>
	</span>
";

$link_path = drupal_get_path_alias('node/'.$nid, $node_language);
$link = '<a href="/'.$link_path.'" title="'.$title.'">'.$link_html.'</a>';

echo $link;
