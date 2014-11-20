<?php

//print kpr($node);

$pdf_path = false;
$download_path = false;
$theme_path = path_to_theme();

if (count($field_pdf_file)) {
	$pdf_path = file_create_url($field_pdf_file[0]['uri']);
	$download_path = $node_url.'/download';
}

$body_content = (count($body)) ? $body[0]['safe_value'] : '';
$provider_block = (isset($field_content_provider_reference[0])) ? views_embed_view('providers', 'block_0', $field_content_provider_reference[0]['target_id']) : '';
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
		//'query' => $query,
		'html'	=> false,
	)
);

$link_download = '<a class="download btn btn-icon" href="'.$download_path.'" title="Download this video" id="download-video">Download</a>';

$meta_info = _ugyouthportal_get_content_meta_info($content, $node);

$languages = locale_language_list('name');

//dsm(get_defined_vars());
//dsm($node);

?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
	
	<div class="content-desc-title-wrapper">
		
		<div class="content-titles clearfix">
			<div class="h1-provider">
				<h1><?php print $title ?></h1>
				<?php print $provider_block ?>
			</div>
			<div class="language">
				<?php print $languages[$language] ?>
			</div>
			<?php //print $topic_title_block ?>
		</div>
	
		<div class="content-desc-wrapper clearfix">
			
			<div class="content-player">
				
				<?php print render($content['field_image']) ?>
				
				<?php if ($meta_info['prev_btn'] || $meta_info['next_btn']): ?>
				<div id="series-nav-btns">
					<?php print $meta_info['prev_btn'].$meta_info['next_btn'] ?>
				</div>
				<?php endif; ?>
				
				<div class="document-icon play-icon"></div>
				
			</div>
			
			<div class="content-desc">
				<h2 class="about"><?php print t('About this Article') ?></h2>
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

<div id="pdf-reader">
	<?php print render($content['field_pdf_file']) ?>
	<div class="unblock-ui close-btn"></div>
</div>

<div id="recommend-processing" class="block-ui-msg" style="display: none;"><div class="img"></div><div class="msg">Please wait as we process your recommendation ...</div></div>
<div id="recommend-finished" class="block-ui-msg" style="display: none;"><div class="msg">Thank you. This content has been promoted on the home page.</div><div class="link"><a href="#" class="btn unblock-ui">OK</a></div>

