
(function($) {


/**
 * prepare content file uploader
 */
Drupal.behaviors.uportal.editContent.initiateContentFileUploader = function($nodeFormWrapper, player) {
	var $contentFileWrapper = $('.content-file-wrapper', $nodeFormWrapper);
	var $uploaderBtn = $('.replace a.replace-file', $contentFileWrapper);
	
	//click to upload
	$uploaderBtn.click( function() {
		if ($uploaderBtn.hasClass('disabled')) {
			return false;
		}
		Drupal.behaviors.uportal.editContent.prepareFileUploader($nodeFormWrapper, 'content', player);
		$('.temporary-data .content-file-info .new-content-file', $nodeFormWrapper).click();
		return false;
	});
	
};
/** ENDS *****************************************/


/**
 * prepare thumbnail uploader
 */
Drupal.behaviors.uportal.editContent.initiateThumbnailUploader = function($nodeFormWrapper) {
	var $thumbnailWrapper = $('.thumbnail .thumbnail-images', $nodeFormWrapper);
	var $uploaderBtn = $('.upload-thumbnail', $thumbnailWrapper);
	
	//click to upload
	$uploaderBtn.click( function() {
		if ($uploaderBtn.hasClass('disabled')) {
			return false;
		}
		Drupal.behaviors.uportal.editContent.prepareFileUploader($nodeFormWrapper, 'thumbnail');
		$('.temporary-data .thumbnail-info .new-thumbnail', $nodeFormWrapper).click();
		return false;
	});
	
};
/** ENDS *****************************************/


/**
 * prepare thumbnail field
 */
Drupal.behaviors.uportal.editContent.prepareFileUploader = function($nodeFormWrapper, fileType, player) {
	
	var $fileInput = $('.temporary-data .thumbnail-info .new-thumbnail', $nodeFormWrapper);
	var $uploaderBtn = $('.thumbnail .thumbnail-images .upload-thumbnail', $nodeFormWrapper);
	
	if (fileType=='content') {
		$fileInput = $('.temporary-data .content-file-info .new-content-file', $nodeFormWrapper);
		$uploaderBtn = $('.content-file-wrapper .replace a.replace-file', $nodeFormWrapper);
	}
	
	$fileInput.fileupload({
		url: Drupal.uportal.editContent.serverURL,
		dataType: 'json',
		autoUpload: true,
		sequentialUploads: false,
		forceIframeTransport: false,
		change: function(e, data) {
		},
		add: function(e, data) {
			$uploaderBtn.addClass('disabled');
			Drupal.behaviors.uportal.editContent.newFileUploading(data, $nodeFormWrapper, fileType, player);
		},
		submit: function(e, data) {
			var files = data.files;
			var file_ids = new Array();
			$.each(files, function (index, file) {
				file_ids.push(file.id);
			});
			data.formData = {
				'file_ids' : file_ids,
				'operation_type' : 'upload_file',
				'file_type' : fileType,
				'content_type' : $nodeFormWrapper.data('node-type')
			};
		},
		send: function (e, data) {
			Drupal.behaviors.uportal.editContent.startedUploadingFile(data);
		},
		done: function (e, data) { //check for errors
			Drupal.behaviors.uportal.editContent.anUploadIsDone(data);
		},
		fail: function (e, data) { //failed ajax
		},
		progress: function (e, data) {
			Drupal.behaviors.uportal.editContent.updateUploadProgress(data);
		}
	});
}
/** ENDS *****************************************/


/**
 * update upload progress
 */
Drupal.behaviors.uportal.editContent.updateUploadProgress = function (data) {
	var progress = parseInt(data.loaded / data.total * 100, 10);
	var $fileDiv = Drupal.uportal.editContent.files[data.files[0].id].$fileDiv;
	var $barDiv = $('div.progress-bar div', $fileDiv);
	$barDiv.css('width', progress+'%');
}
/** ENDS *****************************************/


/**
 * add file to upload queue
 */
Drupal.behaviors.uportal.editContent.newFileUploading = function (data, $nodeFormWrapper, fileType, player) {
	var files = data.files;
	var nodeType = $nodeFormWrapper.data('node-type');
	var checkFile = {
		fileIsOK: false
	};
	
	$.each(files, function (index, file) {
		if (fileType=='thumbnail') {
			var customFileData = $('input.new-thumbnail', $nodeFormWrapper).data('file-upload-settings');
			checkFile = Drupal.behaviors.uportal.bulkUploader.fileIsOKForUpload(file, fileType, customFileData);
		} else {
			checkFile = Drupal.behaviors.uportal.bulkUploader.fileIsOKForUpload(file, nodeType);
		}
		file.id = 'id-'+Drupal.uportal.editContent.fileIDCounter++;
		Drupal.behaviors.uportal.editContent.fileObject(file, $nodeFormWrapper, fileType, player);
		
		//once added, show progress div, disable uploader button
		//if file is invalid, cancel upload
		var fileObj = Drupal.uportal.editContent.files[file.id];
		if (checkFile.fileIsOK) {
			Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-added', file.id);
			fileObj.$fileDiv.fadeIn(400);
			fileObj.$uploaderBtn.addClass('disabled');
			$('.progress-bar div', fileObj.$fileDiv).css('width', '0');
		} else {
			Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-cancelled', file.id, checkFile.errorObj);
		}
		
	});
	
	if (checkFile.fileIsOK) {
		data.process().done(function () {
			var jqXHR = data.submit();
			jqXHR.fail(function( jqXHR, textStatus, errorThrown ) {
				//again, we assume this.files will always contain ONE file hence [0]
				var file = this.files[0];
				var fileObj = Drupal.uportal.bulkUploader.files[file.id];
				var errorObj = {
					'errorType': 'ajax error'
				};
				Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-cancelled', file.id, errorObj);
			});
			$.each(data.files, function(index, fileObj) {
				Drupal.uportal.editContent.files[fileObj.id]['jqXHR'] = jqXHR;
			});
		});
	}
	
}
/** ENDS *****************************************/


/**
* file class to create file objects with sufficient methods to handle their functionality
*
* - fileObject constructor creates object and adds it to global files array
* - also creates DIV to represent file
* - fileType can be content or thumbnail
*/
Drupal.behaviors.uportal.editContent.fileObject = function (file, $nodeFormWrapper, fileType, player) {
	var $fileDiv = $('.thumbnail-upload-progress', $nodeFormWrapper);
	var nid = $nodeFormWrapper.data('nid');
	var parentType = $nodeFormWrapper.data('node-type');
	var $cancelBtn = $('div#cancel-btn', $fileDiv).click( function(e) {
		//Drupal.behaviors.uportal.bulkUploader.cancelUpload(new Array(file.id));
	});
	var $fidInput = $('.temporary-data input.new-thumbnail-fid', $nodeFormWrapper);
	var $currentDisplay = $('.current-thumbnail .img', $nodeFormWrapper);
	var $uploaderBtn = $('.thumbnail-images .upload-thumbnail', $nodeFormWrapper);
	
	if (fileType=='content') {
		$fidInput = $('.temporary-data input.new-content-file-fid', $nodeFormWrapper);
		$fileDiv = $('.content-upload-progress', $nodeFormWrapper);
		$currentDisplay = $('.content-file-wrapper .player', $nodeFormWrapper);
		$uploaderBtn = $('.content-file-wrapper .replace a.replace-file', $nodeFormWrapper);
	}
	
	Drupal.uportal.editContent.files[file.id] = {
		file_id : file.id,
		fid : 0,
		file_name : file.name,
		type : fileType, //thumbnail or content
		parent_type : parentType, //audio, video, document
		new_name : '',
		path : '',
		path_uri : '',
		$fileDiv : $fileDiv,
		$currentDisplay : $currentDisplay,
		nid : $nodeFormWrapper.data('nid'),
		$fidInput: $fidInput,
		status : 'waiting',
		$cancelBtn : $cancelBtn,
		$uploaderBtn : $uploaderBtn,
		player : player
	};
}
/** ENDS *****************************************/


/**
* this function shows the upload divs and shows the file information
*/
Drupal.behaviors.uportal.editContent.startedUploadingFile = function (data) {
	$.each(data.files, function (index, file) {
		var fileObj = Drupal.uportal.editContent.files[file.id];
		fileObj.status = 'uploading';
		Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-uploading', file.id);
	});
};
/** ENDS *****************************************/


/*** function to add class to file div ***************/
Drupal.behaviors.uportal.editContent.updateFileDivClass = function(newClassName, file_id, errorObj) {
	var fileObj = Drupal.uportal.editContent.files[file_id];
	var $fileDiv = fileObj.$fileDiv;
	$fileDiv.removeClass( function (index, css) {
		return (css.match(/\bupload-\S+/g) || []).join(' ');
	})
	if (newClassName=='upload-cancelled') {
		var msg = '';
		switch (errorObj.errorType) {
			case 'invalid file size':
				msg = 'Invalid file size.';
				break;
			case 'invalid file type':
				msg = 'Invalid file type.';
				break;
			case 'ajax error':
				msg = 'Connection error.';
				break;
		}
		$('.error-msg', $fileDiv).text(msg);
		$fileDiv.fadeIn(400);
		fileObj.$uploaderBtn.addClass('disabled');
		fileObj.status = 'cancelled';
		setTimeout(function(){
			$fileDiv.fadeOut(500);
			fileObj.$uploaderBtn.removeClass('disabled');
		}, 1500);
	}
	$fileDiv.addClass(newClassName);
}
/** ENDS *****************************************/


/**
 * function to receive upload finished feedback
 */
Drupal.behaviors.uportal.editContent.anUploadIsDone = function (data) {
	$.each(data.result.files, function (index, file) {
		var fileObj = Drupal.uportal.editContent.files[file.id];
		fileObj.new_name = file.name;
		fileObj.path = file.path;
		fileObj.path_uri = file.path_uri;
		fileObj.status = 'creating-obj';
		Drupal.uportal.editContent.drupalFileObjsToCreate.push(fileObj);
		Drupal.behaviors.uportal.editContent.createDrupalFileObjs();
	});
}
/** ENDS *****************************************/


/**
 * create drupal file objects from uploaded files
 */
Drupal.behaviors.uportal.editContent.createDrupalFileObjs = function() {
	if (Drupal.uportal.editContent.drupalFileObjsToCreate.length>0) {
		var fileObj = Drupal.uportal.editContent.drupalFileObjsToCreate.pop();
		$.post(Drupal.uportal.editContent.serverURL, {
			'operation_type' : 'create_file_object',
			'data_values' : {
				'file_id': fileObj.file_id,
				'file_name': fileObj.new_name,
				'file_path': fileObj.path,
				'file_path_uri': fileObj.path_uri,
				'file_type': fileObj.type,
				'node_type': fileObj.parent_type
			}
		})
			.done( function(data) {
				var fileObj = Drupal.uportal.editContent.files[data.file_id];
				fileObj.fid = data.fid;
				fileObj.$fidInput.val(data.fid);
				fileObj.$fidInput.closest('form').trigger('checkform.areYouSure');
				Drupal.behaviors.uportal.editContent.updateDisplayedFile(fileObj, data);
			})
			.fail( function (data) {
				var decodedData = decodeURIComponent(this.data);
				var decodedDataParts = decodedData.split('&');
				var file_ids = [];
				
				$.each(decodedDataParts, function(index, decodedDataPart) {
					var valuePair = decodedDataPart.split('=');
					if (valuePair[0] == 'data_values[file_id]') {
						file_ids.push(valuePair[1]);
					}
				})
				
				$.each(file_ids, function(index, file_id) {
					var errorObj = {
						'errorType': 'ajax error'
					};
					Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-cancelled', file_id, errorObj);
				})
			});
		Drupal.behaviors.uportal.editContent.createDrupalFileObjs();
	}
}
/** ENDS *****************************************/


/**
 * update displayed file with temporary file
 */
Drupal.behaviors.uportal.editContent.updateDisplayedFile = function(fileObj, data) {
	var filePath = '/'+fileObj.path+fileObj.new_name;
	
	//for thumbnail
	if (fileObj.type=='thumbnail') {
		fileObj.$currentDisplay.html(data.img_html);
		$('img', fileObj.$currentDisplay).load( function() {
			Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-finished', data.file_id);
			fileObj.status = 'finished';
			setTimeout(function(){
				fileObj.$fileDiv.fadeOut(500);
				fileObj.$uploaderBtn.removeClass('disabled');
			}, 1500);
		});
	}
	
	//for content
	if (fileObj.type=='content') {
		var $currentDisplay = fileObj.$currentDisplay;
		
		//update details
		var $playerWrapper = $('div#node-form-wrapper-nid-'+fileObj.nid+' .node-edit-col-1 .content-file-wrapper');
		$('.filename', $playerWrapper).text(data.filename);
		$('.filesize', $playerWrapper).text(data.filesize);
		
		//for videos
		if (fileObj.parent_type=='video') {
			if (typeof fileObj.player !== "undefined") {
				//update player
				fileObj.player.setSrc(data.url);
				fileObj.player.load();
			}
		}
		
		//for audio
		if (fileObj.parent_type=='audio') {
			if (typeof fileObj.player !== "undefined" && fileObj.player) {
				$('.audio-player .total-time', $playerWrapper).text(data.time_str);
				fileObj.player.jPlayer("destroy");
				fileObj.player = Drupal.behaviors.uportal.editContent.initiateAudioPlayer(fileObj.player, data.url, $playerWrapper);
			}
		}
		
		//hide file uploader progress
		//re-enable button
		Drupal.behaviors.uportal.editContent.updateFileDivClass('upload-finished', data.file_id);
		fileObj.status = 'finished';
		fileObj.$fileDiv.fadeOut(500);
		fileObj.$uploaderBtn.removeClass('disabled');
	}
}
/** ENDS *****************************************/


}) (jQuery);

