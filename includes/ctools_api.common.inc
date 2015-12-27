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
            $reflection = new \ReflectionClass($object);

            if ($reflection->isInstantiable()) {
              $file->object = $object;
              $file->sources = $sources;
              $file->abstract = $reflection->isAbstract();
              $file->implements = $reflection->getInterfaceNames();
              $file->module_name = $module_name;
              $file->module_path = $module_path;

              $plugins[] = $file;
            }
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
          'name' => "ctools_api:$plugin->object",
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
 * Extend CTools plugin definition to provide child plugins.
 *
 * @see ctools_plugin_load_includes()
 * @see ctools_plugin_process()
 *
 * @param string $type
 *   Original plugin type (e.g. "access", "styles", "content_types");
 *
 * @return array
 *   ['children'] - all children plugin definitions;
 *   ['get children'] - callback for return all children;
 *   ['get child'] - callback for return selected child.
 */
function ctools_api_type_definition_get($type) {
  return [
    // Custom key. Store all styles to not call this function twice.
    'children' => ctools_api_get_plugins($type),
    // Should return collection of children styles.
    'get children' => __FUNCTION__ . '_children',
    // Should return single definition from the children collection.
    'get child' => __FUNCTION__ . '_child',
  ];
}

/**
 * Collect all child plugins.
 *
 * @see ctools_get_plugins()
 *
 * @param array $plugin
 *   Plugin definition information.
 * @param string $parent
 *   Machine name of the origin plugin (ctools_api).
 *
 * @return array[]
 *   Style plugin definitions.
 */
function ctools_api_type_definition_get_children(array $plugin, $parent) {
  foreach ($plugin['children'] as $name => $info) {
    // Rename keys in array according to CTools documentation.
    $plugin['children']["$parent:$name"] = $info;
    unset($plugin['children'][$name]);
  }

  return $plugin['children'];
}

/**
 * Chose single plugin from children.
 *
 * @param array $plugin
 *   Plugin definition information.
 * @param string $parent
 *   Machine name of the origin plugin (ctools_api).
 * @param string $child
 *   Machine name of the child plugin.
 *
 * @return array
 *   Style plugin definition.
 */
function ctools_api_type_definition_get_child(array $plugin, $parent, $child) {
  return $plugin['children'][$child];
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
 * @param string $plugin_name
 *   Plugin namespace.
 * @param string $variant
 *   Theme variant.
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
