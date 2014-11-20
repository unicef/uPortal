<?php
/**
 * @file
 * uportal_features.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function uportal_features_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'category_page';
  $context->description = '';
  $context->tag = 'page';
  $context->conditions = array(
    'context' => array(
      'values' => array(
        '~series_page' => '~series_page',
      ),
    ),
    'path' => array(
      'values' => array(
        'category/*' => 'category/*',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'ugyouthportal-listing_content' => array(
          'module' => 'ugyouthportal',
          'delta' => 'listing_content',
          'region' => 'content',
          'weight' => '-10',
        ),
        'views-utilities-block_2' => array(
          'module' => 'views',
          'delta' => 'utilities-block_2',
          'region' => 'sidebar_first',
          'weight' => '-9',
        ),
        'views-navigation-block_3' => array(
          'module' => 'views',
          'delta' => 'navigation-block_3',
          'region' => 'category_nav',
          'weight' => '-10',
        ),
        'views-navigation-block_4' => array(
          'module' => 'views',
          'delta' => 'navigation-block_4',
          'region' => 'category_nav',
          'weight' => '-9',
        ),
        'views-navigation-block_5' => array(
          'module' => 'views',
          'delta' => 'navigation-block_5',
          'region' => 'category_nav',
          'weight' => '-8',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('page');
  $export['category_page'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'content_page';
  $context->description = '';
  $context->tag = 'page';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'audio' => 'audio',
        'document' => 'document',
        'video' => 'video',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-statistics-block' => array(
          'module' => 'views',
          'delta' => 'statistics-block',
          'region' => 'footer',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('page');
  $export['content_page'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'global';
  $context->description = '';
  $context->tag = 'Global';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '*' => '*',
      ),
    ),
    'theme' => array(
      'values' => array(
        'ugyouthportal_theme' => 'ugyouthportal_theme',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-navigation-block_topics' => array(
          'module' => 'views',
          'delta' => 'navigation-block_topics',
          'region' => 'mega_menu',
          'weight' => '-10',
        ),
        'system-main' => array(
          'module' => 'system',
          'delta' => 'main',
          'region' => 'content',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Global');
  $export['global'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'home_page';
  $context->description = '';
  $context->tag = 'page';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'welcome-uportal' => 'welcome-uportal',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'ugyouthportal-listing_content' => array(
          'module' => 'ugyouthportal',
          'delta' => 'listing_content',
          'region' => 'content',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('page');
  $export['home_page'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'index_page';
  $context->description = 'Index page';
  $context->tag = 'page';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'content-index/*' => 'content-index/*',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'ugyouthportal-index_search_form' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_search_form',
          'region' => 'before_content',
          'weight' => '-10',
        ),
        'ugyouthportal-index_clear_search' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_clear_search',
          'region' => 'before_content',
          'weight' => '-9',
        ),
        'ugyouthportal-index_main_nav' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_main_nav',
          'region' => 'before_content',
          'weight' => '-8',
        ),
        'ugyouthportal-index_page_header' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_page_header',
          'region' => 'in_col_before_content',
          'weight' => '-9',
        ),
        'views-utilities-block_3' => array(
          'module' => 'views',
          'delta' => 'utilities-block_3',
          'region' => 'sidebar_first',
          'weight' => '-10',
        ),
        'ugyouthportal-index_nav_topics' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_nav_topics',
          'region' => 'category_nav',
          'weight' => '-10',
        ),
        'ugyouthportal-index_nav_types' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_nav_types',
          'region' => 'category_nav',
          'weight' => '-9',
        ),
        'ugyouthportal-index_nav_languages' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_nav_languages',
          'region' => 'category_nav',
          'weight' => '-8',
        ),
        'ugyouthportal-index_nav_tags' => array(
          'module' => 'ugyouthportal',
          'delta' => 'index_nav_tags',
          'region' => 'category_nav',
          'weight' => '-7',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Index page');
  t('page');
  $export['index_page'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'series_page';
  $context->description = 'Series page';
  $context->tag = 'page';
  $context->conditions = array(
    'views' => array(
      'values' => array(
        'listing_pages:page_4' => 'listing_pages:page_4',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-utilities-block_2' => array(
          'module' => 'views',
          'delta' => 'utilities-block_2',
          'region' => 'sidebar_first',
          'weight' => '-9',
        ),
        'views-navigation-block_3' => array(
          'module' => 'views',
          'delta' => 'navigation-block_3',
          'region' => 'category_nav',
          'weight' => '-10',
        ),
        'views-navigation-block_4' => array(
          'module' => 'views',
          'delta' => 'navigation-block_4',
          'region' => 'category_nav',
          'weight' => '-9',
        ),
        'views-navigation-block_5' => array(
          'module' => 'views',
          'delta' => 'navigation-block_5',
          'region' => 'category_nav',
          'weight' => '-8',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Series page');
  t('page');
  $export['series_page'] = $context;

  return $export;
}
