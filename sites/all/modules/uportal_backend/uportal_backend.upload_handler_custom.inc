<?php
/*
 * this modifies the original script by Sebastian
 * 
 * jQuery File Upload Plugin PHP Class 7.1.4
 * https://github.com/blueimp/jQuery-File-Upload
 *
 * Copyright 2010, Sebastian Tschan
 * https://blueimp.net
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/MIT
 */

class UploadHandlerModified extends UploadHandler {
  
  protected function handle_file_upload($uploaded_file, $name, $size, $type, $error, $index = null, $content_range = null) {
    $file = parent::handle_file_upload(
      $uploaded_file, $name, $size, $type, $error, $index, $content_range
    );

    if (empty($file->error)) {
      $new_file_name = str_replace(' ', '-', $file->name);
      $new_file_name = str_replace('_', '-', $new_file_name);
      $new_file_name = _uportal_clean_up_string_pathauto_transliterate_tokens($new_file_name);
      rename($this->options['upload_dir'].$file->name, $this->options['upload_dir'].$new_file_name);
      $file->name = $new_file_name;
      
      //sent ids
      $file_ids = explode(',', $_REQUEST['file_ids']);
      $file_names = $_FILES['files']['name'];
      $current_file_name = $name;
      $file_index = array_search($current_file_name, $file_names);
      if ($file_index>=0) {
        $file_id = $file_ids[$file_index];
      }
      
      $file->old_name = $current_file_name;
      $file->id = $file_id;
    }
    
    return $file;
  }

}
