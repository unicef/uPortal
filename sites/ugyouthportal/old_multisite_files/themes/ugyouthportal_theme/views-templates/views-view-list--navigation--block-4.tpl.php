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
$arg_type = $page_args[3];
$types = array(
	'video' => 'Video',
	'audio' => 'Audio',
	'document' => 'Text',
);
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);


$link_class = '';
if ($arg_type=='all') {
	$link_class = 'active';
}
$all_link = '<a class="'.$link_class.'" href="/category/'.$page_args[1].'/'.$page_args[2].'/all/'.$page_args[4].'" title="View all types">All Media Types</a>';

//dsm($view_results);
//dsm($rendered_results);

?>
<?php print $wrapper_prefix; ?>
  <ul>
		
		<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
		
		<?php
			for ($i=0; $i<$no_results; $i++) {
				$type = $view_results[$i]->node_type_1;
				$type_name = $types[$type];
				$link_class = ($type==$arg_type) ? 'active' : '';
				echo '<li class="type-'.$type.'"><a href="/category/'.$page_args[1].'/'.$page_args[2].'/'.$type.'/'.$page_args[4].'" title="'.$type_name.'" class="'.$link_class.'">'.$type_name.'</a></li>';
			}
		?>
		
  </ul>
<?php print $wrapper_suffix; ?>
