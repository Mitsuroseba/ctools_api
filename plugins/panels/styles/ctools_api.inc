<?php
/**
 * @file
 * Collect all Panels plugins of "Styles" type.
 */

/**
 * Implements <MODULE>_<FILENAME>_panels_styles().
 *
 * Generate an array with information about
 * all plugins of the "ContentTypes" type.
 *
 * @see ctools_plugin_load_includes()
 * @see ctools_plugin_process()
 */
function ctools_api_ctools_api_panels_styles(array $info) {
  return [
    'title' => t('CTools API Style'),
    'category' => t('CTools API'),
    // Custom key. Store all styles to not call this function twice.
    'styles' => ctools_api_get_plugins($info['type']),
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
function ctools_api_ctools_api_panels_styles_children(array $plugin, $parent) {
  foreach ($plugin['styles'] as $name => $info) {
    // Rename keys in array according to CTools documentation.
    $plugin['styles']["$parent:$name"] = $info;
    unset($plugin['styles'][$name]);
  }

  return $plugin['styles'];
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
function ctools_api_ctools_api_panels_styles_child(array $plugin, $parent, $child) {
  return $plugin['styles'][$child];
}
