
WebFontConfig = {
	google: { families: [ 'Armata', 'Wellfleet' ] }
};
(function() {
	var wf = document.createElement('script');
	wf.src = ('https:' == document.location.protocol ? 'https' : 'http') +
			'://ajax.googleapis.com/ajax/libs/webfont/1/webfont.js';
	wf.type = 'text/javascript';
	wf.async = 'true';
	var s = document.getElementsByTagName('script')[0];
	s.parentNode.insertBefore(wf, s);
})();

jQuery(document).ready(function() {
	jQuery('body').addClass('js-enabled');
	jQuery("form#webform-client-form-1--2 label").inFieldLabels(); 
	jQuery("form#webform-client-form-1--2 input").attr("autocomplete","off");
});

jQuery(window).load(function() {
	if (jQuery('body').hasClass('front')) {
		jQuery('body').addClass('page-loaded');
		initiate_home_page_banner();
		initiate_clouds();
	}
});

function initiate_clouds() {
	var pageWrapper = jQuery('div#page-wrapper');
	var movingBGs = jQuery('<div id="moving-bgs"><div class="bg" id="cloud-1"></div><div class="bg" id="cloud-2"></div><div class="bg" id="cloud-3"></div><div class="bg" id="cloud-4"></div><div class="bg" id="bird-1"></div><div class="bg" id="bird-2"></div></div>');
	pageWrapper.css('background-image', 'none');
	movingBGs.prependTo(pageWrapper);
	pageWrapper.prepend('');
	var bgOffsets = { //measured from topleft of centred strip array(x,y)
		'cloud-1' : new Array (1048, 126),
		'cloud-2' : new Array (991, 314),
		'cloud-3' : new Array (-209, 189),
		'cloud-4' : new Array (-61, 412),
		'bird-1'  : new Array (-167, 376),
		'bird-2'  : new Array (1043, 487),
	};
	var firstTime = true;
	jQuery(window).resize( function() {
		var windowWidth = jQuery(window).width();
		var centredWidth = jQuery('.centred-strip', pageWrapper).width();
		var originX = (windowWidth-centredWidth)/2;
		if (firstTime) {
			for (var i in bgOffsets) {
				jQuery('div#'+i, movingBGs).css({
					'left' : bgOffsets[i][0]+originX+'px',
					'top'  : bgOffsets[i][1]+'px'
				});
			}
			firstTime = false;
		}
	}).resize();
}

function initiate_home_page_banner() {
	jQuery('div#block-views-banners-block #slider .wrapper-2 .slides').nivoSlider({
		// effect: 'random', Specify sets like: 'sliceDown,sliceDownLeft,sliceUp,sliceUpLeft,sliceUpDown,sliceUpDownLeft,fold,fade,random,slideInRight,slideInLeft,boxRandom,boxRain,boxRainReverse,boxRainGrow,boxRainGrowReverse'
		effect: 'boxRandom',
		slices: 15, // For slice animations
		boxCols: 12, // For box animations
		boxRows: 6, // For box animations
		animSpeed: 500, // Slide transition speed
		pauseTime: 4000, // How long each slide will show
		startSlide: 0, // Set starting Slide (0 index)
		directionNav: true, // Next & Prev navigation
		directionNavHide: false, // Only show on hover
		controlNav: true, // 1,2,3... navigation
		controlNavThumbs: false, // Use thumbnails for Control Nav
		controlNavThumbsFromRel: false, // Use image rel for thumbs
		controlNavThumbsSearch: '.jpg', // Replace this with...
		controlNavThumbsReplace: '_thumb.jpg', // ...this in thumb Image src
		keyboardNav: true, // Use left & right arrows
		pauseOnHover: false, // Stop animation while hovering
		manualAdvance: false, // Force manual transitions
		captionOpacity: 1, // Universal caption opacity
		//prevText: 'Prev', // Prev directionNav text
		//nextText: 'Next', // Next directionNav text
		prevText: '', // Prev directionNav text
		nextText: '', // Next directionNav text
		randomStart: false, // Start on a random slide
		beforeChange: function(){}, // Triggers before a slide transition
		afterChange: function(){}, // Triggers after a slide transition
		slideshowEnd: function(){}, // Triggers after all slides have been shown
		lastSlide: function(){}, // Triggers when last slide is shown
		afterLoad: function(){} // Triggers when slider has loaded
	});
}

function initiate_slideshow_block() {
	var $bannerBlock = jQuery('#block-views-banners-block');
	var $bannerSlider = jQuery('#slider', $bannerBlock);
	var $footer = jQuery('div#footer');
	var $header = jQuery('div#header');
	var $bannerContentHolder = jQuery('div.banner-caption-0', $bannerBlock);
	var windowHeight;
	var bannerContent = {
		'height' : 483,
		'offset' : 200,
		'minimumPaddingTop' : 65,
		'maximumPaddingTop' : 100
	};
	var footer = {
		'showHeight' : 45,
		'minimumMarginTop' : bannerContent.height+(bannerContent.minimumPaddingTop*2)
	};
	var bannerPic = {
		'minHeight' : footer.minimumMarginTop+$header.height()
	};
	var noBanners = jQuery('a', $bannerSlider).length;
	var useBannerHeight;
	var footerMarginTop;
	
	//store banner content
	var bannerContentHTML = {};
	jQuery('div.banner-caption', $bannerBlock).each( function (index, elem) {
		var $$ = jQuery(this);
		bannerContentHTML[$$.attr('id')] = jQuery('div#caption-content', $$).html();
	});
	
	$bannerSlider.carouFredSel({
		width: jQuery(window).width(),
		height: jQuery(window).height(),
		align: false,
		items: {
			visible: 1,
			minimum: 1,
			width: "variable",
			height: "variable"
		},
		scroll: {
			items: 1,
			fx: "crossfade",
			duration: 1100,
			onAfter: function( oldItems, newItems, newSizes ) {
				var newID = jQuery('a', newItems[0]).attr('id');
				var newHTML = bannerContentHTML[newID];
				//var newHTML = jQuery('div#'+newID+' div#caption-content', $bannerBlock).html();
				jQuery('div#caption-content', $bannerContentHolder).html(newHTML);
			}
		},
		auto: 3000,
		//auto: false,
		prev: {
			button: "div#carousel-btns #prev-btn",
			key: "left"
		},
		next: {
			button: "div#carousel-btns #next-btn",
			key: "right"
		},
		pagination: "div#carousel-pagination"
	});
	
	jQuery(window).resize( function() {
		windowHeight = jQuery(window).height();
		useBannerHeight = ((windowHeight-footer.showHeight)>bannerPic.minHeight) ? windowHeight-footer.showHeight : bannerPic.minHeight;
		footerMarginTop = useBannerHeight-$header.outerHeight();
		$bannerBlock.height(useBannerHeight);
		$bannerSlider.height(useBannerHeight);
		jQuery('.view-content', $bannerBlock).height(useBannerHeight);
		$footer.css('margin-top' , footerMarginTop+'px');
		
		if (footerMarginTop>bannerContent.height+(bannerContent.maximumPaddingTop*2)) {
			$bannerContentHolder.css('padding-top', bannerContent.offset+bannerContent.maximumPaddingTop+'px');
		} else {
			$bannerContentHolder.css('padding-top', bannerContent.offset+((footerMarginTop-bannerContent.height)/2)+'px');
		}
		
		//adjust carousel
		var newCarouselCss = {
			width: jQuery(window).width(),
			height: useBannerHeight
		};
		$bannerSlider.css( 'width', newCarouselCss.width*noBanners );
		$bannerSlider.parent().css( newCarouselCss );
		jQuery('a', $bannerSlider).css( newCarouselCss );
			
	}).resize();
	
	
	//resize window
	//banner html dom modifications
	//run transitions
}
