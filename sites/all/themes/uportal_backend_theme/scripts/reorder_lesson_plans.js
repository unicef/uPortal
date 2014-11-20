
(function($) {

/**
 * globals we need for reordering lesson plans
 */
Drupal.uportal.reorderLessonPlans = {
	$wrapperDiv : {},
	lessonPlanNID: 0,
	initiator: {},
	currentNIDSOrder: new Array(),
	serverURL : '/management/edit-content/ajax-handler/lesson-plans'
};
/** ENDS *****************************************/


/**
 * initiate sorting lesson plans
 */
Drupal.behaviors.uportal.reorderLessonPlans.initiateLessonPlans = function(data) {
	//prepare lesson plans html
	//initiate dragging capabilities
	//remove class loading
	
	//prepare lesson plans html
	var $dialogContent = $('.dialog-content', Drupal.uportal.reorderLessonPlans.$wrapperDiv).html(data.queue_html);
	var $lessonPlanContentWrapper = $('.lesson-plan-items-wrapper .lesson-plan-items', $dialogContent);
	
	//check for additional nodes
	if (
		typeof data.additionalNodes !== "undefined"
		&& data.additionalNodes.length>0
	) {
		
		//remove this to reduce complexity while draggin
		//$('.lesson-plan-item', $lessonPlanContentWrapper).removeClass('last');
		
		var originalNoItems = data.queue_no_items;
		var additionalNoItems = data.additionalNodes.length;
		var counter = originalNoItems+1;
		$.each(data.additionalNodes, function(index, node) {
			
			//check if exists already
			if (typeof data.queue_nids['nid-'+node.nid] !== "undefined") {
				return;
			}
			
			//item classes
			var itemClasses = 'lesson-plan-item series-plan-item clearfix type-'+node.type;
			itemClasses += (index==additionalNoItems-1) ? ' last' : '';
			
			//item html
			var itemHTML = '<div class="'+itemClasses+'" data-nid="'+node.nid+'">';
			itemHTML += '<div class="grippie float"></div>';
			itemHTML += '<div class="number float">'+(counter++)+'</div>';
			itemHTML += '<div class="icon float"></div>';
			itemHTML += '<div class="title float">'+node.title+'</div>';
			itemHTML += (node.status) ? '<div class="node-status">Draft</div>' : '';
			itemHTML += '</div>';
			
			//append to list
			$lessonPlanContentWrapper.append(itemHTML);
		});
		
		//update number of items (original+additional)
		var fullNoItems = counter-1;
		$('.lesson-plan-title span.number', $dialogContent).html(fullNoItems);
		
	}
	
	//initiate dragging capabilities
	$lessonPlanContentWrapper.sortable({
		placeholder: "series-plan-sorter-placeholder",
		handle: ".grippie",
		update: function (event, ui) {
			
			//get nids data
			var nids = new Array();
			var no = 1;
			$('.lesson-plan-item', $lessonPlanContentWrapper).each(
				function(index, elem) {
					var $lessonPlanItem = $(elem);
					$('.number', $lessonPlanItem).text(no++);
					nids.push($lessonPlanItem.data('nid'));
				}
			);
			Drupal.uportal.reorderLessonPlans.currentNIDSOrder = nids;
			
		}
	});
	
	//remove class loading
	Drupal.uportal.reorderLessonPlans.$wrapperDiv.removeClass('loading');
};
/** ENDS *****************************************/


/**
 * initiate save button
 */
Drupal.behaviors.uportal.reorderLessonPlans.initiateSaveBtn = function() {
	var $btn = $('.dialog-footer .dialog-buttons .yes', Drupal.uportal.reorderLessonPlans.$wrapperDiv);
	$btn.click( function() {
		var eventData = {
			'lessonPlanNID': Drupal.uportal.reorderLessonPlans.lessonPlanNID,
			'nidsOrder': Drupal.uportal.reorderLessonPlans.currentNIDSOrder,
			'initiator': Drupal.uportal.reorderLessonPlans.initiator
		};
		Drupal.uportal.$eventWatcher.trigger('orderingLessonPlanFinished', eventData);
		$.unblockUI();
		
		if (Drupal.uportal.reorderLessonPlans.performServerSave) {
			Drupal.behaviors.uportal.blockUIWaitMessage('Saving Lesson Plan Order', 'Please wait as we save this lesson plan order ...');
			eventData['operation_type'] = 'save-lesson-plan-order';
			$.post(Drupal.uportal.bulkEditor.serverLessonPlansURL, eventData)
				.done( function (data) {
					$.unblockUI();
					Drupal.uportal.$eventWatcher.trigger('savedLessonPlanOrderServerside', eventData);
				});
		}
		
		return false;
	});
};
/** ENDS *****************************************/


/**
 * show dialog
 */
Drupal.behaviors.uportal.reorderLessonPlans.reorder = function(args) {
	var lessonPlanNID = args.lessonPlanNID;
	var initiator = args.initiator;
	var additionalNodes = new Array();
	var saveOnServer = false;
	Drupal.uportal.reorderLessonPlans.lessonPlanNID = lessonPlanNID;
	Drupal.uportal.reorderLessonPlans.initiator = initiator;
	
	//additional nodes
	if (typeof args.addNodes !== "undefined") {
		additionalNodes = args.addNodes;
	}
	
	//save on server
	if (typeof args.saveOnServer !== "undefined") {
		saveOnServer = true;
	}
	
	//request for lesson plans HTML
	$.post(Drupal.uportal.reorderLessonPlans.serverURL, {
		'operation_type' : 'get_lesson_plan_for_reorder',
		'data_values' : {
			'lesson-plan-nid' : lessonPlanNID,
			'initiator' : initiator
		}
	}).done( function(data) {
			if (additionalNodes.length>0) {
				data['additionalNodes'] = additionalNodes;
			}
			data['saveOnServer'] = saveOnServer;
			Drupal.uportal.reorderLessonPlans.performServerSave = saveOnServer;
			Drupal.behaviors.uportal.reorderLessonPlans.initiateLessonPlans(data);
		});
	
	//display dialog
	Drupal.uportal.reorderLessonPlans.$wrapperDiv.addClass('loading');
	$.blockUI({
		message: Drupal.uportal.reorderLessonPlans.$wrapperDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * initiate page
 */
Drupal.behaviors.uportal.reorderLessonPlans.initiateReorderLessonPlan = function(context, settings) {
	
	//check page
	/*
	 * ignore, do everywhere
	if (!($('body').hasClass('page-management-edit-content'))) {
		return;
	}
	*/
	
	$('body').once( function() {
		
		//dialog box
		Drupal.uportal.reorderLessonPlans.$wrapperDiv = $('div#reorder-lesson-plan-dialog-box');
		
		//save button
		Drupal.behaviors.uportal.reorderLessonPlans.initiateSaveBtn();
		
	});
	
	//attach these behaviours all the time
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.reorderLessonPlans.initiateReorderLessonPlan);
