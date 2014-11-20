<?php

/***
 * implement hook_block_info
 * 	-	list blocks
 */
function ugyouthportal_block_info() {
	return array(
		'listing_content' => array(
			'info'	=>	t('Content for Listing Pages'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_nav_topics' => array(
			'info'	=>	t('Index Navigation by Topic'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_nav_skill_levels' => array(
			'info'	=>	t('Index Navigation by Skill Level'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_nav_types' => array(
			'info'	=>	t('Index Navigation by Type'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_nav_languages' => array(
			'info'	=>	t('Index Navigation by Language'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_search_form' => array(
			'info'	=> t('Index Page - Search Form'),
			'cache'	=>	DRUPAL_NO_CACHE,
		),
		'index_main_nav' => array(
			'info' => t('Index Navigation - Main'),
			'cache' => DRUPAL_NO_CACHE,
		),
		'index_filters_show_active' => array(
			'info' => t('Index Filters - Showing Active Filters'),
			'cache' => DRUPAL_NO_CACHE,
		),
		'index_page_header' => array(
			'info' => t('Index Page Header title'),
			'cache' => DRUPAL_NO_CACHE,
		),
		'index_clear_search' => array(
			'info' => t('Clear Search button'),
			'cache' => DRUPAL_NO_CACHE,
		),
		'index_nav_tags' => array (
			'info' => t('Index Navigation by Tags'),
			'cache' => DRUPAL_NO_CACHE,
		),
	);
}
/** ENDS **************************************************/


/**
 * implement hook_block_view
 * 	-	return block content
 */
function ugyouthportal_block_view($delta='') {
	$block = array();
	
	switch($delta) {
		case 'listing_content':
			$block['subject'] = t('');
			$block['content'] = ugyouthportal_get_listing_page_content();
			break;
		case 'index_nav_topics':
			$block['subject'] = t('Filter by Category:');
			$block['content'] = _ugyouthportal_create_index_filter_navigation('topics');
			break;
		case 'index_nav_skill_levels':
			$block['subject'] = t('Filter by Skill Level:');
			$block['content'] = _ugyouthportal_create_index_filter_navigation('skill-levels');
			break;
		case 'index_nav_tags':
			$block['subject'] = t('Filter by Tags:');
			$block['content'] = _ugyouthportal_create_index_filter_navigation('tags');
			break;
		case 'index_nav_types':
			$block['subject'] = t('Filter by Type:');
			$block['content'] = _ugyouthportal_create_index_filter_navigation('types');
			break;
		case 'index_nav_languages':
			$block['subject'] = t('Filter by Language:');
			$block['content'] = _ugyouthportal_create_index_filter_navigation('languages');
			break;
		case 'index_search_form':
			$block['subject'] = t('');
			$block['content'] = drupal_get_form('ugyouthportal_index_search_form');
			break;
		case 'index_main_nav':
			$block['subject'] = t('');
			$block['content'] = _ugyouthportal_get_index_navigation_main();
			break;
		case 'index_filters_show_active':
			$block['subject'] = t('');
			$block['content'] = _ugyouthportal_get_index_filters_active();
			break;
		case 'index_page_header':
			$block['subject'] = t('');
			$block['content'] = _ugyouthportal_index_page_header();
			break;
		case 'index_clear_search':
			$block['subject'] = '';
			$block['content'] = _ugyouthportal_index_clear_search();
	}
	
	return $block;
}
/** ENDS **************************************************/


/**
 * block content for listing pages i.e. category and home page
 */
function ugyouthportal_get_listing_page_content() {
	$html_str = '';
	if (!isset($_GET['page']) || (isset($_GET['page']) && is_numeric($_GET['page']) && isset($_SESSION['uportal_category_content_nids_by_page'][$_GET['page']]))) {
		$html_str = views_embed_view('listing_pages', 'block_1');
	}
	return $html_str;
}
/** ENDS **************************************************/


/**
 * block content for index page header
 */
function _ugyouthportal_index_page_header() {

	global $ugyouthportal_data;
	
	if (!isset($ugyouthportal_data['current-index-filter-values']) || count($_SESSION['uportal_index_page_results'])<=0) {
		return FALSE;
	}
	
	$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];
	$full_filters = $_SESSION['uportal_index_full_filters'];
	
	$header_str = '<div class="index-page-header topic-header clearfix">';
	
	//if all page
	if ($current_index_filter_values['category-nid']=='all' && $current_index_filter_values['page-type']!='all-lesson-plans') {
		return FALSE;
	}
	
	//category filter
	elseif ($current_index_filter_values['page-type']!='all-lesson-plans') {
		$topic_nid = $current_index_filter_values['category-nid'];
		$topic_title = $full_filters['topics']['nid-'.$topic_nid]['title'];
		$topic_node = node_load($topic_nid);
		$img = theme_image(array(
			'path' => $topic_node->field_category_icon['und'][0]['uri'],
			'width' => $topic_node->field_category_icon['und'][0]['width'],
			'height' => $topic_node->field_category_icon['und'][0]['height'],
			'attributes' => array(),
		));
		$header_str .= '<div class="img">'.$img.'</div><div class="txt">'.$topic_title.'</div>';
	}
	
	//lesson plans page
	elseif ($current_index_filter_values['page-type']=='all-lesson-plans') {
		$index_lesson_plan_icon = theme_image(array(
			'path' => path_to_theme().'/images/icon-lesson-plan-blue.png',
			'attributes' => array(),
		));
		$header_str .= '<div class="img">'.$index_lesson_plan_icon.'</div><div class="txt">Lesson Plans</div>';
	}
	
	$header_str .= '</div>';
	
	return $header_str;
	
}
/** ENDS **************************************************/


/**
 * block content for clear search on index page
 */
function _ugyouthportal_index_clear_search() {
	global $ugyouthportal_data;
	$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];
	$link = '';
	if (isset($current_index_filter_values['search-term']) && $current_index_filter_values['search-term']) {
		$link_options = array(
			'title' => 'Reset Search',
			'html-link' => false,
			'killsearch' => TRUE,
		);
		$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'all');
	}
	return '<div class="kill-search-btn">'.$link.'</div>';
}


/**
 * block content for active filters on index page
 */
function _ugyouthportal_get_index_filters_active() {
	
	global $ugyouthportal_data;
	
	if (!isset($ugyouthportal_data['current-index-filter-values'])) {
		return '';
	}
	
	$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];
	$full_filters = $_SESSION['uportal_index_full_filters'];
	
	$page_args = arg();
	$link_options = array(
		'title' => 'Clear',
		'html-link' => false,
	);
	$close_btn = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'all');
	$active_filter_str = '<div class="index-active-filters"><ul class="clearfix">';
	$no_active_filters = 0;
	
	//category filter
	if ($current_index_filter_values['category-nid']!='all') {
		$topic_nid = $current_index_filter_values['category-nid'];
		$topic_title = $full_filters['topics']['nid-'.$topic_nid]['title'];
		$topic_node = node_load($topic_nid);
		$link_options = array(
			'title' => $topic_title,
			'icon' => file_create_url($topic_node->field_icon_2['und'][0]['uri']),
			'html-link' => true,
		);
		$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'active-filter-category');
		$li_classes = ($no_active_filters==0) ? ' first' : '';
		$active_filter_str .= '<li class="category'.$li_classes.'">'.$link.'</li>';
		$no_active_filters++;
	}
	
	//skill level filter
	$active_skill_level_nids = $current_index_filter_values['skill-level-nids'];
	$no_active_skill_levels = count($active_skill_level_nids);
	if ($no_active_skill_levels && $active_skill_level_nids[0]!='all') {
		$skill_level_str = '';
		for ($i=0; $i<$no_active_skill_levels; $i++) {
			$skill_level_str .= $full_filters['skill-levels']['nid-'.$active_skill_level_nids[$i]]['title'];
			if ($i<$no_active_skill_levels-1) $skill_level_str .= ', ';
		}
		$link_options = array(
			'title' => $skill_level_str,
			'html-link' => true,
		);
		$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'active-filter-skill-level');
		$li_classes = ($no_active_filters==0) ? ' first' : '';
		$active_filter_str .= '<li class="skill-levels'.$li_classes.'">'.$link.'</li>';
		$no_active_filters++;
	}
	
	//type filter
	if ($current_index_filter_values['type']!='all') {
		$type_title = $full_filters['types'][$current_index_filter_values['type']]['title'];
		$link_options = array(
			'title' => $type_title,
			'html-link' => true,
		);
		$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'active-filter-type');
		$li_classes = ($no_active_filters==0) ? ' first' : '';
		$active_filter_str .= '<li class="type'.$li_classes.'">'.$link.'</li>';
		$no_active_filters++;
	}
	
	//language filter
	if ($current_index_filter_values['language']!='all') {
		$language_title = $full_filters['languages'][$current_index_filter_values['language']]['title'];
		$link_options = array(
			'title' => $language_title,
			'html-link' => true,
		);
		$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'active-filter-language');
		$li_classes = ($no_active_filters==0) ? ' first' : '';
		$active_filter_str .= '<li class="language'.$li_classes.'">'.$link.'</li>';
		$no_active_filters++;
	}
	
	$active_filter_str .= '</ul></div>';
	
	if ($no_active_filters>0) {
		return '<div class="index-active-filters-wrapper clearfix">'.$active_filter_str.'<div class="reset-btn">'.$close_btn.'</div></div>';
	} else {
		return '';
	}
}
/** ENDS **************************************************/


/**
 * block content for main navigation on index page
 */
function _ugyouthportal_get_index_navigation_main() {
	
	global $ugyouthportal_data;
	
	module_load_include('index_page.inc', 'ugyouthportal');
	
	if (!isset($ugyouthportal_data['current-index-filter-values'])) {
		return '';
	}
	
	$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];
	
	$nav_str = '<ul class="clearfix">';
	
	//all content link
	$link_options = array(
		'title' => 'Individual Items',
		'html-link' => true,
	);
	$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'all-content-main-nav');
	$nav_str .= '<li class="all-content">'.$link.'</li>';
	
	//all lesson plans link
	$link_options = array(
		'title' => 'Lesson Plans',
		'html-link' => true,
	);
	$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'all-lesson-plans-main-nav');
	$nav_str .= '<li class="all-lesson-plans">'.$link.'</li>';
	
	//all seriess link
	$link_options = array(
		'title' => 'Series',
		'html-link' => true,
	);
	$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'all-series-main-nav');
	$nav_str .= '<li class="all-series">'.$link.'</li>';
	
	$nav_str .= '</ul>';
	return $nav_str;
}
/** ENDS **************************************************/


/**
 * helper function to create filter navigation
 */
function _ugyouthportal_create_index_filter_navigation($type) {
	
	global $ugyouthportal_data;
	
	module_load_include('index_page.inc', 'ugyouthportal');
	
	if (!isset($ugyouthportal_data['current-index-filter-values'])) {
		return '';
	}
	
	$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];
	$filters = $_SESSION['uportal_index_full_filters'][$type];
	$nav_str = '<ul class="index-nav listing-filter-nav index-nav-'.$type.'">';
	$no_created_filters = 0;
	switch ($type) {
		case 'skill-levels':
			$nav_str = '<ul class="clearfix index-nav-skill-levels">';
			break;
		case 'tags':
			$nav_str = '<select multiple="multiple" data-placeholder="All Tags" class="choose-filter-tags">';
			break;
	}
	
	foreach ($filters as $filter) {
		/*
		if ($type=='skill-levels' && $filter['arg']=='all') {
			continue;
		}
		*/
		$count = $filter['current-counts'][$current_index_filter_values['page-type']];
		if ($current_index_filter_values['all_page'] || ($type=='topics')) {
			$count = $filter['total-counts'][$current_index_filter_values['page-type']];
		}
		$link_options = array (
			'title' => $filter['title'],
			'arg' => $filter['arg'],
			'alias' => $filter['title-aliased'],
			'count' => $count,
			'html-link' => ($type!='skill-levels'),
		);
		if ($type=='tags') {
			if ($link_options['arg']=='all' || $link_options['count']<=0) {
				continue;
			}
			$url = _ugyouthportal_get_url($current_index_filter_values, $link_options, $type);
			$path = url($url['url'], $url['options']);
			$selected = (in_array('active', $url['options']['attributes']['class'])) ? ' selected="selected"' : '';
			$nav_str .= '<option value="'.$path.'"'.$selected.'>'.$filter['title'].'</option>';
			$no_created_filters++;
		} else {
			$link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, $type);
			if ($link) {
				if ($type=='types') {
					$nav_str .= '<li class="type-'.$filter['arg'].'">'.$link.'<li>';
				} else {
					$nav_str .= '<li>'.$link.'</li>';
				}
				$no_created_filters++;
			}
		}
	}
	
	if ($type=='tags') {
		if ($no_created_filters>0) {
			$nav_str .= '</select>';
		} else {
			$nav_str = '<select multiple="multiple" data-placeholder="No tags available" class="choose-filter-tags"></select>';
		}
	} else {
		$nav_str .= '</ul>';
	}
	
	return $nav_str;
	
}
/** ENDS **************************************************/


