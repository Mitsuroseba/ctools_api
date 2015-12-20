;(function($, browser) {
  'use strict';

  var ckfinder = new CKFinder('public://test'),
      // Context element, inside of the method, will be the "Browse files" link.
      module = new CToolsAPI(browser).addImplementation(function() {
        var settings = module.settings,
            context = this;

        $.each(['basePath', 'language'], function() {
          ckfinder[this] = settings[this];
        });

        ckfinder.selectActionFunction = function(url) {
          module.getFidByUri(settings.scheme + ':' + url.replace(new RegExp(settings.schemeUrl), ''), context);
        };

        ckfinder.popup();
      });
})(jQuery, 'ckfinder');
