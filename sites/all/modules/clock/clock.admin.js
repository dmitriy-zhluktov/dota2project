(function ($) {

Drupal.behaviors.clockAdminForm = {
  attach: function (context, settings) {
    var timeZoneType = $('.form-item-time-zone-type');
    var select = timeZoneType.find('select');
    var description = timeZoneType.find('div.description');
    var childDescriptions = timeZoneType.find('.child-descriptions');
    var userDescription = childDescriptions.find('.child-description-user');
    var siteDescription = childDescriptions.find('.child-description-site');
    var localDescription = childDescriptions.find('.child-description-local');
    var customDescription = childDescriptions.find('.child-description-custom');

    description.addClass('element-hidden');
    childDescriptions.removeClass('element-hidden');

    select.bind('keypress mousemove', updateDescription = function () {
      userDescription.addClass('element-hidden');
      siteDescription.addClass('element-hidden');
      localDescription.addClass('element-hidden');
      customDescription.addClass('element-hidden');

      var value = select.val();
      switch(value) {
        case 'user':
          userDescription.removeClass('element-hidden');
          break;
        case 'site':
          siteDescription.removeClass('element-hidden');
          break;
        case 'local':
          localDescription.removeClass('element-hidden');
          break;
        case 'custom':
          customDescription.removeClass('element-hidden');
          break;
      }
    });

    updateDescription();

    $('.form-item-custom-time-zone .description').addClass('element-hidden');
  }
};

})(jQuery);
