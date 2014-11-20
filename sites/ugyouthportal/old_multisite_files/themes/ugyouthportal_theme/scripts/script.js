

jQuery(document).ready(function() {
	jQuery('body').addClass('js-enabled');
	jQuery('body').removeClass('no-js');
	jQuery('audio,video').mediaelementplayer({
		success: function(player, node) {
			jQuery('#' + node.id + '-mode').html('mode: ' + player.pluginType);
		}
	});
	//jQuery('div#content-meta-info').easytabs();
	//alert(Drupal.settings.ugyouthportal.nid);
	//alert('hello');
	
	//unblock ui buttons
	jQuery('.unblock-ui').click( function() {
		jQuery.unblockUI();
		return false;
	});
	
	//recommend button
	jQuery('div.node a#recommend-btn').click( function() {
		jQuery.blockUI({ message: jQuery('div#recommend-processing') });
	});
	
	//audio page
	if (Drupal.settings.ugyouthportal!=undefined && Drupal.settings.ugyouthportal.audio_path!=undefined) {
		jQuery.jPlayer.timeFormat.sepSec = " mins";
		jQuery("#audio-player-jplayer").jPlayer({
			ready: function () {
				jQuery(this).jPlayer("setMedia", {
					mp3: Drupal.settings.ugyouthportal.audio_path
				});
			},
			solution: "html,flash",
			supplied: "mp3",
			swfPath: Drupal.settings.ugyouthportal.theme_path+"/scripts/jplayer/",
			//wmode: "window",
			smoothPlayBar: false,
			keyEnabled: true,
			errorAlerts: true,
			warningAlerts: true,
			cssSelectorAncestor: '#audio-player'
		});
	}
	
	//document page
	if (Drupal.settings.ugyouthportal!=undefined && Drupal.settings.ugyouthportal.node_type == 'document') {
		jQuery('body.node-type-document div#block-system-main .content-player').click( function() {
			jQuery.blockUI({
				message: jQuery('div#pdf-reader'),
				css: {
					width: '95%',
					height: '90%',
					left: '2.5%',
					top: '5%'
				}
			});
			return false;
		});
	}
	
	//mega menu
	var $menuClick = jQuery('header.page-header div#mega-menu-title');
	var $menuPopper = jQuery('header.page-header div#mega-menu-popper');
	$menuClick.click( function() {
		$menuPopper.toggle(150);
	});
	
	//isotope lists
	if (jQuery('body').hasClass('views-listing_pages-page_1') || jQuery('body').hasClass('views-listing_pages-page_2')) {
		var $isotopeUL = jQuery('ul.listing-page-items');
		$isotopeUL.append('<div id="infscr-loading"></div>');
		initiate_isotope('li', $isotopeUL, 220, 20, false, false);
		$isotopeUL.infinitescroll(
			{
				navSelector: "div#block-system-main div.view-listing-pages ul.pager",
				nextSelector: "div#block-system-main div.view-listing-pages ul.pager a:first",
				itemSelector: "div#block-system-main div.view-listing-pages div.view-content li",
				loading: {
					finishedMsg: 'No more content to load',
					//img: 'http://i.imgur.com/qkKy8.gif',
					msgText: "<em>Loading the next set of content ...</em>",
				}
			},
			function(newElements) {
				$isotopeUL.isotope( 'appended', jQuery( newElements ) );
			}
		);
	}
});


jQuery.fn.finished_recommending_content = function() {
	jQuery.blockUI({ message: jQuery('div#recommend-finished') });
};


jQuery(window).load(function() {
	jQuery('body').removeClass('page-loading');
	jQuery('body').addClass('page-loaded');
	if (jQuery('body').hasClass('front')) {
		//initiate_home_page_banner();
	}
});


/**
 * initiate isotope
 */
function initiate_isotope(useItemSelector, $itemsContainer, columnWidth, gutterWidth, $itemsHeading, $isotopeNav) {
	modify_isotope();
	
	jQuery('li', $itemsContainer).css('padding-right', 0);
	
	var masonryObj = {};
	if (!gutterWidth) {
		masonryObj = {columnWidth: columnWidth};
	} else {
		masonryObj = {
			columnWidth: columnWidth,
			gutterWidth: gutterWidth
		};
	}
	$itemsContainer.isotope({
		itemSelector: useItemSelector,
		layoutMode: 'masonry',
		masonry: masonryObj,
		animationEngine: 'best-available',
		animationOptions: {
			duration: 450,
			easing: 'linear',
			queue: false
		}
	});
	
	//append nav container
	if ($isotopeNav!==false) {
		jQuery('a', $isotopeNav).click( function() {
			var $a = jQuery(this);
			var selector = $a.attr('data-filter');
			$itemsContainer.isotope({ filter: selector });
			$itemsHeading.text($a.text());
			jQuery('li', $isotopeNav).removeClass('active');
			$a.parent('li').addClass('active');
			if ($a.attr('href')=='#') {
				return false;
			}
			return true;
		});
	}
}


/**
 * modify isotope
 */
function modify_isotope() {
	// modified Isotope methods for gutters in masonry
	jQuery.Isotope.prototype._getMasonryGutterColumns = function() {
		var gutter = this.options.masonry && this.options.masonry.gutterWidth || 0;
				containerWidth = this.element.width();
	
		this.masonry.columnWidth = this.options.masonry && this.options.masonry.columnWidth ||
									// or use the size of the first item
									this.$filteredAtoms.outerWidth(true) ||
									// if there's no items, use size of container
									containerWidth;
	
		this.masonry.columnWidth += gutter;
	
		this.masonry.cols = Math.floor( ( containerWidth + gutter ) / this.masonry.columnWidth );
		this.masonry.cols = Math.max( this.masonry.cols, 1 );
	};
	
	jQuery.Isotope.prototype._masonryReset = function() {
		// layout-specific props
		this.masonry = {};
		// FIXME shouldn't have to call this again
		this._getMasonryGutterColumns();
		var i = this.masonry.cols;
		this.masonry.colYs = [];
		while (i--) {
			this.masonry.colYs.push( 0 );
		}
	};
	
	jQuery.Isotope.prototype._masonryResizeChanged = function() {
		var prevSegments = this.masonry.cols;
		// update cols/rows
		this._getMasonryGutterColumns();
		// return if updated cols/rows is not equal to previous
		return ( this.masonry.cols !== prevSegments );
	};
}



