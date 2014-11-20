

(function($) {

/**
 * globals we need for the bulk editor
 * trigger events after server call:
 * 		bulkEditorDeletedNodes :: after deleting nodes on server
 * 		 bulkEditorEditedNodeMetaData :: after editing node meta data
 */
Drupal.uportal.bulkEditor = {
	initiated : false,
	serverURL : '/management/bulk-editor',
	serverSeriesURL : '/management/bulk-editor/series',
	serverLessonPlansURL : '/management/bulk-editor/lesson-plans',
	serverReadURL : '/management/bulk-editor/read-only',
	selectedNIDs : new Array(),
	$dialogsWrapper : {},
	$formsWrapper : {},
	pageCategoryNID : '',
	pageContentProviderNID : '',
	pageLanguageCode : '',
	pageSeriesNID : '',
	pageLessonPlanNID : '',
	pageStatus : '',
	existingLessonPlans : {},
	existingSeries : {},
	seriesByCategory : [],
	formStates : false,
	metaDataFormState: {},
	initiatingCategorySelectID: ''
};
/** ENDS *****************************************/


/**
 * bulk edit nodes
 */
Drupal.behaviors.uportal.bulkEditor.bulkEditNodes = function(args) {
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	var $formDialog = Drupal.uportal.bulkEditor.$formsWrapper;
	
	if (noOfSelectedItems<=0) {
		return;
	}
	
	//update title
	Drupal.behaviors.uportal.bulkEditor.updateBulkEditNodesTitle();
	
	//reset forms
	Drupal.behaviors.uportal.bulkEditor.resetForms();
	Drupal.behaviors.uportal.bulkEditor.fillFormDefaultValues(args);
	
	//show dialog box
	$.blockUI({
		message: $formDialog,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * update bulk edit nodes dialog title
 */
Drupal.behaviors.uportal.bulkEditor.updateBulkEditNodesTitle = function(args) {
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	var $formDialog = Drupal.uportal.bulkEditor.$formsWrapper;
	var title = 'Bulk editing <span class="number">'+noOfSelectedItems+'</span> item(s) ...';
	var page_management_series = $('body').hasClass('page-management-content-series');
	var page_management_lesson_plans = $('body').hasClass('page-management-content-lesson-plans');
	
	if (page_management_series) {
		title = 'Managing Series';
	} else if (page_management_lesson_plans) {
		title = 'Managing Lesson Plans';
	} else if (noOfSelectedItems<=0) {
		return;
	}
	
	$('.dialog-header', $formDialog).html(title);
};
/** ENDS *****************************************/


/**
 * delete nodes
 */
Drupal.behaviors.uportal.bulkEditor.deleteNodes = function () {
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	
	//no nids OR disabled
	if ( noOfSelectedItems<=0) {
		return;
	}
	
	//run confirmation box
	var $confirmDeleteDialog = $('div#confirm-deletion', Drupal.uportal.bulkEditor.$dialogsWrapper);
	$('.dialog-header', $confirmDeleteDialog).text('Delete '+noOfSelectedItems+' Items');
	$('a.yes', $confirmDeleteDialog).click( function () {
		Drupal.behaviors.uportal.bulkEditor.deleteNodesServerCall(noOfSelectedItems);
		return false;
	});
	
	//confirm deletion of nodes
	$.blockUI({ 
		message: $confirmDeleteDialog,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
	
}

Drupal.behaviors.uportal.bulkEditor.deleteNodesServerCall = function (noOfSelectedItems) {
	//switch to loading message
	Drupal.behaviors.uportal.blockUIWaitMessage('Deleting '+noOfSelectedItems+' Items', 'Please wait ...');
	
	//delete nodes
	$.post(Drupal.uportal.bulkEditor.serverURL, {
		'operation_type' : 'delete',
		'nids' : Drupal.uportal.bulkEditor.selectedNIDs
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorDeletedNodes', { 'nids' : data.nids });
		});
};
/** ENDS *****************************************/


/**
 * go to edit page
 */
Drupal.behaviors.uportal.bulkEditor.goToEditPage = function () {
	
	//get current url
	var $btn = $(this);
	var currentURL = encodeURIComponent($btn.data('current-url'));
	
	//no nids OR disabled
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		return;
	}
	
	//if nids are present, go to edit page
	var nidsStr = Drupal.uportal.bulkEditor.selectedNIDs.join('+');
	window.location.href = window.location.protocol
		+ '//'
		+ window.location.host
		+ '/management/edit-content/'
		+ encodeURIComponent(nidsStr)
		+ '?from-url='+currentURL;
}
/** ENDS *****************************************/


/**
 * initiate bulk edit form
 */
Drupal.behaviors.uportal.bulkEditor.initiateBulkEditForms = function () {
	
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	
	//implement tabs
	$('.dialog-content', $formsWrapper).tabs();
	
	//content provider data
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'content-providers'
	}).done(Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues);
	
	//language data
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'languages'
	}).done(Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues);
	
	//category data
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'categories'
	}).done(Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues);
	
	//series data
	/*
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'series'
	}).done(Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues);
	*/
	
	//lesson plan data
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'lesson-plans'
	}).done(Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues);
	
	//series by category data
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get_json_data',
		'request-data-type' : 'series-by-category'
	}).done( function(data) {
		Drupal.uportal.bulkEditor.seriesByCategory = data;
	});
	
	//handle save meta data button
	$('.metadata-form .dialog-buttons .yes', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitMetaDataForm);
	
	//handle save choose series button
	$('.series-form .dialog-buttons .choose-series-div .yes', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitChooseSeriesForm);
	
	//cancel create new series
	$('.cancel-create-new-series', $formsWrapper).click( function () {
		if ($('body').hasClass('page-management-content-listing-individual-items')) {
			Drupal.behaviors.uportal.bulkEditor.showChooseSeriesForm();	
		} else {
			$.unblockUI();
		}
	});
	
	//create series button
	$('.create-new-series-btn', $formsWrapper).click(
		function () {
			
			//check user rights
			if (!Drupal.behaviors.uportal.userIsAllowed('manage-series')) {
				return;
			}
			
			var chosenCategoryNID = $('select#category-choose-series', $formsWrapper).val();
			Drupal.behaviors.uportal.bulkEditor.showCreateEditSeriesForm({
				'initiator': 'bulk-editor',
				'default_data': {
					'categoryNID': chosenCategoryNID
				}
			});
		}
	);
	
	//save new series button
	$('.series-form .dialog-buttons .create-series-div .yes', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitCreateEditSeriesForm);
	
	//cancel create new lesson plan
	$('.cancel-create-new-lesson-plan', $formsWrapper).click( function () {
		if ($('body').hasClass('page-management-content-listing-individual-items')) {
			Drupal.behaviors.uportal.bulkEditor.showChooseLessonPlanForm();
		} else {
			$.unblockUI();
		}
	});
	
	//handle save choose lesson plan button
	$('.lesson-plans-form .dialog-buttons .choose-lesson-plans-div .yes', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitChooseLessonPlansForm);
	
	//create lesson plan button
	$('.create-new-lesson-plan-btn', $formsWrapper).click(
		function() {
			
			//check user rights
			if (!Drupal.behaviors.uportal.userIsAllowed('manage-lesson-plans')) {
				return;
			}
			
			Drupal.behaviors.uportal.bulkEditor.showCreateEditLessonPlanForm({
				'initiator': 'bulk-editor'
			});
		}
	);
	
	//create new category button
	$('.create-new-category-btn', $formsWrapper).click(function() {
		
		//check user rights
		if (!Drupal.behaviors.uportal.userIsAllowed('manage-categories')) {
			return;
		}
		
		var $btn = $(this);
		var initiatingSelectID = '';
		if ($btn.hasClass('create-new-category-btn-metadata')) {
			initiatingSelectID = 'category';
		}
		if ($btn.hasClass('create-new-category-btn-choose-series')) {
			initiatingSelectID = 'category-choose-series';
		}
		if ($btn.hasClass('create-new-category-btn-new-series')) {
			initiatingSelectID = 'series-form-category';
		}
		Drupal.behaviors.uportal.bulkEditor.createNewCategory(initiatingSelectID);
	});
	
	//create new language button
	$('.create-new-language-btn', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.createNewLanguage);
	
	//create new provider button
	$('.create-new-content-provider-btn', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.createNewContentProvider);
	
	//save new lesson plan button
	$('.lesson-plans-form .dialog-buttons .create-lesson-plan-div .yes', $formsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitCreateEditLessonPlanForm);
	
	//chain category and series selects
	Drupal.behaviors.uportal.bulkEditor.prepareCategorySeriesSelects();
	
	//initiate change node status buttons
	Drupal.behaviors.uportal.bulkEditor.initiateBulkEditNodeStatusBtns($formsWrapper);
	
};
/** ENDS *****************************************/


/**
 * initiate tags form
 */
Drupal.behaviors.uportal.bulkEditor.initiateTagsForm = function (e, data) {
	var vocabularies = Drupal.uportal.managingMetaDataTags.existingVocabularies;
	var terms = Drupal.uportal.managingMetaDataTags.existingTerms;
	var $dialogsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	var $tagsSelect = $('select#tags', $dialogsWrapper);
	
	//add options
	$.each(terms, function (index, term) {
		var vocab = vocabularies['vid-'+term.vid];
		$tagsSelect.append('<option value="'+term.tid+'">'+term.name+' ('+vocab.name+')'+'</option>');
	});
	
	//initiate chosen
	$tagsSelect.chosen();
	
	//add new button
	$('div.create-new-tag-btn', $dialogsWrapper).click( function() {
		
		//check user rights
		if (!Drupal.behaviors.uportal.userIsAllowed('manage-tags')) {
			return;
		}
		
		Drupal.behaviors.uportal.managingMetaDataTags.showCreateTagForm('add', 0, {initiator:'bulk-edit-choose-tags'});
		
	});
	
	//submit button
	
	//handle save choose lesson plan button
	$('.tags-form .dialog-buttons .choose-tags-btns-div .yes', $dialogsWrapper).click(Drupal.behaviors.uportal.bulkEditor.submitChooseTagsForm);
	
};
/** ENDS *****************************************/


/**
 * initiate change-node-status buttons
 */
Drupal.behaviors.uportal.bulkEditor.initiateBulkEditNodeStatusBtns = function($formsWrapper) {
	var $changeStatusWrapper = $('div.node-status-form', $formsWrapper);
	var $btnsWrapper = $('div#change-node-status', $changeStatusWrapper);
	var $feedbackWrapper = $('div#change-node-status-operations-feedback', $changeStatusWrapper).hide();
	var $formBtn = $('.dialog-footer a.btn', $changeStatusWrapper);
	var messages = {
		'loading': 'Please wait as we update the states of these nodes.',
		'btn': {
			'default': 'Cancel',
			'loading': 'Please wait ...',
			'finished': 'Close'
		},
		'published': {
			'success': 'Success: All selected item(s) published.',
			'warning': 'Warning: [no_not_published] item(s) could not be published due to missing metadata.',
			'error': 'Warning: All item(s) failed to publish due to missing metadata.'
		},
		'draft': {
			'success': 'Success: All selected item(s) reverted to draft status.',
		}
	};
	
	//cancel button
	$formBtn.click( function() {
		var $btn = $(this);
		if ($btn.hasClass('disabled')) {
			return;
		}
		$.unblockUI();
	})
	
	//publish button
	$('a.btn', $btnsWrapper).click( function() {
		var $btn = $(this);
		var status = ($btn.hasClass('publish-status-btn')) ? '1' : '0';
		
		//update feedback and button
		$formBtn.addClass('disabled').text(messages.btn.loading);
		$('.msg', $feedbackWrapper).text(messages.loading);
		$feedbackWrapper.attr('class', '').show();
		$btnsWrapper.hide();
		
		$.post(Drupal.uportal.bulkEditor.serverURL, {
			'operation_type' : 'bulk-edit-node-status',
			'edit-data' : {
				'nids' : Drupal.uportal.bulkEditor.selectedNIDs,
				'status' : status
			}
		})
			.done( function (data) {
				Drupal.uportal.$eventWatcher.trigger('bulkEditorEditedNodeStatus', data);
				
				//make feedback message
				var msg = '';
				var msgClass = '';
				switch (data.status) {
					case '0':
						msg = messages.draft.success;
						msgClass = 'ok';
						break;
					case '1':
						if (data.failed_nids.length == data.all_nids.length) {
							msg = messages.published.error;
							msgClass = 'error-msg';
						} else if (data.changed_nids.length>0 && data.failed_nids.length>0) {
							msg = messages.published.warning.replace('[no_not_published]', data.failed_nids.length).replace('[no_published]', data.changed_nids.length);
							msgClass= 'warning';
						} else {
							msg = messages.published.success;
							msgClass = 'ok';
						}
						break;
				}
				
				$formBtn.removeClass('disabled').text(messages.btn.finished);
				$('.msg', $feedbackWrapper).text(msg);
				$feedbackWrapper.attr('class', msgClass);
			});
	});

};
/** ENDS *****************************************/


/**
 * show CREATE LESSON PLAN form
 */
Drupal.behaviors.uportal.bulkEditor.showCreateEditLessonPlanForm = function(args) {
	
	var initiator = '';
	var lessonPlanNID = '';
	var lessonPlanTitle = '';
	var lessonPlanBody = '';
	var dialogTitle = 'Create a Lesson Plan';
	var $formBody = $('#bulk-edit-forms .form-body');
	
	if(typeof args !== "undefined") {
		if(typeof args.default_data !== "undefined") {
			if(typeof args.default_data.lessonPlanNID !== "undefined") {
				lessonPlanNID = args.default_data.lessonPlanNID;
				lessonPlanTitle = args.default_data.lessonPlanTitle;
				lessonPlanBody = args.default_data.lessonPlanBody;
				dialogTitle = 'Editing Lesson Plan';
			}
		}
		if(typeof args.initiator !== "undefined") {
			initiator = args.initiator;
		}
	}
	
	//remove errors
	$('.error', $formBody).hide();
	$('.duplicate-title-error-wrapper', $formBody).removeClass('duplicate-title-error-wrapper');
	$('.title-duplicate-error', $formBody).hide();
	
	var $dialogContent = $('.dialog-content', Drupal.uportal.bulkEditor.$formsWrapper);
	$dialogContent.tabs( "option", "active", 2 );
	$('.choose-lesson-plans-div', Drupal.uportal.bulkEditor.$formsWrapper).hide();
	$('.create-lesson-plan-div', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('textarea#lesson-plan-form-description', Drupal.uportal.bulkEditor.$formsWrapper).val(lessonPlanBody);
	$('input#lesson-plan-nid', Drupal.uportal.bulkEditor.$formsWrapper).val(lessonPlanNID);
	$('input#lesson-plan-form-title', Drupal.uportal.bulkEditor.$formsWrapper).val(lessonPlanTitle);
	$('.create-lesson-plan-div input#initiator', Drupal.uportal.bulkEditor.$formsWrapper).val(initiator);
	$('.dialog-header', Drupal.uportal.bulkEditor.$formsWrapper).text(dialogTitle);
	$('ul.form-nav', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
}
/** ENDS *****************************************/


/**
 * show CHOOSE TAGS form
 */
Drupal.behaviors.uportal.bulkEditor.showChooseTagsForm = function(args) {
	
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	
	Drupal.behaviors.uportal.bulkEditor.updateBulkEditNodesTitle();
	
	//remove errors
	var $formBodies = $('.form-body', $formsWrapper);
	$('.error', $formBodies).hide();
	$('.duplicate-title-error-wrapper', $formBodies).removeClass('duplicate-title-error-wrapper');
	$('.title-duplicate-error', $formBodies).hide();
	
	var $dialogContent = $('.dialog-content', $formsWrapper);
	var $chooseTagsDiv = $('div#choose-tags-form', $formsWrapper);
	$dialogContent.tabs( "option", "active", 3 );
	$chooseTagsDiv.show();
	$('ul.form-nav', $formsWrapper).slideDown();
	
	//uncheck remove old lesson plans checkbox
	$('input#replace-tags', $chooseTagsDiv).attr('checked', false);
	
	//select tids
	var selectedTIDs = (args.tids !== undefined) ? args.tids : [];
	
	var $tagsSelect = $('select#tags', $chooseTagsDiv);
	$tagsSelect.val(selectedTIDs).trigger("change").trigger("chosen:updated");
	
	$.blockUI({
		message: $formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
	
}
/** ENDS *****************************************/


/**
 * show CHOOSE LESSON PLAN form
 */
Drupal.behaviors.uportal.bulkEditor.showChooseLessonPlanForm = function(args) {
	
	var lessonPlanNID = '';
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	
	if(typeof args !== "undefined") {
		if(typeof args.lessonPlanNID !== "undefined") {
			lessonPlanNID = args.lessonPlanNID;
		}
	}
	
	Drupal.behaviors.uportal.bulkEditor.updateBulkEditNodesTitle();
	
	//remove errors
	var $formBodies = $('.form-body', $formsWrapper);
	$('.error', $formBodies).hide();
	$('.duplicate-title-error-wrapper', $formBodies).removeClass('duplicate-title-error-wrapper');
	$('.title-duplicate-error', $formBodies).hide();
	
	var $dialogContent = $('.dialog-content', $formsWrapper);
	var $chooseLessonPlanDiv = $('div#choose-lesson-plans-form', $formsWrapper);
	$dialogContent.tabs( "option", "active", 2 );
	$chooseLessonPlanDiv.show();
	$('.create-lesson-plan-div', $formsWrapper).hide();
	$('ul.form-nav', $formsWrapper).slideDown();
	$('#tab-lesson-plans .dialog-footer .choose-lesson-plans-div', $formsWrapper).show();
	$('#tab-lesson-plans .dialog-footer .create-lesson-plan-div', $formsWrapper).hide();
	
	//uncheck remove old lesson plans checkbox
	$('input#replace-original-plans', $chooseLessonPlanDiv).attr('checked', false);
	
	var $lessonPlansSelect = $('select#lesson-plans', $chooseLessonPlanDiv);
	var newValue = $lessonPlansSelect.val();
	if (newValue) {
		newValue.push(lessonPlanNID);
	} else {
		newValue = new Array(lessonPlanNID);
	}
	$lessonPlansSelect.val(newValue).trigger("change").trigger("chosen:updated");
	
	$.blockUI({
		message: $formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
	
}
/** ENDS *****************************************/


/**
 * show CHOOSE SERIES form
 */
Drupal.behaviors.uportal.bulkEditor.showChooseSeriesForm = function(args) {
	
	var categoryNID = '';
	var seriesNID = '';
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	
	if(typeof args !== "undefined") {
		if(typeof args.categoryNID !== "undefined") {
			categoryNID = args.categoryNID;
		}
		if(typeof args.seriesNID !== "undefined") {
			seriesNID = args.seriesNID;
		}
	}
	
	Drupal.behaviors.uportal.bulkEditor.updateBulkEditNodesTitle();
	
	//remove errors
	var $formBodies = $('.form-body', $formsWrapper);
	$('.error', $formBodies).hide();
	$('.duplicate-title-error-wrapper', $formBodies).removeClass('duplicate-title-error-wrapper');
	$('.title-duplicate-error', $formBodies).hide();
	
	var $dialogContent = $('.dialog-content', $formsWrapper);
	var $chooseSeriesDiv = $('div#choose-series-form', $formsWrapper);
	$dialogContent.tabs( "option", "active", 1 );
	$chooseSeriesDiv.show();
	$('.create-series-div', $formsWrapper).hide();
	$('ul.form-nav', $formsWrapper).slideDown();
	$('#tab-series .dialog-footer .choose-series-div', $formsWrapper).show();
	$('#tab-series .dialog-footer .create-series-div', $formsWrapper).hide();
	
	$('select.category-select', $chooseSeriesDiv).val(categoryNID).trigger("change").trigger("chosen:updated");
	$('select#series', $chooseSeriesDiv).val(seriesNID).trigger("change").trigger("chosen:updated");
	
	$.blockUI({
		message: $formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
	
}
/** ENDS *****************************************/


/**
 * show CREATE SERIES form
 */
Drupal.behaviors.uportal.bulkEditor.showCreateEditSeriesForm = function(args) {
	
	var defaultCategoryNID = '';
	var seriesNID = '';
	var initiator = '';
	var seriesTitle = '';
	var seriesBody = '';
	var dialogTitle = 'Create a Series';
	var $formBody = $('#bulk-edit-forms .form-body');
	$('div.edit-series-warning-msg', $formBody).hide();
	
	if(typeof args !== "undefined") {
		if(typeof args.default_data !== "undefined") {
			defaultCategoryNID = args.default_data.categoryNID;
			if(typeof args.default_data.seriesNID !== "undefined") {
				seriesNID = args.default_data.seriesNID;
				seriesTitle = args.default_data.seriesTitle;
				seriesBody = args.default_data.seriesBody;
				dialogTitle = 'Editing Series';
				$('div.edit-series-warning-msg', $formBody).show();
			}
		}
		if(typeof args.initiator !== "undefined") {
			initiator = args.initiator;
		}
	}
	
	//remove errors
	$('.error', $formBody).hide();
	$('.duplicate-title-error-wrapper', $formBody).removeClass('duplicate-title-error-wrapper');
	$('.title-duplicate-error', $formBody).hide();
	
	var $dialogContent = $('.dialog-content', Drupal.uportal.bulkEditor.$formsWrapper);
	$dialogContent.tabs( "option", "active", 1 );
	$('.choose-series-div', Drupal.uportal.bulkEditor.$formsWrapper).hide();
	$('.create-series-div', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('textarea#series-form-description', Drupal.uportal.bulkEditor.$formsWrapper).val(seriesBody);
	$('input#series-form-title', Drupal.uportal.bulkEditor.$formsWrapper).val(seriesTitle);
	$('input#series-nid', Drupal.uportal.bulkEditor.$formsWrapper).val(seriesNID);
	$('.create-series-div input#initiator', Drupal.uportal.bulkEditor.$formsWrapper).val(initiator);
	$('select#series-form-category', Drupal.uportal.bulkEditor.$formsWrapper).val(defaultCategoryNID).trigger("chosen:updated");
	$('.dialog-header', Drupal.uportal.bulkEditor.$formsWrapper).text(dialogTitle);
	$('ul.form-nav', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	
	$.blockUI({ 
		message: Drupal.uportal.bulkEditor.$formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * submit create lesson plan form
 */
Drupal.behaviors.uportal.bulkEditor.submitCreateEditLessonPlanForm = function() {
	var $createLessonPlanForm = $('#create-lesson-plan-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var lessonPlanTitle = $('input#lesson-plan-form-title', $createLessonPlanForm).val();
	var lessonPlanNID = $('input#lesson-plan-nid', $createLessonPlanForm).val();
	var lessonPlanDesc = $.trim($('textarea#lesson-plan-form-description', $createLessonPlanForm).val());
	var initiator = $('input#initiator', $createLessonPlanForm).val();
	var waitingDialog = {
		'title': 'Create a Lesson Plan',
		'body': 'Please wait as we create the new lesson plan ...'
	};
	
	//check for empty lesson plan title
	if ($.trim(lessonPlanTitle)=='') {
		$('div#tab-lesson-plans .error', Drupal.uportal.bulkEditor.$formsWrapper).text('Please enter a title for the lesson plan to continue ...').slideDown();
		return false;
	} else {
		$('div#tab-lesson-plans .error', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	}
	
	//check for existing lesson plan
	var $titleFormItem = $('.form-item-lesson-plan-title', $createLessonPlanForm);
	var $titleDuplicateError = $('div.title-duplicate-error', $createLessonPlanForm);
	
	//check if current title matches with current NID, in which case it is not a duplicate title
	//compare strings
	var titleExists = Drupal.behaviors.uportal.checkStringExistsInArray(lessonPlanTitle, Drupal.uportal.bulkEditor.existingLessonPlans);
	if (parseInt(lessonPlanNID)>0) {
		var originalTitleForThisNID = Drupal.uportal.bulkEditor.existingLessonPlans['nid-'+lessonPlanNID];
		if (Drupal.behaviors.uportal.stringsAreTheSame(lessonPlanTitle, originalTitleForThisNID)) {
			titleExists = false;
		}
		waitingDialog = {
			'title': 'Editing Lesson Plan',
			'body': 'Please wait as we edit the lesson plan ...'
		};
	}
	
	if (titleExists) {
		$titleDuplicateError.fadeIn();
		$titleFormItem.addClass('duplicate-title-error-wrapper');
		return false;
	} else {
		$titleDuplicateError.hide();
		$titleFormItem.removeClass('duplicate-title-error-wrapper');
	}
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage(waitingDialog.title, waitingDialog.body);
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverLessonPlansURL, {
		'operation_type' : 'bulk-edit-new-lesson-plan',
		'edit-data' : {
			'body' : lessonPlanDesc,
			'title' : lessonPlanTitle,
			'lesson_plan_nid' : lessonPlanNID,
			'initiator' : initiator
		}
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorCreatedNewLessonPlan', data.data_values);
			Drupal.uportal.$eventWatcher.trigger('created-new-metadata', data.data_values);
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * submit create series form
 */
Drupal.behaviors.uportal.bulkEditor.submitCreateEditSeriesForm = function() {
	var $createSeriesForm = $('#create-series-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var seriesCategory = $('select#series-form-category', $createSeriesForm).val();
	var seriesTitle = $('input#series-form-title', $createSeriesForm).val();
	var seriesNID = $('input#series-nid', $createSeriesForm).val();
	var seriesDesc = $.trim($('textarea#series-form-description', $createSeriesForm).val());
	var initiator = $('input#initiator', $createSeriesForm).val();
	var waitingDialog = {
		'title': 'Creating new Series',
		'body': 'Please wait as we create the new series ...'
	};
	
	//check for empty category or empty title
	if (seriesCategory=='' || $.trim(seriesTitle)=='') {
		$('div#tab-series .error', Drupal.uportal.bulkEditor.$formsWrapper).text('Please select a category and enter a series title to continue ...').slideDown();
		return false;
	} else {
		$('div#tab-series .error', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	}
	
	//check for existing series
	var $seriesFormItem = $('.form-item-series-title', $createSeriesForm);
	var $seriesTitleDuplicateError = $('div.title-duplicate-error', $createSeriesForm);
	
	//check if current title matches with current NID, in which case it is not a duplicate title
	//compare strings
	var seriesTitleExists = Drupal.behaviors.uportal.checkStringExistsInArray(seriesTitle, Drupal.uportal.bulkEditor.existingSeries);
	if (parseInt(seriesNID)>0) {
		var originalTitleForThisNID = Drupal.uportal.bulkEditor.existingSeries['nid-'+seriesNID];
		if (Drupal.behaviors.uportal.stringsAreTheSame(seriesTitle, originalTitleForThisNID)) {
			seriesTitleExists = false;
		}
		waitingDialog = {
			'title': 'Editing Series',
			'body': 'Please wait as we edit the series ...'
		};
	}
	
	if (seriesTitleExists) {
		$seriesTitleDuplicateError.fadeIn();
		$seriesFormItem.addClass('duplicate-title-error-wrapper');
		return false;
	} else {
		$seriesTitleDuplicateError.hide();
		$seriesFormItem.removeClass('duplicate-title-error-wrapper');
	}
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage(waitingDialog.title, waitingDialog.body);
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverSeriesURL, {
		'operation_type' : 'bulk-edit-new-series',
		'edit-data' : {
			'body' : seriesDesc,
			'title' : seriesTitle,
			'category' : seriesCategory,
			'initiator' : initiator,
			'series_nid' : seriesNID,
			'current-url' : window.location.pathname + window.location.search + window.location.hash,
		}
	})
		.done( function (data) {
			if (data.data_values.perform_server_bulk_update) {
				window.location.href = window.location.protocol
					+ '//'
					+ window.location.host
					+ '/management/batch-update';
			} else {
				Drupal.uportal.$eventWatcher.trigger('bulkEditorCreatedNewSeries', data.data_values);
			}
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * submit choose tags form
 */
Drupal.behaviors.uportal.bulkEditor.submitChooseTagsForm = function() {
	var $chooseTagsForm = $('#choose-tags-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var chosenTags = $('select#tags', $chooseTagsForm).val();
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	var removeOldTags = $('input#replace-tags', $chooseTagsForm).is(":checked");
	
	//check and validate
	if (!chosenTags || chosenTags=='') {
		$('div#tab-tags .error', Drupal.uportal.bulkEditor.$formsWrapper).text('Please select at least one tag to continue ...').slideDown();
		return false;
	} else {
		$('div#tab-tags .error', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	}
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage('Updating tags for '+noOfSelectedItems+' items', 'Please wait as we add tags to these items ...');
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverURL, {
		'operation_type' : 'bulk-edit-tags',
		'edit-data' : {
			'nids' : Drupal.uportal.bulkEditor.selectedNIDs,
			'tags' : chosenTags,
			'remove-old-tags' : removeOldTags
		}
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorEditedNodeTags', data);
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * submit choose series form
 */
Drupal.behaviors.uportal.bulkEditor.submitChooseLessonPlansForm = function() {
	var $chooseLessonPlansForm = $('#choose-lesson-plans-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var chosenLessonPlans = $('select#lesson-plans', $chooseLessonPlansForm).val();
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	var removeOldLessonPlans = $('input#replace-original-plans', $chooseLessonPlansForm).is(":checked");
	
	//check and validate
	if (!chosenLessonPlans || chosenLessonPlans=='') {
		$('div#tab-lesson-plans .error', Drupal.uportal.bulkEditor.$formsWrapper).text('Please select at least one lesson plan to continue ...').slideDown();
		return false;
	} else {
		$('div#tab-lesson-plans .error', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	}
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage('Updating lesson plan information for '+noOfSelectedItems+' items', 'Please wait as we edit the lesson plan information for these items ...');
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverURL, {
		'operation_type' : 'bulk-edit-lesson-plans',
		'edit-data' : {
			'nids' : Drupal.uportal.bulkEditor.selectedNIDs,
			'lesson-plans' : chosenLessonPlans,
			'remove-old-lesson-plans' : removeOldLessonPlans
		}
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorEditedNodeLessonPlans', data);
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * submit choose series form
 */
Drupal.behaviors.uportal.bulkEditor.submitChooseSeriesForm = function() {
	var $chooseSeriesForm = $('#choose-series-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var chosenSeries = parseInt($('select#series', $chooseSeriesForm).val());
	var chosenCategory = $('select#category-choose-series', $chooseSeriesForm).val();
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	
	//if both are empty, error!
	if (
		(isNaN(chosenSeries) || chosenSeries <= 0)
		&& (isNaN(chosenCategory) || chosenCategory <= 0)
	) {
		$('div#tab-series .error', Drupal.uportal.bulkEditor.$formsWrapper).text('Please select a category and/or series before saving.').slideDown();
		return false;
	}
	
	//hide error div
	$('div#tab-series .error', Drupal.uportal.bulkEditor.$formsWrapper).slideUp();
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage('Updating series and category information for '+noOfSelectedItems+' items', 'Please wait as we edit the series and category information for these items ...');
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverURL, {
		'operation_type' : 'bulk-edit-series',
		'edit-data' : {
			'nids' : Drupal.uportal.bulkEditor.selectedNIDs,
			'category' : chosenCategory,
			'series' : chosenSeries
		}
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorEditedNodeSeries', data);
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * submit meta data form
 */
Drupal.behaviors.uportal.bulkEditor.submitMetaDataForm = function() {
	var $metaDataForm = $('.metadata-form', Drupal.uportal.bulkEditor.$formsWrapper);
	var chosenContentProvider = parseInt($('select#content-provider', $metaDataForm).val());
	var chosenLanguage = $('select#language', $metaDataForm).val();
	var chosenCategory = parseInt($('select#category', $metaDataForm).val());
	var noOfSelectedItems = Drupal.uportal.bulkEditor.selectedNIDs.length;
	
	//if not integer, return
	if (
		(isNaN(chosenContentProvider) || chosenContentProvider<=0)
		&& (isNaN(chosenCategory) || chosenCategory<=0)
		&& (chosenLanguage=='' || chosenLanguage=='_none')
	){
		$('.error', $metaDataForm).text('Please select a content provider or language before saving.').slideDown();
		return false;
	}
	
	//hide error div
	$('div#tab-metadata .error', $metaDataForm).slideUp();
	
	//start posting data by showing progress message
	Drupal.behaviors.uportal.blockUIWaitMessage('Updating metadata for '+noOfSelectedItems+' items', 'Please wait as we edit the metadata ...');
	
	//post data to URL
	$.post(Drupal.uportal.bulkEditor.serverURL, {
		'operation_type' : 'bulk-edit-metadata',
		'edit-data' : {
			'nids' : Drupal.uportal.bulkEditor.selectedNIDs,
			'content-provider' : chosenContentProvider,
			'language' : chosenLanguage,
			'category' : chosenCategory
		}
	})
		.done( function (data) {
			Drupal.uportal.$eventWatcher.trigger('bulkEditorEditedNodeMetaData', data);
		});
		
	//disable click
	return false;
};
/** ENDS *****************************************/


/**
 * fill default values in forms
 */
Drupal.behaviors.uportal.bulkEditor.fillFormDefaultValues = function(args) {
	
	//meta data form
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	var $metaDataForm = $('.metadata-form', $formsWrapper);
	
	//fill original values
	Drupal.behaviors.uportal.bulkEditor.restoreMetaDataFormState();
	
	//content provider
	if (
		typeof args !== "undefined"
		&& typeof args.data !== "undefined"
		&& typeof args.data['content-provider'] !== "undefined"
	) {
		$('select#content-provider', $metaDataForm).val(args.data['content-provider']).trigger("change").trigger("chosen:updated");
	}
	
	//language
	if (
		typeof args !== "undefined"
		&& typeof args.data !== "undefined"
		&& typeof args.data['language'] !== "undefined"
	) {
		$('select#language', $metaDataForm).val(args.data['language']).trigger("change").trigger("chosen:updated");
	}
	
	//category
	if (
		typeof args !== "undefined"
		&& typeof args.data !== "undefined"
		&& typeof args.data['category'] !== "undefined"
	) {
		$('select#'+Drupal.uportal.bulkEditor.initiatingCategorySelectID, Drupal.uportal.bulkEditor.$formsWrapper)
		.val(args.data['category'])
		.trigger("change")
		.trigger("chosen:updated");
		
		//change to choose series form
		if (Drupal.uportal.bulkEditor.initiatingCategorySelectID=='category-choose-series') {
			$('ul.form-nav li.add-to-series  a', $formsWrapper).click();
			$('.form-body #choose-series-form', $formsWrapper).show();
			$('.form-body #create-series-form', $formsWrapper).hide();
			$('.dialog-footer .create-series-div', $formsWrapper).hide();
			$('.dialog-footer .choose-series-div', $formsWrapper).show();
		}
		
		//change to create series form
		if (Drupal.uportal.bulkEditor.initiatingCategorySelectID == 'series-form-category') {
			$('ul.form-nav li.add-to-series  a', $formsWrapper).click();
			$('ul.form-nav', $formsWrapper).hide();
			$('.form-body #choose-series-form', $formsWrapper).hide();
			$('.form-body #create-series-form', $formsWrapper).show();
			$('.dialog-footer .create-series-div', $formsWrapper).show();
			$('.dialog-footer .choose-series-div', $formsWrapper).hide();
		}
	}
}
/** ENDS *****************************************/


/**
 * reset meta data form
 */
Drupal.behaviors.uportal.bulkEditor.resetForms = function() {
	
	//meta data form
	var $metaDataForm = $('.metadata-form', Drupal.uportal.bulkEditor.$formsWrapper);
	
	//content provider
	var defaultProvider = '';
	if (Drupal.uportal.bulkEditor.pageContentProviderNID!='all') {
		defaultProvider = Drupal.uportal.bulkEditor.pageContentProviderNID;
	}
	$('select#content-provider', $metaDataForm).val(defaultProvider).trigger("change").trigger("chosen:updated");
	
	//language
	var defaultLanguage = '';
	if (Drupal.uportal.bulkEditor.pageLanguageCode!='all') {
		defaultLanguage = Drupal.uportal.bulkEditor.pageLanguageCode;
	}
	$('select#language', $metaDataForm).val(defaultLanguage).trigger("change").trigger("chosen:updated");
	
	//category
	var defaultCategory = '';
	if (Drupal.uportal.bulkEditor.pageCategoryNID!='all') {
		defaultCategory = Drupal.uportal.bulkEditor.pageCategoryNID;
	}
	$('select.category-select', Drupal.uportal.bulkEditor.$formsWrapper).val(defaultCategory).trigger("change").trigger("chosen:updated");
	
	//node status
	$('div#change-node-status', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('div#change-node-status-operations-feedback', Drupal.uportal.bulkEditor.$formsWrapper).attr('class', '').hide();
	$('.node-status-form .dialog-footer a.btn', Drupal.uportal.bulkEditor.$formsWrapper).removeClass('disabled').text('Cancel');
	
	//tags
	$('select#tags', Drupal.uportal.bulkEditor.$formsWrapper).val([]).trigger("change").trigger("chosen:updated");
	
	//error box
	$('.error', Drupal.uportal.bulkEditor.$formsWrapper).hide();
	
	//choose series, hide create series, show choose series
	$('.choose-series-div', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('.create-series-div', Drupal.uportal.bulkEditor.$formsWrapper).hide();
	$('select#series', Drupal.uportal.bulkEditor.$formsWrapper).val('').trigger("chosen:updated");
	
	//choose lesson plans, hide create lesson plan, show choose lesson plan
	$('.choose-lesson-plans-div', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('.create-lesson-plan-div', Drupal.uportal.bulkEditor.$formsWrapper).hide();
	$('input#replace-original-plans', Drupal.uportal.bulkEditor.$formsWrapper).attr('checked', false);
	$('select#lesson-plans', Drupal.uportal.bulkEditor.$formsWrapper).val('').trigger("chosen:updated");
	
	//show tabs nav and first tab
	$('ul.form-nav', Drupal.uportal.bulkEditor.$formsWrapper).show();
	$('ul.form-nav li.first a', Drupal.uportal.bulkEditor.$formsWrapper).click();
}
/** ENDS *****************************************/


/**
 * fill form select values
 */
Drupal.behaviors.uportal.bulkEditor.fillFormSelectValues = function (data) {
	var selectOptions = data.data;
	var $dialogsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	var $select;
	
	switch (data.type) {
		case 'content-providers':
			$select = $('select#content-provider', $dialogsWrapper);
			break;
		case 'languages':
			$select = $('select#language', $dialogsWrapper);
			break;
		case 'categories':
			$select = $('select.category-select', $dialogsWrapper);
			break;
		case 'series':
			$select = $('select#series', $dialogsWrapper);
			break;
		case 'lesson-plans':
			$select = $('select#lesson-plans', $dialogsWrapper);
			break;
	}
	
	if ($select) {
		$.each(data.data, function (index, selectOption) {
			if (
				(data.type=='categories' && selectOption.id==Drupal.uportal.bulkEditor.pageCategoryNID)
				|| (data.type=='content-providers' && selectOption.id==Drupal.uportal.bulkEditor.pageContentProviderNID)
				|| (data.type=='languages' && selectOption.id==Drupal.uportal.bulkEditor.pageLanguageCode)
			) {
				$select.append('<option value="'+selectOption.id+'" selected="selected">'+selectOption.title+'</option>');
			} else {
				$select.append('<option value="'+selectOption.id+'">'+selectOption.title+'</option>');
			}
		});
		$select.chosen();
	}
	
};
/** ENDS *****************************************/


/**
 * chain category and series selects together
 */
Drupal.behaviors.uportal.bulkEditor.prepareCategorySeriesSelects = function() {
	
	var $categorySelect = $('select#category-choose-series', Drupal.uportal.bulkEditor.$formsWrapper);
	var $seriesSelect = $('select#series', Drupal.uportal.bulkEditor.$formsWrapper);
	
	//when category select changes
	$categorySelect.on('change', function(event, params) {
		var categoryNID = $categorySelect.val();
		var categoryObj = Drupal.uportal.bulkEditor.seriesByCategory['nid-'+categoryNID];
		$seriesSelect.html('<option value="_none">- None -</option>');
		if (typeof categoryObj != 'undefined') {
			var childSeriesItems = categoryObj.series;
			$.each(childSeriesItems, function(index, series) {
				$seriesSelect.append('<option value="'+series.nid+'">'+series.series_title+'</option>');
			});
			//$createNewSeriesBtn.removeClass('disabled');
		} else {
			//$createNewSeriesBtn.addClass('disabled');
		}
		$seriesSelect.trigger('chosen:updated');
		$seriesSelect.trigger('change');
	});
	
	//when series select changes
	$seriesSelect.on('change', function(event, params) {
		var seriesNID = $seriesSelect.val();
		if (seriesNID=='_none') {
			//$orderSeriesBtn.addClass('disabled');
		} else {
			//$orderSeriesBtn.removeClass('disabled');
		}
	});
	
	//initiate selects
	$categorySelect.trigger('change');
	
	//check for original value
	var allCategories = Drupal.uportal.bulkEditor.seriesByCategory;
	var currentCategoryNID = $categorySelect.val();
	if (typeof allCategories['nid-'+currentCategoryNID] !== "undefined") {
		var categorySeries = allCategories['nid-'+currentCategoryNID]['series'];
		if (typeof categorySeries['nid-'+originalSeriesNID] !== "undefined") {
			$seriesSelect.val(originalSeriesNID);
			$seriesSelect.trigger('chosen:updated');
			$seriesSelect.trigger('change');
		}
	}
	
	$seriesSelect.chosen();
	
}
/** ENDS *****************************************/


/**
 * new category added, update everyone
 */
Drupal.behaviors.uportal.bulkEditor.newCategoryAdded = function(event, data) {
	
	var categoryNID = data.nid;
	var categoryTitle = data.title;
	var initiator = data.initiator;
	var args = {
		data: {}
	};
	
	//default new data
	args.data['category'] = categoryNID;

	//adding new category to array
	Drupal.uportal.bulkEditor.seriesByCategory['nid-'+categoryNID] = {
		nid: categoryNID,
		category_title: categoryTitle,
		series: {}
	};

	$('select.category-select', Drupal.uportal.bulkEditor.$formsWrapper).append('<option value="'+categoryNID+'">'+categoryTitle+'</option>');
	$('select.category-select', Drupal.uportal.bulkEditor.$formsWrapper).trigger('chosen:updated');
	
	//block ui with form if initiated by bulk-editor
	if (initiator=='bulk-editor') {
		var initiatingCategorySelectID = Drupal.uportal.bulkEditor.initiatingCategorySelectID;
		Drupal.behaviors.uportal.bulkEditor.bulkEditNodes(args);
	}
	
};
/** ENDS *****************************************/


/**
 * new series added
 */
Drupal.behaviors.uportal.bulkEditor.newLessonPlanAdded = function(event, data) {
	
	var lessonPlanNID = data.nid;
	var lessonPlanTitle = data.title;
	var initiator = data.initiator;
	
	//adding new lesson plan to array
	Drupal.uportal.bulkEditor.existingLessonPlans['nid-'+lessonPlanNID] = lessonPlanTitle;
	
	//add option to select
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	var $chooseLessonPlanDiv = $('div#choose-lesson-plans-form', $formsWrapper);
	var $lessonPlansSelect = $('select#lesson-plans', $chooseLessonPlanDiv);
	$lessonPlansSelect.append('<option value="'+lessonPlanNID+'">'+lessonPlanTitle+'</option>').trigger("chosen:updated");
	
	//select lesson plan
	if (initiator=='bulk-editor') {
		//back to form
		Drupal.behaviors.uportal.bulkEditor.showChooseLessonPlanForm({
			'lessonPlanNID': lessonPlanNID
		});
	}
	
};
/** ENDS *****************************************/


/**
 * new series added
 */
Drupal.behaviors.uportal.bulkEditor.newTagAdded = function(event, data) {
	
	var tid = data.tid;
	var term_name = data.name;
	var vocab = Drupal.uportal.managingMetaDataTags.existingVocabularies['vid-'+data.vid];
	
	//add option to select
	var $formsWrapper = Drupal.uportal.bulkEditor.$formsWrapper;
	var $tagsSelect = $('select#tags', $formsWrapper);
	$tagsSelect.append('<option value="'+tid+'">'+term_name+' ('+vocab.name+')</option>').trigger("chosen:updated");
	
	//show choose tags
	Drupal.behaviors.uportal.bulkEditor.showChooseTagsForm({tids: [tid]});
};
/** ENDS *****************************************/


/**
 * new series added
 */
Drupal.behaviors.uportal.bulkEditor.newSeriesAdded = function(event, data) {
	
	var seriesNID = data.nid;
	var parentCategoryNID = data.category_nid;
	var seriesTitle = data.title;
	var initiator = data.initiator;
	var edit_operation = (data.edit_operation=='1'); //0 or 1
	var old_series_title = data.old_series_title;
	
	if (edit_operation) {
		
		//remove old value from array
		delete Drupal.uportal.bulkEditor.existingSeries['nid-'+seriesNID];
		
		//unset from seriesByCategory
		delete Drupal.uportal.bulkEditor.seriesByCategory['nid-'+parentCategoryNID]['series']['nid-'+seriesNID];
		
	}
	
	//adding new series to array
	Drupal.uportal.bulkEditor.existingSeries['nid-'+seriesNID] = seriesTitle;
	Drupal.uportal.bulkEditor.seriesByCategory['nid-'+parentCategoryNID]['series']['nid-'+seriesNID] = {
		nid: seriesNID,
		series_title: seriesTitle
	};
	
	//back to form
	if (initiator=='bulk-editor') {
		
		//make series selected and update category
		Drupal.behaviors.uportal.bulkEditor.showChooseSeriesForm({
			'categoryNID': parentCategoryNID,
			'seriesNID': seriesNID
		});
	}
	
};
/** ENDS *****************************************/


/**
 * get existing series and lesson plans
 */
Drupal.behaviors.uportal.bulkEditor.getExistingSeriesAndLessonPlans = function() {
	$.post(Drupal.uportal.bulkEditor.serverReadURL, {
		'operation_type' : 'get-existing-series-lesson-plans'
	}).done( function(data) {
		
		//series
		$.each(data.series, function(index, seriesName) {
			Drupal.uportal.bulkEditor.existingSeries[index] = seriesName;
		});
		
		//lesson plans
		$.each(data.lesson_plans, function(index, lessonPlanName) {
			Drupal.uportal.bulkEditor.existingLessonPlans[index] = lessonPlanName;
		});
		
	});
};
/** ENDS *****************************************/


/**
 * new metadata created
 */
Drupal.behaviors.uportal.bulkEditor.newMetaDataAdded = function(event, data) {
	//initiate variables
	var id = data.nid;
	var title = data.title;
	var initiator = data.initiator;
	var selectWrapper;
	var args = {
		data: {}
	};
	
	//default new data
	args.data[data.meta_data_type] = id;
	
	//get wrapper
	switch (data.meta_data_type) {
		case 'content-provider':
			selectWrapper = 'form-item-content-provider';
			break;
		case 'language':
			selectWrapper = 'form-item-language';
			break;
		default:
			return;
	}
	
	//add to the select and change value
	$('div.'+selectWrapper+' select', Drupal.uportal.bulkEditor.$formsWrapper).each( function(index, elem) {
		var $select = $(elem);
		$select.append('<option value="'+id+'">'+title+'</option>');
	});
	
	//block ui with form if initiated by bulk-editor
	if (initiator=='bulk-editor') {
		Drupal.behaviors.uportal.bulkEditor.bulkEditNodes(args);
	}
}
/** ENDS *****************************************/


/**
 * store metadata form state
 */
Drupal.behaviors.uportal.bulkEditor.storeMetaDataFormState = function() {
	
	var $metadataForm = $('div#tab-metadata', Drupal.uportal.bulkEditor.$formsWrapper);
	var $languageSelect = $('div.form-item-language select', $metadataForm);
	var $providerSelect = $('div.form-item-content-provider select', $metadataForm);
	var $categorySelect = $('div.form-item-category select', $metadataForm);
	
	Drupal.uportal.bulkEditor.metaDataFormState = {
		'language' : $languageSelect.val(),
		'content-provider' : $providerSelect.val(),
		'category' : $categorySelect.val(),
	};
	
}
/** ENDS *****************************************/


/**
 * restore metadata form state
 */
Drupal.behaviors.uportal.bulkEditor.restoreMetaDataFormState = function() {
	
	var $metadataForm = $('div#tab-metadata', Drupal.uportal.bulkEditor.$formsWrapper);
	var $languageSelect = $('div.form-item-language select', $metadataForm);
	var $providerSelect = $('div.form-item-content-provider select', $metadataForm);
	var $categorySelect = $('div.form-item-category select', $metadataForm);
	var formValues = Drupal.uportal.bulkEditor.metaDataFormState;
	
	if (typeof formValues.language === "undefined" || typeof formValues.category === "undefined") {
		return;
	}
	
	$languageSelect.val(formValues.language).trigger("change").trigger("chosen:updated");
	$providerSelect.val(formValues['content-provider']).trigger("change").trigger("chosen:updated");
	
	//category
	var categoryNID = formValues.category;
	if (categoryNID=='' && Drupal.uportal.bulkEditor.pageCategoryNID!='all') {
		categoryNID = Drupal.uportal.bulkEditor.pageCategoryNID;
	}
	$categorySelect.val(categoryNID).trigger("change").trigger("chosen:updated");
	
	Drupal.uportal.bulkEditor.metaDataFormState = {};
	
}
/** ENDS *****************************************/


/**
 * create new category
 */
Drupal.behaviors.uportal.bulkEditor.createNewCategory = function(initiatingSelectID) {
	
	//check user rights
	if (!Drupal.behaviors.uportal.userIsAllowed('manage-categories')) {
		return;
	}
	
	//first store initial state of form
	Drupal.uportal.bulkEditor.initiatingCategorySelectID = initiatingSelectID;
	Drupal.behaviors.uportal.bulkEditor.storeMetaDataFormState();
	
	Drupal.uportal.managingMetaData.$formDiv = $('div#category-dialog-boxes-wrapper div#category-dialog-box');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'category',
		'title' : 'Category',
		'title_plural' : 'Categories'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val('bulk-editor');
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * create new language
 */
Drupal.behaviors.uportal.bulkEditor.createNewLanguage = function() {
	
	//check user rights
	if (!Drupal.behaviors.uportal.userIsAllowed('manage-languages')) {
		return;
	}
	
	//first store initial state of form
	Drupal.behaviors.uportal.bulkEditor.storeMetaDataFormState();
	
	Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#language-form-wrapper');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'language',
		'title' : 'Language',
		'title_plural' : 'Languages'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val('bulk-editor');
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * create new content provider
 */
Drupal.behaviors.uportal.bulkEditor.createNewContentProvider = function($nodeFormWrapper) {
	
	//check user rights
	if (!Drupal.behaviors.uportal.userIsAllowed('manage-content-providers')) {
		return;
	}
	
	//first store initial state of form
	Drupal.behaviors.uportal.bulkEditor.storeMetaDataFormState();
	
	Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#content-provider-form-wrapper');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'content-provider',
		'title' : 'Content Provider',
		'title_plural' : 'Content Providers'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val('bulk-editor');
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * initiate bulk editor
 */
Drupal.behaviors.uportal.bulkEditor.initiateBulkEditor = function () {
	//check if bulk editor is active on this page
	if (
		!($('body').hasClass('bulk-editor-active'))
		|| Drupal.uportal.bulkEditor.initiated
	) {
		return;
	}
	
	//initiate
	Drupal.uportal.bulkEditor.$formsWrapper = $('div#bulk-edit-forms');
	Drupal.uportal.bulkEditor.$dialogsWrapper = $('div#bulk-editor-dialog-boxes');
	Drupal.behaviors.uportal.bulkEditor.initiateBulkEditForms();
	
	//watch for these events
	Drupal.uportal.$eventWatcher.on('created-new-metadata', Drupal.behaviors.uportal.bulkEditor.newMetaDataAdded);
	Drupal.uportal.$eventWatcher.on('created-new-category', Drupal.behaviors.uportal.bulkEditor.newCategoryAdded);
	Drupal.uportal.$eventWatcher.on('bulkEditorCreatedNewSeries', Drupal.behaviors.uportal.bulkEditor.newSeriesAdded);
	Drupal.uportal.$eventWatcher.on('bulkEditorCreatedNewLessonPlan', Drupal.behaviors.uportal.bulkEditor.newLessonPlanAdded);
	Drupal.uportal.$eventWatcher.on('fetched-existing-taxonomy', Drupal.behaviors.uportal.bulkEditor.initiateTagsForm);
	Drupal.uportal.$eventWatcher.on('added-tag', Drupal.behaviors.uportal.bulkEditor.newTagAdded);
	
	//get existing Series and Lesson Plans
	Drupal.behaviors.uportal.bulkEditor.getExistingSeriesAndLessonPlans();
	
	//initiated
	Drupal.uportal.bulkEditor.initiated = true;
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.bulkEditor.initiateBulkEditor);
