<?php
/**
 * @file
 * CTools Plugins Content Types Interface.
 */

namespace CTools\Plugins\ContentTypes;

use CTools\Plugins\PluginInterface;

/**
 * Interface ContentTypesInterface.
 *
 * @package CTools\Plugins\ContentTypes
 */
interface ContentTypesInterface extends PluginInterface {

  /**
   * Preprocess variables before passing them to theme engine.
   *
   * @param array $variables
   *   Values from configuration form.
   * @param array $arguments
   *   Content type arguments.
   * @param \ctools_context $context
   *   Content type context.
   */
  public static function preprocess(array &$variables, array &$arguments, \ctools_context $context = NULL);

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
  public static function configurationForm(array &$form, array &$form_state, array $conf);

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

}
