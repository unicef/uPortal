<?php

//add NID attribute to button
$nid = $form['nid']['#value'];
$node = $form['#entity'];
$audio_file_obj = $form['field_audio_track']['und'][0]['#file'];

$audio_path = file_create_url($audio_file_obj->uri);
$audio_path_parts = explode('/', $audio_path);
$audio_filename = $audio_path_parts[count($audio_path_parts)-1];
$audio_filesize = _uportal_format_bytes($audio_file_obj->filesize, 0);
$audio_duration = $form['field_file_size_duration']['und'][0]['value']['#value'];

?>

<div id="audio-file-wrapper-<?php print $nid ?>" class="audio-file preview-file content-file-wrapper" data-audio-path="<?php print $audio_path ?>" data-size="" data-duration="<?php print $audio_duration ?>">
	
	<label>Audio Preview</label>
	<div id="audio-player-<?php print $nid ?>" class="audio-player-jplayer"></div>
	<div class="audio-player player clearfix">
		<div class="button-wrapper">
			<div class="btn">
				<a href="javascript:;" class="jp-play" tabindex="1" title="play">play</a>
				<a href="javascript:;" class="jp-pause" tabindex="1" title="pause">pause</a>
			</div>
			<div class="running-time jp-current-time">00:00 mins</div>
		</div>
		<div class="progress-full-wrapper">
			<div class="wave-form"><div class="waves-wrapper">
				<div class="wave-img-orange wave-img"></div>
				<div class="wave-img-grey wave-img"></div>
			</div></div>
			<div class="progress-time-vol">
				<div class="progress-wrapper">
					<div class="jp-progress">
						<div class="jp-seek-bar">
							<div class="jp-play-bar"><div class="scrubber"></div></div>
						</div>
					</div>
				</div>
				<div class="time-volume clearfix">
					<div class="volume clearfix">
						<div class="mute-unmute">
							<a href="javascript:;" class="jp-mute" tabindex="1" title="mute">mute</a>
							<a href="javascript:;" class="jp-unmute" tabindex="1" title="unmute">unmute</a>
						</div>
						<div class="volume-bar">
							<div class="jp-volume-bar">
								<div class="jp-volume-bar-value"></div>
							</div>
						</div>
						<div clas="max-volume">
							<a href="javascript:;" class="jp-volume-max" tabindex="1" title="max volume">max volume</a>
						</div>
					</div>
					<div class="total-time"><?php print $audio_duration ?></div>
				</div>
			</div>
		</div>
		<div class="content-upload-progress wrapper-upload-progress audio-upload-progress">
			<div class="loader-indicator"></div>
			<div class="progress-bar"><div></div></div>
			<div class="error-msg"></div>
		</div>
	</div>
	

	<div class="filename"><?php print $audio_filename ?></div>
	<div class="filesize-replace clearfix">
		<div class="filesize"><?php print $audio_filesize ?></div>
		<div class="replace"><a href="#" class="replace-file">Replace</a> this file.</div>
	</div>
	
	<div class="original-audio-form-field original-content-file-field">
		<?php print drupal_render($form['field_audio']); ?>
	</div>
	
	
</div>

