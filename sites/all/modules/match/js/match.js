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
    $('a.more-info').click(function(e) {
        e.preventDefault();
        id = $(this).attr('row');
        $('tr.row-'+id+' ul.players-list').animate({height: "100px"}, 600, function() {
            $('a.more-info-'+id).addClass('hidden');
            $('a.less-info-'+id).removeClass('hidden');
            $('.vod-'+id).removeClass('hidden');
        });
    });

    $('a.less-info').click(function(e) {
        e.preventDefault();
        id = $(this).attr('row');
        $('.vod-'+id).addClass('hidden');
        $('tr.row-'+id+' ul.players-list').animate({height: "20px"}, 600, function() {
            $('a.less-info-'+id).addClass('hidden');
            $('a.more-info-'+id).removeClass('hidden');
        });
    });

    if($('#no-active').length > 0) {
        $('#quicktabs-tab-front_mathces-0').trigger('click');
    }
    /* Work with reviews */
    $.fn.stars = function() {
        return $(this).each(function() {
            // Get the value
            var val = parseFloat($(this).html());
            // Make sure that the value is in 0 - 5 range, multiply to get width
            var size = Math.max(0, (Math.min(5, val))) * 16;
            // Create stars holder
            var $span = $('<span />').width(size);
            // Replace the numerical value with stars
            $(this).html($span);
        });
    }
    $('span.stars').stars();
});