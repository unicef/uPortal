<?php

/**
 * Implement hook_theme
 */
function uportal_backend_theme_theme($existing, $type, $theme, $path) {
	return array (
		'video_node_form' => array (
			'arguments' => array ( 'form' => NULL, ),
			'render element' => 'form',
			'template' => 'templates/edit-forms/content-node-form',
			'path' => drupal_get_path('theme', 'uportal_backend_theme'),
		),
		'audio_node_form' => array (
			'arguments' => array ( 'form' => NULL, ),
			'render element' => 'form',
			'template' => 'templates/edit-forms/content-node-form',
			'path' => drupal_get_path('theme', 'uportal_backend_theme'),
		),
		'document_node_form' => array (
			'arguments' => array ( 'form' => NULL, ),
			'render element' => 'form',
			'template' => 'templates/edit-forms/content-node-form',
			'path' => drupal_get_path('theme', 'uportal_backend_theme'),
		),
		'user_register_form' => array (
			'arguments' => array ( 'form' => NULL, ),
			'render element' => 'form',
			'template' => 'templates/user-forms/user-register-form',
			'path' => drupal_get_path('theme', 'uportal_backend_theme'),
		),
		'user_profile_form' => array (
			'arguments' => array ( 'form' => NULL, ),
			'render element' => 'form',
			'template' => 'templates/user-forms/user-profile-form',
			'path' => drupal_get_path('theme', 'uportal_backend_theme'),
		),
	);
}
/** ENDS *****************************************/


/**
 * Add body classes if certain regions have content.
 */
function uportal_backend_theme_preprocess_html(&$vars) {
	//print dsm($vars);
	
	//add more path specific classes
	$path = drupal_get_path_alias($_GET['q']);
	$path_parts = explode('/', $path);
	
	//body classes
	if (count($path_parts)>2 && $path_parts[0]=='management' && $path_parts[1]=='users' && ($path_parts[2]=='add' || $path_parts[2]=='edit')) {
		$vars['classes_array'][] = 'management-user-forms';
	}
	
	//add default javascript for everyone
	drupal_add_library('system', 'drupal.ajax');
	drupal_add_library('system', 'drupal.form');
	drupal_add_library('system', 'jquery.form');
	
}
/** ENDS *****************************************/


/**
 * Override or insert variables into the page template.
 */
function uportal_backend_theme_preprocess_page(&$vars) {
	
	//drupal_add_library('system', 'ui.autocomplete', TRUE);
	//drupal_add_library('system', 'ui.tabs', TRUE);
	
	//dsm($vars);
	
	if (arg(0)=='node' && is_numeric($nid=arg(1))) {
		$vars['page_type'] = 'node';
		$node = node_load($nid);
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
/** ENDS *****************************************/


/**
 * overriding forms
 */
function uportal_backend_theme_form_alter(&$form, &$form_state, $form_id) {
}
/** ENDS *****************************************/


/**
 * Override or insert variables into the node template.
 */
function uportal_backend_theme_preprocess_node(&$vars) {
  if ($vars['view_mode'] == 'full' && node_is_page($vars['node'])) {
    $vars['classes_array'][] = 'node-full';
    $vars['classes_array'][] = 'node-nid-'.$vars['nid'];
  }
}
/** ENDS *****************************************/


/**
 * Override or insert variables into the region template.
 */
function uportal_backend_theme_preprocess_region(&$vars) {
}
/** ENDS *****************************************/


/**
 * Override or insert variables into the block template.
 */
function uportal_backend_theme_preprocess_block(&$vars) {
  // In the header region visually hide block titles.
  if (isset($vars['block']->region) && $vars['block']->region == 'header') {
    $vars['title_attributes_array']['class'][] = 'element-invisible';
  }
  
  //add first and last classes for region
	if (isset($vars['block']->region)) {
		$block_count = count(uportal_backend_theme_block_list($vars['block']->region));
		if ($vars['block_id'] == 1 || $block_count == 1) {
			$vars['classes_array'][] = 'block-first';
		}
		if ($vars['block_id'] == $block_count) {
			$vars['classes_array'][] = 'block-last';
		}
	}
}
/** ENDS *****************************************/


/**
 * Implements theme_menu_tree().
 */
function uportal_backend_theme_menu_tree($vars) {
  return '<ul class="menu clearfix">' . $vars['tree'] . '</ul>';
}
/** ENDS *****************************************/


/**
 * get list of blocks in region
 */
function uportal_backend_theme_block_list($region) {
  // Code referenced from Fusion Core theme.
  $drupal_list = block_list($region);
  if (module_exists('context') && $context = context_get_plugin('reaction', 'block')) {
    $context_list = $context->block_list($region);
    $drupal_list = array_merge($context_list, $drupal_list);
  }
  return $drupal_list;
}
/** ENDS *****************************************/


/**
 * changing javascript for some pages
 */
function uportal_backend_theme_js_alter(&$javascript) {
	unset($javascript['sites/all/modules/admin_menu/admin_menu.js']);
	unset($javascript['sites/all/modules/admin_menu_toolbar/admin_menu_toolbar.js']);
	//$javascript['sites/all/modules/jquery_update/replace/jquery/1.5/jquery.min.js']['data'] = drupal_get_path('module', 'jquery_update') . '/replace/jquery/1.8/jquery.min.js';
}
/** ENDS *****************************************/


/**
 * change required label for user form
 */
function uportal_backend_theme_form_required_marker($variables) {
	//node form
	if (
		isset($variables['element']['#array_parents']) && count($variables['element']['#array_parents'])>0 && $variables['element']['#array_parents'][0]=='account'
	) {
		$t = get_t();
		$attributes = array(
			'class' => 'form-required',
			'title' => $t('This field is required.'),
		);
		return '<span' . drupal_attributes($attributes) . '>(required)</span>';
	} else {
		return theme_form_required_marker($variables);
	}
}
/** ENDS *****************************************/


/**
 * theme form item
 * override: theme_form_item
 */
function uportal_backend_theme_form_element($variables) {
	$element = &$variables['element'];
	
	if (!(isset($element['#array_parents']) && count($element['#array_parents'])>0 && $element['#array_parents'][0]=='account')) {
		return theme_form_element($variables);
	}

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
  );

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $attributes['class'] = array('form-item');
  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';

  switch ($element['#title_display']) {
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
    $output .= '<div class="description">' . $element['#description'] . "<div class=\"arrow\"></div></div>\n";
	}

	$output .= "</div>\n"; //close off field elements and main wrapper

  return $output;
}
/** ENDS *****************************************/
