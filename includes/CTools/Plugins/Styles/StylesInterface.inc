<?php
/**
 * @file
 * CTools Plugins Styles Interface.
 */

namespace CTools\Plugins\Styles;

use CTools\Plugins\TemplatablePluginInterface;

/**
 * Interface ContentTypesInterface.
 *
 * @package CTools\Plugins\ContentTypes
 */
interface StylesInterface extends TemplatablePluginInterface {

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
