
(function($) {


/**
 * globals we need for content listing
 */
Drupal.uportal.managingMetaData = {
	$resultsWrapper : {},
	$headerWrapper : {},
	$formDiv : {},
	metaDataType : {},
	addTopButtonsSearch : true,
	liveValidationObjs : new Array(),
	serverURL : '/management/metadata/form-handler',
	serverCategoriesURL : '/management/metadata/form-handler/categories',
	serverLanguagesURL : '/management/metadata/form-handler/languages',
	serverContentProvidersURL : '/management/metadata/form-handler/content-providers',
	serverReadURL : '/management/metadata/form-handler/read-only',
	existingContentProviders : {},
	existingLanguages : {}
};
/** ENDS *****************************************/


/**
 * change form to edit or add status
 */
Drupal.behaviors.uportal.managingMetaData.changeFormStatus = function(opType, nid, values) {
	var $formDiv = Drupal.uportal.managingMetaData.$formDiv;
	var metaDataTypeTitle = Drupal.uportal.managingMetaData.metaDataType.title;
	var metaDataType = Drupal.uportal.managingMetaData.metaDataType.type;
	var titleText = 'Add a '+metaDataTypeTitle;
	var btnText = 'Save';
	var op = 'add-'+metaDataType;
	var op_general = 'add';
	var titleValue = '';
	var urlValue = '';
	
	//reset form
	$('form', $formDiv).data('validator').resetForm();
	$('input, textarea', $formDiv).removeClass('error');
	$('.category-icon-error', $formDiv).hide();
	
	//remove errors
	$('.form-error', $formDiv).hide();
	$('.form-item', $formDiv).removeClass('duplicate-error');
	
	if (opType=='edit') {
		titleText = 'Edit '+metaDataTypeTitle;
		btnText = 'Update';
		op = 'edit-'+metaDataType;
		op_general = 'edit';
		if (values.title) titleValue = values.title;
		if (values.url) urlValue = values.url;
	}
	
	if (metaDataType=='category') {
		var iconName = '';
		var iconHTML = '';
		var desc = '';
		titleText = 'Create a Category';
		
		if (opType=='edit') {
			var $nodeWrapper = $('#parent-nid-'+nid, Drupal.uportal.managingMetaData.$resultsWrapper);
			iconName = $nodeWrapper.data('icon-name');
			iconHTML = $('.category-icon .icon', $nodeWrapper).html();
			desc = $('.category-desc', $nodeWrapper).text();
		}
		
		$('input#image-name', $formDiv).val(iconName);
		$('.chosen-icon-wrapper .chosen-icon', $formDiv).html(iconHTML);
		$('textarea#metadata-body-category', $formDiv).val(desc);
	}
	
	$('.dialog-header', $formDiv).text(titleText);
	$('.dialog-footer .yes', $formDiv).val(btnText);
	$('input.metadata-title', $formDiv).val(titleValue);
	$('input#metadata-url', $formDiv).val(urlValue);
	$('input#op', $formDiv).val(op);
	$('input#op-general', $formDiv).val(op_general);
	$('input#nid', $formDiv).val(nid);
	
}
/** ENDS *****************************************/


/**
 * initiate add button
 */
Drupal.behaviors.uportal.managingMetaData.initiateAddButton = function() {
	
	//separate functionality for taxonomy/tag buttons
	//check separate file
	if (
		Drupal.uportal.managingMetaData.metaDataType.type=='tag'
		|| !Drupal.uportal.managingMetaData.addTopButtonsSearch
	) {
		return;
	}
	
	var btnText = "Add "+Drupal.uportal.managingMetaData.metaDataType.title;
	var $createNewBtn = $('<a class="btn content-provider-btn">'+btnText+'</a>');
	$createNewBtn.appendTo($('header.page-header .header-strip-1 .col-2-header'));
	
	if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
		$createNewBtn.text('Create Category');
	}
	
	if (
		(Drupal.uportal.managingMetaData.metaDataType.type=='category' && Drupal.behaviors.uportal.userIsAllowed('manage-categories'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='language' && Drupal.behaviors.uportal.userIsAllowed('manage-languages'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider' && Drupal.behaviors.uportal.userIsAllowed('manage-content-providers'))
	) {
		$createNewBtn.click( function() {
			Drupal.behaviors.uportal.managingMetaData.showCreateNewForm();
		});
	} else {
		$createNewBtn.addClass('disabled');
	}

};
/** ENDS *****************************************/


/**
 * show create new form
 */
Drupal.behaviors.uportal.managingMetaData.showCreateNewForm = function() {
	Drupal.behaviors.uportal.managingMetaData.initiateForm();
	Drupal.behaviors.uportal.managingMetaData.changeFormStatus('add', 0, {});
	$.blockUI({
		message: Drupal.uportal.managingMetaData.$formDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
};
/** ENDS *****************************************/


/**
 * initiate edit buttons
 */
Drupal.behaviors.uportal.managingMetaData.initiateEditButtons = function() {
	
	//separate functionality for taxonomy/tag buttons
	//check separate file
	if (Drupal.uportal.managingMetaData.metaDataType.type=='tag') {
		return;
	}
	
	//check permissions - if not allowed, disable buttons and exit
	if (!(
		(Drupal.uportal.managingMetaData.metaDataType.type=='category' && Drupal.behaviors.uportal.userIsAllowed('manage-categories'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='language' && Drupal.behaviors.uportal.userIsAllowed('manage-languages'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider' && Drupal.behaviors.uportal.userIsAllowed('manage-content-providers'))
	)) {
		$('.main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('disabled');
		return;
	}
	
	$('.main-result-edit', Drupal.uportal.managingMetaData.$resultsWrapper).click(
		function() {
			var $btn = $(this);
			var nid = $btn.data('nid');
			var $nodeDiv = $('#parent-nid-'+nid, Drupal.uportal.managingMetaData.$resultsWrapper);
			var nodeTitle = $('.main-result-title', $nodeDiv).text();
			var nodeURL = $('.main-result-url', $nodeDiv).text();;
			var nodeValues = {
				'title' : nodeTitle,
				'url' : nodeURL
			};
			
			//update and show form
			Drupal.behaviors.uportal.managingMetaData.changeFormStatus('edit', nid, nodeValues);
			$.blockUI({
				message: Drupal.uportal.managingMetaData.$formDiv,
				blockMsgClass: 'dialog-box-block-ui-wrapper'
			});
		}
	);
};
/** ENDS *****************************************/


/**
 * delete language
 */
Drupal.behaviors.uportal.managingMetaData.deleteLanguage = function(nodeTitle, nid, $removeDiv) {
	var $confirmationDiv = $('div#confirm-deletion');
	$('.dialog-header', $confirmationDiv).text('Delete Language?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete <em>'+nodeTitle+'</em>?');
	$('.dialog-footer .yes', $confirmationDiv).click( function() {
		
		if (Drupal.uportal.editContent.nidToDelete != nid) {
			return false;
		}
		
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting Language ...', 'Please wait as we delete <em>'+nodeTitle+'</em> ...');
		$.post(Drupal.uportal.managingMetaData.serverLanguagesURL, {
			'op' : 'delete-language',
			'nid' : nid
		}).done( function(data) {
				$removeDiv.remove();
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
Drupal.behaviors.uportal.managingMetaData.initiateDeleteButtons = function() {
	
	//separate functionality for taxonomy/tag buttons
	//check separate file
	if (Drupal.uportal.managingMetaData.metaDataType.type=='tag') {
		return;
	}
	
	//check permissions - if not allowed, disable buttons and exit
	if (!(
		(Drupal.uportal.managingMetaData.metaDataType.type=='category' && Drupal.behaviors.uportal.userIsAllowed('manage-categories'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='language' && Drupal.behaviors.uportal.userIsAllowed('manage-languages'))
		|| (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider' && Drupal.behaviors.uportal.userIsAllowed('manage-content-providers'))
	)) {
		$('.main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).addClass('main-result-delete-disabled');
		return;
	}
	
	$('.main-result-delete', Drupal.uportal.managingMetaData.$resultsWrapper).click( function() {
		var $btn = $(this);
		var nid = $btn.data('nid');
		var $nodeDiv = $('#parent-nid-'+nid, Drupal.uportal.managingMetaData.$resultsWrapper);
		var nodeTitle = $('.main-result-title', $nodeDiv).text();
		var deleteURL;
		
		//delete language
		if (Drupal.uportal.managingMetaData.metaDataType.type=='language') {
			Drupal.uportal.editContent.nidToDelete = nid;
			Drupal.behaviors.uportal.managingMetaData.deleteLanguage(nodeTitle, nid, $nodeDiv);
			return false;
		}
		
		//content provider url
		if (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider') {
			deleteURL = Drupal.uportal.managingMetaData.serverContentProvidersURL;
		}
		
		//content provider url
		if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
			deleteURL = Drupal.uportal.managingMetaData.serverCategoriesURL;
		}
		
		//delete content
		Drupal.uportal.editContent.nidToDelete = nid;
		Drupal.behaviors.uportal.editContent.deleteContent(nodeTitle, nid, $nodeDiv, deleteURL);
		
		return false;
	});
}
/** ENDS *****************************************/


/**
 * initiate metadata types for page
 */
Drupal.behaviors.uportal.managingMetaData.initiateMetaDataType = function() {
	if ($('body').hasClass('page-management-metadata-content-providers')) {
		Drupal.uportal.managingMetaData.metaDataType = {
			'type' : 'content-provider',
			'title' : 'Content Provider',
			'title_plural' : 'Content Providers'
		};
	}
	if ($('body').hasClass('page-management-metadata-languages')) {
		Drupal.uportal.managingMetaData.metaDataType = {
			'type' : 'language',
			'title' : 'Language',
			'title_plural' : 'Languages'
		};
	}
	if ($('body').hasClass('page-management-metadata-categories')) {
		Drupal.uportal.managingMetaData.metaDataType = {
			'type' : 'category',
			'title' : 'Category',
			'title_plural' : 'Categories'
		};
	}
	if ($('body').hasClass('page-management-metadata-tags')) {
		Drupal.uportal.managingMetaData.metaDataType = {
			'type' : 'tag',
			'title' : 'Tag',
			'title_plural' : 'Tags'
		};
	}
};
/** ENDS *****************************************/


/**
 * form validator
 */
Drupal.behaviors.uportal.managingMetaData.initiateForm = function() {
	
	//separate functionality for taxonomy/tag buttons
	//check separate file
	if (Drupal.uportal.managingMetaData.metaDataType.type=='tag') {
		return;
	}
	
	//check for category icon
	var validateCategoryIcon = function() {
		if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
			var imageVal = $('form#metadata-category-form input#image-name', Drupal.uportal.managingMetaData.$formDiv).val();
			if (!imageVal) {
				$('form#metadata-category-form label.category-icon-error', Drupal.uportal.managingMetaData.$formDiv).text('Please select an icon for this category').show();
				return false;
			} else {
				$('form#metadata-category-form label.category-icon-error', Drupal.uportal.managingMetaData.$formDiv).hide();
			}
		}
		return true;
	};
	
	//add custom validator hooking into title to check for category icon
	$.validator.addMethod(
		"category_icon_checker",
		function(value, element) {
			validateCategoryIcon();
			return true;
		},
		""
	);
	
	//validate title
	$('form', Drupal.uportal.managingMetaData.$formDiv).validate({
		rules: {
			title: {
				category_icon_checker: "category_icon_checker",
				required: true,
				minlength: 2
			},
			url: {
				url: true
			}
		},
		messages: {
			title: {
				required: 'This field is required.',
				minlength: 'This field requires a minimum of two characters.'
			},
			url: {
				url: 'Enter a valid url e.g. <em>http://www.yahoo.com/</em>'
			}
		},
		showErrors: function(errorMap, errorList) {
			this.defaultShowErrors();
			validateCategoryIcon();
		}
	});
	
	//handle cancel button
	$('.cancel-metadata', Drupal.uportal.managingMetaData.$formDiv).click(
		function () {
			var $cancelBtn = $(this);
			var initiator = $('input#initiator', Drupal.uportal.managingMetaData.$formDiv).val();
			if (initiator=='bulk-editor') {
				if (
					$cancelBtn.hasClass('cancel-metadata-content-provider')
					|| $cancelBtn.hasClass('cancel-metadata-language')
				) {
					Drupal.behaviors.uportal.bulkEditor.bulkEditNodes();
				} else if (
					$cancelBtn.hasClass('cancel-metadata-category')
				) {
					switch (Drupal.uportal.bulkEditor.initiatingCategorySelectID) {
						case 'category':
							Drupal.behaviors.uportal.bulkEditor.bulkEditNodes();
							break;
						case 'category-choose-series':
							Drupal.behaviors.uportal.bulkEditor.showChooseSeriesForm();
							break;
						case 'series-form-category':
							Drupal.behaviors.uportal.bulkEditor.showCreateEditSeriesForm();
							break;
						default:
							$.unblockUI();
							break;
					}
				} else {
					$.unblockUI();
				}
			} else {
				$.unblockUI();
			}
			return false;
		}
	);
	
	//submit via ajax
	$('form', Drupal.uportal.managingMetaData.$formDiv).ajaxForm({
		dataType: 'json',
		beforeSubmit: function(formData, $form, options) {
			var title = 'Adding a new '+Drupal.uportal.managingMetaData.metaDataType.title+' ...';
			var msg = 'Please wait as we add a new '+Drupal.uportal.managingMetaData.metaDataType.title+' ...';
			var op_general = $('input#op-general', $form).val();
			
			if (op_general=='edit') {
				title = 'Editing '+Drupal.uportal.managingMetaData.metaDataType.title+' ...';
				msg = 'Please wait as we update this '+Drupal.uportal.managingMetaData.metaDataType.title+' ...';
			}
			
			//check for duplicate titles
			var duplicateTitleErrorPresent = Drupal.behaviors.uportal.managingMetaData.checkTitleIsDuplicate($form);
			var $formTitleWrapper = $('.form-item-title', $form);
			if (duplicateTitleErrorPresent) {
				$formTitleWrapper.addClass('duplicate-error');
				$('.form-error', $formTitleWrapper).fadeIn();
				return false;
			} else {
				$formTitleWrapper.removeClass('duplicate-error');
				$('.form-item-title .form-error', $form).hide();
			}
			
			//validate icon field for category
			if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
				var imageVal = $('form#metadata-category-form input#image-name', Drupal.uportal.managingMetaData.$formDiv).val();
				if (!imageVal) {
					$('form#metadata-category-form label.category-icon-error', Drupal.uportal.managingMetaData.$formDiv).text('Please select an icon for this category').show();
					return false;
				} else {
					$('form#metadata-category-form label.category-icon-error', Drupal.uportal.managingMetaData.$formDiv).hide();
				}
			}
			
			Drupal.behaviors.uportal.blockUIWaitMessage(title, msg);
			return true;
		},
		success: function(jsonData, statusText, xhr, $form) {
			if (jsonData.op_general=='add') {
				Drupal.uportal.managingMetaData.$resultsWrapper.append(jsonData.new_row_html);
				if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
					var $nodeDiv = $('#parent-nid-'+jsonData.nid, Drupal.uportal.managingMetaData.$resultsWrapper);
					$('.menu-order', $nodeDiv).text($('.full-result-row', Drupal.uportal.managingMetaData.$resultsWrapper).length);
					Drupal.behaviors.uportal.managingCategories.updateExistingCategoryData(jsonData);
					Drupal.uportal.$eventWatcher.trigger('created-new-category', jsonData);
				} else {
					if (Drupal.uportal.managingMetaData.metaDataType.type=='language') {
						Drupal.uportal.managingMetaData.existingLanguages[jsonData.nid] = jsonData.title;
					}
					if (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider') {
						Drupal.uportal.managingMetaData.existingContentProviders['nid-'+jsonData.nid] = jsonData.title;
					}
					Drupal.uportal.$eventWatcher.trigger('created-new-metadata', jsonData);
				}
				Drupal.attachBehaviors();
			} else if (jsonData.op_general=='edit') {
				var $nodeDiv = $('#parent-nid-'+jsonData.nid, Drupal.uportal.managingMetaData.$resultsWrapper);
				$('.main-result-title', $nodeDiv).text(jsonData.title);
				$('div.main-result-url', $nodeDiv).text(jsonData.url);
				if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
					$nodeDiv.attr('data-icon-name', jsonData.icon_name);
					$('.category-icon .icon', $nodeDiv).html(jsonData.icon_html);
					$('.category-desc', $nodeDiv).text(jsonData.desc);
					Drupal.behaviors.uportal.managingCategories.updateExistingCategoryData(jsonData);
				}
			}
			
			//unblock for the metadata pages only
			//everyone else can unblock for themselves
			if ($('body').hasClass('page-management-metadata')) {
				$.unblockUI();
			}
			
		}
	});
	
}
/** ENDS *****************************************/


/**
 * check for duplicate titles
 */
Drupal.behaviors.uportal.managingMetaData.checkTitleIsDuplicate = function($form) {
	var newTitle = $('input[name="title"]', $form).val();
	var operation = $('input[name="op-general"]', $form).val();
	var nid = $('input[name="nid"]', $form).val();
	var metaDataType = Drupal.uportal.managingMetaData.metaDataType.type;
	var existingTitles = new Array();
	
	switch (metaDataType) {
		case 'category':
			existingTitles = Drupal.uportal.managingCategories.existingCategories;
			break;
		case 'language':
			existingTitles = Drupal.uportal.managingMetaData.existingLanguages;
			break;
		case 'content-provider':
			existingTitles = Drupal.uportal.managingMetaData.existingContentProviders;
			break;
	}
	
	var titleExists = Drupal.behaviors.uportal.checkStringExistsInArray(newTitle, existingTitles);
	
	if (parseInt(nid)>0 && operation=='edit') {
		var index = (metaDataType=='language') ? nid : 'nid-'+nid;
		var oldTitle = existingTitles[index];
		if (Drupal.behaviors.uportal.stringsAreTheSame(newTitle, oldTitle)) {
			titleExists = false;
		}
	}
	
	return titleExists;
	
}
/** ENDS *****************************************/


/**
 * initiate sort buttons
 */
Drupal.behaviors.uportal.managingMetaData.initiateSortButtons = function() {
	var $sorters = $('.parent-label, .count-label', Drupal.uportal.managingMetaData.$headerWrapper);
	var $contentWrapper = Drupal.uportal.managingMetaData.$resultsWrapper;
	
	$sorters.each( function (index, elem) {
		var $sorter = $(elem);
		$sorter.click( function() {
			var $sorter = $(this);
			
			//remove active class
			$('col-label', Drupal.uportal.managingMetaData.$headerWrapper).removeClass('active');
			
			//get sort order
			var sortOrder = 'asc';
			var currentOrder = $sorter.data('direction');
			if (currentOrder=='asc') sortOrder = 'desc';
			if (currentOrder=='desc') sortOrder = 'asc';
			
			//sort items using tiny sort
			if ($sorter.hasClass('parent-label')) {
				$('.full-result-row', $contentWrapper).tsort('.main-result-title', {order:sortOrder}, '.result-content-header .result-category-count', {order:'desc'});
			}
			if ($sorter.hasClass('count-label')) {
				$('.full-result-row', $contentWrapper).tsort('.result-content-header .result-category-count', {order:sortOrder}, '.main-result-title', {order:'asc'});
			}
			
			//update data attr
			$sorter.data('direction', sortOrder);
			
			$sorter.addClass('active');
			return false;
		})
	})
}
/** ENDS *****************************************/


/**
 * initiate search box
 */
Drupal.behaviors.uportal.managingMetaData.initiateSearchBox = function() {
	
	if (!Drupal.uportal.managingMetaData.addTopButtonsSearch) {
		return;
	}
	
	var $resultsWrapper = Drupal.uportal.managingMetaData.$resultsWrapper;
	var placeholder = "Search "+Drupal.uportal.managingMetaData.metaDataType.title_plural+" ...";
	var $searchBox = $('<input type="text" class="search-box live-filter-search-box" id="search-box" value="" placeholder="'+placeholder+'" />');
	$searchBox.appendTo($('header.page-header .header-strip-1 .col-2-header'));
	
	var options = { 'filterChildSelector' : '.main-result-title' };
	if (Drupal.uportal.managingMetaData.metaDataType.type=='tag') {
		options = {};
	}
	
	$resultsWrapper.liveFilter(
		'header.page-header input#search-box',
		'.full-result-row', 
		options
	);
}
/** ENDS *****************************************/


/**
 * show graphs
 */
Drupal.behaviors.uportal.managingMetaData.initiateGraphs = function() {
	var largestNumber = 0;
	var $resultsWrapper = Drupal.uportal.managingMetaData.$resultsWrapper;
	$('div.result-category-graph div.full-graph', $resultsWrapper).each(
		function(index, elem) {
			var numberStr = $(elem).data('full-count');
			var number = parseInt(numberStr);
			if (number>largestNumber) {
				largestNumber = number;
			}
		}
	);
	$('div.result-category-graph div.full-graph', $resultsWrapper).each(
		function(index, elem) {
			var $fullGraphDiv = $(elem);
			var totalNumber = parseInt($fullGraphDiv.data('full-count'));
			var widthInPercentage = (totalNumber/largestNumber)*100;
			$fullGraphDiv.css('width', widthInPercentage+'%');
			
			//set children div widths
			$('.count', $fullGraphDiv).each( function(index, elem) {
				var $childDiv = $(elem);
				var childCount = parseInt($childDiv.data('count'));
				var childWidthInPercentage = (childCount/totalNumber)*100;
				$childDiv.css('width', childWidthInPercentage+'%');
				if (childWidthInPercentage==0) {
					$childDiv.hide();
				}
			});
		}
	);
	
}
/** ENDS *****************************************/


/**
 * get existing metadata
 */
Drupal.behaviors.uportal.managingMetaData.getExistingData = function() {
	$.post(Drupal.uportal.managingMetaData.serverReadURL, {
		'op' : 'get-existing-metadata'
	}).done( function(data) {
		
		//content providers
		$.each(data.content_providers, function(index, providerName) {
			Drupal.uportal.managingMetaData.existingContentProviders[index] = providerName;
		});
		
		//languages
		$.each(data.languages, function(index, languageName) {
			Drupal.uportal.managingMetaData.existingLanguages[index] = languageName;
		});
		
	});
}
/** ENDS *****************************************/


/**
 * initiate metadata page
 */
Drupal.behaviors.uportal.managingMetaData.initiateMetaDataPage = function(context, settings) {
	
	//check page
	if (!(
		$('body').hasClass('page-management-metadata-content-providers')
		|| $('body').hasClass('page-management-metadata-languages')
		|| $('body').hasClass('page-management-metadata-categories')
		|| $('body').hasClass('page-management-metadata-tags')
		|| $('body').hasClass('page-management-edit-content')
		|| $('body').hasClass('page-management-content')
	)) {
		return;
	}
	
	$('body').once( function() {
		
		//initiate globals
		Drupal.behaviors.uportal.managingMetaData.initiateMetaDataType();
		Drupal.uportal.managingMetaData.$resultsWrapper = $('div#block-system-main div.metadata-listing-results');
		Drupal.uportal.managingMetaData.$headerWrapper = $('div#block-system-main div.metadata-listing-page-results-header');

		if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
			Drupal.uportal.managingMetaData.$resultsWrapper = $('section#main-content ul.manage-categories-list');
		}
		
		//set form
		if (!Drupal.uportal.managingMetaData.metaDataType.type) {
			Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper, div#category-dialog-boxes-wrapper div#category-dialog-box');
		}
		if (Drupal.uportal.managingMetaData.metaDataType.type=='content-provider') {
			Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#content-provider-form-wrapper');
		}
		if (Drupal.uportal.managingMetaData.metaDataType.type=='language') {
			Drupal.uportal.managingMetaData.$formDiv = $('div#metadata-forms-wrapper div#language-form-wrapper');
		}
		if (Drupal.uportal.managingMetaData.metaDataType.type=='category') {
			Drupal.uportal.managingMetaData.$formDiv = $('div#category-dialog-boxes-wrapper div#category-dialog-box');
		}
		
		//initiate form
		Drupal.behaviors.uportal.managingMetaData.initiateForm();
		
		//initiate functionality
		Drupal.behaviors.uportal.managingMetaData.initiateAddButton();
		Drupal.behaviors.uportal.managingMetaData.initiateSortButtons();
		Drupal.behaviors.uportal.managingMetaData.initiateSearchBox();
		
		//on window loaded
		$(window).load(function() {
			Drupal.behaviors.uportal.managingMetaData.initiateGraphs();
		});
		
		//get existing data
		Drupal.behaviors.uportal.managingMetaData.getExistingData();
		
	});
	
	Drupal.behaviors.uportal.managingMetaData.initiateDeleteButtons();
	Drupal.behaviors.uportal.managingMetaData.initiateEditButtons();
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.managingMetaData.initiateMetaDataPage);
