

(function($) {


/**
 * block ui with operating message
 */
Drupal.behaviors.uportal.blockUIWaitMessage = function (title, msg) {
	var $waitingDialog = $('div#running-operation');
	$('.dialog-header', $waitingDialog).html(title);
	$('.dialog-content', $waitingDialog).html(msg);
	$.blockUI({
		message: $waitingDialog,
		blockMsgClass: 'dialog-box-block-ui-wrapper'
	});
};
/** ENDS *****************************************/


/**
 * check if string is in array
 */
Drupal.behaviors.uportal.checkStringExistsInArray = function (str, arr) {
	var exists = false;
	$.each(arr, function(index, arrStr) {
		var str2 = (typeof arrStr.name !== "undefined") ? arrStr.name : arrStr;
		if (Drupal.behaviors.uportal.stringsAreTheSame(str, str2)) {
			exists = true;
			return false;
		}
		return true;
	});
	return exists;
};
/** ENDS *****************************************/


/**
 * compare strings to test if they are the same
 */
Drupal.behaviors.uportal.stringsAreTheSame = function (str1, str2) {
	str1 = str1.toLowerCase().replace(/\s+/g, ''); //remove all white space and to lowercase
	str2 = str2.toLowerCase().replace(/\s+/g, '');
	if (str1==str2) {
		return true;
	}
	return false;
}
/** ENDS *****************************************/


/**
 * get current Drupal URL
 */
Drupal.behaviors.uportal.getCurrentURL = function() {
	return window.location.protocol + "://" + window.location.host + window.location.pathname + window.location.search + window.location.hash;
};
/** ENDS *****************************************/


/**
 * check user permissions
 */
Drupal.behaviors.uportal.userIsAllowed = function(perm) {
	//check initiated permissions
	if (
		typeof Drupal.uportal.userPerms.disallowed === 'undefined'
		|| typeof Drupal.uportal.userPerms.allowed === 'undefined'
	) {
		return false;
	}
	
	//check allowed for permission
	if (
		($.inArray(perm, Drupal.uportal.userPerms.allowed) > -1)
		&& ($.inArray(perm, Drupal.uportal.userPerms.disallowed) <= -1)
	) {
		return true;
	}
	
	return false;
};
/** ENDS *****************************************/


/**
* behaviors
*/
Drupal.behaviors.uportal.attach = function (context, settings) {
	
	//prepare document
	Drupal.uportal.$eventWatcher = $('div#bulk-editor-dialog-boxes div#event-observer');
	$(context).once('uportal').addClass('js-enabled');
	$(context).once('uportal').removeClass('no-js');
	
	//functions run once
	$('body').once( function() {
		var $body = $('body');
		
		//force reload of page if accessed via BACK button
		var $refreshPageInput = $('input#refresh-page');
		if ($refreshPageInput.val()=='no') {
			$refreshPageInput.val('yes');
		} else {
			$refreshPageInput.val('no');
			location.reload();
		}
		
		/** get user perms from body tag ***********/
		Drupal.uportal.userPerms['allowed'] = $body.data('allowed-perms').split(/[ ]+/);
		Drupal.uportal.userPerms['disallowed'] = $body.data('disallowed-perms').split(/[ ]+/);
		
		/** jquery validate defaults ****************/
		$.validator.setDefaults({
			ignore: ":hidden:not(select)"
		});
		
		/** blockUI defaults ***********************/
		$.blockUI.defaults = { 
			// message displayed when blocking (use null for no message) 
			message:  '<h1>Please wait...</h1>', 
	 
			title: null,        // title string; only used when theme == true 
			draggable: true,    // only used when theme == true (requires jquery-ui.js to be loaded) 
	 
			theme: false, // set to true to use with jQuery UI themes 
	 
			// styles for the message when blocking; if you wish to disable 
			// these and use an external stylesheet then do this in your code: 
			// $.blockUI.defaults.css = {}; 
			css: { 
					padding:        0, 
					margin:         0, 
					width:          '30%', 
					top:            '40%', 
					left:           '35%', 
					textAlign:      'center', 
					color:          '#000', 
					border:         '3px solid #aaa', 
					backgroundColor:'#fff', 
					cursor:         'wait' 
			}, 
	 
			// minimal style set used when themes are used 
			themedCSS: { 
					width:  '30%', 
					top:    '40%', 
					left:   '35%' 
			}, 
	 
			// styles for the overlay 
			overlayCSS:  { 
					backgroundColor: '#000', 
					opacity:         0.6, 
					cursor:          'wait' 
			}, 
	 
			// style to replace wait cursor before unblocking to correct issue 
			// of lingering wait cursor 
			cursorReset: 'default', 
	 
			// styles applied when using $.growlUI 
			growlCSS: { 
					width:    '350px', 
					top:      '10px', 
					left:     '', 
					right:    '10px', 
					border:   'none', 
					padding:  '5px', 
					opacity:   0.6, 
					cursor:    null, 
					color:    '#fff', 
					backgroundColor: '#000', 
					'-webkit-border-radius': '10px', 
					'-moz-border-radius':    '10px' 
			}, 
			 
			// IE issues: 'about:blank' fails on HTTPS and javascript:false is s-l-o-w 
			// (hat tip to Jorge H. N. de Vasconcelos) 
			iframeSrc: /^https/i.test(window.location.href || '') ? 'javascript:false' : 'about:blank', 
	 
			// force usage of iframe in non-IE browsers (handy for blocking applets) 
			forceIframe: false, 
	 
			// z-index for the blocking overlay 
			baseZ: 1000, 
	 
			// set these to true to have the message automatically centered 
			centerX: true, // <-- only effects element blocking (page block controlled via css above) 
			centerY: true, 
	 
			// allow body element to be stetched in ie6; this makes blocking look better 
			// on "short" pages.  disable if you wish to prevent changes to the body height 
			allowBodyStretch: true, 
	 
			// enable if you want key and mouse events to be disabled for content that is blocked 
			bindEvents: true, 
	 
			// be default blockUI will supress tab navigation from leaving blocking content 
			// (if bindEvents is true) 
			constrainTabKey: true, 
	 
			// fadeIn time in millis; set to 0 to disable fadeIn on block 
			fadeIn:  200, 
	 
			// fadeOut time in millis; set to 0 to disable fadeOut on unblock 
			fadeOut:  400, 
	 
			// time in millis to wait before auto-unblocking; set to 0 to disable auto-unblock 
			timeout: 0, 
	 
			// disable if you don't want to show the overlay 
			showOverlay: true, 
	 
			// if true, focus will be placed in the first available input field when 
			// page blocking 
			focusInput: true, 
	 
			// suppresses the use of overlay styles on FF/Linux (due to performance issues with opacity) 
			// no longer needed in 2012 
			// applyPlatformOpacityRules: true, 
	 
			// callback method invoked when fadeIn has completed and blocking message is visible 
			onBlock: function() {
				$("html.uportal-page, body.uportal-page").css('overflow', 'hidden');
				$('.blockUI input.metadata-title').focus();
			}, 
	 
			// callback method invoked when unblocking has completed; the callback is 
			// passed the element that has been unblocked (which is the window object for page 
			// blocks) and the options that were passed to the unblock call: 
			//   onUnblock(element, options) 
			onUnblock: function() {
				$("html.uportal-page, body.uportal-page").css('overflow', 'auto');
			}, 
	 
			// don't ask; if you really must know: http://groups.google.com/group/jquery-en/browse_thread/thread/36640a8730503595/2f6a79a77a78e493#2f6a79a77a78e493 
			quirksmodeOffsetHack: 4, 
	 
			// class name of the message block 
			blockMsgClass: 'blockMsg', 
	 
			// if it is already blocked, then ignore it (don't unblock and reblock) 
			ignoreIfBlocked: false 
		}; 
		/** blockUI defaults end *******************/
		
	});
	
	//on window load
	$(window).load(function() {
		$('body').once('uportal').removeClass('page-loading');
		$('body').once('uportal').addClass('page-loaded');
		
		//call collected functions once loaded
		$.each(Drupal.uportal.windowLoadedFunctions, function(index, windowloadedFunction) {
			windowloadedFunction();
		});
		
	});
	
	//run collected attached behaviour
	$.each(Drupal.uportal.attachedBehaviors, function(index, uportalBehavior) {
		uportalBehavior(context, settings);
	});
	
	//unblock ui buttons
	$('.unblock-ui').click( function () {
		$.unblockUI();
		return false;
	})
	
};

}) (jQuery);


/**
 * remove duplicates from array
 */
Array.prototype.removeDuplicates = function() {
	var a = this.concat();
	for (var i=0; i<a.length; ++i) {
		for (var j=i+1; j<a.length; ++j) {
			if (a[i] === a[j]) {
				a.splice(j--, 1);
			}
		}
	}
	return a;
};
/** ENDS *****************************************/


/**
 * trim functions like in PHP
 * - http://www.sitepoint.com/trimming-strings-in-javascript/
 */
String.prototype.trimLeft = function(charlist) {
  if (charlist === undefined) 
    charlist = "\s";
 
  return this.replace(new RegExp("^[" + charlist + "]+"), "");
};
String.prototype.trimRight = function(charlist) {
  if (charlist === undefined)
    charlist = "\s";
 
  return this.replace(new RegExp("[" + charlist + "]+$"), "");
};
String.prototype.trim = function(charlist) {
  return this.trimLeft(charlist).trimRight(charlist);
};
