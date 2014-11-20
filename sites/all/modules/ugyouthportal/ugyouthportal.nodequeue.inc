<?php

/**
 * update node queue data
 */	
function _ugyouthportal_update_node_queue_data($node, $op) {
	global $ugyouthportal_data;
	$nodequeue_defns = $ugyouthportal_data['nodequeue_defns'];
	
	if (isset($nodequeue_defns[$node->type])) {
		
		//this node's nodequeue definition
		$nodequeue_defn = $nodequeue_defns[$node->type];
		
		//if this type has a parent queue where all nodes of this type belong
		if ($nodequeue_defn['parent-queue-for-type']) {
			_ugyouthportal_update_parent_queue_for_type($node, $op);
		}
		
		//if this type has a child queue where nodes relating to it are supposed to belong
		if ($nodequeue_defn['child-queue-for-type']) {
			_ugyouthportal_update_child_queue_for_type($node, $op, $nodequeue_defn['children']);
		}
		
	}
	
	//if this type has parents or parent queues where it is supposed to belong, update
	$parent_queues_for_node = array();
	foreach ($nodequeue_defns as $parent_type => $defn) {
		foreach ($defn['children'] as $child_type => $relating_field) {
			if ($node->type == $child_type) {
				$parent_queues_for_node[] = array (
					'parent-type' => $parent_type,
					'relating-field' => $relating_field,
				);
			}
		}
	}
	if (count($parent_queues_for_node)) {
		foreach ($parent_queues_for_node as $parent_node) {
			_ugyouthportal_update_parent_queue_for_node($node, $op, $parent_node['parent-type'], $parent_node['relating-field']);
		}
	}
	
}
/** END ************************/


/**
 * update parent queue for types
 */
function _ugyouthportal_update_parent_queue_for_type($node, $op) {
	//first check for existence of queue, if absent, make one
	//only thing to update is its existence in this queue
	//i.e. if not in queue, add into queue
	
	$nid = $node->nid;
	$parent_queue_name = _ugyouthportal_help_get_queue_name($node, 'parent-queue-for-type', array());
	$parent_queue = nodequeue_load_queue_by_name($parent_queue_name['name']);
	
	//if queue does not exist, create it
	if (!isset($parent_queue->qid)) {
		$parent_queue_id = _ugyouthportal_create_nodequeue($parent_queue_name, array( 0=>$node->type ), array( 1=>$parent_queue_name['title'] ) );
		$parent_queue = nodequeue_load($parent_queue_id);
	}
	
	//only add to queue if it is an insert operation
	//nodequeue handles UPDATE and DELETE
	if (($op=='insert' || $op=='bulkupdate') && isset($parent_queue->qid)) {
		$subqueue = current(nodequeue_load_subqueues_by_queue($parent_queue->qid));
		$sqid = $subqueue->sqid;
		$nids_in_queue = nodequeue_nids_visible($sqid); //returns array keyed by nid with value of nid i.e. '$nid'=>$nid
		if (!isset($nids_in_queue[$nid])) {
			nodequeue_subqueue_add($parent_queue, $subqueue, $nid);
		}
	}
}
/** END ************************/


/**
 * update child queue for type
 */
function _ugyouthportal_update_child_queue_for_type($node, $op, $node_children) {
	//first check for existence of parent queue, if absent, make one
	//check for existence of subqueue for children, if absent, make one
	//check if title of subqueue is in sync with updated node title, update
	
	$nid = $node->nid;
	$parent_queue_name = _ugyouthportal_help_get_queue_name($node, 'child-queue-for-type', $node_children);
	$parent_queue = nodequeue_load_queue_by_name($parent_queue_name['name']);
	$parent_queue_exists = isset($parent_queue->qid);
	
	//if parent queue does not exist, create it
	if (!$parent_queue_exists) {
		$node_children_types = array_keys($node_children);
		$parent_queue_id = _ugyouthportal_create_nodequeue($parent_queue_name, $node_children_types, array() );
		$parent_queue = nodequeue_load($parent_queue_id);
	}
	
	/***
	 * above, we have created the parent queue e.g. Queue for the children of Lesson Plans
	 *
	 * now we need to create a subqueue in this parent queue for this node so that the children of this node may be kept
	 * 	within this subqueue e.g. lesson_plan_28 for the children of "Lesson Plan NID 28"
	 */
	
	//if child queue does not exist, create it
	$subqueue = false;
	$subqueue_name = _ugyouthportal_help_get_queue_name($node, 'subqueue-for-child-content', $node_children);
	$subqueues = nodequeue_load_subqueues_by_reference( array (
		$parent_queue->qid => array($subqueue_name['name'])
	));
	if (count($subqueues)) {
		$subqueue = current($subqueues);
		if ($subqueue_name['title']!=$subqueue->title) {
			nodequeue_subqueue_update_title($subqueue->sqid, $subqueue_name['title']);
		}
	} else {
		$subqueue = nodequeue_add_subqueue($parent_queue, $subqueue_name['title'], $subqueue_name['name']);
	}
	
	//if DELETE operation, delete this subqueue
	if ($op=='delete') {
		nodequeue_remove_subqueue($subqueue->sqid);
	}
	
}
/** END ************************/


/**
 * create node queue
 */
function _ugyouthportal_create_nodequeue($queue_names, $types, $subqueues) {
	$nodequeue = new stdClass;
  $nodequeue->name = $queue_names['name'];
  $nodequeue->title = $queue_names['title'];
  $nodequeue->subqueue_title = '%subqueue';
  $nodequeue->size = 0;
  $nodequeue->link = '';
  $nodequeue->link_remove = '';
  $nodequeue->owner = 'ugyouthportal';
  $nodequeue->show_in_ui = 1;
  $nodequeue->show_in_tab = 0;
  $nodequeue->show_in_links = 0;
  $nodequeue->reference = '0';
  $nodequeue->reverse = 0;
  $nodequeue->i18n = 0;
  $nodequeue->roles = array();
  $nodequeue->types = $types;
	if (count($subqueues)) {
		$nodequeue->add_subqueue = $subqueues;
	}
	return nodequeue_save($nodequeue);
}
/** END ************************/


/**
 * get name of a queue
 */
function _ugyouthportal_help_get_queue_name($node, $name_type, $node_children) {
	$queue_name = '';
	$queue_title = '';
	
	switch ($name_type) {
		case 'parent-queue-for-type':
			$queue_name = $node->type.'_queue';
			$queue_title = node_type_get_name($node).' Content';
			break;
		case 'child-queue-for-type':
			$queue_name = $node->type.'_child_queue';
			$queue_title = 'Children of '.node_type_get_name($node);
			break;
		case 'subqueue-for-child-content':
			$queue_name = $node->type.'_child_queue_'.$node->nid;
			$queue_title = 'Content in '.$node->title;
			break;
	}
	
	return array (
		'name' => $queue_name,
		'title' => $queue_title,
	);

}
/** END ************************/


/**
 * update parent queues with child content
 */
function _ugyouthportal_update_parent_queue_for_node($node, $op, $parent_node_type, $relating_field) {
	
	//no need to do anything for DELETE operation
	//nodequeue handles that operation
	if ($op!='delete') {
		$field_values = array();
		
		//check if updating
		if ($op=='update') {
			$new_field_values = _ugyouthportal_get_nids_from_reference_field($node, $relating_field);
			$old_field_values = _ugyouthportal_get_nids_from_reference_field($node->original, $relating_field);
			$new_values_to_be_added = array_diff($new_field_values, $old_field_values);
			$old_values_to_be_removed = array_diff($old_field_values, $new_field_values);
			$field_values = $new_values_to_be_added;
			_ugyouthportal_add_remove_parent_queue($old_values_to_be_removed, $node, 'remove');
		} else {
			$field_values = _ugyouthportal_get_nids_from_reference_field($node, $relating_field);
		}
		
		//add new values
		_ugyouthportal_add_remove_parent_queue($field_values, $node, 'add');
	}
}

/**
 * helper function to add or remove to queue
 */
function _ugyouthportal_add_remove_parent_queue($parent_nids, $node, $operation) {
	if (is_array($parent_nids) && count($parent_nids)) {
		foreach ($parent_nids as $parent_nid) {
			$parent_node = node_load($parent_nid);
			$parent_queue_name = _ugyouthportal_help_get_queue_name($parent_node, 'child-queue-for-type', array());
			$subqueue_name = _ugyouthportal_help_get_queue_name($parent_node, 'subqueue-for-child-content', array());
			$parent_queue = nodequeue_load_queue_by_name($parent_queue_name['name']);
			if (isset($parent_queue->qid)) {
				$subqueues = nodequeue_load_subqueues_by_reference( array (
					$parent_queue->qid => array($subqueue_name['name'])
				));
				if (count($subqueues)) {
					$subqueue = current($subqueues);
					if ($operation=='add') {
						nodequeue_subqueue_add($parent_queue, $subqueue, $node->nid);
					} elseif ($operation=='remove') {
						nodequeue_subqueue_remove_node($subqueue->sqid, $node->nid);
					}
				}
			}
		}
	}
}
/** END ************************/


/**
 * start batch process of updating nodes
 */
function ugyouthportal_bulk_update_queue() {
	
	drupal_set_message('Updating queue data for all content');
	
	//load topics first
	$topics = _ugyouthportal_node_load_by_type(array('topic'));
	
	//lesson plans and series
	$lesson_plans_series = _ugyouthportal_node_load_by_type(array('lesson_plan','series'));
	
	//content
	$content = _ugyouthportal_node_load_by_type(array('audio', 'document', 'video'));
	
	//all nodes, keeping order with topics first
	$all_nodes = array_merge($topics, $lesson_plans_series, $content);
	$node_count = count($all_nodes);
	
	$operations = array();
	foreach ($all_nodes as $nid=>$node) {
		$operations[] = array('_ugyouthportal_batch_update_queue_data_for_node', array($node));
	}
	
	$batch = array (
		'title' => t('Updating queues for all content'),
		'operations' => $operations,
		'error_message' => t('Error! Failed to update all queue data for content! Please try again.'),
		'finished' => '_ugyouthportal_finished_updating_node_queue_data',
		'file' => drupal_get_path('module', 'ugyouthportal').'/ugyouthportal.nodequeue.inc',
	);
	
	batch_set($batch);
	batch_process('admin/content');
}
/** END ************************/


/**
 * bulk update nodequeue
 */
function _ugyouthportal_batch_update_queue_data_for_node($node, &$context) {
	$context['results'][] = $node->nid . ' : ' . check_plain($node->title);
  // Optional message displayed under the progressbar.
  $context['message'] = t('Processing @type: "@title"', array('@title' => $node->title, '@type' => $node->type, ));
	_ugyouthportal_update_node_queue_data($node, 'bulkupdate');
	drupal_set_message('Queue data for '.$node->title.' has been updated.');
}
/** END ************************/


/**
 * bulk update finished
 */
function _ugyouthportal_finished_updating_node_queue_data($success, $results, $operations) {
	if ($success) {
    drupal_set_message(t('@count nodes have been updated.', array('@count' => count($results))));
  } else {
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
  }
}
/** END ************************/


/**
 * return values from reference field
 */
function _ugyouthportal_get_nids_from_reference_field($node, $field_name) {
	$field_values = field_get_items('node', $node, $field_name, $node->language);
	$field_nids = array();
	if (is_array($field_values) && count($field_values)) {
		foreach ($field_values as $value) {
			if (isset($value['target_id']) && is_numeric($value['target_id'])) {
				$field_nids[] = $value['target_id'];
			}
		}
	}
	return $field_nids;
}
/** END ************************/


/**
 * load multiple nodes by type
 */
function _ugyouthportal_node_load_by_type($types) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', $types, 'IN');
  $results = $query->execute();
  return node_load_multiple(array_keys($results['node']));
}
