<?php

//dsm(get_defined_vars());
//print kpr($node);

$video_image = false;
$video_poster = false;
$video_path = false;
$download_path = false;
$video_mime = false;
$theme_path = path_to_theme();

if (isset($field_image[0]['is_default']) && $field_image[0]['is_default'] && isset($field_video[0]['thumbnailfile']->uri)) {
	$video_image = $field_video[0]['thumbnailfile']->uri;
	$video_poster = image_style_url('video_player_poster', $video_image);
} else {
	$video_image = $field_image[0]['uri'];
	$video_poster = image_style_url('video_player_poster', $video_image);
}

//create image backdrop for blur
$img_backdrop = image_style_url('content_image_backdrop', $video_image);

if (count($field_video)) {
	$video_path = file_create_url($field_video[0]['uri']);
	$download_path = $node_url.'/download';
	$video_mime = $field_video[0]['filemime'];
	$video_path_parts = explode('.', $video_path);
	$video_extension = $video_path_parts[count($video_path_parts)-1];
	drupal_add_js(
		array(
			'ugyouthportal' => array(
				'video_path' => $video_path,
				'video_extension' => 'm4v',
				'video_poster' => $video_poster,
			)
		),
	'setting'
	);
}

$body_content = isset($body[0]['safe_value']) ? $body[0]['safe_value'] : '';
//$provider_block = isset($field_content_provider_reference[0]['target_id']) ? views_embed_view('providers', 'block_0', $field_content_provider_reference[0]['target_id']) : '';
//$topic_title_block = isset($field_topic_reference[0]['target_id']) ? views_embed_view('navigation', 'block_2', $field_topic_reference[0]['target_id']) : '';

$query = array( 'tok' => drupal_get_token('recommend_content' . $node->nid),) + drupal_get_destination();
$link_share = l(
	'<span>Promote to Home Page</span>',
	'uportal/recommend-content/nojs/'.$node->nid,
	array(
		'attributes' => array (
			'title'	=>	'Recommend this content',
			'class' => array('share', 'btn', 'btn-icon', 'use-ajax',),
			'id' => 'recommend-btn',
		),
		//'query' => $query,
		'html'	=> true,
	)
);

$link_download = '<a class="download btn btn-icon" href="'.$download_path.'" title="Download this video" id="download-video"><span>Download</span></a>';


$meta_info = _uportal_new_theme_get_content_meta_info_video($content, $node);

$languages = locale_language_list('name');

$back_btn = '<div class="back-btn">'.l('<span></span>', '<front>', array('html'=>true,)).'</div>';
$comments_btn = '<a class="btn comments-btn" href="#comments-section"><span>'.$meta_info['comment-info']['comment_count'].' Comments</span></a>';

//dsm(get_defined_vars());

?>
	
<div class="content-links">
	<div class="centred-strip clearfix">
		<div class="download-recommend-comment-btns clearfix">
			<?php print $link_download.$link_share.$comments_btn ?>
		</div>
		<?php print $back_btn ?>
	</div>
</div>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
	
	<div class="video-player-outer-wrapper clearfix">
		<div class="img-backdrop" style="background-image: url('<?php print $img_backdrop ?>');"></div>
		<div class="video-player-wrapper">
			<div class="video-wrapper">
				
				<!-- video player ------>
				<div class="video-player-real-wrapper" id="video-player-real-wrapper">
					<div id="video-player-jplayer"></div>
					<div id="fullscreen"><div></div></div>
					<div id="series-playlist" class="jcarousel-wrapper">
						<div class="series-playlist-wrapper jcarousel">
							<?php print $meta_info['series-info']['small-playlist-html'] ?>
						</div>
						<a href="#" class="prev-btn"><span></span></a>
            <a href="#" class="next-btn"><span></span></a>
					</div>
					<div id="series-playlist-player">
						<div class="centred-strip">
							<div class="buttons">
								<?php print $link_download.$link_share ?>
							</div>
							<div class="playlist-videos">
								<h2>Next Video:</h2>
								<div class="playlist-wrapper jcarousel-wrapper">
									<div class="playlist jcarousel">
										<?php print $meta_info['series-info']['large-playlist-html'] ?>
									</div>
									<a href="#" class="prev-btn"><span></span></a>
									<a href="#" class="next-btn"><span></span></a>
								</div>
							</div>
						</div>
					</div>
					<div id="video-player" class="clearfix">
						<div class="video-controls">
							<div id="btn" class="float">
								<a href="javascript:;" class="jp-play" tabindex="1" title="play"><span>play</span></a>
								<a href="javascript:;" class="jp-pause" tabindex="1" title="pause"><span>pause</span></a>
							</div>
							<div id="running-time" class="jp-current-time float">00:00</div>
							<div id="progress" class="float">
								<div class="jp-progress">
									<div class="jp-seek-bar">
										<div class="jp-play-bar"><div class="scrubber"></div></div>
									</div>
								</div>
							</div>
							<div id="volume" class="clearfix float-right">
								<div class="volume-bar">
									<div class="jp-volume-bar">
										<div class="jp-volume-bar-value"></div>
									</div>
								</div>
							</div>
							<div id="duration" class="jp-duration float-right">00:00</div>
						</div>
					</div>
				</div>
				<!-- video player ends here ------>
				
				<div class="content-info">
					<div class="clearfix content-info-title">
						<div class="node-series-category-title"><?php print $meta_info['node-series-category-title'] ?></div>
						<h1><?php print $title ?></h1>
					</div>
					<div class="body">
						<?php print $body_content ?>
					</div>
				</div>
			</div>
		</div>
	</div>
	
	<div class="node-meta-info">
		
		<div class="related-series-content">
			<h2><?php print $meta_info['series-info']['series-title'] ?></h2>
			<div class="related-series-content-body">
				<?php print $meta_info['series-info']['series-list-html'] ?>
			</div>
		</div>
		
		<div class="comments-wrapper" id="comments-section">
			<h2><?php print '<strong>'.$meta_info['comment-info']['comment_count'].'</strong> comments' ?></h2>
			<?php print $meta_info['comment-info']['comments_html'] ?>
		</div>
	
	</div>


</div>


<div id="recommend-processing" class="block-ui-msg" style="display: none;"><div class="img"></div><div class="msg">Please wait as we process your recommendation ...</div></div>
<div id="recommend-finished" class="block-ui-msg" style="display: none;"><div class="msg">Thank you. This content has been promoted on the home page.</div><div class="link"><a href="#" class="btn unblock-ui">OK</a></div>
