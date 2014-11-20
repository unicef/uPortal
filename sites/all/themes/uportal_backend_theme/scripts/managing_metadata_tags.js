
(function($) {


/**
 * globals we need for content listing
 */
Drupal.uportal.managingMetaDataTags = {
	$tagFormDiv : {},
	$tagThemeFormDiv : {},
	serverURL : '/management/metadata/form-handler/tags',
	serverReadURL : '/management/metadata/form-handler/read-only',
	existingVocabularies: {},
	existingTerms: {},
};
/** ENDS *****************************************/


/**
 * change tag label form to edit or add status
 */
Drupal.behaviors.uportal.managingMetaDataTags.changeTagFormStatus = function(opType, tid, values) {
	var $formDiv = Drupal.uportal.managingMetaDataTags.$tagFormDiv;
	var titleText = 'Add a Tag to a Tag Theme';
	var btnText = 'Save';
	var op = 'add-term';
	var op_general = 'add';
	var titleValue = '';
	var initiator = '';
	var vid = '';
	
	//reset form
	$('form', $formDiv).data('validator').resetForm();
	$('input, textarea', $formDiv).removeClass('error');
	
	//hide errors
	$('div.select-tag-theme-error', $formDiv).hide();
	$('.form-item-title .title-duplicate-error', $formDiv).hide();
	
	//show vocabulary select by default
	$('div.form-item-tag-theme', $formDiv).show();
	
	//update tag theme select
	var tagThemes = Drupal.uportal.managingMetaDataTags.existingVocabularies;
	var $select = $('select#tag_theme', $formDiv);
	$select.empty().append('<option value=""></option>');
	$.each(tagThemes, function(index, tagTheme) {
		$select.append('<option value="'+tagTheme.vid+'">'+tagTheme.name+'</option>');
	});
	$select.val('');
	$select.trigger('change').trigger('chosen:updated');
	
	if (opType=='edit' && parseInt(tid)>0) {
		var term = Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+tid];
		titleValue = term.name;
		vid = term.vid;
		titleText = 'Edit Tag';
		btnText = 'Update';
		op = 'edit-term';
		op_general = 'edit';
		
		//hide changing vocabulary till proper updates to node reference fields can be made
		$('div.form-item-tag-theme', $formDiv).hide();
	}
	
	if (values.vid !== 'undefined') {
		vid = values.vid;
	}
	if (vid) {
		$select.val(vid).trigger('change').trigger("chosen:updated");
	}
	
	if (typeof values.initiator !== 'undefined') {
		initiator = values.initiator;
	}
	
	$('.dialog-header', $formDiv).text(titleText);
	$('.dialog-footer .yes', $formDiv).val(btnText);
	$('input.metadata-title', $formDiv).val(titleValue);
	$('input#op', $formDiv).val(op);
	$('input#op-general', $formDiv).val(op_general);
	$('input#tid', $formDiv).val(tid);
	$('input#old-vid', $formDiv).val(vid);
	$('input#initiator', $formDiv).val(initiator);
	
}
/** ENDS *****************************************/


/**
 * change tag theme form to edit or add status
 */
Drupal.behaviors.uportal.managingMetaDataTags.changeTagThemeFormStatus = function(opType, nid, values) {
	var $formDiv = Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv;
	var titleText = 'Create a Tag Theme';
	var btnText = 'Save';
	var op = 'add-vocabulary';
	var op_general = 'add';
	var titleValue = '';
	var initiator = '';
	
	if (opType=='edit') {
		titleText = 'Edit Tag Theme';
		btnText = 'Update';
		op = 'edit-vocabulary';
		op_general = 'edit';
		if (values.title) titleValue = values.title;
	}
	
	//reset form
	$('form', $formDiv).data('validator').resetForm();
	$('input, textarea', $formDiv).removeClass('error');
	
	if (typeof values.initiator !== "undefined") {
		initiator = values.initiator;
	}

	$('.dialog-header', $formDiv).text(titleText);
	$('.dialog-footer .yes', $formDiv).val(btnText);
	$('input.metadata-title', $formDiv).val(titleValue);
	$('input#op', $formDiv).val(op);
	$('input#op-general', $formDiv).val(op_general);
	$('input#vid', $formDiv).val(nid);
	$('input#initiator', $formDiv).val(initiator);
	
	//hide errors
	$('.form-item-title .title-duplicate-error', $formDiv).hide();
	
}
/** ENDS *****************************************/


/**
 * initiate add button
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateAddButton = function() {
	
	if (!($('body').hasClass('page-management-metadata-tags'))) {
		return;
	}
	
	var $createNewTagThemeBtn = $('<a class="btn tag-theme-btn">Create Tag Theme</a>').prependTo($('header.page-header .header-strip-1 .col-2-header'));
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$createNewTagThemeBtn.click( function() {
			Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagThemeForm('add', 0, {initiator:"tags-management-page"});
		});
	} else {
		$createNewTagThemeBtn.addClass('disabled');
	}
	
	var $createNewTagBtn = $('<a class="btn tag-btn">Add Tag</a>').prependTo($('header.page-header .header-strip-1 .col-2-header'));
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$createNewTagBtn.click( function() {
			Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagForm('add', 0, {initiator:"tags-management-page"});
		});
	} else {
		$createNewTagBtn.addClass('disabled');
	}

};
/** ENDS *****************************************/


/**
 * show create tag form
 */
Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagForm = function (op, nid, args) {
	Drupal.behaviors.uportal.managingMetaDataTags.changeTagFormStatus(op, nid, args);
	$.blockUI({
		message: Drupal.uportal.managingMetaDataTags.$tagFormDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
};
/** ENDS *****************************************/


/**
 * show create tag theme form
 */
Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagThemeForm = function (op, nid, args) {
	Drupal.behaviors.uportal.managingMetaDataTags.changeTagThemeFormStatus(op, nid, args);
	$.blockUI({
		message: Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
};
/** ENDS *****************************************/


/**
 * initiate edit buttons
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateEditButtons = function() {
	
	//vocabulary edit button
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$('.main-result-wrapper .main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).click(
			function() {
				var $btn = $(this);
				var nid = $btn.data('nid');
				var $nodeDiv = $('#parent-nid-'+nid, Drupal.uportal.managingMetaData.$resultsWrapper);
				var vocabularyTitle = $('.main-result-wrapper .main-result-title', $nodeDiv).text();
				var vocabularyValues = {
					'title' : vocabularyTitle,
					'initiator' : 'tags-management-page'
				};
				
				//update and show form
				Drupal.behaviors.uportal.managingMetaDataTags.changeTagThemeFormStatus('edit', nid, vocabularyValues);
				$.blockUI({
					message: Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv,
					blockMsgClass: 'dialog-box-block-ui-wrapper'
				});
			}
		);
	} else {
		$('.main-result-wrapper .main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('disabled');
	}
	
	//tag edit button
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$('.main-result-content-wrapper .main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).click(
			function() {
				var $btn = $(this);
				var tid = $btn.data('tid');
				var $tagDiv = $('.main-result-content-wrapper #result-row-tid-'+tid, Drupal.uportal.managingMetaData.$resultsWrapper);
				var tagValues = {
					'initiator' : 'tags-management-page'
				};
				
				//update and show form
				Drupal.behaviors.uportal.managingMetaDataTags.changeTagFormStatus('edit', tid, tagValues);
				$.blockUI({
					message: Drupal.uportal.managingMetaDataTags.$tagFormDiv,
					blockMsgClass: 'dialog-box-block-ui-wrapper'
				});
			}
		);
	} else {
		$('.main-result-content-wrapper .main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('disabled');
	}
};
/** ENDS *****************************************/


/**
 * delete vocabulary or term
 */
Drupal.behaviors.uportal.managingMetaDataTags.deleteVocabOrTerm = function(args) {
	var $removeDiv;
	var deleteTitle = "Delete Tag Theme";
	var deleteMsg = "Are you sure you want to delete: <em>" + args.name + "</em>?";
	var waitTitle = "Deleting Tag Theme";
	var waitMsg = "Please wait as we delete this tag theme ...";
	var postVars = {
		op: 'delete-term-or-vocabulary',
		vid: args.vid
	};
	
	if (args.type=='term') {
		deleteTitle = "Delete Tag";
		waitTitle = "Deleting Tag";
		waitMsg = "Please wait as we delete this tag ...";
		postVars.tid = args.tid;
		postVars.type = 'term';
		var $parentDiv = $('#parent-nid-'+args.vid, Drupal.uportal.managingMetaData.$resultsWrapper);
		$removeDiv = $('#result-row-tid-'+args.tid, $parentDiv);
	} else {
		postVars.type = 'vocabulary';
		$removeDiv = $('#parent-nid-'+args.vid, Drupal.uportal.managingMetaData.$resultsWrapper);
	}
	
	//prepare delete confirmation
	var $confirmationDiv = $('div#confirm-deletion-taxonomy');
	$('.dialog-header', $confirmationDiv).text(deleteTitle);
	$('.dialog-content', $confirmationDiv).html(deleteMsg);
	
	//if confirmed
	$('.dialog-footer .yes', $confirmationDiv).click( function() {
		Drupal.behaviors.uportal.blockUIWaitMessage(waitTitle, waitMsg);
		$.post(Drupal.uportal.managingMetaDataTags.serverURL, postVars).done(
			function(data) {
				$removeDiv.remove();
				if (data.type=='term') {
					delete Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+data.tid];
				} else {
					delete Drupal.uportal.managingMetaDataTags.existingVocabularies['vid-'+data.vid];
				}
				$.unblockUI();
			});
	});
	
	//show confirmation message
	$.blockUI({
		message:$confirmationDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
};
/** ENDS *****************************************/


/**
 * initiate all delete buttons
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateDeleteButtons = function() {
	
	//vocabulary delete button
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$('.main-result-wrapper .main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).click( function() {
			var $btn = $(this);
			var vid = $btn.data('nid');
			var vocab = Drupal.uportal.managingMetaDataTags.existingVocabularies['vid-'+vid];
			Drupal.behaviors.uportal.managingMetaDataTags.deleteVocabOrTerm({
				type: 'vocabulary',
				name: vocab.name,
				vid: vid
			});
		});
	} else {
		$('.main-result-wrapper .main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('main-result-delete-disabled');
	}
	
	//term delete button
	if (Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
		$('.main-result-content-wrapper .main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).click( function() {
			var $btn = $(this);
			var tid = $btn.data('tid');
			var term = Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+tid];
			Drupal.behaviors.uportal.managingMetaDataTags.deleteVocabOrTerm({
				type: 'term',
				name: term.name,
				vid: term.vid,
				tid: tid
			});
		});
	} else {
		$('.main-result-content-wrapper .main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('main-result-delete-disabled');
	}
}
/** ENDS *****************************************/


/**
 * form validator
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateTagThemeForm = function() {
	
	//validate title
	var $form = $('form', Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv);
	$form.validate({
		rules: {
			title: {
				required: true,
				minlength: 2
			}
		},
		messages: {
			title: {
				required: 'This field is required.',
				minlength: 'This field requires a minimum of two characters.'
			}
		}
	});
	
	//initiate cancel button
	$('form a.cancel-tag-theme-form', Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv).click( function() {
		var initiatorVal = $('input#initiator', Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv).val();
		
		//unblock ui
		$.unblockUI();
		
		if (initiatorVal=='tag-form' || initiatorVal=='tag-form-bulk-edit') {
			var initiatorStr = (initiatorVal=='tag-form-bulk-edit') ? 'bulk-edit-choose-tags' : 'tags-management-page';
			Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagForm('add', 0, {initiator:initiatorStr});
		}
		
	});
	
	//submit via ajax
	$('form', Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv).ajaxForm({
		dataType: 'json',
		beforeSubmit: function(formData, $form, options) {
			
			//check for duplicate titles
			var titleExists = Drupal.behaviors.uportal.managingMetaDataTags.checkDuplicateVocabularyTitles($form);
			if (titleExists) {
				$('.form-item-title .title-duplicate-error', $form).slideDown();
				return false;
			} else {
				$('.form-item-title .title-duplicate-error', $form).slideUp();
			}
			
			var title = 'Adding a new tag theme ...';
			var msg = 'Please wait as we add this new tag theme ...';
			var op_general = $('input#op-general', $form).val();
			
			if (op_general=='edit') {
				title = 'Editing Tag Theme ...';
				msg = 'Please wait as we update this tag theme ...';
			}
			
			Drupal.behaviors.uportal.blockUIWaitMessage(title, msg);
			return true;
		},
		success: function(jsonData, statusText, xhr, $form) {
			if (jsonData.op_general=='add') {
				Drupal.uportal.$eventWatcher.trigger('added-vocabulary', jsonData);
				Drupal.attachBehaviors();
			} else if (jsonData.op_general=='edit') {
				Drupal.uportal.$eventWatcher.trigger('updated-vocabulary', jsonData);
			}
		}
	});
	
}
/** ENDS *****************************************/


/**
 * check for duplicate title
 */
Drupal.behaviors.uportal.managingMetaDataTags.checkDuplicateVocabularyTitles = function($form) {
	
	var vid = $('input#vid', $form).val();
	var newTitle = $('input#metadata-title-tag-theme', $form).val();
	var existingTitles = Drupal.uportal.managingMetaDataTags.existingVocabularies;
	var titleExists = Drupal.behaviors.uportal.checkStringExistsInArray(newTitle, existingTitles);
	
	if ((parseInt(vid)>0)) {
		var oldTitle = existingTitles['vid-'+vid]['name'];
		if (Drupal.behaviors.uportal.stringsAreTheSame(newTitle, oldTitle)) {
			titleExists = false;
		}
	}
	
	return titleExists;
};
/** ENDS *****************************************/


/**
 * check for duplicate title
 */
Drupal.behaviors.uportal.managingMetaDataTags.checkDuplicateTermTitles = function($form) {
	
	var vid = $('select#tag_theme', $form).val();
	var tid = $('input#tid', $form).val();
	var newTitle = $('input#metadata-title-tag', $form).val();
	var existingTerms = {};
	
	//terms in vocabulary
	$.each(Drupal.uportal.managingMetaDataTags.existingTerms, function(index, term) {
		if (term.vid==vid) {
			existingTerms[index] = term;
		}
	})
	
	//title exists
	var titleExists = Drupal.behaviors.uportal.checkStringExistsInArray(newTitle, existingTerms);
	
	if ((parseInt(tid)>0)) {
		var oldTitle = '';
		if (typeof existingTerms['tid-'+tid] !== 'undefined') {
			oldTitle = existingTerms['tid-'+tid]['name'];
		}
		if (Drupal.behaviors.uportal.stringsAreTheSame(newTitle, oldTitle)) {
			titleExists = false;
		}
	}
	
	return titleExists;
};
/** ENDS *****************************************/


/**
 * form validator
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateTagForm = function() {
	
	//validate title
	$('form', Drupal.uportal.managingMetaDataTags.$tagFormDiv).validate({
		rules: {
			tag_theme: {
				required: true
			},
			tag_name: {
				required: true,
				minlength: 2
			}
		},
		messages: {
			tag_name: {
				required: 'This field is required.',
				minlength: 'This field requires a minimum of two characters.'
			},
			tag_theme: {
				required: 'Please select a tag theme for this tag.'
			}
		},
		errorPlacement: function(error, element) {
			error.appendTo( element.parent(".field") );
		}
	});
	
	//turn select into chosen
	$('select#tag_theme', Drupal.uportal.managingMetaDataTags.$tagFormDiv).chosen();
	
	//initiate add new tag theme button
	var tagThemeFormInitiator = ($('body').hasClass('page-management-metadata-tags')) ? 'tag-form' : 'tag-form-bulk-edit';
	$('form div.create-new-tag-theme-btn', Drupal.uportal.managingMetaDataTags.$tagFormDiv).click( function() {
		Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagThemeForm('add', 0, {initiator:"tag-form"});
	});
	
	//initiate cancel button
	$('form a.cancel-tag-form', Drupal.uportal.managingMetaDataTags.$tagFormDiv).click( function() {
		var initiatorVal = $('input#initiator', Drupal.uportal.managingMetaDataTags.$tagFormDiv).val();
		
		//unblock ui
		$.unblockUI();
		
		if (initiatorVal=='bulk-edit-choose-tags') {
			Drupal.behaviors.uportal.bulkEditor.showChooseTagsForm();
		}
		
	});
	
	//submit via ajax
	$('form', Drupal.uportal.managingMetaDataTags.$tagFormDiv).ajaxForm({
		dataType: 'json',
		beforeSubmit: function(formData, $form, options) {
			
			//check for duplicate titles
			var titleExists = Drupal.behaviors.uportal.managingMetaDataTags.checkDuplicateTermTitles($form);
			if (titleExists) {
				$('.form-item-title .title-duplicate-error', $form).slideDown();
				return false;
			} else {
				$('.form-item-title .title-duplicate-error', $form).slideUp();
			}
			
			var title = 'Adding a new tag ...';
			var msg = 'Please wait as we add this new tag ...';
			var op_general = $('input#op-general', $form).val();
			var $tagThemeSelect = $('select#tag_theme', Drupal.uportal.managingMetaDataTags.$tagFormDiv);
			var tagThemeValue = $tagThemeSelect.val();
			if (tagThemeValue=="") {
				$('div.select-tag-theme-error', Drupal.uportal.managingMetaDataTags.$tagFormDiv).show();
				return false;
			} else {
				$('div.select-tag-theme-error', Drupal.uportal.managingMetaDataTags.$tagFormDiv).hide();
			}
			
			if (op_general=='edit') {
				title = 'Editing Tag ...';
				msg = 'Please wait as we update this tag ...';
			}
			
			Drupal.behaviors.uportal.blockUIWaitMessage(title, msg);
			return true;
		},
		success: function(jsonData, statusText, xhr, $form) {
			if (jsonData.op_general=='add') {
				Drupal.uportal.$eventWatcher.trigger('added-tag', jsonData);
				Drupal.attachBehaviors();
			} else if (jsonData.op_general=='edit') {
				Drupal.uportal.$eventWatcher.trigger('updated-tag', jsonData);
			}
		}
	});
	
}
/** ENDS *****************************************/


/**
 * watch event - vocabulary updated
 */
Drupal.behaviors.uportal.managingMetaDataTags.vocabularyUpdated = function(event, data) {
	
	//update existing data
	Drupal.uportal.managingMetaDataTags.existingVocabularies['vid-'+data.vid].name = data.name;
	
	if (data.initiator=='tags-management-page') {
		var $nodeDiv = $('#parent-nid-'+data.vid, Drupal.uportal.managingMetaData.$resultsWrapper);
		$('.main-result-wrapper .main-result-title', $nodeDiv).text(data.name);
		
		$.unblockUI();
	}
};
/** ENDS *****************************************/


/**
 * watch event - vocabulary added
 */
Drupal.behaviors.uportal.managingMetaDataTags.vocabularyAdded = function(event, data) {
	
	//update existing data
	Drupal.uportal.managingMetaDataTags.existingVocabularies['vid-'+data.vid] = {
		vid: data.vid,
		name: data.name
	};
	
	if ($('body').hasClass('page-management-metadata-tags')) {
		Drupal.uportal.managingMetaData.$resultsWrapper.append(data.new_row_html);
	}
	
	$.unblockUI();
	
	if (data.initiator=='tag-form' || data.initiator=='tag-form-bulk-edit') {
		var initiatorStr = (data.initiator=='tag-form-bulk-edit') ? 'bulk-edit-choose-tags' : 'tags-management-page';
		Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagForm('add', 0, {initiator:initiatorStr, vid:data.vid});
	}
};
/** ENDS *****************************************/


/**
 * watch event - tag added
 */
Drupal.behaviors.uportal.managingMetaDataTags.tagAdded = function(event, data) {
	
	//update existing data
	Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+data.tid] = {
		vid: data.vid,
		tid: data.tid,
		name: data.name
	};
	
	if (data.initiator=='tags-management-page') {
		var $parentDiv = $('#parent-nid-'+data.vid+' .main-result-content-wrapper .result-content', Drupal.uportal.managingMetaData.$resultsWrapper);
		$parentDiv.append(data.new_row_html);
		$.unblockUI();
	}
};
/** ENDS *****************************************/


/**
 * watch event - tag updated
 */
Drupal.behaviors.uportal.managingMetaDataTags.tagUpdated = function(event, data) {
	
	//update existing data
	Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+data.tid].name = data.name;
	Drupal.uportal.managingMetaDataTags.existingTerms['tid-'+data.tid].vid = data.vid;
	
	if (data.initiator=='tags-management-page') {
		var $parentDiv = $('#parent-nid-'+data.old_vid, Drupal.uportal.managingMetaData.$resultsWrapper);
		var $childDiv = $('#result-row-tid-'+data.tid, $parentDiv);
		$('.tag-link a', $childDiv).text(data.name);
		
		//if parent vocabulary has changed, move $nodeDiv
		//TODO: update totals for vocabulary
		if (data.vid!=data.old_vid) {
			var $oldParentDiv = $parentDiv;
			var $newParentDiv = $('#parent-nid-'+data.vid, Drupal.uportal.managingMetaData.$resultsWrapper);
			$childDiv.appendTo($('.main-result-content-wrapper .result-content', $newParentDiv));
			
			//update totals for old and new vocabulary
			$('.result-content-header .result-category-count', $oldParentDiv).text(data.oldVocabularyCount);
			$('.result-content-header .result-category-count', $newParentDiv).text(data.newVocabularyCount);
		}
	
		$.unblockUI();
	}
};
/** ENDS *****************************************/


/**
 * initiate metadata page
 */
Drupal.behaviors.uportal.managingMetaDataTags.initiateMetaDataTagsPage = function(context, settings) {
	
	//check page
	if (
		!($('body').hasClass('page-management-metadata-tags'))
		&& !($('body').hasClass('page-management-edit-content'))
		&& !($('body').hasClass('page-management-content-individual-items'))
	) {
		return;
	}
	
	$('body').once( function() {
		Drupal.uportal.managingMetaDataTags.$tagFormDiv = $('div#metadata-forms-wrapper div#tag-form-wrapper');
		Drupal.uportal.managingMetaDataTags.$tagThemeFormDiv = $('div#metadata-forms-wrapper div#tag-theme-form-wrapper');
		
		//initiate form
		Drupal.behaviors.uportal.managingMetaDataTags.initiateTagForm();
		Drupal.behaviors.uportal.managingMetaDataTags.initiateTagThemeForm();
		
		//initiate functionality
		Drupal.behaviors.uportal.managingMetaDataTags.initiateAddButton();
		
		//watch these events
		Drupal.uportal.$eventWatcher.on('updated-vocabulary', Drupal.behaviors.uportal.managingMetaDataTags.vocabularyUpdated);
		Drupal.uportal.$eventWatcher.on('added-vocabulary', Drupal.behaviors.uportal.managingMetaDataTags.vocabularyAdded);
		Drupal.uportal.$eventWatcher.on('updated-tag', Drupal.behaviors.uportal.managingMetaDataTags.tagUpdated);
		Drupal.uportal.$eventWatcher.on('added-tag', Drupal.behaviors.uportal.managingMetaDataTags.tagAdded);
		
		//get existing terms and vocabularies
		$.post(Drupal.uportal.managingMetaDataTags.serverReadURL, {
			'op' : 'get-existing-taxonomy'
		}).done( function(data) {
			Drupal.uportal.managingMetaDataTags.existingTerms = data.terms;
			Drupal.uportal.managingMetaDataTags.existingVocabularies = data.vocabularies;
			Drupal.uportal.$eventWatcher.trigger('fetched-existing-taxonomy', {});
		});
		
	});
	
	Drupal.behaviors.uportal.managingMetaDataTags.initiateDeleteButtons();
	Drupal.behaviors.uportal.managingMetaDataTags.initiateEditButtons();
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.managingMetaDataTags.initiateMetaDataTagsPage);
