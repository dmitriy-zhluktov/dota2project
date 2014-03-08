(function ($) {
    Drupal.behaviors.fluxSlider = {
        attach: function (context, settings) {
            // Initialise the slider.
            var myFlux = new flux.slider('#slider', {
                autoplay: true,
                delay: 4000,
                transitions: 'dissolve',
                pagination: false,
                controls: false,
                captions: false,
                width: 650,
                height: 251,
                onTransitionEnd: function(data) {
                    implement_pagination(data);
                }
            });
            // Adjust container height to cope with bug http://drupal.org/node/1880556.
            // Additional height required if pagination controls are shown.

            function implement_pagination(data) {
                $('ul#pagination li').each(function() {
                   $(this).removeClass('active');
                   $(this).find('.pagination-arrow').addClass('hidden');
                });

                img = data.currentImage.outerHTML;
                nav_id = $(img).attr('nav-id');
                $('#nav-index-'+nav_id).addClass('active');
                $('#nav-index-'+nav_id).find('.pagination-arrow').removeClass('hidden');
            }
            $('#pagination .item').mouseover(function() {
                myFlux.stop();
                myFlux.showImage($(this).attr('show-image'));
                $('ul#pagination li').each(function() {
                    $(this).removeClass('active');
                    $(this).find('.pagination-arrow').addClass('hidden');
                });
                $(this).addClass('active');
                $(this).find('.pagination-arrow').removeClass('hidden');
            });
            $('#pagination .item').mouseleave(function() {
                myFlux.start();
            });
        }
    }
}(jQuery));