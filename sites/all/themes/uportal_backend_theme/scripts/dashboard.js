(function($) {

/**
 * create dasboard graphs
 *
 * - get all numbers to create baseline using the largest number
 * - give div widths using percentage of largest number
 */
Drupal.behaviors.uportal.initiateDashboardGraphs = function() {
	//check if dashboard page
	if (!($('body').hasClass('page-management-dashboard'))) {
		return;
	}
	
	var $categoriesDiv = $('div#dashboard-page-content');
	var largestNumber = 0;
	$('div.type-numbers div.numbers', $categoriesDiv).each( function(index, elem) {
		var numberStr = $(elem).text();
		var number = parseInt(numberStr.replace(',',''));
		if (number>largestNumber) {
			largestNumber = number;
		}
	});
	$('div.type-numbers', $categoriesDiv).each( function(index, elem) {
		var $wrapper = $(elem);
		var $graphDiv = $('div.graph', $wrapper);
		var numberStr = $('div.numbers', $wrapper).text();
		var number = parseInt(numberStr.replace(',',''));
		var widthInPercentage = (number/largestNumber)*100;
		$graphDiv.css('width', widthInPercentage+'%');
	});
	
}

}) (jQuery);

/**
* functions to call on page load
*/
Drupal.uportal.windowLoadedFunctions.push(Drupal.behaviors.uportal.initiateDashboardGraphs);
