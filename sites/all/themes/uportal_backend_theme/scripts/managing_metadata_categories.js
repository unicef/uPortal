
(function($) {


/**
 * globals we need for content listing
 */
Drupal.uportal.managingCategories = {
	$categoriesWrapper : {},
	$statusDiv : {},
	serverURL : '/management/metadata/categories/operations',
	serverReadURL : '/management/metadata/categories/operations/read-only',
	existingCategories : {}
};
/** ENDS *****************************************/


/**
 * show status message
 */
Drupal.behaviors.uportal.managingCategories.showOrderCategoriesStatus = function(status) {
	var $statusDiv = Drupal.uportal.managingCategories.$statusDiv;
	var showStatusTimer;
	
	if (showStatusTimer) {
		clearTimeout(showStatusTimer);
	}
	
	if (status=='saving') {
		$statusDiv.removeClass('finished');
		$statusDiv.removeClass('unsaved');
		$statusDiv.addClass('loading');
		$statusDiv.stop().fadeIn(300);
	}
	if (status=='unsaved') {
		$statusDiv.removeClass('finished');
		$statusDiv.removeClass('loading');
		$statusDiv.addClass('unsaved');
		$statusDiv.stop().fadeIn(300);
	}
	if (status=='finished') {
		$statusDiv.removeClass('loading');
		$statusDiv.removeClass('unsaved');
		$statusDiv.addClass('finished');
		showStatusTimer = setTimeout( function() {
			$statusDiv.stop().slideUp(300);
		}, 3000);
	}
}
/** ENDS *****************************************/


/**
 * make categories sortable
 */
Drupal.behaviors.uportal.managingCategories.makeCategoriesSortable = function() {
	
	//check permissions - if not allowed, exit
	if (!(
		(Drupal.uportal.managingMetaData.metaDataType.type=='category' && Drupal.behaviors.uportal.userIsAllowed('manage-categories'))
	)) {
		return;
	}
	
	Drupal.uportal.managingCategories.$categoriesWrapper.sortable({
		placeholder: "category-sorter-placeholder",
		handle: ".handle",
		update: function (event, ui) {
			
			//update message
			Drupal.behaviors.uportal.managingCategories.showOrderCategoriesStatus('unsaved');
			
			//update numbers
			var no = 1;
			$('li', Drupal.uportal.managingCategories.$categoriesWrapper).each(
				function(index, elem) {
					var $li = $(elem);
					$('.menu-order', $li).text(no++);
				}
			);
		}
	});
}
/** ENDS *****************************************/


/**
 * get existing categories
 */
Drupal.behaviors.uportal.managingCategories.getExistingCategories = function() {
	$.post(Drupal.uportal.managingCategories.serverReadURL, {
		'op' : 'get-existing-categories'
	}).done( function(data) {
		$.each(data.categories, function(index, categoryName) {
			Drupal.uportal.managingCategories.existingCategories[index] = categoryName;
		});
	});
}
/** ENDS *****************************************/


/**
 * get category icons
 */
Drupal.behaviors.uportal.managingCategories.getCategoryIcons = function() {
	var $categoryForm = $('form#metadata-category-form');
	var $iconsWrapper = $('div.icon-options-wrapper ul', $categoryForm);
	$.post(Drupal.uportal.managingCategories.serverReadURL, {
		'op' : 'get-category-icons'
	}).done( function(data) {
		$.each(data.files, function(index, picName) {
			var imgHTML = '<img src="'+data.path+picName+'">';
			var liClasses = (data.usedFiles.indexOf(picName)>=0) ? 'disabled' : '';
			var $li = $('<li filename="'+picName+'" class="'+liClasses+'">'+imgHTML+'</li>');
			$li.appendTo($iconsWrapper);
			$li.data('imgHTML', imgHTML);
			$li.data('picName', picName);
			$li.click( function() {
				var $$ = $(this);
				if ($$.hasClass('disabled')) {
					return;
				}
				$('form#metadata-category-form .chosen-icon-wrapper .chosen-icon').html($$.data('imgHTML'));
				$('form#metadata-category-form input#image-name').val($$.data('picName'));
			});
		});
	});
}
/** ENDS *****************************************/


/**
 * update data after creation or edit
 */
Drupal.behaviors.uportal.managingCategories.updateExistingCategoryData = function(data) {
	Drupal.uportal.managingCategories.existingCategories['nid-'+data.nid] = data.title;
	
	//remove disabled from OLD icon
	//add disabled to NEW icon
	var $categoryForm = $('form#metadata-category-form');
	var $iconsWrapper = $('div.icon-options-wrapper ul', $categoryForm);
	var $oldLI = $('li[filename="'+data.old_icon_name+'"]', $iconsWrapper);
	var $selectedLI = $('li[filename="'+data.icon_name+'"]', $iconsWrapper);
	$oldLI.removeClass('disabled');
	$selectedLI.addClass('disabled');
	
};
/** ENDS *****************************************/


/**
 * activate save and cancel buttons
 */
Drupal.behaviors.uportal.managingCategories.activateButtons = function() {
	var $btnsWrapper = $('div#save-cancel-categories-btns-wrapper');
	var $saveBtn = $('a.save-btn', $btnsWrapper);
	var $cancelBtn = $('a.cancel', $btnsWrapper);
	
	//cancel button
	$cancelBtn.click( function() {
		if ($cancelBtn.hasClass('disabled')) {
			return false;
		}
		location.reload();
		return false;
	})
	
	//save button
	$saveBtn.click( function() {
		
		//disable button if already saving
		if ($saveBtn.hasClass('disabled')) {
			return false;
		}
		
		//update message
		Drupal.behaviors.uportal.managingCategories.showOrderCategoriesStatus('saving');
		
		//get nids data
		var nids = new Array();
		$('li', Drupal.uportal.managingCategories.$categoriesWrapper).each(
			function(index, elem) {
				var $li = $(elem);
				nids.push($li.data('nid'));
			}
		);
		
		//post data
		$saveBtn.addClass('disabled');
		$cancelBtn.addClass('disabled');
		$.post(Drupal.uportal.managingCategories.serverURL, {
			'op' : 'order-categories',
			'nids' : nids
		})
			.done( function (data) {
				$saveBtn.removeClass('disabled');
				$cancelBtn.removeClass('disabled');
				Drupal.behaviors.uportal.managingCategories.showOrderCategoriesStatus('finished');
			});
			
		return false;
	})
};
/** ENDS *****************************************/


/**
 * add menu preview button
 */
Drupal.behaviors.uportal.managingCategories.addMenuPreviewBtn = function() {
	
	if (!$('body').hasClass('page-management-metadata-categories')) {
		return;
	}
	
	var $categoryMenuBtnHolder = $('div#save-cancel-categories-btns-wrapper');
	var menuLoaderHTML = '<div class="menu-loader-wrapper"><div class="loader-icon"></div><div class="loader-text">Loading menu ...</div></div>';
	
	//add button
	var btnText = "Preview Menu Order";
	var $menuPreviewBtn = $('<a class="btn menu-preview-btn light-btn">'+btnText+'</a>');
	$menuPreviewBtn.appendTo($categoryMenuBtnHolder);
	
	//add category menu
	var $categoryMenu = $('<div class="category-menu-wrapper"><div class="category-menu-content"></div><div class="notice">Categories with zero (0) items are hidden.</div><div class="menu-arrow"></div></div>');
	$('.category-menu-content', $categoryMenu).html(menuLoaderHTML);
	$categoryMenu.appendTo($categoryMenuBtnHolder);
	
	//watch resize
	$categoryMenu.resize(function(e) {
		var topPosn = -1 * $categoryMenu.height();
		$categoryMenu.css('top', topPosn+'px');
	}).resize();
	
	//get category menu
	$menuPreviewBtn.click( function() {
		$categoryMenu.resize();
		if ($categoryMenu.is(':visible')) {
			$categoryMenu.stop().fadeOut(400);
		} else {
			$('.category-menu-content', $categoryMenu).html(menuLoaderHTML);
			$.post(Drupal.uportal.managingCategories.serverReadURL, {
				'op' : 'get-category-menu'
			}).done( function(data) {
				$('.category-menu-content', $categoryMenu).html(data.menu_html);
			})
			$categoryMenu.stop().fadeIn(400);
		}
		return false;
	});

};
/** ENDS *****************************************/


/**
 * initiate metadata page
 */
Drupal.behaviors.uportal.managingCategories.initiateCategoriesPage = function(context, settings) {
	
	//check page
	if (!(
		$('body').hasClass('management-categories')
		|| $('body').hasClass('page-management-edit-content')
		|| $('body').hasClass('page-management-content')
	)) {
		return;
	}
	
	$('body').once( function() {
		
		//initiate globals
		Drupal.uportal.managingCategories.$categoriesWrapper = $('section#main-content .view-id-management_categories ul.manage-categories-list');
		Drupal.uportal.managingCategories.$statusDiv = $('section#main-content .view-content div#categories-queue-operations');
		
		//get icon options
		Drupal.behaviors.uportal.managingCategories.getCategoryIcons();
		
		//get existing categories
		Drupal.behaviors.uportal.managingCategories.getExistingCategories();
		
		//initiate functionality
		Drupal.behaviors.uportal.managingCategories.makeCategoriesSortable();
		
		//activate save and cancel buttons
		Drupal.behaviors.uportal.managingCategories.activateButtons();
		
		//add menu preview button
    Drupal.behaviors.uportal.managingCategories.addMenuPreviewBtn();
		
		//append buttons to full page wrapper for z-index stacking problems
		$('div#save-cancel-categories-btns-wrapper').appendTo($('div.full-page-wrapper'));
		
	});
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.managingCategories.initiateCategoriesPage);
