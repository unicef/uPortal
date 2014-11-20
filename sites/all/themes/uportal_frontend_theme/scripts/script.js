

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
	
	//video page
	if (Drupal.settings.ugyouthportal!=undefined && Drupal.settings.ugyouthportal.video_path!=undefined) {
		//jQuery.jPlayer.timeFormat.sepSec = " mins";
		var $videoWrapper = jQuery('div#video-player-real-wrapper');
		var $videoPlayer = jQuery("#video-player-jplayer", $videoWrapper);
		var mediaOptions = {};
		mediaOptions[Drupal.settings.ugyouthportal.video_extension] = Drupal.settings.ugyouthportal.video_path;
		mediaOptions['poster'] = Drupal.settings.ugyouthportal.video_poster;
		$videoPlayer.jPlayer({
			ready: function () {
				jQuery(this).jPlayer("setMedia", mediaOptions);
				jQuery(this).jPlayer("play");
			},
			solution: "html,flash",
			size: {
				width: "700px",
				height: "395px"
			},
			ended: function() {
				videoPlayEnded();
			},
			backgroundColor:"#000000",
			supplied: Drupal.settings.ugyouthportal.video_extension,
			swfPath: Drupal.settings.ugyouthportal.theme_path+"/scripts/jplayer/",
			//wmode: "window",
			smoothPlayBar: true,
			keyEnabled: true,
			errorAlerts: false,
			warningAlerts: false,
			cssSelectorAncestor: '#video-player'
		});
		
		//btn play
		var $videoPlayerBtns = jQuery('#video-player', $videoWrapper);
		jQuery('#btn a.jp-play', $videoPlayerBtns).click( function() {
			jQuery(this).hide();
			jQuery('#series-playlist-player', $videoWrapper).hide();
			jQuery('#btn a.jp-pause', $videoPlayerBtns).css('display', 'block');
		});
		jQuery('#btn a.jp-pause', $videoPlayerBtns).click( function() {
			jQuery(this).css('display', 'block');
			jQuery('#series-playlist-player', $videoWrapper).hide();
			jQuery('#btn a.jp-pause', $videoPlayerBtns).hide();
		});
		
		$videoPlayer.bind(jQuery.jPlayer.event.timeupdate, function(event) {
			//$orangeWaveForm.width(event.jPlayer.status.currentPercentAbsolute+"%");
		});
		
		//screenfull
		var $fullScreenBtn = jQuery('div#fullscreen', $videoWrapper);
		$fullScreenBtn.click( function() {
			if (!screenfull.enabled) {
				return false;
			}
			screenfull.toggle($videoWrapper[0]);
			return false;
		});
		
		//resize progress bar
		var $progressDiv = jQuery('#progress', $videoPlayerBtns);
		$videoPlayerBtns.resize( function() {
			var totalWidth = $videoPlayerBtns.width();
			var usedWidth = 265;
			$progressDiv.css('width', (totalWidth-usedWidth)+'px');
		});
		
		//initiate
		document.addEventListener(screenfull.raw.fullscreenchange, function () {
			if (screenfull.isFullscreen) {
				jQuery('#series-playlist', $videoWrapper).hide();
				$videoWrapper.addClass('in-fullscreen');
				$videoPlayer.jPlayer("option", {"fullScreen": true});
			} else {
				jQuery('#video-player', $videoWrapper).show();
				jQuery('#series-playlist', $videoWrapper).show();
				$videoWrapper.removeClass('in-fullscreen');
				$videoPlayer.jPlayer("option", {"fullScreen": false});
			}
			$videoPlayerBtns.resize();
		});
		
		//initiate playlist
		jQuery('#series-playlist .playlist-small li, #series-playlist-player .playlist-large li', $videoWrapper).click( function() {
			var $li = jQuery(this);
			var path = jQuery('div.file-path', $li).html();
			var extension = jQuery('div.extension', $li).html();
			var video_poster = jQuery('div.video-poster', $li).html();
			var video_backdrop = jQuery('div.video-backdrop', $li).html();
			var title = jQuery('div.title', $li).html();
			var description = jQuery('div.description', $li).html();
			var mediaOptions = {};
			mediaOptions[Drupal.settings.ugyouthportal.video_extension] = path;
			mediaOptions['poster'] = video_poster;
			$videoPlayer.jPlayer("setMedia", mediaOptions);
			jQuery('div#block-system-main .content-info .content-info-title').html(title);
			jQuery('div#block-system-main .content-info .body').html(description);
			var $imgBackdrop = jQuery('div#block-system-main div.img-backdrop');
			$imgBackdrop.css('background-image', 'url("'+video_backdrop+'")');
			
			// TODO: download path
			// TODO: recommend button
			
			//classes
			var nid = $li.data('nid');
			jQuery('#series-playlist li, #series-playlist-player li', $videoWrapper).removeClass('active');
			jQuery('#series-playlist li.nid-'+nid+', #series-playlist-player li.nid-'+nid, $videoWrapper).addClass('active');
			
			//scroll carousel to active items
			jQuery('#series-playlist .jcarousel', $videoWrapper).jcarousel('scroll', jQuery('#series-playlist .jcarousel li.active'));
			jQuery('#series-playlist-player .jcarousel', $videoWrapper).jcarousel('scroll', jQuery('#series-playlist-player .jcarousel li.active'));
			
			//play video after hiding video playlist
			var $videoPlayerPlaylist = jQuery('#series-playlist-player', $videoWrapper);
			if ($videoPlayerPlaylist.is(':hidden')) {
				$videoPlayer.jPlayer("play");
			} else {
				$videoPlayerPlaylist.stop().fadeOut(500, function() {
					$videoPlayer.jPlayer("play");
				})
			}
			
		});

		// trigger the onchange() to set the initial values
		screenfull.onchange();
		
		// initiate hide and show of video controls in full screen on mousemove
		var hideVideoControls = function() {
			if (screenfull.isFullscreen) {
				jQuery('#video-player', $videoWrapper).hide();
				//jQuery('body').css('cursor', 'none');
			}
		};
		var showVideoControls = function() {
			//jQuery('body').css('cursor', 'default');
			jQuery('#video-player', $videoWrapper).show();
		};
		var hideVideoControlsTimer = setTimeout(function() {
			hideVideoControls();
		}, 2000);
		jQuery('body').mousemove(function() {
			clearTimeout(hideVideoControlsTimer);
			hideVideoControlsTimer = setTimeout(function() {
				hideVideoControls();
			}, 2000);
			showVideoControls();
		});
		
		//initiate small carousel
		jQuery('#series-playlist, #series-playlist-player', $videoWrapper).each( function (index, elem) {
			var $carouselDiv = jQuery(this);
			jQuery('.jcarousel', $carouselDiv).jcarousel();
			jQuery('.prev-btn', $carouselDiv)
				.on('jcarouselcontrol:active', function() {
					jQuery(this).removeClass('inactive');
				})
				.on('jcarouselcontrol:inactive', function() {
					jQuery(this).addClass('inactive');
				})
				.jcarouselControl({
					target: '-=1'
				});
			jQuery('.next-btn', $carouselDiv)
				.on('jcarouselcontrol:active', function() {
					jQuery(this).removeClass('inactive');
				})
				.on('jcarouselcontrol:inactive', function() {
					jQuery(this).addClass('inactive');
				})
				.jcarouselControl({
					target: '+=1'
				});
		});
			
		//on video play ended
		var videoPlayEnded = function() {
			jQuery('#series-playlist-player .jcarousel', $videoWrapper).jcarousel('scroll', jQuery('#series-playlist-player .jcarousel li.active'));
			jQuery('#series-playlist-player', $videoWrapper).fadeIn(3600);
			/*
			if (screenfull.isFullscreen) {
			}
			*/
		}
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



