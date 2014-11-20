<?php

/**
 * bulk uploader to receive files
 *
 * jQuery File Upload Plugin PHP Example 5.14
 * https://github.com/blueimp/jQuery-File-Upload
 *
 * Copyright 2010, Sebastian Tschan
 * https://blueimp.net
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/MIT
 *
 */
function uportal_backend_bulk_upload() {
	module_load_include('inc', 'uportal_backend', 'uportal_backend.upload_handler');
	module_load_include('inc', 'uportal_backend', 'uportal_backend.upload_handler_custom');
	
	//upload handler options
	$options['upload_dir'] = variable_get('file_public_path', conf_path() . '/files').'/bulk-uploader/';
	
	$upload_handler = new UploadHandlerModified($options);

}
/** ENDS *****************************************/


/**
 * testing node creation script given file
 */
function uportal_backend_bulk_upload_test() {
	$file = new stdClass();
	$file->name = 'small_6.ogv';
	$file->size = 438268;
	$file->type = 'video/ogg';

	_uportal_create_content_given_file($file, 'sites/ugyouthportal/files/bulkuploader/', 100);
	
	dsm($file);
	return 'hello';
}
/** ENDS *****************************************/


/**
 * create nodes from given files
 */
function uportal_backend_bulk_upload_create_nodes() {
	$new_file_names = json_decode($_POST['file_names']);
	$new_file_ids = json_decode($_POST['file_ids']);
	$location_dir = variable_get('file_public_path', conf_path() . '/files').'/bulk-uploader/';
	$key_index = 0;
	$json_output = array(
		'files' => array(),
	);
	foreach ($new_file_names as $new_file_name) {
		if (file_exists($location_dir.$new_file_name)) {
			$file = new stdClass();
			$file->name = $new_file_name;
			$file->type = _uportal_backend_mime_content_type($new_file_name)[0];
			$file->js_file_id = $new_file_ids[$key_index++];
			$json_output['files'][] = _uportal_create_content_given_file($file, $location_dir, $category_nid);
		}
	}
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * create nodes from given files
 */
function uportal_backend_delete_nodes() {
	$nids = $_POST['nids'];
	$file_ids = $_POST['file_ids'];
	$json_output = array(
		'file_ids' => $file_ids,
	);
	node_delete_multiple($nids);
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * delete content or files created by bulk_uploader
 */
function uportal_backend_bulk_upload_cancel_upload() {
	$nids = $_POST['nids'];
	$file_ids = $_POST['file_ids'];
	$finished_file_names = $_POST['finished_file_names'];
	$json_output = array(
		'file_ids' => $file_ids,
	);
	if (count($nids)>0) {
		node_delete_multiple($nids);
	}
	if (count($finished_file_names)>0) {
		$location_dir = variable_get('file_public_path', conf_path() . '/files').'/bulk-uploader/';
		foreach ($finished_file_names as $finished_file_name) {
			unlink($location_dir.$finished_file_name);
		}
	}
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


