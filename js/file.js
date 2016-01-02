(function($, module) {
  'use strict';

  /**
   * @constructor
   */
  function FileManager() {
    /**
     * File manager name, prefixed with "ctools_api".
     *
     * @type {String}
     */
    this.fileManagerID = '';
    /**
     * Stream wrapper scheme name.
     *
     * @type {String}
     */
    this.schemeName = '';
    /**
     * URL representation of stream wrapper scheme.
     *
     * @type {String}
     */
    this.schemeURL = '';
    /**
     * Path to the file manager library.
     *
     * @type {String}
     */
    this.basePath = '';
    /**
     * Uploading location (stream wrapper).
     *
     * @type {String}
     */
    this.location = '';
    /**
     * UI language.
     *
     * @type {String}
     */
    this.language = '';
  }

  /**
   * Process uploaded file.
   *
   * @param {String} uri
   *   Uploaded file URI.
   * @param {String} element
   *   CSS selector for container with file field.
   * @param {Function} [success]
   *   Callback for execution when all were done.
   */
  FileManager.prototype.process = function(uri, element, success) {
    CToolsAPI.getFIDByURI(uri, function(fid) {
      var $container = $($(element).data('target'));

      if ($container.length > 0) {
        $container.children('[type=hidden]').val(fid);
        $container.children('[type=submit]').mousedown();

        if (success instanceof Function) {
          success(fid);
        }
      }
    });
  };

  /**
   * Add an implementation of a file manager.
   *
   * @param {String} fileManager
   *   Machine name of file manager.
   * @param {Function} callback
   *   Processing callback.
   */
  FileManager.implementation = function(fileManager, callback) {
    // Generate unique name for this implementation depending on manager name.
    var implementation = module.moduleName + '_' + fileManager;
    // CSS selector of fields that need to be extended by file manager.
    var className = fileManager + '-file-manager';
    // Each file manager could have its own settings. They are passed from
    // a backend and can be distinguished by hash, added after class name.
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
