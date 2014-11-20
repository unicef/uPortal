<?php

//dsm($fields);

//global $ugyouthportal_data;
//$ugyouthportal_data['dsm_fields'] = $row;

$content_type = $fields['type']->content;
$img = $fields['field_image']->content;
$duration = $fields['field_file_size_duration']->content;
if (isset($row->field_field_image[0]['raw']['is_default']) && $row->field_field_image[0]['raw']['is_default'] && $content_type=='video' && isset($fields['field_video']->content) && $fields['field_video']->content) {
	$img = $fields['field_video']->content;
}

?>

<span class="listing-item-wrapper <?php print 'type-'.$content_type ?>">
	<span class="topic-language clearfix">
		<span class="topic clearfix">
			<span class="icon"><?php print $fields['field_icon_2']->content ?></span>
			<span class="name"><?php print $fields['title_1']->content ?></span>
		</span>
		<span class="language"><?php print $fields['language']->content  ?></span>
	</span>
	<a class="listing-page" href="<?php print $fields['path']->content ?>" title="Click to view: <?php print $fields['title']->content ?>">
		<span class="wrapper">
			<span class="img">
				<?php print $img ?>
				<span class="type-icon"></span>
			</span>
			<span class="title"><?php print $fields['title']->content ?></span>
			<span class="duration-comments clearfix">
				<span class="duration"><?php print $duration ?></span>
				<span class="comments">
					<span class="comment-count"><?php print $fields['comment_count']->content ?></span>
					<span class="comment-icon"></span>
					<span class="new"></span>
				</span>
			</span>
		</span>
	</a>
</span>
