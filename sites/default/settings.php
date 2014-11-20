<?php $databases = array (
  'default' =>
  array (
    'default' =>
    array (
      'database' => 'ugyouthportaldb',
      'username' => 'db_user',
      'password' => 'db_password',
      'host' => 'localhost',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => '',
    ),
  ),
 );
 $update_free_access = FALSE; $drupal_hash_salt = 'd8EeYQJGwTg2ZpLnI0WCMM_obaDSa6zCnlp3gHGRfWg';

# $base_url = 'http://www.example.com'; // NO trailing slash!

ini_set('session.gc_probability', 1); 
ini_set('session.gc_divisor', 100); 
ini_set('session.gc_maxlifetime', 200000); 
ini_set('session.cookie_lifetime', 2000000);

## varnish settings
/* $conf['cache_backends'][] = 'sites/all/modules/varnish/varnish.cache.inc'; $conf['cache_class_cache_page'] = 'VarnishCache'; $conf['reverse_proxy'] = TRUE; 
$conf['page_cache_invoke_hooks'] = FALSE; $conf['cache'] = 1; $conf['cache_lifetime'] = 0; $conf['page_cache_maximum_age'] = 21600; $conf['reverse_proxy_header'] = 
'HTTP_X_FORWARDED_FOR'; $conf['reverse_proxy_addresses'] = array('127.0.0.1'); $conf['omit_vary_cookie'] = TRUE; $conf['varnish_control_key'] = 
substr(file_get_contents('/etc/varnish/secret'),0,-1); */

## memcache settings
 $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc'; 
$conf['cache_default_class'] = 'MemCacheDrupal'; 
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache'; 
$conf['memcache_key_prefix'] = 'uportal'; 

# ini_set('pcre.backtrack_limit', 200000); ini_set('pcre.recursion_limit', 200000); $cookie_domain = 'example.com'; $conf['site_name'] = 'My Drupal site'; 
# $conf['theme_default'] = 'garland'; $conf['anonymous'] = 'Visitor'; $conf['maintenance_theme'] = 'site_theme'; $conf['reverse_proxy'] = TRUE; $conf['reverse_proxy_header'] 
# = 'HTTP_X_CLUSTER_CLIENT_IP'; $conf['reverse_proxy_addresses'] = array('a.b.c.d', ...); $conf['omit_vary_cookie'] = TRUE; $conf['css_gzip_compression'] = FALSE; 
# $conf['js_gzip_compression'] = FALSE; $conf['locale_custom_strings_en'][''] = array(
#   'forum' => 'Discussion board', '@count min' => '@count minutes', ); $conf['blocked_ips'] = array( 'a.b.c.d', ); $conf['allow_authorize_operations'] = FALSE;
