<?php

/**
 * page callback to managing categories
 */
function uportal_backend_metadata_categories() {
	global $uportal_backend_data;
	$uportal_backend_data['page_type'] = 'management_metadata_categories';
	$uportal_backend_data['body_classes'][] = 'management-categories';
	
	//content
	return '';
}
/** ENDS *****************************************/


/**
 * page callback to handle category operations
 */
function uportal_backend_metadata_categories_operations() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'order-categories':
				_uportal_backend_metadata_order_categories();
				break;
			case 'add-category':
			case 'edit-category':
				_uportal_backend_metadata_categories_edit_add_category();
				break;
		}
	} else {
		drupal_json_output( array (
			'errors' => 'No operation indicated',
		));
	}
}
/** ENDS *****************************************/


/**
 * page callback to handle category read only operations
 */
function uportal_backend_metadata_categories_operations_read_only() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'get-category-icons':
				_uportal_backend_metadata_categories_get_icons();
				break;
			case 'get-existing-categories':
				_uportal_backend_metadata_categories_get_existing_categories();
				break;
			case 'get-category-menu':
				_uportal_backend_metadata_categories_get_existing_category_menu();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * add or edit category
 */
function _uportal_backend_metadata_categories_edit_add_category() {
	
	module_load_include('content_listing.inc', 'uportal_backend');
	
	global $user;
	//$icons_directory = 'public://category-icons/unused-icons/';
	$icons_directory = drupal_get_path('module', 'uportal_backend').'/images/category-icons/';
	$icon_destination = 'public://category-icons/';
	$category_nid = $_POST['nid'];
	$category_title = $_POST['title'];
	$category_desc = $_POST['body'];
	$category_icon_name = false;
	$category_icon_path = false;
	$icon_destination_path = false;
	$old_icon_name = '';
	if ($_POST['image-name']) {
		$category_icon_name = $_POST['image-name'];
		$category_icon_path = $icons_directory.$category_icon_name;
		$icon_destination_path = $icon_destination.$category_icon_name;
	}
	$operation_type = $_POST['op'];
	$operation_general = '';
	$node = false;

	
	//prepare node
	if ($operation_type=='add-category') {
		$node = new stdClass();
		$node->title = $category_title;
		$node->type = 'topic';
		node_object_prepare($node);
		$node->language = LANGUAGE_NONE;
		$node->uid = $user->uid;
		$node->status = 1;
		$node->promote = 0; //not promoted to front page
		$operation_general = 'add';
	} else {
		$node = node_load($category_nid);
		$operation_general = 'edit';
		if (isset($node->field_category_icon['und'])) {
			$old_icon_name = $node->field_category_icon['und'][0]['filename'];
		}
	}
	
	if ($node) {
		//new values
		$node->title = $category_title;
		
		//category description
		if ($category_desc) {
			$langcode_body_field = field_language('node', $node, 'body');
			$node->body[$langcode_body_field][0] = array (
				'value' => $category_desc,
			);
		}
		
		//category icon
		if ($category_icon_path) {
			$category_icon_file_obj = _uportal_backend_get_managed_file_by_uri($category_icon_path);
			
			//try and create if non-existent
			if (!$category_icon_file_obj) {
				$category_icon_path_real = $category_icon_path;
				//$category_icon_path_real = $category_icon_path;
				$category_icon_file_obj = _uportal_backend_create_managed_file_from_server_path($category_icon_path_real, $icon_destination_path, FILE_EXISTS_REPLACE);
			}
			
			//update if file obj now exists
			if ($category_icon_file_obj) {
				$langcode_icon_field = field_language('node', $node, 'field_category_icon');
				$node->field_category_icon[$langcode_icon_field][0] = array (
					'fid' => $category_icon_file_obj->fid,
					'filename' => $category_icon_file_obj->filename,
					'filemime' => $category_icon_file_obj->filemime,
					'uid' => $user->uid,
					'uri' => $category_icon_file_obj->uri,
					'status' => 1,
					'display' => 1,
				);
			}
		}
		
		//save node
		$node = node_submit($node);
		node_save($node);
	}
	
	//make new row html
	$new_row_html = '';
	if ($operation_general=='add') {
		
		//make category path
		$category_path_args = uportal_backend_content_listing_build_path(array (
			'category_nid' => $node->nid,
			'link_type' => 'category',
		));
	  $category_path = $category_path_args['path'];
		
		$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata-category-row.tpl.php';
		$row_vars = array (
			'category_nid' => $node->nid,
      'icon_name' => $category_icon_name,
      'cnter' => 0,
      'category_path' => $category_path,
      'category_title' => $category_title,
      'category_icon' => $category_icon_path ? '<img src="'.file_create_url($category_icon_path).'">' : '',
      'category_desc' => $category_desc,
      'category_count' => 0,
      'category_real_count' => 0,
		);
		$new_row_html = _uportal_backend_load_view($template_path_row, $row_vars);
	}
	
	//respond
	$json_data = array (
		'op' => $operation_type,
		'op_general' => $operation_general,
		'nid' => $node->nid,
		'title' => $node->title,
		'desc' => $category_desc,
    'old_icon_name' => $old_icon_name,
		'icon_name' => $category_icon_name,
		'icon_html' => $category_icon_path ? '<img src="'.file_create_url($category_icon_path).'">' : '',
		'new_row_html' => $new_row_html,
		'initiator' => $_POST['initiator'],
	);
	drupal_json_output($json_data);
	
}
/** ENDS *****************************************/


/**
 * reorder categories
 */
function _uportal_backend_metadata_order_categories() {
	module_load_include('inc', 'uportal_backend', 'uportal_backend.queue_operations');
	$nids = $_POST['nids'];
	_uportal_backend_rearrange_queue('topic', $nids);
	drupal_json_output( array (
		'success' => true,
		'nids' => $nids,
	));
}
/** ENDS *****************************************/


/**
 * build categories page
 */
function _uportal_backend_metadata_categories_page_build(&$page) {
	$page_blocks = array (
		'content' => array (
			array('module'=>'views', 'delta'=>'cdb672f71c3a63da008d1e0f6ebde22b'),
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
 * get category icons
 */
function _uportal_backend_metadata_categories_get_icons() {
	
	//all icons
	$directory = drupal_get_path('module', 'uportal_backend').'/images/category-icons/';
	//$directory = variable_get('file_public_path', conf_path().'/files').'/category-icons/unused-icons/';
	$all_files = array_diff(scandir($directory), array('..', '.'));
	$all_icons_files = array();
	foreach ($all_files as $filename) {
		$extension = explode('.', $filename)[1];
		if ($extension=='png') {
			$all_icons_files[] = $filename;
		}
	}
	
	//get category icons under use
	$used_icons_directory = variable_get('file_public_path', conf_path().'/files').'/category-icons/';
	$used_icons_all_files = array_diff(scandir($used_icons_directory), array('..', '.'));
	$used_icons_files = array();
	foreach ($used_icons_all_files as $used_icon_filename) {
		$extension = explode('.', $used_icon_filename)[1];
		if ($extension=='png') {
			$used_icons_files[] = $used_icon_filename;
		}
	}
	
	//get only unused icons
	$unused_icons_files = array_diff($all_icons_files, $used_icons_files);
	
	drupal_json_output(array (
		'files' => $all_icons_files,
		'usedFiles' => $used_icons_files,
		'path' => '/'.$directory,
	));
}
/** ENDS *****************************************/


/**
 * get categories
 */
function _uportal_backend_metadata_categories_get_existing_categories() {
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'topic');
	$result = $query->execute();
	
	$categories = array();
	if (isset($result['node'])) {
		$nids = array_keys($result['node']);
	  $nodes = entity_load('node', $nids);
		foreach($nodes as $category) {
			$categories['nid-'.$category->nid] = $category->title;
		}
	}
	
	drupal_json_output(array (
		'categories' => $categories,
	));
}
/** ENDS *****************************************/


/**
 * get existing category menu
 */
function _uportal_backend_metadata_categories_get_existing_category_menu() {
	$menu = views_embed_view('navigation', 'block_topics');
	drupal_json_output(array (
		'menu_html' => $menu,
	));
}
/** ENDS *****************************************/

