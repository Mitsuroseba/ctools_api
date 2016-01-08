<?php
/**
 * @file
 * CTools Plugins Configurable Plugin Interface.
 */

namespace CTools\Plugins;

/**
 * Interface ConfigurablePluginInterface.
 *
 * @package CTools\Plugins
 * @access private
 */
interface ConfigurablePluginInterface extends PluginInterface {

  /**
   * Configuration form.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf);

  /**
   * Submission callback for configuration form.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf);

  /**
   * Dispatcher for AJAX callbacks.
   *
   * Will be triggered on elements with '#ajax' => TRUE.
   *
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Saved configuration values.
   * @param array $commands
   *   Collection of AJAX commands.
   * @param array $element
   *   Triggering element.
   */
  public static function configurationFormElementCallback(array &$form, array &$form_state, array &$conf, array &$commands, array $element);
}
