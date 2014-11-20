<?php

/**
 * metadata page callback - content providers
 */
function uportal_backend_metadata_content_providers() {
	return _uportal_backend_metadata_listing_page('content_provider');
}
/** ENDS *****************************************/

/**
 * metadata page callback - languages
 */
function uportal_backend_metadata_languages() {
	return _uportal_backend_metadata_listing_page('language');
}
/** ENDS *****************************************/


/**
 * metadata page callback - tags
 */
function uportal_backend_metadata_tags() {
	return _uportal_backend_metadata_listing_page('tags');
}
/** ENDS *****************************************/


/**
 * metadata page - get listing page
 */
function _uportal_backend_metadata_listing_page($parent_type) {
	
	drupal_add_library('system', 'drupal.ajax');
	
	//all content
	$results_html = _uportal_backend_metadata_listing_page_results($parent_type);
	
	//general page info
	$results_header_html = _uportal_backend_metadata_listing_page_header($parent_type);
	
	//create full page
	$page_vars = array (
		'results_header_html' => $results_header_html,
		'results_rows_html' => $results_html,
	);
	$page_template_path = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page.tpl.php';
	$page_html = _uportal_backend_load_view($page_template_path, $page_vars);
	
	return $page_html;
	
}
/** ENDS *****************************************/


/**
 * get all nids to create listing_page results
 */
function _uportal_backend_metadata_listing_page_results($parent_type) {
	
	$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row.tpl.php';
	$full_results_html_str = '';
	
	if ($parent_type=='tags') {
		$results = array();
		$vocabularies = taxonomy_get_vocabularies();
		foreach ($vocabularies as $vocabulary) {
			$results['nid-'.$vocabulary->vid] = array (
				'count-total' => 0,
				'count-drafts' => 0,
				'count-drafts-and-published' => 0,
				'parent-nid' => $vocabulary->vid,
				'parent-title' => $vocabulary->name,
				'parent-description' => $vocabulary->description,
				'parent-machine-name' => $vocabulary->machine_name,
				'parent-url-content' => '',
				'category-results' => array (),
			);
			$terms = taxonomy_get_tree($vocabulary->vid);
			$term_ids = array();
			foreach ($terms as $term) {
				$term_id = $term->tid;
				$term_ids[] = $term_id;
				$term_description = $term->description;
				$term_name = $term->name;
				$term_count_audio = _uportal_backend_taxonomy_term_count_nodes(array($term_id), 'audio');
				$term_count_document = _uportal_backend_taxonomy_term_count_nodes(array($term_id), 'document');
				$term_count_video = _uportal_backend_taxonomy_term_count_nodes(array($term_id), 'video');
				$term_count_drafts = _uportal_backend_taxonomy_term_count_draft_nodes(array('term'=>$term, 'vocab'=>$vocabulary, ));
				$term_count_total = $term_count_audio + $term_count_document + $term_count_video;
				$results['nid-'.$vocabulary->vid]['category-results']['nid-'.$term_id] = array (
					'category-nid' => $term_id,
					'category-title' => $term_name,
					'category-description' => $term_description,
					'type-results' => array (
						'count-audio' => $term_count_audio,
						'count-document' => $term_count_document,
						'count-total' => $term_count_total,
						'count-drafts' => $term_count_drafts,
						'count-drafts-and-published' => $term_count_total,
						'count-video' => $term_count_video,
					),
				);
			}
			if (count($term_ids)>0) {
				$results['nid-'.$vocabulary->vid]['count-drafts-and-published'] = $results['nid-'.$vocabulary->vid]['count-total'] = _uportal_backend_taxonomy_term_count_nodes($term_ids);
				$results['nid-'.$vocabulary->vid]['count-drafts'] = $results['nid-'.$vocabulary->vid]['count-drafts'] = _uportal_backend_taxonomy_term_count_draft_nodes(array('vocab'=>$vocabulary));
			}
		}
	} else {
		//get all content
		$all_nids = views_get_view_result('management_meta_data', 'block_general');
		//first arrange all_nids by parent_type in mega-array
		$results = _uportal_backend_metadata_listing_page_arrange_results($parent_type, $all_nids);
	}
	
	foreach ($results as $parent_result) {
		$row_content_html = _uportal_backend_create_html_row_categories($parent_result, $parent_type);
		$row_vars = array (
			'parent_nid' => $parent_result['parent-nid'],
			'parent_title_html' => $parent_result['parent-title'],
			'parent_url_content' => $parent_result['parent-url-content'],
			'parent_content_count' => $parent_result['count-total'],
			'parent_content_count_drafts' => $parent_result['count-drafts'],
			'parent_content_count_drafts_and_published' => $parent_result['count-drafts-and-published'],
			'parent_row_content' => $row_content_html,
		);
		$full_results_html_str .= _uportal_backend_load_view($template_path_row, $row_vars);
	}
	
	return $full_results_html_str;

}
/** ENDS *****************************************/


/**
 * get draft content count for terms
 */
function _uportal_backend_taxonomy_term_count_draft_nodes($args = array()) {
	$table_name = 'field_data_field_'.$args['vocab']->machine_name.'_reference';
	$tid_field = 'field_'.$args['vocab']->machine_name.'_reference_tid';
	$tid = 0;
	if (isset($args['term'])) {
		$tid = $args['term']->tid;
	}
	
  $query = db_select($table_name, 't');
	$query->fields('t', array('entity_id', $tid_field));
	$query->join('node', 'n', 't.entity_id = n.nid');
	$query->condition('n.type', array('audio', 'video', 'document'), 'IN');
	$query->condition('n.status', 0, '=');
  if ($tid) $query->condition('t.'.$tid_field, $tid, '=');
	
	//get results
	$nids = array();
	$node_count = 0;
	$results = $query->execute();
	foreach ($results as $result) {
		if (!isset($nids['nid-'.$result->entity_id])) {
			$nids['nid-'.$result->entity_id] = 1;
			$node_count++;
		}
	}
  
  return $node_count;
	
}
/** ENDS *****************************************/


/**
 * get content count for each term
 */
function _uportal_backend_taxonomy_term_count_nodes($tids = array(), $type = 0) {
  $query = db_select('taxonomy_index', 't');
  $query->condition('tid', $tids, 'IN');
	$query->fields('t', array('nid', 'tid'));
 // $query->addExpression('COUNT(*)', 'count_nodes');

	// Restrict query by Content Type
  if (!empty($type)) {
    $query->join('node', 'n', 't.nid = n.nid');
		$query->condition('type', $type, '=');
  }
	
	//get results
	$nids = array();
	$node_count = 0;
	$results = $query->execute();
	foreach ($results as $result) {
		if (!isset($nids['nid-'.$result->nid])) {
			$nids['nid-'.$result->nid] = 1;
			$node_count++;
		}
	}
  
  return $node_count;
}
/** ENDS *****************************************/


/**
 * make row category content
 */
function _uportal_backend_create_html_row_categories($parent_result, $parent_type) {
	
	module_load_include('content_listing.inc', 'uportal_backend');

	$template_path_row_category_content = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row-content.tpl.php';
	
	$category_results = $parent_result['category-results'];
	$row_content_vars = array(
		'total_content_count' => $parent_result['count-total'],
		'total_content_count_drafts' => isset($parent_result['count-drafts']) ? $parent_result['count-drafts'] : 0,
		'total_content_count_drafts_and_published' => isset($parent_result['count-drafts-and-published']) ? $parent_result['count-drafts-and-published'] : 0,
		'result_content_rows' => array(),
	);
	
	$i = 0;
	$count_results = count($category_results);
	foreach ($category_results as $category_result) {
		
		$category_id_attribute = '';
		$category_row_classes = '';
		$category_row_classes .= ($i==0) ? ' first' : '';
		$category_row_classes .= ($i==($count_results-1)) ? ' last' : '';
		
		$category_link_status = 1;
		if ($category_result['type-results']['count-total']==0 && $category_result['type-results']['count-drafts-and-published']>0) {
			$category_link_status = 0;
		}
		$category_link_status = 'all'; //just show both published and drafts
		
		if ($parent_type=='language') {
			$category_link_args = uportal_backend_content_listing_build_path( array (
				'category_nid' => $category_result['category-nid'],
				'status' => $category_link_status,
				'lang_code' => $parent_result['parent-nid'],
				'link_type' => 'language',
			));
		} elseif ($parent_type=='tags') {
			$category_link_args = uportal_backend_content_listing_build_path( array (
				'tags' => $category_result['category-nid'],
				'link_type' => 'tags',
			));
		} else { //content provider
			$category_link_args = uportal_backend_content_listing_build_path( array (
				'category_nid' => $category_result['category-nid'],
				'status' => $category_link_status,
				'provider_nid' => $parent_result['parent-nid'],
			));
		}
		
		if (trim($category_result['category-title'])=='') $category_result['category-title'] = 'None';
		$category_link = '<a href="'.$category_link_args['path'].'">'.$category_result['category-title'].'</a>';
		$count_total_link = '<a href="'.$category_link_args['path'].'">'.number_format($category_result['type-results']['count-total']).' / '.number_format($category_result['type-results']['count-drafts']).'</a>';
		
		if ($parent_type=='tags') {
			$category_link_html = '<div class="tag-link-wrapper clearfix">';
			$category_link_html .= '<div class="tag-link float">'.$category_link.'</div>';
			$category_link_html .= '<div class="main-result-edit tag-edit float" data-tid="'.$category_result['category-nid'].'" data-vid="'.$parent_result['parent-nid'].'"></div>';
			$category_link_html .= '<div class="main-result-delete tag-delete float" data-tid="'.$category_result['category-nid'].'" data-vid="'.$parent_result['parent-nid'].'"></div>';
			$category_link_html .= '</div>';
			$category_link = $category_link_html;
			$category_id_attribute = 'id="result-row-tid-'.$category_result['category-nid'].'"';
		}
		
		$category = array(
			'title-html' => $category_link,
			'classes' => $category_row_classes,
			'id-attribute' => $category_id_attribute,
			'count-content-html' => $count_total_link,
			'count-total' => $category_result['type-results']['count-total'],
			'count-drafts' => $category_result['type-results']['count-drafts'],
			'count-drafts-and-published' => $category_result['type-results']['count-drafts-and-published'],
			'video-count' => $category_result['type-results']['count-video'],
			'audio-count' => $category_result['type-results']['count-audio'],
			'document-count' => $category_result['type-results']['count-document'],
		);
		$row_content_vars['result_content_rows'][] = $category;
		$i++;
	}
	
	return _uportal_backend_load_view($template_path_row_category_content, $row_content_vars);

}
/** ENDS *****************************************/


/**
 * rearrange nids by page request type
 */
function _uportal_backend_metadata_listing_page_arrange_results($parent_type, $all_nids) {
	$results = array();
	$parent_nid_key = false;
	$parent_title_key = false;
	$parent_url_key = false;
	$processed_nids = array();
	
	//get parent keys
	switch ($parent_type) {
		case 'content_provider':
			$parent_nid_key = 'node_field_data_field_content_provider_reference_nid';
			$parent_title_key = 'node_field_data_field_content_provider_reference_title';
			$parent_url_key = 'field_field_url';
			break;
		case 'language':
			$parent_nid_key = 'node_language';
			break;
	}
	
	if ($parent_nid_key) {
		
		//get all parent results first
		$results = _uportal_backend_metadata_initialise_listing_results($parent_type);
		
		foreach ($all_nids as $content_node) {
			
			$parent_nid = $content_node->{$parent_nid_key};
			$content_nid = $content_node->nid;
			
			//avoid repetition of content nodes
			if (isset($processed_nids['nid-'.$content_nid])) {
				continue;
			} else {
				$processed_nids['nid-'.$content_nid] = 1;
			}
			
			//avoid content without parent attributes
			if (!isset($results['nid-'.$parent_nid])) {
				continue;
			}
			
			$parent_results = &$results['nid-'.$parent_nid];
			$category_nid = $content_node->node_field_data_field_topic_reference_nid;
			$category_title = $content_node->node_field_data_field_topic_reference_title;
			$content_node_type = $content_node->node_type;
			$content_node_status = intval($content_node->node_status);
			
			//check for existence in parent's category results
			if (!isset($parent_results['category-results']['nid-'.$category_nid])) {
				$parent_results['category-results']['nid-'.$category_nid] = array (
					'category-nid' => $category_nid,
					'category-title' => $category_title,
					'type-results' => array (
						'count-total' => 0,
						'count-drafts' => 0,
						'count-drafts-and-published' => 0,
						'count-video' => 0,
						'count-audio' => 0,
						'count-document' => 0,
					),
				);
			}
			
			//update values and counters
			$category_results = &$parent_results['category-results']['nid-'.$category_nid];
			if ($content_node_status) {
				$category_results['type-results']['count-'.$content_node_type]++;
				$category_results['type-results']['count-total']++;
				$parent_results['count-total']++;
			} else {
				$category_results['type-results']['count-drafts']++;
				$parent_results['count-drafts']++;
			}
			$category_results['type-results']['count-drafts-and-published']++;
			$parent_results['count-drafts-and-published']++;
			
		}
		
		//sort results by total number
		$total_counts = array();
		foreach ($results as $result) {
			$total_counts[] = $result['count-total'];
		}
		array_multisort($total_counts, SORT_DESC, $results);
		
	}
	
	return $results;
}
/** ENDS *****************************************/

/**
 * create full results array for metadata listing page by seeding with all parent content
 */
function _uportal_backend_metadata_initialise_listing_results($parent_type) {
	
	$results = array();
	
	if ($parent_type=='language') {
		$languages = locale_language_list('name');
		foreach ($languages as $language_code=>$language) {
			if (!isset($results['nid-'.$language_code])) {
				$results['nid-'.$language_code] = array (
					'parent-nid' => $language_code,
					'parent-title' => $language,
					'parent-url-content' => '',
					'count-total' => 0,
					'count-drafts' => 0,
					'count-drafts-and-published' => 0,
					'category-results' => array(),
				);
			}
		}
	}
	
	if ($parent_type=='content_provider') {
		
		$all_content_providers = views_get_view_result('management_meta_data', 'block_1');
		
		foreach ($all_content_providers as $content_provider) {
			if (!isset($results['nid-'.$content_provider->nid])) {
				$content_provider_nid = $content_provider->nid;
				$results['nid-'.$content_provider_nid] = array (
					'parent-nid' => $content_provider_nid,
					'parent-title' => $content_provider->node_title,
					'parent-url-content' => '',
					'count-total' => 0,
					'count-drafts' => 0,
					'count-drafts-and-published' => 0,
					'category-results' => array(),
				);
				
				//set content provider url
				if ($content_provider->field_field_url && count($content_provider->field_field_url)) {
					$results['nid-'.$content_provider_nid]['parent-url-content'] = $content_provider->field_field_url[0]['rendered']['#markup'];
				}
			}
		}
	}

	return $results;
}


/**
 * metadata page - get header label or title
 */
function _uportal_backend_metadata_listing_page_header($type) {
	$general_info = array (
		'header_label' => '',
		'category_label' => 'Category',
		'count_label' => 'Items (P/D)',
		'types_label' => '<div class="key key-video"><div class="colour"></div><div class="type-name">Videos</div></div><div class="key key-audio"><div class="colour"></div><div class="type-name">Audio</div></div><div class="key key-document"><div class="colour"></div><div class="type-name">Documents</div></div>',
	);
	
	switch ($type) {
		case 'content_provider':
			$general_info['header_label'] = 'Organisation';
			break;
		case 'language':
			$general_info['header_label'] = 'Language';
			break;
		case 'tags':
			$general_info['header_label'] = 'Tag Theme';
			$general_info['category_label'] = 'Tag Name';
			break;
	}
	
	//create HTML
	$template_path = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--header.tpl.php';
	$results_header_html = _uportal_backend_load_view($template_path, $general_info);
	
	//return general info
	return $results_header_html;
}
/** ENDS *****************************************/


/**
 * metadata form handler - content providers
 */
function uportal_backend_metadata_form_handler_content_providers() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'delete_node':
				_uportal_backend_metadata_delete_content_provider($_POST['data_values']['nid']);
				break;
			case 'add-content-provider':
			case 'edit-content-provider':
				_uportal_backend_metadata_edit_add_content_provider();
				break;
		}
	}
	
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'delete_node':
				_uportal_backend_metadata_delete_content_provider($_POST['data_values']['nid']);
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * metadata form handler - tags
 */
function uportal_backend_metadata_form_handler_tags() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'add-vocabulary':
			case 'edit-vocabulary':
				_uportal_backend_metadata_edit_add_vocabulary();
				break;
			case 'add-term':
			case 'edit-term':
				_uportal_backend_metadata_edit_add_term();
				break;
			case 'delete-term-or-vocabulary':
				_uportal_backend_metadata_delete_term_or_vocabulary();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * metadata form handler - languages
 */
function uportal_backend_metadata_form_handler_languages() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'add-language':
			case 'edit-language':
				_uportal_backend_metadata_edit_add_language();
				break;
			case 'delete-language':
				_uportal_backend_metadata_delete_language();
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * metadata form handler - languages
 */
function uportal_backend_metadata_form_handler_categories() {
	if (isset($_POST['operation_type'])) {
		switch ($_POST['operation_type']) {
			case 'delete_node':
				_uportal_backend_metadata_delete_category($_POST['data_values']['nid']);
				break;
		}
	}
}
/** ENDS *****************************************/


/**
 * metadata form handler
 */
function uportal_backend_metadata_form_handler() {
	if (isset($_POST['op'])) {
		switch ($_POST['op']) {
			case 'get-json-vocabularies':
				_uportal_backend_metadata_get_vocabularies_json();
				break;
			case 'get-existing-metadata':
				_uportal_backend_metadata_get_existing_metadata();
				break;
			case 'get-existing-taxonomy':
				_uportal_backend_metadata_get_existing_taxonomy();
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
 * delete language
 */
function _uportal_backend_metadata_delete_language() {
	module_load_include('admin.inc', 'locale');
	$langcode = $_POST['nid'];
	
	//build fake form and use existing locale functionality
	$form = array();
	$form_state = array (
		'values' => array (
			'langcode' => $langcode,
		),
	);
	locale_languages_delete_form_submit($form, $form_state);
	
	//output
	drupal_json_output(array(
		'nid' => $langcode,
	));
	
}
/** ENDS *****************************************/


/**
 * edit or add content provider
 */
function _uportal_backend_metadata_edit_add_language() {
	module_load_include('inc', 'content', 'includes/iso');
	
	$language_name = $_POST['title'];
	$langcode = $_POST['nid'];
	$language_name_native = $language_name;
	$language_direction = LANGUAGE_LTR;
	$operation_type = $_POST['op'];
	$operation_general = '';
	$node = false;
	
	if ($operation_type=='add-language' && $language_name) {
		$operation_general = 'add';
		$predef_languages = _locale_get_predefined_list();
		$installed_languages = locale_language_list('name');
		$language_is_predefined = false;
		$existing_langcodes = array_merge(array_keys($predef_languages), array_keys($installed_languages));
		foreach ($predef_languages as $predef_langcode => $predef_language) {
			if ($predef_language[0]==$language_name) {
				$langcode = $predef_langcode;
				if (isset($predef_language[1])) {
					$language_name_native = $predef_language[1];
				}
				if (isset($predef_language[2])) {
					$language_direction = $predef_language[2];
				}
				$language_is_predefined = true;
				break;
			}
		}
		if (!$language_is_predefined) {
			//find a unique code for the language
			// --- use pattern: up-lg i.e. uportal-language_name
			$new_langcode = 'up-';
			$language_name_str = strtolower(preg_replace("/[^a-zA-Z]+/", "", $language_name));
			$language_name_length = strlen($language_name_str);
			$code_length = 2;
			$alphabet_chars = str_split('abcdefghijklmnopqrstuvwxyz');
			$alphabet_index = 0;
			$alphabet_chars_length = count($alphabet_chars);
			while ($new_langcode=='up-') {
				//start at code length 2
				//increment till code_length=langname_length
				//if above happens, append alphabetic chars till unique string is found
				//otherwise, if unique, terminate at any point
				$new_langcode_names = str_split($language_name_str, $code_length);
				foreach ($new_langcode_names as $new_langcode_name) {
					$new_langcode_potential = 'up-'.$new_langcode_name;
					if (!in_array($new_langcode_potential, $existing_langcodes)) {
						$new_langcode = $new_langcode_potential;
						break;
					}
				}
				if ($code_length>=$language_name_length) {
					$language_name_str .= $alphabet_chars[$alphabet_index];
					$alphabet_index++;
					if ($alphabet_index>=$alphabet_chars_length) $alphabet_index=0;
				}
				$code_length++;
			}
			$langcode = $new_langcode;
		}
		
		//create new language
		locale_add_language($langcode, $language_name, $language_name_native, $language_direction);
		
	} else {
		$operation_general = 'edit';
		db_update('languages')
			->fields(array(
				'name' => $language_name,
			))
			->condition('language',  $langcode)
			->execute();
	}
	
	//make new row html
	$new_row_html = '';
	if ($operation_general=='add') {
		$new_language = array(
			'count-total' => 0,
			'category-results' => array(),
		);
		$row_content = _uportal_backend_create_html_row_categories($new_language, 'language');
		$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row.tpl.php';
		$row_vars = array (
			'parent_nid' => $langcode,
			'parent_title_html' => $language_name,
			'parent_url_content' => '',
			'parent_row_content' => $row_content,
		);
		$new_row_html = _uportal_backend_load_view($template_path_row, $row_vars);
	}
	
	//respond
	$json_data = array (
		'op' => $operation_type,
		'op_general' => $operation_general,
		'nid' => $langcode,
		'title' => $language_name,
		'new_row_html' => $new_row_html,
		'meta_data_type' => 'language',
		'initiator' => $_POST['initiator'],
	);
	drupal_json_output($json_data);
	
}
/** ENDS *****************************************/


/**
 * delete category
 */
function _uportal_backend_metadata_delete_category($nid) {
	$node = node_load($nid);
	$allowed_types = array('topic');
	
	if (!in_array($node->type, $allowed_types)) {
		return;
	}
	
	//delete all series under this category
	$category_series_query = new EntityFieldQuery();
	$category_series_query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('series'))
		->fieldCondition('field_topic_reference', 'target_id', $nid, '=');
	$category_series= $category_series_query->execute();
	if (isset($category_series['node'])) {
		$series_nids = array_keys($category_series['node']);
		node_delete_multiple(array($series_nids));
	}
	
	
	$json_output = array(
		'nid' => $nid,
	);
	node_delete_multiple(array($nid));
	drupal_json_output($json_output);
}
/** ENDS *****************************************/


/**
 * delete provider
 */
function _uportal_backend_metadata_delete_content_provider($nid) {
	$node = node_load($nid);
	$allowed_types = array('content_provider');
	
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
 * edit or add content provider
 */
function _uportal_backend_metadata_edit_add_content_provider() {
	global $user;
	$content_provider_name = $_POST['title'];
	$content_provider_url = $_POST['url'];
	$content_provider_nid = $_POST['nid'];
	$operation_type = $_POST['op'];
	$operation_general = '';
	$node = false;
	
	//prepare node
	if ($operation_type=='add-content-provider') {
		$node = new stdClass();
		$node->title = $content_provider_name;
		$node->type = 'content_provider';
		node_object_prepare($node);
		$node->language = LANGUAGE_NONE;
		$node->uid = $user->uid;
		$node->status = 1; //not published
		$node->promote = 0; //not promoted to front page
		$operation_general = 'add';
	} else {
		$node = node_load($content_provider_nid);
		$operation_general = 'edit';
	}
	
	if ($node) {
		//new values
		$node->title = $content_provider_name;
		if ($content_provider_url) {
			$langcode_url_field = field_language('node', $node, 'field_url');
			$node->field_url[$langcode_url_field][0] = array (
				'value' => $content_provider_url,
			);
		}
		
		//save node
		$node = node_submit($node);
		node_save($node);
	}
	
	//make new row html
	$new_row_html = '';
	if ($operation_general=='add') {
		$new_parent_node = array(
			'count-total' => 0,
			'category-results' => array(),
		);
		$row_content = _uportal_backend_create_html_row_categories($new_parent_node, 'content_provider');
		$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row.tpl.php';
		$row_vars = array (
			'parent_nid' => $node->nid,
			'parent_title_html' => $node->title,
			'parent_url_content' => $content_provider_url,
			'parent_row_content' => $row_content,
		);
		$new_row_html = _uportal_backend_load_view($template_path_row, $row_vars);
	}
	
	//respond
	$json_data = array (
		'op' => $operation_type,
		'op_general' => $operation_general,
		'nid' => $node->nid,
		'title' => $node->title,
		'url' => $content_provider_url,
		'new_row_html' => $new_row_html,
		'meta_data_type' => 'content-provider',
		'initiator' => $_POST['initiator'],
	);
	drupal_json_output($json_data);
	
}
/** ENDS *****************************************/


/**
 * get vocabularies in JSON format
 */
function _uportal_backend_metadata_get_vocabularies_json() {
	$vocabularies = taxonomy_get_vocabularies();
	drupal_json_output($vocabularies);
}
/** ENDS *****************************************/


/**
 * get existing metadata
 */
function _uportal_backend_metadata_get_existing_metadata() {
	//languages
	$languages = locale_language_list();
	
	//content providers
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'content_provider');
	$result = $query->execute();
	$content_providers = array();
	if (isset($result['node'])) {
		$nids = array_keys($result['node']);
	  $nodes = entity_load('node', $nids);
		foreach($nodes as $content_provider) {
			$content_providers['nid-'.$content_provider->nid] = $content_provider->title;
		}
	}
	
	drupal_json_output(array(
		'languages' => $languages,
		'content_providers' => $content_providers,
	));
}
/** ENDS *****************************************/


/**
 * edit existing vocabulary
 */
function _uportal_backend_metadata_get_existing_taxonomy() {
	$vocabularies = taxonomy_get_vocabularies();
	$vocabularies_json = array();
	foreach($vocabularies as $vocabulary) {
		$vocabularies_json['vid-'.$vocabulary->vid] = array (
			'vid' => $vocabulary->vid,
			'name' => $vocabulary->name,
			'machine_name' => $vocabulary->machine_name,
		);
	}
	
	//terms
	$terms = array();
	foreach ($vocabularies as $vocabulary) {
		$tree = taxonomy_get_tree($vocabulary->vid);
		foreach($tree as $term) {
			$terms['tid-'.$term->tid] = array (
				'tid' => $term->tid,
				'vid' => $vocabulary->vid,
				'name' => $term->name,
			);
		}
	}
	
	drupal_json_output(array(
		'vocabularies' => $vocabularies_json,
		'terms' => $terms,
	));
}
/** ENDS *****************************************/


/**
 * edit or create term
 */
function _uportal_backend_metadata_edit_add_term() {
	$vid = $_POST['tag_theme'];
	$tid = $_POST['tid'];
	$op_general = $_POST['op-general'];
	$term_title = $_POST['tag_name'];
	$old_vid = $_POST['old_vid'];
	
	if ($op_general=='add') {
		$term = (object) array( 
			'name' => $term_title,
			'vid' => $vid,
		);
		taxonomy_term_save($term);
	} elseif ($op_general=='edit') {
		$term = taxonomy_term_load($tid);
		$old_vid = $term->vid;
		$term->name = $term_title;
		$term->vid = $vid;
		taxonomy_term_save($term);
	}
	
	$term_vars = array(
		'title-html' => '
			<div class="tag-link-wrapper clearfix">
				<div class="tag-link float"><a href="javascript:;">'.$term->name.'</a></div>
				<div class="main-result-edit tag-edit float" data-tid="'.$term->tid.'" data-vid="'.$term->vid.'"></div>
				<div class="main-result-delete tag-delete float" data-tid="'.$term->tid.'" data-vid="'.$term->vid.'"></div>
			</div>
			',
		'classes' => '',
		'id-attribute' => ' id="result-row-tid-'.$term->tid.'"',
		'count-content-html' => '<a href="javascript:;">0</a>',
		'count-total' => 0,
		'video-count' => 0,
		'audio-count' => 0,
		'document-count' => 0,
	);
	$row_vars = array(
		'total_content_count' => $parent_result['count-total'],
		'result_content_rows' => array(),
		'print_only_result' => true,
		'result_content_rows' => array($term_vars),
	);
	$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row-content.tpl.php';
	$new_row_html = _uportal_backend_load_view($template_path_row, $row_vars);
	
	drupal_json_output(array(
		'vid' => $term->vid,
		'old_vid' => $old_vid,
		'tid' => $term->tid,
		'name' => $term->name,
		'op_general' => $_POST['op-general'],
		'initiator' => $_POST['initiator'],
		'new_row_html' => $new_row_html,
	));
}
/** ENDS *****************************************/


/**
 * edit or create vocabulary
 */
function _uportal_backend_metadata_edit_add_vocabulary() {
	$t = get_t();
	$vocabulary = false;
	$content_node_types = array('video', 'audio', 'document');
	
	if ($_POST['op-general']=='edit') {
		$vocabulary = taxonomy_vocabulary_load($_POST['vid']);
		$vocabulary->name = $_POST['title'];
	} elseif ($_POST['op-general']=='add') {
		$vocabulary = (object) array (
			'name' => $_POST['title'],
			'machine_name' => _uportal_backend_convert_human_string_to_machine($_POST['title']),
			'module' => 'uportal_backend',
		);
	}
	
	if (!$vocabulary) {
		drupal_json_output(array(
			'vid' => 0,
			'name' => $_POST['title'],
			'op_general' => $_POST['op-general'],
			'initiator' => $_POST['initiator'],
			'new_row_html' => '',
		));
		return;
	}
	
	//save vocabulary
	taxonomy_vocabulary_save($vocabulary);
		
	//create field if a new vocabulary is created
	if ($_POST['op-general']=='add') {
		$taxonomy_field_name = 'field_'.$vocabulary->machine_name.'_reference';
		
		//create field
		$taxonomy_field = array(
			'field_name' => $taxonomy_field_name,
			'type' => 'taxonomy_term_reference',
			'label' => $t($vocabulary->name),
			'cardinality' => '-1',
			'settings' => array(
				'allowed_values' => array(
					array(
						'vocabulary' => $vocabulary->machine_name,
						'parent' => 0
					)
				)
			)
		);
		field_create_field($taxonomy_field);
		
		//attach field to node types
		foreach($content_node_types as $node_type) {
			$instance = array(
				'field_name' => $taxonomy_field_name,
				'entity_type' => 'node',
				'bundle' => $node_type,
				'label' => $t($vocabulary->name),
				'description' => '',
				'required' => FALSE,
				'widget' => array(
					'type' => 'options_select',
					'weight' => 50,
				),
			);
			field_create_instance($instance);
		}
	}
	
	//update field name if vocabulary name has been edited
	if ($_POST['op-general']=='edit') {
		$taxonomy_field_name = 'field_'.$vocabulary->machine_name.'_reference';
		foreach($content_node_types as $node_type) {
			$instance_info = field_info_instance('node', $taxonomy_field_name, $node_type);
			$instance_info['label'] = $t($vocabulary->name);
			field_update_instance($instance_info);
		}
	}
	
	$row_vars = array (
		'parent_nid' => $vocabulary->vid,
		'parent_title_html' => $vocabulary->name,
		'parent_url_content' => '',
		'parent_content_count' => 0,
		'parent_row_content' => '
			<div class="result-content">
				<div class="result-content-header result-row clearfix">
					<div class="result-category-title">All</div>
					<div class="result-category-count">0</div>
					<div class="result-category-graph"></div>
				</div>
			</div>',
	);
	$template_path_row = drupal_get_path('module', 'uportal_backend').'/templates/managing-metadata/metadata--listing-page--results--row.tpl.php';
	$new_row_html = _uportal_backend_load_view($template_path_row, $row_vars);
	
	drupal_json_output(array(
		'vid' => $vocabulary->vid,
		'name' => $_POST['title'],
		'op_general' => $_POST['op-general'],
		'initiator' => $_POST['initiator'],
		'new_row_html' => $new_row_html,
	));
}
/** ENDS *****************************************/


/**
 * delete term or vocabulary
 */
function _uportal_backend_metadata_delete_term_or_vocabulary() {
	$type = $_POST['type'];
	$vid = isset($_POST['vid']) ? $_POST['vid'] : 0;
	$tid = isset($_POST['tid']) ? $_POST['tid'] : 0;
	
	//delete term
	if ($type=='term') {
		taxonomy_term_delete($tid);
	}
	
	//delete vocabulary
	if ($type=='vocabulary') {
		taxonomy_vocabulary_delete($vid);
	}
	
	drupal_json_output( array (
		'type' => $type,
		'tid' => $tid,
		'vid' => $vid,
	));
}
/** ENDS *****************************************/
