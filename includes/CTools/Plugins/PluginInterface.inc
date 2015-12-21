<?php
/**
 * @file
 * CTools Plugin Interface.
 */

namespace CTools\Plugins;

/**
 * Interface PluginInterface.
 *
 * @package CTools\Plugins
 */
interface PluginInterface {

  /**
   * Get plugin name.
   *
   * @return string
   *   Human-readable name.
   */
  public static function name();

  /**
   * Additional plugin information. Could be empty.
   *
   * @return string[]
   *   For "content_types":
   *   - 'category' => <string>,
   *   - 'top level' => <bool>,
   *   - 'theme variants' => <string[]>,
   *   - 'required context' => <\ctools_context>
   *   For "styles":
   *   - 'theme variants' => <string[]>,
   *   - 'renderers' => <string[]>,
   */
  public static function info();

  /**
   * Plugin dependencies (Drupal modules).
   *
   * @return string[]
   *   Module machine names.
   */
  public static function dependencies();

}
