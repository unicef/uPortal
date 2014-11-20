
(function($) {

/**
 * globals we need for content listing
 */
Drupal.uportal.editContent = {
	fileIDCounter : 0,
	files : new Object(),
	$nodesWrapper : {},
	drupalFileObjsToCreate : new Array(),
	serverURL : '/management/edit-content/ajax-handler',
	serverReadURL : '/management/edit-content/ajax-handler/read-only',
	nidToDelete: 0,
	categories: new Array(),
	fetchedTaxonomy: false
};


/**
 * initiate expand/collapse sections
 */
Drupal.behaviors.uportal.editContent.initiateExpandCollapse = function() {
	$('.node-header', Drupal.uportal.editContent.$nodesWrapper).click( function() {
		var $nodeHeader = $(this);
		var $nodeWrapperDiv = $nodeHeader.parent('.node-edit-wrapper');
		var $formWrapper = $('.node-form-footer-wrapper', $nodeWrapperDiv);
		var nid = $nodeWrapperDiv.data('nid');
		var $form = $('form', $formWrapper);
		
		if ($nodeWrapperDiv.hasClass('form-collapsed')) {
			Drupal.behaviors.uportal.editContent.loadNodeForm(nid, $nodeWrapperDiv);
		} else {
			Drupal.behaviors.uportal.editContent.collapseNodeForm($nodeWrapperDiv, true);
		}
	});
}
/** ENDS *****************************************/


/**
 * collapse the form
 */
Drupal.behaviors.uportal.editContent.collapseNodeForm = function($nodeWrapperDiv, checkForm) {
	
	if ($nodeWrapperDiv.hasClass('form-collapsed')) {
		return;
	}
	
	var $formWrapper = $('.node-form-footer-wrapper', $nodeWrapperDiv);
	var nid = $nodeWrapperDiv.data('nid');
	var $form = $('form', $formWrapper);
	var $nodeHeader = $('.node-header', $nodeWrapperDiv);
	
	if (checkForm && $form.hasClass('dirty')) {
		if (!confirm("This form has unsaved changes. Click Cancel to go back and save the changes. Click OK to discard and continue.")) {
			return;
		}
	}
	$nodeWrapperDiv.removeClass('form-is-dirty');
	$nodeWrapperDiv.addClass('form-collapsed');
	$formWrapper.stop().slideUp(700, function() {
		$('#node-form-wrapper-nid-'+nid, $formWrapper).html('');
		$('html, body').animate({
			scrollTop: ($nodeHeader.offset().top - 150)
		},300);
		//TODO also stop any XHR if active
	});
};
/** ENDS *****************************************/


/**
 * load node form
 */
Drupal.behaviors.uportal.editContent.loadNodeForm = function(nid, $nodeWrapperDiv) {
	var $formWrapper = $('.node-form-footer-wrapper', $nodeWrapperDiv);
	$('#node-form-wrapper-nid-'+nid, $formWrapper).html('<div class="loading-form"><div class="img"></div><div class="loading-msg">Loading edit form ...</div></div>');
	$.post(Drupal.uportal.editContent.serverURL, {
		'operation_type' : 'get_node_form',
		'data_values' : {
			'nid' : nid
		}
	}).done( function(data) {
			var $nodeFormWrapper = $('#node-form-wrapper-nid-'+data.nid, $formWrapper);
			$nodeFormWrapper.html(data.form_html);
			Drupal.attachBehaviors();
			Drupal.behaviors.uportal.editContent.prepareNodeForm($nodeFormWrapper);
			Drupal.behaviors.uportal.editContent.changeFormStatus($nodeWrapperDiv, 'active');
		});
	
	//prepare divs
	$nodeWrapperDiv.removeClass('form-collapsed');
	$('.form-errors', $nodeWrapperDiv).html('');
	Drupal.behaviors.uportal.editContent.changeFormStatus($nodeWrapperDiv, 'loading');
	$formWrapper.stop().slideDown();
	
}
/** ENDS *****************************************/


/**
 * prepare node form for display and operations
 */
Drupal.behaviors.uportal.editContent.prepareNodeForm = function($nodeFormWrapper) {
	
	//prepare category and series selects
	Drupal.behaviors.uportal.editContent.prepareCategorySeriesSelects($nodeFormWrapper);
	
	//always enable auto URL alias
	var $autoPathCheckbox = $('input#edit-path-pathauto', $nodeFormWrapper).attr('checked', 'checked');
	
	//add new buttons for content provider, language and lesson plans
	Drupal.behaviors.uportal.editContent.prepareMetaDataNewButtons($nodeFormWrapper);
	
	//disable NONE from these multiselects
	Drupal.behaviors.uportal.editContent.disableNoneOption($("select#edit-field-lesson-plan-reference-und", $nodeFormWrapper));
	Drupal.behaviors.uportal.editContent.disableNoneOption($("select.custom-all-tags-select", $nodeFormWrapper));
	
	//instead of disabling _none like above which brings problems with the dynamic Tags field
	//disable _none and set original dirty-form value as _none if empty
	//Drupal.behaviors.uportal.editContent.initialiseMultiSelect($("select#edit-field-lesson-plan-reference-und", $nodeFormWrapper));
	
	//change selects into CHOSENS
	$('select.form-select', $nodeFormWrapper).chosen();
	$('select.form-select', $nodeFormWrapper).on('change', function(evt, params) {
		var $select = $(this);
		if ($select.hasClass('custom-all-tags-select')) {
			Drupal.behaviors.uportal.editContent.updateTaxonomyFields($select);
		}
		$('form', $nodeFormWrapper).trigger('checkform.areYouSure');
	});
	
	//prepare tag select
	Drupal.behaviors.uportal.editContent.setUpFormTagsField($('form', $nodeFormWrapper));
	
	//update all URLs for ajax objects
	$('div.image-widget.form-managed-file input.form-submit', $nodeFormWrapper).each(
		function(index, elem) {
			var id = $(this).attr('id');
			if (typeof Drupal.ajax[id] !== "undefined") {
				var url = "/file/ajax/field_image/und/0/"+$('input[name="form_build_id"]', $nodeFormWrapper).val();
				Drupal.ajax[id]['options']['url'] = url;
				Drupal.ajax[id]['url'] = url;
			}
		}
	);
	
	//watch form using are-you-sure jquery plugin
	$('form', $nodeFormWrapper).areYouSure({
		'message': 'There are unsaved changes in some forms. Do you want to continue without saving?'
	});
	$('form', $nodeFormWrapper).on('dirty.areYouSure', function() {
		var $form = $(this);
		var formNID = $('input#form-nid-value', $form).val();
		var $fullFormWrapper = $('div#node-wrapper-nid-'+formNID, Drupal.uportal.editContent.$nodesWrapper);
		$fullFormWrapper.addClass('form-is-dirty');
	});
	$('form', $nodeFormWrapper).on('clean.areYouSure', function() {
		var $form = $(this);
		var formNID = $('input#form-nid-value', $form).val();
		var $fullFormWrapper = $('div#node-wrapper-nid-'+formNID, Drupal.uportal.editContent.$nodesWrapper);
		$fullFormWrapper.removeClass('form-is-dirty');
	});
	
	//player for video or audio
	var player = false;
	
	//video player
	var nodeType = $nodeFormWrapper.data('node-type');
	var nid = $nodeFormWrapper.data('nid');
	if (nodeType=='video') {
		player = new MediaElementPlayer('#video-player-'+nid, {
			pauseOtherPlayers: false
		});
	} else if (nodeType=='audio') {
		var $audioFileWrapperDiv = $('#audio-file-wrapper-'+nid, $nodeFormWrapper);
		var $audioPlayerWrapper = $("#audio-player-"+nid, $audioFileWrapperDiv);
		var audioSrc = $audioFileWrapperDiv.data('audio-path');
		player = Drupal.behaviors.uportal.editContent.initiateAudioPlayer($audioPlayerWrapper, audioSrc, $audioFileWrapperDiv);
	} else if (nodeType=='document') {
		player = $('#document-thumbnail-'+nid, $nodeFormWrapper);
		player.click( function() {
			var $$ = $(this);
			var documentSrc = '/sites/all/libraries/pdf.js/web/viewer.html?file='+$$.data('document-url');
			$('iframe', $$).attr('src', documentSrc);
			$.blockUI({
				message: $('div.pdf-display-file', $$),
				css: {
					width: '95%',
					height: '90%',
					left: '2.5%',
					top: '5%'
				}
			});
		});
	}
	
	//thumbnail uploader
	Drupal.behaviors.uportal.editContent.initiateThumbnailUploader($nodeFormWrapper);
	
	//content file uploader
	Drupal.behaviors.uportal.editContent.initiateContentFileUploader($nodeFormWrapper, player);
	
}
/** ENDS *****************************************/


/**
 * disable _none option in multiple select
 */
Drupal.behaviors.uportal.editContent.disableNoneOption = function($select) {
	var origVal = $select.val();
	if (!origVal || origVal.length<=0) {
		$("option[value='_none']", $select).attr('selected', 'selected');
	}
	$("option[value='_none']", $select).attr('disabled', 'disabled');
	$select.on('change', function(evt, params) {
		var currVal = $select.val();
		var $noneOption = $("option[value='_none']", $select);
		if (!currVal || currVal.length<=0) {
			$noneOption.attr('selected', 'selected');
			$select.val('_none');
		} else {
			$noneOption.removeAttr('selected');
		}
		$select.trigger('chosen:updated');
		$select.closest('form').trigger('checkform.areYouSure');
	});
};
/** ENDS *****************************************/


/**
 * disable _none option in multiple select
 * set original dirty-form value as _none if empty
 */
Drupal.behaviors.uportal.editContent.initialiseMultiSelect = function($select) {
	var origVal = $select.val();
	
	//disable _none option
	$("option[value='_none']", $select).attr('disabled', 'disabled');
	
	//original value to _none
	if (!origVal || origVal.length<=0) {
		$select.data('ays-orig', '_none');
	}
	
	$select.trigger('chosen:updated');
	
};
/** ENDS *****************************************/


/**
 * initiate audio player
 */
Drupal.behaviors.uportal.editContent.initiateAudioPlayer = function($audioPlayerWrapper, audioURL, $parentWrapper) {
	$.jPlayer.timeFormat.sepSec = " mins";
	var $orangeWaveFormDiv = jQuery('.wave-img-orange', $parentWrapper);
	var $player = $audioPlayerWrapper.jPlayer({
		ready: function () {
			$(this).jPlayer("setMedia", {
				mp3: audioURL
			});
		},
		solution: "html",
		supplied: "mp3",
		smoothPlayBar: false,
		keyEnabled: true,
		errorAlerts: false,
		warningAlerts: false,
		cssSelectorAncestor: '.audio-player'
	});
	$player.bind($.jPlayer.event.timeupdate, function(event) {
		$orangeWaveFormDiv.width(event.jPlayer.status.currentPercentAbsolute+"%");
	});
	return $player;
}
/** ENDS *****************************************/


/**
 * delete content
 * shared functionality
 */
Drupal.behaviors.uportal.editContent.deleteContent = function(nodeTitle, nid, $removeDiv, deleteURL) {
	
	var $confirmationDiv = $('div#confirm-deletion');
	if (typeof deleteURL == "undefined" || !deleteURL) {
		deleteURL = Drupal.uportal.editContent.serverURL;
	}
	
	$('.dialog-header', $confirmationDiv).text('Delete content?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete this content: <em>'+nodeTitle+'</em>?');
	$('.dialog-footer .yes', $confirmationDiv).click( function() {
		
		//lousy fix to lousy code.
		//Somehow this function is called multiple times instead of just once
		//TODO: Fix it!
		if (Drupal.uportal.editContent.nidToDelete != nid) {
			return false;
		}
		
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting content ...', 'Please wait as we delete this content: <em>'+nodeTitle+'</em> ...');
		$.post(deleteURL, {
			'operation_type' : 'delete_node',
			'data_values' : {
				'nid' : nid
			}
		}).done( function(data) {
				$removeDiv.remove();
				
				//if category page
				if (Drupal.uportal.managingMetaData.metaDataType.type && Drupal.uportal.managingMetaData.metaDataType.type=='category') {
					var cnter = 1;
					$('.full-result-row', Drupal.uportal.managingMetaData.$resultsWrapper).each( function(index, elem) {
						var $row = $(elem);
						$('.menu-order', $row).text(cnter++);
					})
				}
				
				$.unblockUI();
			});
		return false;
	});
	
	//block UI with confirm message
	$.blockUI({ 
		message:$confirmationDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
	
	return false;
}
/** ENDS *****************************************/


/**
 * initiate all delete buttons
 */
Drupal.behaviors.uportal.editContent.initiateDeleteButtons = function() {
	$('a.delete-btn', Drupal.uportal.editContent.$nodesWrapper).each(
		function (index, elem) {
			var $btn = $(elem);
			var nid = $btn.data('nid');
			var $nodeDiv = $('div#node-wrapper-nid-'+nid, Drupal.uportal.editContent.$nodesWrapper);
			var nodeTitle = $('.node-header .title span.content-title', $nodeDiv).text();
			$btn.click( function() {
				
				//ensure only one node is deleted
				Drupal.uportal.editContent.nidToDelete = nid;
				
				if($btn.hasClass('disabled')){
					return false;
				}
				
				//block form
				Drupal.behaviors.uportal.editContent.changeFormStatus($nodeDiv, 'potential-delete');
				
				//delete content
				Drupal.behaviors.uportal.editContent.deleteContent(nodeTitle, nid, $nodeDiv);
				
				return false;
			
			});
		}
	);
}
/** ENDS *****************************************/


/**
 * initiate save buttons
 */
Drupal.behaviors.uportal.editContent.initiateSaveButtons = function() {
	
	console.log('save');
	
	$('a.save-draft-btn, a.publish-btn', Drupal.uportal.editContent.$nodesWrapper).click( function() {
		var $btn = $(this);
		var $nodeWrapper = $btn.parents('.node-edit-wrapper');
		var $nodeForm = $('.node-form-wrapper form', $nodeWrapper);
		
		//check if button is not in disabled mode
		if ($btn.hasClass('disabled')) {
			return false;
		}
		
		if ($nodeForm.length>0) {
			
			//initiate saveType
			var saveType = "published";
			if ($btn.hasClass('save-draft-btn')) {
				saveType = "draft";
			}
			
			//validate form
			var formIsValid = Drupal.behaviors.uportal.editContent.validateForm(saveType, $nodeForm);
			if (!formIsValid) {
				return false;
			}
			
			//set published status
			var $publishedCheckBox = $('input#edit-status', $nodeForm);
			var $customStatusInput = $('input#custom-node-status', $nodeForm);
			if (saveType=='published') {
				$publishedCheckBox.attr('checked', true);
				$customStatusInput.val('1');
			}
			if (saveType=='draft') {
				$publishedCheckBox.attr('checked', false);
				$customStatusInput.val('0');
			}
			
			//show saving data div
			$('.saving-form-data', $nodeForm).fadeIn();
			
			//update form state
			Drupal.behaviors.uportal.editContent.changeFormStatus($nodeWrapper, 'saving-form-data');
			
			//click save button failed
			//mousedown on save button failed
			//dug into Drupal Ajax - http://stackoverflow.com/questions/4042535/use-drupal7-ajax-goodness-programmatically
			//$saveFormBtn.trigger('mousedown');
			//$nodeForm.trigger('submit');
			var $saveFormBtn = $('div.node-actions input.save-form-btn', $nodeForm);
			var saveFormBtnID = $saveFormBtn.attr('id');
			var drupalAjaxObj = Drupal.ajax[saveFormBtnID];
			drupalAjaxObj.eventResponse(drupalAjaxObj.element, drupalAjaxObj.event);
		}
		
		return false;
		
	});
}
/** ENDS *****************************************/


/**
 * validate form, for publish and save-draft
 */
Drupal.behaviors.uportal.editContent.validateForm = function(saveType, $nodeForm) {
	var formIsValid = true;
	var formFields = {
		'draft': {
			'title': {
				'$wrapper': $('.form-item-title', $nodeForm),
				'$field': $('.form-item-title input.form-text', $nodeForm),
				'validation': {
					'required': true,
					'maxlength': parseInt($('.form-item-title .character-count-msg span.maximum-no', $nodeForm).text()),
				}
			}
		},
		'published': {
			'title': {
				'$wrapper': $('.form-item-title', $nodeForm),
				'$field': $('.form-item-title input.form-text', $nodeForm),
				'validation': {
					'required': true,
					'maxlength': parseInt($('.form-item-title .character-count-msg span.maximum-no', $nodeForm).text()),
				}
			},
			'body': {
				'$wrapper': $('.field-name-body', $nodeForm),
				'$field': $('.field-name-body textarea', $nodeForm),
				'validation': {
					'required': true,
					'maxlength': parseInt($('.field-name-body .character-count-msg span.maximum-no', $nodeForm).text()),
				}
			},
			'category': {
				'$wrapper': $('.field-name-field-topic-reference', $nodeForm),
				'$field': $('.field-name-field-topic-reference select', $nodeForm),
				'validation': {
					'required': true,
				}
			},
			'content-provider': {
				'$wrapper': $('.field-name-field-content-provider-reference', $nodeForm),
				'$field': $('.field-name-field-content-provider-reference select', $nodeForm),
				'validation': {
					'required': true,
				}
			},
			'language': {
				'$wrapper': $('.form-item-language', $nodeForm),
				'$field': $('.form-item-language select', $nodeForm),
				'validation': {
					'required': true,
				}
			},
		},
	};
	var fieldsToValidate = formFields[saveType];
	
	//first clear all to remove error classes
	$('.form-error', $nodeForm).removeClass('form-error');
	
	
	//run validation
	$.each(fieldsToValidate, function(index, fieldInfo) {
		var fieldValue = fieldInfo.$field.val();
		$.each(fieldInfo.validation, function(validationType, validationValue) {
			//check for required validation
			if (validationType=='required') {
				if (fieldValue=='und' || fieldValue=='_none' || !fieldValue.trim()) {
					formIsValid = false;
					fieldInfo.$wrapper.addClass("form-error required-error");
				} else {
					fieldInfo.$wrapper.removeClass("form-error required-error");
				}
			}
			
			//check for maxlength validation
			var fieldValueLength = fieldValue.length;
			if (validationType=='maxlength' && fieldValueLength>0) {
				var maxLength = validationValue;
				if (fieldValueLength>maxLength) {
					formIsValid = false;
					fieldInfo.$wrapper.addClass('form-error maxlength-error');
				} else {
					fieldInfo.$wrapper.removeClass('form-error maxlength-error');
				}
			}
			
		});
	});
	
	//scroll to first element with error
	if (!formIsValid) {
		$('html, body').animate({
			scrollTop: ($('.form-error', $nodeForm).first().offset().top - 300)
		},500);
	}
	
	return formIsValid;
}
/** ENDS *****************************************/


/**
 * change form status depending on operations
 */
Drupal.behaviors.uportal.editContent.changeFormStatus = function($nodeWrapperDiv, status) {
	var $nodeForm = $('.node-form-wrapper form', $nodeWrapperDiv);
	
	if (status=='active') {
		$('.node-footer .btn', $nodeWrapperDiv).removeClass('disabled');
	} else {
		if (status!='potential-delete') {
			$('.node-footer .btn', $nodeWrapperDiv).addClass('disabled');
		}
	}
	if (status=='saving-form-data') {
		$nodeForm.removeClass('dirty');
	}
}
/** ENDS *****************************************/


/**
 * after saving form
 */
Drupal.ajax.prototype.commands.contentFormAfterSaving =  function(ajax, response, status) {
	//update form block UI to show save complete
	//hide form
	//update title
	//update published or draft status
	
	//hide form
	var $form = ajax.form;
	var nid = $('input.save-form-btn', $form).data('nid');
	var title = $('.form-item-title input', $form).val();
	var nodeStatus = $('.form-item-status input', $form).is(':checked');
	var nodeStatusClass = nodeStatus ? 'status-published' : 'status-draft';
	var nodeStatusText = nodeStatus ? 'Published' : 'Draft';
	
	//update titles
	var $nodeFormWrapper = $('div#node-wrapper-nid-'+nid, Drupal.uportal.editContent.$nodesWrapper);
	var $nodeHeaderDiv = $('.node-header', $nodeFormWrapper);
	$('.title', $nodeHeaderDiv).text(title);
	$('.node-status', $nodeHeaderDiv).text(nodeStatusText);
	$nodeFormWrapper.removeClass('status-draft');
	$nodeFormWrapper.removeClass('form-is-dirty');
	$nodeFormWrapper.removeClass('status-published');
	$nodeFormWrapper.addClass(nodeStatusClass);
	
	//update saving data message
	var $dataSavingLoader = $('.saving-form-data', $form);
	$dataSavingLoader.addClass('data-saved');
	$('.loader-text', $dataSavingLoader).text('Data saved.');
	
	//hide form
	setTimeout(function() {
		if (nodeStatus) {
			$nodeHeaderDiv.click();
		} else {
			Drupal.behaviors.uportal.editContent.loadNodeForm(nid, $nodeFormWrapper);
		}
	}, 700);
	
};
/** ENDS *****************************************/


/**
 * before sending form
 */
Drupal.behaviors.uportal.editContent.beforeSendingForm = function (first, second) {
	// Call Drupal parent method
 	Drupal.ajax.prototype.beforeSend.call(this, first, second);
};
/** ENDS *****************************************/


/**
 * retrieve categories and series information
 */
Drupal.behaviors.uportal.editContent.retrieveCategoriesAndSeries = function() {
	$.post(Drupal.uportal.editContent.serverReadURL, {
		'operation_type' : 'retrieve_categories_and_series',
	}).done( function(data) {
			Drupal.uportal.editContent.categories = data;
		});
};
/** ENDS *****************************************/


/**
 * create add new buttons for metadata
 */
Drupal.behaviors.uportal.editContent.prepareMetaDataNewButtons = function($nodeFormWrapper) {
	
	//new content provider button
	var $contentProviderWrapper = $('div.form-item-field-content-provider-reference-und', $nodeFormWrapper);
	var $createNewContentProviderBtn = $('<div class="create-new-content-provider-btn create-new-btn-icon small-icon-button"></div>').appendTo($contentProviderWrapper);
	if (Drupal.behaviors.uportal.userIsAllowed('manage-content-providers')) {
		$createNewContentProviderBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.createNewContentProvider($nodeFormWrapper);
			}
		});
	}
	
	//new language button
	var $languageWrapper = $('div.form-item-language', $nodeFormWrapper);
	var $createNewLanguageBtn = $('<div class="create-new-language-btn create-new-btn-icon small-icon-button"></div>').appendTo($languageWrapper);
	if (Drupal.behaviors.uportal.userIsAllowed('manage-languages')) {
		$createNewLanguageBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.createNewLanguage($nodeFormWrapper);
			}
		});
	}
	
	//new lesson plan button
	var $lessonPlanWrapper = $('div.form-item-field-lesson-plan-reference-und', $nodeFormWrapper);
	var $createNewLessonPlanBtn = $('<div class="create-new-lesson-plan-btn create-new-btn-icon small-icon-button"></div>').appendTo($lessonPlanWrapper);
	if (Drupal.behaviors.uportal.userIsAllowed('manage-lesson-plans')) {
		$createNewLessonPlanBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.createNewLessonPlan($nodeFormWrapper);
			}
		});
	}
	
}
/** ENDS *****************************************/


/**
 * chain category and series selects together
 */
Drupal.behaviors.uportal.editContent.prepareCategorySeriesSelects = function($nodeFormWrapper) {
	
	//select wrappers
	var $categorySelectWrapper = $('div.form-item-field-topic-reference-und', $nodeFormWrapper);
	var $seriesSelectWrapper = $('div.form-item-field-series-reference-und', $nodeFormWrapper);
	
	//add create new and order series buttons
	var $createNewCategoryBtn = $('<div class="create-new-category-btn create-new-btn-icon small-icon-button"></div>').appendTo($categorySelectWrapper);
	var $createNewSeriesBtn = $('<div class="create-new-series-btn create-new-btn-icon small-icon-button"></div>').appendTo($seriesSelectWrapper);
	var $orderSeriesBtn = $('<div class="order-series-btn order-btn-icon small-icon-button disabled"></div>').appendTo($seriesSelectWrapper);
	
	//button functionality
	if (Drupal.behaviors.uportal.userIsAllowed('manage-categories')) {
		$createNewCategoryBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.createNewCategory($nodeFormWrapper);
			}
		});
	}
	if (Drupal.behaviors.uportal.userIsAllowed('manage-series')) {
		$createNewSeriesBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.createNewSeries($nodeFormWrapper);
			}
		});
		$orderSeriesBtn.click( function() {
			if (!$(this).hasClass('disabled')) {
				Drupal.behaviors.uportal.editContent.orderSeries($nodeFormWrapper);
			}
		});
	}
	
	var $categorySelect = $('select', $categorySelectWrapper);
	var $seriesSelect = $('select', $seriesSelectWrapper);
	var originalSeriesNID = $seriesSelect.val();
	
	//when category select changes
	$categorySelect.on('change', function(event, params) {
		var categoryNID = $categorySelect.val();
		var categoryObj = Drupal.uportal.editContent.categories['nid-'+categoryNID];
		$seriesSelect.html('<option value="_none">- None -</option>');
		if (typeof categoryObj != 'undefined') {
			var childSeriesItems = categoryObj.series;
			$.each(childSeriesItems, function(index, series) {
				$seriesSelect.append('<option value="'+series.nid+'">'+series.series_title+'</option>');
			});
			$createNewSeriesBtn.removeClass('disabled');
		} else {
			$createNewSeriesBtn.addClass('disabled');
		}
		$seriesSelect.trigger('chosen:updated');
		$seriesSelect.trigger('change');
	});
	
	//when series select changes
	$seriesSelect.on('change', function(event, params) {
		var seriesNID = $seriesSelect.val();
		if (seriesNID=='_none') {
			$orderSeriesBtn.addClass('disabled');
		} else {
			$orderSeriesBtn.removeClass('disabled');
		}
	});
	
	//initiate selects
	$categorySelect.trigger('change');
	
	//check for original value
	var allCategories = Drupal.uportal.editContent.categories;
	var currentCategoryNID = $categorySelect.val();
	if (typeof allCategories['nid-'+currentCategoryNID] !== "undefined") {
		var categorySeries = allCategories['nid-'+currentCategoryNID]['series'];
		if (typeof categorySeries['nid-'+originalSeriesNID] !== "undefined") {
			$seriesSelect.val(originalSeriesNID);
			$seriesSelect.trigger('chosen:updated');
			$seriesSelect.trigger('change');
		}
	}
	
}
/** ENDS *****************************************/


/**
 * create new language
 */
Drupal.behaviors.uportal.editContent.createNewLanguage = function($nodeFormWrapper) {
	Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#language-form-wrapper');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'language',
		'title' : 'Language',
		'title_plural' : 'Languages'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val($nodeFormWrapper.data('nid'));
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * create new content provider
 */
Drupal.behaviors.uportal.editContent.createNewContentProvider = function($nodeFormWrapper) {
	Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#content-provider-form-wrapper');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'content-provider',
		'title' : 'Content Provider',
		'title_plural' : 'Content Providers'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val($nodeFormWrapper.data('nid'));
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * create new lesson plan
 */
Drupal.behaviors.uportal.editContent.createNewLessonPlan = function($nodeFormWrapper) {
	var args = {
		initiator: $nodeFormWrapper.data('nid')
	};
	Drupal.behaviors.uportal.bulkEditor.showCreateEditLessonPlanForm(args);
	$.blockUI({ 
		message: Drupal.uportal.bulkEditor.$formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * create new category
 */
Drupal.behaviors.uportal.editContent.createNewCategory = function($nodeFormWrapper) {
	Drupal.uportal.managingMetaData.$formDiv = $('div#category-dialog-boxes-wrapper div#category-dialog-box');
	Drupal.uportal.managingMetaData.metaDataType = {
		'type' : 'category',
		'title' : 'Category',
		'title_plural' : 'Categories'
	};
	$('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val($nodeFormWrapper.data('nid'));
	Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
}
/** ENDS *****************************************/


/**
 * create new series
 */
Drupal.behaviors.uportal.editContent.createNewSeries = function($nodeFormWrapper) {
	var args = {
		default_data: {
			categoryNID: $('.form-item-field-topic-reference-und select', $nodeFormWrapper).val()
		},
		initiator: $nodeFormWrapper.data('nid')
	};
	Drupal.behaviors.uportal.bulkEditor.showCreateEditSeriesForm(args);
}
/** ENDS *****************************************/


/**
 * order series
 */
Drupal.behaviors.uportal.editContent.orderSeries = function($nodeFormWrapper) {
	var $seriesSelect = $('div.form-item-field-series-reference-und select', $nodeFormWrapper);
	var node = {
		'nid': $nodeFormWrapper.data('nid'),
		'type': $nodeFormWrapper.data('node-type'),
		'title': $('.form-item-title input.form-text', $nodeFormWrapper).val(),
		'status': $('.form-item-status input.form-checkbox', $nodeFormWrapper).val()
	};
	var args = {
		'seriesNID': $seriesSelect.val(),
		'initiator' : $nodeFormWrapper.data('nid'),
		'addNodes' : new Array(node)
	}
	Drupal.behaviors.uportal.reorderSeries.reorder(args);
}
/** ENDS *****************************************/


/**
 * new content provider added
 */
Drupal.behaviors.uportal.editContent.newMetaDataAdded = function(event, data) {
	//initiate variables
	var id = data.nid;
	var title = data.title;
	var initiatorNID = data.initiator;
	var selectWrapper;
	
	//get wrapper
	switch (data.meta_data_type) {
		case 'content-provider':
			selectWrapper = 'form-item-field-content-provider-reference-und';
			break;
		case 'language':
			selectWrapper = 'form-item-language';
			break;
		case 'lesson-plan':
			selectWrapper = 'form-item-field-lesson-plan-reference-und';
			break;
		default:
			return;
	}
	
	//add to all selects
	$('div.'+selectWrapper+' select').each( function(index, elem) {
		var $select = $(this);
		$select.append('<option value="'+id+'">'+title+'</option>');
		$select.trigger("chosen:updated");
	});
	
	//switch value for initiating select
	var $initiatingSelect = $('div#node-form-wrapper-nid-'+initiatorNID+' div.'+selectWrapper+' select');
	
	//for lesson plan multiple select, change ID to append to current value
	if (data.meta_data_type=='lesson-plan') {
		var currentValue = $initiatingSelect.val();
		if (currentValue) {
			currentValue.push(id);
		} else {
			currentValue = new Array(id);
		}
		id = currentValue;
	}
	
	$initiatingSelect.val(id);
	$initiatingSelect.trigger("chosen:updated");
	$initiatingSelect.trigger("change");
	
	//unblock ui if we are on the Edit Content page
	if ($initiatingSelect.length>0) {
		$.unblockUI();
	}
}
/** ENDS *****************************************/


/**
 * new category added, refresh
 */
Drupal.behaviors.uportal.editContent.newCategoryAdded = function(event, data) {
	//add new category to javascript array
	//add new category to all selects
	//check initiator and set select to new value
	
	var categoryNID = data.nid;
	var categoryTitle = data.title;
	var initiatorNID = data.initiator;
	
	//adding new category to array
	Drupal.uportal.editContent.categories['nid-'+categoryNID] = {
		nid: categoryNID,
		category_title: categoryTitle,
		series: {}
	};
	
	//add new category to all selects
	$('div.form-item-field-topic-reference-und select').append('<option value="'+categoryNID+'">'+categoryTitle+'</option>').trigger("chosen:updated");
	
	//update category select for initiator
	var $initiatingSelect = $('div#node-form-wrapper-nid-'+initiatorNID+' div.form-item-field-topic-reference-und select');
	$initiatingSelect.val(categoryNID);
	$initiatingSelect.trigger("chosen:updated");
	$initiatingSelect.trigger("change");
	
	if ($initiatingSelect.length>0) {
		$.unblockUI();
	}
};
/** ENDS *****************************************/


/**
 * series ordered, store data
 */
Drupal.behaviors.uportal.editContent.seriesOrdered = function(event, data) {
	var initiatorNID = data.initiator;
	var $nodeFormWrapper = $('div#node-form-wrapper-nid-'+initiatorNID);
	var $seriesOrderedInfoWrapper = $('.temporary-data .series-ordered-info', $nodeFormWrapper);
	$('input.series-ordered-nid', $seriesOrderedInfoWrapper).val(data.seriesNID);
	$('input.series-ordered-order', $seriesOrderedInfoWrapper).val(data.nidsOrder);
};
/** ENDS *****************************************/


/**
 * all nodes edited - metadata
 */
Drupal.behaviors.uportal.editContent.allNodesEditedMetaData = function(event, data) {
	
	//content provider, language, category
	var contentProviderNID = parseInt(data.data_values.content_provider.nid);
	var categoryNID = parseInt(data.data_values.category.nid);
	var languageCode = data.data_values.language.code;
	
	//content provider
	if (!isNaN(contentProviderNID) && contentProviderNID>0) {
		$('div.field-name-field-content-provider-reference div.form-item-field-content-provider-reference-und select')
			.data('ays-orig', contentProviderNID.toString())
			.val(contentProviderNID)
			.trigger("chosen:updated")
			.trigger('change');
	}
	
	//language code
	if (languageCode && languageCode!='') {
		$('div.form-item-language select')
			.data('ays-orig', languageCode.toString())
			.val(languageCode)
			.trigger("chosen:updated")
			.trigger('change');
	}
	
	//category
	if (!isNaN(categoryNID) && categoryNID>0) {
		$('div.field-name-field-topic-reference div.form-item-field-topic-reference-und select').each(
			function (index, elem) {
				var $select = $(elem);
				if ($select.val()==categoryNID) {
					return true;
				}
				
				//if new category, revert series to _none
				var $categorySeriesWrapperParent = $select.closest('div.category-series-wrapper');
				var $seriesSelect = $('div.form-item-field-series-reference-und select', $categorySeriesWrapperParent);
				$seriesSelect
					.data('ays-orig', '_none')
					.val('_none')
					.trigger("chosen:updated")
					.trigger('change');
				
				//new category, change category
				$select
					.data('ays-orig', categoryNID.toString())
					.val(categoryNID)
					.trigger("chosen:updated")
					.trigger('change');
				return true;
			}
		);
	}
	
	//update form changes and dirty status
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * all nodes edited - lesson plans
 */
Drupal.behaviors.uportal.editContent.allNodesEditedLessonPlans = function(event, data) {
	
	var lessonPlanNIDs = data.data_values.lesson_plans;
	var removeOldLessonPlans = data.removed_old_lesson_plans;
	
	//update lesson plan fields
	$('div.field-name-field-lesson-plan-reference div.form-item-field-lesson-plan-reference-und select').each(
		function(index, elem) {
			var $select = $(elem);
			var newValue = (removeOldLessonPlans) ? [] : $select.val();
			if (newValue) {
				$.each(lessonPlanNIDs, function(index, nid) {
					newValue.push(nid);
				});
			} else {
				newValue = lessonPlanNIDs;
			}
			
			var newValueStr = newValue.toString().trim(",").replace(",,", ",");
			
			$select
				.data('ays-orig', newValueStr)
				.val(newValue)
				.trigger("change")
				.trigger("chosen:updated");
			
			//restore original value
			var selectVal = $select.val();
			var newValueStr = '';
			if (selectVal) {
				newValueStr = selectVal.toString();
			}
			$select.data('ays-orig', newValueStr);
			
		}
	);
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * all nodes edited - tags
 */
Drupal.behaviors.uportal.editContent.allNodesEditedTags = function(event, data) {
	
	var nids = data.nids;
	var tids = data.data_values.tids;
	var removeOldTags = data.removed_old_tags;
	var vocabularies = Drupal.uportal.managingMetaDataTags.existingVocabularies;
	var terms = Drupal.uportal.managingMetaDataTags.existingTerms;
	
	//first update all the superficial tag fields
	$('div.field-type-custom-all-taxonomy-terms select').each(
		function (index, elem) {
			var $select = $(elem);
			var newValue = (removeOldTags) ? [] : $select.val();
			if (newValue) {
				$.each(tids, function(index, tid) {
					newValue.push(tid);
				});
			} else {
				newValue = tids;
			}
			
			var newValueStr = newValue.toString().trim(",").replace(",,", ",");
			
			console.log('first orig: '+$select.data('ays-orig'));
			console.log('first val: '+$select.val());
			$select
				.data('ays-orig', newValueStr)
				.val(newValue)
				.trigger("change")
				.trigger("chosen:updated");
			$select.data('ays-orig', $select.val().toString());
			$select.closest('form').trigger('checkform.areYouSure');
			console.log('second orig: '+$select.data('ays-orig'));
			console.log('second val: '+$select.val());
			
		}
	);
	
	//get all vocabularies and their new values
	var newTidsByVocab = {};
	$.each(tids, function(index, tid) {
		var term = terms['tid-'+tid];
		var vocab = vocabularies['vid-'+term.vid];
		
		if (typeof newTidsByVocab['vid-'+term.vid] == 'undefined') {
			newTidsByVocab['vid-'+term.vid] = {
				'tids': new Array()
			};
		}
		
		newTidsByVocab['vid-'+term.vid].tids.push(tid);
	});
	
	//reset all original vocabulary selects
	$.each(nids, function (index, nid) {
		var $form = $('div#node-wrapper-nid-'+nid+' form');
		return false;
		$.each(vocabularies, function(index, vocab) {
			var vocabulary_id = vocab.machine_name.replace(/\_/g, '-');
			var $vocabSelect = $('select#edit-field-'+vocabulary_id+'-reference-und', $form);
			var selectedTIDs = removeOldTags ? [] : $vocabSelect.val();
			if (typeof newTidsByVocab['vid-'+vocab.vid] != 'undefined') {
				var vocabTIDs = newTidsByVocab['vid-'+vocab.vid]['tids'];
				selectedTIDs = vocabTIDs.concat(selectedTIDs).removeDuplicates();
			}
			
			if (selectedTIDs && selectedTIDs.length>0) {
				var newValueStr = selectedTIDs.toString().trim(",").replace(",,", ",");
				$vocabSelect
					.data('ays-orig', newValueStr)
					.val(selectedTIDs)
					.trigger("change")
					.trigger("chosen:updated");
			}
		});
	});
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * all nodes edited - series
 */
Drupal.behaviors.uportal.editContent.allNodesEditedSeries = function(event, data) {
	
	//series and category nids
	var seriesNID = data.data_values.series.nid;
	var categoryNID = data.data_values.category.nid;
	
	//check for invalid series NID
	if (isNaN(seriesNID) || seriesNID<=0) {
		seriesNID = '_none';
	}
	
	//update category fields
	$('div.field-name-field-topic-reference div.form-item-field-topic-reference-und select')
		.data('ays-orig', categoryNID.toString())
		.val(categoryNID)
		.trigger("chosen:updated")
		.trigger('change');
	$('div.field-name-field-series-reference div.form-item-field-series-reference-und select')
		.data('ays-orig', seriesNID.toString())
		.val(seriesNID)
		.trigger("chosen:updated")
		.trigger('change');
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * new series added, refresh
 */
Drupal.behaviors.uportal.editContent.newSeriesAdded = function(event, data) {
	//add new series to javascript array
	//add new series to all selects with current category
	//check initiator and set select to new value
	
	var seriesNID = data.nid;
	var parentCategoryNID = data.category_nid;
	var seriesTitle = data.title;
	var initiatorNID = data.initiator;
	
	//adding new series to array
	Drupal.uportal.editContent.categories['nid-'+parentCategoryNID]['series']['nid-'+seriesNID] = {
		nid: seriesNID,
		series_title: seriesTitle
	};
	
	//add new series to all selects with active category
	$('div.node-edit-wrapper').each( function(index, elem) {
		var $wrapper = $(this);
		var $categorySelect = $('div.form-item-field-topic-reference-und select', $wrapper);
		var $seriesSelect = $('div.form-item-field-series-reference-und select', $wrapper);
		var selectedCategoryNID = $categorySelect.val();
		if (selectedCategoryNID==parentCategoryNID) {
			$seriesSelect.append('<option value="'+seriesNID+'">'+seriesTitle+'</option>');
			$seriesSelect.trigger("chosen:updated");
		}
	});
	
	//update series select for initiator
	var $initiatingSelect = $('div#node-form-wrapper-nid-'+initiatorNID+' div.form-item-field-series-reference-und select');
	$initiatingSelect.val(seriesNID);
	$initiatingSelect.trigger("chosen:updated");
	$initiatingSelect.trigger("change");
	
	//unblock ui
	if ($initiatingSelect.length>0) {
		$.unblockUI();
	}
};
/** ENDS *****************************************/


/**
 * set up tags field in form, to be able to update real taxonomy fields when changed
 * - just hide it until taxonomy data is fetched
 */
Drupal.behaviors.uportal.editContent.setUpFormTagsField = function($form) {
	
	//temporarily hide tags field as we fetch taxonomy data
	var $tagsFieldWrapper = $('div.field-type-custom-all-taxonomy-terms div.form-item', $form);
	var $select = $('select', $tagsFieldWrapper);
	$('.chosen-container', $tagsFieldWrapper).hide();
	$tagsFieldWrapper.append('<div class="waiting-msg">Please wait, loading tags data ...</div>');
	
	if (!Drupal.uportal.editContent.fetchedTaxonomy) {
		return;
	}
	
	//taxonomy fetched, hide waiting msg, show select
	$('.chosen-container', $tagsFieldWrapper).show();
	$('.waiting-msg', $tagsFieldWrapper).remove();
	
	//disable option _none
	//Drupal.behaviors.uportal.editContent.disableNoneOption($select);
	//Drupal.behaviors.uportal.editContent.initialiseMultiSelect($select);
	
};
/** ENDS *****************************************/


/**
 * update tags and vocabulary fields with new tags value
 */
Drupal.behaviors.uportal.editContent.updateTaxonomyFields = function($tagsSelect) {
	var selectedTIDs = $tagsSelect.val();
	var vocabularies = Drupal.uportal.managingMetaDataTags.existingVocabularies;
	var terms = Drupal.uportal.managingMetaDataTags.existingTerms;
	var $form = $tagsSelect.closest('form');

	//first reset all vocab selects
	$.each(vocabularies, function(index, vocab) {
		var vocabulary_id = vocab.machine_name.replace(/\_/g, '-');
		var $vocabSelect = $('select#edit-field-'+vocabulary_id+'-reference-und', $form);
		$vocabSelect.val([]);
	});

	//update vocab select values
	if (selectedTIDs && selectedTIDs.length) {
		$.each(selectedTIDs, function(index, tid) {
			var term = terms['tid-'+tid];
			var vocab = vocabularies['vid-'+term.vid];
			var vocab_machine_name = vocab.machine_name;
			var vocabulary_id = vocab_machine_name.replace(/\_/g, '-');
			var $vocabSelect = $('select#edit-field-'+vocabulary_id+'-reference-und', $form);
			var vocabValues = $vocabSelect.val();
			if (vocabValues && vocabValues.length) {
				vocabValues.push(tid);
			} else {
				vocabValues = new Array(tid);
			}
			$vocabSelect.val(vocabValues);
		});
	}
};
/** ENDS *****************************************/


/**
 * set up tags fields
 */
Drupal.behaviors.uportal.editContent.fetchedTaxonomy = function(event) {
	
	Drupal.uportal.editContent.fetchedTaxonomy = true;
	
	//check if any forms have already been loaded so we can set up the tags field in the form
	//otherwise, only set up tags field when form is being initiated
	$('div.node-edit-wrapper form').each( function(index, elem) {
		var $form = $(elem);
		Drupal.behaviors.uportal.editContent.setUpFormTagsField($form);
	})
};
/** ENDS *****************************************/


/**
 * after updating nodes status, update node divs
 */
Drupal.behaviors.uportal.editContent.updateNodesStatus = function(e, data) {
	var $nodesWrapper = Drupal.uportal.editContent.$nodesWrapper;
	var changedNIDs = data.changed_nids;
	var failedNIDs = data.failed_nids;
	var newStatus = data.status;
	console.log('status updated');
	
	//update divs
	$.each(changedNIDs, function(index, nid) {
		var $nodeDiv = $('div#node-wrapper-nid-'+nid, $nodesWrapper);
		if (newStatus=='1') {
			$nodeDiv.removeClass('status-draft').addClass('status-published');
			$('div.node-status', $nodeDiv).text('Published');
			Drupal.behaviors.uportal.editContent.collapseNodeForm($nodeDiv, false);
		} else {
			$nodeDiv.removeClass('status-published').addClass('status-draft');
			$('div.node-status', $nodeDiv).text('Draft');
			Drupal.behaviors.uportal.editContent.loadNodeForm(nid, $nodeDiv);
		}
	});
	
	//alert for failed NIDs - failed to publish
	$.each(failedNIDs, function(index, nid) {
		if (newStatus=='1') {
			var $nodeDiv = $('div#node-wrapper-nid-'+nid, $nodesWrapper);
			$('a.publish-btn', $nodeDiv).click();
		}
	});
};
/** ENDS *****************************************/


/**
 * initiate page
 */
Drupal.behaviors.uportal.editContent.initiateEditContentPage = function(context, settings) {
	
	//check page
	if (!($('body').hasClass('page-management-edit-content'))) {
		return;
	}
	
	$('body.page-management-edit-content').once( function() {
		
		//tap into metadata functionality
		Drupal.uportal.managingMetaData.addTopButtonsSearch = false;
		
		//initiate
		Drupal.uportal.editContent.$nodesWrapper = $('div#block-system-main div.edit-content-wrapper');
		Drupal.behaviors.uportal.editContent.retrieveCategoriesAndSeries();
		Drupal.behaviors.uportal.editContent.initiateSaveButtons();
		Drupal.behaviors.uportal.editContent.initiateDeleteButtons();
		Drupal.behaviors.uportal.editContent.initiateExpandCollapse();
		var noNodes = $('div.node-edit-wrapper', Drupal.uportal.editContent.$nodesWrapper).length;
		
		//initiate bulk edit button
		var $bulkEditBtn = $('header.page-header .region-header-strip-2 a#bulk-edit-btn');
		if (noNodes<=1) {
			$bulkEditBtn.hide();
		}
		$bulkEditBtn.click(
			function() {
				Drupal.uportal.bulkEditor.selectedNIDs = [];
				$('section#main-content div#block-system-main .node-edit-wrapper').each(
					function (index, elem) {
						var $$ = $(elem);
						Drupal.uportal.bulkEditor.selectedNIDs.push($$.data('nid'));
					}
				)
				Drupal.behaviors.uportal.bulkEditor.bulkEditNodes();
			}
		);
		
		//watch for events
		Drupal.uportal.$eventWatcher.on('created-new-category', Drupal.behaviors.uportal.editContent.newCategoryAdded);
		Drupal.uportal.$eventWatcher.on('created-new-metadata', Drupal.behaviors.uportal.editContent.newMetaDataAdded);
		Drupal.uportal.$eventWatcher.on('bulkEditorCreatedNewSeries', Drupal.behaviors.uportal.editContent.newSeriesAdded);
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeSeries', Drupal.behaviors.uportal.editContent.allNodesEditedSeries);
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeLessonPlans', Drupal.behaviors.uportal.editContent.allNodesEditedLessonPlans);
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeTags', Drupal.behaviors.uportal.editContent.allNodesEditedTags);
		Drupal.uportal.$eventWatcher.on('orderingSeriesFinished', Drupal.behaviors.uportal.editContent.seriesOrdered);
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeMetaData', Drupal.behaviors.uportal.editContent.allNodesEditedMetaData);
		Drupal.uportal.$eventWatcher.on('fetched-existing-taxonomy', Drupal.behaviors.uportal.editContent.fetchedTaxonomy);
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeStatus', Drupal.behaviors.uportal.editContent.updateNodesStatus);
		
		//start page with all items expanded
		$('div.node-header', Drupal.uportal.editContent.$nodesWrapper).click();
		
		//hook into all submit button ajax
		for (ajax_elem in settings.ajax) {
			Drupal.settings.ajax[ajax_elem].beforeSend = Drupal.behaviors.uportal.editContent.beforeSendingForm;
			if (typeof Drupal.ajax[ajax_elem] != 'undefined' && Drupal.ajax[ajax_elem].element.form) {
			}
		}
		
		//disable enter key submitting forms
		$(window).bind("keyup keypress keydown", function(e) {
			var code = e.keyCode || e.which;
			if (code  == 13) {
				e.preventDefault();
				return false;
			}
		});
		
		//done button
		$('header.page-header .btns-wrapper a.done-btn').click( function() {
			parent.history.back();
      return false;
		});
		
	});
	
	//attach these behaviours all the time
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.editContent.initiateEditContentPage);
