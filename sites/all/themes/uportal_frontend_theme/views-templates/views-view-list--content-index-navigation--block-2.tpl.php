<?php

$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);

//result nids based on current results
$nids_present_on_page = array();
foreach($rendered_results as $result) {
	$nids_present_on_page['nid-'.$result['nid']] = $result['nid'];
}

//nids present in URL
$page_args = arg();
$arg_topic = $page_args[1];
$arg_skill_level_nids = array();
$arg_skill_levels = explode(',', $page_args[2]);
foreach($arg_skill_levels as $arg_skill_level) {
	if ($arg_skill_level != 'all') {
		$arg_skill_level_nids['nid-'.$arg_skill_level] = $arg_skill_level;
	}
}

//get all skill levels
$get_skill_levels_query = "
	SELECT node.title AS node_title, node.nid AS nid, node.language AS node_language, nodequeue_nodes_node.position AS nodequeue_nodes_node_position
	FROM 
		{node} node
		LEFT JOIN {nodequeue_nodes} nodequeue_nodes_node ON node.nid = nodequeue_nodes_node.nid AND nodequeue_nodes_node.qid = '9'
	WHERE (( (node.status = '1') AND (node.type IN  ('skill_level')) ))
	ORDER BY nodequeue_nodes_node_position ASC, node_title ASC";
$skill_level_results = db_query($get_skill_levels_query);
$skill_levels = array();
$skill_level_nids = array();
foreach ($skill_level_results as $skill_level_result) {
	$skill_levels['nid-'.$skill_level_result->nid] = $skill_level_result->node_title;
	$skill_level_nids[] = $skill_level_result->nid;
}

//build links
foreach ($skill_level_nids as $skill_level_nid) {
	$url = 'index/'.$arg_topic.'/'.$skill_level_nid.'/all/all';
	$classes = '';
}


$total_count = 0;
foreach ($view_results as $result) {
	$total_count += $result->nid;
}

$link_classes = array();
if ($arg_topic_nid=='all' || $arg_topic_nid=='all-topics') {
	$link_classes[] = 'active';
}
$all_link = l(
	'All Categories ('.$total_count.')<span class="arrow"></span>',
	'index/all-topics/all-skill-levels/all-types/all-languages',
	array(
		'attributes' => array (
			'title'	=>	'View all categories',
			'class' => $link_classes,
		),
		'html'	=> true,
	)
);

dsm($view_results);
dsm($rendered_results);

?>

<ul>
	
	<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
	
	<?php
		for ($i=0; $i<$no_results; $i++) {
			$category_nid = $view_results[$i]->node_field_data_field_topic_reference_nid;
			$category_title = $view_results[$i]->node_field_data_field_topic_reference_title.' ('.$view_results[$i]->nid.')';
			if ($category_nid) {
				$link_classes = array();
				if ($category_nid==$arg_topic_nid) {
					$link_classes[] = 'active';
				}
				$link = l(
					$category_title.'<span class="arrow"></span>',
					'index/'.$category_nid.'/all/all/all',
					array(
						'attributes' => array (
							'title'	=>	$category_title,
							'class' => $link_classes,
						),
						'html'	=> true,
					)
				);
				echo '<li>'.$link.'</li>';
			}
		}
	?>
	
</ul>

