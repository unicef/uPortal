

(function($) {

/**
 * globals we need for content listing
 */
Drupal.uportal.contentListing = {
	$wrapperDiv : {},
	$operationsDiv : {},
	$displayFormattersDiv : {}
};


/**
 * initiate edit buttons
 */
Drupal.behaviors.uportal.contentListing.initiateEditButtons = function () {
	var $operationsDiv = $('header.page-header .header-strip-3 .content-listing-operations');
	
	//edit button
	$('.edit-btn', $operationsDiv).click(
		Drupal.behaviors.uportal.bulkEditor.goToEditPage
	);
	
	//delete button
	$('.delete-btn', $operationsDiv).click(Drupal.behaviors.uportal.bulkEditor.deleteNodes);
	Drupal.uportal.$eventWatcher.on('bulkEditorDeletedNodes', Drupal.behaviors.uportal.contentListing.nodesDeleted);
	
	//bulk edit button
	$('.bulk-edit-btn', $operationsDiv).click( function() {
		Drupal.behaviors.uportal.bulkEditor.bulkEditNodes();
	});
	Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeMetaData', Drupal.behaviors.uportal.contentListing.nodesBulkEditedMetaData);
	Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeSeries', Drupal.behaviors.uportal.contentListing.nodesBulkEditedSeries);
	Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeLessonPlans', Drupal.behaviors.uportal.contentListing.nodesBulkEditedLessonPlans);
	Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeTags', Drupal.behaviors.uportal.contentListing.nodesBulkEditedTags);
	
}
/** ENDS *****************************************/


/**
 * finished updating node lesson plan
 */
Drupal.behaviors.uportal.contentListing.nodesBulkEditedLessonPlans = function (editEvent, data) {
	editEvent.stopPropagation();
	
	/**
	 * commenting this bit out to fulfill
	 * request to leave selected items AS selected after operations
	 * 
	
	$.each(data.nids, function(index, nid) {
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, Drupal.uportal.contentListing.$wrapperDiv);
		$('input.nid-checkbox', $nodeDiv).attr('checked', false);
	});
	
	//reset select all checkbox and remove operations
	var $operationsDiv = Drupal.uportal.contentListing.$operationsDiv;
	$('input.all-nid-checkbox', $operationsDiv).attr('checked', false);
	$operationsDiv.addClass('no-items-selected');
	
	//reset selected NIDS
	Drupal.uportal.bulkEditor.selectedNIDs = new Array();
	
	*******************/
	
	//disable if empty
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		Drupal.uportal.contentListing.$operationsDiv.addClass('no-items-selected');
	}
	
	//unblock ui
	$.unblockUI();
};
/** ENDS *****************************************/


/**
 * finished updating node tags
 */
Drupal.behaviors.uportal.contentListing.nodesBulkEditedTags = function (editEvent, data) {
	editEvent.stopPropagation();
	
	/**
	 * commenting this bit out to fulfill
	 * request to leave selected items AS selected after operations
	 * 
	
	$.each(data.nids, function(index, nid) {
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, Drupal.uportal.contentListing.$wrapperDiv);
		$('input.nid-checkbox', $nodeDiv).attr('checked', false);
	});
	
	//reset select all checkbox and remove operations
	var $operationsDiv = Drupal.uportal.contentListing.$operationsDiv;
	$('input.all-nid-checkbox', $operationsDiv).attr('checked', false);
	$operationsDiv.addClass('no-items-selected');
	
	//reset selected NIDS
	Drupal.uportal.bulkEditor.selectedNIDs = new Array();
	
	*******************/
	
	//disable if empty
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		Drupal.uportal.contentListing.$operationsDiv.addClass('no-items-selected');
	}
	
	//unblock ui
	$.unblockUI();
};
/** ENDS *****************************************/


/**
 * finished updating node series
 *
 * commented out bits that reset all selected checkboxes
 */
Drupal.behaviors.uportal.contentListing.nodesBulkEditedSeries = function (editEvent, data) {
	editEvent.stopPropagation();
	
	var newSeries = data.data_values.series;
	var newCategory = data.data_values.category;
	var nidsChanged = data.data_values.nids_changed;
	
	$.each(data.nids, function(index, nid) {
		
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, Drupal.uportal.contentListing.$wrapperDiv);
		
		/**
		 * do NOT reset selected checkboxes
		$('input.nid-checkbox', $nodeDiv).attr('checked', false);
		*********/
		
		//only work on changed nodes
		if (!($.inArray(nid, nidsChanged) > -1)) {
			return true;
		}
		
		if (newCategory!=0 && newCategory!='0') {
			$('.desc .meta-info .category', $nodeDiv).text(newCategory.title).show();
			$('.desc .meta-info .series', $nodeDiv).text('-').show();
		}
		if (newSeries!=0 && newSeries!='0') {
			$('.desc .meta-info .category', $nodeDiv).text(newCategory.title).show();
			$('.desc .meta-info .series', $nodeDiv).text(newSeries.title).show();
		}
		
		//check if published action was tried
		Drupal.behaviors.uportal.contentListing.updateNodeFailedPublishMsg($nodeDiv);
		
		if (
			(newCategory!=0 && newCategory!='0' && Drupal.uportal.bulkEditor.pageCategoryNID!='all' && newCategory.nid!=Drupal.uportal.bulkEditor.pageCategoryNID)
			|| (newSeries!=0 && newSeries!='0' && Drupal.uportal.bulkEditor.pageSeriesNID!='all' && newSeries.nid!=Drupal.uportal.bulkEditor.pageSeriesNID)
		) {
			Drupal.uportal.bulkEditor.selectedNIDs.splice( $.inArray(nid, Drupal.uportal.bulkEditor.selectedNIDs), 1 );
			$nodeDiv.remove();
		}
		
		return true;
	});
	
	/**
	 * leave selected checkboxes as selected
	 * 
	//reset select all checkbox and remove operations
	var $operationsDiv = Drupal.uportal.contentListing.$operationsDiv;
	$('input.all-nid-checkbox', $operationsDiv).attr('checked', false);
	$operationsDiv.addClass('no-items-selected');
	
	//reset selected NIDS
	Drupal.uportal.bulkEditor.selectedNIDs = new Array();
	
	***********/
	
	//disable if empty
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		Drupal.uportal.contentListing.$operationsDiv.addClass('no-items-selected');
	}
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * finished updating node meta data
 */
Drupal.behaviors.uportal.contentListing.nodesBulkEditedMetaData = function (editEvent, data) {
	editEvent.stopPropagation();
	
	var newCategory = data.data_values.category;
	var newContentProvider = data.data_values.content_provider;
	var newLanguage = data.data_values.language;
	var nidsWithChangedCategory = data.data_values.nids_category_changed;
	
	$.each(data.nids, function(index, nid) {
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, Drupal.uportal.contentListing.$wrapperDiv);
		if (
			(newCategory!=0 && newCategory!='0' && Drupal.uportal.bulkEditor.pageCategoryNID!='all' && newCategory.nid!=Drupal.uportal.bulkEditor.pageCategoryNID)
			|| (newContentProvider!=0 && newContentProvider!='0' && Drupal.uportal.bulkEditor.pageContentProviderNID!='all' && newContentProvider.nid!=Drupal.uportal.bulkEditor.pageContentProviderNID)
			|| (newLanguage!=0 && newLanguage!='0' && Drupal.uportal.bulkEditor.pageLanguageCode!='all' && newLanguage.code!=Drupal.uportal.bulkEditor.pageLanguageCode)
		) {
			Drupal.uportal.bulkEditor.selectedNIDs.splice( $.inArray(nid, Drupal.uportal.bulkEditor.selectedNIDs), 1 );
			$nodeDiv.remove();
		} else {
			if (newCategory!=0 && newCategory!='0' && $.inArray(nid, nidsWithChangedCategory)>-1) {
				$('.desc .meta-info .category', $nodeDiv).text(newCategory.title).show();
				$('.desc .meta-info .series', $nodeDiv).text('-').show();
			}
			if (newContentProvider!=0 && newContentProvider!='0') {
				$('.desc .meta-info .provider', $nodeDiv).text(newContentProvider.title).show();
			}
			if (newLanguage!=0 && newLanguage!='0') {
				$('.desc .meta-info .language', $nodeDiv).text(newLanguage.name).show();
			}
			
			/**
			 * leave checkbox as selected
			 * 
			$('input.nid-checkbox', $nodeDiv).attr('checked', false);
			****************/
			
			Drupal.behaviors.uportal.contentListing.updateNodeFailedPublishMsg($nodeDiv);
			
		}
	});
	
	/**
	 * leave selected checkboxes
	 * 
	//reset select all checkbox and remove operations
	var $operationsDiv = Drupal.uportal.contentListing.$operationsDiv;
	$('input.all-nid-checkbox', $operationsDiv).attr('checked', false);
	$operationsDiv.addClass('no-items-selected');
	
	//reset selected NIDS
	Drupal.uportal.bulkEditor.selectedNIDs = new Array();
	
	***********/
	
	//disable if empty
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		Drupal.uportal.contentListing.$operationsDiv.addClass('no-items-selected');
	}
	
	//unblock ui
	$.unblockUI();
	
}
/** ENDS *****************************************/


/**
 * finished deleting nodes
 */
Drupal.behaviors.uportal.contentListing.nodesDeleted = function (deleteEvent, data) {
	deleteEvent.stopPropagation();
	
	var nids = data.nids;
	var $operationsDiv = Drupal.uportal.contentListing.$operationsDiv;
	var $contentDiv = $('.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv);
	
	//reset select all checkbox and remove operations
	$('input.all-nid-checkbox', $operationsDiv).attr('checked', false);
	$('.operations', Drupal.uportal.contentListing.$wrapperDiv).addClass('no-items-selected');
	
	//remove deleted content
	$.each(nids, function (index, nid) {
		$('div#content-listing-item-nid-'+nid, $contentDiv).remove();
	});
	
	//reset selected NIDS
	Drupal.uportal.bulkEditor.selectedNIDs = new Array();
	
	//disable if empty
	if (Drupal.uportal.bulkEditor.selectedNIDs.length<=0) {
		Drupal.uportal.contentListing.$operationsDiv.addClass('no-items-selected');
	}
	
	//unblock ui
	$.unblockUI();
	
};
/** ENDS *****************************************/


/**
* initiate checkboxes
*/
Drupal.behaviors.uportal.contentListing.initiateCheckboxes = function() {
	var $contentWrapper = $('.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv);
	var $operationsWrapper = Drupal.uportal.contentListing.$operationsDiv;
	
	//check user rights
	if (
		typeof Drupal.uportal.userPerms.disallowed !== 'undefined'
		&& ($.inArray('edit-content', Drupal.uportal.userPerms.disallowed) > -1)
	) {
		$('input.nid-checkbox', $contentWrapper).attr("disabled", true);
		$('input.all-nid-checkbox', $operationsWrapper).attr("disabled", true);
		return;
	}
	
	$('input.nid-checkbox', $contentWrapper).click( function() {
		var $checkbox = $(this);
		var nid = $checkbox.val();
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, Drupal.uportal.contentListing.$wrapperDiv);
		if ($checkbox.is(':checked')) {
			Drupal.uportal.bulkEditor.selectedNIDs.push(nid);
			$nodeDiv.addClass('checkbox-selected');
		} else {
			Drupal.uportal.bulkEditor.selectedNIDs.splice( $.inArray(nid, Drupal.uportal.bulkEditor.selectedNIDs), 1 );
			$nodeDiv.removeClass('checkbox-selected');
		}
		if (Drupal.uportal.bulkEditor.selectedNIDs.length>0) {
			$operationsWrapper.removeClass('no-items-selected');
		} else {
			$operationsWrapper.addClass('no-items-selected');
		}
	});
	$('input.all-nid-checkbox', $operationsWrapper).click( function() {
		var $checkbox = $(this);
		var $listingItems = $('.content-listing-item:visible', $contentWrapper);
		if ($checkbox.is(':checked')) {
			Drupal.uportal.bulkEditor.selectedNIDs = new Array();
			$('input.nid-checkbox', $listingItems).attr('checked', true).each(
				function (index, elem) {
					Drupal.uportal.bulkEditor.selectedNIDs.push($(elem).val());
				}
			);
			$operationsWrapper.removeClass('no-items-selected');
			$listingItems.addClass('checkbox-selected');
		} else {
			$('input.nid-checkbox', $listingItems).attr('checked', false);
			$operationsWrapper.addClass('no-items-selected');
			Drupal.uportal.bulkEditor.selectedNIDs = new Array();
			$listingItems.removeClass('checkbox-selected');
		}
	});
};
/** ENDS *****************************************/


/**
* sorting content
*/
Drupal.behaviors.uportal.contentListing.initiateContentSorters = function() {
	var $sortersWrapper = $('ul.sort-options', Drupal.uportal.contentListing.$displayFormattersDiv);
	var $contentWrapper = $('.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv);
	
	$('li a', $sortersWrapper).each( function (index, elem) {
		var $a = $(elem);
		$a.click( function() {
			var $anchor = $(this);
			var $li = $anchor.parent('li');
			
			//remove active class
			$('li', $sortersWrapper).removeClass('active');
			
			//sort items using tiny sort
			if ($li.hasClass('sort-updated')) {
				$('.content-listing-item', $contentWrapper).tsort({order:'desc', attr:'data-updated'});
			}
			if ($li.hasClass('sort-created')) {
				$('.content-listing-item', $contentWrapper).tsort({order:'desc', attr:'data-created'});
			}
			if ($li.hasClass('sort-alphabetic')) {
				$('.content-listing-item', $contentWrapper).tsort('.desc h2 a', {order:'asc'});
			}
			
			$li.addClass('active');
			return false;
		})
	})
};
/** ENDS *****************************************/


/**
* changing content display
*/
Drupal.behaviors.uportal.contentListing.initiateContentFormatters = function() {
	$('.btn', Drupal.uportal.contentListing.$displayFormattersDiv).each( function (index, elem) {
		var $btn = $(elem);
		$btn.click( function() {
			var $$ = $(this);
			var $contentWrapper = $('.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv);
			$('.btn', Drupal.uportal.contentListing.$displayFormattersDiv).removeClass('active');
			$$.addClass('active');
			if ($$.hasClass('expanded')) {
				$contentWrapper.removeClass('collapsed-items-wrapper');
			}
			if ($$.hasClass('collapsed')) {
				$contentWrapper.addClass('collapsed-items-wrapper');
			}
		});
	});
};
/** ENDS *****************************************/


/**
 * initiate search box
 */
Drupal.behaviors.uportal.contentListing.initiateSearchBox = function() {
	
	if ($('body').hasClass('page-management-content-individual-items')) {
		return;
	}
	
	var $resultsWrapper = Drupal.uportal.contentListing.$wrapperDiv;
	var placeholder = "Filter by keyword ...";
	var $searchBox = $('<input type="text" class="search-box live-filter-search-box" id="search-box" value="" placeholder="'+placeholder+'" />');
	$searchBox.appendTo($('header.page-header .header-strip-3 .total-clear-link'));
	$resultsWrapper.liveFilter(
		'header.page-header input#search-box',
		'.content-listing-item', {
			filterChildSelector: '.desc'
		}
	);
}
/** ENDS *****************************************/


/**
 * edit series info
 * - on series listing page
 */
Drupal.behaviors.uportal.contentListing.editSeries = function(seriesNID) {

	if (!Drupal.behaviors.uportal.userIsAllowed('manage-series')) {
		return;
	}
	
	var $seriesItemDiv = $('div#series-listing-item-nid-'+seriesNID, Drupal.uportal.contentListing.$wrapperDiv);
	
	if ($seriesItemDiv.length<=0) {
		return;
	}
	
	var args = {
		default_data: {
			categoryNID: $seriesItemDiv.data('category-nid'),
			seriesNID: seriesNID,
			seriesTitle: $('.desc h2', $seriesItemDiv).text(),
			seriesBody: $('.desc .body', $seriesItemDiv).text(),
		},
		initiator: 'series-listing-page'
	};
	Drupal.behaviors.uportal.bulkEditor.showCreateEditSeriesForm(args);
	$.blockUI({ 
		message: Drupal.uportal.bulkEditor.$formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * delete series
 * - on series listing page
 */
Drupal.behaviors.uportal.contentListing.deleteSeries = function(seriesNID) {

	if (!Drupal.behaviors.uportal.userIsAllowed('manage-series')) {
		return false;
	}
	
	var $seriesItemDiv = $('div#series-listing-item-nid-'+seriesNID, Drupal.uportal.contentListing.$wrapperDiv);
	
	if ($seriesItemDiv.length<=0) {
		return false;
	}
	
	var args = {
		categoryNID: $seriesItemDiv.data('category-nid'),
		seriesNID: seriesNID,
		title: $('.desc h2', $seriesItemDiv).text(),
		initiator: 'series-listing-page'
	};
	var $confirmationDiv = $('div#confirm-deletion');
	$('.dialog-header', $confirmationDiv).text('Delete Series?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete this series: <em>'+args.title+'</em>?');
	$('.dialog-footer .yes', $confirmationDiv).click( function() {
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting Series ...', 'Please wait as this series: <em>'+args.title+'</em> is deleted ...');
		$.post(Drupal.uportal.bulkEditor.serverSeriesURL, {
			'operation_type' : 'delete-series',
			'nid' : seriesNID
		}).done( function(data) {
				$seriesItemDiv.remove();
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

};
/** ENDS *****************************************/


/**
 * edit lesson plan info
 * - on lesson plan listing page
 */
Drupal.behaviors.uportal.contentListing.editLessonPlan = function(lessonPlanNID) {

	if (!Drupal.behaviors.uportal.userIsAllowed('manage-lesson-plans')) {
		return;
	}
	
	var $lessonPlanItemDiv = $('div#lesson-plan-listing-item-nid-'+lessonPlanNID, Drupal.uportal.contentListing.$wrapperDiv);
	
	if ($lessonPlanItemDiv.length<=0) {
		return;
	}
	
	var args = {
		default_data: {
			lessonPlanNID: lessonPlanNID,
			lessonPlanTitle: $('.desc h2', $lessonPlanItemDiv).text(),
			lessonPlanBody: $('.desc .body', $lessonPlanItemDiv).text(),
		},
		initiator: 'lesson-plan-listing-page'
	};
	Drupal.behaviors.uportal.bulkEditor.showCreateEditLessonPlanForm(args);
	$.blockUI({ 
		message: Drupal.uportal.bulkEditor.$formsWrapper,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * delete series
 * - on series listing page
 */
Drupal.behaviors.uportal.contentListing.deleteLessonPlan = function(lessonPlanNID) {

	if (!Drupal.behaviors.uportal.userIsAllowed('manage-lesson-plans')) {
		return false;
	}
	
	var $lessonPlanItemDiv = $('div#lesson-plan-listing-item-nid-'+lessonPlanNID, Drupal.uportal.contentListing.$wrapperDiv);
	
	if ($lessonPlanItemDiv.length<=0) {
		return false;
	}
	
	var args = {
		categoryNID: $lessonPlanItemDiv.data('category-nid'),
		lessonPlanNID: lessonPlanNID,
		title: $('.desc h2', $lessonPlanItemDiv).text(),
		initiator: 'lesson-plan-listing-page'
	};
	var $confirmationDiv = $('div#confirm-deletion');
	$('.dialog-header', $confirmationDiv).text('Delete Lesson Plan?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete this lesson plan: <em>'+args.title+'</em>?');
	$('.dialog-footer .yes', $confirmationDiv).click( function() {
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting Lesson Plan ...', 'Please wait as this lesson plan: <em>'+args.title+'</em> is deleted ...');
		$.post(Drupal.uportal.bulkEditor.serverLessonPlansURL, {
			'operation_type' : 'delete-lesson-plan',
			'nid' : lessonPlanNID
		}).done( function(data) {
				$lessonPlanItemDiv.remove();
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

};
/** ENDS *****************************************/


/**
 * new series created or edited
 */
Drupal.behaviors.uportal.contentListing.newSeriesCreatedEdited = function(event, data) {
	
	if (!($('body').hasClass('page-management-content-series'))) {
		return;
	}
	
	var seriesNID = data.nid;
	var parentCategoryNID = data.category_nid;
	var seriesTitle = data.title;
	var seriesBody = data.body;
	var initiator = data.initiator;
	var edit_operation = (data.edit_operation=='1'); //0 or 1
	
	//check if node belongs to the current filters esp. category
	if (Drupal.uportal.bulkEditor.pageCategoryNID!='all' && parentCategoryNID!=Drupal.uportal.bulkEditor.pageCategoryNID) {
		$('div#series-listing-item-nid-'+seriesNID, Drupal.uportal.contentListing.$wrapperDiv).remove();
		$.unblockUI();
		return;
	}
	
	if (edit_operation) {
		var $seriesNodeDiv = $('div#series-listing-item-nid-'+seriesNID, Drupal.uportal.contentListing.$wrapperDiv);
		$('h2', $seriesNodeDiv).text(seriesTitle);
		$('.body', $seriesNodeDiv).text(seriesBody);
		$('.category-title', $seriesNodeDiv).text(data.category_title);
	} else {
		var $seriesNodeDiv = $(data.series_node_html);
		$seriesNodeDiv.appendTo($('div.series-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv));
	}
	
	$.unblockUI();
	
};
/** ENDS *****************************************/


/**
 * new lesson plan created or edited
 */
Drupal.behaviors.uportal.contentListing.newLessonPlanCreatedEdited = function(event, data) {
	
	if (!($('body').hasClass('page-management-content-lesson-plans'))) {
		return;
	}
	
	var lessonPlanNID = data.nid;
	var parentCategoryNID = data.category_nid;
	var lessonPlanTitle = data.title;
	var lessonPlanBody = data.body;
	var initiator = data.initiator;
	var edit_operation = (data.edit_operation=='1'); //0 or 1
	
	//check if node belongs to the current filters esp. category
	if (Drupal.uportal.bulkEditor.pageCategoryNID!='all' && parentCategoryNID!=Drupal.uportal.bulkEditor.pageCategoryNID) {
		$('div#lesson-plan-listing-item-nid-'+lessonPlanNID, Drupal.uportal.contentListing.$wrapperDiv).remove();
		$.unblockUI();
		return;
	}
	
	if (edit_operation) {
		var $lessonPlanNodeDiv = $('div#lesson-plan-listing-item-nid-'+lessonPlanNID, Drupal.uportal.contentListing.$wrapperDiv);
		$('h2', $lessonPlanNodeDiv).text(lessonPlanTitle);
		$('.body', $lessonPlanNodeDiv).text(lessonPlanBody);
		$('.category-title', $lessonPlanNodeDiv).text(data.category_title);
	} else {
		var $lessonPlanNodeDiv = $(data.lesson_plan_node_html);
		$lessonPlanNodeDiv.appendTo($('div.lesson-plan-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv));
	}
	
	$.unblockUI();
	
};
/** ENDS *****************************************/


/**
 * initiate series edit buttons
 */
Drupal.behaviors.uportal.contentListing.intitiateEditSeriesBtns = function() {
	
	//check user rights
	if (
		typeof Drupal.uportal.userPerms.disallowed !== 'undefined'
		&& ($.inArray('manage-series', Drupal.uportal.userPerms.disallowed) > -1)
	) {
		return;
	}
	
	$('div.edit-series.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			if ($btn.hasClass("disabled")) {
				return;
			}
			Drupal.behaviors.uportal.contentListing.editSeries($btn.data('series-nid'));
		}
	);
	$('div.delete-series.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			Drupal.behaviors.uportal.contentListing.deleteSeries($btn.data('series-nid'));
		}
	);
	$('div.order-series.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			var seriesNID = $btn.data('series-nid');
			if ($btn.hasClass("disabled")) {
				return;
			}
			var args = {
				'seriesNID': seriesNID,
				'initiator': 'series',
				'saveOnServer': true,
			};
			Drupal.behaviors.uportal.reorderSeries.reorder(args);
		}
	);
};
/** ENDS *****************************************/


/**
 * initiate lesson plan edit buttons
 */
Drupal.behaviors.uportal.contentListing.intitiateEditLessonPlanBtns = function() {
	
	//check user rights
	if (
		typeof Drupal.uportal.userPerms.disallowed !== 'undefined'
		&& ($.inArray('manage-lesson-plans', Drupal.uportal.userPerms.disallowed) > -1)
	) {
		return;
	}
	
	$('div.edit-lesson-plan.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			if ($btn.hasClass("disabled")) {
				return;
			}
			Drupal.behaviors.uportal.contentListing.editLessonPlan($btn.data('lesson-plan-nid'));
		}
	);
	$('div.delete-lesson-plan.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			Drupal.behaviors.uportal.contentListing.deleteLessonPlan($btn.data('lesson-plan-nid'));
		}
	);
	$('div.order-lesson-plan.edit-btn', Drupal.uportal.contentListing.$wrapperDiv).click(
		function() {
			var $btn = $(this);
			var lessonPlanNID = $btn.data('lesson-plan-nid');
			if ($btn.hasClass("disabled")) {
				return;
			}
			var args = {
				'lessonPlanNID': lessonPlanNID,
				'initiator': 'lesson-plans',
				'saveOnServer': true,
			};
			Drupal.behaviors.uportal.reorderLessonPlans.reorder(args);
		}
	);
};
/** ENDS *****************************************/


/**
 * highlight any searched text
 */
Drupal.behaviors.uportal.contentListing.highlightSearchText = function() {
	var searchText = $('header.page-header .search-form-wrapper .form-type-textfield input.form-text').val();
	if (searchText) {
		Drupal.uportal.contentListing.$wrapperDiv.highlight(searchText, { className: 'highlight-search-text' });
	}
};
/** ENDS *****************************************/


/**
 * create a message for a node showing missing fields required for a successful publish
 */
Drupal.behaviors.uportal.contentListing.updateNodeFailedPublishMsg = function($nodeDiv) {
	
	if (!$nodeDiv.hasClass('failed-publish')) {
		return;
	}
	
	var description = $('div.desc div.body', $nodeDiv).text();
	var category = $('div.desc div.category', $nodeDiv).text();
	var content_provider = $('div.desc div.provider', $nodeDiv).text();
	var language = $('div.desc div.language', $nodeDiv).text();
	
	//make message
	var msg = 'Bulk publish failed due to missing: ';
	var missingFields = new Array();
	if (!category || category=='-') missingFields.push('Category');
	if (!content_provider || content_provider=='-') missingFields.push('Content Provider');
	if (!language || language=='-') missingFields.push('Language');
	if (!description || description=='-') missingFields.push('Description');
	msg += missingFields.join(', ');
	
	//check if all are still missing
	if (missingFields.length<=0) {
		msg = 'This content can now be published!';
		$nodeDiv.addClass('failed-publish-possible');
	}
	
	//remove any existing message and add a new one
	$('div.failed-publish-msg', $nodeDiv).remove();
	
	//add new message
	var $msgDiv = $('<div class="failed-publish-msg">'+msg+'</div>').appendTo($nodeDiv);
	
};
/** ENDS *****************************************/


/**
 * after updating nodes status, update node divs
 */
Drupal.behaviors.uportal.contentListing.updateNodesStatus = function(e, data) {
	var $nodesWrapper = Drupal.uportal.contentListing.$wrapperDiv;
	var changedNIDs = data.changed_nids;
	var failedNIDs = data.failed_nids;
	var newStatus = data.status;
	
	//update divs
	$.each(changedNIDs, function(index, nid) {
		var $nodeDiv = $('div#content-listing-item-nid-'+nid, $nodesWrapper);
		if (newStatus=='1') {
			$('div.published-status', $nodeDiv).remove();
			$nodeDiv.removeClass('failed-publish');
		} else {
			$('<div class="published-status">Draft</div>').appendTo($('.desc', $nodeDiv));
		}
		
		if (Drupal.uportal.bulkEditor.pageStatus!='all' && newStatus!=Drupal.uportal.bulkEditor.pageStatus) {
			Drupal.uportal.bulkEditor.selectedNIDs.splice( $.inArray(nid, Drupal.uportal.bulkEditor.selectedNIDs), 1 );
			$nodeDiv.remove();
		}
		
	});
	
	//alert for failed NIDs - failed to publish
	$.each(failedNIDs, function(index, nid) {
		if (newStatus=='1') {
			var $nodeDiv = $('div#content-listing-item-nid-'+nid, $nodesWrapper).addClass('failed-publish');
			Drupal.behaviors.uportal.contentListing.updateNodeFailedPublishMsg($nodeDiv);
		}
	});
};
/** ENDS *****************************************/


/**
 * initiate content listing page functionality
 */
Drupal.behaviors.uportal.contentListing.initiateContentListingPage = function() {
	
	//check page
	if (!($('body').hasClass('page-management-content'))) {
		return;
	}
	
	$('body.page-management-content').once( function() {
		
		//tap into metadata functionality
		Drupal.uportal.managingMetaData.addTopButtonsSearch = false;
		
		Drupal.uportal.contentListing.$wrapperDiv = $('.page-col-1 section#main-content .view-content');
		Drupal.uportal.contentListing.$operationsDiv = $('header.page-header .header-strip-3 .content-listing-operations');
		Drupal.uportal.contentListing.$displayFormattersDiv = $('header.page-header .header-strip-2 .settings-menu');
		Drupal.uportal.bulkEditor.pageCategoryNID = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('category-nid');
		Drupal.uportal.bulkEditor.pageContentProviderNID = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('content-provider-nid');
		Drupal.uportal.bulkEditor.pageLanguageCode = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('language-code');
		Drupal.uportal.bulkEditor.pageSeriesNID = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('series-nid');
		Drupal.uportal.bulkEditor.pageLessonPlanNID = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('lesson-plan-nid');
		Drupal.uportal.bulkEditor.pageStatus = $('div.content-listing-wrapper', Drupal.uportal.contentListing.$wrapperDiv).data('status');
		
		//content sorting is now a server-side thing
		//Drupal.behaviors.uportal.contentListing.initiateContentSorters();
		
		Drupal.behaviors.uportal.contentListing.initiateContentFormatters();
		Drupal.behaviors.uportal.contentListing.initiateCheckboxes();
		Drupal.behaviors.uportal.contentListing.initiateEditButtons();
		Drupal.behaviors.uportal.contentListing.initiateSearchBox();
		Drupal.behaviors.uportal.contentListing.highlightSearchText();
		
		//series listing page functionality
		Drupal.uportal.$eventWatcher.on('bulkEditorCreatedNewSeries', Drupal.behaviors.uportal.contentListing.newSeriesCreatedEdited);
		Drupal.behaviors.uportal.contentListing.intitiateEditSeriesBtns();
		
		//lesson plans listing page functionality
		Drupal.uportal.$eventWatcher.on('bulkEditorCreatedNewLessonPlan', Drupal.behaviors.uportal.contentListing.newLessonPlanCreatedEdited);
		Drupal.behaviors.uportal.contentListing.intitiateEditLessonPlanBtns();
		
		//watch for change of status
		Drupal.uportal.$eventWatcher.on('bulkEditorEditedNodeStatus', Drupal.behaviors.uportal.contentListing.updateNodesStatus);
		
		//turn filter selects into chosens
		//$('.page-col-0 .filters-dropdown-wrapper select:not([multiple="multiple"])').each(
		$('.page-col-0 .filters-dropdown-wrapper select').each(
			function(index, elem) {
			var $select = $(elem);
			$select.chosen();
			$select.on("change", function(e, params) {
				var $$ = $(this);
				var url = (params.selected) ? params.selected : params.deselected;
				window.location.href = window.location.protocol
					+ '//'
					+ window.location.host
					+ url;
			});
		});
		
	});
};
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.contentListing.initiateContentListingPage);
