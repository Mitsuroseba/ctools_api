<?php
/**
 * @file
 * Collect all CTools plugins of "ContentTypes" type.
 */

/**
 * Implements <MODULE>_<FILENAME>_ctools_content_types().
 *
 * Generate an array with information about
 * all plugins of the "ContentTypes" type.
 *
 * @see ctools_plugin_load_includes()
 * @see ctools_plugin_process()
 */
function ctools_api_ctools_api_ctools_content_types(array $info) {
  return [
    'title' => t('CTools API Content'),
    'category' => t('CTools API'),
    'admin info' => '_ctools_api_content_types_base_admin_info',
    'content types' => ctools_api_get_plugins($info['type']),
  ];
}
