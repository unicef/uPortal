<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */

$page_args = arg();
$arg_type = isset($page_args[3]) ? $page_args[3] : 'all';
$types = array(
	'video' => 'Video',
	'audio' => 'Audio',
	'document' => 'Text',
);
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);
$arg3 = isset($page_args[3]) ? $page_args[3] : 'all';
$arg4 = isset($page_args[4]) ? $page_args[4] : 'all';

$all_url = 'category/'.$page_args[1].'/'.$page_args[2].'/all/'.$arg4;
if (is_numeric($page_args[2]) && $arg4=='all') { //if showing all languages and series is active, just point to series page
	$all_url = 'category/'.$page_args[1].'/'.$page_args[2];
}

$link_classes = array('link');
if ($arg_type=='all') {
	$link_classes[] = 'active';
}
$all_link = l(
	'All Media Types<span class="arrow"></span>',
	$all_url,
	array(
		'attributes' => array (
			'title'	=>	'View all types',
			'class' => $link_classes,
		),
		'html'	=> true,
	)
);

//dsm($view_results);
//dsm($rendered_results);

?>
<?php print $wrapper_prefix; ?>
  <ul class="listing-filter-nav">
		
		<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
		
		<?php
			for ($i=0; $i<$no_results; $i++) {
				$type = $view_results[$i]->node_type_1;
				$type_name = $types[$type];
				$link_classes = array('link');
				if ($type==$arg_type) {
					$link_classes[] = 'active';
				}
				$link = l(
					$type_name.'<span class="arrow"></span>',
					'category/'.$page_args[1].'/'.$page_args[2].'/'.$type.'/all',
					array(
						'attributes' => array (
							'title'	=>	$type_name,
							'class' => $link_classes,
						),
						'html'	=> true,
					)
				);
				echo '<li>'.$link.'</li>';
			}
		?>
		
  </ul>
<?php print $wrapper_suffix; ?>
