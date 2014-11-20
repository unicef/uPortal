<?php


/**
 *implements hook_pathauto
 */
function ugyouthportal_pathauto($op) {
	$settings = new stdClass();
	$settings->module = 'ugyouthportal';
  $settings->groupheader = t('Custom path aliases for uPortal');
  $settings->patterndescr = t('Default path pattern');
  $settings->patterndefault = '[node:title]';
  $settings->token_type = 'node';
  $settings->patternitems = array();
	/*
  $settings->patternitems = array(
		'category-page' => 'Category Page',
	);
	*/
  $settings->batch_update_callback = 'ugyouthportal_pathauto_bulkupdate';  
  return $settings;
}



/**
 * Implements hook_pathauto_bulkupdate.
 */
function ugyouthportal_pathauto_bulkupdate() {
  // find all node ids for the topic type
  $query = db_select('node', 'n');
  $query
    ->condition('n.type', 'topic')
    ->fields('n', array('nid'));
  $results = $query->execute()->fetchCol();
  $count = 0;
  foreach($results as $nid) {
    $node = node_load($nid);
    _ugyouthportal_create_alias($node, 'bulkupdate');
    $count++;
  }
	//dsm('Count: '.$count);
  drupal_set_message($count . ' custom uportal aliases were updated.');
}



/**
 * helper function to create the alias
 */
function _ugyouthportal_create_alias($node, $op) {
	
	//for topic: get all child series
	//for series: get parent topic
	//create for video, document, audio
	//create for all languages
	// -- use pathauto_cleanstring to remove unwanted words/tokens according to settings - pathauto.inc
	// -- use pathauto_clean_alias to clean alias according to settings
	// -- use pathauto_alias_uniquify to avoid duplicate aliases
	
	module_load_include('inc', 'pathauto');
	
	$paths_to_alias = array();
	$paths_before_languages_and_types = array();
	
	if ($node->type=='topic') {
		$category_paths = _ugyouthportal_get_category_paths_for_topic($node);
		/**
		 * creating index paths in the database is too expensive given random combinations of skill levels
		 * so created cum-random aliases with the real arguments as query parameters of the form $_GET['...']
		 *
		$index_paths = _ugyouthportal_get_index_paths_for_topic($node);
		$paths_before_languages_and_types = array_merge($paths_before_languages_and_types, $category_paths, $index_paths);
		*/
	}
	
	if ($node->type=='series') {
		$category_paths = _ugyouthportal_get_category_paths_for_series($node);
		$paths_before_languages_and_types = array_merge($paths_before_languages_and_types, $category_paths);
	}
	
	$paths_to_alias = _ugyouthportal_append_paths_to_types_and_languages($paths_before_languages_and_types);
	
	_ugyouthportal_create_aliases($paths_to_alias, $op, $node->language);
	
}


/**
 * get paths and append to media types and languages
 */
function _ugyouthportal_append_paths_to_types_and_languages($paths) {
	
	module_load_include('inc', 'pathauto');
	
	$appended_paths = array();
	
	//work on child paths
	$media_types = array(
		array( 'raw'=>'all', 'alias'=>pathauto_cleanstring('all-types') ),
		array( 'raw'=>'video', 'alias'=>pathauto_cleanstring('video') ),
		array( 'raw'=>'document', 'alias'=>pathauto_cleanstring('text') ),
		array( 'raw'=>'audio', 'alias'=>pathauto_cleanstring('audio') ),
	);
	$languages = array(
		array( 'raw'=>'all', 'alias'=>pathauto_cleanstring('all-languages') ),
	);
	$enabled_languages = locale_language_list('name');
	foreach($enabled_languages as $language_code => $language_name) {
		$languages[] = array( 'raw'=>$language_code, 'alias'=>pathauto_cleanstring($language_name) );
	}
	
	foreach ($paths as $path) {
		
		$raw_path = $path['raw'];
		$alias_path = $path['alias'];
	
		//aliasing the child paths for each series
		foreach ($media_types as $media_type) {
			foreach($languages as $language) {
				$appended_paths[] = array(
					'raw' => $raw_path.'/'.$media_type['raw'].'/'.$language['raw'],
					'alias' => $alias_path.'/'.$media_type['alias'].'/'.$language['alias'],
				);
			}
		}
	}
	
	return $appended_paths;
}


/**
 * get paths for a topic
 */
function _ugyouthportal_get_index_paths_for_topic($topic_node) {
	
	module_load_include('inc', 'pathauto');
	
	$index_paths = array();
	
	$topic_nid = $topic_node->nid;
	$topic_title_aliased = pathauto_cleanstring($topic_node->title);
	$raw_path = 'index/'.$topic_nid.'/';
	$alias_path = 'index/'.$topic_title_aliased.'/';
	
	//default for all content
	$index_paths[] = array(
		'raw' => $raw_path.'all',
		'alias' => $alias_path.pathauto_cleanstring('all-skill-levels'),
	);
	
	//get all skill levels and append to topic paths
	$get_skill_levels_query = "SELECT node.title AS node_title, node.nid AS nid, node.language AS node_language FROM {node} node WHERE (( (node.status = '1') AND (node.type IN  ('skill_level')) )) ORDER BY node_title ASC";
	$skill_level_results = db_query($get_skill_levels_query);
	$skill_levels = array();
	$skill_level_nids = array();
	foreach ($skill_level_results as $skill_level_result) {
		$skill_levels['nid-'.$skill_level_result->nid] = pathauto_cleanstring($skill_level_result->node_title);
		$skill_level_nids[] = $skill_level_result->nid;
	}
	
	//get all available combinations of skill levels in ascending order
	$no_skill_levels = count($skill_level_nids);
	$skill_level_combinations = array();
	for($i = 1; $i < (1 << $no_skill_levels); $i++) {
		$combination = array();
		for($j = 0; $j < $no_skill_levels; $j++)
			if($i & (1 << $j))
				$combination[] = $skill_level_nids[$j];
		$skill_level_combinations[] = $combination;
	}
	
	//make paths
	foreach ($skill_level_combinations as $skill_level_combination) {
		$no_of_levels_in_combination = count($skill_level_combination);
		$raw_skill_level_str = '';
		$alias_skill_level_str = '';
		for ($i=0; $i<$no_of_levels_in_combination; $i++) {
			$skill_level_nid = $skill_level_combination[$i];
			if ($i==0) {
				$raw_skill_level_str .= $skill_level_nid;
				$alias_skill_level_str .= $skill_levels['nid-'.$skill_level_nid];
			} else {
				$raw_skill_level_str .= ','.$skill_level_nid;
				$alias_skill_level_str .= ','.$skill_levels['nid-'.$skill_level_nid];
			}
		}
		
		//add combination to paths
		$index_paths[] = array(
			'raw' => $raw_path.$raw_skill_level_str,
			'alias' => $alias_path.$alias_skill_level_str,
		);
	}
	
	return $index_paths;

}


/**
 * get paths for a topic
 */
function _ugyouthportal_get_category_paths_for_topic($topic_node) {
	
	module_load_include('inc', 'pathauto');
	
	$category_paths = array();
	
	$topic_nid = $topic_node->nid;
	$topic_title_aliased = pathauto_cleanstring($topic_node->title);
	
	//default for all content
	$category_paths[] = array(
		'raw' => 'category/'.$topic_nid.'/all',
		'alias' => 'category/'.$topic_title_aliased.'/'.pathauto_cleanstring('all-content'),
	);
	
	//get series in this topic and alias them
	$topic_series = array();
	$get_series_query = "SELECT node.title AS node_title, node.nid AS nid, node.language AS node_language FROM {node} node LEFT JOIN {field_data_field_topic_reference} field_data_field_topic_reference ON node.nid = field_data_field_topic_reference.entity_id AND (field_data_field_topic_reference.entity_type = 'node' AND field_data_field_topic_reference.deleted = '0') WHERE (( (node.status = '1') AND (node.type IN  ('series')) AND (field_data_field_topic_reference.field_topic_reference_target_id = '$topic_nid') ))";
	$topic_series = db_query($get_series_query);
	foreach ($topic_series as $series) {
		$series_title_aliased = pathauto_cleanstring($series->node_title);
		$category_paths[] = array(
			'raw' => 'category/'.$topic_nid.'/'.$series->nid,
			'alias' => 'category/'.$topic_title_aliased.'/'.$series_title_aliased,
		);
	}
	
	return $category_paths;

}


/**
 * get paths for a series
 */
function _ugyouthportal_get_category_paths_for_series($series_node) {
	
	module_load_include('inc', 'pathauto');
	
	$category_paths = array();
	
	$series_nid = $series_node->nid;
	$series_title_aliased = pathauto_cleanstring($series_node->title);
	
	$topic_nid = $series_node->field_topic_reference['und'][0]['target_id'];
	$topic_title = db_query("SELECT node.title AS node_title FROM {node} node WHERE (( (node.nid = '$topic_nid') AND (node.type IN  ('topic')) ))")->fetchField();

	if ($topic_title) {
		$topic_title_aliased = pathauto_cleanstring($topic_title);
		$category_paths[] = array(
			'raw' => 'category/'.$topic_nid.'/'.$series_nid,
			'alias' => 'category/'.$topic_title_aliased.'/'.$series_title_aliased,
		);
	}
	
	return $category_paths;

}


/**
 * create paths
 *
 * expects array with raw and alias keys
 */
function _ugyouthportal_create_aliases($paths, $op, $language) {
	
	if (is_array($paths) && count($paths)) {
		
		module_load_include('inc', 'pathauto');
		
		foreach ($paths as $path) {
			$raw_path = $path['raw'];
			$alias_path = pathauto_clean_alias($path['alias']);
			pathauto_alias_uniquify($alias_path, $raw_path, $language);
		
			// Special handling when updating an item which is already aliased.
			$existing_alias = NULL;
			if ($op == 'update' || $op == 'bulkupdate') {
				if ($existing_alias = _pathauto_existing_alias_data($raw_path, $language)) {
					/*
					switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
						case PATHAUTO_UPDATE_ACTION_NO_NEW:
							// If an alias already exists, and the update action is set to do nothing,
							// then gosh-darn it, do nothing.
							return '';
					}
					*/
				}
			}

			$path = array(
				'source' => $raw_path,
				'alias' => $alias_path,
				'language' => $language,
			);
			
			$path = _pathauto_set_alias($path, $existing_alias, $op);
		}
	}
}

