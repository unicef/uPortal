<?php

/**
 * content listing page callback
 */
function uportal_backend_dashboard() {
	//body classes
	global $uportal_backend_data;
	$uportal_backend_data['body_classes'][] = 'upload-functionality';
	
	drupal_set_title('Management Dashboard');
	
	//content
	return '<div id="dashboard-page-content">'.views_embed_view('dashboard', 'block_categories').'</div>';
}
/** ENDS *****************************************/
