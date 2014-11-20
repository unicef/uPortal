<?php

/**
 * page callback - manage users
 */
function uportal_backend_manage_users_list() {
	global $uportal_backend_data;
	$uportal_backend_data['page_type'] = 'management_users_list';
	$uportal_backend_data['body_classes'][] = 'page-management-metadata';
	$uportal_backend_data['body_classes'][] = 'management-users-list';
	
	//content
	return '';
}
/** ENDS *****************************************/


/**
 * build categories page
 */
function _uportal_backend_manage_users_list_page_build(&$page) {
	$page_blocks = array (
		'content' => array (
			array('module'=>'views', 'delta'=>'9e7400dcf5b5fa0f7a84233a20060dcc'),
		),
	);
	foreach ($page_blocks as $region => $region_blocks_info) {
		$region_blocks = array();
		foreach ($region_blocks_info as $block_info) {
			$region_blocks[] = block_load($block_info['module'], $block_info['delta']);
		}
		$page[$region] = _block_get_renderable_array(_block_render_blocks($region_blocks));
	}
}
/** ENDS *****************************************/


/**
 * page callback - delete user
 */
function uportal_backend_manage_users_delete() {
	$uid = $_POST['data_values']['uid'];
	$json_output = array(
		'uid' => $uid,
	);
	user_delete_multiple(array($uid));
	drupal_json_output($json_output);
}
/** ENDS *****************************************/
