(function($) {

/**
 * file status'es can be: waiting, uploading, finished, node-created
	
/**
* global variables I need
*/
Drupal.uportal.bulkUploader = {
	fileIDCounter : 0,
	noFilesToUpload : 0,
	noFilesUploaded : 0,
	totalProgress : 'none',
	files : new Object(),
	$progressDiv: {},
	$uploaderDiv: {},
	pageType: '',
	$filesInputField: {},
	$headerUploadBtn: false,
	$allContentEditBtn: false,
	$allContentCancelBtn: false,
	allUploadsCancelled: false,
	$initiatingUploadBtn: {},
	nodesToCreate: new Array(),
	creatingNodeOperationActive: false, //use to avoid database conflicts, write nodes one by one
	uploaderDivShowing: false, //once shown first time, set to true
	fileExtensions: {},
};

/**
 * initiate upload functionality
 */
Drupal.behaviors.uportal.bulkUploader.initiateUploadFunctionality = function() {

	//check if page has upload functionality
	if (!($('body').hasClass('upload-functionality'))) {
		return;
	}
	
	$('body').once( function() {
		
		var serverURL = '/management/bulk-uploader';
		var $filesField = $('#fileupload');
		
		//get allowed extensions and file sizes
		var uploadData = $('div#uportal-bulk-uploader div#uploader-progress').data('file-upload-settings');
		$.each(uploadData, function(typeName, typeDetails) {
			$.each(typeDetails.extensions, function(index, ext) {
				Drupal.uportal.bulkUploader.fileExtensions[ext] = {
					'type': typeName,
					'max_file_size': typeDetails.max_size,
				};
			})
		})
		
		//ensure visibility of filesField to enable click event.
		//put filesField in a visible element
		$filesField.appendTo('header.page-header');
		
		Drupal.uportal.bulkUploader.$filesInputField = $filesField;
		Drupal.uportal.bulkUploader.$uploaderDiv = $('div#uportal-bulk-uploader');
		Drupal.uportal.bulkUploader.$progressDiv = $('div#uploader-progress');
		
		//edit content button
		Drupal.behaviors.uportal.bulkUploader.activateEditContentBtn();
		
		//cancel uploads button
		Drupal.behaviors.uportal.bulkUploader.activateCancelAllBtn();
		
		//initiate buttons
		Drupal.behaviors.uportal.bulkUploader.initiateUploadButton();
		
		//set up multiple upload
		$filesField.fileupload({
			url: serverURL,
			dataType: 'json',
			autoUpload: true,
			sequentialUploads: false,
			forceIframeTransport: false,
			change: function(e, data) {
				Drupal.behaviors.uportal.bulkUploader.showStartedBulkUpload(data.files);
			},
			add: function(e, data) {
				Drupal.behaviors.uportal.bulkUploader.filesAdded(data);
				Drupal.uportal.bulkUploader.$filesInputField = $(this);
			},
			submit: function(e, data) {
				var files = data.files;
				var file_ids = new Array();
				$.each(files, function (index, file) {
					file_ids.push(file.id);
				});
				data.formData = {
					'file_ids' : file_ids
				};
				Drupal.uportal.bulkUploader.$filesInputField = $(this);
			},
			send: function (e, data) {
				Drupal.behaviors.uportal.bulkUploader.startedUploadingFile(data);
				Drupal.uportal.bulkUploader.$filesInputField = $(this);
			},
			done: function (e, data) { //check for errors
				Drupal.behaviors.uportal.bulkUploader.anUploadIsDone(data);
				Drupal.uportal.bulkUploader.$filesInputField = $(this);
			},
			fail: function (e, data) { //failed ajax
				var x = 1;
			},
			progress: function (e, data) {
				Drupal.behaviors.uportal.bulkUploader.updateUploadProgress(data);
				Drupal.uportal.bulkUploader.$filesInputField = $(this);
			}
		});
		
	});
	
}
/** ENDS *****************************************/


/**
 * check if file is ok for upload
 */
Drupal.behaviors.uportal.bulkUploader.fileIsOKForUpload = function(file, fileType, customAllowedExtensions) {
	
	//check if file is correct format and correct file size
	var fileIsOK = true;
	var allowedExtensions = false;
	var fileExtension = file.name.split('.').pop();
	var errorObj = {
		'errorType': '',
		'fileSize': ''
	};
	
	if (typeof fileType === "undefined" || fileType == 'all') {
		allowedExtensions = Drupal.uportal.bulkUploader.fileExtensions;
	} else {
		if (
			typeof customAllowedExtensions !== "undefined"
			&& customAllowedExtensions
		) {
			allowedExtensions = customAllowedExtensions;
		} else {
			allowedExtensions = {};
			$.each(Drupal.uportal.bulkUploader.fileExtensions, function(ext, extInfo) {
				if (extInfo.type == fileType) {
					allowedExtensions[ext] = extInfo;
				}
			})
		}
	}
	
	if (!allowedExtensions || typeof allowedExtensions[fileExtension] === 'undefined') {
		fileIsOK = false;
		errorObj.errorType = 'invalid file type';
	} else {
		fileIsOK = (file.size < allowedExtensions[fileExtension].max_file_size);
		if (!fileIsOK) {
			errorObj.errorType = 'invalid file size';
			errorObj.fileSize = file.size;
			errorObj.maxFileSize = allowedExtensions[fileExtension].max_file_size;
		}
	}
	
	return {
		'fileIsOK': fileIsOK,
		'errorObj': errorObj,
	};
	
};
/** ENDS *****************************************/


/**
 * activate edit content button
 */
Drupal.behaviors.uportal.bulkUploader.activateEditContentBtn = function() {
	Drupal.uportal.bulkUploader.$allContentEditBtn = $('div.edit-content-button', Drupal.uportal.bulkUploader.$uploaderDiv);
	Drupal.uportal.bulkUploader.$allContentEditBtn.click( function (e) {
		var $btn = $(this);
		if (!($btn.hasClass('disabled'))) {
			var fileObjs = Drupal.uportal.bulkUploader.files;
			var nids = new Array();
			$.each(fileObjs, function(index, fileObj) {
				if (fileObj.status=='node-created') {
					nids.push(fileObj.nid);
				}
			});
			var nidsStr = nids.join('+');
			window.location.href = window.location.protocol
				+ '//'
				+ window.location.host
				+ '/management/edit-content/'
				+ encodeURIComponent(nidsStr)
				+ '?from-url='+encodeURIComponent('/management/dashboard');
		}
	});
}
/** ENDS *****************************************/


/**
 * activate cancel all button
 */
Drupal.behaviors.uportal.bulkUploader.activateCancelAllBtn = function() {
	Drupal.uportal.bulkUploader.$allContentCancelBtn = $('div.cancel-button', Drupal.uportal.bulkUploader.$uploaderDiv);
	Drupal.uportal.bulkUploader.$allContentCancelBtn.click( function (e) {
		var $btn = $(this);
		if (!($btn.hasClass('disabled'))) {
			
			Drupal.uportal.bulkUploader.allUploadsCancelled = true;
			var fileObjs = Drupal.uportal.bulkUploader.files;
			var fileIDs = new Array();
			$.each(fileObjs, function(index, fileObj) {
				if (fileObj.status=='node-created' || fileObj.status=='finished' || fileObj.status=='uploading') {
					fileIDs.push(fileObj.file_id);
				}
			});
			Drupal.behaviors.uportal.bulkUploader.cancelUpload(fileIDs);
			
			//if nothing is uploading
			if (fileIDs.length<=0) {
				location.reload();
			}
			
		}
	});
}
/** ENDS *****************************************/


/**
 * this function shows the upload divs and file information
 *
 * fired when files are added for upload
 */
Drupal.behaviors.uportal.bulkUploader.filesAdded = function (data) {
	var files = data.files;
	
	$.each(files, function (index, file) {
		file.id = 'id-'+Drupal.uportal.bulkUploader.fileIDCounter++;
		Drupal.uportal.bulkUploader.noFilesToUpload++;
		Drupal.behaviors.uportal.bulkUploader.fileObject(file);
	});
	
	//update title
	var $titleBar = $('div.title-bar', Drupal.uportal.bulkUploader.$uploaderDiv);
	$titleBar.removeClass('bulk-upload-finished');
	$titleBar.removeClass('bulk-upload-node-created');
	$titleBar.text('Uploading files ...');
	
	//update header bar
	if (!$('body').hasClass('bulk-upload-active')) {
		$('body').addClass('bulk-upload-active');
		$('header.page-header h1').text('Upload');
		Drupal.behaviors.uportal.bulkUploader.showUploaderDiv();
	}
	
	//check if file is correct format and correct file size
	//files is an array, but it always contains ONE file, so just pick first file.
	//TODO: test further to see if this assumption is wrong
	var file = files[0];
	var fileObj = Drupal.uportal.bulkUploader.files[file.id];
	var checkFile = Drupal.behaviors.uportal.bulkUploader.fileIsOKForUpload(file);
	
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
				Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'cancelled', errorObj);
			});
			$.each(data.files, function(index, fileObj) {
				Drupal.uportal.bulkUploader.files[fileObj.id]['jqXHR'] = jqXHR;
			});
		});
	} else {
		Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'cancelled', checkFile.errorObj);
	}
	
}
/** ENDS *****************************************/


/**
* this function shows the upload divs and shows the file information
*/
Drupal.behaviors.uportal.bulkUploader.showStartedBulkUpload = function (files) {
	Drupal.uportal.bulkUploader.totalProgress = 'uploading';
};
/** ENDS *****************************************/


/**
* this function shows the upload divs and shows the file information
*/
Drupal.behaviors.uportal.bulkUploader.startedUploadingFile = function (data) {
	$.each(data.files, function (index, file) {
		var fileObj = Drupal.uportal.bulkUploader.files[file.id];
		Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'uploading');
	});
	Drupal.uportal.bulkUploader.totalProgress = 'uploading';
};
/** ENDS *****************************************/


/**
* file class to create file objects with sufficient methods to handle their functionality
*
* - fileObject constructor creates object and adds it to global files array
* - also creates DIV to represent file
*/
Drupal.behaviors.uportal.bulkUploader.fileObject = function (file) {
	var $createdFileDiv = Drupal.behaviors.uportal.bulkUploader.createFileDiv(file);
	var $createdCancelBtn = $('div#cancel-btn', $createdFileDiv).click( function(e) {
		Drupal.behaviors.uportal.bulkUploader.cancelUpload(new Array(file.id));
	});
	var fileObj = {
		file_id : file.id,
		file_name : file.name,
		new_name : '',
		$fileDiv : $createdFileDiv,
		nid : 0,
		status : 'waiting',
		$cancelBtn : $createdCancelBtn
	};
	Drupal.uportal.bulkUploader.files[file.id] = fileObj;
	Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'waiting');
}
/** ENDS *****************************************/


/**
 * cancel upload
 */
Drupal.behaviors.uportal.bulkUploader.cancelUpload = function (file_ids) {
	
	//cancel server upload data
	var postFileIDs = new Array();
	var postNIDs = new Array();
	var finishedFileNames = new Array();
	
	$.each(file_ids, function(index, file_id) {
		var fileObj = Drupal.uportal.bulkUploader.files[file_id];
		var uploadStatus = fileObj.status; //waiting, uploading, finished, node-created, cancelled, deleted, processing-deletion
		
		//if uploading
		if (uploadStatus=='uploading') {
			fileObj.jqXHR.abort();
			Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'cancelled');
			Drupal.uportal.bulkUploader.noFilesToUpload--;
		}
		
		//if upload finished or node-created
		if (uploadStatus=='finished' || uploadStatus=='node-created' || uploadStatus=='processing-node-creation') {
			postFileIDs.push(file_id);
			
			if (uploadStatus=='finished' || uploadStatus=='processing-node-creation') { finishedFileNames.push(fileObj.new_name); }
			if (uploadStatus=='node-created') { postNIDs.push(fileObj.nid); }
			
			Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'processing-deletion');
			
		}
	});
	
	if (finishedFileNames.length>0 || postNIDs.length>0) {
		Drupal.behaviors.uportal.bulkUploader.cancelServerUpload(postFileIDs, postNIDs, finishedFileNames);
	}
};
/** ENDS *****************************************/


/**
 * cancel upload on server
 */
Drupal.behaviors.uportal.bulkUploader.cancelServerUpload = function(fileIDs, nids, finishedFileNames) {
	var url = '/management/bulk-uploader/cancel-upload';
	
	$.post(url, {
		'file_ids' : fileIDs,
		'finished_file_names' : finishedFileNames,
		'nids' : nids
	})
		.done( function (data) {
			$.each(data.file_ids, function (index, file_id) {
				var fileObj = Drupal.uportal.bulkUploader.files[file_id];
				Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'cancelled');
			});
			if (Drupal.uportal.bulkUploader.allUploadsCancelled) {
				window.location.reload(true);
			}
		});
}
/** ENDS *****************************************/


/**
 * function to receive upload finished feedback
 */
Drupal.behaviors.uportal.bulkUploader.anUploadIsDone = function (data) {
	$.each(data.result.files, function (index, file) {
		var fileObj = Drupal.uportal.bulkUploader.files[file.id];
		fileObj.new_name = file.name;
		Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'finished');
		Drupal.uportal.bulkUploader.noFilesUploaded++;
		Drupal.uportal.bulkUploader.nodesToCreate.push(fileObj);
		Drupal.behaviors.uportal.bulkUploader.createNodeAfterFileUpload();
	});
	if (Drupal.uportal.bulkUploader.noFilesToUpload == Drupal.uportal.bulkUploader.noFilesUploaded) {
		Drupal.uportal.bulkUploader.totalProgress = 'finished';
		Drupal.uportal.bulkUploader.noFilesToUpload = 0;
		Drupal.uportal.bulkUploader.noFilesUploaded = 0;
	}
	if (Drupal.uportal.bulkUploader.totalProgress == 'finished' && !Drupal.uportal.bulkUploader.allUploadsCancelled) {
		
	}
}
/** ENDS *****************************************/


/**
 * create a node after each file is uploaded
 */
Drupal.behaviors.uportal.bulkUploader.createNodeAfterFileUpload = function() {
	
	if (Drupal.uportal.bulkUploader.creatingNodeOperationActive) {
		//return;
	}
	
	//avoid database conflicts by creating nodes one by one
	Drupal.uportal.bulkUploader.creatingNodeOperationActive = true;
	
	var finishedUploadedFiles = Drupal.uportal.bulkUploader.nodesToCreate;
	if (finishedUploadedFiles.length>0) {
		var finishedUploadedFile = Drupal.uportal.bulkUploader.nodesToCreate.shift();
		var file_names = [];
		var file_ids = [];
		var fileObjects = Drupal.uportal.bulkUploader.files;
		var url = '/management/bulk-uploader/create-nodes';
		
		if (finishedUploadedFile.status == 'finished') {
			file_names.push(finishedUploadedFile.new_name);
			file_ids.push(finishedUploadedFile.file_id);
			Drupal.behaviors.uportal.bulkUploader.updateFileStatus(finishedUploadedFile, 'processing-node-creation');
		}
		
		$.post(url, {
			'file_names' : JSON.stringify(file_names),
			'file_ids' : JSON.stringify(file_ids),
		})
			.done( function (data) {
				Drupal.uportal.bulkUploader.creatingNodeOperationActive = false;
				Drupal.behaviors.uportal.bulkUploader.updateFileDivsToNodes(data);
				Drupal.behaviors.uportal.bulkUploader.createNodeAfterFileUpload();
			})
			.fail( function (data) {
				var file_ids = [];
				var dataParts = this.data.split("&");
				for (var i = 0, length = dataParts.length; i < length; i++ ) {
					var parts =  dataParts[i].split("=");
					if (parts[0]=='file_ids') {
						file_ids = JSON.parse(decodeURIComponent(parts[1]));
					}
				}
				$.each(file_ids, function(index, file_id) {
					var fileObj = Drupal.uportal.bulkUploader.files[file_id];
					var errorObj = {
						'errorType': 'ajax error'
					};
					Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'cancelled', errorObj);
				})
			});
	}
}
/** ENDS *****************************************/


/**
 * turn files to nodes
 */
Drupal.behaviors.uportal.bulkUploader.updateFileDivsToNodes = function (data) {
	var files = data.files;
	$.each(files, function (index, file) {
		Drupal.behaviors.uportal.bulkUploader.updateFileObjNodeCreated(file.file_id, file.nid);
	});
}
/** ENDS *****************************************/


/**
 * update total progress as finished
 */
Drupal.behaviors.uportal.bulkUploader.updateTotalProgress = function (status) {
	var $titleBar = $('div.title-bar', Drupal.uportal.bulkUploader.$uploaderDiv);
	
	if (status=='finished') {
		$titleBar.addClass('bulk-upload-node-created');
		$titleBar.text('All uploads completed :-)');
	}
	
	if (status=='more-uploads') {
		$titleBar.removeClass('bulk-upload-node-created');
		$titleBar.text('Uploading more files ...');
	}

};
/** ENDS *****************************************/


/**
 * update upload progress
 */
Drupal.behaviors.uportal.bulkUploader.updateUploadProgress = function (data) {
	var progress = parseInt(data.loaded / data.total * 100, 10);
	var $fileDiv = Drupal.uportal.bulkUploader.files[data.files[0].id].$fileDiv;
	var $barDiv = $('div#progress-bar-wrapper div', $fileDiv);
	$barDiv.css('width', progress+'%');
}
/** ENDS *****************************************/


/**
 * update file object as node created
 */
Drupal.behaviors.uportal.bulkUploader.updateFileObjNodeCreated = function (file_id, nid) {
	var fileObj = Drupal.uportal.bulkUploader.files[file_id];
	fileObj.nid = nid;
	Drupal.uportal.bulkUploader.files[file_id] = fileObj;
	Drupal.behaviors.uportal.bulkUploader.updateFileStatus(fileObj, 'node-created');
	
	//check all file states
	var allNodesCreated = true;
	$.each(Drupal.uportal.bulkUploader.files, function(index, fileObj) {
		if (allNodesCreated && fileObj.status!='node-created') {
			allNodesCreated = false;
		}
	});
	if (allNodesCreated) {
		Drupal.behaviors.uportal.bulkUploader.updateTotalProgress('finished');
	}
}
/** ENDS *****************************************/


/*** function to create div for a file ***************/
Drupal.behaviors.uportal.bulkUploader.createFileDiv = function(file) {
	var html_str = '<div class="clearfix file bulk-upload-waiting">'; //starts .file
	
	html_str += '<div class="icon"></div>'; //icon
	
	html_str += '<div class="title-wrapper">'; //starts .title-wrapper
	html_str += '<div class="title">'+file.name+'</div>';
	html_str += '<div id="progress-bar-wrapper"><div></div></div>';
	html_str += '<div class="error-msg upload-cancelled">Upload cancelled by user</div>';
	html_str += '<div class="error-msg big-file">Upload cancelled by user</div>';
	html_str += '<div class="error-msg wrong-file">Wrong file format.</div>';
	html_str += '</div>'; //ends .title-wrapper
	
	html_str += '<div id="cancel-btn" data-file-id="'+ file.id + '"><div></div></div>'; //close button
	
	html_str += '</div>'; //ends .file
	
	//create file's DIV
	var $fileDiv = $(html_str);
	
	//append file to progress DIV
	$fileDiv.appendTo(Drupal.uportal.bulkUploader.$progressDiv);
	
	return $fileDiv;
};
/** ENDS *****************************************/


/**
 * function to update file status
 */
Drupal.behaviors.uportal.bulkUploader.updateFileStatus = function(fileObj, status, errorObj) {
	
	//update file object
	fileObj.status = status;
	
	//update file div classes
	var newCSSClass = false;
	switch (status) {
		case 'node-created':
		case 'finished':
		case 'uploading':
			newCSSClass = 'bulk-upload-'+status;
			break;
		case 'deleted':
		case 'cancelled':
			newCSSClass = 'bulk-upload-cancelled-upload';
			break;
		case 'waiting':
		case 'processing-deletion':
		case 'processing-node-creation':
			break;
	}
	if (newCSSClass) {
		Drupal.behaviors.uportal.bulkUploader.updateFileDivClass(newCSSClass, fileObj.file_id);
	}
	
	if (typeof errorObj !== "undefined") {
		var msg = '';
		if (errorObj.errorType=='invalid file size') {
			var fileSize = Drupal.behaviors.uportal.bulkUploader.formatBytes(errorObj.fileSize);
			var maxFileSize = Drupal.behaviors.uportal.bulkUploader.formatBytes(errorObj.maxFileSize);
			msg = 'This file\'s size is too big (' + fileSize + '). Max file size allowed: ' + maxFileSize;
		}
		if (errorObj.errorType=='invalid file type') {
			msg = 'Invalid file type. Please read allowed file types.';
		}
		if (errorObj.errorType=='ajax error') {
			msg = 'Connection error. Check your Internet connection.';
		}
		$('.error-msg', fileObj.$fileDiv).text(msg);
	}
	
	//update edit and cancel buttons
	/**
	 * only enable EDIT CONTENT button if all files are either node-created, cancelled
	 * only disable CANCEL ALL button if any one file is: waiting, processing-deletion, processing-node-creation
	 */
	var fileObjs = Drupal.uportal.bulkUploader.files;
	var fileObjsTotalNo = Drupal.uportal.bulkUploader.fileIDCounter;
	var enableEditContentBtnStates = {
		'node-created': 0,
		'cancelled': 0,
		'total': 0,
	};
	var disableCancelAllBtnStates = {
		'waiting': 0,
		'processing-deletion': 0,
		'processing-node-creation': 0,
		'total': 0,
	};
	
	//get current states
	$.each(fileObjs, function(index, fileObj) {
		var status = fileObj.status;
		if (typeof enableEditContentBtnStates[status] !== "undefined") {
			enableEditContentBtnStates[status]++;
			enableEditContentBtnStates.total++;
		}
		if (typeof disableCancelAllBtnStates[status] !== "undefined") {
			disableCancelAllBtnStates[status]++;
			disableCancelAllBtnStates.total++;
		}
	});
	
	//enable or disable edit content button
	if (
		enableEditContentBtnStates.total==fileObjsTotalNo
		&& enableEditContentBtnStates.total != enableEditContentBtnStates.cancelled
	) {
		Drupal.uportal.bulkUploader.$allContentEditBtn.removeClass('disabled');
	} else {
		Drupal.uportal.bulkUploader.$allContentEditBtn.addClass('disabled');
	}
	
	//enable or disable cancel all button
	if (disableCancelAllBtnStates.total>0) {
		Drupal.uportal.bulkUploader.$allContentCancelBtn.addClass('disabled');
	} else {
		Drupal.uportal.bulkUploader.$allContentCancelBtn.removeClass('disabled');
	}
	
	//if all uploads have been cancelled, refresh the page
	if (
		enableEditContentBtnStates.total==fileObjsTotalNo
		&& enableEditContentBtnStates.total == enableEditContentBtnStates.cancelled
		&& Drupal.uportal.bulkUploader.allUploadsCancelled
	) {
		location.reload();
	}
	
};
/** ENDS *****************************************/


/*** function to add class to file div ***************/
Drupal.behaviors.uportal.bulkUploader.updateFileDivClass = function(newClassName, file_id) {
	var fileObj = Drupal.uportal.bulkUploader.files[file_id];
	var $fileDiv = fileObj.$fileDiv;
	$fileDiv.removeClass( function (index, css) {
		return (css.match(/\bbulk-upload-\S+/g) || []).join(' ');
	})
	$fileDiv.addClass(newClassName);
}
/** ENDS *****************************************/


/**
 * initiate upload buttons
 */
Drupal.behaviors.uportal.bulkUploader.initiateUploadButton = function () {

	var $btn = $('<a href="#" class="btn" id="bulk-uploader-btn">Upload</a>');
	
	//add to dom
	$btn.appendTo($('header.page-header div.col-2-header'));
	
	//add click event
	$btn.click( function() {
		
		//check user rights
		if (
			typeof Drupal.uportal.userPerms.disallowed !== 'undefined'
			&& ($.inArray('edit-content', Drupal.uportal.userPerms.disallowed) > -1)
		) {
			return false;
		}
		
		Drupal.uportal.bulkUploader.$initiatingUploadBtn = $btn;
		Drupal.uportal.bulkUploader.$filesInputField.click();
		return false;
	});
}
/** ENDS *****************************************/


/**
 * show or hide dashboard divs when upload has started
 */
Drupal.behaviors.uportal.bulkUploader.showUploaderDiv = function() {
	$('section.page-content').slideUp();
	$('header.page-header .header-strip-1 .col-2-header').slideUp();
	$('header.page-header .header-strip-2').slideUp();
	$('header.page-header .header-strip-3').slideUp();
	Drupal.uportal.bulkUploader.$uploaderDiv.slideDown( function() {
		//make footer sticky
		var $uploaderFooter = $('div.uploader-footer', Drupal.uportal.bulkUploader.$uploaderDiv);
		$uploaderFooter.appendTo($('body'));
		$uploaderFooter.addClass('sticky-footer');
	});
}
/** ENDS *****************************************/


/**
 * format bytes
 * credit: http://stackoverflow.com/questions/2510434/format-bytes-to-kilobytes-megabytes-gigabytes
 */
Drupal.behaviors.uportal.bulkUploader.formatBytes = function(bytes, precision) { 
	var units = ['B', 'KB', 'MB', 'GB', 'TB'];
	
	precision = (typeof precision === "undefined") ? 2 : precision;
	bytes = Math.max(bytes, 0); 
	var pow = Math.floor((bytes ? Math.log(bytes) : 0) / Math.log(1024)); 
	pow = Math.min(pow, units.length - 1); 
	bytes /= (1 << (10 * pow)); 

	return Math.round(bytes) + ' ' + units[pow]; 
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.bulkUploader.initiateUploadFunctionality);

