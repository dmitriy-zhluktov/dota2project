(function ($) {

    Drupal.behaviors.localScript = {
        attach: function (context){
            $('h2#control').click(function() {
                $('#block-ajax-register-ajax-register-block').stop(true,true).slideToggle(600);
                $('#block-system-user-menu').stop(true,true).slideToggle(600);
            });
            $('#change-pic').click(function() {
               $('input[name="files[profile]"]').trigger('click');
            });
            $('input[name="files[profile]"]').change(function() {
               $('#helper-picture-form').find('.form-submit').trigger('click');
            });
        }
    }

    Drupal.behaviors.uniteller = {
        attach: function (context, settings){
            $('#edit-qty').keyup(function(e) {
                if($(this).val() == '')
                    $('#qty-recounter').val('');
                else
                    $('#qty-recounter').val($(this).val()*settings.uniteller.currency);
            });
        }
    }
})(jQuery);
