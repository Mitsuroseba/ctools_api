<?php
/**
 * @file
 * Contexts collection.
 */

namespace CTools;

/**
 * Class PluginContexts.
 *
 * @package CTools
 */
class PluginContexts {

  /**
   * @var PluginContext[]
   */
  private $contexts = [];

  /**
   * PluginContexts constructor.
   *
   * @param \ctools_context[] $contexts
   *   An array of contexts.
   */
  public function __construct($contexts) {
    if (!is_array($contexts)) {
      $contexts = [$contexts];
    }

    foreach (array_filter($contexts) as $context) {
      if ($context instanceof \ctools_context) {
        // Change "viewer" by "user" to provide full compatibility of types.
        // For example, when we define context for nodes - we choosing "node"
        // and get the "node", when for users - we choosing "user" and get
        // the "viewer".
        if ('viewer' === $context->keyword) {
          $context->keyword = 'user';
        }

        $this->contexts[$context->keyword ?: 'empty'] = new PluginContext($context);
      }
    }
  }

  /**
   * Ensure that context is not registered.
   *
   * @param string $name
   *   Context name.
   *
   * @return bool
   *   Checking state.
   */
  public function has($name) {
    return isset($this->contexts[$name]);
  }

  /**
   * Get specific context.
   *
   * @param string $name
   *   Context name.
   *
   * @return PluginContext
   *   Context object.
   *
   * @throws \InvalidArgumentException
   *   When context does not exists.
   */
  public function get($name) {
    if (!$this->has($name)) {
      throw new \InvalidArgumentException(t('Context with name "@name" is not registered.', [
        '@name' => $name,
      ]));
    }

    return $this->contexts[$name];
  }

  /**
   * Get list of all registered contexts.
   *
   * @return string[]
   *   Registered context names.
   */
  public function registered() {
    return array_keys($this->contexts);
  }

}
