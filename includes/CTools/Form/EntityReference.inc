<?php
/**
 * @file
 * Abstraction for "Entity Reference" form element.
 *
 * @todo Think about easiest implementation of this feature for end users.
 */

namespace CTools\Form;

/**
 * Trait EntityReference.
 *
 * @package CTools\Form
 */
trait EntityReference {

  /**
   * {@inheritdoc}
   */
  protected static function entityReferenceCollection(array $parents, array &$form, array &$form_state, array $conf) {
    self::initFormState($form_state);

    new Elements\Collection($parents, $form, $form_state, $conf, function ($i, $data) use ($form_state) {
      return self::referenceItem($form_state, $data);
    });
  }

  /**
   * {@inheritdoc}
   */
  protected static function entityReferenceElement(array $parents, array &$form, array &$form_state, array $conf) {
    self::initFormState($form_state);

    $item = ['#tree' => TRUE];
    $item += self::referenceItem($form_state, drupal_array_get_nested_value($conf, $parents) ?: $conf);

    drupal_array_set_nested_value($form, $parents, $item);
  }

  /**
   * Store entity types and information in form state.
   *
   * @param array $form_state
   *   Drupal form state.
   */
  private static function initFormState(array &$form_state) {
    if (empty($form_state['entity_info'])) {
      $form_state['entity_info'] = entity_get_info();
    }

    if (empty($form_state['entity_types'])) {
      $form_state['entity_types'] = [];

      foreach ($form_state['entity_info'] as $type => $info) {
        $form_state['entity_types'][$type] = $info['label'];
      }
    }
  }

  /**
   * Clean value of the "reference" field when entity type or bundle changed.
   *
   * @param string $type
   *   Machine name of the element.
   * @param array $form
   *   Form elements implementation.
   * @param array $form_state
   *   Drupal form state.
   *
   * @return array
   *   Interacting element.
   */
  private static function &cleanReferenceValueWhenAjaxTriggeredOn($type, array &$form, array &$form_state) {
    $parents = $form_state['triggering_element']['#parents'];
    $element =& drupal_array_get_nested_value($form, array_slice($parents, 0, -1));

    if (NULL !== $element && end($parents) === $type) {
      unset($element['reference']['#value']);
    }

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  private static function referenceItem(array $form_state, array $data) {
    $item = [];

    $item['entity_type'] = [
      '#ajax' => TRUE,
      '#type' => 'select',
      '#title' => t('Entity'),
      '#options' => $form_state['entity_types'],
      '#required' => TRUE,
    ];

    $item['bundles'] = [
      '#ajax' => TRUE,
      '#type' => 'select',
      '#title' => t('Bundles'),
      '#access' => FALSE,
      '#multiple' => TRUE,
    ];

    $item['reference'] = [
      '#type' => 'entityreference',
      '#title' => t('Item'),
      '#access' => FALSE,
    ];

    if (!empty($data['entity_type'])) {
      $bundles = [];

      foreach ($form_state['entity_info'][$data['entity_type']]['bundles'] as $bundle => $info) {
        $bundles[$bundle] = $info['label'];
      }

      $item['bundles'] = array_merge($item['bundles'], [
        '#access' => TRUE,
        '#options' => $bundles,
      ]);
    }

    if (!empty($data['bundles'])) {
      $item['reference'] = array_merge($item['reference'], [
        '#access' => TRUE,
        '#era_bundles' => $data['bundles'],
        '#era_entity_type' => $data['entity_type'],
      ]);
    }

    return $item;
  }

  /**
   * Element AJAX callback.
   *
   * @see ctools_api_ctools_api_content_type_edit_form_ajax()
   */
  public static function ajaxCallbackEntityType(array &$form, array &$form_state, array &$commands, array $conf) {
    $element =& self::cleanReferenceValueWhenAjaxTriggeredOn('entity_type', $form, $form_state);

    // Check for "#action" existence is necessary to
    // prevent hiding root form elements.
    if (isset($element['reference']) && !isset($element['#action'])) {
      hide($element['reference']);
    }
  }

  /**
   * Element AJAX callback.
   *
   * @see ctools_api_ctools_api_content_type_edit_form_ajax()
   */
  public static function ajaxCallbackBundles(array &$form, array &$form_state, array &$commands, array $conf) {
    self::cleanReferenceValueWhenAjaxTriggeredOn('bundles', $form, $form_state);
  }

  /**
   * Necessary modules for form elements building.
   *
   * @return string[]
   *   Module machine names.
   */
  public static function dependencies() {
    return ['entityreference_autocomplete', 'table_element'];
  }

}
