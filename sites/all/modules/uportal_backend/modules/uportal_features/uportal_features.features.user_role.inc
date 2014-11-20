<?php
/**
 * @file
 * uportal_features.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function uportal_features_user_default_roles() {
  $roles = array();

  // Exported role: Can manage categories.
  $roles['Can manage categories'] = array(
    'name' => 'Can manage categories',
    'weight' => '6',
  );

  // Exported role: Can manage content.
  $roles['Can manage content'] = array(
    'name' => 'Can manage content',
    'weight' => '5',
  );

  // Exported role: Can manage content providers.
  $roles['Can manage content providers'] = array(
    'name' => 'Can manage content providers',
    'weight' => '9',
  );

  // Exported role: Can manage languages.
  $roles['Can manage languages'] = array(
    'name' => 'Can manage languages',
    'weight' => '10',
  );

  // Exported role: Can manage lesson plans.
  $roles['Can manage lesson plans'] = array(
    'name' => 'Can manage lesson plans',
    'weight' => '8',
  );

  // Exported role: Can manage series.
  $roles['Can manage series'] = array(
    'name' => 'Can manage series',
    'weight' => '7',
  );

  // Exported role: Can manage tags.
  $roles['Can manage tags'] = array(
    'name' => 'Can manage tags',
    'weight' => '11',
  );

  // Exported role: Can manage users.
  $roles['Can manage users'] = array(
    'name' => 'Can manage users',
    'weight' => '12',
  );

  return $roles;
}
