<?php

/**
 * convert into bytes from string
 */
function _uportal_convert_bytes( $value ) {
	if ( is_numeric( $value ) ) {
		return $value;
	} else {
		$value_length = strlen( $value );
		$qty = substr( $value, 0, $value_length - 1 );
		$unit = strtolower( substr( $value, $value_length - 1 ) );
		switch ( $unit ) {
			case 'k':
				$qty *= 1024;
				break;
			case 'm':
				$qty *= 1048576;
				break;
			case 'g':
				$qty *= 1073741824;
				break;
		}
		return $qty;
	}
}
/** ENDS *****************************************/


/**
 * returns content_type given mime type of file
 */
function _uportal_content_type_by_mimetype($mimetype) {
	$field_settings = _uportal_default_filefield_settings();
	foreach ($field_settings as $content_type => $settings) {
		if (in_array($mimetype, $settings['mime-types'])) {
			return $content_type;
		}
	}
	return false;
}
/** ENDS *****************************************/


/**
 * return file_field name given content type
 */
function _uportal_file_field_name_by_content_type($content_type) {
	$field_name = false;
	switch ($content_type) {
		case 'video': $field_name = 'field_video'; break;
		case 'audio': $field_name = 'field_audio_track'; break;
		case 'document': $field_name = 'field_pdf_file'; break;
	}
	return $field_name;
}
/** ENDS *****************************************/


/**
 * create node given file
 */
function _uportal_create_content_given_file($file, $location_dir) {
	global $user;
	$content_type = _uportal_content_type_by_mimetype($file->type);
	$file_field_name = _uportal_file_field_name_by_content_type($content_type);
	
	//attach file to node
	if ($file_field_name) {
		
		$file_field_settings = _uportal_field_settings_given_field_and_type($file_field_name, $content_type);
		
		//prepare node
		$node = new stdClass();
		$node->title = $file->name;
		$node->type = $content_type;
		node_object_prepare($node);
		$node->language = LANGUAGE_NONE;
		$node->uid = $user->uid;
		$node->status = 0; //not published
		$node->promote = 0; //not promoted to front page
		
		//prepare file
		$filepath = drupal_realpath($location_dir.$file->name);
		$file_obj = (object) array (
			'uid' => $user->uid,
			'uri' => $filepath,
			'filemime' => $file->type,
			'status' => 1,
			'display' => 1,
		);
		
		//check for directory
		$new_file_path = _uportal_clean_up_string_pathauto_transliterate_tokens($file_field_settings['folder-path'], array($file_obj));
		$new_location_dir = 'public://'.$new_file_path;
		file_prepare_directory($new_location_dir, FILE_CREATE_DIRECTORY);
		$new_location_dir .= '/';
		
		//check for non-conflicting filename
		$new_file_name = _uportal_get_non_conflicting_filename($file->name, drupal_realpath($new_location_dir));
		$new_file_name = _uportal_clean_up_string_pathauto_transliterate_tokens($new_file_name);
		if ($new_file_name != $file->name) {
			rename($filepath, $location_dir.$new_file_name);
			$file_obj->uri = $location_dir.$new_file_name;
		}
		$file_obj = file_move($file_obj, $new_location_dir);
		$langcode_filefield = field_language('node', $node, $file_field_name);
		$node->{$file_field_name}[$langcode_filefield][0] = (array) $file_obj;
		
		$node = node_submit($node);
		node_save($node);
		
		//generate thumbnails
		if ($node->type=='video') {
			_uportal_create_node_thumbnail_video($node);
		} elseif ($node->type=='document') {
			_uportal_create_node_thumbnail_document($node);
		}
		
		return array (
			'file_id' => $file->js_file_id,
			'nid' => $node->nid,
		);
		
	}
}
/** ENDS *****************************************/


/**
 * update video node thumbnail
 */
function _uportal_create_node_thumbnail_video($node) {
	//check for video thumbnail and use that if available
	$video = $node->field_video['und'][0];
	$video_thumbnail = $video['thumbnailfile'];
	$file_field_settings = _uportal_field_settings_given_field_and_type('field_image', $node->type);
	$folder_path = _uportal_clean_up_string_pathauto_transliterate_tokens($file_field_settings['folder-path']);
	$new_location_uri = 'public://'.$folder_path.'/';
	file_prepare_directory($new_location_uri, FILE_CREATE_DIRECTORY);
	$node_edited = false;
	
	//first check if default video thumbnail ON video field is available
	//if not, create it
	if (!(
		isset($video_thumbnail->fid)
		&& is_numeric($video_thumbnail->fid)
		&& $video_thumbnail->fid>0
	)) {
		//check for default video thumbnail in folder and attach it
		$generatedThumbFID = db_query('SELECT thumbnailfid FROM {video_thumbnails} t WHERE t.videofid = :fid', array(':fid' => $video['fid']))->fetchField();
		
		//check if thumb exists
		if ($generatedThumbFID) {
			$thumbObj = file_load($generatedThumbFID);
			$thumbObj->status = 1;
			file_save($thumbObj);
			file_usage_add($thumbObj, 'file', 'node', $node->nid);
			
			//attach file to node
			$video['thumbnail'] = $generatedThumbFID;
			$video['thumbnailfile'] = $thumbObj;
			$video_thumbnail = $thumbObj;
			$node->field_video['und'][0] = $video;
			
			//node has been edited
			$node_edited = true;
		}
		
	}
	
	//use video thumbnails to update field_image
	if (
		isset($video_thumbnail->fid)
		&& is_numeric($video_thumbnail->fid)
		&& $video_thumbnail->fid>0
	) {
		$new_location_uri .= $video_thumbnail->filename;
		$new_thumbnail_copy = file_copy($video_thumbnail, $new_location_uri, FILE_EXISTS_RENAME);
		if ($new_thumbnail_copy) {
			$new_thumbnail_image = image_load($new_thumbnail_copy->uri);
			$node->field_image['und'][0]['fid'] = $new_thumbnail_copy->fid;
			$node->field_image['und'][0]['height'] = $new_thumbnail_image->info['height'];
			$node->field_image['und'][0]['width'] = $new_thumbnail_image->info['width'];
			
			//node has been edited
			$node_edited = true;
			
			//ensure image styles exist
			$node_form_image_styles = array('management_video_player', 'management_edit_content_thumbnail');
			$source_img_uri = $node->field_image['und'][0]['uri'];
			foreach ($node_form_image_styles as $image_style) {
				$style = image_style_load($image_style);
				$style_path = image_style_path($image_style, $source_img_uri);
				image_style_create_derivative($style, $source_img_uri, $style_path);
			}
		}

	}
	
	if ($node_edited) {
		node_submit($node);
		node_save($node);
	}
	
}
/** ENDS *****************************************/


/**
 * update document node thumbnail
 * - using pdf_to_image module
 * - pdf_to_image_entity_update()
 * - pdf_to_image_generate_process()
 * 
 */
function _uportal_create_node_thumbnail_document($node) {
	module_load_include('module', 'pdf_to_image');
	
	$entity = $node;
	$entity_type = 'node';
	$bundle = 'document';
	
  $pdf_fields = pdf_to_image_source_fields($entity_type, $bundle);
  foreach ($pdf_fields as $field_id => $field_instance) {
		_uportal_pdf_to_image_generate_process($entity_type, $entity, $field_id, $field_instance);
  }
	
}
/** ENDS *****************************************/


/**
 * my adaptation of pdf_to_image_generate_process
 */
function _uportal_pdf_to_image_generate_process($entity_type, $entity, $field_id, $field_instance) {
  $field_lang = field_language($entity_type, $entity, $field_id);
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  $items = $entity->{$field_id};

  if (empty($items[$field_lang][0]['fid'])) {
    // No file attachment found in the source field.
    // Do nothing.
    return;
  }
  $pdf_file = file_load($items[$field_lang][0]['fid']);
  $pdf_realpath = file_stream_wrapper_get_instance_by_uri($pdf_file->uri)->realpath();
  $count = pdf_to_image_count_pages($pdf_realpath);
  if (!empty($field_instance['widget']['settings']['pdf_to_image']['target_field'])) {
    $target_field = $field_instance['widget']['settings']['pdf_to_image']['target_field'];
  }
  // Should I check if the target field is already populated and stop then?

  if ($target_field && $count) {
    // Arguments to give to the batch job.
    $params = array(
      'entity' => $entity, // Don't actually need the whole thing, just the id really.
      'entity_type' => $entity_type,
      'entity_id' => $id,
      'image'  => array(
        'field'    => field_info_field($target_field),
        'instance' => field_info_instance($entity_type, $target_field, $field_instance['bundle']),
      ),
      'pdf'    => array(
        'instance' => $field_instance,
        'file'     => $pdf_file,
      ),
    );

    // Prepare count parameter.
    if ($params['image']['field']['cardinality'] != -1 && $count > $params['image']['field']['cardinality']) {
      $count = $params['image']['field']['cardinality'];
    }
		
		//process image
		$thumbnail_file = pdf_to_image_generate_page($params, 0);
		
		//attach thumbnail to entity
		$field_name = $params['image']['field']['field_name'];
		$entity_id = $params['entity_id'];
		$entity_type = $params['entity_type'];
	
		if (isset($entity_id) && is_numeric($entity_id)) {
			if (is_object($entity)) {
				$field_lang = field_language($entity_type, $entity, $field_name);
				$entity->{$field_name}[$field_lang] = array();
				$entity->{$field_name}[$field_lang][] = (array) $thumbnail_file;
				$saved = pdf_to_image_entity_save($entity_type, $entity);
			}
		}
  }
  else {
    // No target image field set, or invalid count from the PDF.
  }
}
/** ENDS *****************************************/


/**
 * change file name till it does not replace another file in the location
 * returns compatible filename
 * $location_dir should have trailing slash
 */
function _uportal_get_non_conflicting_filename($full_filename, $location_dir) {
	
	//check if file does not exist
	if (!file_exists($location_dir.$full_filename)) {
		return $full_filename;
	}
	
	//work out new filename
	$full_filename_parts = explode('.', $full_filename);
	$file_name = $full_filename_parts[0];
	$file_extension = $full_filename_parts[1];
	$append_counter = 0;
	do {
		$new_file_name = $file_name.'-'.$append_counter.'.'.$file_extension;
		$append_counter++;
	} while (file_exists($location_dir.$new_file_name));
	
	//found new file name
	return $new_file_name;
}
/** ENDS *****************************************/


/**
 * cleans up file name according to transliteration and pathauto settings
 */
function _uportal_clean_up_string_pathauto_transliterate_tokens($value, $data = array()) {
  $transliterate = module_exists('transliteration');
  $pathauto = module_exists('pathauto');
  if ($pathauto == TRUE) {
    module_load_include('inc', 'pathauto');
  }

  // Process string tokens.
  $value = token_replace($value, $data, array('clear' => TRUE));

  $paths = explode('/', $value);
  foreach ($paths as &$path) {

    // Cleanup with pathauto.
    if ($pathauto == TRUE) {
      $path_parts = explode('.', $path);
      foreach ($path_parts as &$path_part) {
        $path_part = pathauto_cleanstring($path_part);
      }
      $path = implode('.', $path_parts);
    }

    // Transliterate string.
    if ($transliterate == TRUE) {
      $path = transliteration_clean_filename($path);
    }
  }
  $value = implode('/', $paths);

  // Ensure that there are no double-slash sequences due to empty token values.
  $value = preg_replace('/\/+/', '/', $value);

  return $value;
}
/** ENDS *****************************************/


/**
 * returns file entity given file info
 */
function _uportal_file_entity_given_file($file, $dir) {
	//see: http://drupal.stackexchange.com/questions/69526/how-to-set-value-of-file-field-with-entity-metadata-wrapper
	global $user;
	$entity_file = new stdClass();
	$entity_file->uid = $user->uid;
	$entity_file->uri = $dir.$file->name;
	$entity_file->filename = $file->name;
	$entity_file->origname = $file->name;
	$entity_file->filemime = $file->type;
	$entity_file->filesize = $file->size;
	$entity_file->status = FILE_STATUS_PERMANENT;
	$entity_file->display = '1';
	file_save($entity_file);
	$entity_file = (array) $entity_file;
	$entity_file_items = array($entity_file);
	
	return $entity_file_items;
}
/** ENDS *****************************************/


/**
 * get field settings and defaults
 */
function _uportal_default_filefield_settings() {
	
	//content types and their defaults
	$types = array ( 'video', 'audio', 'document', );
	$settings = array();
	
	foreach ($types as $type) {
		$settings[$type] = _uportal_field_settings_by_type($type);
	}
	
	return $settings;

}
/** ENDS *****************************************/


/**
 * get file field settings given content type
 */
function _uportal_field_settings_by_type($content_type) {
	$field_name = _uportal_file_field_name_by_content_type($content_type);
	
	if (!$field_name) {
		return false;
	}
	
	return _uportal_field_settings_given_field_and_type($field_name, $content_type);
	
}
/** ENDS *****************************************/


/**
 * return field settings given field and type
 */
function _uportal_field_settings_given_field_and_type($field_name, $content_type) {
	$field = field_info_instance('node', $field_name, $content_type);
	$allowed_extensions = array();
	$allowed_mimetypes = array();
	$max_file_size = _uportal_get_php_max_upload_size();
	
	//get extensions and mime types
	if ($field['settings']['file_extensions']) {
		$allowed_extensions = explode(' ', $field['settings']['file_extensions']);
		foreach ($allowed_extensions as $ext) {
			$mimetypes = _uportal_mimetype_from_extension($ext);
			if (count($mimetypes)) {
				$allowed_mimetypes = array_merge($allowed_mimetypes, $mimetypes);
			}
		}
	}
	
	//folder settings
	$folder_path = $field['settings']['file_directory'];
	if (isset($field['settings']['filefield_paths_enabled']) && $field['settings']['filefield_paths_enabled']) {
		$folder_path = $field['settings']['filefield_paths']['file_path']['value'];
	}
	
	//get max filesize
	if ($field['settings']['max_filesize'])
		$max_file_size = $field['settings']['max_filesize'];
	
	return array (
		'extensions' => $allowed_extensions,
		'mime-types' => $allowed_mimetypes,
		'max-file-size' => $max_file_size,
		'folder-path' => $folder_path,
	);
}
/** ENDS *****************************************/


/**
 * return default max_upload file size
 */
function _uportal_get_php_max_upload_size() {
	$max_upload = (int)(_uportal_convert_bytes(ini_get('upload_max_filesize')));
	$max_post = (int)(_uportal_convert_bytes(ini_get('post_max_size')));
	$memory_limit = (int)(_uportal_convert_bytes(ini_get('memory_limit')));
	$upload_mb = min($max_upload, $max_post, $memory_limit);
	return $upload_mb;
}
/** ENDS *****************************************/


/**
 * format bytes
 * credit: http://stackoverflow.com/questions/2510434/format-bytes-to-kilobytes-megabytes-gigabytes
 */
function _uportal_format_bytes($bytes, $precision = 2) { 
    $units = array('B', 'KB', 'MB', 'GB', 'TB'); 

    $bytes = max($bytes, 0); 
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024)); 
    $pow = min($pow, count($units) - 1); 
		$bytes /= (1 << (10 * $pow)); 

    return round($bytes, $precision) . ' ' . $units[$pow]; 
}
/** ENDS *****************************************/


/**
 * helper function for mime types
 *
 * assuming correct extensions
 */
function _uportal_backend_mime_content_type($filename) {
	$ext = strtolower(array_pop(explode('.',$filename)));
	$mimetype = _uportal_mimetype_from_extension($ext);
	if ($mimetype) {
			return $mimetype;
	}
	elseif (function_exists('finfo_open')) {
			$finfo = finfo_open(FILEINFO_MIME);
			$mimetype = finfo_file($finfo, $filename);
			finfo_close($finfo);
			return $mimetype;
	}
	else {
			return 'application/octet-stream';
	}
}
/** ENDS *****************************************/


/**
 * get mime type from extension
 */
function _uportal_mimetype_from_extension($extension) {
	$extension_mime_types = array();
	$mime_types = array(
		'ogg' => array('audio/ogg'),
		'oga' => array('audio/ogg'),
		'mp4' => array('video/mp4'),
		'ogv' => array('video/ogg'),
		'm4v' => array('video/x-m4v'),
		'swf' => array('application/x-shockwave-flash'),
		'flv' => array('video/x-flv'),
		'mp3' => array('audio/mpeg3', 'audio/mpeg'),
		'qt' => array('video/quicktime'),
		'mov' => array('video/quicktime'),
		'pdf' => array('application/pdf'),
		'jpg' => array('image/jpeg'),
		'jpeg' => array('image/jpeg'),
		'gif' => array('image/gif'),
		'png' => array('image/png'),
	);
	if (array_key_exists($extension, $mime_types)) {
		return $mime_types[$extension];
	}
	return $extension_mime_types;
}
/** ENDS *****************************************/


/**
 * load view, insert data, return string
 */
function _uportal_backend_load_view($view_file_path, $data_array) {
	extract($data_array);
	ob_start();
	include($view_file_path);
	$view_str = ob_get_contents();
	ob_end_clean();
	return $view_str;
}
/** ENDS *****************************************/


/**
 * get managed file by URI
 */
function _uportal_backend_get_managed_file_by_uri($uri) {
  $fid = db_query('SELECT fid FROM {file_managed} WHERE uri = :uri', array(':uri' => $uri))->fetchField();
  if (!empty($fid)) {
    $file_object = file_load($fid);
    return $file_object;
  }
  return FALSE;
}
/** ENDS *****************************************/


/**
 * create managed file given server path
 */
function _uportal_backend_create_managed_file_from_server_path($server_path, $destination_path, $mode=FILE_EXISTS_RENAME) {
	if (!is_file($server_path)) {
		return FALSE;
	}
	
	//get directory
	$path_parts = pathinfo($destination_path);
	file_prepare_directory($path_parts['dirname'], FILE_CREATE_DIRECTORY);
	
	$file_data = file_get_contents($server_path);
  $file_obj  = file_save_data($file_data, $destination_path, $mode);
	return $file_obj;
}
/** ENDS *****************************************/


/**
 * turn human string into machine-safe string
 */
function _uportal_backend_convert_human_string_to_machine($str) {
	$machine_readable = strtolower($str);
	$machine_readable = preg_replace('@[^a-z0-9_]+@', '_', $machine_readable);
	return $machine_readable;
}
/** ENDS *****************************************/

