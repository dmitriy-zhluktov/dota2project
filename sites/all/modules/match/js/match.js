jQuery(document).ready(function($) {
	$('.active-players').click(function() {
		$(this).find('.item-list ul.players-list').stop(true, true).toggle(800);
	});
	
	$('.finished-players').click(function() {
		$(this).find('.item-list ul.players-list').stop(true, true).toggle(800);
	});
	
	$('.winner').click(function() {
		$(this).find('div').stop(true, true).toggle(800);
	});
	
	$('.active-ticket').click(function() {
		//$('#block-match-0').after('купить билет на матч №  '+$(this).attr('match-id'));
	});
	
});