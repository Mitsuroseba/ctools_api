;(function($, module) {
  'use strict';

  Drupal.behaviors[module] = {
    attach: function(context) {
      var $context = $(context);

      // Process all [type=checkbox] and [type=radio] fields.
      $context.find('.form-type-checkbox, .form-type-radio').once(module, function() {
        var $checkbox = $(this).children('input'),
            input = $checkbox[0];

        $checkbox.after('<label class="pseudo-box ' + input.type + '" for="' + input.id + '"></label>');
      });

      // Process all [type=file] fields.
      $context.find('.form-type-managed-file').once(module, function() {
        var $container = $(this);

        $container.find('[type=file]').css({position: 'fixed', top: '-100px'}).each(function() {
          var $filed = $(this).bind('change', function() {
                $wrapper.text(this.value.replace(/.*\\(.*)$/g, '$1')).toggleClass('focus');
              }),
              $wrapper = $('<div class="pseudo-file" />').bind('click', function() {
                $filed.click();
                $(this).toggleClass('focus');
              });

          $wrapper.prependTo(this.parentNode);
        });

        $container.find('a').attr('target', '_blank');
      });

      // Handler for "horizontal_tabs" type.
      $context.find('.horizontal-tabs').once(module, function() {
        var $tabs = $('<ul class="clearfix" />'),
            $fieldsets = $(this).children('fieldset'),
            hash = location.hash.substr(1).split(':');

        $fieldsets.each(function() {
          var $fieldset = $(this),
              containerId = this.parentNode.id;

          $tabs.append($('<li class="' + this.id + '">' + $fieldset.children('legend').text() + '</li>').bind('click', function() {
            var $tab = $(this);

            $fieldsets.add($tab.siblings()).removeClass('active');
            $fieldset.add($tab).addClass('active');

            // Store active tab to location hash.
            location.replace('#horizontal-tab:' + containerId + ':' + $tab.index());
          }));
        });

        $tabs.prependTo(this);

        // Read active tabs from location hash.
        if (3 === hash.length && 'horizontal-tab' === hash[0]) {
          $('#' + hash[1]).find('li').eq(hash[2]).click();
        }
        else {
          // Open the first tab. The "addClass()" method cannot be
          // used, because on click we operate with a location hash.
          $tabs.children().first().click();
        }
      });
    }
  };
})(jQuery, 'ctools_api');
