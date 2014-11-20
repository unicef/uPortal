<?php

$show_no_per_topic = 4;
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);

$results_by_topic = array();
$cnter = 0;

foreach ($view_results as $result) {
	$topic_nid = $result->node_field_data_field_topic_reference_nid;
	
	if (!isset($results_by_topic['topic-'.$topic_nid])) {
		$results_by_topic['topic-'.$topic_nid] = array (
			'topic-nid' => $topic_nid,
			'topic-title' => $result->node_field_data_field_topic_reference_title,
			'topic-icon' => $rendered_results[$cnter]['field_icon_2'],
			'no-of-content' => 0,
			'content' => array(),
		);
	}
	
	$results_by_topic['topic-'.$topic_nid]['no-of-content']++;
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
	$str .= '<div class="topic-results">';
	
	//topic header
	$str .= '<div class="topic-header"><div class="img">'.$topic['topic-icon'].'</div><div class="txt">'.$topic['topic-title'].'</div></div>';
	
	//topic content
	$content_cnter = 1;
	$str .= '<div class="topic-content-wrapper">';
	foreach ($topic['content'] as $content_piece) {
		$str .= '<div class="topic-content">';
		$str .= $content_piece.'<span class="counter">'.($content_cnter++).'</span>';
		$str .= '</div>';
	}
	$str .= '</div>';
	
	//topic footer
	$topic_link = l(
		t('See all content in ').$topic['topic-title'].'('.$topic['no-of-content'].')',
		'index/category/'.$topic['topic-nid'],
		array(
			'attributes' => array (
				'title'	=>	'View all content in this category: '.$topic['topic-title'],
				'class' => array(),
			),
			'html'	=> false,
		)
	);
	$str .= '<div class="topic-footer">'.$topic_link.'</div>';
	
	$str .= '</div>';
}
$str .= '</div>';

echo $str;
