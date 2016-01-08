<?php
/**
 * @file
 * CTools API.
 */

/**
 * Get directory with plugin sources.
 *
 * @see ctools_api_search_plugins()
 *
 * @return string
 *   Relative path to module where defined.
 */
function hook_ctools_api_plugins_directory() {
  return 'plugins';
}
