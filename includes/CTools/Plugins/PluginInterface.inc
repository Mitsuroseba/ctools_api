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
   *   - 'required context' => <\ctools_context|\ctools_context[]>
   */
  public static function info();

  /**
   * Theme variations. "default" variant provided by default.
   *
   * @see CTools\PluginConstructor::attachThemeAssets()
   *
   * @code
   * namespace VendorName\PluginsGroupName;
   *
   * class ContentType implements ContentTypesInterface {
   *   public static function themeVariants() {
   *     return [
   *       'preview' => t('Preview'),
   *     ];
   *   }
   *
   *   // Other necessary methods.
   * }
   * @endcode
   *
   * The next theme hooks will be available for code in the above example:
   * - ctools--vendor-name--plugins-group-name--content-type--default.tpl.php
   * - ctools--vendor-name--plugins-group-name--content-type--preview.tpl.php
   *
   * In simple words: theme hooks generated using the value of plugin namespace.
   * Formula: ctools--<NAMESPACE>--<VARIANT>.[tpl.php|css|js]
   *
   * Also you should know that plugins are able to automatically load CSS & JS.
   * Assets also could have variations. The location of files should repeat the
   * location of template and must have the same name, except extension, of
   * course.
   *
   * Imagine that you've placed template in the "templates/ctools" directory.
   * In this case, all CSS files should be placed in the "css/ctools" and
   * JS - in the "js/ctools". If you want to move the template to another
   * directory, "templates/ctools/content-types" for example, then DO NOT
   * forget to relocate assets too.
   *
   * @return string[]
   *   Short names of the suffixes for theme hooks.
   */
  public static function themeVariants();

  /**
   * Plugin dependencies (Drupal modules).
   *
   * @return string[]
   *   Module machine names.
   */
  public static function dependencies();

}
