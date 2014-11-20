<?php

/**
 * Implementation of hook_context_default_contexts().
 */
function uportal_backend_context_default_contexts() {
	$contexts = array();
  $path = drupal_get_path('module', 'uportal_backend') . '/contexts';
  $files = drupal_system_listing('/\.inc$/', $path, 'name', 0);
  foreach($files as $file) {
    require $path.'/'.$file->filename;
		if (isset($context)) {
			$contexts[$context->name] = $context;
		}
  }
  return $contexts;
}
/** ENDS *****************************************/

