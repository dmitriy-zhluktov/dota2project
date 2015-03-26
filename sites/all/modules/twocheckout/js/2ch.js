(function ($) {
    Drupal.behaviors.tch = {
        attach: function (context, settings) {
            $("#edit-ccno").mask('0000 0000 0000 0000');

            var successCallback = function (data) {
                var myForm = document.getElementById('twocheckout-order-form');
                myForm.token.value = data.response.token.token;
                myForm.submit();
            };
            var ajaxFailedCounter = 0;
            var errorCallback = function (data) {
                if (data.errorCode === 200) {
                    // This error code indicates that the ajax call failed. We recommend that you retry the token request.
                    if (++ajaxFailedCounter < 10) {
                        tokenRequest();
                    } else {
                        alert('Ajax failed. Refresh page or tey again later');
                    }
                } else {
                    alert(data.errorMsg);
                }
            };
            var tokenRequest = function () {
                var args = {
                    sellerId: settings.tch.sellerId,
                    publishableKey: settings.tch.publishableKey,
                    ccNo: $("#edit-ccno").val(),
                    cvv: $("#edit-cvv").val(),
                    expMonth: $("#edit-expmonth").val(),
                    expYear: $("#edit-expyear").val()
                };
                TCO.requestToken(successCallback, errorCallback, args);
            };

            $(function () {
                TCO.loadPubKey(settings.tch.mode);

                $("#twocheckout-order-form").submit(function (e) {
                    tokenRequest();
                    return false;
                });
            });
        }
    }
})(jQuery);