
(function($) {


/**
 * globals we need for content listing
 */
Drupal.uportal.userForms = {
	$form : {},
	serverDeleteURL : '/management/users/delete'
};
/** ENDS *****************************************/


/**
 * initiate all delete buttons
 */
Drupal.behaviors.uportal.userForms.initiateDeleteButton = function() {
	$('a.delete-btn', Drupal.uportal.userForms.$form).click( function() {
		var $btn = $(this);
		var uid = $btn.data('uid');
		var userName = $btn.data('username');
		
		//delete content
		Drupal.behaviors.uportal.userForms.deleteUser(userName, uid);
		
	});
}
/** ENDS *****************************************/


/**
 * delete user
 */
Drupal.behaviors.uportal.userForms.deleteUser = function(userName, uid) {
	var $confirmationDiv = $('div#confirm-deletion');
	$('.dialog-header', $confirmationDiv).text('Delete User?');
	$('.dialog-content', $confirmationDiv).html('Are you sure you want to delete this user: <em>'+userName+'</em>?');
	$('.dialog-footer .yes').click( function() {
		Drupal.behaviors.uportal.blockUIWaitMessage('Deleting User ...', 'Please wait as we delete this user: <em>'+userName+'</em> ...');
		$.post(Drupal.uportal.userForms.serverDeleteURL, {
			'data_values' : {
				'uid' : uid
			}
		})
			.done( function(data) {
				window.location.href = '/management/users';
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
 * initiate metadata page
 */
Drupal.behaviors.uportal.userForms.initiateUserFormPage = function(context, settings) {
	
	//check page
	if (!($('body').hasClass('management-user-forms'))) {
		return;
	}
	
	$('body').once( function() {
		
		//initiate globals
		Drupal.uportal.userForms.$form = $('form#user-register-form, form#user-profile-form');
		
		//initiate functionality
		Drupal.behaviors.uportal.userForms.initiateDeleteButton();
		
		//modify DOM structure
		var $passwordFormDiv = $('.form-type-password-confirm', Drupal.uportal.userForms.$form);
		var $parentPasswordDescriptionDiv = $('.description:last', $passwordFormDiv);
		var $passwordSuggestionsDiv = $('.password-suggestions', $passwordFormDiv);
		var $passwordConfirmStatusDiv = $('div.password-confirm', $passwordFormDiv);
		var $passwordStrengthDiv = $('.password-strength', $passwordFormDiv);
		$passwordSuggestionsDiv.removeClass('description');
		$passwordSuggestionsDiv.appendTo($parentPasswordDescriptionDiv);
		$passwordConfirmStatusDiv.prependTo($parentPasswordDescriptionDiv);
		//$passwordStrengthDiv.prependTo($parentPasswordDescriptionDiv);
		
		//field descriptions
		$('.form-item-name, .form-item-mail, .form-type-password-confirm', Drupal.uportal.userForms.$form).each( function(index, elem) {
			var $formItem = $(elem);
			var $desc = $('.description:last', $formItem);
			$desc.hide();
			$('.password-strength', $formItem).hide();
			$('input.form-text', $formItem).focus( function() {
				$desc.stop().fadeIn(500);
				$('.password-strength', $formItem).stop().fadeIn(500);
			});
			$('input.form-text', $formItem).blur( function() {
				$desc.stop().fadeOut(500);
				$('.password-strength', $formItem).stop().fadeOut(500);
			});
		});
		
	});
	
	
}
/** ENDS *****************************************/


}) (jQuery);


/**
* functions to call on page ready or behavior
*/
Drupal.uportal.attachedBehaviors.push(Drupal.behaviors.uportal.userForms.initiateUserFormPage);
