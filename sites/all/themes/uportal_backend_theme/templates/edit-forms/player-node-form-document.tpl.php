<?php

//add NID attribute to button
$nid = $form['nid']['#value'];
$node = $form['#entity'];
$document_file_obj = $form['field_pdf_file']['und'][0]['#file'];

$document_path = file_create_url($document_file_obj->uri);
$document_path_parts = explode('/', $document_path);
$document_filename = $document_path_parts[count($document_path_parts)-1];
$document_filesize = _uportal_format_bytes($document_file_obj->filesize, 0);

//use current thumbnail
$field_image = $form['field_image']['und'][0]['#value'];
if ($field_image['fid']) {
	$use_img_uri = $field_image['uri'];
	$use_img = '';
	if ($use_img_uri) {
		$use_img = theme_image_style(array(
			'style_name' => 'management_video_player',
			'path' => $use_img_uri,
		));
	}
}

?>

<div id="document-file-wrapper-<?php print $nid ?>"
	class="document-file preview-file content-file-wrapper" >
	
	<label>Document Preview</label>
	
	<div id="document-player-<?php print $nid ?>" class="document-player player">
		
		<!-- player ------------>
		<div id="document-thumbnail-<?php print $nid ?>"
			class="document-thumbnail"
			data-document-url="<?php print $document_path ?>" >
			<div class="img"><?php print $use_img ?></div>
			<div class="icon"></div>
			<div class="pdf-display-file">
				<iframe webkitallowfullscreen="" mozallowfullscreen="" allowfullscreen="" frameborder="no" height="100%" width="100%" src="/sites/all/libraries/pdf.js/web/viewer.html?file=http://uportal.dev/sites/ugyouthportal/files/documents/2013/11-november/7ec903c1.pdf"></iframe>
				<div class="unblock-ui close-btn"></div>
			</div>
		</div>
		
		<!-- loader ------------>
		<div class="content-upload-progress wrapper-upload-progress document-upload-progress">
			<div class="loader-indicator"></div>
			<div class="progress-bar"><div></div></div>
			<div class="error-msg"></div>
		</div>
		
	</div>
	
	<div class="filename"><?php print $document_filename ?></div>
	<div class="filesize-replace clearfix">
		<div class="filesize"><?php print $document_filesize ?></div>
		<div class="replace"><a href="#" class="replace-file">Replace</a> this file.</div>
	</div>
	
	<div class="original-document-form-field original-content-file-field">
		<?php print drupal_render($form['field_pdf_file']); ?>
	</div>
		
</div>

