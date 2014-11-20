<?php

/**
 * install page callback
 */
function ugyouthportal_install_nodequeue() {
	$content = '<p>Click the button below to install the following queues:</p><ul><li>Topics Queue</li><li>Sub-topics Queue</li><li>Sub-topics\' Content Queue</li><li>Series Content Queue</li></ul>';
	$queues_installed = variable_get('ugyouthportal_queues_installed', false);
	//variable_set('ugyouthportal_queues_installed', false);
	//kpr('installed: '.$queues_installed);
	
	if ($queues_installed) {
		/**
		 * TODO: Uninstall queues ... more like never to do!
		 */
		$content = '<p>The queues have already been enabled!</p><p><strong>(TODO: uninstall queues)</strong></p>';
		return array (
			'content' => array (
				'#type' => 'markup',
				'#markup' => $content,
			),
		);
	} else {
		return array (
			'content' => array (
				'#type' => 'markup',
				'#markup' => $content,
			),
			'form' => drupal_get_form('ugyouthportal_install_nodequeue_form'),
		);
	}
}

/**
 * create install nodequeue form
 */
function ugyouthportal_install_nodequeue_form($form, $form_state) {
	return system_settings_form(array(
		'#submit' => array('ugyouthportal_install_nodequeue_form_submit'),
	));
}

/**
 * install nodequeue form submitted
 */
function ugyouthportal_install_nodequeue_form_submit($form_id, $form_state) {
	//install queues
	$queues = array (
		'topics_queue' => array (
			'title' => 'Topics Queue',
			'subqueues' => array(
				1 => 'Topics Queue',
			),
			'types' => array (
				0 => 'topic',
			),
		),
		'sub_topics_queue' => array (
			'title' => 'Sub-topics Queue',
			'subqueues' => array(),
			'types' => array (
				0 => 'sub_topic',
			),
		),
		'sub_topic_content_queue' => array (
			'title' => 'Sub-topic Content Queue',
			'subqueues' => array(),
			'types' => array (
				0 => 'series',
				1 => 'video',
				2 => 'audio',
				3 => 'document',
			),
		),
		'series_content_queue' => array (
			'title' => 'Series Content Queue',
			'subqueues' => array(),
			'types' => array (
				0 => 'video',
				1 => 'audio',
				2 => 'document',
			),
		),
	);
	
	foreach ($queues as $queue_name => $queue_details) {
		$nodequeue = new stdClass;
		$nodequeue->api_version = 2;
		$nodequeue->name = $queue_name;
		$nodequeue->title = $queue_details['title'];
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
		$nodequeue->roles = array(3);
		$nodequeue->types = $queue_details['types'];
		$nodequeue->add_subqueue = $queue_details['subqueues'];
		nodequeue_save($nodequeue);
	}
	
	variable_set('ugyouthportal_queues_installed', true);
	drupal_set_message(t('You have installed the UG Youth Portal queues!'));
}

