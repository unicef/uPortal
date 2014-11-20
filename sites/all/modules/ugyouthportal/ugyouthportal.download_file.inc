<?php

/**
 * function to download file
 * - extends pubdlcnt module functionality
 * 
 */
function _ugyouthportal_download_file($nid) {
	
	$node = node_load($nid);
	$url = '';
	$file_mimetype = '';
	$filename = '';
	
	switch ($node->type) {
		case 'video':
			$url = file_create_url($node->field_video['und'][0]['uri']);
			$file_mimetype = $node->field_video['und'][0]['filemime'];
			$filename = $node->field_video['und'][0]['filename'];
			break;
		case 'document':
			$url = file_create_url($node->field_pdf_file['und'][0]['uri']);
			$file_mimetype = $node->field_pdf_file['und'][0]['filemime'];
			$filename = $node->field_pdf_file['und'][0]['filename'];
			break;
		case 'audio':
			$url = file_create_url($node->field_audio_track['und'][0]['uri']);
			$file_mimetype = $node->field_audio_track['und'][0]['filemime'];
			$filename = $node->field_audio_track['und'][0]['filename'];
			break;
	}
	
	if (!preg_match('/^(f|ht)tps?:\/\/.*/i', $url)) { // check if this is absolute URL 
		// if the URL is relative, then convert it to absolute
		$url = "http://" . $_SERVER['SERVER_NAME'] . $url;
	}
	
	/**
	 * check if the url is valid or not and update the counter
	 */
	if (is_valid_file_url($url)) {
		pubdlcnt_update_counter($url, $filename, $nid);
	}
	
	/**
	 * write a better filename
	 */
	$original_filename_parts = explode('.', $filename);
	$file_extension = array_pop($original_filename_parts);
	$download_filename = drupal_html_class($node->title).'.'.$file_extension;
	
	/**
	 * redirect to the original URL of the file
	 */
	
	//header ('Location: ' . $url);
	header ('Content-type: ' . $file_mimetype);
	header ('Content-Disposition: attachment; filename="' . $download_filename . '";');
	//header ('Content-Length: ' . filesize($url));
	//readfile($url);
	
	//readfile doesn't work on classmate
	//solution: http://stackoverflow.com/questions/8031301/php-readfile-not-working-for-me-and-i-dont-know-why
	$handle = fopen($url, 'rb'); 
  $buffer = ''; 
  while (!feof($handle)) { 
    $buffer = fread($handle, 4096); 
    echo $buffer; 
    ob_flush(); 
    flush(); 
  } 
  fclose($handle);
	
	exit;
}
/** ENDS *****************************************/


/**
 * Function to check if the specified file URL is valid or not
 */
function is_valid_file_url($url) {
  // replace space characters in the URL with '%20' to support file name
  // with space characters
  $url = preg_replace('/\s/', '%20', $url);

  if (!valid_url($url, true)) {
    return false;
  }
  // URL end with slach (/) and no file name
  if (preg_match('/\/$/', $url)) {
    return false;
  }
  // in case of FTP, we just return TRUE (the file exists)
  if (preg_match('/ftps?:\/\/.*/i', $url)) {
    return true;
  }

  // extract file name and extention
  $filename = basename($url);
  $extension = explode(".", $filename);
  // file name does not have extension
  if (($num = count($extension)) <= 1) {
    return false;
  }
  $ext = $extension[$num - 1];

  // get valid extensions settings from Drupal
  $result = db_query("SELECT value FROM {variable} 
                      WHERE name = :name", array(':name' => 'pubdlcnt_valid_extensions'))->fetchField();
  $valid_extensions = unserialize($result);
  if (!empty($valid_extensions)) {
    // check if the extension is a valid extension or not (case insensitive)
    $s_valid_extensions = strtolower($valid_extensions);
    $s_ext = strtolower($ext);
    $s_valid_ext_array = explode(" ", $s_valid_extensions);
    if (!in_array($s_ext, $s_valid_ext_array)) {
      return false;
    }
  }
  
  if (!url_exists($url)) {
    return false;
  }
  return true; // it seems that the file URL is valid
}

/**
 * Function to check if the specified file URL really exists or not
 */
function url_exists($url) {
  $a_url = parse_url($url);
  if (!isset($a_url['port'])) $a_url['port'] = 80;
  $errno = 0;
  $errstr = '';
  $timeout = 30;
  if (isset($a_url['host']) && $a_url['host'] != gethostbyname($a_url['host'])) {
    $fid = @fsockopen($a_url['host'], $a_url['port'], $errno, $errstr, $timeout);
    if (!$fid) return false;
    $page = isset($a_url['path']) ? $a_url['path'] : '';
    $page .= isset($a_url['query']) ? '?' . $a_url['query'] : '';
    fputs($fid, 'HEAD ' . $page . ' HTTP/1.0' . "\r\n" . 'HOST: ' 
        . $a_url['host'] . "\r\n\r\n");
    $head = fread($fid, 4096);
    $head = substr($head, 0, strpos($head, 'Connection: close'));
    fclose($fid);
    // Here are popular status code back from the server
    //
    // URL exits                  'HTTP/1.1 200 OK'
    // URL exits (but redirected) 'HTTP/1.1 302 Found'
    // URL does not exits         'HTTP/1.1 404 Not Found'
    // Can not access URL         'HTTP/1.1 403 Forbidden'
    // Can not access server      'HTTP/1.1 500 Internal Server Error
    // 
    // So we return true only when status 200 or 302
    if (preg_match('#^HTTP/.*\s+[200|302]+\s#i', $head)) {
      return true;
    }
  }
  return false;
}

/**
 * Function to check duplicate download from the same IP address within a day
 * @return   0 - OK,  1 - duplicate (skip counting)
 */
function pubdlcnt_check_duplicate($url, $name, $nid) {
  // get the settings
  $result = db_query("SELECT value FROM {variable} 
						WHERE name = :name", array(':name' => 'pubdlcnt_skip_duplicate'))->fetchField();
  $skip_duplicate = unserialize($result);
  if(!$skip_duplicate) return 0; // OK

  // OK, we should check the duplicate download
  $ip = $_SERVER['REMOTE_ADDR'];
  if (!preg_match("/^(([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/", $ip)) {
    return 1; // NG (Invalid IPv4 IP-addresss)
  }
  $today = mktime(0, 0, 0, date("m"), date("d"), date("Y")); // Unix timestamp

  // obtain fid 
  $fid = db_query("SELECT fid FROM {pubdlcnt} WHERE name=:name", array(':name' => $name))->fetchField();
  if ($fid) {
    $result = db_query("SELECT * FROM {pubdlcnt_ip} WHERE fid=:fid AND ip=:ip AND utime=:utime", array(':fid' => $fid, ':ip' => $ip, ':utime' => $today));
    if ($result->rowCount()) {
      return 1; // found duplicate!
    }
    else {
      // add IP address to the database
      db_insert('pubdlcnt_ip')
        ->fields(array(
          'fid' => $fid,
          'ip' => $ip,
          'utime' => $today))
        ->execute();
    }
  }
  else {
    // no file record -> create file record first
    $fid = db_insert('pubdlcnt')
      ->fields(array(
        'nid' => $nid,
        'name' => $name,
        'url' => $url,
        'count' => 1,
        'utime' => $today))
      ->execute();
    // next, add IP address to the database
    db_insert('pubdlcnt_ip')
      ->fields(array(
        'fid' => $fid,
        'ip' => $ip,
        'utime' => $today))
      ->execute();
  }
  return 0;
}

/**
 * Function to update the data base with new counter value
 */
function pubdlcnt_update_counter($url, $name, $nid) {
  $count = 1;
  $name = addslashes($name);	// security purpose

  if (empty($nid)) { // node nid is invalid
    return;
  }

  // check the duplicate download from the same IP and skip updating counter
  if (pubdlcnt_check_duplicate($url, $name, $nid)) {
    return;
  }

  // today(00:00:00AM) in Unix time
  $today = mktime(0, 0, 0, date("m"), date("d"), date("Y"));

  // obtain fid 
  $result = db_query("SELECT fid, count FROM {pubdlcnt} WHERE name=:name", array(':name' => $name));
  if (!$result->rowCount()) {
    // no file record -> create file record first
    $fid = db_insert('pubdlcnt')
      ->fields(array(
        'nid'   => $nid,
        'name'  => $name,
        'url'   => $url,
        'count' => 1,
        'utime' => $today))
      ->execute();
  }
  else {
    $rec = $result->fetchObject();
    $fid = $rec->fid;
    // update total counter
    $total_count = $rec->count + 1;
    db_update('pubdlcnt')
      ->fields(array(
        'nid'   => $nid,
        'url'   => $url,
        'count' => $total_count,
        'utime' => $today))
      ->condition('fid', $rec->fid)
      ->execute();
  }

  // get the settings
  $result = db_query("SELECT value FROM {variable} WHERE name=:name", 
                      array(':name' => 'pubdlcnt_save_history'))->fetchField();
  $save_history = unserialize($result);

  if ($save_history) {
    $count = db_query("SELECT count FROM {pubdlcnt_history} WHERE fid=:fid AND utime=:utime", 
                     array(':fid' => $fid, ':utime' => $today))->fetchField();
    if ($count) {
      $count++;
      // update an existing record
      db_update('pubdlcnt_history')
        ->fields(array('count' => $count))
        ->condition('fid', $fid)
        ->condition('utime', $today)
        ->execute();
    }
    else {
      // insert a new record
      db_insert('pubdlcnt_history')
        ->fields(array(
          'fid' => $fid,
          'utime' => $today,
          'count' => 1))
        ->execute();
    }
  }
}
