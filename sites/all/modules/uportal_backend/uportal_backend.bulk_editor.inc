<?php

/**
 * main function to handle all bulk editor operations which are received by POST
 */
function uportal_backend_bulk_editor() {
	switch ($_POST['operation_type']) {
		case 'delete':
			uportal_backend_bulk_editor_delete_nodes();
			break;
		case 'bulk-edit-metadata':
			_uportal_backend_bulk_editor_edit_metadata();
			break;
		case 'bulk-edit-node-status':
			_uportal_backend_bulk_editor_edit_node_status();
			break;
		case 'bulk-edit-series':
			_uportal_backend_bulk_editor_edit_series();
			break;
		case 'bulk-edit-lesson-plans':
			_uportal_backend_bulk_editor_edit_lesson_plans();
			break;
		case 'bulk-edit-tags':
			_uportal_backend_bulk_editor_edit_tags();
			break;
	}
}
/** ENDS *****************************************/


/**
 * lesson plan operations
 */
function uportal_backend_bulk_editor_lesson_plans() {
	switch ($_POST['operation_type']) {
		case 'bulk-edit-new-lesson-plan':
			_uportal_backend_bulk_editor_create_edit_lesson_plan();
			break;
		case 'save-lesson-plan-order':
			_uportal_backend_bulk_editor_save_lesson_plan_order();
			break;
		case 'delete-lesson-plan':
			_uportal_backend_bulk_editor_delete_lesson_plan();
			break;
	}
}
/** ENDS *****************************************/


/**
 * series operations
 */
function uportal_backend_bulk_editor_series() {
	switch ($_POST['operation_type']) {
		case 'save-series-order':
			_uportal_backend_bulk_editor_save_series_order();
			break;
		case 'bulk-edit-new-series':
			_uportal_backend_bulk_editor_create_edit_series();
			break;
		case 'delete-series':
			_uportal_backend_bulk_editor_delete_series();
			break;
	}
}
/** ENDS *****************************************/


/**
 * read only operations
 */
function uportal_backend_bulk_editor_read_only() {
	switch ($_POST['operation_type']) {
		case 'get_json_data':
			_uportal_backend_bulk_editor_get_json_data();
			break;
		case 'get-existing-series-lesson-plans':
			_uportal_backend_bulk_editor_get_existing_series_lesson_plans();
			break;
	}
}
/** ENDS *****************************************/


/**
 * bulk edit metadata
 */
function _uportal_backend_bulk_editor_edit_metadata() {
	$nids = $_POST['edit-data']['nids'];
	$content_provider_nid = $_POST['edit-data']['content-provider'];
	$language = $_POST['edit-data']['language'];
	$category_nid = $_POST['edit-data']['category'];
	
	$languages = locale_language_list('name');
	$language_name = $languages[$language];
	$content_provider_node = node_load($content_provider_nid);
	$category_node = node_load($category_nid);
	
	$edited = false;
	$json_language = 0;
	$json_category = 0;
	$json_content_provider = 0;
	$json_category_nids_changed = array();
	
	//edit multiple NIDs
	foreach ($nids as $nid) {
		$edited = false;
		$node = node_load($nid);
		
		if (isset($languages[$language])) {
			$node->language = $language;
			$json_language = array (
				'code' => $language,
				'name' => $language_name,
			);
			$edited = true;
		}
		
		//category field
		if (is_numeric($category_nid) && intval($category_nid)>0) {
			$category_field_langcode = field_language('node', $node, 'field_topic_reference', NULL);
			$old_category_nid = $node->field_topic_reference[$category_field_langcode][0]['target_id'];
			
			//if old category is the same as the new category, do nothing
			if ($old_category_nid != $category_nid) {
				$node->field_topic_reference[$category_field_langcode] = array( array('target_id' => $category_nid) );
				
				//remove series
				$series_field_langcode = field_language('node', $node, 'field_series_reference', NULL);
				$node->field_series_reference[$series_field_langcode] = array();
				
				$json_category = array (
					'nid' => $category_nid,
					'title' => $category_node->title,
				);
				$json_category_nids_changed[] = $nid;
				$edited = true;
			}
		}
		
		//content provider field
		if (is_numeric($content_provider_nid) && intval($content_provider_nid)>0) {
			$content_provider_field_langcode = field_language('node', $node, 'field_content_provider_reference', NULL);
			$node->field_content_provider_reference[$content_provider_field_langcode] = array( array('target_id' => $content_provider_nid) );
			$json_content_provider = array (
				'nid' => $content_provider_nid,
				'title' => $content_provider_node->title,
			);
			$edited = true;
		}
		
		//save node
		if ($edited) {
			node_save($node);
		}
	}
	
	//return JSON with NIDS and changed values
	$json_data = array (
		'nids' => $nids,
		'data_values' => array (
			'category' => $json_category,
			'content_provider' => $json_content_provider,
			'language' => $json_language,
			'nids_category_changed' => $json_category_nids_changed,
		),
	);
	
	//return json data
	drupal_json_output($json_data);
	
}
/** ENDS *****************************************/


/**
 * check if node can be published
 */
function _uportal_backend_bulk_editor_check_node_to_publish($node, $content_providers, $topics, $languages) {
	$node_can_be_published =
		(isset($node->body['und']) && count($node->body['und']) && $node->body['und'][0]['value'])
		&& (isset($node->field_topic_reference['und']) && count($node->field_topic_reference['und']) && $node->field_topic_reference['und'][0]['target_id']) && isset($topics['nid-'.$node->field_topic_reference['und'][0]['target_id']])
		&& (isset($node->field_content_provider_reference['und']) && count($node->field_content_provider_reference['und']) && $node->field_content_provider_reference['und'][0]['target_id']) && isset($content_providers['nid-'.$node->field_content_provider_reference['und'][0]['target_id']])
		&& ($node->language && $node->language != 'und' && isset($languages[$node->language]))
		;
	return $node_can_be_published;
}
/** ENDS *****************************************/


/**
 * bulk edit metadata
 */
function _uportal_backend_bulk_editor_edit_node_status() {
	$nids = $_POST['edit-data']['nids'];
	$new_status = $_POST['edit-data']['status'];
	$json_nids_unchanged = array();
	$json_nids_changed = array();
	$json_nids_failed = array();
	
	//content providers, topics, languages
	$languages = locale_language_list('name');

	$content_providers = array();
	$valid_provider_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'content_provider', '=')
		->execute()
		->fetchCol();
	foreach ($valid_provider_nids as $provider_nid) {
		$content_providers['nid-'.$provider_nid] = 1;
	}

	$topics = array();
	$valid_topic_nids = db_select('node', 'n')
		->fields('n', array('nid'))
		->condition('n.type', 'topic', '=')
		->execute()
		->fetchCol();
	foreach ($valid_topic_nids as $topic_nid) {
		$topics['nid-'.$topic_nid] = 1;
	}
	
	//edit multiple NIDs
	foreach ($nids as $nid) {
		$node = node_load($nid);
		
		//no need to bother with this node if status is already as desired
		if ($node->status==$new_status) {
			$json_nids_unchanged[] = $nid;
			continue;
		}
		
		//if new status is published
		if ($new_status=='1') {
			$can_be_published = _uportal_backend_bulk_editor_check_node_to_publish($node, $content_providers, $topics, $languages);
			if (!$can_be_published) {
				$json_nids_failed[] = $nid;
				continue;
			}
		}
		
		//update node using $_POST['custom_node_status'] which is used to override the node status in the pre_save hook - needed for weird publish permissions
		if ($new_status=='0' || ($new_status=='1' && $can_be_published)) {
			$node->status = $_POST['custom_node_status'] = $new_status;
			node_save($node);
			$json_nids_changed[] = $nid;
		} else {
			$json_nids_failed[] = $nid;
		}
	}
	
	//return JSON with NIDS and changed values
	$json_data = array (
		'status' => $new_status,
		'all_nids' => $nids,
		'changed_nids' => $json_nids_changed,
		'unchanged_nids' => $json_nids_unchanged,
		'failed_nids' => $json_nids_failed,
	);
	
	//return json data
	drupal_json_output($json_data);
	
}
/** ENDS *****************************************/


/**
 * bulk editor's version of creating a new lesson plan
 */
function _uportal_backend_bulk_editor_create_edit_lesson_plan() {
	global $user;
	$node_language = LANGUAGE_NONE;
	$lesson_plan_title = $_POST['edit-data']['title'];
	$lesson_plan_body = $_POST['edit-data']['body'];
	$lesson_plan_nid = $_POST['edit-data']['lesson_plan_nid'];
	$edit_operation = 0;
	$old_lesson_plan_title = '';

	if (is_numeric($lesson_plan_nid) && $lesson_plan_nid>0) {
		$lesson_plan_node = node_load($lesson_plan_nid);
		$old_lesson_plan_title = $lesson_plan_node->title;
		$lesson_plan_node->title = $lesson_plan_title;
		$edit_operation = 1;
	} else {
		$lesson_plan_node = new stdClass();
		$lesson_plan_node->title = $lesson_plan_title;
		$lesson_plan_node->type = 'lesson_plan';
		node_object_prepare($lesson_plan_node);
		$lesson_plan_node->language = $node_language;
		$lesson_plan_node->uid = $user->uid;
		$lesson_plan_node->status = 1; //published
	}
	
	$lesson_plan_node->body[$node_language] = array(
		array(
			'value' => $lesson_plan_body,
		),
	);
	$lesson_plan_node = node_submit($lesson_plan_node);
	node_save($lesson_plan_node);
	
	$json_data = array (
		'data_values' => array (
			'nid' => $lesson_plan_node->nid,
			'title' => $lesson_plan_node->title,
			'body' => $lesson_plan_body,
			'initiator' => $_POST['edit-data']['initiator'],
			'meta_data_type' => 'lesson-plan',
			'edit_operation' => $edit_operation,
			'old_lesson_plan_title' => $old_lesson_plan_title,
			'lesson_plan_node_html' => '<div id="lesson-plan-listing-item-nid-'.$lesson_plan_node->nid.'" class="clearfix lesson-plan-listing-item content-listing-item series-lesson-plan-listing-item" data-updated="" data-created="" data-category-nid="" data-lesson-plan-nid="'.$lesson_plan_node->nid.'"><div class="thumbnail lesson-plan-thumbnails"></div><div class="desc"><div class="clearfix lesson-plan-title-wrapper title-wrapper"><h2>'.$lesson_plan_node->title.'</h2><div class="edit-operations"><div class="edit-lesson-plan edit-btn" data-lesson-plan-nid="'.$lesson_plan_node->nid.'"><div class="icon"></div><div class="txt">Edit Plan</div></div><div class="order-lesson-plan order edit-btn" data-category-nid="" data-lesson-plan-nid="'.$lesson_plan_node->nid.'"><div class="icon"></div><div class="txt">Order Plan</div></div></div></div><div class="category-numbers-wrapper clearfix"><div class="numbers">0 items</div></div><div class="body">'.$lesson_plan_body.'</div></div></div>',
		),
	);
	
	//return json data
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * get series by category
 */
function _uportal_backend_bulk_editor_get_series_by_category() {
	
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
 * deleting series
 */
function _uportal_backend_bulk_editor_delete_series() {
	$series_nid = $_POST['nid'];
	$json_output = array(
		'series_nid' => $series_nid,
	);
	node_delete_multiple(array($series_nid));
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * deleting series
 */
function _uportal_backend_bulk_editor_delete_lesson_plan() {
	$lesson_plan_nid = $_POST['nid'];
	$json_output = array(
		'lesson_plan_nid' => $lesson_plan_nid,
	);
	node_delete_multiple(array($lesson_plan_nid));
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * bulk editor's version of creating a new series
 */
function _uportal_backend_bulk_editor_create_edit_series() {
	global $user;
	$node_language = LANGUAGE_NONE;
	$series_title = $_POST['edit-data']['title'];
	$series_body = $_POST['edit-data']['body'];
	$series_category_nid = $_POST['edit-data']['category'];
	$series_nid = $_POST['edit-data']['series_nid'];
	$edit_operation = 0;
	$old_series_title = '';
	$old_series_category_nid = '';
	$category_node = node_load($series_category_nid);
	$perform_server_bulk_update = false;
	$maximum_nodes_to_update_in_one_pass = 10;
	$from_url = (isset($_POST['edit-data']['current-url'])) ? $_POST['edit-data']['current-url'] : 'management/dashboard';
	
	if (is_numeric($series_nid) && $series_nid>0) {
		$series_node = node_load($series_nid);
		$old_series_title = $series_node->title;
		$old_series_category_nid = $series_node->field_topic_reference['und'][0]['target_id'];
		$series_node->title = $series_title;
		$edit_operation = 1;
	} else {
		$series_node = new stdClass();
		$series_node->title = $series_title;
		$series_node->type = 'series';
		node_object_prepare($series_node);
		$series_node->language = $node_language;
		$series_node->uid = $user->uid;
		$series_node->status = 1; //published
	}
	
	$series_node->body[$node_language] = array(
		array(
			'value' => $series_body,
		),
	);
	$series_node->field_topic_reference[$node_language] = array(
		array (
			'target_id' => $series_category_nid,
			'target_type' => 'node',
		),
	);
	$series_node = node_submit($series_node);
	node_save($series_node);
	
	//post data to bulk operation: change category for all child nodes if nodes are more than maximum allowed in one pass
	
	//update series content to new category if the category has changed
	if (
		$edit_operation
		&& $old_series_category_nid!=$category_node->nid
	) {
		
		//get child nodes
		$child_nodes_query = new EntityFieldQuery();
		$child_nodes_query->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('video','audio','document'))
			->fieldCondition('field_series_reference', 'target_id', $series_nid, '=');
		$child_nodes_result = $child_nodes_query->execute();
		
		if (isset($child_nodes_result['node'])) {
			$child_nids = array_keys($child_nodes_result['node']);
			if (count($child_nids)>$maximum_nodes_to_update_in_one_pass) {
				$_SESSION['batch-info'] = array (
					'operation-type' => 'bulk-node-edit',
					'redirect-url' => $from_url,
					'new-data' => array (
						'category-nid' => $category_node->nid,
					),
					'old-nids-data' => array (
						'series-nid' => $series_nid,
					),
				);
				$perform_server_bulk_update = true;
			} else {
				$child_nodes = entity_load('node', $child_nids);
				foreach ($child_nodes as $child_node) {
					$category_field_langcode = field_language('node', $child_node, 'field_topic_reference', NULL);
					$child_node->field_topic_reference[$category_field_langcode] = array( array('target_id' => $category_node->nid) );
					node_save($child_node);
				}
			}
		}
	}
	
	$json_data = array (
		'data_values' => array (
			'nid' => $series_node->nid,
			'body' => $series_body,
			'category_nid' => $series_category_nid,
			'category_title' => $category_node->title,
			'title' => $series_node->title,
			'edit_operation' => $edit_operation,
			'old_series_title' => $old_series_title,
			'old_series_category_nid' => $old_series_category_nid,
			'initiator' => $_POST['edit-data']['initiator'],
			'perform_server_bulk_update' => $perform_server_bulk_update,
			'series_node_html' => '<div id="series-listing-item-nid-'.$series_node->nid.'" class="clearfix series-listing-item content-listing-item series-lesson-plan-listing-item" data-updated="" data-created="" data-category-nid="'.$series_category_nid.'" data-series-nid="'.$series_node->nid.'"><div class="thumbnail series-thumbnails"></div><div class="desc"><div class="clearfix series-title-wrapper title-wrapper"><h2>'.$series_node->title.'</h2><div class="edit-operations"><div class="edit-series edit-btn" data-series-nid="'.$series_node->nid.'"><div class="icon"></div><div class="txt">Edit Series</div></div><div class="order-series edit-btn" data-category-nid="'.$series_category_nid.'" data-series-nid="'.$series_node->nid.'"><div class="icon"></div><div class="txt">Order Series</div></div></div></div><div class="category-numbers-wrapper clearfix"><div class="category-title">'.$category_node->title.'</div><div class="numbers">0 items</div></div><div class="body">'.$series_body.'</div></div></div>',
		),
	);
	
	//return json data
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * bulk edit tags
 */
function _uportal_backend_bulk_editor_edit_tags() {
	$nids = $_POST['edit-data']['nids'];
	$tids = $_POST['edit-data']['tags'];
	$remove_old_tags = $_POST['edit-data']['remove-old-tags']==='true';
	$terms = taxonomy_term_load_multiple($tids);
	$vocabularies = taxonomy_get_vocabularies();
	$terms_by_vocab = array();
	
	//group terms by vocab
	foreach ($terms as $term) {
		if (!isset($terms_by_vocab['vid-'.$term->vid])) {
			$terms_by_vocab['vid-'.$term->vid] = array();
		}
		$terms_by_vocab['vid-'.$term->vid][] = $term->tid;
	}
	
	//edit multiple NIDs
	foreach ($nids as $nid) {
		$edited = false;
		$node = node_load($nid);
		
		//by each vocabulary
		foreach ($vocabularies as $vocab) {
			$field_name = 'field_'.$vocab->machine_name.'_reference';
			if ($remove_old_tags) { //remove old tags
				if (isset($node->{$field_name}['und'])) {
					$node->{$field_name}['und'] = array();
				}
				if (!isset($terms_by_vocab['vid-'.$vocab->vid])) {
					$node->{$field_name} = array();
				} else {
					$node->{$field_name}['und'] = array();
				}
			}
			
			//insert tids
			$vocab_tids = $terms_by_vocab['vid-'.$vocab->vid];
			if (!isset($node->{$field_name}['und'])) {
				$node->{$field_name}['und'] = array();
			}
			foreach ($vocab_tids as $tid) {
				$node->{$field_name}['und'][] = array( 'tid' => $tid );
			}

		}
		
		//save node
		node_save($node);
		
	}
	
	//return JSON with NIDS and changed values
	$json_data = array (
		'nids' => $nids,
		'removed_old_tags' => $remove_old_tags,
		'data_values' => array (
			'tids' => $tids,
		),
	);
	
	//return json data
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * bulk edit lesson plans
 */
function _uportal_backend_bulk_editor_edit_lesson_plans() {
	$nids = $_POST['edit-data']['nids'];
	$lesson_plan_nids = $_POST['edit-data']['lesson-plans'];
	$remove_old_lesson_plans = $_POST['edit-data']['remove-old-lesson-plans']==='true';
	
	$json_lesson_plans = $lesson_plan_nids;
	
	//edit multiple NIDs
	foreach ($nids as $nid) {
		$edited = false;
		$node = node_load($nid);
		
		//lesson plan field
		$lesson_plan_field_langcode = field_language('node', $node, 'field_lesson_plan_reference', NULL);
		if ($remove_old_lesson_plans) {
			$node->field_lesson_plan_reference[$lesson_plan_field_langcode] = array();
		}
		foreach ($lesson_plan_nids as $lesson_plan_nid) {
			if (is_numeric($lesson_plan_nid) && intval($lesson_plan_nid)>0) {
				$node->field_lesson_plan_reference[$lesson_plan_field_langcode][] = array('target_id' => $lesson_plan_nid);
				$edited = true;
			}
		}
		
		//save node
		if ($edited) {
			node_save($node);
		}
	}
	
	//return JSON with NIDS and changed values
	$json_data = array (
		'nids' => $nids,
		'removed_old_lesson_plans' => $remove_old_lesson_plans,
		'data_values' => array (
			'lesson_plans' => $json_lesson_plans,
		),
	);
	
	//return json data
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * bulk edit series
 */
function _uportal_backend_bulk_editor_edit_series() {
	$nids = $_POST['edit-data']['nids'];
	$series_nid = $_POST['edit-data']['series'];
	$category_nid = $_POST['edit-data']['category'];
	$series_node = node_load($series_nid);
	$category_node = node_load($category_nid);
	
	$edited = false;
	$json_series = 0;
	$json_category = 0;
	$json_nids_changed = array();
	
	//edit multiple NIDs
	foreach ($nids as $nid) {
		$node = node_load($nid);
		$series_field_langcode = field_language('node', $node, 'field_series_reference', NULL);
		$category_field_langcode = field_language('node', $node, 'field_topic_reference', NULL);
		
		//original values
		$old_category_nid = $node->field_topic_reference[$category_field_langcode][0]['target_id'];
		$old_series_nid = $node->field_series_reference[$series_field_langcode][0]['target_id'];
		
		//category or topic field
		if (
			(is_numeric($category_nid) && intval($category_nid)>0)
			&& ($old_category_nid!=$category_nid)
		) {
			
			//edit category
			$node->field_topic_reference[$category_field_langcode] = array( array('target_id' => $category_nid) );
			$json_category = array (
				'nid' => $category_nid,
				'title' => $category_node->title,
			);
			
			//edit series
			$node->field_series_reference[$series_field_langcode] = array();
			$json_series = 0;
			
			$edited = true;
		}
		
		//series field
		if (
			(is_numeric($series_nid) && intval($series_nid)>0)
			&& ($old_series_nid!=$series_nid)
		) {
			$node->field_series_reference[$series_field_langcode] = array( array('target_id' => $series_nid) );
			$json_series = array (
				'nid' => $series_nid,
				'title' => $series_node->title,
			);
			$edited = true;
		}
		
		//save node
		if ($edited) {
			node_save($node);
			$json_nids_changed[] = $nid;
		}
	}
	
	//return JSON with NIDS and changed values
	$json_data = array (
		'nids' => $nids,
		'data_values' => array (
			'series' => $json_series,
			'category' => $json_category,
			'nids_changed' => $json_nids_changed,
		),
	);
	
	//return json data
	drupal_json_output($json_data);
}
/** ENDS *****************************************/


/**
 * delete nodes
 */
function uportal_backend_bulk_editor_delete_nodes() {
	$nids = $_POST['nids'];
	$json_output = array(
		'nids' => $nids,
	);
	node_delete_multiple($nids);
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * get JSON data
 */
function _uportal_backend_bulk_editor_get_json_data() {
	$json_output = array(
		'type' => $_POST['request-data-type'],
	);
	$data = array();
	
	if ($_POST['request-data-type']=='languages') {
		$languages = locale_language_list('name');
		foreach ($languages as $language_code => $language_name) {
			$data[] = array (
				'id' => $language_code,
				'title' => $language_name,
			);
		}
	} elseif ($_POST['request-data-type']=='series-by-category') {
		_uportal_backend_bulk_editor_get_series_by_category();
		return;
	} else {
		$content_type = '';
		switch ($_POST['request-data-type']) {
			case 'content-providers':
				$content_type = 'content_provider';
				break;
			case 'categories':
				$content_type = 'topic';
				break;
			case 'series':
				$content_type = 'series';
				break;
			case 'lesson-plans':
				$content_type = 'lesson_plan';
				break;
		}
		$query = "SELECT node.title AS node_title, node.nid AS nid FROM {node} node WHERE node.type IN  ('".$content_type."')";
		$query_results = db_query($query);
		foreach ($query_results as $result) {
			$data[] = array (
				'id' => $result->nid,
				'title' => $result->node_title,
			);
		}
	}

	$json_output['data'] = $data;
	drupal_json_output($json_output);
}
/** ENDS *****************************************/

/**
 * get existing series and lesson plans
 */
function _uportal_backend_bulk_editor_get_existing_series_lesson_plans() {
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('series','lesson_plan'), 'IN');
	$result = $query->execute();
	
	$series = array();
	$lesson_plans = array();
	if (isset($result['node'])) {
		$nids = array_keys($result['node']);
	  $nodes = entity_load('node', $nids);
		foreach($nodes as $node) {
			if ($node->type=='series') {
				$series['nid-'.$node->nid] = $node->title;
			}
			if ($node->type=='lesson_plan') {
				$lesson_plans['nid-'.$node->nid] = $node->title;
			}
		}
	}
	
	drupal_json_output(
		array (
			'series' => $series,
			'lesson_plans' => $lesson_plans,
		)
	);
}
/** ENDS *****************************************/


/**
 * save series order
 */
function _uportal_backend_bulk_editor_save_series_order() {
	$series_nid = $_POST['seriesNID'];
	$initiator = $_POST['initiator'];
	$nids = $_POST['nidsOrder'];
	$series_parent_queue = nodequeue_load_queue_by_name('series_child_queue');
	$series_subqueue_name = 'series_child_queue_'.$series_nid;
	$subqueues = nodequeue_load_subqueues_by_reference( array (
		$series_parent_queue->qid => array($series_subqueue_name)
	));
	
	if (count($subqueues) && count($nids)) {
		$series_subqueue = current($subqueues);
		$sqid = $series_subqueue->sqid;
		nodequeue_queue_clear($sqid);
		foreach ($nids as $content_nid) {
			nodequeue_subqueue_add($series_parent_queue, $series_subqueue, $content_nid);
		}
	}
	
	drupal_json_output(
		array (
			'nid' => $series_nid,
			'initiator' => $initiator,
			'nids' => $nids,
		)
	);
	
}
/** ENDS *****************************************/


/**
 * save series order
 */
function _uportal_backend_bulk_editor_save_lesson_plan_order() {
	$lesson_plan_nid = $_POST['lessonPlanNID'];
	$initiator = $_POST['initiator'];
	$nids = $_POST['nidsOrder'];
	$lesson_plan_parent_queue = nodequeue_load_queue_by_name('lesson_plan_child_queue');
	$lesson_plan_subqueue_name = 'lesson_plan_child_queue_'.$lesson_plan_nid;
	$subqueues = nodequeue_load_subqueues_by_reference( array (
		$lesson_plan_parent_queue->qid => array($lesson_plan_subqueue_name)
	));
	
	if (count($subqueues) && count($nids)) {
		$lesson_plan_subqueue = current($subqueues);
		$sqid = $lesson_plan_subqueue->sqid;
		nodequeue_queue_clear($sqid);
		foreach ($nids as $content_nid) {
			nodequeue_subqueue_add($lesson_plan_parent_queue, $lesson_plan_subqueue, $content_nid);
		}
	}
	
	drupal_json_output(
		array (
			'nid' => $lesson_plan_nid,
			'initiator' => $initiator,
			'nids' => $nids,
		)
	);
	
}
/** ENDS *****************************************/
