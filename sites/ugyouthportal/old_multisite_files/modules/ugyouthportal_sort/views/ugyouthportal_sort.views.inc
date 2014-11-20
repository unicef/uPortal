<?php

/**
 * Implements hook_views_api().
 */
function ugyouthportal_sort_views_data() {
	//refer to:
	//    http://internetdevels.com/blog/describe-custom-table-for-views
	//    http://evolvingweb.ca/story/developing-views-part-i-describing-your-data-views
	//    http://www.metaltoad.com/blog/drupal-7-tutorial-creating-custom-filters-views
	//for comments

  $data['ugyouthportal_sort']['table']['group'] = t('uPortal Sort');
 
  // as a base table
  $data['ugyouthportal_sort']['table']['base'] = array(
    'field' => 'uportal_sort_nid',
    'title' => t('uPortal Sort Table'),
    'help' => t("This table contains sort positions for uPortal content."),
    'weight' => -10,
  );
 
  // relationship with NODE table via nid
  $data['ugyouthportal_sort']['table']['join'] = array(
    'node' => array( //connects to table "node"
      'left_field' => 'nid', //field in "node" table
      'field' => 'uportal_sort_nid', //field in "sort" table
    ),
  );
	
	//individual fields in this table
	$data['ugyouthportal_sort']['uportal_sort_nid'] = array(
    'title' => t('Content NID in the uPortal Sort Table'),
    'help' => t('Content NID in the uPortal Sort Table and is associated with a uPortal Sort Position'),
    'relationship' => array( //setting up relationship
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('uPortal Sort Content NID'),
    ),
  );
	$data['ugyouthportal_sort']['uportal_sort_posn'] = array(
    'title' => t('Content Position Index in uPortal Sort Table'),
    'help' => t('Content Position Index in uPortal Sort Table associated with content NID'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
	
	return $data;
}

