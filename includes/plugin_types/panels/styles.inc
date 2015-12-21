<?php
/**
 * @file
 * API for Panels styles plugins.
 */

/**
 * Collect all plugins of "styles" type.
 *
 * @param array $information
 *   Basic plugin information.
 * @param string $plugin_type
 *   Native plugin type.
 * @param string[] $interfaces
 *   Implemented interfaces.
 * @param \CTools\Plugins\Styles\StylesInterface|string $object
 *   Plugin object (static).
 *
 * @return array
 *   Updated plugin definition.
 */
function ctools_api_styles(array $information, $plugin_type, array $interfaces, $object) {
  foreach (['pane', 'region'] as $renderer) {
    if (isset($interfaces['CTools\Plugins\Styles\Styles' . ucfirst($renderer) . 'Interface'])) {
      $prefix = 'pane' === $renderer ? "$renderer " : '';

      $information['render ' . $renderer] = "_ctools_api_{$plugin_type}_base_render_$renderer";
      $information[$prefix . 'settings form'] = "_ctools_api_{$plugin_type}_base_settings_form";

      ctools_api_set_plugin_theme_variants($information, $object);
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

  return ctools_api_apply_theme($plugin['object'], $settings['theme'], $variables);
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
  $parts = explode('\\', $variables['pane']->subtype);

  $content->title_overriden = !empty($content->title_heading);
  // Collect the CSS settings of panel into array that can be rendered by
  // "drupal_attributes()" function.
  $content->attributes = [
    // Add transformed vendor and object names to pane classes.
    'class' => array_map('ctools_api_to_dashes', [reset($parts), end($parts)]),
  ];

  foreach (['class', 'id'] as $prop) {
    $object_prop = "css_$prop";

    if (!empty($content->$object_prop)) {
      $content->attributes[$prop] = explode(' ', $content->$object_prop);
      unset($content->$object_prop);
    }
  }

  $plugin['object']::preprocess($content, $settings, $variables);

  return ctools_api_apply_theme($plugin['object'], $settings['theme'], $variables);
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
    '#options' => $plugin['theme variants'],
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
