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
$arg_series_nid = $page_args[2];
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);
$arg3 = isset($page_args[3]) ? $page_args[3] : 'all';
$arg4 = isset($page_args[4]) ? $page_args[4] : 'all';


$link_classes = array('link');
if ($arg_series_nid=='all') {
	$link_classes[] = 'active';
}
$all_link = l(
	'All Content<span class="arrow"></span>',
	'category/'.$page_args[1].'/all/all/all',
	array(
		'attributes' => array (
			'title'	=>	'View all content',
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
				$series_nid = $view_results[$i]->node_field_data_field_series_reference_nid_1;
				$series_title = $view_results[$i]->node_field_data_field_series_reference_title;
				if ($series_nid) {
					$link_classes = array('link');
					if ($series_nid==$arg_series_nid) {
						$link_classes[] = 'active';
					}
					$link = l(
						$series_title.'<span class="arrow"></span>',
						'category/'.$page_args[1].'/'.$series_nid,
						array(
							'attributes' => array (
								'title'	=>	$series_title,
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
<?php print $wrapper_suffix; ?>
