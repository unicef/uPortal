<?php
/**
 * @file
 * uportal_features.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function uportal_features_user_default_permissions() {
  $permissions = array();

  // Exported permission: access comments.
  $permissions['access comments'] = array(
    'name' => 'access comments',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: access content.
  $permissions['access content'] = array(
    'name' => 'access content',
    'roles' => array(
      0 => 'Can manage content',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'node',
  );

  // Exported permission: access user profiles.
  $permissions['access user profiles'] = array(
    'name' => 'access user profiles',
    'roles' => array(
      0 => 'authenticated user',
    ),
    'module' => 'user',
  );

  // Exported permission: administer comments.
  $permissions['administer comments'] = array(
    'name' => 'administer comments',
    'roles' => array(),
    'module' => 'comment',
  );

  // Exported permission: administer languages.
  $permissions['administer languages'] = array(
    'name' => 'administer languages',
    'roles' => array(
      0 => 'Can manage languages',
    ),
    'module' => 'locale',
  );

  // Exported permission: administer nodequeue.
  $permissions['administer nodequeue'] = array(
    'name' => 'administer nodequeue',
    'roles' => array(),
    'module' => 'nodequeue',
  );

  // Exported permission: administer nodes.
  $permissions['administer nodes'] = array(
    'name' => 'administer nodes',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: administer permissions.
  $permissions['administer permissions'] = array(
    'name' => 'administer permissions',
    'roles' => array(
      0 => 'Can manage users',
    ),
    'module' => 'user',
  );

  // Exported permission: administer taxonomy.
  $permissions['administer taxonomy'] = array(
    'name' => 'administer taxonomy',
    'roles' => array(
      0 => 'Can manage tags',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: administer users.
  $permissions['administer users'] = array(
    'name' => 'administer users',
    'roles' => array(
      0 => 'Can manage users',
    ),
    'module' => 'user',
  );

  // Exported permission: bypass conversion video.
  $permissions['bypass conversion video'] = array(
    'name' => 'bypass conversion video',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'video',
  );

  // Exported permission: change own username.
  $permissions['change own username'] = array(
    'name' => 'change own username',
    'roles' => array(
      0 => 'authenticated user',
    ),
    'module' => 'user',
  );

  // Exported permission: convert on submission.
  $permissions['convert on submission'] = array(
    'name' => 'convert on submission',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'video',
  );

  // Exported permission: create audio content.
  $permissions['create audio content'] = array(
    'name' => 'create audio content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: create content_provider content.
  $permissions['create content_provider content'] = array(
    'name' => 'create content_provider content',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'node',
  );

  // Exported permission: create document content.
  $permissions['create document content'] = array(
    'name' => 'create document content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: create lesson_plan content.
  $permissions['create lesson_plan content'] = array(
    'name' => 'create lesson_plan content',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'node',
  );

  // Exported permission: create page content.
  $permissions['create page content'] = array(
    'name' => 'create page content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: create series content.
  $permissions['create series content'] = array(
    'name' => 'create series content',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'node',
  );

  // Exported permission: create topic content.
  $permissions['create topic content'] = array(
    'name' => 'create topic content',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'node',
  );

  // Exported permission: create url aliases.
  $permissions['create url aliases'] = array(
    'name' => 'create url aliases',
    'roles' => array(),
    'module' => 'path',
  );

  // Exported permission: create video content.
  $permissions['create video content'] = array(
    'name' => 'create video content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any audio content.
  $permissions['delete any audio content'] = array(
    'name' => 'delete any audio content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any content_provider content.
  $permissions['delete any content_provider content'] = array(
    'name' => 'delete any content_provider content',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any document content.
  $permissions['delete any document content'] = array(
    'name' => 'delete any document content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any lesson_plan content.
  $permissions['delete any lesson_plan content'] = array(
    'name' => 'delete any lesson_plan content',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any page content.
  $permissions['delete any page content'] = array(
    'name' => 'delete any page content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: delete any series content.
  $permissions['delete any series content'] = array(
    'name' => 'delete any series content',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any topic content.
  $permissions['delete any topic content'] = array(
    'name' => 'delete any topic content',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any video content.
  $permissions['delete any video content'] = array(
    'name' => 'delete any video content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own audio content.
  $permissions['delete own audio content'] = array(
    'name' => 'delete own audio content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own comments.
  $permissions['delete own comments'] = array(
    'name' => 'delete own comments',
    'roles' => array(),
    'module' => 'comment_goodness',
  );

  // Exported permission: delete own content_provider content.
  $permissions['delete own content_provider content'] = array(
    'name' => 'delete own content_provider content',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own document content.
  $permissions['delete own document content'] = array(
    'name' => 'delete own document content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own lesson_plan content.
  $permissions['delete own lesson_plan content'] = array(
    'name' => 'delete own lesson_plan content',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own page content.
  $permissions['delete own page content'] = array(
    'name' => 'delete own page content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: delete own series content.
  $permissions['delete own series content'] = array(
    'name' => 'delete own series content',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own topic content.
  $permissions['delete own topic content'] = array(
    'name' => 'delete own topic content',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own video content.
  $permissions['delete own video content'] = array(
    'name' => 'delete own video content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: delete revisions.
  $permissions['delete revisions'] = array(
    'name' => 'delete revisions',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: download all audio files.
  $permissions['download all audio files'] = array(
    'name' => 'download all audio files',
    'roles' => array(
      0 => 'Can manage content',
      1 => 'anonymous user',
    ),
    'module' => 'audiofield',
  );

  // Exported permission: download own audio files.
  $permissions['download own audio files'] = array(
    'name' => 'download own audio files',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'audiofield',
  );

  // Exported permission: edit any audio content.
  $permissions['edit any audio content'] = array(
    'name' => 'edit any audio content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any content_provider content.
  $permissions['edit any content_provider content'] = array(
    'name' => 'edit any content_provider content',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any document content.
  $permissions['edit any document content'] = array(
    'name' => 'edit any document content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any lesson_plan content.
  $permissions['edit any lesson_plan content'] = array(
    'name' => 'edit any lesson_plan content',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any page content.
  $permissions['edit any page content'] = array(
    'name' => 'edit any page content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: edit any series content.
  $permissions['edit any series content'] = array(
    'name' => 'edit any series content',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any topic content.
  $permissions['edit any topic content'] = array(
    'name' => 'edit any topic content',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any video content.
  $permissions['edit any video content'] = array(
    'name' => 'edit any video content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: edit content.
  $permissions['edit content'] = array(
    'name' => 'edit content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: edit own audio content.
  $permissions['edit own audio content'] = array(
    'name' => 'edit own audio content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own comments.
  $permissions['edit own comments'] = array(
    'name' => 'edit own comments',
    'roles' => array(),
    'module' => 'comment',
  );

  // Exported permission: edit own content_provider content.
  $permissions['edit own content_provider content'] = array(
    'name' => 'edit own content_provider content',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own document content.
  $permissions['edit own document content'] = array(
    'name' => 'edit own document content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own lesson_plan content.
  $permissions['edit own lesson_plan content'] = array(
    'name' => 'edit own lesson_plan content',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own page content.
  $permissions['edit own page content'] = array(
    'name' => 'edit own page content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: edit own series content.
  $permissions['edit own series content'] = array(
    'name' => 'edit own series content',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own topic content.
  $permissions['edit own topic content'] = array(
    'name' => 'edit own topic content',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own video content.
  $permissions['edit own video content'] = array(
    'name' => 'edit own video content',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: manage categories.
  $permissions['manage categories'] = array(
    'name' => 'manage categories',
    'roles' => array(
      0 => 'Can manage categories',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage content providers.
  $permissions['manage content providers'] = array(
    'name' => 'manage content providers',
    'roles' => array(
      0 => 'Can manage content providers',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage languages.
  $permissions['manage languages'] = array(
    'name' => 'manage languages',
    'roles' => array(
      0 => 'Can manage languages',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage lesson plans.
  $permissions['manage lesson plans'] = array(
    'name' => 'manage lesson plans',
    'roles' => array(
      0 => 'Can manage lesson plans',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage metadata.
  $permissions['manage metadata'] = array(
    'name' => 'manage metadata',
    'roles' => array(
      0 => 'Can manage categories',
      1 => 'Can manage content providers',
      2 => 'Can manage languages',
      3 => 'Can manage lesson plans',
      4 => 'Can manage series',
      5 => 'Can manage tags',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage series.
  $permissions['manage series'] = array(
    'name' => 'manage series',
    'roles' => array(
      0 => 'Can manage series',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage tags.
  $permissions['manage tags'] = array(
    'name' => 'manage tags',
    'roles' => array(
      0 => 'Can manage tags',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manage users.
  $permissions['manage users'] = array(
    'name' => 'manage users',
    'roles' => array(
      0 => 'Can manage users',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: manipulate all queues.
  $permissions['manipulate all queues'] = array(
    'name' => 'manipulate all queues',
    'roles' => array(
      0 => 'Can manage series',
      1 => 'authenticated user',
    ),
    'module' => 'nodequeue',
  );

  // Exported permission: manipulate queues.
  $permissions['manipulate queues'] = array(
    'name' => 'manipulate queues',
    'roles' => array(),
    'module' => 'nodequeue',
  );

  // Exported permission: post comments.
  $permissions['post comments'] = array(
    'name' => 'post comments',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: revert revisions.
  $permissions['revert revisions'] = array(
    'name' => 'revert revisions',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: skip comment approval.
  $permissions['skip comment approval'] = array(
    'name' => 'skip comment approval',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: view backend.
  $permissions['view backend'] = array(
    'name' => 'view backend',
    'roles' => array(
      0 => 'authenticated user',
    ),
    'module' => 'uportal_backend',
  );

  // Exported permission: view own unpublished content.
  $permissions['view own unpublished content'] = array(
    'name' => 'view own unpublished content',
    'roles' => array(
      0 => 'Can manage content',
      1 => 'authenticated user',
    ),
    'module' => 'node',
  );

  // Exported permission: view revisions.
  $permissions['view revisions'] = array(
    'name' => 'view revisions',
    'roles' => array(
      0 => 'Can manage content',
    ),
    'module' => 'node',
  );

  // Exported permission: view total download count in nodes.
  $permissions['view total download count in nodes'] = array(
    'name' => 'view total download count in nodes',
    'roles' => array(),
    'module' => 'pubdlcnt',
  );

  return $permissions;
}
