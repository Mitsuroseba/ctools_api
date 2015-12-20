;(function($, browser) {
  'use strict';

  // Context element, inside of the method, will be the "Browse files" link.
  var module = new CToolsAPI(browser).addImplementation(function() {
    var settings = module.settings,
        context = this;

    window[settings.browserId] = function(imce_window) {
      var imce = imce_window.imce;

      // In case, when IMCE call this method, the "filename" parameter will
      // exist and "lastFid" method will return the "undefined" value, because
      // selected data will be cleared at this moment. Otherwise, when user
      // clicks on the "Insert file" button, the "filename" parameter will
      // have the "true" value and "lastFid" method returns necessary value.
      imce.send = function(filename) {
        filename = imce.lastFid() || filename;

        // The "filename" can be "true" here only if a user clicks
        // on the "Insert file" button without any selected file.
        if (true === filename) {
          imce.setMessage(Drupal.t('You have not selected any file.'), 'error');
        }
        else {
          module.getFidByUri(settings.scheme + '://' + imce.fileGet(filename).relpath, context, function() {
            // The "close" is a native method of "Window" object. It must be
            // called directly and must not be passed to function as parameter.
            imce_window.close();
          });
        }
      };

      return imce.opAdd({
        name: 'sendto',
        title: Drupal.t('Insert file'),
        func: imce.send
      });
    };

    window.open('/' + browser + '/' + settings.scheme + '?app=nomatter|imceload@' + settings.browserId, '', 'width=760,height=560,resizable=1');
  });
})(jQuery, 'imce');
