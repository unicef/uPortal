<?php

global $ugyouthportal_data;

module_load_include('index_page.inc', 'ugyouthportal');

$show_no_per_topic = 4;
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);

$full_index_filters = $_SESSION['uportal_index_full_filters'];
$current_index_filter_values = $ugyouthportal_data['current-index-filter-values'];

$results_by_topic = array();
$cnter = 0;

foreach ($view_results as $result) {
	$topic_nid = $result->node_field_data_field_topic_reference_nid;
	$type = $result->node_type;
	$filter_topic = $full_index_filters['topics']['nid-'.$topic_nid];
	
	if (!isset($results_by_topic['topic-'.$topic_nid])) {
		$results_by_topic['topic-'.$topic_nid] = array (
			'topic-nid' => $topic_nid,
			'topic-title' => $result->node_field_data_field_topic_reference_title,
			'topic-title-aliased' => $filter_topic['title-aliased'],
			'topic-icon' => $rendered_results[$cnter]['field_icon_2'],
			'no-of-content' => $filter_topic['total-counts'][$current_index_filter_values['page-type']],
			'content' => array(),
		);
		if (!$current_index_filter_values['all_page']) {
			$results_by_topic['topic-'.$topic_nid]['no-of-content'] = $filter_topic['current-counts'][$current_index_filter_values['page-type']];
		}
	}
	
	if (count($results_by_topic['topic-'.$topic_nid]['content']) < $show_no_per_topic) {
		$results_by_topic['topic-'.$topic_nid]['content'][] = $rows[$cnter];
	}
	
	$cnter++;
	
}

//dsm($view_results);
//dsm($rendered_results);
//dsm(get_defined_vars());
//dsm($results_by_topic);

$str = '<div class="index-results index-all-content">';
foreach ($results_by_topic as $topic) {
	
	//topic link
	$link_options = array(
		'title' => t('More in ').$topic['topic-title'],
		'arg' => $topic['topic-nid'],
		'alias' => $topic['topic-title-aliased'],
		'count' => $topic['no-of-content'],
		'html-link' => TRUE,
	);
	$topic_link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'landing-page-topic-in-content');
	
	//topic header link
	$link_options = array(
		'title' => '<span class="img">'.$topic['topic-icon'].'</span><span class="txt">'.$topic['topic-title'].'</span>',
		'arg' => $topic['topic-nid'],
		'alias' => $topic['topic-title-aliased'],
		'html-link' => TRUE,
	);
	$topic_header_link = _ugyouthportal_get_index_link($current_index_filter_values, $link_options, 'landing-page-topic-in-content-header');
	
	//build string
	$str .= '<div class="topic-results">';
	
	//topic header
	$str .= '<div class="topic-header clearfix">'.$topic_header_link.'</div>';
	
	//topic content
	$content_cnter = 1;
	$no_in_topic = count($topic['content']);
	$str .= '<div class="topic-content-wrapper">';
	foreach ($topic['content'] as $content_piece) {
		$classes = ($content_cnter==0) ? ' first-row' : '';
		$classes .= ($content_cnter==$no_in_topic) ? ' last-row' : '';
		$str .= '<div class="index-topic-content'.$classes.'">';
		$str .= $content_piece.'<span class="counter">'.($content_cnter++).'</span>';
		$str .= '</div>';
	}
	$str .= '</div>';
	
	//topic footer
	$str .= ($show_no_per_topic<$topic['no-of-content']) ? '<div class="topic-footer"><div class="topic-link">'.$topic_link.'</div></div>' : '';
	
	$str .= '</div>';
}
$str .= '</div>';

echo $str;
