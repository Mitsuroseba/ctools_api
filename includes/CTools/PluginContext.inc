<?php
/**
 * @file
 * Context wrapper.
 */

namespace CTools;

/**
 * Class PluginContext.
 *
 * @package CTools
 */
class PluginContext {

  /**
   * @var \stdClass
   */
  private $info;
  /**
   * @var mixed
   */
  private $data;

  /**
   * PluginContext constructor.
   *
   * @param \ctools_context $context
   *   Plugin context.
   */
  public function __construct(\ctools_context $context) {
    $this->info = (object) (array) $context;
    $this->data = $this->info->data;
    unset($this->info->data);
  }

  /**
   * Get context information.
   *
   * @return \stdClass
   *   Context information.
   */
  public function info() {
    return $this->info;
  }

  /**
   * Get context data.
   *
   * @return mixed
   *   Context data.
   */
  public function data() {
    return $this->data;
  }

  /**
   * Ensure that context data is empty.
   *
   * @return bool
   *   Checking state.
   */
  public function isEmpty() {
    return empty($this->data);
  }

}
