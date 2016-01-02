(function($, module) {
  'use strict';

  function FileManager() {
    this.fileManagerID = '';
    this.schemeName = '';
    this.schemeURL = '';
    this.basePath = '';
    this.location = '';
    this.language = '';
  }

  FileManager.prototype.process = function(uri, context, success) {
    CToolsAPI.getFIDByURI(uri, function(fid) {
      var $container = $($(context).data('target'));

      if ($container.length > 0) {
        $container.children('[type=hidden]').val(fid);
        $container.children('[type=submit]').mousedown();

        if (success instanceof Function) {
          success(fid);
        }
      }
    });
  };

  FileManager.implementation = function(fileManager, callback) {
    var implementation = module.moduleName + '_' + fileManager;
    var className = fileManager + '-file-manager';
    var regexp = new RegExp('.*' + className + ' (\\w+).*', 'g');

    Drupal.behaviors[implementation] = {
      attach: function(context, settings) {
        $(context).find('.' + className).once().each(function() {
          var manager = new FileManager();

          manager = $.extend(manager, settings[this.className.replace(regexp, '$1')]);
          manager.fileManagerID = implementation;

          $(this).before($('<span class="pseudo-link" data-target="#' + this.id + '">' + Drupal.t('Browse files') + '</span>').bind('click', function() {
            callback(manager, this);
          }));
        });
      }
    };
  };

  CToolsAPI.FileManager = FileManager;
})(jQuery, CToolsAPI);
