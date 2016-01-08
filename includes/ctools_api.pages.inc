<?php
/**
 * @file
 * Menu callbacks.
 */

/**
 * Menu callback.
 *
 * @internal
 *
 * @see ctools_api_menu()
 */
function _ctools_api_get_fid_by_uri() {
  global $user;

  $params = drupal_get_query_parameters();

  if (isset($params['uri'])) {
    $files = file_load_multiple([], $params);

    if (empty($files)) {
      $file = file_save_data(file_get_contents($params['uri']), $params['uri'], FILE_EXISTS_REPLACE);

      if (is_object($file)) {
        file_usage_add($file, 'ctools_api', 'user', $user->uid);
        exit($file->fid);
      }
    }
    else {
      $file = reset($files);
      $usage = file_usage_list($file);

      if (empty($usage)) {
        file_usage_add($file, 'ctools_api', 'user', $user->uid);
      }

      exit($file->fid);
    }
  }

  exit(0);
}
