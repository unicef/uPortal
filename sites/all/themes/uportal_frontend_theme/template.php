<?php

/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return a string containing the breadcrumb output.
 */
function uportal_frontend_theme_breadcrumb($vars) {
  $breadcrumb = $vars['breadcrumb'];

  if (!empty($breadcrumb)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">You are here:</h2><div class="breadcrumb">'.implode('<span class="sep">›››</span>', $breadcrumb) . '</div>';
    return $output;
  }
}


/**
 * Add body classes if certain regions have content.
 */
function uportal_frontend_theme_preprocess_html(&$vars) {
	//print dsm($vars);
	//drupal_add_css('http://fonts.googleapis.com/css?family=Armata|Wellfleet', array( 'type'=>'external', 'group'=>CSS_DEFAULT, 'preprocess'=>FALSE ));
	if ($vars['is_front']) {
    $vars['classes_array'][] = 'no-h1';
	}
	if (!empty($vars['page']['featured'])) {
    $vars['classes_array'][] = 'featured';
  }
	
	//add more path specific classes
	$path = drupal_get_path_alias($_GET['q']);
	if (!$vars['is_front']) {
		$path_parts = explode('/', $path);
		$vars['classes_array'][] = drupal_html_class('section-'.$path_parts[0]);
		if (isset($path_parts[1])) $vars['classes_array'][] = drupal_html_class('section-2-'.$path_parts[1]);
		if (arg(0)=='node' && is_numeric($nid=arg(1))) {
			$no_h1_nids = array('1078'=>1, '234'=>1, );
			//$no_breadcrumb_nids = array('10'=>1);
			if (isset($no_h1_nids[$nid]))
				$vars['classes_array'][] = 'no-h1';
				
			//add category class nid
			$node = node_load($nid);
			$node_types = array('audio','video','document');
			if (in_array($node->type, $node_types)) {
				$vars['classes_array'][] = 'category-class-nid-'.$node->field_topic_reference['und'][0]['target_id'];
			}
		}
		
		//check paths
		$no_h1_paths = array('products'=>1, );
		if (isset($no_h1_paths[$path]))
			$vars['classes_array'][] = 'no-h1';
		
		//page callbacks
		$vars['menu_item'] = menu_get_item();
		switch ($vars['menu_item']['page_callback']) {
			case 'views_page':
				// Is this a Views page?
				$vars['classes_array'][] = 'page-views';
				$class_str = '';
				$path_parts_no = count($path_parts);
				for ($i=0; $i<$path_parts_no; $i++) {
					$class_str .= $path_parts[$i];
					if ($i<$path_parts_no-1) $class_str .= '-';
				}
				$vars['classes_array'][] = 'page-views-'.$class_str;
				break;
			case 'page_manager_page_execute':
			case 'page_manager_node_view':
			case 'page_manager_contact_site':
				// Is this a Panels page?
				$vars['classes_array'][] = 'page-panels';
				break;
		}
	}
	
	/*
  // Add conditional stylesheets for IE
  drupal_add_css(path_to_theme() . '/css/ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'lte IE 7', '!IE' => FALSE), 'preprocess' => FALSE));
  drupal_add_css(path_to_theme() . '/css/ie6.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'IE 6', '!IE' => FALSE), 'preprocess' => FALSE));
	*/
}


/**
 * Override or insert variables into the page template.
 */
function uportal_frontend_theme_preprocess_page(&$vars) {
	$vars['page_type'] = '';
	$vars['title_string'] = false;
	
	if (arg(0)=='node' && is_numeric($nid=arg(1))) {
		$vars['page_type'] = 'node';
		$node = node_load($nid);
		
		if ($node->type=='video' || $node->type=='audio' || $node->type=='document') {
			$vars['title_string'] = ' ';
		}
	}
	
	if (isset($vars['main_menu'])) {
    $vars['primary_nav'] = theme('links__system_main_menu', array(
      'links' => $vars['main_menu'],
      'attributes' => array(
        'class' => array('navbar', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Main menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  else {
    $vars['primary_nav'] = FALSE;
  }
	
  $vars['home_link_logo'] = l('', '<front>', array(
  	'attributes' => array (
  		'title'	=>	'Back to home page',
  	),
  	'html'	=> false,
  ));
  
}


/**
 * overriding search form
 */
function uportal_frontend_theme_form_alter(&$form, &$form_state, $form_id) {
	if ($form['#id'] == 'comment-form') {
		$body_label = '';
		switch ($form['#node']->type) {
			case 'video': $body_label = 'video'; break;
			case 'audio': $body_label = 'audio'; break;
			case 'document': $body_label = 'document'; break;
			case 'series': $body_label = 'series'; break;
		}
    $form['author']['#title_display'] = 'invisible';
    $form['author']['_author']['#title_display'] = 'invisible';
    $form['author']['#weight'] = 1;
		$form['author']['name']['#title_display'] = 'invisible';
		$form['author']['name']['#attributes']['placeholder'] = 'Type your name here :-)';
		
		$form['comment_body']['und'][0]['#title_display'] = 'invisible';
		$form['comment_body']['und'][0]['#attributes']['placeholder'] = 'Say something about this '.$body_label.' ...';
		$form['comment_body']['und'][0]['#title'] = $body_label;
		$form['comment_body']['und']['#after_build'][] = '_uportal_frontend_theme_customise_comment_form';
		$form['actions']['submit']['#value'] = 'Post';
		$form['actions']['submit']['#attributes']['class'] = array('btn');
	}
}


/**
 * customise comment form body after building
 */
function _uportal_frontend_theme_customise_comment_form(&$form) {
	$form[0]['format']['#access'] = false;
	return $form;
}


/**
 * overriding comment textarea
 */
function uportal_frontend_theme_textarea($vars) {
	if ($vars['element']['#entity_type']=='comment') {
		$element = $vars['element'];
		element_set_attributes($element, array('id', 'name', 'cols', 'rows'));
		_form_set_class($element, array('form-textarea'));
	
		$wrapper_attributes = array(
			'class' => array('form-textarea-wrapper'),
		);
	
		/*
		// Add resizable behavior.
		if (!empty($element['#resizable'])) {
			drupal_add_library('system', 'drupal.textarea');
			$wrapper_attributes['class'][] = 'resizable';
		}
		*/
	
		$output = '<div' . drupal_attributes($wrapper_attributes) . '>';
		$output .= '<textarea' . drupal_attributes($element['#attributes']) . '>' . check_plain($element['#value']) . '</textarea>';
		$output .= '</div>';
		return $output;
	} else {
		return theme_textarea($vars);
	}
}



/**
 * Implements hook_preprocess_maintenance_page().
 */
function uportal_frontend_theme_preprocess_maintenance_page(&$vars) {
  //drupal_add_css(drupal_get_path('theme', 'ugyouthportal') . '/css/maintenance-page.css');
}

/**
 * Override or insert variables into the maintenance page template.
 */
function uportal_frontend_theme_process_maintenance_page(&$vars) {
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
}

/**
 * Override or insert variables into the node template.
 */
function uportal_frontend_theme_preprocess_node(&$vars) {
  if ($vars['view_mode'] == 'full' && node_is_page($vars['node'])) {
    $vars['classes_array'][] = 'node-full';
    $vars['classes_array'][] = 'node-nid-'.$vars['nid'];
  }
}


/**
 * Override or insert variables into the region template.
 */
function uportal_frontend_theme_preprocess_region(&$vars) {
	//print kpr($vars);
	if ($vars['region']=='footer_2') {
		$vars['classes_array'][] = 'col-md-6';
	}
	if ($vars['region']=='footer_menu') {
		$vars['classes_array'][] = 'col-md-6';
	}
	if ($vars['region']=='content_2' && $_GET['q']=='node/1078') {
		$vars['classes_array'][] = 'row';
	}
	if ($vars['region']=='search_box') {
		$vars['classes_array'][] = 'col-md-offset-9';
	}
}


/**
 * override webforms
 */
function uportal_frontend_theme_webform_element(&$variables) {
	// Ensure defaults.
  $variables['element'] += array(
    '#title_display' => 'before',
  );

  $element = $variables['element'];

  // All elements using this for display only are given the "display" type.
  if (isset($element['#format']) && $element['#format'] == 'html') {
    $type = 'display';
  }
  else {
    $type = (isset($element['#type']) && !in_array($element['#type'], array('markup', 'textfield', 'webform_email', 'webform_number'))) ? $element['#type'] : $element['#webform_component']['type'];
  }

  // Convert the parents array into a string, excluding the "submitted" wrapper.
  $nested_level = $element['#parents'][0] == 'submitted' ? 1 : 0;
  $parents = str_replace('_', '-', implode('--', array_slice($element['#parents'], $nested_level)));

  $wrapper_classes = array(
    'form-item',
		'form-group',
    'webform-component',
    'webform-component-' . $type,
  );
  if (isset($element['#title_display']) && strcmp($element['#title_display'], 'inline') === 0) {
    $wrapper_classes[] = 'webform-container-inline';
  }
  $output = '<div class="' . implode(' ', $wrapper_classes) . '" id="webform-component-' . $parents . '">' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . _webform_filter_xss($element['#field_prefix']) . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . _webform_filter_xss($element['#field_suffix']) . '</span>' : '';

  switch ($element['#title_display']) {
    case 'inline':
    case 'before':
    case 'invisible':
      $output .= ' ' . theme('form_element_label', $variables);
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;

    case 'after':
      $output .= ' ' . $prefix . $element['#children'] . $suffix;
      $output .= ' ' . theme('form_element_label', $variables) . "\n";
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;
  }

  if (!empty($element['#description'])) {
    $output .= ' <div class="description">' . $element['#description'] . "</div>\n";
  }

  $output .= "</div>\n";

  return $output;
}


/**
 * Override or insert variables into the block template.
 */
function uportal_frontend_theme_preprocess_block(&$vars) {
  // In the header region visually hide block titles.
  if ($vars['block']->region == 'header') {
    $vars['title_attributes_array']['class'][] = 'element-invisible';
  }
  
  //add first and last classes for region
  $block_count = count(uportal_frontend_theme_block_list($vars['block']->region));
  if ($vars['block_id'] == 1 || $block_count == 1) {
    $vars['classes_array'][] = 'block-first';
  }
  if ($vars['block_id'] == $block_count) {
    $vars['classes_array'][] = 'block-last';
  }
	
	//add relevant classes
	//print kpr($vars);
	if ($vars['block']->region=='footer_2') {
		if ($vars['block_html_id']=='block-block-4' || $vars['block_html_id']=='block-webform-client-block-1114') { //contacts and sign up blocks
			$vars['classes_array'][] = 'col-md-6';
		}
	}
	if ($vars['block']->region=='footer_menu') {
		if ($vars['block_html_id']=='block-menu-menu-footer-menu') {
			$vars['classes_array'][] = 'col-md-4';
		}
		if ($vars['block_html_id']=='block-views-countries-block-3') {
			$vars['classes_array'][] = 'col-md-8';
		}
	}
	if ($vars['block']->region=='content_2' && $_GET['q']=='node/1078') {
		$vars['classes_array'][] = 'col-md-3';
	}
}

/**
 * Implements theme_menu_tree().
 */
function uportal_frontend_theme_menu_tree($vars) {
  return '<ul class="menu clearfix">' . $vars['tree'] . '</ul>';
}


/**
 * get list of blocks in region
 */
function uportal_frontend_theme_block_list($region) {
  // Code referenced from Fusion Core theme.
  $drupal_list = block_list($region);
  if (module_exists('context') && $context = context_get_plugin('reaction', 'block')) {
    $context_list = $context->block_list($region);
    $drupal_list = array_merge($context_list, $drupal_list);
  }
  return $drupal_list;
}




/**
 * changing javascript for some pages
 */
function uportal_frontend_theme_js_alter(&$javascript) {
	unset($javascript['sites/all/modules/admin_menu/admin_menu.js']);
	unset($javascript['sites/all/modules/admin_menu_toolbar/admin_menu_toolbar.js']);
	/*
	 *if (arg(0)=='welcome' || (arg(0)=='node' && arg(1)=='1165')) {
		//kpr($javascript);
		$javascript['sites/all/modules/jquery_update/replace/jquery/1.5/jquery.min.js']['data'] = drupal_get_path('module', 'jquery_update') . '/replace/jquery/1.8/jquery.min.js';
	}
	*/
	$javascript['sites/all/modules/jquery_update/replace/jquery/1.5/jquery.min.js']['data'] = drupal_get_path('module', 'jquery_update') . '/replace/jquery/1.8/jquery.min.js';
}
