<?php

/**
 * page callback with NIDS string
 */
function uportal_backend_edit_content($nids_str) {
	
	global $uportal_backend_data;
	$uportal_backend_data['body_classes'][] = 'bulk-editor-active';
	//$uportal_backend_data['body_classes'][] = 'upload-functionality';
	$uportal_backend_data['page_type'] = 'management_edit_content';
	
	drupal_add_library('system', 'drupal.ajax');
	drupal_add_library('system', 'drupal.form');
	drupal_add_library('system', 'drupal.textarea');
	drupal_add_library('system', 'jquery.form');
	drupal_add_js(drupal_get_path('module', 'maxlength') .'/js/maxlength.js');
	
	$nids = explode('+', $nids_str);
	drupal_set_title('Edit Content');

	//module_load_include('inc', 'node', 'node.pages');
	//$node = node_load($nids[0]);
	//return node_page_edit($node);

	return _uportal_backend_edit_content_html($nids);
}
/** ENDS *****************************************/


/**
 * get content for the edit content page
 * 	- a general framework
 */
function _uportal_backend_edit_content_html($nids) {
	
	$html_str = '<div class="edit-content-wrapper">';
	
	foreach ($nids as $nid) {
		$node = node_load($nid);
		if ($node) {
			$node_status = ($node->status==0) ? 'Draft' : 'Published';
			$node_classes = 'node-edit-wrapper form-collapsed node-type-'.$node->type;
			$node_classes .= ($node->status==0) ? ' status-draft' : ' status-published';
			$node_str = '<div class="'.$node_classes.'" id="node-wrapper-nid-'.$nid.'" data-nid="'.$nid.'" data-node-type="'.$node->type.'">';
			$node_str .= '<div class="node-header clearfix"><div class="icon"></div><div class="title"><span class="content-title">'.$node->title.'</span> <span class="unsaved-msg inline unsaved-form">(Unsaved changes)</span></div><div class="arrow"><div></div></div><div class="node-status">'.$node_status.'</div></div>';
			$node_str .= '<div class="node-form-footer-wrapper">';
			$node_str .= '<div class="node-form-wrapper"><div class="form-errors" id="form-errors-nid-'.$nid.'"></div><div id="node-form-wrapper-nid-'.$nid.'" data-nid="'.$nid.'" data-node-type="'.$node->type.'"></div></div>';
			$node_str .= '<div class="node-footer clearfix">';
			$node_str .= '<div class="unsaved-msg unsaved-changes">This form has unsaved changes.</div>';
			$node_str .= '<a href="#" class="delete-btn btn" data-nid="'.$nid.'">Delete</a>';
			$node_str .= '<a href="#" class="publish-btn btn">Publish</a>';
			$node_str .= '<a href="#" class="save-draft-btn btn">Save Draft</a>';
			$node_str .= '<a href="/node/'.$nid.'" title="View live content" class="preview-btn btn" target="_blank">View Live Content</a>';
			/*
			$node_str .= l('View Live Content', 'node/'.$nid, array(
				'attributes' => array (
					'class' => array('preview-btn', 'btn'),
					'target' => '_blank',
				)
			));
			*/
			$node_str .= '</div>';
			$node_str .= '</div>';
			$node_str .= '</div>';
			$html_str .= $node_str;
			
			//get fake form buttons to initiate Drupal.setttings ajax functionality
			$fakeform = _uportal_backend_get_fake_node_form($nid);
			$submit_ajax_js = $fakeform['#attached']['js'];
			foreach ($submit_ajax_js as $js) {
				drupal_add_js($js['data'], $js['type']);
			}
			
		}
	}
	
	$html_str .= '</div>';
	
	return $html_str;
}
/** ENDS *****************************************/


/**
 * forms handler for edit content page
 *
 *	they saved me:
 *		https://drupal.org/node/752056
 *		** really big help: http://drupal.stackexchange.com/questions/11869/load-ajax-form-in-another-page
 *		http://www.digitalkozak.com/blog/drupal-7-exposing-ajaxified-node-creation-form-visitors
 *		https://drupal.org/node/2009252
 *		http://www.digitalkozak.com/blog/drupal-7-exposing-ajaxified-node-creation-form-visitors
 */
function uportal_backend_edit_content_ajax_handler() {
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'delete_node':
				_uportal_backend_edit_content_delete_node($_POST['data_values']['nid']);
				break;
			case 'get_node_form':
				_uportal_backend_edit_content_get_node_form($_POST['data_values']['nid']);
				break;
			case 'upload_file':
				_uportal_backend_edit_content_upload_files();
				break;
			case 'create_file_object':
				_uportal_backend_edit_content_create_file_object();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * edit content - lesson plans
 */
function uportal_backend_edit_content_ajax_handler_lesson_plans() {
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'get_lesson_plan_for_reorder':
				_uportal_backend_get_lesson_plan_content_for_reorder();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * edit content - series
 */
function uportal_backend_edit_content_ajax_handler_series() {
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'get_series_for_reorder':
				_uportal_backend_get_series_content_for_reorder();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * edit content - read only data
 */
function uportal_backend_edit_content_ajax_handler_read_only() {
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'retrieve_categories_and_series':
				_uportal_backend_get_categories_and_series();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * get hidden node form to help initialise javascript and AJAX
 */
function _uportal_backend_edit_content_get_node_form($nid) {
	module_load_include('inc', 'node', 'node.pages');
	$node = node_load($nid);
	
	//check that thumnails are present
	if (count($node->field_image)<=0) {
		if ($node->type=='video') {
			_uportal_create_node_thumbnail_video($node);
		} elseif ($node->type=='document') {
			_uportal_create_node_thumbnail_document($node);
		}
	}
	
	$node_form = drupal_get_form($node->type.'_node_form_'.$nid, $node);
	
	$form_html = drupal_render($node_form);
	$json_output = array (
		'nid' => $nid,
		'form_html' => $form_html,
	);
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * get node form
 */
function _uportal_backend_get_fake_node_form($nid) {
	module_load_include('inc', 'node', 'node.pages');
	$node = node_load($nid);
	$node_form = drupal_get_form($node->type.'_node_form_'.$nid, $node);
	
	// Process the submit buttons which use ajax
	// submit buttons have been rewritten to have custom ID for each NID to avoid AJAX overwriting
	$node_form['actions']['custom-submit-'.$nid] = ajax_pre_render_element($node_form['actions']['custom-submit-'.$nid]);
	
	//build fake form
	$fakeform = array (
		'#type' => 'markup',
		'#markup' => '<div style="display=none;"></div>',
		'#attached' => $node_form['actions']['custom-submit-'.$nid]['#attached'],
	);
	
	//add on other button settings
	if (isset($node_form['field_image']) && isset($node_form['field_image']['und']) && count($node_form['field_image']['und'])>0) {
		if (isset($node_form['field_image']['und'][0]['remove_button'])) {
			$to_attach = $node_form['field_image']['und'][0]['remove_button']['#attached'];
			foreach ($to_attach as $key=>$attach) {
				foreach ($attach as $attach_elem) {
					$fakeform['#attached'][$key][] = $attach_elem;
				}
			}
		}
		if (isset($node_form['field_image']['und'][0]['upload_button'])) {
			$to_attach = $node_form['field_image']['und'][0]['upload_button']['#attached'];
			foreach ($to_attach as $key=>$attach) {
				foreach ($attach as $attach_elem) {
					$fakeform['#attached'][$key][] = $attach_elem;
				}
			}
		}
	}
	
	return $fakeform;

}
/** ENDS *****************************************/


/**
 * delete node
 */
function _uportal_backend_edit_content_delete_node($nid) {
	$node = node_load($nid);
	$allowed_types = array('document', 'audio', 'video');
	
	if (!in_array($node->type, $allowed_types)) {
		return;
	}
	
	$json_output = array(
		'nid' => $nid,
	);
	node_delete_multiple(array($nid));
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * update or edit node
 */
function uportal_backend_node_edit_form($form, &$form_state) {
	
	//skip validation because of on-the-fly node creation and a whole multitude of errors I was unable to resolve
	//TODO: fix server side validation
  //node_form_validate($form, $form_state);
	//$form_errors = form_get_errors();
	$form_errors = false;
	
  if (!$form_errors) {
    node_form_submit($form, $form_state);
		$ajax_commands = array();
		$ajax_commands[] = array(
			'command' => 'contentFormAfterSaving',
		);
		return array (
			'#type' => 'ajax',
			'#commands' => $ajax_commands,
		);
  } else {
		
		//error html
		$error_html = '<ul class="form-errors-ul">';
		foreach ($form_errors as $error) {
			$error_html .= '<li>'.$error.'</li>';
		}
		$error_html .= '</ul>';
		
		$form_state['rebuild'] = true;
		$ajax_commands = array();
		
		$ajax_commands[] = ajax_command_html("#node-form-wrapper-nid-".$form_state['node']->nid, drupal_render($form));
		$ajax_commands[] = ajax_command_html("#form-errors-nid-".$form_state['node']->nid, $error_html);
		return array (
			'#type' => 'ajax',
			'#commands' => $ajax_commands,
		);
	
	}
}
/** ENDS *****************************************/


/**
 * jQuery File Upload Plugin PHP Example 5.14
 * https://github.com/blueimp/jQuery-File-Upload
 *
 */
function _uportal_backend_edit_content_upload_files() {
	
	module_load_include('inc', 'uportal_backend', 'uportal_backend.upload_handler');
	module_load_include('inc', 'uportal_backend', 'uportal_backend.upload_handler_custom');
	
	//defaults
	$file_field_name = 'field_image';
	$input_param_name = 'new_thumbnail_upload';
	
	//field name
	switch ($_POST['file_type']) {
		case 'thumbnail':
			break;
		case 'content':
			$file_field_name = _uportal_file_field_name_by_content_type($_POST['content_type']);
			$input_param_name = 'new_content_file_upload';
			break; 
	}
	
	//content type
	$files_directory_path = variable_get('file_public_path', conf_path() . '/files').'/';
	$file_field_settings = _uportal_field_settings_given_field_and_type($file_field_name, $_POST['content_type']);
	$folder_path = _uportal_clean_up_string_pathauto_transliterate_tokens($file_field_settings['folder-path']);
	$new_location_dir = $files_directory_path.$folder_path;
	$new_location_uri = 'public://'.$folder_path;
	file_prepare_directory($new_location_dir, FILE_CREATE_DIRECTORY);
	$new_location_dir .= '/';
	
	//ensure unique filename
	$old_filename = $_FILES[$input_param_name]['name'];
	$new_filename = _uportal_clean_up_string_pathauto_transliterate_tokens($old_filename);
	$new_filename = _uportal_get_non_conflicting_filename($new_filename, drupal_realpath($new_location_dir).'/');
	$_FILES[$input_param_name]['name'] = $new_filename;
		
	//upload handler options
	$options = array();
	$options['image_versions'] = array();
	$options['upload_dir'] = $new_location_dir;
	$options['param_name'] = $input_param_name;
	$upload_handler = new UploadHandlerModified($options, false);
	$upload_response = $upload_handler->post(false);
	
	//return response
	$json_data = array();
	if (
		is_array($upload_response)
		&& isset($upload_response[$options['param_name']])
		&& count($upload_response[$options['param_name']])>0
	) {
		
		//we are only dealing with one file in the array so we can risk the [0]
		//we can also risk not splitting the file_ids string
		//bad code ... FIX LATER
		$file = $upload_response[$options['param_name']][0];
		$file_id = $_POST['file_ids'];
		$json_data['files'][0] = array (
			'id' => $file_id,
			'name' => $file->name,
			'path' => $options['upload_dir'],
			'path_uri' => $new_location_uri,
		);
		
	}
	
	drupal_json_output($json_data);

}
/** ENDS *****************************************/


/**
 * create drupal file object and return fid
 */
function _uportal_backend_edit_content_create_file_object() {
	$filepath = $_POST['data_values']['file_path'].$_POST['data_values']['file_name'];
	$file_data = file_get_contents($filepath);
	$destination = $_POST['data_values']['file_path_uri'].'/'.$_POST['data_values']['file_name'];
	$replace = FILE_EXISTS_REPLACE;
	$new_file = file_save_data($file_data, $destination, $replace);
	db_update('file_managed')
		->fields(array('status' => 0))
		->condition('fid', $new_file->fid, '=')
		->execute();
	
	//initiate json data
	$json_data = array (
		'file_id' => $_POST['data_values']['file_id'],
		'fid' => $new_file->fid,
		'filename' => $new_file->filename,
		'filesize' => _uportal_format_bytes($new_file->filesize, 0),
	);
	
	if ($_POST['data_values']['file_type']=='thumbnail') {
		$img_html = theme('image_style', array('style_name' => 'management_edit_content_thumbnail', 'path' => $destination));
		$json_data['img_html'] = $img_html;
	} else {
		$url = file_create_url($destination);
		$json_data['url'] = $url;
		if ($_POST['data_values']['node_type']=='audio') {
			$fileinfo = getID3_analyze(drupal_realpath($destination));
			$time_str = isset($fileinfo['playtime_string']) ? $fileinfo['playtime_string'] : '';
			$json_data['time_str'] = $time_str.' mins';
		}
		if ($_POST['data_values']['node_type']=='document') {
			$pdf_thumbnail_obj = _uportal_backend_edit_content_generate_pdf_thumbnail($new_file);
		}
	}
	
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * generate thumbnail for pdf document
 */
function _uportal_backend_edit_content_generate_pdf_thumbnail($pdf_file_object) {
}
/** ENDS *****************************************/


/**
 * save temporarily stored data to node
 * - called from uportal_backend_field_attach_submit
 */
function _uportal_backend_edit_content_attach_temporary_data($entity_type, $entity, $form, &$form_state) {
	
	//check entity
	$content_types = array('video'=>1, 'audio'=>1, 'document'=>1, );
	if ($entity_type=='node' && !isset($content_types[$entity->type])) {
		return;
	}
	
	//attach new files
	_uportal_backend_edit_content_attach_new_files($entity_type, $entity, $form, $form_state);
	
	//attach node data
	_uportal_backend_edit_content_update_nodequeue($entity_type, $entity, $form, $form_state);
	
}
/** ENDS *****************************************/


/**
 * saving temporary nodequeue order for node
 * - ideally, this should be in hook_update
 * - but, what the heck, it will be saved anyway
 * TODO: bad code, rewrite original module to combine hook_node_update functionality
 */
function _uportal_backend_edit_content_update_nodequeue($entity_type, $entity, $form, $form_state) {
	$node_series_nid = $entity->field_series_reference['und'][0]['target_id'];
	$ordered_series_nid = $_POST['series_ordered_nid'];
	
	//reverse order and enter into queue backwards
	$ordered_series_content_nids = explode(',', $_POST['series_ordered_order']);
	
	if ($node_series_nid == $ordered_series_nid) {
		$series_nid = $node_series_nid;
		$series_parent_queue = nodequeue_load_queue_by_name('series_child_queue');
		$series_subqueue_name = 'series_child_queue_'.$series_nid;
		$subqueues = nodequeue_load_subqueues_by_reference( array (
			$series_parent_queue->qid => array($series_subqueue_name)
		));
		if (count($subqueues)) {
			$series_subqueue = current($subqueues);
			$sqid = $series_subqueue->sqid;
			nodequeue_queue_clear($sqid);
			foreach ($ordered_series_content_nids as $content_nid) {
				nodequeue_subqueue_add($series_parent_queue, $series_subqueue, $content_nid);
			}
		}
	}
}
/** ENDS *****************************************/


/**
 * saving temporarily uploaded file to node
 */
function _uportal_backend_edit_content_attach_new_files($entity_type, $entity, $form, &$form_state) {
	
	$temporary_keys = array('new_thumbnail_fid', 'new_content_file_fid');
	foreach ($temporary_keys as $key) {
		if (isset($_POST[$key])) {
			$new_fid = $_POST[$key];
			
			//test for positive integer
			//http://stackoverflow.com/questions/2559923/shortest-way-to-check-if-a-variable-contains-positive-integer-using-php
			if (!((int)$new_fid == $new_fid && (int)$new_fid > 0)) {
				continue;
			}
			
			switch($key) {
				case 'new_thumbnail_fid':
					_uportal_backend_edit_content_attach_new_thumbnail($entity, $new_fid);
					break;
				case 'new_content_file_fid':
					_uportal_backend_edit_content_attach_new_content_file($entity, $new_fid);
					break;
			}
			
		}
	}
	
}
/** ENDS *****************************************/


/**
 * attach new thumbnail to content
 */
function _uportal_backend_edit_content_attach_new_thumbnail($node, $new_thumbnail_fid) {
	$new_thumbnail_file = file_load($new_thumbnail_fid);
	$new_thumbnail_image = image_load($new_thumbnail_file->uri);
	$node->field_image['und'][0]['fid'] = $new_thumbnail_file->fid;
	$node->field_image['und'][0]['height'] = $new_thumbnail_image->info['height'];
	$node->field_image['und'][0]['width'] = $new_thumbnail_image->info['width'];
}
/** ENDS *****************************************/


/**
 * attach new content file to content
 */
function _uportal_backend_edit_content_attach_new_content_file($node, $new_content_file_fid) {
	switch ($node->type) {
		case 'video':
			$node->field_video['und'][0]['fid'] = $new_content_file_fid;
			break;
		case 'audio':
			$node->field_audio_track['und'][0]['fid'] = $new_content_file_fid;
			break;
	}
}
/** ENDS *****************************************/


/**
 * retrieve lesson plan nodequeue
 */
function _uportal_backend_get_lesson_plan_content_for_reorder() {
	$lesson_plan_nid = $_POST['data_values']['lesson-plan-nid'];
	$lesson_plan_node = node_load($lesson_plan_nid);
	$queue_nids = array();

	if (!$lesson_plan_node) {
		drupal_json_output(array(
			'error' => 'This node does not exist!',
		));
		return;
	}
	
	//get content nids
	$lesson_plans_content_view = views_get_view_result('management_lesson_plans', 'block_lesson_plans_ordered_results', $lesson_plan_nid);
	$lesson_plans_content = array();
	foreach ($lesson_plans_content_view as $content) {
		$nid = $content->nid;
		if (!isset($lesson_plans_content['nid-'.$nid])) {
			$lesson_plans_content['nid-'.$nid] = array (
				'nid' => $nid,
				'title' => $content->node_title,
				'type' => $content->node_type,
				'status' => $content->node_status,
			);
			$queue_nids['nid-'.$nid] = 1;
		}
	}
	$lesson_plan_content_no = count($lesson_plans_content);
	
	//get queue html
	$print_vars = array (
		'lesson_plan_title' => $lesson_plan_node->title,
		'lesson_plan_content_no' => $lesson_plan_content_no,
		'lesson_plan_content' => $lesson_plans_content,
	);
	$template_file_path = drupal_get_path('module', 'uportal_backend').'/templates/lesson-plan-reorder.tpl.php';
	$queue_html = _uportal_backend_load_view($template_file_path, $print_vars);
	
	$json_data = array (
		'lesson_plan_nid' => $lesson_plan_nid,
		'lesson_plan_title' => $lesson_plan_node->title,
		'queue_no_items' => $lesson_plan_content_no,
		'queue_html' => $queue_html,
		'queue_nids' => $queue_nids,
		'initiator' => $_POST['data_values']['initiator'],
	);
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * retrieve series nodequeue
 */
function _uportal_backend_get_series_content_for_reorder() {
	$series_nid = $_POST['data_values']['series-nid'];
	$series_node = node_load($series_nid);
	$queue_nids = array();

	if (!$series_node) {
		drupal_json_output(array(
			'error' => 'This node does not exist!',
		));
		return;
	}
	
	//get content nids
	$series_content_view = views_get_view_result('management_series', 'block_series_ordered_results', $series_nid);
	$series_content = array();
	foreach ($series_content_view as $content) {
		$nid = $content->nid;
		if (!isset($series_content['nid-'.$nid])) {
			$series_content['nid-'.$nid] = array (
				'nid' => $nid,
				'title' => $content->node_title,
				'type' => $content->node_type,
				'status' => $content->node_status,
			);
			$queue_nids['nid-'.$nid] = 1;
		}
	}
	$series_content_no = count($series_content);
	
	//get queue html
	$print_vars = array (
		'series_title' => $series_node->title,
		'series_content_no' => $series_content_no,
		'series_content' => $series_content,
	);
	$template_file_path = drupal_get_path('module', 'uportal_backend').'/templates/series-reorder.tpl.php';
	$queue_html = _uportal_backend_load_view($template_file_path, $print_vars);
	
	$json_data = array (
		'series_nid' => $series_nid,
		'series_title' => $series_node->title,
		'queue_no_items' => $series_content_no,
		'queue_html' => $queue_html,
		'queue_nids' => $queue_nids,
		'initiator' => $_POST['data_values']['initiator'],
	);
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * retrieve categories and series
 */
function _uportal_backend_get_categories_and_series() {
	
	//retrieve categories and series
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('topic', 'series'), 'IN')
		->propertyOrderBy('title', 'DESC');
	$result = $query->execute();
	$nids = array_keys($result['node']);
	$nodes = node_load_multiple($nids);
	
	//categories
	$categories = array();
	foreach ($nodes as $node) {
		
		if ($node->type=='topic') {
			if (!isset($categories['nid-'.$node->nid])) {
				$categories['nid-'.$node->nid] = array (
					'nid' => $node->nid,
					'category_title' => $node->title,
					'series' => array(),
				);
			}
			$categories['nid-'.$node->nid]['category_title'] = $node->title;
		}
		
		if ($node->type=='series') {
			$category_nid = $node->field_topic_reference['und'][0]['target_id'];
			$series_nid = $node->nid;
			$series_title = $node->title;
			if (!isset($categories['nid-'.$category_nid])) {
				$categories['nid-'.$category_nid] = array (
					'nid' => $category_nid,
					'category_title' => '',
					'series' => array(),
				);
			}
			$categories['nid-'.$category_nid]['series']['nid-'.$series_nid] = array (
				'nid' => $series_nid,
				'series_title' => $series_title,
			);
		}
		
	}
	
	drupal_json_output($categories);
}
/** ENDS *****************************************/


/**
 * page build
 */
function _uportal_backend_edit_content_page_build(&$page) {
	$args = arg();
	$nids = explode('+', $args[2]);
	$no_items = count($nids);
	$items_str = ($no_items==1) ? 'item' : 'items';
	
	//get back link
	$back_url = '/management/dashboard';
	if (isset($_GET['from-url'])) {
		$back_url = $_GET['from-url'];
	}
	
	//header bock
	$header_block = array (
		'#type' => 	'container',
		'#attributes' => array( 'class' => array('edit-content-no-items-header', 'clearfix')),
		'no-items-wrapper' => array (
			'#type' => 'container',
			'#attributes' => array('class' => array('no-items')),
			'no-items' => array (
				'#markup' => 'Editing: <span>'.$no_items.'</span> '.$items_str,
			),
		),
		'buttons-wrapper' => array (
			'#type' => 'container',
			'#attributes' => array('class' => array('btns-wrapper')),
			'bulk-editor-btn' => array (
				'#markup' => '<a href="javascript:;" class="btn bulk-editor-btn light-btn" id="bulk-edit-btn">Bulk Edit</a>',
				'#weight' => 20,
			),
			'done-btn' => array (
				'#markup' => '<a class="btn done-btn light-btn" href="'.$back_url.'">Done</a>',
				'#weight' => 10,
			),
		),
	);
	
	$page['header_strip_2'] = $header_block;
}
/** ENDS *****************************************/


/**
 * get existing taxonomy
 */
function _uportal_backend_edit_content_get_existing_taxonomy() {
	$existing_taxonomy = array();
	
	//get existing vocabularies
	$vocabularies = taxonomy_get_vocabularies();
	foreach($vocabularies as $vocabulary) {
		
		//get terms for vocabulary
		$terms = array();
		$tree = taxonomy_get_tree($vocabulary->vid);
		foreach($tree as $term) {
			$terms['tid-'.$term->tid] = array (
				'tid' => $term->tid,
				'vid' => $vocabulary->vid,
				'name' => $term->name,
			);
		}
		
		$existing_taxonomy['vid-'.$vocabulary->vid] = array (
			'vid' => $vocabulary->vid,
			'name' => $vocabulary->name,
			'machine_name' => $vocabulary->machine_name,
			'terms' => $terms,
		);
	}
	
	return $existing_taxonomy;
}
/** ENDS *****************************************/

/**
 * function to create custom field in NODE EDIT form with all tags from all vocabularies
 */
function _uportal_backend_edit_content_create_custom_tags_select(&$form, &$form_state, $form_id) {
	$selected_terms = array();
	$node = $form['#node'];
	$all_terms_options = array(
		'_none' => '- None -',
	);
	
	//get all present terms
	$vocabularies = _uportal_backend_edit_content_get_existing_taxonomy();
	foreach ($vocabularies as $vocabulary) {
		$terms = $vocabulary['terms'];
		
		//get selected terms for this vocabulary
		$vocabulary_field = 'field_'.$vocabulary['machine_name'].'_reference';
		$form_field = $form[$vocabulary_field];
		$vocab_node_selected_terms = $form_field['und']['#default_value'];
		$selected_terms = array_merge($selected_terms, $vocab_node_selected_terms);
		
		foreach ($terms as $term) {
			$all_terms_options[$term['tid']] = $term['name'].' ('.$vocabulary['name'].')';
		}
	}
	
	//get all selected terms
	
	$form['field_all_vocabularies_reference'] = array (
		'#access' => true,
		'#attributes' => array (
			'class' => array (
				'field-type-custom-all-taxonomy-terms',
				'field-widget-options-select',
			),
		),
		'#type' => 'container',
		'und' => array (
			'#default_value' => $selected_terms,
			'#field_name' => 'field_custom_all_taxonomy_terms',
			'#multiple' => true,
			'#options' => $all_terms_options,
			'#title' => 'Tags',
			'#type' => 'select',
		),
	);
}
/** ENDS *****************************************/
