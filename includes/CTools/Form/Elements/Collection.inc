<?php
/**
 * @file
 * CTools Plugins Content Types Collection Element.
 */

namespace CTools\Form\Elements;

/**
 * Class Collection.
 *
 * @package CTools\Form\Elements
 */
class Collection {

  const TYPE = 'collection';

  /**
   * The "name" HTML attribute for collection.
   *
   * @var string
   */
  private $name = '';
  /**
   * Button names. Texts can be changed before creation.
   *
   * @var string[]
   */
  public static $buttonTexts = [
    'remove' => 'Remove',
    'add' => 'Add new item',
  ];

  /**
   * Helper for creating a fields collection.
   *
   * @param string[] $parents
   *   Nesting names.
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   * @param array $conf
   *   Values from configuration form.
   * @param callable $process
   *   Callback for processing each item in a collection.
   */
  public function __construct(array $parents, array &$form, array &$form_state, array &$conf, callable $process) {
    $this->setName($parents);
    // Pass button texts to translate interface.
    array_map('t', static::$buttonTexts);
    // Check that we already have saved values for our collection.
    $values = drupal_array_get_nested_value($conf, $parents);
    $element = [
      '#tree' => TRUE,
      '#type' => 'table',
      '#sticky' => FALSE,
      '#subtype' => self::TYPE,
      '#attributes' => [
        'class' => ['form-item', self::TYPE],
      ],
    ];

    // If it's a new collection, then init it with a single, empty, value.
    if (NULL === $values) {
      $values = [[]];
    }

    // Walk through the collection items.
    foreach ($values as $i => $value) {
      $items = $process($i, $value);
      $children = element_children($items);

      if (empty($children)) {
        throw new \RuntimeException(t('Callback for collection must return an associative array of form items.'));
      }

      // Remove titles from elements and set them to table cells.
      foreach ($children as $td) {
        $element['#header'][$td] = isset($items[$td]['#title']) ? $items[$td]['#title'] : '';
        unset($items[$td]['#title']);
      }

      // Add the "Remove" button to each row.
      $element['#header']['_operations'] = t('Operations');

      $items['_operations'] = [
        '#type' => 'items',
        '#cell' => [
          'class' => ['operations'],
        ],
      ];

      $items['_operations']['_remove'] = [
        '#ajax' => TRUE,
        // This value MUST BE UNIQUE for every row. Otherwise we always will
        // have the button from last row in $form_state['triggering_element'].
        '#name' => "$this->name[$i]",
        '#type' => 'button',
        '#value' => static::$buttonTexts['remove'],
        '#operation' => 'remove',
        '#limit_validation_errors' => [],
      ];

      $element[] = $items;
    }

    $element['_last']['_add'] = [
      '#ajax' => TRUE,
      '#name' => $this->name,
      '#type' => 'button',
      '#value' => static::$buttonTexts['add'],
      '#operation' => 'add',
      '#limit_validation_errors' => [],
      '#cell' => [
        'colspan' => count($element['#header']),
      ],
    ];

    // Attach a collection, that was generated, to our form.
    drupal_array_set_nested_value($form, $parents, $element);
    // Update form values to able to add new elements to collection.
    drupal_array_set_nested_value($conf, $parents, $values);
  }

  /**
   * Generate "name" HTML attribute for input.
   *
   * @param string[] $parents
   *   Element parents.
   */
  private function setName(array $parents) {
    $this->name = array_shift($parents) . (count($parents) > 1 ? '[' . implode('][', $parents) . ']' : '');
  }

}
