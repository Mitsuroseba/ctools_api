<?php
/**
 * @file
 * CTools API Styles Interface.
 */

namespace CTools\API\Styles;

use CTools\API\PluginInterface;

/**
 * Interface ContentTypesInterface.
 *
 * @package CTools\API\ContentTypes
 */
interface StylesInterface extends PluginInterface {

  /**
   * Configuration form for pane.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   * @param \panels_display $display
   *   Panels display.
   * @param string $id
   *   Panel ID or region name (depends on renderer).
   */
  public static function configurationForm(array &$form, array &$form_state, array $conf, \panels_display $display, $id);

}
