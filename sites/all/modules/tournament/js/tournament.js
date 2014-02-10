jQuery(document).ready(function($) {
    $('h2#control').click(function() {
        $('#block-ajax-register-ajax-register-block').stop(true,true).slideToggle(600);
        $('#block-system-user-menu').stop(true,true).slideToggle(600);
    });
    $('.tournament-button').click(function(e) {
        e.preventDefault();
        if ($(this).attr('id') == 'command-1')
            $('#tournament-get-buttons-form').submit();
        else if ($(this).attr('id') == 'command-2')
            $('#tournament-get-buttons-form--2').submit();
    });
    var targetDate = new Date($('#timetostart').text()*1000);

    if((targetDate - new Date()) >= 0 && (targetDate - new Date()) <= 86400*1000) {
        $('#countdown').countdown({
            until: targetDate,
            compact: true,
            onExpiry: function() {
                $('#tournament-countdown-form').submit()
            }
        });
    }

    $('.result-title').click(function() {
       $(this).stop(true, true).slideUp(600);
    });
    $('.live').click(function() {
        document.getElementById('stream').scrollIntoView();
    });
});