
/**
 * needed global variables
 */
Drupal.uportal = {
	userPerms : {}, //current user perms
	$eventWatcher : {}, //used to watch for events
	windowLoadedFunctions : [], //call these functions when window loads
	attachedBehaviors : [] //call these functions in Drupal.attach.behaviors
}

//global uportal behaviors
Drupal.behaviors.uportal = {
	'contentListing' : {},
	'bulkUploader' : {},
	'bulkEditor' : {},
	'editContent' : {},
	'managingUsers' : {},
	'userForms' : {},
	'managingMetaData' : {},
	'managingMetaDataTags' : {},
	'managingCategories' : {},
	'reorderSeries' : {},
	'reorderLessonPlans' : {}
};