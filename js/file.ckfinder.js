(function($, fileManager) {
  'use strict';

  CToolsAPI.FileManager.implementation(fileManager, function(manager, triggeringElement) {
    var ckfinder = new CKFinder(manager.location);

    $.each(['basePath', 'language'], function() {
      ckfinder[this] = manager[this];
    });

    ckfinder.selectActionFunction = function(url) {
      manager.process(manager.schemeName + ':' + url.replace(new RegExp(manager.schemeURL), ''), triggeringElement);
    };

    ckfinder.popup();
  });
})(jQuery, 'ckfinder');
