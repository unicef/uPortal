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
$arg_language = isset($page_args[4]) ? $page_args[4] : 'all';
$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);
$arg3 = isset($page_args[3]) ? $page_args[3] : 'all';
$arg4 = isset($page_args[4]) ? $page_args[4] : 'all';

$all_url = 'category/'.$page_args[1].'/'.$page_args[2].'/'.$arg3.'/all';
if (is_numeric($page_args[2]) && $arg3=='all') { //if showing all types and series is active, just point to series page
	$all_url = 'category/'.$page_args[1].'/'.$page_args[2];
}

$link_classes = array('link');
if ($arg_language=='all') {
	$link_classes[] = 'active';
}
$all_link = l(
	'All Languages<span class="arrow"></span>',
	$all_url,
	array(
		'attributes' => array (
			'title'	=>	'View all languages',
			'class' => $link_classes,
		),
		'html'	=> true,
	)
);

?>
<?php print $wrapper_prefix; ?>
  <ul class="listing-filter-nav">
		
		<?php print '<li class="all-link">'.$all_link.'</li>'; ?>
		
		<?php
			for ($i=0; $i<$no_results; $i++) {
				$language_code = $view_results[$i]->node_language_1;
				$language_name = $rendered_results[$i]['language_1'];
				$link_classes = array('link');
				if ($language_code==$arg_language) {
					$link_classes[] = 'active';
				}
				$link = l(
					$language_name.'<span class="arrow"></span>',
					'category/'.$page_args[1].'/'.$page_args[2].'/'.$arg3.'/'.$language_code,
					array(
						'attributes' => array (
							'title'	=>	$language_name,
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
