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
            $('#show-vods-list').click(function() {
               $('.vods-list-wrapper').toggleClass('hide-left');
            });

            $('.play-button').click(function() {
                $('#vod-player iframe').attr('src', $(this).attr('data-video'));
                $.colorbox({inline:true, open:true, href:'#vod-player', closeButton:false});
            });
            $('.info-icon').click(function() {
                $.colorbox({
                    inline:true,
                    open:true,
                    href:"#show-info",
                    onOpen: function() { console.log('open'); $('#cboxContent').addClass('info'); },
                    onClosed: function() { console.log('close'); $('#cboxContent').removeClass('info'); }
                });
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
