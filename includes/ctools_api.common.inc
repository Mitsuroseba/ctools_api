<?php
/**
 * @file
 * Auxiliary functionality.
 */

/**
 * Get namespace of a module component.
 *
 * @param string|string[] $compare
 *   Namespace string or parts.
 *
 * @return string[]
 *   Basic namespace when no comparisons, empty - if
 *   comparison failed, namespace object - in other cases.
 */
function ctools_api_namespace($compare = NULL) {
  $base = ['CTools', 'Plugins'];

  if (isset($compare)) {
    if (is_string($compare)) {
      $compare = explode('\\', $compare);
    }

    $base = array_slice($compare, 0, count($base)) === $base ? $compare : [];
  }

  return $base;
}

/**
 * Convert string to "camelCase" format.
 *
 * @param string $argument
 *   Input string.
 *
 * @return string
 *   Transformed string.
 */
function ctools_api_to_camel_case($argument) {
  return lcfirst(implode(array_map('ucfirst', explode('_', $argument))));
}

/**
 * Convert string to "underscore" format.
 *
 * @param string $argument
 *   Input string.
 *
 * @return string
 *   Transformed string.
 */
function ctools_api_to_underscore($argument) {
  return strtolower(preg_replace('/(?<=\w)(?=[A-Z])/', '_$1', $argument));
}

/**
 * Replace all "_" and ":" by "-".
 *
 * @example
 * TestTest:test_test => test-test-test-test
 *
 * @param string $argument
 *   Input string.
 *
 * @return string
 *   Transformed string.
 */
function ctools_api_to_dashes($argument) {
  return str_replace(['_', ':'], '-', ctools_api_to_underscore($argument));
}

/**
 * Generate theme suggestions for CTools content type.
 *
 * @param array $data
 *   Updated array of CTools content type definition.
 * @param string $plugin_name
 *   Plugin machine name.
 */
function ctools_api_set_plugin_theme_variants(array &$data, $plugin_name) {
  if (!isset($data['theme variants']) || !is_array($data['theme variants'])) {
    $data['theme variants'] = [];
  }

  $data['theme variants']['default'] = t('Default');

  // Create all theme suggestions.
  foreach ($data['theme variants'] as $variant => $title) {
    $data['theme hooks'][] = [$variant, ctools_api_theme_hook($plugin_name, $variant)];
  }
}

/**
 * Build theme hook.
 *
 * @return string
 *   Theme hook name.
 */
function ctools_api_theme_hook($plugin_name, $variant) {
  $arguments = array_merge(['ctools'], explode('\\', $plugin_name));
  $arguments[] = $variant;

  return implode('__', array_map('ctools_api_to_underscore', $arguments));
}

/**
 * Apply theme hook.
 *
 * @param string $plugin_name
 *   Plugin machine name.
 * @param string $variant
 *   Theme variant.
 * @param array $variables
 *   An array of variables that will be passed to template.
 *
 * @throws \Exception
 *   When theme is not initialized.
 *
 * @return string
 *   HTML content.
 */
function ctools_api_apply_theme($plugin_name, $variant, array $variables) {
  return theme(ctools_api_theme_hook($plugin_name, $variant), $variables);
}
