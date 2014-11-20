<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */

$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);

$page_args = arg();
$arg_topic_nid = $page_args[1];

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

//dsm($view_results);
//dsm($rendered_results);

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

