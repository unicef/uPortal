<?php

//dsm(get_defined_vars());

$video_poster = false;
$video_path = false;
$video_mime = false;
$theme_path = path_to_theme();

if ($field_image[0]['is_default'] && isset($field_video[0]['thumbnailfile']->uri)) {
	$video_poster = image_style_url('content_480_250', $field_video[0]['thumbnailfile']->uri);
} else {
	$video_poster = image_style_url('content_480_250', $field_image[0]['uri']);
}

if (count($field_video)) {
	$video_path = file_create_url($field_video[0]['uri']);
	$video_mime = $field_video[0]['filemime'];
}
$body_content = $body[0]['safe_value'];
$provider_block = views_embed_view('providers', 'block_0', $field_content_provider_reference[0]['target_id']);
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

$link_download = '<a class="download btn btn-icon" href="'.$video_path.'" title="Download this video" id="download-video">Download</a>';

$meta_info = _ugyouthportal_get_content_meta_info($content, $node);

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
				<div class="video-wrapper">
					<video width="480" height="250" id="video-player" poster="<?php print $video_poster ?>" controls="controls" preload="none" autoplay="true'>
						<source type="<?php print $video_mime ?>" src="<?php print $video_path ?>" />
						<!-- Fallback flash player for no-HTML5 browsers with JavaScript turned off -->
						<object width="480" height="250" type="application/x-shockwave-flash" data="<?php print $theme_path.'/mediaelement.js/flashmediaelement.swf' ?>"> 		
							<param name="movie" value="<?php print $theme_path.'/mediaelement.js/flashmediaelement.swf' ?>" /> 
							<param name="flashvars" value="controls=true&poster=<?php print $video_poster ?>&file=<?php print $video_path ?>" /> 		
							<!-- Image fall back for non-HTML5 browser with JavaScript turned off and no Flash player installed -->
							<?php //print theme_image(); ?>
						</object> 	
					</video>
				</div>
				
				<?php if ($meta_info['prev_btn'] || $meta_info['next_btn']): ?>
				<div id="series-nav-btns">
					<?php print $meta_info['prev_btn'].$meta_info['next_btn'] ?>
				</div>
				<?php endif; ?>
				
			</div>
			
			<div class="content-desc">
				<h2 class="about">About this Video</h2>
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
