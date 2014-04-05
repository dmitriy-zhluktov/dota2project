(function ($) {

    Drupal.behaviors.localScript = {
        attach: function (context){
            $('h2#control').click(function() {
                $('#block-ajax-register-ajax-register-block').stop(true,true).slideToggle(600);
                $('#block-system-user-menu').stop(true,true).slideToggle(600);
            });
        }
    }

})(jQuery);
