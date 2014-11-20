<?php

//add NID attribute to button
$nid = $form['nid']['#value'];
$node = $form['#entity'];
$form['actions']['custom-submit-'.$nid]['#attributes']['data-nid'] = $nid;
$form['actions']['custom-submit-'.$nid]['#attributes']['class'][] = 'save-form-btn';

//add CHOSEN placeholder to lesson plan field
$form['field_lesson_plan_reference']['und']['#attributes']['data-placeholder'] = 'Select lesson plan(s) ...';

//tags field - add CHOSEN placeholder and select class
$form['field_all_vocabularies_reference']['und']['#attributes']['data-placeholder'] = 'Select descriptive tag(s) ...';
$form['field_all_vocabularies_reference']['und']['#attributes']['class'] = array (
	'custom-all-tags-select',
);

//get rid of HTML special characters in dropdowns
$dropdown_keys = array(
	'field_topic_reference',
	'field_series_reference',
	'field_content_provider_reference',
	'field_lesson_plan_reference',
);
foreach ($dropdown_keys as $dropdown_key) {
	$options = $form[$dropdown_key]['und']['#options'];
	foreach ($options as $key => $name) {
		$form[$dropdown_key]['und']['#options'][$key] = htmlspecialchars_decode($name, ENT_QUOTES);
	}
}

//get image to work with
$field_image = $form['field_image']['und'][0]['#default_value'];
$use_img = (isset($field_image['uri']) && $field_image['uri']) ? $field_image['uri'] : false;
$img_thumbnail = false;

if (isset($form['field_video']) && count($form['field_video']['und'])) {
	$field_video = $form['field_video']['und'][0]['#default_value'];
	if (((isset($field_image['is_default']) && $field_image['is_default']) || $field_image['fid']==0) && isset($field_video['thumbnailfile']->uri)) {
		$use_img = $field_video['thumbnailfile']->uri;
	}
}

if ($use_img) {
	$img_thumbnail = theme_image_style(array(
		'style_name' => 'management_edit_content_thumbnail',
		'path' => $use_img,
	));
}

//change labels
$form['title']['#title'] = 'Title <span class="publish-required">(Required)</span>';
$form['body']['und'][0]['value']['#title'] = 'Description <span class="publish-required">(Required)</span>';
$form['field_topic_reference']['und']['#title'] = 'Category <span class="publish-required">(Required)</span>';
$form['field_series_reference']['und']['#title'] = 'Series';
$form['field_lesson_plan_reference']['und']['#title'] = 'Lesson Plans';
$form['field_lesson_plan_reference']['und']['#description'] = 'You may attach this content to one or more lesson plans.';
$form['field_topic_reference']['und']['#title'] = 'Category <span class="publish-required">(Required)</span>';
$form['field_content_provider_reference']['und']['#title'] = 'Content Provider <span class="publish-required">(Required)</span>';
$form['language']['#title'] = 'Language <span class="publish-required">(Required)</span>';

//get player HTML
$template_path_row = drupal_get_path('theme', 'uportal_backend_theme').'/templates/edit-forms/player-node-form-'.$node->type.'.tpl.php';
$content_player_html = _uportal_backend_load_view($template_path_row, array('form'=>$form));

//get allowed file types
$file_field_settings = _uportal_default_filefield_settings();
$allowed_mime_types = $file_field_settings[$node->type]['mime-types'];
$allowed_file_types = implode(',', $allowed_mime_types);

//get allowed file types for thumbnail
$thumbnail_file_field_settings = _uportal_field_settings_given_field_and_type('field_image', $node->type);
$thumbnail_allowed_mime_types = implode(',', $thumbnail_file_field_settings['mime-types']);
$thumbnail_file_field_info = array();
foreach ($thumbnail_file_field_settings['extensions'] as $ext) {
	$thumbnail_file_field_info[$ext] = array (
		'type' => 'image',
		'max_file_size' => $thumbnail_file_field_settings['max-file-size'],
	);
}

//any user accessing this form has the EDIT CONTENT permission and they should be able to PUBLISH/UNPUBLISH even if they do not have the ADMINISTER NODES permission
$form['options']['status']['#access'] = true;

?>

<div class="node-edit-form-wrapper clearfix">
	
	<div class="node-edit-col-1">
		<?php print $content_player_html; ?>
		<div class="thumbnail">
			<label>Thumbnail</label>
			<div class="thumbnail-images clearfix">
				<div class="current-thumbnail">
					<div class="img">
						<?php print $img_thumbnail; ?>
					</div>
					<div class="thumbnail-upload-progress wrapper-upload-progress">
						<div class="loader-indicator"></div>
						<div class="progress-bar"><div></div></div>
						<div class="error-msg"></div>
					</div>
				</div>
				<div class="upload-thumbnail">Upload a thumbnail</div>
			</div>
			<div class="original-thumbnail-field">
				<?php print drupal_render($form['field_image']); ?>
			</div>
		</div>
		<?php print drupal_render($form['field_all_vocabularies_reference']); ?>
	</div>
	
	<div class="node-edit-col-2">
		<?php print drupal_render($form['title']); ?>
		<?php print drupal_render($form['body']); ?>
		<div class="clearfix category-series-wrapper">
			<?php print drupal_render($form['field_topic_reference']); ?>
			<?php print drupal_render($form['field_series_reference']); ?>
		</div>
		<div class="category-series-desc">You must select a Category before selecting a Series.</div>
		<?php print drupal_render($form['field_content_provider_reference']); ?>
		<?php print drupal_render($form['language']); ?>
		<?php print drupal_render($form['field_lesson_plan_reference']); ?>
		<?php print drupal_render($form['options']['status']); ?>
	</div>
	
</div>

<div class="node-actions element-invisible">
	<input type="hidden" id="form-nid-value" value="<?php print $nid ?>">
	<?php print drupal_render($form['actions']); ?>
	<?php print drupal_render_children($form); ?>
</div>

<div class="temporary-data element-invisible">
	<input type="hidden" id="custom-node-status" name="custom_node_status" value="-1">
	<div class="thumbnail-info">
		<input type="hidden" name="new_thumbnail_fid" value="0" class="new-thumbnail-fid">
		<input type="file" name="new_thumbnail_upload" class="new-thumbnail"
			accept="<?php print $thumbnail_allowed_mime_types ?>"
			data-ays-ignore="true"
			data-file-upload-settings='<?php print json_encode($thumbnail_file_field_info) ?>'
		>
	</div>
	<div class="<?php print $node->type ?>-info content-file-info">
		<input type="hidden" name="new_content_file_fid" value="0" class="new-content-file-fid">
		<input type="file" name="new_content_file_upload" class="new-content-file" accept="<?php print $allowed_file_types ?>" data-ays-ignore="true">
	</div>
	<div class="series-ordered-info">
		<input type="hidden" name="series_ordered_nid" value="" class="series-ordered-nid">
		<input type="hidden" name="series_ordered_order" value="" class="series-ordered-order">
	</div>
</div>

<div class="saving-form-data">
	<div class="loader"></div>
	<div class="loader-text">Saving Form Data. Please wait ...</div>
</div>
