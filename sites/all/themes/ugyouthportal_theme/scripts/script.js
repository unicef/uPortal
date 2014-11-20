

jQuery(document).ready(function() {
	jQuery('body').addClass('js-enabled');
	jQuery('body').removeClass('no-js');
	jQuery('video').mediaelementplayer({
		features: ['playpause','progress','current','duration','tracks','volume','fullscreen'],
		alwaysShowControls: false,
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
		var $audioPlayer = jQuery("#audio-player-jplayer");
		var $orangeWaveForm = jQuery('#audio-player .field-name-field-wave-form-orange');
		$audioPlayer.jPlayer({
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
			errorAlerts: false,
			warningAlerts: false,
			cssSelectorAncestor: '#audio-player'
		});
		$audioPlayer.bind(jQuery.jPlayer.event.timeupdate, function(event) {
			$orangeWaveForm.width(event.jPlayer.status.currentPercentAbsolute+"%");
		});
	}
	
	//document page
	if (Drupal.settings.ugyouthportal!=undefined && Drupal.settings.ugyouthportal.node_type == 'document') {
		jQuery('body.node-type-document div#block-system-main .content-player').click( function() {
			var $$ = jQuery(this);
			var documentSrc = '/sites/all/libraries/pdf.js/web/viewer.html?file='+$$.data('document-url');
			var $pdfReader = jQuery('div#pdf-reader');
			jQuery('iframe', $pdfReader).attr('src', documentSrc);
			jQuery.blockUI({
				message: $pdfReader,
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
	
	//content index page
	if (jQuery('body').hasClass('page-content-index')) {
		var $tagsSelect = jQuery('section.page-content .page-col-0 div#block-ugyouthportal-index-nav-tags select.choose-filter-tags').chosen();
		$tagsSelect.on("change", function(e, params) {
			var url = (params.selected) ? params.selected : params.deselected;
			window.location.href = window.location.protocol
				+ '//'
				+ window.location.host
				+ url;
		});
	}
	
	//isotope lists
	if (jQuery('body').hasClass('listing-page')) {
		var $contentUL = jQuery('ul.listing-page-items');
		var isotopePage = jQuery('body').hasClass('isotope-page');
		var indexPage = jQuery('body').hasClass('index-page');
		var scrollItemSelector = "section#main-content .region-content ul.listing-page-items li";
		
		if (indexPage) {
			$contentUL = jQuery('ul.index-page-items');
			scrollItemSelector = "section#main-content .region-content ul.index-page-items li";
		}
		if (isotopePage) {
			initiate_isotope('li', $contentUL, 220, 20, false, false);
		}
		
		$contentUL.append('<div id="infscr-loading"></div>');
		jQuery('section#main-content .region-content ul.pager').hide();
		$contentUL.infinitescroll(
			{
				state: {
					currPage: 0
				},
				navSelector: "section#main-content .region-content ul.pager",
				nextSelector: "section#main-content .region-content ul.pager a:first",
				itemSelector: scrollItemSelector,
				debug: false,
				bufferPx: 170,
				pixelsFromNavToBottom: 10,
				loading: {
					finishedMsg: 'No more content',
					//img: 'http://i.imgur.com/qkKy8.gif',
					msgText: "<em>Loading more content ...</em>",
				}
			},
			function(newElements) {
				console.log(newElements);
				if (isotopePage) {
					$contentUL.isotope( 'appended', jQuery( newElements ) );
				}
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



