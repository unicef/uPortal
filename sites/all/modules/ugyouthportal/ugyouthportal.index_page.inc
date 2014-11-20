<?php

/**
 * we use the following globals
 *
 * 	these pass on the true filter values:
 * 		- $_GET['cid'] = category id
 * 		- $_GET['skid'] = skill level ids
 * 		- $_GET['type'] = node type
 * 		- $_GET['lang'] = language
 *
 * 	these are used to store data in the session variable
 * 		- $_SESSION['uportal_index_search_term'] = holds the current search terms
 * 		- $_SESSION['uportal_index_full_results'] = holds ALL the results
 * 		- $_SESSION['uportal_index_full_filters'] = holds ALL the filter values
 * 		- $_SESSION['uportal_index_page_results'] = holds current page results
 *
 */



/**
 * get content index page
 */
function ugyouthportal_get_index_page($page_type) {
	
	//to fix ajax 404 error because it didn't work on the unicef server
	//header('HTTP/1.1 200 OK', true, 200);
	
	//dsm($_GET);
	
	global $ugyouthportal_data;
	$content = '';
	$page_args = arg();
	
	//add class
	$ugyouthportal_data['body_classes'][] = 'listing-page';
	$ugyouthportal_data['body_classes'][] = 'index-page';
	$ugyouthportal_data['body_classes'][] = $page_type;
	
	//empty string
	$link_options = array(
		'title' => 'Reset Search',
		'html-link' => false,
		'killsearch' => TRUE,
		'class' => array('btn'),
	);
	$all_link = _ugyouthportal_get_index_link(array(), $link_options, 'all');
	$empty_string = '<div class="no-index-results no-index-results-page"><p>Sorry, no content matches your search terms.</p><p class="link-btn">'.$all_link.'</p>';
	
	//set current filters
	$current_index_filter_values = _ugyouthportal_get_current_index_filter_values();
	if (!($current_index_filter_values && count($current_index_filter_values))) {
		drupal_set_title('No Results | Content Index');
		return $empty_string;
	}
	$ugyouthportal_data['current-index-filter-values'] = $current_index_filter_values;
	
	//add separate classes for landing page and non-landing page
	if ($page_args[1]=='all-content' && $page_args[2]=='all-categories') {
		$ugyouthportal_data['body_classes'][] = 'index-landing-page';
	} else {
		$ugyouthportal_data['body_classes'][] = 'index-not-landing-page';
	}
	
	//add class for active filter
	if (!$current_index_filter_values['all_page'] || isset($current_index_filter_values['search-term'])) {
		$ugyouthportal_data['body_classes'][] = 'index-active-filters';
	}
	
	//check for killsearch and send to ALL page
	if (isset($current_index_filter_values['killsearch'])) {
		$all_url = _ugyouthportal_get_url($current_index_filter_values, array(), 'all');
		drupal_goto($all_url['url'], $all_url['options']);
	}
	
	//set the initial global results holders if they are not set or if it is a landing ALL page
	/** reset remove TRUE ****/
	if (($current_index_filter_values['all_page'] && !isset($_GET['page'])) || !isset($_SESSION['uportal_index_full_results'])) {
		_ugyouthportal_set_full_index_results($current_index_filter_values);
	}
	
	//set content for this particular filter set if not already set
	/** reset remove TRUE ****/
	if (!isset($_GET['page']) || !isset($_SESSION['uportal_index_page_results'])) {
		_ugyouthportal_set_index_page_results($current_index_filter_values);
	}
	
	//get result nids to send to page
	$page_result_nids = $_SESSION['uportal_index_page_results'];
	$results_by_page = array_chunk($page_result_nids, $ugyouthportal_data['no_per_listing_page']);
	$page_no = isset($_GET['page']) ? $_GET['page'] : 0;
	$ugyouthportal_data['index_current_page'] = $page_no; //for use in views template
	$ugyouthportal_data['index_page_no'] = count($results_by_page); //for use in views template
	
	//check for empty page
	if (!isset($results_by_page[$page_no]) || count($page_result_nids)<=0) {
		drupal_set_title('No Results | Content Index');
		return $empty_string;
	}
	
	//get content from views
	$views_args = implode('+', $results_by_page[$page_no]);
	$views_display = 'block_2';
	if ($current_index_filter_values['page-type']=='all-content' && ($current_index_filter_values['all_page'] || $current_index_filter_values['category-nid']=='all')) {
		//dsm($page_result_nids);
		//dsm(implode('+', $page_result_nids));
		$views_args = implode('+', $page_result_nids);
		$views_display = 'block_1';
	} elseif ($current_index_filter_values['page-type']=='all-series' && ($current_index_filter_values['all_page'] || $current_index_filter_values['category-nid']=='all')) {
		$views_args = implode('+', $page_result_nids);
		$views_display = 'block_6';
	} elseif ($current_index_filter_values['page-type']=='all-lesson-plans' || $current_index_filter_values['page-type']=='all-series') {
		$views_display = 'block_3';
	}
	$content = views_embed_view('content_index', $views_display, $views_args);
	
	//set title
	$page_title = _ugyouthportal_get_index_page_title($current_index_filter_values);
	drupal_set_title($page_title);
	
	return $content;

}
/** END ************************/


/**
 * get result nids for this filter set
 */
function _ugyouthportal_set_index_page_results($current_index_filters) {
	
	$full_index_results = $_SESSION['uportal_index_full_results'];
	$full_index_filters = $_SESSION['uportal_index_full_filters'];
	
	$result_nids = array();
	$page_type = $current_index_filters['page-type'];
	$all_page = $current_index_filters['all_page'];
	$full_results = $full_index_results[$page_type];
	
	//reset filter counts
	foreach ($full_index_filters as $key => &$filter_type) {
		foreach ($filter_type as &$filter_item) {
			$filter_item['current-counts'][$page_type] = 0;
		}
	}
	
	//get page results
	if ($all_page) {
		foreach ($full_results as $result) {
			$result_nids[] = $result['nid'];
		}
	} else {
		foreach ($full_results as $result) {
			$satisfies_filters = false;
			
			//category check
			$satisfies_filters = isset($result['topic-nids']['nid-'.$current_index_filters['category-nid']]);
			if (!$satisfies_filters) continue;
			
			//activate the media type filters here - they are independent of type and language
			foreach ($result['types'] as $type_key) {
				$full_index_filters['types'][$type_key]['current-counts'][$page_type]++;
			}
			
			//type check
			$satisfies_filters = $satisfies_filters && isset($result['types'][$current_index_filters['type']]);
			if (!$satisfies_filters) continue;
			
			//activate language filters here - they are independent of language
			foreach ($result['languages'] as $language_code) {
				if (isset($full_index_filters['languages'][$language_code])) {
					$full_index_filters['languages'][$language_code]['current-counts'][$page_type]++;
				}
			}
			
			//language check
			$satisfies_filters = $satisfies_filters && isset($result['languages'][$current_index_filters['language']]);
			if (!$satisfies_filters) continue;
			
			//activate these tag filters for this result
			foreach ($result['tags'] as $tags_key => $term_tid) {
				if ($tags_key!='tid-all') {
					$full_index_filters['tags'][$tags_key]['current-counts'][$page_type]++;
				}
			}
			
			//check tag tids
			$satisfies_tags = false;
			foreach ($current_index_filters['tags'] as $term_tid) {
				$satisfies_tags = isset($result['tags']['tid-'.$term_tid]);
				if ($satisfies_tags) break;
			}
			$satisfies_filters = $satisfies_filters && $satisfies_tags;
			if (!$satisfies_filters) continue;
			
			//update current count for topic
			//must satisfy everything because it is used on landing page view
			foreach ($result['topic-nids'] as $topic_nid_key => $topic_nid) {
				$full_index_filters['topics'][$topic_nid_key]['current-counts'][$page_type]++;
			}
			
			if ($satisfies_filters) {
				$result_nids[] = $result['nid'];
			}
		}
	}
	
	$_SESSION['uportal_index_page_results'] = $result_nids;
	$_SESSION['uportal_index_full_filters'] = $full_index_filters;
}
/** END ************************/


/**
 * build index query to set global results
 */
function _ugyouthportal_build_index_query($current_index_filters) {
	$search_active = isset($current_index_filters['search-term']);
	$query_args = array();
	$query = "
		SELECT
			node.nid AS nid,
			node.title AS title,
			node.type AS node_type,
			node.language AS node_language,
			node_field_data_field_topic_reference.nid AS topic_nid,
			node_field_data_field_series_reference.nid AS series_nid,
			taxonomy_index.tid AS term_tid,
			taxonomy_term_data.name AS term_name,
			node_field_data_field_lesson_plan_reference.nid AS lesson_plan_nid
		FROM 
			{node} node
			INNER JOIN {node_type} node_node_type ON node.type = node_node_type.type
			LEFT JOIN {languages} languages ON node.language = languages.language
			LEFT JOIN {field_data_field_topic_reference} field_data_field_topic_reference ON node.nid = field_data_field_topic_reference.entity_id AND (field_data_field_topic_reference.entity_type = 'node' AND field_data_field_topic_reference.deleted = '0')
			LEFT JOIN {node} node_field_data_field_topic_reference ON field_data_field_topic_reference.field_topic_reference_target_id = node_field_data_field_topic_reference.nid
			LEFT JOIN {field_data_field_series_reference} field_data_field_series_reference ON node.nid = field_data_field_series_reference.entity_id AND (field_data_field_series_reference.entity_type = 'node' AND field_data_field_series_reference.deleted = '0')
			LEFT JOIN {node} node_field_data_field_series_reference ON field_data_field_series_reference.field_series_reference_target_id = node_field_data_field_series_reference.nid
			LEFT JOIN {taxonomy_index} taxonomy_index ON node.nid = taxonomy_index.nid
			LEFT JOIN {taxonomy_term_data} taxonomy_term_data ON  taxonomy_index.tid = taxonomy_term_data.tid
			LEFT JOIN {field_data_field_lesson_plan_reference} field_data_field_lesson_plan_reference ON node.nid = field_data_field_lesson_plan_reference.entity_id AND (field_data_field_lesson_plan_reference.entity_type = 'node' AND field_data_field_lesson_plan_reference.deleted = '0')
			LEFT JOIN {node} node_field_data_field_lesson_plan_reference ON field_data_field_lesson_plan_reference.field_lesson_plan_reference_target_id = node_field_data_field_lesson_plan_reference.nid
			LEFT JOIN {field_data_field_content_provider_reference} field_data_field_content_provider_reference ON node.nid = field_data_field_content_provider_reference.entity_id AND (field_data_field_content_provider_reference.entity_type = 'node' AND field_data_field_content_provider_reference.deleted = '0')
			LEFT JOIN {node} node_field_data_field_content_provider_reference ON field_data_field_content_provider_reference.field_content_provider_reference_target_id = node_field_data_field_content_provider_reference.nid
	";

	if ($search_active) {
		$query .= ' LEFT JOIN {field_data_body} field_data_body ON node.nid = field_data_body.entity_id'; //content body
		$query .= ' LEFT JOIN {field_data_body} field_data_body_lesson_plan ON node_field_data_field_lesson_plan_reference.nid = field_data_body.entity_id'; //lesson plan body
		$query .= ' LEFT JOIN {field_data_body} field_data_body_series ON node_field_data_field_series_reference.nid = field_data_body.entity_id'; //series body
		//$query .= ' LEFT JOIN {field_data_body} field_data_body_topic ON node_field_data_field_topic_reference.nid = field_data_body_topic.entity_id'; //topic body
	}
	
	$query .= " WHERE ((( (node.status = '1') AND (node.type IN  ('audio', 'document', 'video')) )))";
	
	//check category nid
	if ($current_index_filters['category-nid']!='all') {
		$query .= " AND (node_field_data_field_topic_reference.nid = :category_nid )";
		$query_args[':category_nid'] = $current_index_filters['category-nid'];
	}
	
	/*
	//check skill level nids
	$skill_level_nids = $current_index_filters['skill-level-nids'];
	if ($skill_level_nids[0]!='all') {
		$query .= " AND (node_field_data_field_skill_level_reference.nid IN(:skill_level_nids) )";
		$query_args[':skill_level_nids'] = $skill_level_nids;
	}
	*/
	
	//check tag tids
	$tags = $current_index_filters['tags'];
	if ($tags[0]!='all') {
		$query .= " AND (taxonomy_index.tid IN(:tids) )";
		$query_args[':tids'] = $tags;
	}
	
	//check media type
	$filter_media_type = $current_index_filters['type'];
	if ($filter_media_type!='all') {
		$query .= "  AND (node.type = :filter_media_type)";
		$query_args[':filter_media_type'] = $filter_media_type;
	}
	
	//check language
	$filter_language = $current_index_filters['language'];
	if ($filter_language!='all') {
		$query .= " AND (node.language = :filter_language)";
		$query_args[':filter_language'] = $filter_language;
	}
	
	//check search terms
	if ($search_active) {
		/**
		 * removed ....
			OR field_data_body_topic.body_value LIKE :search_term
		*/
		$query .= " AND(
									(node.title LIKE :search_term)
									OR field_data_body.body_value LIKE :search_term
									OR node_node_type.name LIKE :search_term
									OR languages.name LIKE :search_term
									OR node_field_data_field_topic_reference.title LIKE :search_term
									OR taxonomy_term_data.name LIKE :search_term
									OR node_field_data_field_lesson_plan_reference.title LIKE :search_term
									OR node_field_data_field_series_reference.title LIKE :search_term
									OR node_field_data_field_content_provider_reference.title LIKE :search_term
									OR field_data_body_lesson_plan.body_value LIKE :search_term
									OR field_data_body_series.body_value LIKE :search_term )";
		$query_args[':search_term'] = '%'.db_like($current_index_filters['search-term']).'%';
	}
	
	//order by
	$query .= " ORDER BY title ASC";
	
	return array( 'str'=>$query, 'args'=>$query_args );
}
/** END ************************/


/**
 * build full results for index search
 *  - all searches are based on these results
 */
function _ugyouthportal_set_full_index_results($current_index_filters) {
	
	//module_load_include('inc', 'pathauto');
	
	//set full filter values
	$full_index_filters = _ugyouthportal_set_full_index_filters();
	
	//set full index results
	$content_results = array();
	$lesson_plan_results = array();
	$series_results = array();
	$query = _ugyouthportal_build_index_query($current_index_filters);
	$query_results = db_query($query['str'], $query['args']);
	foreach ($query_results as $result) {
		_ugyouthportal_help_make_result_array($content_results, $result, $result->nid, 'all-content', $full_index_filters);
		if ($result->lesson_plan_nid) {
			_ugyouthportal_help_make_result_array($lesson_plan_results, $result, $result->lesson_plan_nid, 'all-lesson-plans', $full_index_filters);
		}
		if ($result->series_nid) {
			_ugyouthportal_help_make_result_array($series_results, $result, $result->series_nid, 'all-series', $full_index_filters);
		}
	}
	
	$_SESSION['uportal_index_full_results'] = array(
		'all-content' => $content_results,
		'all-lesson-plans' => $lesson_plan_results,
		'all-series' => $series_results,
	);
	$_SESSION['uportal_index_full_filters'] = $full_index_filters;
	
}

function _ugyouthportal_help_make_result_array(&$arr, $result, $primary_nid, $arr_type, &$filters) {
	$type = $result->node_type;
	$language = $result->node_language;
	$topic_nid = $result->topic_nid;
	$lesson_plan_nid = $result->lesson_plan_nid;
	$series_nid = $result->series_nid;
	$term_tid = $result->term_tid;
	$term_name = $result->term_name;
	$terms_checker = array();
	
	//make array and set filter counter
	if (!isset($arr['nid-'.$primary_nid])) {
		$arr['nid-'.$primary_nid] = array (
			'nid' => $primary_nid,
			'types' => array( 'all'=>'all' ),
			'languages' => array( 'all'=>'all' ),
			'topic-nids' => array( 'nid-all'=>'all' ),
			'tags' => array( 'tid-all'=>'all' ),
		);
		$filters['topics']['nid-all']['total-counts'][$arr_type]++;
		$filters['topics']['nid-'.$topic_nid]['total-counts'][$arr_type]++;
		$filters['types']['all']['total-counts'][$arr_type]++;
		$filters['types'][$type]['total-counts'][$arr_type]++;
		if (isset($filters['languages'][$language])) {
			$filters['languages']['all']['total-counts'][$arr_type]++;
			$filters['languages'][$language]['total-counts'][$arr_type]++;
		}
	}
	
	//add term tids after checking
	if ($term_tid) {
		if (!isset($terms_checker['tid-'.$term_tid])) {
			$terms_checker['tid-'.$term_tid] = array (
				'all-content' => array(),
				'all-lesson-plans' => array(),
				'all-series' => array(),
			);
		}
		if (!isset($terms_checker['tid-'.$term_tid][$arr_type]['nid-'.$primary_nid])) {
			$terms_checker['tid-'.$term_tid][$arr_type]['nid-'.$primary_nid] = 1;
			$filters['tags']['tid-all']['total-counts'][$arr_type]++;
			$filters['tags']['tid-'.$term_tid]['total-counts'][$arr_type]++;
		}
	}
	
	//enter values for type
	if (!isset($arr['nid-'.$primary_nid]['types'][$type])) {
		$arr['nid-'.$primary_nid]['types'][$type] = $type;
	}
	
	//enter values for language
	if (!isset($arr['nid-'.$primary_nid]['languages'][$language])) {
		$arr['nid-'.$primary_nid]['languages'][$language] = $language;
	}
	
	//enter values for topic nids
	if (!isset($arr['nid-'.$primary_nid]['topic-nids']['nid-'.$topic_nid])) {
		$arr['nid-'.$primary_nid]['topic-nids']['nid-'.$topic_nid] = $topic_nid;
	}
	
	//enter values for skill level nids
	if ($term_tid && !isset($arr['nid-'.$primary_nid]['tags']['tid-'.$term_tid])) {
		$arr['nid-'.$primary_nid]['tags']['tid-'.$term_tid] = $term_tid;
	}
	
}
/** END ************************/


/**
 * set full filter values for index page
 */
function _ugyouthportal_set_full_index_filters() {
	
	module_load_include('inc', 'pathauto');
	
	$full_filters = array();
	
	//set filter values for topics
	$topics = array(
		'nid-all' => array (
			'arg' => 'all',
			'title' => 'All Categories',
			'title-aliased' => 'all-categories',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
	);
	$topics_query = db_query("SELECT node.title AS title, node.nid AS nid FROM {node} node WHERE (( (node.status = '1') AND (node.type IN  ('topic')) )) ORDER BY title ASC");
	foreach ($topics_query as $topic) {
		$topics['nid-'.$topic->nid] = array (
			'arg' => $topic->nid,
			'title' => $topic->title,
			'title-aliased' => pathauto_cleanstring($topic->title),
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		);
	}
	$full_filters['topics'] = $topics;
	
	//set filter values for skill levels
	/*
	$skill_levels = array(
		'nid-all' => array (
			'arg' => 'all',
			//'title' => 'All Skill Levels',
			'title' => 'All',
			'title-aliased' => 'all-skill-levels',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
	);
	$skill_levels_query = db_query("
		SELECT
			node.title AS node_title, node.nid AS nid, node.language AS node_language, nodequeue_nodes_node.position AS nodequeue_nodes_node_position
		FROM 
			{node} node
			LEFT JOIN {nodequeue_nodes} nodequeue_nodes_node ON node.nid = nodequeue_nodes_node.nid AND nodequeue_nodes_node.qid = '9'
			LEFT JOIN {field_data_field_skill_level_reference} field_data_field_skill_level_reference ON node.nid = field_data_field_skill_level_reference.field_skill_level_reference_target_id
			LEFT JOIN {node} field_skill_level_reference_node ON field_data_field_skill_level_reference.entity_id = field_skill_level_reference_node.nid
		WHERE ((( (node.status = '1') AND (node.type IN  ('skill_level')) )))
		ORDER BY nodequeue_nodes_node_position ASC
	");
	foreach ($skill_levels_query as $skill_level) {
		$skill_levels['nid-'.$skill_level->nid] = array (
			'arg' => $skill_level->nid,
			'title' => $skill_level->node_title,
			'title-aliased' => pathauto_cleanstring($skill_level->node_title),
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		);
	}
	$full_filters['skill-levels'] = $skill_levels;
	*/
	
	//set filter value for types
	$full_filters['types'] = array (
		'all' => array(
			'arg' => 'all',
			'title' => 'All Types',
			'title-aliased' => 'all-types',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
		'video' => array(
			'arg' => 'video',
			'title' => 'Video',
			'title-aliased' => 'video',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
		'audio' => array(
			'arg' => 'audio',
			'title' => 'Audio',
			'title-aliased' => 'audio',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
		'document' => array(
			'arg' => 'document',
			'title' => 'Text',
			'title-aliased' => 'text',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
	);
	
	//set filter value for languages
	$full_filters['languages']['all'] = array (
		'arg' => 'all',
		'title' => 'All Languages',
		'title-aliased' => 'all-languages',
		'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
	);
	$languages = locale_language_list('name');
	foreach ($languages as $code => $language) {
		$full_filters['languages'][$code] = array (
			'arg' => $code,
			'title' => $language,
			'title-aliased' => pathauto_cleanstring($language),
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		);
	}
	
	//set filter value for tags
	$tags = array();
	$tags = array(
		'tid-all' => array (
			'arg' => 'all',
			'title' => 'All Tags',
			'title-aliased' => 'all-tags',
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		),
	);
	$tags_query = db_query("SELECT terms.tid AS tid, term_data.name AS term_name, vocab.name AS vocab_name FROM taxonomy_index AS terms INNER JOIN taxonomy_term_data AS term_data ON terms.tid = term_data.tid INNER JOIN taxonomy_vocabulary AS vocab ON term_data.vid = vocab.vid INNER JOIN node ON node.nid = terms.nid WHERE node.status='1' AND node.type IN ('video','audio','document') ORDER BY term_name ASC");
	foreach ($tags_query as $tag) {
		$tags['tid-'.$tag->tid] = array (
			'arg' => $tag->tid,
			'title' => $tag->term_name.' ('.$tag->vocab_name.')',
			'title-aliased' => pathauto_cleanstring($tag->term_name),
			'total-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
			'current-counts' => array( 'all-content'=>0, 'all-lesson-plans'=>0, 'all-series'=>0, ),
		);
	}
	$full_filters['tags'] = $tags;
	
	return $full_filters;
	
}
/** END ************************/


/**
 * get index page title
 */
function _ugyouthportal_get_index_page_title($current_index_filters) {
	$filter_values = $_SESSION['uportal_index_full_filters'];
	
	//category title
	$category_title = $filter_values['topics']['nid-'.$current_index_filters['category-nid']]['title'];
	
	//skill level title
	/*
	$skill_level_title = '';
	$skill_level_nids = $current_index_filters['skill-level-nids'];
	$no_skill_levels = count($skill_level_nids);
	for ($i=0; $i<$no_skill_levels; $i++) {
		$skill_level_title .= $filter_values['skill-levels']['nid-'.$skill_level_nids[$i]]['title'];
		if ($i<$no_skill_levels-1) $skill_level_title .= ', ';
	}
	*/
	
	//tags title
	$tags_title = '';
	$tag_tids = $current_index_filters['tags'];
	$no_tags = count($tag_tids);
	for ($i=0; $i<$no_tags; $i++) {
		$tags_title .= $filter_values['tags']['tid-'.$tag_tids[$i]]['title'];
		if ($i<$no_tags-1) $tags_title .= ', ';
	}
	
	//media type title
	$media_type_title = $filter_values['types'][$current_index_filters['type']]['title'];
	
	//language title
	$language_title = $filter_values['languages'][$current_index_filters['language']]['title'];
	
	//section titles
	$section_titles = array (
		'all-content' => 'All Content',
		'all-lesson-plans' => 'All Lesson Plans',
		'all-series' => 'All Series',
	);
	$section_title = $section_titles[$current_index_filters['page-type']];
	
	return $category_title.' | '.$tags_title.' | '.$media_type_title.' | '.$language_title.' | '.$section_title.' | Content Index';
}
/** END ************************/



/**
 * return current index filter values
 */
function _ugyouthportal_get_current_index_filter_values() {
	if (isset($_GET['cid']) && isset($_GET['type']) && isset($_GET['lang']) && isset($_GET['tags'])) {
		$filter_values = array( 'all_page'=>FALSE );
		$page_args = arg();
		$check_for_all_page = 0; //ALL if it is 4
		
		//check category
		if ($_GET['cid']=='all' || is_numeric($_GET['cid'])) {
			$filter_values['category-nid'] = $_GET['cid'];
			$check_for_all_page = ($_GET['cid']=='all') ? ($check_for_all_page+1) : $check_for_all_page;
		} else {
			return false;
		}
		
		//check skill level ids
		/*
		$skill_level_nids = explode(',', $_GET['skid']);
		if (count($skill_level_nids) && ($skill_level_nids[0]=='all' || is_numeric($skill_level_nids[0]))) {
			$filter_values['skill-level-nids'] = $skill_level_nids;
			$check_for_all_page = ($skill_level_nids[0]=='all') ? ($check_for_all_page+1) : $check_for_all_page;
		} else {
			return false;
		}
		*/
		
		//check tags
		$tag_tids = explode(',', $_GET['tags']);
		if (count($tag_tids) && ($tag_tids[0]=='all' || is_numeric($tag_tids[0]))) {
			$filter_values['tags'] = $tag_tids;
			$check_for_all_page = ($tag_tids[0]=='all') ? ($check_for_all_page+1) : $check_for_all_page;
		} else {
			return false;
		}
		
		//check type
		if ($_GET['type']=='all' || in_array($_GET['type'], array('video', 'document', 'audio'))) {
			$filter_values['type'] = $_GET['type'];
			$check_for_all_page = ($_GET['type']=='all') ? ($check_for_all_page+1) : $check_for_all_page;
		} else {
			return false;
		}
		
		//check language
		$languages = locale_language_list('name');
		if ($_GET['lang']=='all' || isset($languages[$_GET['lang']])) {
			$filter_values['language'] = $_GET['lang'];
			$check_for_all_page = ($_GET['lang']=='all') ? ($check_for_all_page+1) : $check_for_all_page;
		} else {
			return false;
		}

		if (isset($_GET['killsearch'])) {
			if (isset($_SESSION['uportal_index_search_term'])) {
				unset($_SESSION['uportal_index_search_term']);
			}
			$filter_values['killsearch'] = true;
		}

		if (isset($_SESSION['uportal_index_search_term'])) {
			$filter_values['search-term'] = $_SESSION['uportal_index_search_term'];
		}
		
		$filter_values['all_page'] = ($check_for_all_page==4);
		$filter_values['page-type'] = $page_args[1];
		
		return $filter_values;
	
	} else {
		return false;
	}
}
/** END ************************/


/**
 * make URL
 */
function _ugyouthportal_get_url($current_index_filter_values, $item_options, $url_type) {
	$page_args = arg();
	$url = isset($page_args[1]) ? 'content-index/'.$page_args[1] : 'content-index/all-content';
	$url_options = array();
	$url_options['attributes']['title'] = isset($item_options['title']) ? $item_options['title'] : '';
	$url_options['attributes']['title'] = isset($item_options['html-title']) ? $item_options['html-title'] : $url_options['attributes']['title'];
	$url_options['html'] = isset($item_options['html-link']) ? $item_options['html-link'] : FALSE;
	$url_options['attributes']['class'] = (isset($item_options['class']) && is_array($item_options['class'])) ? $item_options['class'] : array();
	
	if ($url_type=='primary-nav') {
		$url = 'content-index/all-content';
	}
	
	switch ($url_type) {
		case 'primary-nav':
		case 'all':
			$url .= '/all-categories/all-tags/all-types/all-languages';
			$url_options['query'] = array( 'cid'=>'all', 'tags'=>'all', 'type'=>'all', 'lang'=>'all' );
			if (isset($item_options['killsearch'])) {
				$url_options['query']['killsearch'] = 1;
			}
			break;
		case 'all-content-main-nav':
			$url = 'content-index/all-content/all-categories/all-tags/all-types/all-languages';
			$url_options['query'] = array( 'cid'=>'all', 'tags'=>'all', 'type'=>'all', 'lang'=>'all', );
			$url_options['attributes']['class'][] = ($current_index_filter_values['page-type'] == 'all-content') ? 'active' : '';
			break;
		case 'all-lesson-plans-main-nav':
			$url = 'content-index/all-lesson-plans/all-categories/all-tags/all-types/all-languages';
			$url_options['query'] = array( 'cid'=>'all', 'tags'=>'all', 'type'=>'all', 'lang'=>'all', );
			$url_options['attributes']['class'][] = ($current_index_filter_values['page-type'] == 'all-lesson-plans') ? 'active' : '';
			break;
		case 'all-series-main-nav':
			$url = 'content-index/all-series/all-categories/all-tags/all-types/all-languages';
			$url_options['query'] = array( 'cid'=>'all', 'tags'=>'all', 'type'=>'all', 'lang'=>'all', );
			$url_options['attributes']['class'][] = ($current_index_filter_values['page-type'] == 'all-series') ? 'active' : '';
			break;
		case 'topics':
			$url .= '/'.$item_options['alias'].'/all-tags/all-types/all-languages';
			if ($current_index_filter_values['category-nid'] == $item_options['arg']) {
				$url_options['attributes']['class'][] = 'link';
				$url_options['attributes']['class'][] = 'active';
			} else {
				$url_options['attributes']['class'][] = 'link';
			}
			$url_options['query'] = array (
				'cid' => $item_options['arg'],
				'tags' => 'all',
				'type' => 'all',
				'lang' => 'all'
			);
			break;
		case 'landing-page-topic-in-content':
		case 'landing-page-topic-in-content-header':
			$url .= '/'.$item_options['alias'].'/'.$page_args[3].'/'.$page_args[4].'/'.$page_args[5];
			$url_options['query'] = array (
				'cid' => $item_options['arg'],
				'tags' => implode(',', $current_index_filter_values['tags']),
				'type' => $current_index_filter_values['type'],
				'lang' => $current_index_filter_values['language'],
			);
			break;
		case 'active-filter-category':
			$url .= '/'.$page_args[2].'/all-tags/all-types/all-languages';
			$url_options['attributes']['class'][] = 'category';
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => 'all',
				'lang' => 'all',
				'tags' => 'all',
			);
			break;
		case 'active-filter-type':
			$url .= '/'.$page_args[2].'/'.$page_args[3].'/'.$page_args[4].'/all-languages';
			$url_options['attributes']['class'][] = 'type';
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $current_index_filter_values['type'],
				'lang' => 'all',
				'tags' => 'all',
			);
			break;
		case 'active-filter-language':
			$url .= '/'.$page_args[2].'/'.$page_args[3].'/'.$page_args[4].'/'.$page_args[5];
			$url_options['attributes']['class'][] = 'language';
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $current_index_filter_values['type'],
				'lang' => $current_index_filter_values['language'],
				'tags' => 'all',
			);
			break;
		case 'active-filter-tags':
			$url .= '/'.$page_args[2].'/'.$page_args[3].'/'.$page_args[4].'/all-languages';
			$url_options['attributes']['class'][] = 'tags';
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $current_index_filter_values['type'],
				'lang' => $current_index_filter_values['language'],
				'tags' => implode(',', $current_index_filter_values['tags']),
			);
			break;
		case 'types':
			$url .= '/'.$page_args[2].'/'.$page_args[3].'/'.$item_options['alias'].'/all-languages';
			if ($current_index_filter_values['type'] == $item_options['arg']) {
				$url_options['attributes']['class'][] = 'link';
				$url_options['attributes']['class'][] = 'active';
			} else {
				$url_options['attributes']['class'][] = 'link';
			}
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $item_options['arg'],
				'lang' => 'all',
				'tags' => 'all',
			);
			break;
		case 'languages':
			$url .= '/'.$page_args[2].'/'.$page_args[3].'/'.$page_args[4].'/'.$item_options['arg'];
			if ($current_index_filter_values['language'] == $item_options['arg']) {
				$url_options['attributes']['class'][] = 'link';
				$url_options['attributes']['class'][] = 'active';
			} else {
				$url_options['attributes']['class'][] = 'link';
			}
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $current_index_filter_values['type'],
				'lang' => $item_options['arg'],
				'tags' => 'all',
			);
			break;
		case 'tags':
			$url .= '/'.$page_args[2].'/';
			
			//build tags args
			$url_part_2_args = array();
			$current_args = $current_index_filter_values['tags'];
			$in_current_args = in_array($item_options['arg'], $current_args);
			if ($item_options['arg']=='all') {
				$url_part_2_args = array('all');
			} else {
				if ($current_args[0]=='all') { //if showing all, arguments should be singular e.g. /p1/
					$url_part_2_args = array($item_options['arg']);
				} elseif (!$in_current_args) { //if not showing all and not among the current args, just add to current args i.e. /p1,p2/
					$url_part_2_args = $current_args;
					$url_part_2_args[] = $item_options['arg'];
				} elseif ($in_current_args) { //if not showing all and among current args, remove thyself from the link
					$url_part_2_args = array();
					if (count($current_args)==1) { //remove thyself and go back to all if singular arg
						$url_part_2_args = array('all');
					} else { //remove thyself from among many other nids
						$url_part_2_args = array();
						foreach ($current_args as $arg) {
							if ($arg != $item_options['arg']) {
								$url_part_2_args[] = $arg;
							}
						}
					}
				}
			}
			$url_part_2_args_aliases = '';
			$no_args = count($url_part_2_args);
			for ($i=0; $i<$no_args; $i++) {
				$url_part_2_args_aliases .= $_SESSION['uportal_index_full_filters']['tags']['tid-'.$url_part_2_args[$i]]['title-aliased'];
				if ($i<$no_args-1) $url_part_2_args_aliases .= ',';
			}
			
			//continue
			$url .= $url_part_2_args_aliases;
			$url .= '/'.$page_args[4].'/'.$page_args[5];
			if ($in_current_args) {
				$url_options['attributes']['class'][] = 'link';
				$url_options['attributes']['class'][] = 'active';
			} else {
				$url_options['attributes']['class'][] = 'link';
			}
			$url_options['query'] = array (
				'cid' => $current_index_filter_values['category-nid'],
				'type' => $current_index_filter_values['type'],
				'lang' => $current_index_filter_values['language'],
				'tags' => implode(',', $url_part_2_args),
			);
			break;
	}
	
	return array (
		'url' => $url,
		'options' =>$url_options,
	);
	
}
/** END ************************/


/**
 * make URL
 */
function _ugyouthportal_get_index_link($current_index_filter_values, $item_options, $url_type) {
	$url = _ugyouthportal_get_url($current_index_filter_values, $item_options, $url_type);
	switch ($url_type) {
		case 'topics':
		case 'types':
		case 'languages':
		case 'landing-page-topic-in-content':
		case 'landing-page-topic-in-content-header':
			$str = $item_options['title'];
			$str .= (isset($item_options['count'])) ? ' <span class="count">('.$item_options['count'].')</span>' : '';
			$str .= ($url_type!='skill-level' && $url_type!='landing-page-topic-in-content') ? '<span class="arrow"></span>' : '';
			if ((isset($item_options['count']) && $item_options['count']>0) || $url_type=='landing-page-topic-in-content-header') {
				return l($str, $url['url'], $url['options']);
			} else {
				/*
				if ($url_type=='topics') {
					return '<span class="link no-link">'.$str.'</span>';
				} else {
					return FALSE;
				}
				*/
				return FALSE;
			}
		case 'tags':
			$str = $item_options['title'];
			if ($item_options['count']>0) {
				return l($str, $url['url'], $url['options']);
			} else {
				return false;
			}
		case 'skill-levels':
			$str = $item_options['title'];
			if ($item_options['count']>0) {
				return l($str, $url['url'], $url['options']);
			} else {
				return false;
			}
		case 'all-content-main-nav':
		case 'all-lesson-plans-main-nav':
		case 'all-series-main-nav':
			$str = $item_options['title'].'<span class="arrow"></span>';
			return l($str, $url['url'], $url['options']);
		case 'active-filter-category':
			$str = '<span style="background-image: url(\''.$item_options['icon'].'\');">'.$item_options['title'].'</span>';
			return l($str, $url['url'], $url['options']);
		case 'active-filter-skill-level':
		case 'active-filter-type':
		case 'active-filter-language':
			$str = '<span>'.$item_options['title'].'</span>';
			return l($str, $url['url'], $url['options']);
		case 'all':
			$str = $item_options['title'];
			return l($str, $url['url'], $url['options']);
	}
}
/** END ************************/



