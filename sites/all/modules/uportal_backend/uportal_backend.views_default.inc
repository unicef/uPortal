<?php

/**
 * Implementation of hook_context_default_contexts().
 */
function uportal_backend_views_default_views() {
	$views = array();
  $path = drupal_get_path('module', 'uportal_backend') . '/views';
  $files = drupal_system_listing('/\.inc$/', $path, 'name', 0);
  foreach($files as $file) {
    require $path.'/'.$file->filename;
		if (isset($view)) {
			$views[$view->name] = $view;
		}
  }
  return $views;
}
/** ENDS *****************************************/

