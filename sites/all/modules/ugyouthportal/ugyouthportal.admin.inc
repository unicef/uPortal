<?php

/**
 * get category page callback
 */
function ugyouthportal_get_category_page($category_node, $series_nid, $filter_media_type, $filter_language) {
	/**************************************************
	 *
	 * if no page is set, then we are at the start - page 0
	 * 	-- so we prepare the array of result nids according to the given filters
	 * 			-- first, pick out the special NIDs
	 * 			-- then get the rest, excluding the special NIDs
	 * 			-- concatenate the two arrays and store into the session variable
	 * if page is set
	 * 	-- pick out the page number section of NIDs from the session variable
	 *
	 * set selected_nids, current page number, number of results, number of pages
	 *
	 */
	global $ugyouthportal_data;
	
	//add class
	$ugyouthportal_data['body_classes'][] = 'listing-page';
	$ugyouthportal_data['body_classes'][] = 'isotope-page';
	
	if (!isset($_GET['page']) || !isset($_SESSION['uportal_category_content_nids_by_page'])) {
		_ugyouthportal_set_result_nids($category_node, $series_nid, $filter_media_type, $filter_language);
	}
	
	$page_title = _ugyouthportal_get_category_page_title($category_node, $series_nid, $filter_media_type, $filter_language);
	drupal_set_title($page_title);
	return '';
}


/**
 * get home page callback
 */
function ugyouthportal_get_home_page() {
	global $ugyouthportal_data;
	
	//add class
	$ugyouthportal_data['body_classes'][] = 'listing-page';
	$ugyouthportal_data['body_classes'][] = 'isotope-page';
	
	if (!isset($_GET['page']) || !isset($_SESSION['uportal_category_content_nids_by_page'])) {
		_ugyouthportal_set_result_nids(FALSE, FALSE, FALSE, FALSE, FALSE);
	}
	
	return '';
}






/**
 * run query and set session variable category_content_nids
 */
function _ugyouthportal_set_result_nids($category_node, $series_nid, $filter_media_type, $filter_language) {
	global $ugyouthportal_data;

	//get special nids
	$special_nids_results = _ugyouthportal_set_listing_pages_special_nids();
	$special_nids = $special_nids_results['listing_pages_special_nids'];
	$special_nids_by_class = $special_nids_results['listing_pages_special_nids_by_class'];
	
	//get all results excluding special nids
	/**
	 * get all result nids
	 * run them through the random sort
	 * return and store sorted result nids excluding special nids
	 */
	if ($category_node && isset($category_node->nid)) {
		$page_result_nids = _ugyouthportal_get_category_page_result_nids($category_node->nid, $series_nid, $filter_media_type, $filter_language, $special_nids);		
	} else {
		$page_result_nids = _ugyouthportal_get_home_page_result_nids($special_nids);
	}
	if (count($page_result_nids)) {
		$page_result_nids = ugyouthportal_sort_run_random_content_sorter($page_result_nids);
	}
	
	//join result nids together
	$full_result_nids = array_merge($special_nids, $page_result_nids);
	$results_by_page = array_chunk($full_result_nids, $ugyouthportal_data['no_per_listing_page']);
	
	//set results into session
	$_SESSION['uportal_category_content_nids_by_page'] = $results_by_page;
	$_SESSION['uportal_category_content_nids_by_class'] = $special_nids_by_class;
	
}


/**
 * get result nids for this page
 */
function _ugyouthportal_get_home_page_result_nids($special_nids) {
	$result_nids = array();
	
	$query = "SELECT node.nid AS nid
						FROM 
						{node} node
						WHERE ((( (node.status = '1') AND (node.type IN  ('audio', 'document', 'video')) )))";
	
	//exclude special nids
	if (count($special_nids)) {
		$query .= " AND (node.nid NOT IN (:special_nids))";
	}
	
	//get query results
	$query_args = array(
		':special_nids' => $special_nids,
	);
	$query_results = db_query($query, $query_args);
	foreach ($query_results as $result) {
		$result_nids[] = $result->nid;
	}
	
	return $result_nids;
}


/**
 * get result nids for this page
 */
function _ugyouthportal_get_category_page_result_nids($category_nid, $series_nid, $filter_media_type, $filter_language, $special_nids) {
	$result_nids = array();
	$query_args = array(
		':category_nid' => $category_nid,
	);
	
	$query = "SELECT node.nid AS nid
						FROM 
						{node} node
						LEFT JOIN {field_data_field_topic_reference} field_data_field_topic_reference ON node.nid = field_data_field_topic_reference.entity_id AND (field_data_field_topic_reference.entity_type = 'node' AND field_data_field_topic_reference.deleted = '0')
						LEFT JOIN {node} node_field_data_field_topic_reference ON field_data_field_topic_reference.field_topic_reference_target_id = node_field_data_field_topic_reference.nid
						LEFT JOIN {field_data_field_series_reference} field_data_field_series_reference ON node.nid = field_data_field_series_reference.entity_id AND (field_data_field_series_reference.entity_type = 'node' AND field_data_field_series_reference.deleted = '0')
						LEFT JOIN {node} node_field_data_field_series_reference ON field_data_field_series_reference.field_series_reference_target_id = node_field_data_field_series_reference.nid
						WHERE (( (node_field_data_field_topic_reference.nid = :category_nid ) )AND(( (node.status = '1') AND (node.type IN  ('audio', 'document', 'video')) )))";
	
	//check series
	if ($series_nid && $series_nid!='all' && is_numeric($series_nid)) {
		$query .= " AND (node_field_data_field_series_reference.nid = :series_nid )";
		$query_args[':series_nid'] = $series_nid;
	}
	
	//check media type
	if ($filter_media_type && $filter_media_type!='all' && in_array($filter_media_type, array('video','document','audio'))) {
		$query .= "  AND (node.type = :filter_media_type )";
		$query_args[':filter_media_type'] = $filter_media_type;
	}
	
	//check language
	$enabled_languages = locale_language_list('name');
	if ($filter_language && $filter_language!='all' && in_array($filter_language, array_keys($enabled_languages))) {
		$query .= " AND (node.language = :filter_language )";
		$query_args[':filter_language'] = $filter_language;
	}
	
	//exclude special nids
	if (count($special_nids)) {
		$query .= " AND (node.nid NOT IN (:special_nids))";
		$query_args[':special_nids'] = $special_nids;
	}
	
	//get query results
	$query_results = db_query($query, $query_args);
	foreach ($query_results as $result) {
		$result_nids[] = $result->nid;
	}
	
	return $result_nids;
}




/**
 * get category page title
 */
function _ugyouthportal_get_category_page_title($category_node, $series_nid, $filter_media_type, $filter_language) {
	//category title
	$category_title = $category_node->title;
	
	//series title
	$series_title = 'All Series';
	if (is_numeric($series_nid)) {
		$series_node = node_load($series_nid);
		$series_title = $series_node->title;
	}
	
	//media type title
	$media_type_title = 'All Types';
	if ($filter_media_type!='all') {
		$media_type_title = ucfirst($filter_media_type);
	}
	
	//language title
	$language_title = 'All Languages';
	if ($filter_language!='all') {
		$languages = locale_language_list('name');
		$language_title = $languages[$filter_language];
	}
	
	return $category_title.' | '.$series_title.' | '.$media_type_title.' | '.$language_title;
}


/**
 * redirect to content given lesson plan or series
 */
function ugyouthportal_redirect_to_content($type, $nid) {
	$content_nid = false;
	
	$series_query = "
		SELECT node.nid AS nid, nodequeue_nodes_node.position AS nodequeue_nodes_node_position, node.created AS node_created
		FROM 
			{node} node
			LEFT JOIN {field_data_field_series_reference} field_data_field_series_reference ON node.nid = field_data_field_series_reference.entity_id AND (field_data_field_series_reference.entity_type = 'node' AND field_data_field_series_reference.deleted = '0')
			LEFT JOIN {node} node_field_data_field_series_reference ON field_data_field_series_reference.field_series_reference_target_id = node_field_data_field_series_reference.nid
			LEFT JOIN {nodequeue_nodes} nodequeue_nodes_node ON node.nid = nodequeue_nodes_node.nid AND nodequeue_nodes_node.qid = '15'
		WHERE (( (node_field_data_field_series_reference.nid = :series_nid ) )AND(( (node.type IN  ('audio', 'document', 'video')) )))
		ORDER BY nodequeue_nodes_node_position ASC, node_created ASC
	";
	
	$lesson_plans_query = "
		SELECT node.nid AS nid, nodequeue_nodes_node.position AS nodequeue_nodes_node_position, node.created AS node_created
		FROM 
		{node} node
		LEFT JOIN {field_data_field_lesson_plan_reference} field_data_field_lesson_plan_reference ON node.nid = field_data_field_lesson_plan_reference.entity_id AND (field_data_field_lesson_plan_reference.entity_type = 'node' AND field_data_field_lesson_plan_reference.deleted = '0')
		LEFT JOIN {node} node_field_data_field_lesson_plan_reference ON field_data_field_lesson_plan_reference.field_lesson_plan_reference_target_id = node_field_data_field_lesson_plan_reference.nid
		LEFT JOIN {nodequeue_nodes} nodequeue_nodes_node ON node.nid = nodequeue_nodes_node.nid AND nodequeue_nodes_node.qid = '12'
		WHERE (( (node_field_data_field_lesson_plan_reference.nid = :lesson_plan_nid ) )AND(( (node.type IN  ('audio', 'document', 'video')) )))
		ORDER BY nodequeue_nodes_node_position ASC, node_created ASC
	";
	
	if ($type=='lesson-plan') {
		$content_nid_result = db_query($lesson_plans_query, array(':lesson_plan_nid' => $nid))->fetchObject();
		$content_nid = $content_nid_result->nid;
	} elseif ($type=='series') {
		$content_nid_result = db_query($series_query, array(':series_nid' => $nid))->fetchObject();
		$content_nid = $content_nid_result->nid;
	}
	
	if ($content_nid) {
		drupal_goto(
			'node/'.$content_nid,
			array (
				'query' => array( 'from'=>$type, 'from-nid'=>$nid, ),
			)
		);
		return '<a href="node/'.$content_nid.'">View content here</a>';
	} else {
		return 'No content found for this '.(($type=='lesson-plan') ? 'lesson plan.' : ' series.');
	}
}


