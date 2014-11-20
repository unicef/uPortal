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
$arg_language = $page_args[4];
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);


$link_class = '';
if ($arg_language=='all') {
	$link_class = 'active';
}
$all_link = '<a class="'.$link_class.'" href="/category/'.$page_args[1].'/'.$page_args[2].'/'.$page_args[3].'/all" title="View all languages">All Languages</a>';

//dsm(get_defined_vars());

?>
<?php print $wrapper_prefix; ?>
  <ul>
		
		<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
		
		<?php
			for ($i=0; $i<$no_results; $i++) {
				$language_code = $view_results[$i]->node_language_1;
				$language_name = $rendered_results[$i]['language_1'];
				$link_class = ($language_code==$arg_language) ? 'active' : '';
				echo '<li><a href="/category/'.$page_args[1].'/'.$page_args[2].'/'.$page_args[3].'/'.$language_code.'" title="'.$language_name.'" class="'.$link_class.'">'.$language_name.'</a></li>';
			}
		?>
		
  </ul>
<?php print $wrapper_suffix; ?>
