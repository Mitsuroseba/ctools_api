<?php
/**
 * @file
 * Plugin Constructor.
 */

namespace CTools;

/**
 * Class PluginConstructor.
 *
 * @method array info()
 * @method string name()
 * @method array dependencies()
 *
 * @package CTools
 */
class PluginConstructor {

  /**
   * Plugin definition object.
   *
   * @var \CTools\Plugins\PluginInterface
   */
  private $object = '';
  /**
   * Interfaces implemented via plugin.
   *
   * @var string[]
   */
  private $implements = [];
  /**
   * CTools plugin type.
   *
   * @var string
   */
  private $type = '';
  /**
   * Plugin definition information.
   *
   * @var array
   */
  private $info = [];

  /**
   * Init new plugin.
   *
   * @param string $object
   *   Plugin definition object.
   * @param string[] $implements
   *   Interfaces implemented via plugin.
   * @param string $type
   *   CTools plugin type.
   */
  public function __construct($object, array $implements, $type) {
    $this->implements = $implements;
    $this->object = $object;
    $this->type = $type;

    $this->info = $this->info();
    $this->info['title'] = $this->name();

    $this
      ->validateDependencies()
      ->callTypeConstructor()
      ->generateThemeHooks()
      ->attachThemeAssets();
  }

  /**
   * Call plugin-specific methods.
   *
   * @param string $method
   *   The name of method.
   * @param array $arguments
   *   Method arguments.
   *
   * @return mixed|null
   *   Invocation result or NULL if method does not exist.
   */
  public function __call($method, array $arguments) {
    if (method_exists($this->object, $method)) {
      return call_user_func_array([$this->object, $method], $arguments);
    }

    return NULL;
  }

  /**
   * Get plugin definition information.
   *
   * @return array
   *   Plugin definition information.
   */
  public function getInfo() {
    return $this->info;
  }

  /**
   * Validate plugin dependencies.
   */
  private function validateDependencies() {
    $dependencies = $this->dependencies();

    // Process content type dependencies.
    if (!empty($dependencies)) {
      $disabled = array_diff_key($dependencies, array_filter(array_map('module_exists', $dependencies)));

      if (!empty($disabled)) {
        throw new \RuntimeException(t('The following dependencies of "@title" content type are disabled or does not exists: "@modules".', [
          '@title' => $this->info['title'],
          '@modules' => implode('", "', $disabled),
        ]));
      }
    }

    return $this;
  }

  /**
   * Call plugin type constructor for updating/changing definition information.
   */
  private function callTypeConstructor() {
    $constructor = "ctools_api_$this->type";

    if (!function_exists($constructor)) {
      throw new \InvalidArgumentException(t('The "@type" plugin type is unsupported!', [
        '@type' => $this->type,
      ]));
    }

    $constructor($this->info, $this->type, $this->implements);

    return $this;
  }

  /**
   * Generate theme hooks.
   */
  private function generateThemeHooks() {
    if (!isset($this->info['theme variants']) || !is_array($this->info['theme variants'])) {
      $this->info['theme variants'] = [];
    }

    $this->info['theme variants']['default'] = t('Default');

    // Create all theme suggestions.
    foreach ($this->info['theme variants'] as $variant => $title) {
      $this->info['theme hooks'][] = ctools_api_theme_hook($this->object, $variant);
    }

    return $this;
  }

  /**
   * Attach CSS/JS to plugin definition for future autoloading.
   */
  private function attachThemeAssets() {
    static $registry = [];

    if (empty($this->info['theme hooks'])) {
      return $this->info;
    }

    if (empty($registry)) {
      $registry = theme_get_registry();
    }

    // @see ctools_api_set_plugin_theme_variants()
    foreach ($this->info['theme hooks'] as $hook) {
      if (isset($registry[$hook])) {
        foreach (['css', 'js'] as $asset) {
          $this->info[$asset][] = str_replace(
            'templates', $asset, "{$registry[$hook]['path']}/{$registry[$hook]['template']}.$asset"
          );
        }
      }
    }

    return $this;
  }

}
