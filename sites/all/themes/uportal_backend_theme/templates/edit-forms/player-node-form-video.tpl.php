<?php

//add NID attribute to button
$nid = $form['nid']['#value'];

//build video player
$field_image = $form['field_image']['und'][0]['#default_value'];
$field_video = $form['field_video']['und'][0]['#default_value'];
$use_img = false;
$img_thumbnail = false;
if (((isset($field_image['is_default']) && $field_image['is_default']) || $field_image['fid']==0) && isset($field_video['thumbnailfile']->uri)) {
	$use_img = $field_video['thumbnailfile']->uri;
} elseif (isset($field_image['uri']) && $field_image['uri']) {
	$use_img = $field_image['uri'];
}

if ($use_img) {
	$video_poster = image_style_url('management_video_player', $use_img);
	$img_thumbnail = theme_image_style(array(
		'style_name' => 'management_edit_content_thumbnail',
		'path' => $use_img,
	));
}
$video_mime = $field_video['filemime'];
$video_path = file_create_url($field_video['uri']);
$video_path_parts = explode('/', $video_path);
$video_filename = $video_path_parts[count($video_path_parts)-1];
$video_filesize = _uportal_format_bytes($field_video['filesize'], 0);

?>

<div class="video-file content-file-wrapper">
	
	<label>Video preview</label>
	
	<div class="video-player player">
		<video width="285" height="160" id="video-player-<?php print $nid ?>" poster="<?php print $video_poster ?>" controls="controls" preload="metadata">
			<source type="<?php print $video_mime ?>" src="<?php print $video_path ?>" />
		</video>
		<div class="content-upload-progress wrapper-upload-progress video-upload-progress">
			<div class="loader-indicator"></div>
			<div class="progress-bar"><div></div></div>
			<div class="error-msg"></div>
		</div>
	</div>
	<div class="filename"><?php print $video_filename ?></div>
	<div class="filesize-replace clearfix">
		<div class="filesize"><?php print $video_filesize ?></div>
		<div class="replace"><a href="#" class="replace-file">Replace</a> this file.</div>
	</div>
	
	<div class="original-video-form-field original-content-file-field">
		<?php print drupal_render($form['field_video']); ?>
	</div>
	
</div>
