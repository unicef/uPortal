<?php

/***
 * implement hook_block_info
 * 	-	list blocks
 */
function uportal_backend_block_info() {
	return array(
		'uploader' => array (
			'info'	=>	t('Uploader Block'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_search_box' => array (
			'info'	=>	t('Content Listing - Search Box'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_status' => array (
			'info'	=>	t('Content Listing - Filters - Status'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_category' => array (
			'info'	=>	t('Content Listing - Filters - Categories'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_series' => array (
			'info'	=>	t('Content Listing - Filters - Series'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_plans' => array (
			'info'	=>	t('Content Listing - Filters - Lesson Plans'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_types' => array (
			'info'	=>	t('Content Listing - Filters - Types'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filters_editors' => array (
			'info'	=>	t('Content Listing - Filters - Editors'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filter_providers' => array (
			'info'	=>	t('Content Listing - Filters - Providers'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filter_languages' => array (
			'info'	=>	t('Content Listing - Filters - Languages'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'content_listing_filter_tags' => array (
			'info'	=>	t('Content Listing - Filters - Tags'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'header_manager_menu' => array (
			'info'	=>	t('Header Manager Menu'),
			'cache'	=>	DRUPAL_NO_CACHE,
		)
	);
}
/** ENDS **************************************************/


/**
 * implement hook_block_view
 * 	-	return block content
 */
function uportal_backend_block_view($delta='') {
	
	module_load_include('content_listing.inc', 'uportal_backend');
	
	$block = array();
	
	switch($delta) {
		case 'uploader':
			$block['subject'] = t('');
			$block['content'] = _uportal_uploader_block();
			break;
		case 'content_listing_search_box':
			$block['subject'] = '';
			$block['content'] = drupal_get_form('uportal_content_listing_search_form');
			break;
		case 'content_listing_filters_status':
			$block['subject'] = t('Status');
			$block['content'] = _uportal_content_listing_filters_status();
			break;
		case 'content_listing_filters_category':
			$block['subject'] = t('Categories');
			$block['content'] = _uportal_content_listing_filters_categories();
			break;
		case 'content_listing_filters_types':
			$block['subject'] = t('Media Types');
			$block['content'] = _uportal_content_listing_filters_types();
			break;
		case 'content_listing_filters_series':
			$block['subject'] = t('Series');
			$block['content'] = _uportal_content_listing_filters_series();
			break;
		case 'content_listing_filters_plans':
			$block['subject'] = t('Lesson Plans');
			$block['content'] = _uportal_content_listing_filters_lesson_plans();
			break;
		case 'content_listing_filters_editors':
			$block['subject'] = t('Editors');
			$block['content'] = _uportal_content_listing_filters_editors();
			break;
		case 'content_listing_filter_providers':
			$block['subject'] = t('Content Providers');
			$block['content'] = _uportal_content_listing_filters_providers();
			break;
		case 'content_listing_filter_languages':
			$block['subject'] = t('Languages');
			$block['content'] = _uportal_content_listing_filters_languages();
			break;
		case 'content_listing_filter_tags':
			$block['subject'] = t('Tags');
			$block['content'] = _uportal_content_listing_filters_tags();
			break;
		case 'header_manager_menu':
			$block['subject'] = '';
			$block['content'] = _uportal_backend_block_header_manager_menu();
			break;
	}
	
	return $block;
}
/** ENDS **************************************************/


/**
 * block content for the uploader block
 */
function _uportal_uploader_block() {
	
	//get default settings
	$file_field_settings = _uportal_default_filefield_settings();
	$file_upload_settings = array(
		'audio' => array (
			'extensions' => array(),
			'max_size' => 0,
		),
		'video' => array (
			'extensions' => array(),
			'max_size' => 0,
		),
		'document' => array (
			'extensions' => array(),
			'max_size' => 0,
		),
	);
	
	//get upload guidelines
	$guidelines_node = node_load(293);
	$guidelines = $guidelines_node->body['und'][0]['safe_value'];
	
	//video details
	$video_extensions = array();
	foreach ($file_field_settings['video']['extensions'] as $ext) {
		$video_extensions[] = '*.'.$ext;
		$file_upload_settings['video']['extensions'][] = $ext;
	}
	$guidelines = str_replace('[file_formats_video]', implode(', ', $video_extensions), $guidelines);
	$guidelines = str_replace('[max_file_size_video]', _uportal_format_bytes($file_field_settings['video']['max-file-size']), $guidelines);
	$file_upload_settings['video']['max_size'] = $file_field_settings['video']['max-file-size'];
	
	//audio details
	$audio_extensions = array();
	foreach ($file_field_settings['audio']['extensions'] as $ext) {
		$audio_extensions[] = '*.'.$ext;
		$file_upload_settings['audio']['extensions'][] = $ext;
	}
	$guidelines = str_replace('[file_formats_audio]', implode(', ', $audio_extensions), $guidelines);
	$guidelines = str_replace('[max_file_size_audio]', _uportal_format_bytes($file_field_settings['audio']['max-file-size']), $guidelines);
	$file_upload_settings['audio']['max_size'] = $file_field_settings['audio']['max-file-size'];
	
	//document details
	$doc_extensions = array();
	foreach ($file_field_settings['document']['extensions'] as $ext) {
		$doc_extensions[] = '*.'.$ext;
		$file_upload_settings['document']['extensions'][] = $ext;
	}
	$guidelines = str_replace('[file_formats_document]', implode(', ', $doc_extensions), $guidelines);
	$guidelines = str_replace('[max_file_size_document]', _uportal_format_bytes($file_field_settings['document']['max-file-size']), $guidelines);
	$file_upload_settings['document']['max_size'] = $file_field_settings['document']['max-file-size'];
	
	//uploader input field
	$allowed_mime_types = implode(',', array_merge($file_field_settings['video']['mime-types'], $file_field_settings['audio']['mime-types'], $file_field_settings['document']['mime-types']));
	$input_field = '<input id="fileupload" type="file" name="files[]" multiple accept="'.$allowed_mime_types.'">';
	
	$html_str = '<div class="uploader-title-content">';
	$html_str .= '<div class="title-bar"></div>';
	$html_str .= '<div class="clearfix files-upload-guidelines">';
	$html_str .= '<div id="uploader-progress" data-file-upload-settings=\''.json_encode($file_upload_settings).'\'>'.$input_field.'</div>';
	$html_str .= '<div class="uploader-guidelines">'.$guidelines.'</div>';
	$html_str .= '</div></div>';
	$html_str .= '<div class="uploader-footer"><div class="centred-strip clearfix">';
	$html_str .= '<div class="cancel-button btn">Cancel</div>';
	$html_str .= '<div class="edit-content-button btn disabled">Edit Content</div>';
	$html_str .= '</div></div>';
	
	return $html_str;
}
/** ENDS **************************************************/


/**
 * return header manager menu block
 */
function _uportal_backend_block_header_manager_menu() {
	$block_html_str = '';
	
	//logo to front end
	$block_html_str .= '<div class="logo"><a href="/" title="Back to frontend">&nbsp;</a></div>';
	
	//dashboard link
	$block_html_str .= '<div class="dashboard-link"><a href="/management/dashboard" title="Back to dashboard">Management Dashboard</a></div>';
	
	//quick links
	$block_html_str .= _uportal_backend_block_management_quick_links();
	
	//add new theme link
	$path = false;
	$menu_object = menu_get_object();
	if (drupal_is_front_page()) {
		$path = url('<front>', array(
			'query' => array (
				'new-theme' => 1,
			),
		));
	}
	if (isset($menu_object->type) && $menu_object->type=='video') {
		$path = url(current_path(), array(
			'query' => array (
				'new-theme' => 1,
			),
		));
	}
	if ($path) {
		//$block_html_str .= '<div class="try-new-theme"><a href="'.$path.'" title="Try out the new theme">Try the New Theme</a></div>';
	}
	
	//user menu
	$block_html_str .= _uportal_backend_block_user_links();
	
	return array (
		'#markup' => $block_html_str,
		'#attached' => array (
			'css' => array(
				drupal_get_path('module', 'uportal_backend').'/css/uportal-management-menu.css',
			),
		),
	);

}
/** ENDS *****************************************/


/**
 * create user html string
 */
function _uportal_backend_block_user_links() {
	global $user;
	
	//user link
	$user_link = l($user->name.' Profile', 'user/'.$user->uid, array(
  	'attributes' => array (
  		'title'	=>	'Check user profile and preferences',
  	),
  	'html'	=> false,
  ));
	$logout_link = l('Log Out', 'user/logout', array(
  	'attributes' => array (
  		'title'	=>	'Logout',
  	),
  	'html'	=> false,
  ));
	
	$user_html = '<div id="user" class="user clearfix">';
	$user_html .= '<div class="name">'.$user->name.'</div>';
	$user_html .= '<ul class="user-menu">';
	$user_html .= '<li class="user-link">'.$user_link.'</li>';
	$user_html .= '<li class="logout-link">'.$logout_link.'</li>';
	$user_html .= '</ul>';
	$user_html .= '</div>';
	
	return $user_html;
}
/** ENDS *****************************************/


/**
 * create quick links
 */
function _uportal_backend_block_management_quick_links() {
	$quick_links_html = '<div class="links-title">Quick Links</div>';
	$quick_links_html .= '<ul class="quick-links-menu">';
	
	$quick_links_html .= '<li class="first"><a href="/management/content-listing/individual-items">Manage Content</a></li>';
	$quick_links_html .= '<li><a href="/management/metadata/categories">Manage Categories</a></li>';
	$quick_links_html .= '<li><a href="/management/metadata/content-providers">Manage Content Providers</a></li>';
	$quick_links_html .= '<li><a href="/management/metadata/languages">Manage Languages</a></li>';
	$quick_links_html .= '<li><a href="/management/metadata/tags">Manage Tags</a></li>';
	$quick_links_html .= '<li class="last"><a href="/management/users">Manage Users</a></li>';
	
	$quick_links_html .= '</ul>';
	
	return '<div class="quick-links-wrapper">'.$quick_links_html.'</div>';
}
/** ENDS *****************************************/
