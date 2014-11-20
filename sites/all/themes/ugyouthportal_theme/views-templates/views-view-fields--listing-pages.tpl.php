<?php

//dsm($fields);

global $ugyouthportal_data;
//$ugyouthportal_data['dsm_fields'] = get_defined_vars();
//$ugyouthportal_data['dsm_fields']['row'] = $row;

$content_type = $fields['type']->content;
$img = $fields['field_image']->content;
$duration = $fields['field_file_size_duration']->content;
if (isset($row->field_field_image[0]['raw']['is_default']) && $row->field_field_image[0]['raw']['is_default'] && $content_type=='video' && isset($fields['field_video']->content) && $fields['field_video']->content) {
	$img = $fields['field_video']->content;
}

$enabled_languages = language_list();
$title = $row->node_title;
$nid = $row->nid;
$node_language = $row->node_language;
$language_object = new stdClass();
if (isset($enabled_languages[$node_language])) {
  $language_object = $enabled_languages[$node_language];
}

//$ugyouthportal_data['dsm_fields']['enabled-languages'] = $enabled_languages;
//$ugyouthportal_data['dsm_fields']['language_object'] = $language_object;

$link_html = "
	<span class=\"wrapper\">
		<span class=\"img\">
			$img
			<span class=\"type-icon\"></span>
		</span>
		<span class=\"title\">".$fields['title']->content."</span>
		<span class=\"duration-comments clearfix\">
			<span class=\"duration\">$duration</span>
			<span class=\"comments\">
				<span class=\"comment-count\">".$fields['comment_count']->content."</span>
				<span class=\"comment-icon\"></span>
				<span class=\"new\"></span>
			</span>
		</span>
	</span>
";

$link_path = drupal_get_path_alias('node/'.$nid, $node_language);
$link = '<a href="/'.$link_path.'" title="'.$title.'" class="listing-page">'.$link_html.'</a>';


?>

<span class="listing-item-wrapper <?php print 'type-'.$content_type ?>">
	<span class="topic-language clearfix">
		<span class="topic clearfix">
			<span class="icon"><?php print $fields['field_category_icon']->content ?></span>
			<span class="name"><?php print $fields['title_1']->content ?></span>
		</span>
		<span class="language"><?php print $fields['language']->content  ?></span>
	</span>
	<?php print $link ?>
</span>
