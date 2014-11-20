<?php

/**
 * content listing page callback
 */
function uportal_backend_content_listing() {
	//body classes and other globals
	global $uportal_backend_data;
	$uportal_backend_data['page_type'] = 'management_content_listing';
	$uportal_backend_data['body_classes'][] = 'page-management-content';
	$uportal_backend_data['body_classes'][] = 'page-management-content-'.arg(2);
	$uportal_backend_data['body_classes'][] = 'bulk-editor-active';
	$uportal_backend_data['body_classes'][] = 'upload-functionality';
	$uportal_backend_data['content-listing']['total-showing-results'] = 0;
	
	//titles
	$page_title = 'Manage Content';
	/*
	switch (arg(2)) {
		case 'series':
			$page_title = 'Series';
			break;
		case 'lesson-plans':
			$page_title = 'Lesson Plans';
			break;
	}
	*/
	drupal_set_title($page_title);
	
	//content
	return '';
}
/** ENDS *****************************************/


/**
 * build content listing page
 */
function _uportal_backend_content_listing_page_build(&$page) {
	$args = arg();
	
	//content blocks
	$page_content_blocks = array ();
	if ($args[2]=='individual-items') {
		$page_content_blocks[] = array('module'=>'views', 'delta'=>'bf750f2bdb36c2163705ec66e372dd30');
	}
	if ($args[2]=='series') {
		$page_content_blocks[] = array('module'=>'views', 'delta'=>'442004c03b1746f6074062dbbb2cb0a7');
	}
	if ($args[2]=='lesson-plans') {
		$page_content_blocks[] = array('module'=>'views', 'delta'=>'8106dc8714519d65d9799298504d43c5');
	}
	
	$page_blocks = array (
		'content' => $page_content_blocks,
		'sidebar_first' => array(
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_status'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_category'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_series'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_plans'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_types'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filter_languages'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filter_providers'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filters_editors'),
			array('module'=>'uportal_backend', 'delta'=>'content_listing_filter_tags'),
		),
	);
	
	//hide series and lesson plans filters for series and lesson plan sections
	$section_type = arg(2);
	if ($section_type=='lesson-plans' || $section_type=='series') {
		unset($page_blocks['sidebar_first'][2]);
		unset($page_blocks['sidebar_first'][3]);
	}
	
	foreach ($page_blocks as $region => $region_blocks_info) {
		$region_blocks = array();
		foreach ($region_blocks_info as $block_info) {
			$region_blocks[] = block_load($block_info['module'], $block_info['delta']);
		}
		$page[$region] = _block_get_renderable_array(_block_render_blocks($region_blocks));
	}
}
/** ENDS *****************************************/


/**
 * alter content listing page
 */
function _uportal_backend_content_listing_page_alter(&$page) {
	
	$i = 0;
	
	//markup header strip 2
	$page['header_strip_2'] = array (
		'sub-menu' => _uportal_backend_content_listing_page_sub_menu(),
		'settings-menu' => _uportal_backend_content_listing_settings_menu(),
	);
	
	//markup header strip 3
	$page['header_strip_3'] = array (
		'filters-markup' => array (
			'#markup' => _uportal_backend_content_listing_filter_settings(),
		),
	);
	
	//if no results, show empty text
	$content_array_keys = array_keys($page['content']);
	$content_is_empty = true;
	foreach ($content_array_keys as $key) {
		if (strpos($key, '#')!==0) {
			$content_is_empty = false;
		}
	}
	if ($content_is_empty) {
		$page['content']['#markup'] = '<div class="no-listing-results"><p class="text">Sorry, no content matches the current filters.</p><p class="link-btn"><a href="/management/content-listing/'.arg(2).'?killsearch=1" title="Reset Filters" class="btn">Clear Filters</a></p></div>';
	}
	
}
/** ENDS *****************************************/


/**
 * get filter settings
 */
function _uportal_backend_content_listing_filter_settings() {
	global $uportal_backend_data;
	$path_args = arg();
	$current_url = $_SERVER['REQUEST_URI'];
	$current_section = $path_args[2];
	$clear_filter_args = uportal_backend_content_listing_build_path( array (
		'content_section_type' => $current_section,
	));
	$view_mode = isset($_GET['collapsed-view']) ? '&collapsed-view=1' : '';
	$clear_filter_btn = '<a href="/management/content-listing/'.$current_section.'?killsearch=1'.$view_mode.'" title="Clear and reset all filter values" class="btn" id="clear-filters-btn">Clear filters</a>';
	
	//search form wrapper
	$search_form_block_content = '';
	if ($current_section=='individual-items') {
		$search_form_block = module_invoke('uportal_backend', 'block_view', 'content_listing_search_box');
		$search_form_block_content = '<div class="search-form-wrapper">'.drupal_render($search_form_block['content']).'</div>';
	}
	
	//get total number of results in system
	$query = false;
	$total_number_results_in_system = 0;
	$no_results_on_current_filter = $uportal_backend_data['content-listing']['total-showing-results'];
	switch ($current_section) {
		case 'individual-items':
			$query = "SELECT COUNT(nid) FROM node WHERE type IN ('video','audio','document')";
			break;
		case 'series':
			$query = "SELECT COUNT(nid) FROM node WHERE type IN ('series')";
			break;
		case 'lesson-plans':
			$query = "SELECT COUNT(nid) FROM node WHERE type IN ('lesson_plan')";
			break;
	}
	if ($query) {
		$total_number_results_in_system = number_format((int)db_query($query)->fetchField());
	}
	
	$markup = '
		<h2 class="filters-heading">Filter '.$total_number_results_in_system.' items:</h2>
		<div class="content-listing-operations no-items-selected">
			<div class="parent-checkbox">
				<input type="checkbox" name="all_nids" class="all-nid-checkbox" value="all">
			</div>
			<div class="edit-btn btn" data-current-url="'.$current_url.'"></div>
			<div class="delete-btn btn"></div>
			<div class="bulk-edit-btn btn">Bulk Edit</div>
			<div class="total-clear-link clearfix">
				<div class="clear-link">'.$clear_filter_btn.'</div>'
				.$search_form_block_content.
			'</div>
		</div>
	';
	return $markup;
}
/** ENDS *****************************************/


/**
 * get settings menu for content listing page
 */
function _uportal_backend_content_listing_settings_menu() {
	
	//create sorter menu
	$path_args = _uportal_backend_content_listing_get_current_args();
	$path_args['link_type'] = 'sorter';
	$current_path = drupal_parse_url(request_uri());
	$current_path['path'] = trim($current_path['path'], '/');
	$view_is_collapsed = isset($current_path['query']['collapsed-view']);
	$display_style_menu = '<ul class="options display-options">';
	$sorter_menu = '<ul class="options sort-options">';
	
	//expanded display link
	$li_class = 'expanded-view-link';
	$btn_expanded_options = array (
		'attributes' => array (
			'title' => 'Expanded View',
			'class' => array('expanded'),
		),
		'query' => $current_path['query'],
		'fragment' => $current_path['fragment'],
	);
	if ($view_is_collapsed) {
		unset($btn_expanded_options['query']['collapsed-view']);
	} else {
		$btn_expanded_options['attributes']['class'][] = 'active-view';
		$li_class .= ' active';
	}
	$btn_expanded = l('Expanded', $current_path['path'], $btn_expanded_options);
	$display_style_menu .= '<li class="'.$li_class.'">'.$btn_expanded.'</li>';
	
	//collapsed display link
	$li_class = 'collapsed-view-link';
	$btn_collapsed_options = array (
		'attributes' => array (
			'title' => 'Collapsed View',
			'class' => array('collapsed'),
		),
		'query' => $current_path['query'],
		'fragment' => $current_path['fragment'],
	);
	if ($view_is_collapsed) {
		$btn_collapsed_options['attributes']['class'][] = 'active-view';
		$li_class .= ' active';
	} else {
		$btn_collapsed_options['query']['collapsed-view'] = 1;
	}
	$btn_collapsed = l('Condensed', $current_path['path'], $btn_collapsed_options);
	$display_style_menu .= '<li class="'.$li_class.'">'.$btn_collapsed.'</li>';
	
	//close display style menu
	$display_style_menu .= '</ul>';
	
	//sort created link
	$sort_path = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'created')));
	$sorter_menu .= '<li class="sort-created '.$sort_path['class'].'"><a href="'.$sort_path['path'].'">Newest at top</a></li>';
	
	//sort recently modified link
	$sort_path = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'updated')));
	$sorter_menu .= '<li class="sort-updated '.$sort_path['class'].'"><a href="'.$sort_path['path'].'">Last modified</a></li>';
	
	//sort alphabetic a-z link
	$sort_path = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'alphabetic-a-z')));
	$sorter_menu .= '<li class="sort-alphabetic-a-z '.$sort_path['class'].'"><a href="'.$sort_path['path'].'">Alphabetic (A-Z)</a></li>';
	
	//sort alphabetic z-a link
	$sort_path = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'alphabetic-z-a')));
	$sorter_menu .= '<li class="sort-alphabetic-z-a '.$sort_path['class'].'"><a href="'.$sort_path['path'].'">Alphabetic (Z-A)</a></li>';
	
	//sort series
	if (isset($_GET['series_nid']) && is_numeric($_GET['series_nid'])) {
		$sort_path_series = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'series')));
		$sorter_menu .= '<li class="sort-series '.$sort_path_series['class'].'"><a href="'.$sort_path_series['path'].'">Series Order</a></li>';
	}
	
	//sort lesson plans
	if (isset($_GET['lesson_plan_nid']) && is_numeric($_GET['lesson_plan_nid'])) {
		$sort_path_lesson_plans = uportal_backend_content_listing_build_path(array_merge($path_args, array('order_by'=>'lesson-plan')));
		$sorter_menu .= '<li class="sort-lesson-plan '.$sort_path_lesson_plans['class'].'"><a href="'.$sort_path_lesson_plans['path'].'">Lesson Plan Order</a></li>';
	}
	
	$sorter_menu .= '</ul>';
	
	$settings_menu = array (
		'#markup' => '
			<div class="content-listing-settings">
				<div class="btn settings-btn"></div>
				<div class="settings-menu">
					<div class="arrow"></div>
					<div class="full-options-wrapper">
						<div class="view-style options-wrapper">
							<div class="title">Display mode:</div>
							'.$display_style_menu.'
						</div>
						<div class="sorter options-wrapper">
							<div class="title">Sort by:</div>
							'.$sorter_menu.'
						</div>
					</div>
				</div>
			</div>
		',
	);
	return $settings_menu;
}
/** ENDS *****************************************/


/**
 * get sub menu for content listing page
 */
function _uportal_backend_content_listing_page_sub_menu() {
	$path_args = arg();
	$active_section = $path_args[2];
	
	//individual items
	$sub_menu_individual_items_args = uportal_backend_content_listing_build_path(array (
		'link_type' => 'parent-sub-menu',
		'content_section_type' => 'individual-items',
	));
	$sub_menu_individual_items_link = '<a class="btn" href="/management/content-listing/individual-items">Individual Items</a>';
	
	$sub_menu_series_args = uportal_backend_content_listing_build_path(array (
		'link_type' => 'parent-sub-menu',
		'content_section_type' => 'series',
	));
	$sub_menu_series_link = '<a class="btn" href="/management/content-listing/series">Series</a>';
	
	$sub_menu_lesson_plans_args = uportal_backend_content_listing_build_path(array (
		'link_type' => 'parent-sub-menu',
		'content_section_type' => 'lesson-plans',
	));
	$sub_menu_lesson_plans_link = '<a class="btn" href="/management/content-listing/lesson-plans">Lesson Plans</a>';
	
	$sub_menu = array(
    '#theme' => 'item_list',
    '#type' => 'ul',
		'#attributes' => array('class' => array('content-listing-sub-menu', 'clearfix')),
		'#items' => array (
			'individual-items' => array (
				'class' => ($active_section=='individual-items') ? array('active') : array(),
				'data' => $sub_menu_individual_items_link,
			),
			'series' => array (
				'class' => ($active_section=='series') ? array('active') : array(),
				'data' => $sub_menu_series_link,
			),
			'lesson-plans' => array (
				'class' => ($active_section=='lesson-plans') ? array('active') : array(),
				'data' => $sub_menu_lesson_plans_link,
			),
		),
  );
	
	return $sub_menu;
}
/** ENDS *****************************************/


/**
 * get current args
 */
function _uportal_backend_content_listing_get_current_args() {
	$args = array();
	
	$numeric_args = array(
		'cid' => 'category_nid',
		'status' => 'status',
		'uid' => 'user_id',
		'prid' => 'provider_nid',
		'series_nid' => 'series_nid',
		'lesson_plan_nid' => 'lesson_plan_nid',
	);
	foreach ($numeric_args as $numeric_arg => $arg_name) {
		$arg_value = 'all';
		if (isset($_GET[$numeric_arg]) && is_numeric($_GET[$numeric_arg])) {
			$arg_value = $_GET[$numeric_arg];
		}
		$args[$arg_name] = $arg_value;
	}
	
	//type
	$allowed_types = array('video', 'audio', 'document');
	$arg_type_value = 'all';
	if (isset($_GET['type']) && in_array($_GET['type'], $allowed_types)) {
		$arg_type_value = $_GET['type'];
	}
	$args['type_machine_name'] = $arg_type_value;
	
	//language
	$arg_lang_value = 'all';
	$enabled_languages = locale_language_list('name');
	$enabled_languages['und'] = 'Undefined Language';
	if (isset($_GET['lang']) && in_array($_GET['lang'], array_keys($enabled_languages))) {
		$arg_lang_value = $_GET['lang'];
	}
	$args['lang_code'] = $arg_lang_value;
	
	//content section type
	$section_type = arg(2);
	$arg_section_type_value = 'individual-items';
	$allowed_section_types = array('individual-items', 'series', 'lesson-plans');
	if (in_array($section_type, $allowed_section_types)) {
		$arg_section_type_value = $section_type;
	}
	$args['content_section_type'] = $arg_section_type_value;
	
	return $args;

}
/** ENDS *****************************************/


/**
 * build path to content listing page
 *
 * management/content/category/type/status/user/provider/language?cid=&type=&status=&user=&provider=&lang=
 *
 * now changed to:  management/content-listing/listing_type?cid=&type=&status=&user=&provider=&series_nid=&lang=
 *
 * args array (
 * 	'category_nid' => '',
 * 	'type_machine_name' => '',
 * 	'status' => '',
 * 	'user_id' => '',
 * 	'provider_nid' => '',
 * 	'lang_code' => '',
 * 	'link_type' => '',
 * 	'order_by' => '',
 * 	'content_section_type' => '',
 * )
 */
function uportal_backend_content_listing_build_path($args = array()) {
	
	$category_nid = isset($args['category_nid']) ? $args['category_nid'] : 'all';
	$type_machine_name = isset($args['type_machine_name']) ? $args['type_machine_name'] : 'all';
	$status = isset($args['status']) ? $args['status'] : 'all';
	$user_id = isset($args['user_id']) ? $args['user_id'] : 'all';
	$provider_nid = isset($args['provider_nid']) ? $args['provider_nid'] : 'all';
	$series_nid = isset($args['series_nid']) ? $args['series_nid'] : 'all';
	$lesson_plan_nid = isset($args['lesson_plan_nid']) ? $args['lesson_plan_nid'] : 'all';
	$lang_code = isset($args['lang_code']) ? $args['lang_code'] : 'all';
	$tags = isset($args['tags']) ? $args['tags'] : 'all';
	$collapsed_view = isset($args['collapsed-view']) ? $args['collapsed-view'] : false;

	$link_type = isset($args['link_type']) ? $args['link_type'] : '';
	$order_by = isset($args['order_by']) ? $args['order_by'] : 'created';
	$content_section_type = isset($args['content_section_type']) ? $args['content_section_type'] : 'individual-items';
	
	$components = array();
	$class = '';
	$current_path_parts = explode('/', $_GET['q']);
	
	//content section type
	$section_types = array('individual-items'=>1, 'series'=>1, 'lesson-plans'=>1);
	$section_type_in_path = arg(2);
	if (!isset($args['content_section_type']) && isset($section_types[$section_type_in_path])) {
		$content_section_type = $section_type_in_path;
	}
	
	//category
	$category_path = array('path'=>'all-categories', 'param'=>'all', 'param-name'=>'cid', );
	if (is_numeric($category_nid)) {
		$category_node = node_load($category_nid);
		if ($category_node) {
			$category_path['path'] = drupal_html_class($category_node->title);
			$category_path['param'] = $category_nid;
		}
	}
	$components['category'] = $category_path;
	if (
		$link_type=='category'
		&& (
			(isset($_GET['cid']) && $_GET['cid']==$category_path['param'])
			|| (!isset($_GET['cid']) && $category_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
	
	//type
	$type_path = array('path'=>'all-types', 'param'=>'all', 'param-name'=>'type', );
	$allowed_types = array('video'=>1, 'audio'=>1, 'document'=>1, 'series'=>1, );
	if (isset($allowed_types[$type_machine_name])) {
		$type_path['path'] = $type_path['param'] = $type_machine_name;
	}
	$components['type'] = $type_path;
	if (
		$link_type=='type'
		&& (
			(isset($_GET['type']) && $_GET['type']==$type_path['param'])
			|| (!isset($_GET['type']) && $type_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
	
	//status
	$status_path = array('path'=>'published', 'param'=>1, 'param-name'=>'status', );
	if ($status==='all') {
		$status_path['path'] = 'all-content';
		$status_path['param'] = 'all';
	} elseif ($status===1 || $status==='1') {
		$status_path['path'] = 'published';
		$status_path['param'] = 1;
	} elseif ($status===0 || $status==='0') {
		$status_path['path'] = 'draft';
		$status_path['param'] = 0;
	}
	$components['status'] = $status_path;
	if ( //most inefficient function and boolean check - i am sleepy!!
		$link_type=='status'
		&& (
			(isset($_GET['status']) && (
				($_GET['status']!=='all' && intval($_GET['status'])===0 && $status_path['param']===0)
				|| ($_GET['status']!=='all' && intval($_GET['status'])===1 && $status_path['param']===1)
				|| ($_GET['status']==='all' && $status_path['param']==='all')
			))
			|| (!isset($_GET['status']) && $status_path['param']!==0 && $status_path['param']==='all')
		)
	) {
		$class .= ' active';
	}
	
	//user
	$user_path = array( 'path'=>'all-users', 'param'=>'all', 'param-name'=>'uid', );
	if (is_numeric($user_id)) {
		$user = user_load($user_id);
		if ($user) {
			$user_path['path'] = drupal_html_class($user->name);
			$user_path['param'] = $user_id;
		}
	}
	$components['user'] = $user_path;
	if (
		$link_type=='user'
		&& (
			(isset($_GET['uid']) && $_GET['uid']==$user_path['param'])
			|| (!isset($_GET['uid']) && $user_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
	
	//provider
	$provider_path = array('path'=>'all-providers', 'param'=>'all', 'param-name'=>'prid', );
	if (is_numeric($provider_nid)) {
		$provider_node = node_load($provider_nid);
		if ($provider_node) {
			$provider_path['path'] = drupal_html_class($provider_node->title);
			$provider_path['param'] = $provider_nid;
		}
	}
	$components['provider'] = $provider_path;
	if (
		$link_type=='provider'
		&& (
			(isset($_GET['prid']) && $_GET['prid']==$provider_path['param'])
			|| (!isset($_GET['prid']) && $provider_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
	
	//series
	$series_path = array('path'=>'all-series', 'param'=>'all', 'param-name'=>'series_nid', );
	if (is_numeric($series_nid)) {
		$series_node = node_load($series_nid);
		if ($series_node) {
			$series_path['path'] = drupal_html_class($series_node->title);
			$series_path['param'] = $series_nid;
		}
	}
	$components['series'] = $series_path;
	if (
		$link_type=='series'
		&& (
			(isset($_GET['series_nid']) && $_GET['series_nid']==$series_path['param'])
			|| (!isset($_GET['series_nid']) && $series_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
		
	//lesson plan
	$lesson_plan_path = array('path'=>'all-lesson-plans', 'param'=>'all', 'param-name'=>'lesson_plan_nid', );
	if (is_numeric($lesson_plan_nid)) {
		$lesson_plan_node = node_load($lesson_plan_nid);
		if ($lesson_plan_node) {
			$lesson_plan_path['path'] = drupal_html_class($lesson_plan_node->title);
			$lesson_plan_path['param'] = $lesson_plan_nid;
		}
	}
	$components['lesson-plan'] = $lesson_plan_path;
	if (
		$link_type=='lesson-plan'
		&& (
			(isset($_GET['lesson_plan_nid']) && $_GET['lesson_plan_nid']==$lesson_plan_path['param'])
			|| (!isset($_GET['lesson_plan_nid']) && $lesson_plan_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
	
	//language code
	$language_path = array('path'=>'all-languages', 'param'=>'all', 'param-name'=>'lang', );
	$enabled_languages = locale_language_list('name');
	$enabled_languages['und'] = 'Undefined Language';
	if (in_array($lang_code, array_keys($enabled_languages))) {
		$language_path['path'] = drupal_html_class($enabled_languages[$lang_code]);
		$language_path['param'] = $lang_code;
	}
	$components['language'] = $language_path;
	if (
		$link_type=='language'
		&& (
			(isset($_GET['lang']) && $_GET['lang']==$language_path['param'])
			|| (!isset($_GET['lang']) && $language_path['param']=='all')
		)
	) {
		$class .= ' active';
	}
		
	//tags
	$tag_tids_in_path = array();
	$components['tags'] = array('path'=>'all-tags', 'param'=>'all', 'param-name'=>'tags', );
	if (isset($_GET['tags'])) {
		if ($_GET['tags']=='all') { //if $tags is tid
			if (is_numeric($tags)) {
				$tag_tids_in_path = array($tags);
			}
		} else {
			$tag_tids_in_path = explode('+', $_GET['tags']);
			if ($link_type=='tags' && is_numeric($tags)) {
				if (in_array($tags, $tag_tids_in_path)) {
					$tag_tids_in_path = array_diff($tag_tids_in_path, array($tags));
					$class .= ' active';
				} else {
					$tag_tids_in_path[] = $tags;
				}
			}
		}
		$components['tags']['param'] = $tag_tids_in_path;
	} else {
		if ($link_type=='tags' && is_numeric($tags)) {
			$components['tags']['param'] = array($tags);
		}
	}
	
	//sorter
	if (
		$link_type=='sorter'
		&& (
			(isset($_GET['order']) && $order_by==$_GET['order'])
			|| (!isset($_GET['order']) && $order_by=='created')
		)
	) {
		$class .= ' active';
	}
	
	//collapsed view
	if ($collapsed_view || isset($_GET['collapsed-view'])) {
		$components['collapsed-view'] = array(
			'param'=>'1',
			'param-name'=>'collapsed-view',
		);
	}
	
	//make path
	$path = '/management/content-listing/'.$content_section_type;
	$param_str = '';
	$no_components = count($components);
	$cnter = 0;
	foreach ($components as $key=>$component) {
		$param_str .= ($cnter==0) ? '?' : '&';
		if (is_array($component['param'])) {
			if (count($component['param'])) {
				$component['param'] = implode('+', $component['param']);
			} else {
				$component['param'] = 'all';
			}
		}
		$param_str .= $component['param-name'].'='.urlencode($component['param']);
		$cnter++;
	}
	$path .= $param_str.'&order='.$order_by;
	
	return array (
		'path' => $path,
		'class' => $class,
	);
	
}
/** ENDS *****************************************/


/**
 * create filter list html
 */
function _uportal_content_listing_create_filter_list_html($filters, $filter_type, $format='list') {
	
	$filter_list = '<ul class="filters '.$filter_type.'-filters">';
	
	if ($format=='dropdown') {
		$filter_list = '<div class="filters-dropdown-wrapper"><select class="filters-dropdown" id="'.$filter_type.'-filter-dropdown"';
		if ($filter_type=='tags') {
			$filter_list .= ' data-placeholder="All Tags" multiple="multiple"';
		}
		$filter_list .= '>';
	}
	
	foreach ($filters as $filter) {
		//filter values
		$category =	($filter_type=='category') ?
							$filter['value']
							: (isset($_GET['cid']) ? $_GET['cid'] : 'all');
		$type =	($filter_type=='type') ?
						$filter['value']
						: (isset($_GET['type']) ? $_GET['type'] : 'all');
		$status =	($filter_type=='status') ?
						$filter['value']
						: (isset($_GET['status']) ? $_GET['status'] : 'all');
		$user = ($filter_type=='user') ?
						$filter['value']
						: (isset($_GET['uid']) ? $_GET['uid'] : 'all');
		$provider = ($filter_type=='provider') ?
						$filter['value']
						: (isset($_GET['prid']) ? $_GET['prid'] : 'all');
		$series = ($filter_type=='series') ?
						$filter['value']
						: (isset($_GET['series_nid']) ? $_GET['series_nid'] : 'all');
		$lesson_plan = ($filter_type=='lesson-plan') ?
						$filter['value']
						: (isset($_GET['lesson_plan_nid']) ? $_GET['lesson_plan_nid'] : 'all');
		$language = ($filter_type=='language') ?
						$filter['value']
						: (isset($_GET['lang']) ? $_GET['lang'] : 'all');
		$tags = ($filter_type=='tags') ?
						$filter['value']
						: (isset($_GET['tags']) ? $_GET['tags'] : 'all');
		
		//create path
		$path_args = array (
			'category_nid' => $category,
			'type_machine_name' => $type,
			'status' => $status,
			'user_id' => $user,
			'provider_nid' => $provider,
			'series_nid' => $series,
			'lesson_plan_nid' => $lesson_plan,
			'lang_code' => $language,
			'link_type' => $filter_type,
			'tags' => $tags,
		);
		if ($filter_type=='series') {
			$path_args['order_by'] = 'series';
		}
		if ($filter_type=='lesson-plan') {
			$path_args['order_by'] = 'lesson-plan';
		}
		$path = uportal_backend_content_listing_build_path($path_args);
		if ($format=='dropdown') {
			$selected_option = (strpos($path['class'], 'active')!==false) ? 'selected="selected"' : '';
			$filter_list .= '<option value="'.$path['path'].'" '.$selected_option.'>'.$filter['title'].'</option>';
		} else {
			$filter_list .= '<li class="'.$path['class'].'"><a href="'.$path['path'].'">'.$filter['title'].'</a></li>';
		}
	}
	
	if ($format=='dropdown') {
		$filter_list .= '</select></div>';
	} else {
		$filter_list .= '</ul>';
	}
	
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: status
 */
function _uportal_content_listing_filters_status() {
	$filters = array (
		array('value'=>'all', 'title'=>'All', ),
		array( 'value'=>0, 'title'=>'Drafts', ),
		array( 'value'=>1, 'title'=>'Published', ),
	);
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'status');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: status
 */
function _uportal_content_listing_filters_categories() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Categories', ),
	);
	
	//get all categories
	$categories_query = db_query("SELECT node.title AS title, node.nid AS nid FROM {node} node WHERE node.type IN  ('topic') ORDER BY title ASC");
	foreach ($categories_query as $category) {
		$filters[] = array(
			'value' => $category->nid,
			'title' => $category->title,
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'category');
	return $filter_list;

}
/** ENDS *****************************************/


/**
 * filter block: status
 */
function _uportal_content_listing_filters_types() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Types', ),
		array('value'=>'audio', 'title'=>'Audio', ),
		array('value'=>'document', 'title'=>'Text', ),
		array('value'=>'video', 'title'=>'Video', ),
	);
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'type', 'dropdown');
	return $filter_list;

}
/** ENDS *****************************************/


/**
 * filter block: series
 */
function _uportal_content_listing_filters_series() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Series', ),
	);
	
	//get all categories
	$query = db_query("SELECT node.title AS title, node.nid AS nid FROM {node} node WHERE node.type IN  ('series') ORDER BY title ASC");
	foreach ($query as $node) {
		$filters[] = array(
			'value' => $node->nid,
			'title' => $node->title,
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'series', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: lesson plans
 */
function _uportal_content_listing_filters_lesson_plans() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Lesson Plans', ),
	);
	
	//get all categories
	$query = db_query("SELECT node.title AS title, node.nid AS nid FROM {node} node WHERE node.type IN  ('lesson_plan') ORDER BY title ASC");
	foreach ($query as $node) {
		$filters[] = array(
			'value' => $node->nid,
			'title' => $node->title,
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'lesson-plan', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: editors
 */
function _uportal_content_listing_filters_editors() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Editors', ),
	);
	
	//get all editors
	$editors_query = db_query("SELECT users.name AS name, users.uid AS uid FROM {users} users ORDER BY name ASC");
	foreach ($editors_query as $editor) {
		$filters[] = array(
			'value' => $editor->uid,
			'title' => $editor->name,
		);
	}
	
	//create filter list
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'user', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: providers
 */
function _uportal_content_listing_filters_providers() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Providers', ),
	);
	
	//get all categories
	$providers_query = db_query("SELECT node.title AS title, node.nid AS nid FROM {node} node WHERE node.type IN  ('content_provider') ORDER BY title ASC");
	foreach ($providers_query as $provider) {
		$filters[] = array(
			'value' => $provider->nid,
			'title' => $provider->title,
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'provider', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: tags
 */
function _uportal_content_listing_filters_tags() {
	$filters = array (
		//array('value'=>'all', 'title'=>'All Tags', ),
	);
	
	//get all categories
	$tags_query = db_query("SELECT terms.tid AS tid, terms.name AS term_name, vocab.name AS vocab_name FROM {taxonomy_term_data} terms INNER JOIN {taxonomy_vocabulary} vocab ON terms.vid = vocab.vid ORDER BY term_name ASC");
	foreach ($tags_query as $term) {
		$filters[] = array(
			'value' => $term->tid,
			'title' => $term->term_name.' ('.$term->vocab_name.')',
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'tags', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * filter block: languages
 */
function _uportal_content_listing_filters_languages() {
	$filters = array (
		array('value'=>'all', 'title'=>'All Languages', ),
	);
	
	//get enabled languages
	$languages = locale_language_list('name');
	$filters[] = array('value'=>'und', 'title'=>'Undefined Language', );
	foreach ($languages as $lang_code => $language_name) {
		$filters[] = array (
			'value' => $lang_code,
			'title' => $language_name,
		);
	}
	
	$filter_list = _uportal_content_listing_create_filter_list_html($filters, 'language', 'dropdown');
	return $filter_list;
}
/** ENDS *****************************************/


/**
 * alter views query for series content
 */
function _uportal_content_listing_views_query_alter_series(&$view, &$query) {
	$allowed_types = array('video', 'audio', 'document', 'series');
	$languages = locale_language_list('name');
	
	//only go through this if at least one filter is set
	if (!(
		(isset($_GET['cid']) && is_numeric($_GET['cid']))
		|| (isset($_GET['type']) && in_array($_GET['type'], $allowed_types))
		|| (isset($_GET['status']) && is_numeric($_GET['status']) && ($_GET['status']==0 || $_GET['status']==1))
		|| (isset($_GET['uid']) && is_numeric($_GET['uid']) && $_GET['uid']>0)
		|| (isset($_GET['prid']) && is_numeric($_GET['prid']))
		|| (isset($_GET['lang']) && isset($languages[$_GET['lang']]))
		|| (isset($_GET['tags']) && $_GET['tags']!='all')
	)) {
		return;
	}
	
	//custom where conditions
	$where_conditions = array();
	
	//check if only category is set
	$only_category_set = (isset($_GET['cid']) && is_numeric($_GET['cid']))
		&& (isset($_GET['type']) && !in_array($_GET['type'], $allowed_types))
		&& (isset($_GET['status']) && !is_numeric($_GET['status']))
		&& (isset($_GET['uid']) && !is_numeric($_GET['uid']))
		&& (isset($_GET['prid']) && !is_numeric($_GET['prid']))
		&& (isset($_GET['lang']) && !isset($languages[$_GET['lang']]))
		&& (isset($_GET['tags']) && $_GET['tags']=='all');
	
	//base query only on category owner of series
	if ($only_category_set) {
		$where_conditions[] = array (
			'field' => 'node_field_data_field_topic_reference.nid',
			'value' => $_GET['cid'],
			'operator' => '=',
		);
	} else {
	
		//filter view to selected series nids
		$series_nids = array();
		$series = array();
		$individual_items = views_get_view_result('management_content_listing', 'block_1');
		foreach ($individual_items as $content_item) {
			$series_nid = $content_item->node_field_data_field_series_reference_nid;
			if (!isset($series_nids['nid-'.$series_nid])) {
				$series_nids['nid-'.$series_nid] = $series_nid;
				$series[] = $series_nid;
			}
		}
		$where_conditions[] = array (
			'field' => 'node.nid',
			'value' => $series,
			'operator' => 'IN',
		);
		
	}
		
	//insert conditions
	if (count($where_conditions)) {
		$query->where[] = array (
			'conditions' => $where_conditions,
			'args' => array(),
			'type' => 'AND',
		);
	}
}
/** ENDS *****************************************/


/**
 * alter views query for lesson plans content
 */
function _uportal_content_listing_views_query_alter_lesson_plans(&$view, &$query) {
	$allowed_types = array('video', 'audio', 'document', 'series');
	$languages = locale_language_list('name');
	
	//only go through this if at least one filter is set
	if (!(
		(isset($_GET['cid']) && is_numeric($_GET['cid']))
		|| (isset($_GET['type']) && in_array($_GET['type'], $allowed_types))
		|| (isset($_GET['status']) && is_numeric($_GET['status']) && ($_GET['status']==0 || $_GET['status']==1))
		|| (isset($_GET['uid']) && is_numeric($_GET['uid']) && $_GET['uid']>0)
		|| (isset($_GET['prid']) && is_numeric($_GET['prid']))
		|| (isset($_GET['lang']) && isset($languages[$_GET['lang']]))
		|| (isset($_GET['tags']) && $_GET['tags']!='all')
	)) {
		return;
	}
	
	//filter view to selected series nids
	$lesson_plan_nids = array();
	$lesson_plans = array();
	$individual_items = views_get_view_result('management_content_listing', 'block_1');
	foreach ($individual_items as $content_item) {
		$content_node = $content_item->_field_data['nid']['entity'];
		$lesson_plan_field = $content_node->field_lesson_plan_reference;
		if (isset($lesson_plan_field['und']) && count($lesson_plan_field['und'])) {
			foreach ($lesson_plan_field['und'] as $lesson_plan_value) {
				$lesson_plan_nid = $lesson_plan_value['target_id'];
				if (!isset($lesson_plan_nids['nid-'.$lesson_plan_nid])) {
					$lesson_plan_nids['nid-'.$lesson_plan_nid] = $lesson_plan_nid;
					$lesson_plans[] = $lesson_plan_nid;
				}
			}
		}
	}
	
	//custom where conditions
	$where_conditions = array();
	
	//type filter
	$where_conditions[] = array (
		'field' => 'node.nid',
		'value' => $lesson_plans,
		'operator' => 'IN',
	);
		
	//insert conditions
	if (count($where_conditions)) {
		$query->where[] = array (
			'conditions' => $where_conditions,
			'args' => array(),
			'type' => 'AND',
		);
	}
}
/** ENDS *****************************************/


/**
 * alter views query for individual items content
 */
function _uportal_content_listing_views_query_alter_individual_items(&$view, &$query) {
	
	//custom where conditions
	$where_conditions = array();
	
	//category filter
	if (isset($_GET['cid']) && is_numeric($_GET['cid'])) {
		$where_conditions[] = array (
			'field' => 'node_field_data_field_topic_reference.nid = :node_nid',
			'value' => array(
				':node_nid' => $_GET['cid'],
			),
			'operator' => 'formula',
		);
	}
	
	//type filter
	$allowed_types = array('video', 'audio', 'document', 'series');
	if (isset($_GET['type']) && in_array($_GET['type'], $allowed_types)) {
		$where_conditions[] = array (
			'field' => 'node.type',
			'value' => $_GET['type'],
			'operator' => '=',
		);
	}
	
	//status
	if (isset($_GET['status']) && is_numeric($_GET['status']) && ($_GET['status']==0 || $_GET['status']==1)) {
		$where_conditions[] = array (
			'field' => 'node.status',
			'value' => $_GET['status'],
			'operator' => '=',
		);
	}
	
	//user
	if (isset($_GET['uid']) && is_numeric($_GET['uid']) && $_GET['uid']>0) {
		$where_conditions[] = array (
			'field' => 'node.uid',
			'value' => $_GET['uid'],
			'operator' => '=',
		);
	}
	
	//provider
	if (isset($_GET['prid']) && is_numeric($_GET['prid'])) {
		$where_conditions[] = array (
			'field' => 'node_field_data_field_content_provider_reference.nid',
			'value' => $_GET['prid'],
			'operator' => '=',
		);
	}
	
	//series
	if (isset($_GET['series_nid']) && is_numeric($_GET['series_nid'])) {
		$where_conditions[] = array (
			'field' => 'node_field_data_field_series_reference.nid',
			'value' => $_GET['series_nid'],
			'operator' => '=',
		);
	}
	
	//language
	$languages = locale_language_list('name');
	if (isset($_GET['lang']) && isset($languages[$_GET['lang']])) {
		$where_conditions[] = array (
			'field' => 'node.language',
			'value' => $_GET['lang'],
			'operator' => '=',
		);
	}
	
	//tags
	/** do NOT use taxonomy_index because it only holds published nodes.
	 *  - switched to a field table to include draft results
	 */
	if (isset($_GET['tags']) && $_GET['tags']!='all') {
		$filter_tids = explode('+', $_GET['tags']);
		if (count($filter_tids)) {
			$tagged_nids = array();
			$terms = taxonomy_term_load_multiple($filter_tids);
			$handled_vocabs = array();
			foreach ($terms as $term) {
				$field_data_table = 'field_data_field_'.$term->vocabulary_machine_name.'_reference';
				$field_name = 'field_'.$term->vocabulary_machine_name.'_reference_tid';
				
				//check vocabulary table just once
				if (!isset($handled_vocabs[$term->vocabulary_machine_name])) {
					$handled_vocabs[$term->vocabulary_machine_name] = 1;
				} else {
					continue;
				}
				
				$term_nids = db_select($field_data_table, 'f')
					->fields('f', array('entity_id'))
					->distinct()
					->condition($field_name, $filter_tids, 'IN')
					->execute()
					->fetchCol();
				$tagged_nids = array_merge($tagged_nids, $term_nids);
			}
			$where_conditions[] = array (
				'field' => 'node.nid',
				'value' => $tagged_nids,
				'operator' => 'IN',
			);
		}
	}
	
	//check for search results - only on individual items view
	if (arg(2)=='individual-items') {
		$search_result_nids = _uportal_content_listing_search_individual_items();
		
		//above function returns FALSE if search is not active
		if ($search_result_nids !== FALSE) {
			//if search results are zero, put an impossible value for NID
			if (count($search_result_nids)<=0) {
				$search_result_nids = array(0);
			}
			$where_conditions[] = array (
				'field' => 'node.nid',
				'value' => $search_result_nids,
				'operator' => 'IN',
			);
		}
	}
	
	//if not individual items page, no need to use a pager on this view
	if (arg(2)!='individual-items') {
		$view->set_items_per_page(0);
	}
	
	//insert conditions
	if (count($where_conditions)) {
		$query->where[] = array (
			'conditions' => $where_conditions,
			'args' => array(),
			'type' => 'AND',
		);
	}
	
	//alter order by - only for individual items view
	if (arg(2)=='individual-items') {
		
		$query->orderby = array();
		if (!isset($_GET['order'])) {
			$query->orderby = array (
				array (
					'field' => 'node_created',
					'direction' => 'DESC',
				)
			);
		} else {
			$orderby = array (
				'field' => 'node_created',
				'direction' => 'DESC',
			);
			switch ($_GET['order']) {
				case 'alphabetic-a-z':
					$orderby = array (
						'field' => 'node_title',
						'direction' => 'ASC',
					);
					break;
				case 'alphabetic-z-a':
					$orderby = array (
						'field' => 'node_title',
						'direction' => 'DESC',
					);
					break;
				case 'updated':
					$orderby = array (
						'field' => 'node_changed',
						'direction' => 'DESC',
					);
					break;
			}
			$query->orderby = array($orderby);
			
			//order by series
			if ($_GET['order']=='series' && isset($_GET['series_nid']) && is_numeric($_GET['series_nid']) && $_GET['series_nid']>0) {
				$join = new views_join();
				$join->type = 'LEFT';
				$join->table = 'nodequeue_nodes';
				$join->left_table = 'node';
				$join->left_field = 'nid';
				$join->field = 'nid';
				$join->extra_type = 'AND';
				$join->extra = array (
					array (
						'value' => array(15),
						'field' => 'qid',
					),
				);
				$query->add_relationship('series_queue_table', $join, 'node');
				$query->add_field('series_queue_table', 'position');
				$series_queue_orderby = array (
					'field' => 'series_queue_table_position',
					'direction' => 'ASC',
				);
				array_unshift($query->orderby, $series_queue_orderby);
			}
		}
		
		//check if lesson plan filter is present
		if (isset($_GET['lesson_plan_nid']) && is_numeric($_GET['lesson_plan_nid']) && $_GET['lesson_plan_nid']>0) {
			
			$lesson_plan_nid = $_GET['lesson_plan_nid'];
			
			//join lesson plan reference table
			$query->add_table('field_data_field_lesson_plan_reference');
			
			//add where clause
			$query->add_where(1, 'field_data_field_lesson_plan_reference.field_lesson_plan_reference_target_id', $lesson_plan_nid, '=');
			
			/*
			 * implementing this join results in repeated results
			 * so lesson plans are ordered in views_pre_render to reorder content
			 * 
			$join = new views_join();
			$join->type = 'LEFT';
			$join->table = 'nodequeue_nodes';
			$join->left_table = 'node';
			$join->left_field = 'nid';
			$join->field = 'nid';
			$join->extra_type = 'AND';
			$join->extra = array (
				array (
					'value' => array(12),
					'field' => 'qid',
				),
			);
			$query->add_relationship('queue_table', $join, 'node');
			$query->add_field('queue_table', 'position');
			$queue_orderby = array (
				'field' => 'queue_table_position',
				'direction' => 'ASC',
			);
			array_unshift($query->orderby, $queue_orderby);
			*/
			
		}
		
	} //close and finish altering order by

}
/** ENDS *****************************************/


/**
 * pre render views and arrange results e.g. for lesson plan
 */
function _uportal_content_listing_views_pre_render_individual_items(&$view) {
	if (isset($_GET['lesson_plan_nid']) && is_numeric($_GET['lesson_plan_nid']) && $_GET['lesson_plan_nid']>0 && isset($_GET['order']) && $_GET['order']=='lesson-plan') {
		
		//get lesson plan content ordered according to queue
		$lesson_plan_content = views_get_view_result('management_content_listing', 'block_5', $_GET['lesson_plan_nid']);
		
		//order original view result according to ordered lesson plans
		$old_view_result = $view->result;
		$new_view_result = array();
		foreach ($lesson_plan_content as $lesson_plan) {
			$nid = $lesson_plan->nid;
			foreach ($old_view_result as $key => $view_row) {
				if ($view_row->nid == $nid) {
					$new_view_result[] = $view_row;
					unset($old_view_result[$key]);
					break;
				}
			}
		}
		$new_view_result = $new_view_result + $old_view_result;
		
		$view->result = $new_view_result;
	}
}
/** ENDS *****************************************/


/**
 * explicitly specify lesson plan items order
 */
function _uportal_content_listing_views_alter_lesson_plan_items(&$view) {
	$lesson_plan_nid = $view->args[0];
	$lesson_plan_queue = nodequeue_load_queue_by_name('lesson_plan_child_queue');
	$lesson_plan_subqueue = current(nodequeue_load_subqueues_by_reference( array (
		$lesson_plan_queue->qid => array('lesson_plan_child_queue_'.$lesson_plan_nid)
	)));
	$lesson_plans = nodequeue_load_nodes($lesson_plan_subqueue->sqid, false, 0, 100, false);
	
	//order original view result according to ordered lesson plans
	$old_view_result = $view->result;
	$new_view_result = array();
	foreach ($lesson_plans as $lesson_plan) {
		$nid = $lesson_plan->nid;
		foreach ($old_view_result as $key => $view_row) {
			if ($view_row->nid == $nid) {
				$new_view_result[] = $view_row;
				unset($old_view_result[$key]);
				break;
			}
		}
	}
	$new_view_result = $new_view_result + $old_view_result;
	
	$view->result = $new_view_result;
}
/** ENDS *****************************************/


/**
 * make search form for index page
 */
function uportal_content_listing_search_form($form, &$form_state) {
	$form['search_field'] = array (
		'#type' => 'textfield',
		//'#required' => TRUE,
		'#attributes' =>array(
			'placeholder' => t('Filter by keyword ...'),
			'value' => (isset($_SESSION['uportal_content_listing_search_term']) && $_SESSION['uportal_content_listing_search_term']) ? $_SESSION['uportal_content_listing_search_term'] : '',
		),
		//'#title' => "", //no label
		//'#description' => '', //no description
	);
	$form['submit'] = array (
		'#type' => 'submit',
		'#value' => 'Search',
	);
	return $form;
}
/** ENDS *****************************************/


/**
 * submit function for search form
 */
function uportal_content_listing_search_form_submit($form, &$form_state) {
	$search_term = $form_state['values']['search_field'];
	$current_path = drupal_parse_url(request_uri());
	$current_path['path'] = trim($current_path['path'], '/');
	
	//check for existence of words
	//if it is empty, kill search
	if ($search_term=='' || ctype_space($search_term)) { //either empty or full of whitespace
		$current_path['query']['killsearch'] = 1;
	} else {
		$_SESSION['uportal_content_listing_search_term'] = $search_term;
		if (isset($current_path['query']['killsearch'])) {
			unset($current_path['query']['killsearch']);
		}
	}
	
	drupal_goto($current_path['path'], array(
		'query' => $current_path['query'],
		'fragment' => $current_path['fragment'],
	));
	
}
/** ENDS *****************************************/


/**
 * search individual items
 *
 * use these session variables to store:
 *  - current search term
 *  - previous or old search term
 *  - search result NIDs
 *  
 */
function _uportal_content_listing_search_individual_items() {
	$search_result_nids = array();
	
	//return FALSE if there is no search if these conditions are met
	if (
		isset($_GET['killsearch'])
		|| !isset($_SESSION['uportal_content_listing_search_term'])
	) {
		unset($_SESSION['uportal_content_listing_search_term']);
		unset($_SESSION['uportal_content_listing_search_term_previous']);
		unset($_SESSION['uportal_content_listing_search_result_nids']);
		return false;
	}
	
	//check if new term is the same as the previous one.
	//if true, send cached results back
	if (
		isset($_SESSION['uportal_content_listing_search_term_previous'])
		&& isset($_SESSION['uportal_content_listing_search_result_nids'])
		&& ($_SESSION['uportal_content_listing_search_term_previous'] == $_SESSION['uportal_content_listing_search_term'])
	) {
		return $_SESSION['uportal_content_listing_search_result_nids'];
	}
	
	//store the search term for comparison to check if we need to refresh the search results
	$_SESSION['uportal_content_listing_search_term'] = strtolower($_SESSION['uportal_content_listing_search_term']);
	$_SESSION['uportal_content_listing_search_term_previous'] = $_SESSION['uportal_content_listing_search_term'];
	
	//if we have a valid search session and keyword, find the results
	$search_term = $_SESSION['uportal_content_listing_search_term'];
	$search_result_nids = _uportal_content_listing_search_individual_items_by_keyword($search_term);
	$_SESSION['uportal_content_listing_search_result_nids'] = $search_result_nids;
	
	return $search_result_nids;

}
/** ENDS *****************************************/


/**
 * search content given keyword
 */
function _uportal_content_listing_search_individual_items_by_keyword($search_term) {
	
	$search_result_nids = array();
	$search_term = '%'.$search_term.'%';
	$search_result_query = db_select('node', 'n')
		->fields('n', array('nid'));
		
	//make all search conditions with an OR
	$or_conditions = db_or();
	$or_conditions->condition('n.title', $search_term, 'LIKE');
	
	//languages with search term
	$valid_language_codes = db_select('languages', 'l')
		->fields('l', array('language'))
		->condition('name', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_language_codes)) {
		$or_conditions->condition('n.language', $valid_language_codes, 'IN');
	}
	
	//node type names with search term
	$valid_node_types = db_select('node_type', 'nt')
		->fields('nt', array('type'))
		->condition('name', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_node_types)) {
		$or_conditions->condition('n.type', $valid_node_types, 'IN');
	}
	
	//body field with search term
	$search_result_query->leftJoin('field_data_body', 'body', 'n.nid = body.entity_id');
	$or_conditions->condition('body.body_value', $search_term, 'LIKE');
	
	//parent category with search term
	$valid_topic_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'topic', '=')
		->condition('title', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_topic_nids)) {
		$search_result_query->leftJoin('field_data_field_topic_reference', 'topic', 'n.nid = topic.entity_id');
		$or_conditions->condition('topic.field_topic_reference_target_id', $valid_topic_nids, 'IN');
	}
	
	//parent series with search term
	$valid_series_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'series', '=')
		->condition('title', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_series_nids)) {
		$search_result_query->leftJoin('field_data_field_series_reference', 'series', 'n.nid = series.entity_id');
		$or_conditions->condition('series.field_series_reference_target_id', $valid_series_nids, 'IN');
	}
	
	//content provider with search term
	$valid_provider_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'content_provider', '=')
		->condition('title', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_provider_nids)) {
		$search_result_query->leftJoin('field_data_field_content_provider_reference', 'provider', 'n.nid = provider.entity_id');
		$or_conditions->condition('provider.field_content_provider_reference_target_id', $valid_provider_nids, 'IN');
	}
	
	//lesson plan fields with search term
	$valid_lesson_plan_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'lesson_plan', '=')
		->condition('title', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($valid_lesson_plan_nids)) {
		$search_result_query->leftJoin('field_data_field_lesson_plan_reference', 'lesson_plan', 'n.nid = lesson_plan.entity_id');
		$or_conditions->condition('lesson_plan.field_lesson_plan_reference_target_id', $valid_lesson_plan_nids, 'IN');
	}
	
	//users with search term
	$user_uids = db_select('users', 'u')
		->fields('u', array('uid'))
		->condition('name', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($user_uids)) {
		$or_conditions->condition('n.uid', $user_uids, 'IN');
	}
	
	//tags with search term
	$tagged_nids_query = db_select('taxonomy_term_data', 'tt')
		->fields('ti', array('nid'))
		->distinct();
	$tagged_nids_query->join('taxonomy_index', 'ti', 'ti.tid = tt.tid');
	$tagged_nids = $tagged_nids_query
		->condition('tt.name', $search_term, 'LIKE')
		->execute()
		->fetchCol();
	if (count($tagged_nids)>0) {
		$or_conditions->condition('n.nid', $tagged_nids, 'IN');
	}
	
	//search nodes query
	$and_condition = db_and()
		->condition('n.type', array('video','audio','document'), 'IN')
		->condition($or_conditions);
	$search_result_nids = $search_result_query
		->condition($and_condition)
		->execute()
		->fetchCol();
	
	//dpq($search_result_query);
	
	return $search_result_nids;

}
/** ENDS *****************************************/

