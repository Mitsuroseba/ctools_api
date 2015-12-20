;(function($, module) {
  'use strict';

  var CToolsAPI = function CToolsAPI(browser) {
    this.settings = {
      browser: browser,
      browserId: module + '_' + browser
    };
  };

  CToolsAPI.prototype = {
    constructor: CToolsAPI,
    getFidByUri: function(uri, context, success) {
      $.get('/' + module + '/get_fid_by_uri', {uri: uri}, function(fid) {
        if (fid > 0) {
          var $container = $('#' + $(context).data('target'));

          if ($container.length > 0) {
            $container.children('[type=hidden]').val(fid);
            $container.children('[type=submit]').mousedown();

            if (success instanceof Function) {
              success(fid, uri);
            }
          }
        }
      });
    },
    addImplementation: function(func) {
      var self = this,
          browserId = self.settings.browserId;

      Drupal.behaviors[browserId] = {
        attach: function(context, settings) {
          self.settings = $.extend(self.settings, settings[browserId]);

          $(context).find('.' + self.settings.browser + '-files-browser [type=file]').once().each(function() {
            $(this).parent().before(
              $('<span class="pseudo-link" data-target="' + this.parentNode.id + '">' + Drupal.t('Browse files') + '</span>')
                .bind('click', func)
            );
          });
        }
      };

      return self;
    }
  };

  // @todo Rename this object.
  window.CToolsAPI = CToolsAPI;
})(jQuery, 'ctools_api');
