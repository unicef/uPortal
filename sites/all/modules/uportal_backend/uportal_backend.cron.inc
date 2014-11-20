<?php

/**
 * clear temporary uploaded files
 */
function _uportal_backend_clear_temporary_files() {
	$directory = variable_get('file_public_path', conf_path() . '/files').'/bulk-uploader';
	$files = array_diff(scandir($directory), array('..', '.'));
	$oldest_time = strtotime('-1 day'); //a day
	foreach ($files as $filename) {
		$filepath = $directory.'/'.$filename;
		$file_created = filemtime($filepath);
		if ($file_created<$oldest_time) {
			unlink($filepath);
		}
	}
}
/** ENDS *****************************************/
