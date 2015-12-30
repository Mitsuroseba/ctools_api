<?php
/**
 * @file
 * Menu callbacks.
 */

/**
 * Menu callback.
 *
 * @see ctools_api_menu()
 */
function _ctools_api_get_fid_by_uri() {
  $params = drupal_get_query_parameters();

  if (isset($params['uri'])) {
    $files = file_load_multiple([], $params);

    if (empty($files)) {
      $file = file_save_data(file_get_contents($params['uri']), $params['uri'], FILE_EXISTS_REPLACE);

      if (is_object($file)) {
        file_usage_add($file, 'ctools_api', 'user', $GLOBALS['user']->uid);
        exit($file->fid);
      }
    }
    else {
      exit(reset($files)->fid);
    }
  }

  exit(0);
}
