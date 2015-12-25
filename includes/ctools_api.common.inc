<?php
/**
 * @file
 * Auxiliary functionality.
 */

/**
 * Scan modules for CTools API plugins.
 *
 * @return \stdClass[]
 *   Plugins that were found.
 */
function ctools_api_search_plugins() {
  $cache = cache_get(__FUNCTION__);
  $plugins = [];

  if (FALSE === $cache || empty($cache->data)) {
    $hook = 'ctools_api_plugins_directory';

    foreach (module_implements($hook) as $module_name) {
      $function = $module_name . '_' . $hook;

      if (function_exists($function)) {
        $module_path = drupal_get_path('module', $module_name);
        $sources = $module_path . '/' . $function();

        ctools_api_register_autoloader_path($sources);

        foreach (file_scan_directory($sources, '/\.inc$/') as $file) {
          $object = str_replace('/', '\\', trim(str_replace([$sources, '.inc'], '', $file->uri), '/'));

          if (class_exists($object)) {
            $file->object = $object;
            $file->sources = $sources;
            $file->implements = class_implements($object);
            $file->module_name = $module_name;
            $file->module_path = $module_path;

            $plugins[] = $file;
          }
        }
      }
    }

    cache_set(__FUNCTION__, $plugins);
  }
  else {
    $plugins = $cache->data;
  }

  return $plugins;
}

/**
 * Get CTools API plugins.
 *
 * @param string $plugin_type
 *   CTools plugin type.
 *
 * @return array[]
 *   Plugin definitions.
 */
function ctools_api_get_plugins($plugin_type) {
  // Transform "content_types" to "ContentTypes", for example.
  $namespace = ucfirst(ctools_api_to_camel_case($plugin_type));
  $plugins = [];

  foreach (ctools_api_search_plugins() as $plugin) {
    ctools_api_register_autoloader_path($plugin->sources);

    if (isset($plugin->implements[implode('\\', ['CTools', 'Plugins', $namespace, $namespace . 'Interface'])])) {
      try {
        $plugins[$plugin->object] = array_merge((new \CTools\PluginConstructor($plugin->object, $plugin->implements, $plugin_type))->getInfo(), [
          'name' => $plugin->name,
          'path' => dirname($plugin->uri),
          'file' => $plugin->filename,
          'object' => $plugin->object,
          'location' => $plugin->uri,
          'plugin type' => $plugin_type,
          'plugin module' => $plugin->module_name,
          'plugin module path' => $plugin->module_path,
        ]);
      }
      catch (\Exception $e) {
        watchdog_exception(CTOOLS_API_MODULE_TITLE, $e);
      }
    }
  }

  return $plugins;
}

/**
 * Register objects loader.
 *
 * @param string $path
 *   Path to directory with objects.
 *
 * @return string[]
 *   Paths to plugin sources.
 */
function ctools_api_register_autoloader_path($path = '') {
  static $paths = [];

  if (!empty($path) && !isset($paths[$path])) {
    $paths[$path] = $path;
  }

  return $paths;
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
