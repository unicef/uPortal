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


$link_class = '';
if ($arg_series_nid=='all') {
	$link_class = 'active';
}
$all_link = '<a class="'.$link_class.'" href="/category/'.$page_args[1].'/all/'.$page_args[3].'/'.$page_args[4].'" title="View all series in this filter">All Series</a>';

//dsm($view_results);
//dsm($rendered_results);

?>
<?php print $wrapper_prefix; ?>

  <ul>
		
		<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
		
		<?php
			for ($i=0; $i<$no_results; $i++) {
				$series_nid = $view_results[$i]->node_field_data_field_series_reference_nid_1;
				$series_title = $view_results[$i]->node_field_data_field_series_reference_title;
				if ($series_nid) {
					$link_class = ($series_nid==$arg_series_nid) ? 'active' : '';
					echo '<li><a href="/category/'.$page_args[1].'/'.$series_nid.'/'.$page_args[3].'/'.$page_args[4].'" title="'.$series_title.'" class="'.$link_class.'">'.$series_title.'</a></li>';
				}
			}
		?>
		
  </ul>
<?php print $wrapper_suffix; ?>
