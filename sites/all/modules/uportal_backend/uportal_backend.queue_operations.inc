<?php

/**
 * rearrange queue
 */
function _uportal_backend_rearrange_queue($queue_type, $data_nids) {
	
	$queue_details = _uportal_backend_get_queue_details_by_type($queue_type);
	
	if (!$queue_details) {
		return;
	}
	
	$sqid = $queue_details['sqid'];
	$qid = $queue_details['qid'];
	$queue = nodequeue_load($qid);
	$subqueue = nodequeue_load_subqueue($sqid);
	
	//empty subqueue
	nodequeue_queue_clear($sqid);
	
	//add back to subqueue
	foreach($data_nids as $nid) {
		nodequeue_subqueue_add($queue, $subqueue, $nid);
	}
}
/** ENDS *****************************************/


/**
 * get queue ids, given type
 */
function _uportal_backend_get_queue_details_by_type($content_type) {
	$queue_details = false;
	switch ($content_type) {
		case 'topic':
			$queue_details = array (
				'sqid' => 52,
				'qid' => 13,
				'queue_name' => 'topic_queue',
				'subqueue_name' => '1',
			);
			break;
	}
	return $queue_details;
}
/** ENDS *****************************************/
