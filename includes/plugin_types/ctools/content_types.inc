<?php
/**
 * @file
 * API for CTools content types plugins.
 */

/**
 * Collect all plugins of "content_types" type.
 *
 * @param array $information
 *   Basic plugin information.
 * @param string $plugin_type
 *   Native plugin type.
 * @param string[] $interfaces
 *   Implemented interfaces.
 * @param \CTools\Plugins\ContentTypes\ContentTypesInterface|string $object
 *   Plugin object (static).
 *
 * @return array
 *   Updated plugin definition.
 */
function ctools_api_content_types(array $information, $plugin_type, array $interfaces, $object) {
  $information['render callback'] = "_ctools_api_{$plugin_type}_base_render";
  $information['edit form'] = "_ctools_api_{$plugin_type}_base_settings_form";

  if (!isset($information['category'])) {
    $information['category'] = CTOOLS_API_MODULE_TITLE;
  }

  ctools_api_set_plugin_theme_variants($information, $object);

  return $information;
}

/**
 * Load subtype of "ctools_api" content type.
 *
 * @param string $subtype
 *   Content type (subtype) name.
 * @param array $info
 *   Definition of "ctools_api" content type.
 *
 * @return array
 *   Subtype definition.
 */
function _ctools_api_content_type_get_subtype($subtype, array $info = []) {
  $plugin = empty($info) ? ctools_get_plugins('ctools', 'content_types', 'ctools_api') : $info;
  $definition = [];

  if (isset($plugin['content types'][$subtype])) {
    $definition = $plugin['content types'][$subtype];
  }

  return $definition;
}

/**
 * Base render for admin preview.
 *
 * @internal
 *
 * @param string $content_type
 *   Machine name of the pane.
 * @param array $conf
 *   Pane settings.
 * @param array $context
 *   Pane context.
 *
 * @return \stdClass
 *   Pane block.
 */
function _ctools_api_content_types_base_admin_info($content_type, array $conf, array $context) {
  return call_user_func_array('_ctools_api_content_types_base_render', func_get_args());
}

/**
 * Base renderer for CTools content type plugin.
 *
 * @internal
 *
 * @param string $content_type
 *   Pane machine name.
 * @param array $conf
 *   Saved data.
 * @param array $args
 *   Pane arguments.
 * @param \ctools_context $context
 *   Pane context.
 *
 * @return \stdClass
 *   Drupal block.
 */
function _ctools_api_content_types_base_render($content_type, array $conf, array $args, \ctools_context $context = NULL) {
  static $processed = [];

  $plugin = _ctools_api_content_type_get_subtype($content_type);
  $plugin['object']::preprocess($conf, $args, $context);

  $block = new \stdClass();
  $block->title = $conf['override_title_text'];
  $block->delta = $content_type;
  $block->region = $block->module = 'ctools_api';

  if (isset($conf['theme'])) {
    if (!isset($processed[$content_type])) {
      foreach ([
        'css' => [],
        'js' => ['scope' => 'footer'],
      ] as $type => $options) {
        if (isset($plugin[$type]) && is_array($plugin[$type])) {
          foreach (array_map('file_exists', $plugin[$type]) as $file) {
            call_user_func("drupal_add_$type", $file, $options);
          }
        }
      }

      // Do this only once.
      $processed[$content_type] = TRUE;
    }

    $block->content = ctools_api_apply_theme($content_type, $conf['theme'], [
      'content' => $conf,
    ]);
  }
  else {
    $block->content = t('You must create at least one theme hook and template for output pane content.');
  }

  return $block;
}

/**
 * Base form for CTools content type plugin.
 *
 * @internal
 *
 * @param array $form
 *   Form implementation.
 * @param array $form_state
 *   Form state.
 *
 * @return array
 *   Updated form.
 */
function _ctools_api_content_types_base_settings_form(array $form, array &$form_state) {
  $plugin = _ctools_api_content_type_get_subtype($form_state['subtype_name'], $form_state['plugin']);
  $conf = $form_state['conf'];
  $items = [];

  // Update "conf" with temporary values after AJAX call.
  if (!empty($form_state['ajax']) && !empty($form_state['input'])) {
    // @todo
    $conf = array_merge($conf, $form_state['input']);
  }

  if (isset($form_state['triggering_element'])) {
    $trigger = $form_state['triggering_element'];

    if (isset($trigger['#operation'])) {
      $item =& drupal_array_get_nested_value($conf, array_slice($trigger['#parents'], 0, -2));

      switch ($trigger['#operation']) {
        // @code
        // ['content', 'references', 'references', '_last', '_add']
        // @endcode
        case 'add':
          // Add new empty item to collection.
          $item[] = [];
          break;

        // @code
        // ['content', 'references', 'references', 2, 'operations', '_remove']
        // @endcode
        case 'remove':
          // Remove all values from the collection item.
          $item = NULL;
          // Get all items in collection.
          $item =& drupal_array_get_nested_value($conf, array_slice($trigger['#parents'], 0, -3));
          // Remove empty values.
          $item = array_filter($item);
          break;
      }
    }
  }

  $plugin['object']::configurationForm($items, $form_state, $conf);

  $theme = [
    'theme' => [
      '#type' => 'select',
      '#title' => t('Theme variant'),
      '#options' => $plugin['theme variants'],
      '#default_value' => 'default',
    ],
  ];

  if (!empty($plugin['theme variants'])) {
    $items = array_merge($theme, $items);
  }

  if (isset($form['override_title'])) {
    // Enable title overriding by default and hide the checkbox.
    $form['override_title']['#default_value'] = TRUE;
    $form['override_title']['#access'] = FALSE;

    // Make-up field for pane title.
    $form['override_title_text'] = array_merge($form['override_title_text'], [
      '#title' => t('Pane title'),
      '#attributes' => [
        'placeholder' => t('Title'),
      ],
    ]);
  }

  $form += _ctools_api_content_types_base_settings_process_form($items, $form_state, $form_state['conf'], $plugin);
  // Store all parent elements.
  $form_state['#elements'] = array_keys($items);

  foreach (['css', 'js'] as $extension) {
    // Attach "form.css" and "form.js" provided by
    // CTools API for customizing configuration form.
    $form['#attached'][$extension][] = CTOOLS_API_MODULE_PATH . "/$extension/form.$extension";

    // Attach CSS and/or JS files that located near the "*.inc" file
    // with a content type definition (if exists).
    $plugin_asset = rtrim($plugin['location'], 'inc') . $extension;

    if (file_exists($plugin_asset)) {
      $form['#attached'][$extension][] = $plugin_asset;
    }
  }

  // Add form callbacks.
  // @see _ctools_api_content_types_base_settings_form()
  foreach (['submit'] as $type) {
    $form["#$type"][] = __FUNCTION__ . '_' . $type;
  }

  return $form;
}

/**
 * Process form field types.
 *
 * @param array $items
 *   Form items.
 * @param array $form_state
 *   Form state.
 * @param array $conf
 *   Saved data.
 * @param array $plugin
 *   Plugin definition information.
 *
 * @return array
 *   Processed items.
 */
function _ctools_api_content_types_base_settings_process_form(array $items, array &$form_state, array $conf, array $plugin) {
  static $once = [];

  foreach ($items as $name => &$element) {
    $type = isset($element['#type']) ? $element['#type'] : '';
    // Attach the file with plugin definition to the form to be
    // sure that everything will be okay on rebuilding state.
    $form_state['build_info']['files']['ctools_api'] = $plugin['location'];

    // Allow "#ajax => TRUE".
    if (isset($element['#ajax'])) {
      // MUST to be added to render default fields, provided by CTools.
      if (empty($once['include_content_added'])) {
        ctools_form_include($form_state, 'content');
        $once['include_content_added'] = TRUE;
      }

      $element['#ajax'] = [
        'wrapper' => $form_state['build_info']['form_id'],
        // @see _ctools_api_content_types_base_settings_form_ajax()
        'callback' => $form_state['build_info']['form_id'] . '_ajax',
      ];
    }

    switch ($type) {
      case 'horizontal_tabs':
        $element = array_merge($element, [
          '#tree' => TRUE,
          '#prefix' => '<div class="form-item horizontal-tabs" id="form-item-' . $name . '">',
          '#suffix' => '</div>',
        ]);

        unset($element['#type']);
        break;

      case 'horizontal_tab':
        $element = array_merge($element, [
          '#type' => 'fieldset',
        ]);
        break;

      case 'link_field':
        $title = '';
        $description = '';
        $titles = [
          'url' => t('URL'),
          'title' => t('Title'),
        ];

        if (!empty($element['#title'])) {
          $title = '<label>' . $element['#title'] . '</label>';
        }

        if (!empty($element['#description'])) {
          $description = '<div class="description">' . $element['#description'] . '</div>';
        }

        $element = array_merge($element, [
          '#tree' => TRUE,
          '#prefix' => '<div class="form-item form-item-link" id="form-item-' . $name . '">' . $title . '<div class="form-link-item-wrapper">',
          '#suffix' => '</div>' . $description . '</div>',
          'title' => [
            '#type' => 'textfield',
            '#title' => $titles['title'],
            '#attributes' => [
              'placeholder' => $titles['title'],
            ],
          ],
          'url' => [
            '#type' => 'textfield',
            '#title' => $titles['url'],
            '#attributes' => [
              'placeholder' => $titles['url'],
            ],
          ],
        ]);

        unset($element['#type']);
        break;

      case 'managed_file':
        if (!isset($element['#upload_location'])) {
          $element['#upload_location'] = file_default_scheme() . '://';
        }

        // Use one of file browsers.
        if (isset($element['#widget'])) {
          foreach ([
            'imce' => [
              '#module' => 'imce',
              // Function to determine the path of JS library.
              '#function' => ['drupal_get_path', ['module', 'imce']],
              '#subfolder' => 'js',
            ],
            'ckfinder' => [
              '#module' => 'ckeditor',
              '#function' => ['ckfinder_path', []],
              '#subfolder' => '',
            ],
          ] as $widget => $data) {
            if ($widget !== $element['#widget']) {
              continue;
            }

            if (empty($once["$type:$widget"])) {
              if (empty($once[$widget]) && module_exists($data['#module'])) {
                list($func, $arguments) = $data['#function'];
                $once[$widget] = trim(call_user_func_array($func, $arguments), '/');
              }

              // In this case the module will not exist.
              if (empty($once[$widget])) {
                continue;
              }
              else {
                $once[$widget] .= '/' . $data['#subfolder'];
              }

              drupal_add_js(sprintf('%s/js/file.js', CTOOLS_API_MODULE_PATH));

              $element['#attached']['js'] = [
                sprintf("%s/$widget.js", $once[$widget]),
                sprintf("%s/js/$widget.js", CTOOLS_API_MODULE_PATH),
              ];

              $stream_wrapper_scheme = file_uri_scheme($element['#upload_location']);
              /* @var \DrupalLocalStreamWrapper $stream_wrapper */
              $stream_wrapper = file_stream_wrapper_get_instance_by_scheme($stream_wrapper_scheme);

              drupal_add_js([
                "ctools_api_$widget" => [
                  'scheme' => $stream_wrapper_scheme,
                  'basePath' => DRUPAL_ROOT . '/' . $once[$widget],
                  'language' => $GLOBALS['language']->language,
                  'schemeUrl' => $stream_wrapper->getDirectoryPath(),
                ],
              ], 'setting');

              $once["$type:$widget"] = TRUE;
            }

            $element['#attributes']['class'][] = "$widget-files-browser";
          }
        }

        if (empty($once['image_module_enabled'])) {
          $once['image_module_enabled'] = module_exists('image');
        }

        // Allow to affect on image preview.
        $element += [
          '#preview' => TRUE,
        ];

        if ($once['image_module_enabled'] && $element['#preview']) {
          // @see _ctools_api_field_managed_file_pre_render().
          $element['#pre_render'][] = "_ctools_api_field_{$type}_pre_render";
        }
        break;
    }

    $conf_child = $conf;

    if (isset($conf[$name])) {
      $conf_child = $conf[$name];

      if (is_array($conf[$name]) && isset($conf[$name]['value'])) {
        $conf[$name] = $conf[$name]['value'];
      }

      if ('!default' !== $conf[$name]) {
        $element['#default_value'] = $conf[$name];
      }
    }

    // Make our processing recursive.
    foreach (element_children($element) as $child) {
      // Re-run processing only for elements.
      if (is_array($element[$child])) {
        $data = call_user_func_array(__FUNCTION__, [[$child => $element[$child]], &$form_state, $conf_child, $plugin]);
        $element[$child] = reset($data);
      }
    }
  }

  return $items;
}

/**
 * Handle AJAX calls.
 *
 * @param array $form
 *   Form items.
 * @param array $form_state
 *   Form state.
 *
 * @return array
 *   Form after rebuild.
 */
function _ctools_api_content_types_base_settings_form_ajax(array $form, array &$form_state) {
  $element = $form_state['triggering_element'];
  $plugin = _ctools_api_content_type_get_subtype($form_state['subtype_name'], $form_state['plugin']);
  $method = ctools_api_to_camel_case('ajax_callback_' . end($element['#parents']));
  $commands = [];

  if (method_exists($plugin['object'], $method)) {
    $plugin['object']::$method($form, $form_state, $commands, $form_state['values']);
  }

  array_unshift($commands, ajax_command_replace('#' . $form['#form_id'], drupal_render($form)));

  return [
    '#type' => 'ajax',
    '#commands' => $commands,
  ];
}

/**
 * Save the CTools plugin options.
 *
 * @internal
 *
 * @param array $form
 *   Drupal form definition.
 * @param array $form_state
 *   Drupal form state.
 */
function _ctools_api_content_types_base_settings_form_submit(array $form, array &$form_state) {
  if (!empty($form_state['#elements'])) {
    foreach ($form_state['#elements'] as $field_name) {
      if (isset($form_state['values'][$field_name])) {
        $form_state['conf'][$field_name] = $form_state['values'][$field_name];
      }
    }

    _ctools_api_content_types_base_process_specific_elements_submit($form, $form_state);

    $plugin = _ctools_api_content_type_get_subtype($form_state['subtype_name'], $form_state['plugin']);
    $plugin['object']::configurationFormSubmit($form, $form_state);
  }
}

/**
 * Recursive processing of the specific form elements (e.g. "managed_file").
 *
 * @internal
 *
 * @param array $form
 *   Drupal form definition.
 * @param array $form_state
 *   Drupal form state.
 */
function _ctools_api_content_types_base_process_specific_elements_submit(array $form, array &$form_state) {
  static $once = [];

  // Process the files.
  if (isset($form['#type'], $form['#name']) && 'managed_file' === $form['#type'] && empty($once[$form['#name']])) {
    if ($form['#value']['fid'] > 0) {
      $file = file_load($form['#value']['fid']);
      $file->status = FILE_STATUS_PERMANENT;

      file_usage_add(file_save($file), 'ctools_api', 'user', $GLOBALS['user']->uid);
    }

    $once[$form['#name']] = TRUE;
  }

  // Process collections.
  // @see \CTools\Plugins\ContentTypes\Form\Elements\Collection
  if (isset($form['#subtype'], $form['#name']) && 'collection' === $form['#subtype'] && empty($once[$form['#name']])) {
    $items =& drupal_array_get_nested_value($form_state['values'], $form['#parents']);

    // Remove buttons.
    foreach ($items as &$item) {
      unset($item['_remove'], $item['_add']);
    }

    // Remove empty items.
    $items = array_filter($items);

    $once[$form['#name']] = TRUE;
  }

  // Recursively call this function for sure that all field
  // will be saved correctly.
  foreach (element_children($form) as $child) {
    call_user_func_array(__FUNCTION__, [$form[$child], &$form_state]);
  }
}

/**
 * Pre render callback for "managed_file" field type.
 *
 * @see theme_image_style()
 * @see theme_image()
 *
 * @param array $element
 *   Form element.
 *
 * @throws \Exception
 *   When theme is not initialize.
 *
 * @return array
 *   Processed form element.
 */
function _ctools_api_field_managed_file_pre_render(array $element) {
  if (empty($element['#file'])) {
    hide($element['remove_button']);
  }
  else {
    $file = $element['#file'];

    hide($element['upload']);
    hide($element['upload_button']);

    if (!file_validate_is_image($file)) {
      $info = image_get_info($file->uri);
      $variables = [
        'alt' => $file->filename,
        'path' => $file->uri,
        'style_name' => 'thumbnail',
        'attributes' => [
          'class' => ['upload-preview'],
        ],
      ];

      if (is_array($info)) {
        $variables += $info;
      }

      $element['preview'] = [
        '#type' => 'markup',
        '#weight' => -10,
        '#markup' => theme('image_style', $variables),
      ];
    }
  }

  return $element;
}
