<?php
/**
 * @file
 * CTools API Styles Pane Interface.
 */

namespace CTools\API\Styles;

/**
 * Interface StylesPaneInterface.
 *
 * @package CTools\API\Styles
 */
interface StylesPaneInterface extends StylesInterface {

  /**
   * Preprocess variables before passing them to theme engine.
   *
   * @param \stdClass $content
   *   Content block.
   * @param array $settings
   *   Values from configuration form.
   * @param array $variables
   *   All available variables (including "settings" and "content").
   */
  public static function preprocess(\stdClass $content, array &$settings, array &$variables);

}
