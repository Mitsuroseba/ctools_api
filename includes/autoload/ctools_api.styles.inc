<?php
/**
 * @file
 * API for Panels styles.
 */

/**
 * Collect all plugins of "styles" type.
 *
 * @param array $information
 *   Basic plugin information.
 * @param \CTools\API\Styles\StylesInterface|string $object
 *   Plugin object (static).
 * @param string $type
 *   Native plugin type.
 *
 * @return array
 *   Updated plugin definition.
 */
function ctools_api_styles(array $information, $object, $type) {
  $implementations = class_implements($object);

  foreach (['pane', 'region'] as $renderer) {
    if (isset($implementations['CTools\API\Styles\Styles' . ucfirst($renderer) . 'Interface'])) {
      $prefix = 'pane' === $renderer ? "$renderer " : '';

      $information['render ' . $renderer] = "_ctools_api_{$type}_base_render_$renderer";
      $information[$prefix . 'settings form'] = "_ctools_api_{$type}_base_settings_form";

      _ctools_api_set_plugin_theme($information, $type, $information['machine_name']);
      break;
    }
  }

  // Prevent creation of additional theme suggestions.
  // @see panels_theme()
  unset($information['hook theme']);

  return $information;
}

/**
 * Implements theme_HOOK().
 *
 * @see ctools_api_styles()
 */
function theme__ctools_api_styles_base_render_region(array $variables) {
  $settings =& $variables['settings'];
  $plugin = $variables['style'];

  $plugin['object']::preprocess($variables['display'], $settings, $variables);

  return _ctools_api_apply_plugin_theme('styles', $plugin['name'], $settings['theme'], $variables);
}

/**
 * Implements theme_HOOK().
 *
 * @see ctools_api_styles()
 */
function theme__ctools_api_styles_base_render_pane(array $variables) {
  $settings =& $variables['settings'];
  $content =& $variables['content'];
  $plugin = $variables['style'];

  $content->title_overriden = !empty($content->title_heading);
  // Collect the CSS settings of panel into array that can be rendered by
  // "drupal_attributes()" function.
  $content->attributes = [];

  foreach (['class', 'id'] as $prop) {
    $object_prop = "css_$prop";

    if (!empty($content->$object_prop)) {
      $content->attributes[$prop] = explode(' ', $content->$object_prop);
      unset($content->$object_prop);
    }
  }

  $content->attributes['class'][] = _ctools_api_dashes($variables['pane']->subtype);
  $plugin['object']::preprocess($content, $settings, $variables);

  return _ctools_api_apply_plugin_theme('styles', $plugin['name'], $settings['theme'], $variables);
}

/**
 * Main settings form for a style plugin.
 *
 * @param array $settings
 *   Saved, by user, settings.
 * @param \panels_display $display
 *   Panels display object.
 * @param int $pid
 *   Panel ID.
 * @param string $type
 *   The type of a panel.
 * @param array $form_state
 *   Drupal form state.
 *
 * @return array
 *   Drupal form definition.
 */
function _ctools_api_styles_base_settings_form($settings, $display, $pid, $type, $form_state) {
  $plugin = $form_state['style'];
  $form = [];

  $form['theme'] = [
    '#type' => 'select',
    '#title' => t('Choose theme'),
    '#options' => $plugin['theme hooks'],
    '#default_value' => 'default',
  ];

  // @codingStandardsIgnoreStart
  require_once $plugin['location'];
  // @codingStandardsIgnoreEnd

  $plugin['object']::configurationForm($form, $form_state, $settings, $display, $pid);

  // @todo Set values recursive.
  // Process default values and field types.
  foreach ($form as $field_name => $definition) {
    if (isset($settings[$field_name])) {
      $form[$field_name]['#default_value'] = $settings[$field_name];
    }

    if (!isset($definition['#type'])) {
      $form[$field_name]['#type'] = 'textfield';
    }
  }

  return $form;
}
