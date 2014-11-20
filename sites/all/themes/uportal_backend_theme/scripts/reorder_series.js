
(function($) {

/**
 * globals we need for reordering series
 */
Drupal.uportal.reorderSeries = {
	$wrapperDiv : {},
	seriesNID: 0,
	performServerSave: false,
	initiator: {},
	currentNIDSOrder: new Array(),
	serverURL : '/management/edit-content/ajax-handler/series'
};
/** ENDS *****************************************/


/**
 * initiate sorting series
 */
Drupal.behaviors.uportal.reorderSeries.initiateSeries = function(data) {
	//prepare series html
	//initiate dragging capabilities
	//remove class loading
	
	//prepare series html
	var $dialogContent = $('.dialog-content', Drupal.uportal.reorderSeries.$wrapperDiv).html(data.queue_html);
	var $seriesContentWrapper = $('.series-items-wrapper .series-items', $dialogContent);
	
	//check for additional nodes
	if (
		typeof data.additionalNodes !== "undefined"
		&& data.additionalNodes.length>0
	) {
		
		//remove this to reduce complexity while draggin
		//$('.series-item', $seriesContentWrapper).removeClass('last');
		
		var originalNoItems = data.queue_no_items;
		var additionalNoItems = data.additionalNodes.length;
		var counter = originalNoItems+1;
		$.each(data.additionalNodes, function(index, node) {
			
			//check if exists already
			if (typeof data.queue_nids['nid-'+node.nid] !== "undefined") {
				return;
			}
			
			//item classes
			var itemClasses = 'series-item series-plan-item clearfix type-'+node.type;
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
			$seriesContentWrapper.append(itemHTML);
		});
		
		//update number of items (original+additional)
		var fullNoItems = counter-1;
		$('.series-title span.number', $dialogContent).html(fullNoItems);
		
	}
	
	//initiate dragging capabilities
	$seriesContentWrapper.sortable({
		placeholder: "series-plan-sorter-placeholder",
		handle: ".grippie",
		update: function (event, ui) {
			
			//get nids data
			var nids = new Array();
			var no = 1;
			$('.series-item', $seriesContentWrapper).each(
				function(index, elem) {
					var $seriesItem = $(elem);
					$('.number', $seriesItem).text(no++);
					nids.push($seriesItem.data('nid'));
				}
			);
			Drupal.uportal.reorderSeries.currentNIDSOrder = nids;
			
		}
	});
	
	//remove class loading
	Drupal.uportal.reorderSeries.$wrapperDiv.removeClass('loading');
};
/** ENDS *****************************************/


/**
 * initiate save button
 */
Drupal.behaviors.uportal.reorderSeries.initiateSaveBtn = function() {
	var $btn = $('.dialog-footer .dialog-buttons .yes', Drupal.uportal.reorderSeries.$wrapperDiv);
	$btn.click( function() {
		var eventData = {
			'seriesNID': Drupal.uportal.reorderSeries.seriesNID,
			'nidsOrder': Drupal.uportal.reorderSeries.currentNIDSOrder,
			'initiator': Drupal.uportal.reorderSeries.initiator
		};
		Drupal.uportal.$eventWatcher.trigger('orderingSeriesFinished', eventData);
		$.unblockUI();
		
		if (Drupal.uportal.reorderSeries.performServerSave) {
			Drupal.behaviors.uportal.blockUIWaitMessage('Saving Series Order', 'Please wait as we save this series order ...');
			eventData['operation_type'] = 'save-series-order';
			$.post(Drupal.uportal.bulkEditor.serverSeriesURL, eventData)
				.done( function (data) {
					$.unblockUI();
					Drupal.uportal.$eventWatcher.trigger('savedSeriesOrderServerside', eventData);
				});
		}
		
		return false;
	});
};
/** ENDS *****************************************/


/**
 * show dialog
 */
Drupal.behaviors.uportal.reorderSeries.reorder = function(args) {
	var seriesNID = args.seriesNID;
	var initiator = args.initiator;
	var additionalNodes = new Array();
	var saveOnServer = false;
	Drupal.uportal.reorderSeries.seriesNID = seriesNID;
	Drupal.uportal.reorderSeries.initiator = initiator;
	
	//additional nodes
	if (typeof args.addNodes !== "undefined") {
		additionalNodes = args.addNodes;
	}
	
	//save on server
	if (typeof args.saveOnServer !== "undefined") {
		saveOnServer = true;
	}
	
	//request for series HTML
	$.post(Drupal.uportal.reorderSeries.serverURL, {
		'operation_type' : 'get_series_for_reorder',
		'data_values' : {
			'series-nid' : seriesNID,
			'initiator' : initiator
		}
	}).done( function(data) {
			if (additionalNodes.length>0) {
				data['additionalNodes'] = additionalNodes;
			}
			data['saveOnServer'] = saveOnServer;
			Drupal.uportal.reorderSeries.performServerSave = saveOnServer;
			Drupal.behaviors.uportal.reorderSeries.initiateSeries(data);
		});
	
	//display dialog
	Drupal.uportal.reorderSeries.$wrapperDiv.addClass('loading');
	$.blockUI({
		message: Drupal.uportal.reorderSeries.$wrapperDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * initiate page
 */
Drupal.behaviors.uportal.reorderSeries.initiateReorderSeries = function(context, settings) {
	
	//check page
	/*
	 * ignore, do everywhere
	if (!($('body').hasClass('page-management-edit-content'))) {
		return;
	}
	*/
	
	$('body').once( function() {
		
		//dialog box
		Drupal.uportal.reorderSeries.$wrapperDiv = $('div#reorder-series-dialog-box');
		
		//save button
		Drupal.behaviors.uportal.reorderSeries.initiateSaveBtn();
		
	});
	
	//attach these behaviours all the time
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.reorderSeries.initiateReorderSeries);
