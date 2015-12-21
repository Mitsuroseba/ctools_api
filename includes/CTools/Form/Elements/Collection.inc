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
   * Helper for creating a fields collection.
   *
   * @param array $parents
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
    $name = end($parents);
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

      // Remove titles from elements and set them to table cells.
      foreach (element_children($items) as $td) {
        $element['#header'][$td] = isset($items[$td]['#title']) ? $items[$td]['#title'] : '';
        unset($items[$td]['#title']);
      }

      // Add the "Remove" button to each row.
      $element['#header']['operations'] = t('Operations');

      $items['operations'] = [
        '#type' => 'items',
        '#cell' => [
          'class' => ['operations'],
        ],
      ];

      $items['operations']['_remove'] = [
        '#ajax' => TRUE,
        '#name' => $name,
        '#type' => 'button',
        '#value' => t('Remove'),
        '#operation' => 'remove',
        '#limit_validation_errors' => [],
      ];

      $element[] = $items;
    }

    // Create the last row with all empty cells, except the last one.
    $element['_last'] = array_fill(0, count($element['#header']) - 1, '');
    // Add the "Add item" button to the last cell of the last row.
    $element['_last']['_add'] = [
      '#ajax' => TRUE,
      '#name' => $name,
      '#type' => 'button',
      '#value' => t('Add item'),
      '#operation' => 'add',
      '#limit_validation_errors' => [],
    ];

    // Attach a collection, that was generated, to our form.
    drupal_array_set_nested_value($form, $parents, $element);
    // Update form values to able to add new elements to collection.
    drupal_array_set_nested_value($conf, $parents, $values);
  }

}
