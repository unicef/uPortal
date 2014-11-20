<?php
/**
 * @file
 * uportal_features.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function uportal_features_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}

/**
 * Implements hook_views_api().
 */
function uportal_features_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_image_default_styles().
 */
function uportal_features_image_default_styles() {
  $styles = array();

  // Exported image style: audio_player_wave_form.
  $styles['audio_player_wave_form'] = array(
    'name' => 'audio_player_wave_form',
    'label' => 'Audio Player Wave Form',
    'effects' => array(
      9 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '318',
          'height' => '60',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: content_480_250.
  $styles['content_480_250'] = array(
    'name' => 'content_480_250',
    'label' => 'content_page_480_250',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '480',
          'height' => '250',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: content_image_backdrop.
  $styles['content_image_backdrop'] = array(
    'name' => 'content_image_backdrop',
    'label' => 'Content Image Backdrop',
    'effects' => array(
      2 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '1024',
          'height' => '550',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: content_index.
  $styles['content_index'] = array(
    'name' => 'content_index',
    'label' => 'Content Index',
    'effects' => array(
      10 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '130',
          'height' => '80',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: content_index_series_and_plans.
  $styles['content_index_series_and_plans'] = array(
    'name' => 'content_index_series_and_plans',
    'label' => 'Content Index: Series and Plans',
    'effects' => array(
      11 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '65',
          'height' => '40',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: content_page_playlist.
  $styles['content_page_playlist'] = array(
    'name' => 'content_page_playlist',
    'label' => 'content_page_playlist',
    'effects' => array(
      4 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '130',
          'height' => '80',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: listing_page.
  $styles['listing_page'] = array(
    'name' => 'listing_page',
    'label' => 'Listing Page',
    'effects' => array(
      7 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '220',
          'height' => '130',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: management_content_listing.
  $styles['management_content_listing'] = array(
    'name' => 'management_content_listing',
    'label' => 'Management Content Listing',
    'effects' => array(
      14 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '83',
          'height' => '56',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: management_edit_content_thumbnail.
  $styles['management_edit_content_thumbnail'] = array(
    'name' => 'management_edit_content_thumbnail',
    'label' => 'Management Edit Content Thumbnail',
    'effects' => array(
      13 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '105',
          'height' => '70',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: management_video_player.
  $styles['management_video_player'] = array(
    'name' => 'management_video_player',
    'label' => 'Management Video Player',
    'effects' => array(
      12 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '285',
          'height' => '160',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: nav_icon.
  $styles['nav_icon'] = array(
    'name' => 'nav_icon',
    'label' => 'Nav Icon',
    'effects' => array(
      6 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '16',
          'height' => '16',
          'upscale' => 0,
        ),
        'weight' => '-10',
      ),
    ),
  );

  // Exported image style: on_wikipedia.
  $styles['on_wikipedia'] = array(
    'name' => 'on_wikipedia',
    'label' => 'on_wikipedia',
    'effects' => array(
      5 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '130',
          'height' => '72',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: provider.
  $styles['provider'] = array(
    'name' => 'provider',
    'label' => 'provider',
    'effects' => array(
      2 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '',
          'height' => '35',
          'upscale' => 0,
        ),
        'weight' => '1',
      ),
      3 => array(
        'label' => 'Desaturate',
        'help' => 'Desaturate converts an image to grayscale.',
        'effect callback' => 'image_desaturate_effect',
        'dimensions passthrough' => TRUE,
        'module' => 'image',
        'name' => 'image_desaturate',
        'data' => array(),
        'weight' => '2',
      ),
    ),
  );

  // Exported image style: series_list_item_image.
  $styles['series_list_item_image'] = array(
    'name' => 'series_list_item_image',
    'label' => 'Series List Item Image',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '200',
          'height' => '120',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: topic_icon_listing_pages.
  $styles['topic_icon_listing_pages'] = array(
    'name' => 'topic_icon_listing_pages',
    'label' => 'Listing Page - Topic Icon',
    'effects' => array(
      8 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '',
          'height' => '18',
          'upscale' => 1,
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: video_player_playlist_big.
  $styles['video_player_playlist_big'] = array(
    'name' => 'video_player_playlist_big',
    'label' => 'Video Player Playlist Big',
    'effects' => array(
      3 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '263',
          'height' => '170',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: video_player_playlist_small.
  $styles['video_player_playlist_small'] = array(
    'name' => 'video_player_playlist_small',
    'label' => 'Video Player Playlist Small',
    'effects' => array(
      4 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '120',
          'height' => '70',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: video_player_poster.
  $styles['video_player_poster'] = array(
    'name' => 'video_player_poster',
    'label' => 'Video Player Poster',
    'effects' => array(
      5 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '700',
          'height' => '395',
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function uportal_features_node_info() {
  $items = array(
    'audio' => array(
      'name' => t('Content - Audio'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'content_provider' => array(
      'name' => t('Content Provider'),
      'base' => 'node_content',
      'description' => t('Create a new content provider e.g. Straight Talk, Khan University, etc.'),
      'has_title' => '1',
      'title_label' => t('Name of Provider'),
      'help' => '',
    ),
    'document' => array(
      'name' => t('Content - Text'),
      'base' => 'node_content',
      'description' => t('Create a new document'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'lesson_plan' => array(
      'name' => t('Lesson Plan'),
      'base' => 'node_content',
      'description' => t('Create a new lesson plan.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'page' => array(
      'name' => t('Basic page'),
      'base' => 'node_content',
      'description' => t('Use <em>basic pages</em> for your static content, such as an \'About us\' page.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'series' => array(
      'name' => t('Series'),
      'base' => 'node_content',
      'description' => t('Create a new series.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'skill_level' => array(
      'name' => t('Skill Level'),
      'base' => 'node_content',
      'description' => t('Create a new skill level'),
      'has_title' => '1',
      'title_label' => t('Name of Skill Level'),
      'help' => '',
    ),
    'topic' => array(
      'name' => t('Category'),
      'base' => 'node_content',
      'description' => t('Create a new category.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'video' => array(
      'name' => t('Content - Video'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_video_default_presets().
 */
function uportal_features_video_default_presets() {
  $items = array(
    'uportal-video-preset' => array(
      'name' => 'uportal-video-preset',
      'description' => '',
      'settings' => array(
        'video_extension' => 'mp4',
        'video_codec' => 'h261',
        'video_preset' => '',
        'video_quality' => '4',
        'video_speed' => '3',
        'wxh' => '640x360',
        'video_aspectmode' => 'preserve',
        'video_upscale' => 0,
        'audio_codec' => 'mp2',
        'audio_quality' => '3',
        'deinterlace' => 'detect',
        'max_frame_rate' => '',
        'frame_rate' => '',
        'keyframe_interval' => '',
        'video_bitrate' => '',
        'bitrate_cap' => '',
        'buffer_size' => '',
        'one_pass' => 0,
        'skip_video' => 0,
        'pixel_format' => '',
        'h264_profile' => '',
        'audio_bitrate' => '',
        'audio_channels' => '2',
        'audio_sample_rate' => '',
        'skip_audio' => 0,
        'video_watermark_enabled' => 0,
        'video_watermark_fid' => 0,
        'video_watermark_x' => '5',
        'video_watermark_y' => '5',
        'video_watermark_width' => '',
        'video_watermark_height' => '',
        'video_watermark_origin' => 'content',
        'autolevels' => 0,
        'deblock' => 0,
        'denoise' => '',
        'clip_start' => '',
        'clip_length' => '',
      ),
    ),
  );
  return $items;
}
