<?php


/**
 * page callback for importing content
 */
function uportal_backend_import_content() {
	$print_vars = array (
		'series_title' => $series_node->title,
		'series_content_no' => $series_content_no,
		'series_content' => $series_content,
	);
	$template_file_path = drupal_get_path('module', 'uportal_backend').'/templates/series-reorder.tpl.php';
	$queue_html = _uportal_backend_load_view($template_file_path, $print_vars);
}
/** ENDS *****************************************/


/**
 * page callback for batch importing content
 */
function uportal_backend_batch_import_content() {
	if (!isset($_SESSION['batch-info'])) {
		drupal_goto('management/dashboard');
		return;
	}
	
	$batch_info = $_SESSION['batch-info'];
	
	//redirect info
	$redirect = array (
		'type' => 'drupal',
		'url' => array(
			'path' => 'management/dashboard',
			'query' => array(),
		),
	);
	if (isset($batch_info['redirect-url'])) {
		$url_parts = explode('?', $batch_info['redirect-url']);
		$url_params = explode('&', $url_parts[1]);
		$query_params = array();
		foreach($url_params as $url_param) {
			$url_param_parts = explode('=', $url_param);
			$query_params[$url_param_parts[0]] = $url_param_parts[1];
		}
		$redirect['url'] = array (
			'path' => $url_parts[0],
			'query' => $query_params,
		);
	}
	$_SESSION['batch-update-redirect'] = $redirect;
	
	switch ($batch_info['operation-type']) {
		case 'bulk-node-edit':
			batch_set_up_bulk_node_edit($batch_info);
			break;
	}
	
}
/** ENDS *****************************************/


/**
 * set up bulk node edit batch process
 */
function batch_set_up_bulk_node_edit($batch_info) {
	
	//get NIDs to be operated
	$old_nids_data = $batch_info['old-nids-data'];
	$to_edit = false;
	$nids = array();
	$new_data = $batch_info['new-data'];
	
	//build query
	$child_nodes_query = new EntityFieldQuery();
	$child_nodes_query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('video','audio','document'));
	
	//series nid
	if(
		isset($old_nids_data['series-nid'])
		&& is_numeric($old_nids_data['series-nid'])
		&& $old_nids_data['series-nid']>0
	) {
		$series_nid = $old_nids_data['series-nid'];
		$child_nodes_query->fieldCondition('field_series_reference', 'target_id', $series_nid, '=');
		$to_edit = true;
	}
	
	//get nids
	if ($to_edit) {
		$child_nodes_result = $child_nodes_query->execute();
		if (isset($child_nodes_result['node'])) {
			$nids = array_keys($child_nodes_result['node']);
		}
	}
	
	if (count($nids)<=0 || !$to_edit) {
		drupal_goto($_SESSION['batch-update-redirect']['url']['path'], $_SESSION['batch-update-redirect']['url']['query']);
		return;
	}
	
	//break up NIDs into 10 nodes per chunk
  $operations = array();
	$chunk_no = 10;
	$chunks = array_chunk($nids, $chunk_no);
  $count_chunks = count($chunks);
	
	//update each chunk of nodes
	$i = 0;
  foreach ($chunks as $chunk) {
    $i++;
		$operation_details = array (
			'details' => t(
				'Updating chunk @chunk of @count. Each chunk has @chunk_no nodes',
				array (
					'@chunk' => $i,
					'@count'=>$count_chunks,
					'@chunk_no'=>$chunk_no,
				)
			),
		);
    $operations[] = array("batch_bulk_edit_nodes", array($chunk , $new_data, $operation_details));
  }
	
	$batch = array(
    'operations' => $operations,
    'finished' => 'batch_bulk_edit_nodes_finished',
    'title' => t('Nodes Bulk Editor'),
    'init_message' => t('Beginning to bulk edit selected nodes.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Nodes Bulk Editor has experienced an error.'),
    'file' => drupal_get_path('module', 'uportal_backend') . '/uportal_backend.batch_update.inc',
  );
  batch_set($batch);
  batch_process($_SESSION['batch-update-redirect']['url']);
}
/** ENDS *****************************************/


/**
 * process nodes in bulk
 */
function batch_bulk_edit_nodes($nids, $new_data, $operation_details, $context) {
	
	//update message
	$context['message'] = $operation_details['details'];
	
	//edit nodes
	$nodes = entity_load('node', $nids);
	foreach ($nodes as $node) {
		$edited = false;
		
		//if new category
		$category_nid = isset($new_data['category-nid']) ? $new_data['category-nid'] : false;
		if ($category_nid && is_numeric($category_nid) && intval($category_nid)>0) {
			$category_field_langcode = field_language('node', $node, 'field_topic_reference', NULL);
			$node->field_topic_reference[$category_field_langcode] = array( array('target_id' => $category_nid) );
			$edited = true;
		}
		
		//if new series
		$series_nid = isset($new_data['series-nid']) ? $new_data['series-nid'] : false;
		if ($series_nid && is_numeric($series_nid) && intval($series_nid)>0) {
			$series_field_langcode = field_language('node', $node, 'field_series_reference', NULL);
			$node->field_series_reference[$series_field_langcode] = array( array('target_id' => $series_nid) );
			$edited = true;
		}
		
		//save node
		if ($edited) {
			node_save($node);
		}
		
	}

}
/** ENDS *****************************************/


/**
 * finished processing all nodes
 */
function batch_bulk_edit_nodes_finished($success, $results, $operations) {
	drupal_set_message('Nodes bulk updated.');
	drupal_goto(
		$_SESSION['batch-update-redirect']['url']['path'],
		array('query' => $_SESSION['batch-update-redirect']['url']['query'])
	);
}
/** ENDS *****************************************/
