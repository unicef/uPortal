<?php

$video_poster = false;
$audio_path = false;
$audio_mime = false;
$theme_path = path_to_theme();
if (count($field_image)) {
	$video_poster = image_style_url('content_480_250', $field_image[0]['uri']);
}
if (count($field_audio_track)) {
	$audio_path = file_create_url($field_audio_track[0]['uri']);
	drupal_add_js(
		array(
			'ugyouthportal' => array(
				'audio_path' => $audio_path,
			)
		),
	'setting'
	);
}
$body_content = isset($body[0]['safe_value']) ? $body[0]['safe_value'] : '';;
$provider_block = isset($field_content_provider_reference[0]['target_id']) ? views_embed_view('providers', 'block_0', $field_content_provider_reference[0]['target_id']) : '';
$topic_title_block = views_embed_view('navigation', 'block_2', $field_topic_reference[0]['target_id']);

$query = array( 'tok' => drupal_get_token('recommend_content' . $node->nid),) + drupal_get_destination();
$link_share = l(
	'Promote to Home Page',
	'uportal/recommend-content/nojs/'.$node->nid,
	array(
		'attributes' => array (
			'title'	=>	'Recommend this content',
			'class' => array('share', 'btn', 'btn-icon', 'use-ajax',),
			'id' => 'recommend-btn',
		),
		'query' => $query,
		'html'	=> false,
	)
);
$link_download = '<a class="download btn btn-icon" href="'.$audio_path.'" title="Download this audio track" id="download-audio">Download</a>';

$meta_info = _ugyouthportal_get_content_meta_info($content, $node);

$wave_orange = render($content['field_wave_form_orange']);
$wave_grey = render($content['field_wave_form_grey']);

dsm(get_defined_vars());

?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
	
	<div class="content-desc-title-wrapper">
		
		<div class="content-titles clearfix">
			<div class="h1-provider">
				<h1><?php print $title ?></h1>
				<?php print $provider_block ?>
			</div>
			<?php print $topic_title_block ?>
		</div>
	
		<div class="content-desc-wrapper clearfix">
			
			<div class="content-player">
				<div id="audio-player-jplayer"></div>
				<div id="audio-player" class="clearfix">
					<div class="button-wrapper">
						<div id="btn">
							<a href="javascript:;" class="jp-play" tabindex="1" title="play">play</a>
							<a href="javascript:;" class="jp-pause" tabindex="1" title="pause">pause</a>
						</div>
						<div id="running-time" class="jp-current-time">00:00 mins</div>
					</div>
					<div class="progress-wrapper">
						<div class="wave-form"><div class="waves-wrapper">
							<?php print $wave_grey.$wave_orange ?>
						</div></div>
						<div class="progress-time-vol">
							<div id="progress">
								<div class="jp-progress">
									<div class="jp-seek-bar">
										<div class="jp-play-bar"><div class="scrubber"></div></div>
									</div>
								</div>
							</div>
							<div class="time-volume clearfix">
								<div id="volume" class="clearfix">
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
								<div class="total-time">04:23 mins</div>
							</div>
						</div>
					</div>
				</div>
				
				<?php if ($meta_info['prev_btn'] || $meta_info['next_btn']): ?>
				<div id="series-nav-btns">
					<?php print $meta_info['prev_btn'].$meta_info['next_btn'] ?>
				</div>
				<?php endif; ?>
				
			</div>
			
			<div class="content-desc">
				<h2 class="about">About this Audio Track</h2>
				<div class="body">
					<?php print $body_content ?>
				</div>
				<div class="links clearfix">
					<?php print $link_share.$link_download ?>
				</div>
			</div>
			
		</div>
	
	</div>
	
	<div id="content-meta-info">
		<?php print $meta_info['content']; ?>
	</div>

</div>

<div id="recommend-processing" class="block-ui-msg" style="display: none;"><div class="img"></div><div class="msg">Please wait as we process your recommendation ...</div></div>
<div id="recommend-finished" class="block-ui-msg" style="display: none;"><div class="msg">Thank you for your recommending this content. It has been shared on the home page.</div><div class="link"><a href="#" class="btn unblock-ui">Click to close this message</a></div>
