(function ($) {
  Drupal.behaviors.addLinkBehavior =  {
    attach: function(context, settings) {
          $("a.steam-browser-link").click(function() {
               var url = decodeURI(this.href);
               var splittedUrlArray = url.split("steam");
               var neededUrl = "steam" + splittedUrlArray[1];
               window.open(neededUrl);
               return false; 
          }    
          ); 
    }
  };

}(jQuery));