
(function($) {


/**
 * globals we need for content listing
 */
Drupal.uportal.managingUsers = {
	$resultsWrapper : {},
	$headerWrapper : {},
	serverDeleteURL : '/management/users/delete'
};
/** ENDS *****************************************/


/**
 * initiate all delete buttons
 */
Drupal.behaviors.uportal.managingUsers.initiateDeleteButtons = function() {
	if (Drupal.behaviors.uportal.userIsAllowed('manage-users')) {
		$('a.delete-user', Drupal.uportal.managingUsers.$resultsWrapper).click( function() {
			var $btn = $(this);
			var uid = $btn.data('uid');
			var $userDiv = $('#uid-'+uid, Drupal.uportal.managingUsers.$resultsWrapper);
			var userName = $('span.main-result-title a', $userDiv).text();
			
			//delete content
			Drupal.behaviors.uportal.managingUsers.deleteUser(userName, uid, $userDiv);
			
		});
	} else {
		$('a.delete-user', Drupal.uportal.managingUsers.$resultsWrapper).addClass('disabled');
	}
}
/** ENDS *****************************************/


/**
 * initiate all edit buttons
 */
Drupal.behaviors.uportal.managingUsers.initiateEditButtons = function() {
	if (!Drupal.behaviors.uportal.userIsAllowed('manage-users')) {
		$('a.edit-user', Drupal.uportal.managingUsers.$resultsWrapper).addClass('disabled').click( function() {
			return false;
		})
	}
}
/** ENDS *****************************************/


/**
 * delete user
 */
Drupal.behaviors.uportal.managingUsers.deleteUser = function(userName, uid, $removeDiv) {
	var $confirmationDiv = $('div#confirm-deletion');
	$('.dialog-header', $confirmationDiv).text('Delete User?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete this user: <em>'+userName+'</em>?');
	$('.dialog-footer .yes').click( function() {
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting User ...', 'Please wait as we delete this user: <em>'+userName+'</em> ...');
		$.post(Drupal.uportal.managingUsers.serverDeleteURL, {
			'data_values' : {
				'uid' : uid
			}
		})
			.done( function(data) {
				$removeDiv.remove();
				$.unblockUI();
			});
	});
	
	//block UI with confirm message
	$.blockUI({
		message:$confirmationDiv,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
}
/** ENDS *****************************************/


/**
 * initiate sort buttons
 */
Drupal.behaviors.uportal.managingUsers.initiateSortButtons = function() {
	var $sorters = $('.name-label, .user-status-label, .last-access-label', Drupal.uportal.managingUsers.$headerWrapper);
	var $contentWrapper = Drupal.uportal.managingUsers.$resultsWrapper;
	
	$sorters.each( function (index, elem) {
		var $sorter = $(elem);
		$sorter.click( function() {
			var $sorter = $(this);
			
			//remove active class
			$('.col-label', Drupal.uportal.managingUsers.$headerWrapper).removeClass('active');
			
			//get sort order
			var sortOrder = 'asc';
			var currentOrder = $sorter.data('direction');
			if (currentOrder=='asc') sortOrder = 'desc';
			if (currentOrder=='desc') sortOrder = 'asc';
			
			//sort items using tiny sort
			if ($sorter.hasClass('name-label')) {
				$('.full-result-row', $contentWrapper).tsort('.username', {order:sortOrder});
			}
			if ($sorter.hasClass('user-status-label')) {
				$('.full-result-row', $contentWrapper).tsort('.user-status', {order:sortOrder});
			}
			if ($sorter.hasClass('last-access-label')) {
				$('.full-result-row', $contentWrapper).tsort({order:sortOrder, attr:'data-last-access'});
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
Drupal.behaviors.uportal.managingUsers.initiateSearchBox = function() {
	var $resultsWrapper = Drupal.uportal.managingUsers.$resultsWrapper;
	var placeholder = "Search Users ...";
	var $searchBox = $('<input type="text" class="search-box live-filter-search-box" id="search-box" value="" placeholder="'+placeholder+'" />');
	$searchBox.appendTo($('header.page-header .header-strip-1 .col-2-header'));
	$resultsWrapper.liveFilter(
		'header.page-header input#search-box',
		'.full-result-row', {
			filterChildSelector: '.main-result-title a'
		}
	);
}
/** ENDS *****************************************/


/**
 * initiate add button
 */
Drupal.behaviors.uportal.managingUsers.initiateAddButton = function() {
	var btnText = "Add User";
	var $createNewBtn = $('<a class="btn user-btn" href="/management/users/add">'+btnText+'</a>');
	if (!Drupal.behaviors.uportal.userIsAllowed('manage-users')) {
		$createNewBtn = $('<a class="btn user-btn disabled" href="javascript:;">'+btnText+'</a>');
	}
	$createNewBtn.appendTo($('header.page-header .header-strip-1 .col-2-header'));
};
/** ENDS *****************************************/


/**
 * initiate metadata page
 */
Drupal.behaviors.uportal.managingUsers.initiateManageUserPage = function(context, settings) {
	
	//check page
	if (!($('body').hasClass('management-users-list'))) {
		return;
	}
	
	$('body').once( function() {
		
		//initiate globals
		Drupal.uportal.managingUsers.$resultsWrapper = $('section#main-content ul.manage-users-list');
		Drupal.uportal.managingUsers.$headerWrapper = $('section#main-content div.metadata-listing-page-results-header ul.manage-users-list-header');
		
		//initiate functionality
		Drupal.behaviors.uportal.managingUsers.initiateSortButtons();
		Drupal.behaviors.uportal.managingUsers.initiateAddButton();
		Drupal.behaviors.uportal.managingUsers.initiateSearchBox();
		
	});
	
	Drupal.behaviors.uportal.managingUsers.initiateDeleteButtons();
	Drupal.behaviors.uportal.managingUsers.initiateEditButtons();
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.managingUsers.initiateManageUserPage);
