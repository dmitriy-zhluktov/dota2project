<?php

/**
 * @file
 * Backports the 'table' form element from Drupal 7
 *
 * Together with the properly defined 'table' element itself these functions
 * are sufficient for the 'table' element. And are identical to those in Drupal
 * 8 core.
 *
 * @see clock_element_info()
 * @see https://drupal.org/node/80855
 */

/**
 * Determines the value of a table form element.
 *
 * @param array $element
 *   The form element whose value is being populated.
 * @param array|false $input
 *   The incoming input to populate the form element. If this is FALSE,
 *   the element's default value should be returned.
 *
 * @return array
 *   The data that will appear in the $form_state['values'] collection
 *   for this element. Return nothing to use the default.
 */
function form_type_table_value(array $element, $input = FALSE) {
  // If #multiple is FALSE, the regular default value of radio buttons is used.
  if (!empty($element['#tableselect']) && !empty($element['#multiple'])) {
    // Contrary to #type 'checkboxes', the default value of checkboxes in a
    // table is built from the array keys (instead of array values) of the
    // #default_value property.
    // @todo D8: Remove this inconsistency.
    if ($input === FALSE) {
      $element += array('#default_value' => array());
      return drupal_map_assoc(array_keys(array_filter($element['#default_value'])));
    }
    else {
      return is_array($input) ? drupal_map_assoc($input) : array();
    }
  }
}

/**
 * #process callback for #type 'table' to add tableselect support.
 *
 * @param array $element
 *   An associative array containing the properties and children of the
 *   table element.
 * @param array $form_state
 *   The current state of the form.
 *
 * @return array
 *   The processed element.
 *
 * @see form_process_tableselect()
 * @see theme_tableselect()
 */
function form_process_table($element, &$form_state) {
  if ($element['#tableselect']) {
    if ($element['#multiple']) {
      $value = is_array($element['#value']) ? $element['#value'] : array();
    }
    // Advanced selection behaviour makes no sense for radios.
    else {
      $element['#js_select'] = FALSE;
    }
    // Add a "Select all" checkbox column to the header.
    // @todo D8: Rename into #select_all?
    if ($element['#js_select']) {
      $element['#attached']['library'][] = array('system', 'drupal.tableselect');
      array_unshift($element['#header'], array('class' => array('select-all')));
    }
    // Add an empty header column for radio buttons or when a "Select all"
    // checkbox is not desired.
    else {
      array_unshift($element['#header'], '');
    }

    if (!isset($element['#default_value']) || $element['#default_value'] === 0) {
      $element['#default_value'] = array();
    }
    // Create a checkbox or radio for each row in a way that the value of the
    // tableselect element behaves as if it had been of #type checkboxes or
    // radios.
    foreach (element_children($element) as $key) {
      // Do not overwrite manually created children.
      if (!isset($element[$key]['select'])) {
        // Determine option label; either an assumed 'title' column, or the
        // first available column containing a #title or #markup.
        // @todo Consider to add an optional $element[$key]['#title_key']
        //   defaulting to 'title'?
        $title = '';
        if (!empty($element[$key]['title']['#title'])) {
          $title = $element[$key]['title']['#title'];
        }
        else {
          foreach (element_children($element[$key]) as $column) {
            if (isset($element[$key][$column]['#title'])) {
              $title = $element[$key][$column]['#title'];
              break;
            }
            if (isset($element[$key][$column]['#markup'])) {
              $title = $element[$key][$column]['#markup'];
              break;
            }
          }
        }
        if ($title !== '') {
          $title = t('Update !title', array('!title' => $title));
        }

        // Prepend the select column to existing columns.
        $element[$key] = array('select' => array()) + $element[$key];
        $element[$key]['select'] += array(
          '#type' => $element['#multiple'] ? 'checkbox' : 'radio',
          '#title' => $title,
          '#title_display' => 'invisible',
          // @todo If rows happen to use numeric indexes instead of string keys,
          //   this results in a first row with $key === 0, which is always FALSE.
          '#return_value' => $key,
          '#attributes' => $element['#attributes'],
        );
        $element_parents = array_merge($element['#parents'], array($key));
        if ($element['#multiple']) {
          $element[$key]['select']['#default_value'] = isset($value[$key]) ? $key : NULL;
          $element[$key]['select']['#parents'] = $element_parents;
        }
        else {
          $element[$key]['select']['#default_value'] = ($element['#default_value'] == $key ? $key : NULL);
          $element[$key]['select']['#parents'] = $element['#parents'];
          $element[$key]['select']['#id'] = drupal_html_id('edit-' . implode('-', $element_parents));
        }
      }
    }
  }

  return $element;
}

/**
 * #element_validate callback for #type 'table'.
 *
 * @param array $element
 *   An associative array containing the properties and children of the
 *   table element.
 * @param array $form_state
 *   The current state of the form.
 */
function form_validate_table($element, &$form_state) {
  // Skip this validation if the button to submit the form does not require
  // selected table row data.
  if (empty($form_state['triggering_element']['#tableselect'])) {
    return;
  }
  if ($element['#multiple']) {
    if (!is_array($element['#value']) || !count(array_filter($element['#value']))) {
      form_error($element, t('No items selected.'));
    }
  }
  elseif (!isset($element['#value']) || $element['#value'] === '') {
    form_error($element, t('No item selected.'));
  }
}

/**
 * #pre_render callback to transform children of an element into #rows suitable for theme_table().
 *
 * This function converts sub-elements of an element of #type 'table' to be
 * suitable for theme_table():
 * - The first level of sub-elements are table rows. Only the #attributes
 *   property is taken into account.
 * - The second level of sub-elements is converted into columns for the
 *   corresponding first-level table row.
 *
 * Simple example usage:
 * @code
 * $form['table'] = array(
 *   '#type' => 'table',
 *   '#header' => array(t('Title'), array('data' => t('Operations'), 'colspan' => '1')),
 *   // Optionally, to add tableDrag support:
 *   '#tabledrag' => array(
 *     array('order', 'sibling', 'thing-weight'),
 *   ),
 * );
 * foreach ($things as $row => $thing) {
 *   $form['table'][$row]['#weight'] = $thing['weight'];
 *
 *   $form['table'][$row]['title'] = array(
 *     '#type' => 'textfield',
 *     '#default_value' => $thing['title'],
 *   );
 *
 *   // Optionally, to add tableDrag support:
 *   $form['table'][$row]['#attributes']['class'][] = 'draggable';
 *   $form['table'][$row]['weight'] = array(
 *     '#type' => 'textfield',
 *     '#title' => t('Weight for @title', array('@title' => $thing['title'])),
 *     '#title_display' => 'invisible',
 *     '#size' => 4,
 *     '#default_value' => $thing['weight'],
 *     '#attributes' => array('class' => array('thing-weight')),
 *   );
 *
 *   // The amount of link columns should be identical to the 'colspan'
 *   // attribute in #header above.
 *   $form['table'][$row]['edit'] = array(
 *     '#type' => 'link',
 *     '#title' => t('Edit'),
 *     '#href' => 'thing/' . $row . '/edit',
 *   );
 * }
 * @endcode
 *
 * @param array $element
 *   A structured array containing two sub-levels of elements. Properties used:
 *   - #tabledrag: The value is a list of arrays that are passed to
 *     drupal_add_tabledrag(). The HTML ID of the table is prepended to each set
 *     of arguments.
 *
 * @see system_element_info()
 * @see theme_table()
 * @see drupal_process_attached()
 * @see drupal_add_tabledrag()
 */
function drupal_pre_render_table(array $element) {
  foreach (element_children($element) as $first) {
    $row = array('data' => array());
    // Apply attributes of first-level elements as table row attributes.
    if (isset($element[$first]['#attributes'])) {
      $row += $element[$first]['#attributes'];
    }
    // Turn second-level elements into table row columns.
    // @todo Do not render a cell for children of #type 'value'.
    // @see http://drupal.org/node/1248940
    foreach (element_children($element[$first]) as $second) {
      // Assign the element by reference, so any potential changes to the
      // original element are taken over.
      $column = array('data' => &$element[$first][$second]);

      // Apply wrapper attributes of second-level elements as table cell
      // attributes.
      if (isset($element[$first][$second]['#wrapper_attributes'])) {
        $column += $element[$first][$second]['#wrapper_attributes'];
      }

      $row['data'][] = $column;
    }
    $element['#rows'][] = $row;
  }

  // Take over $element['#id'] as HTML ID attribute, if not already set.
  element_set_attributes($element, array('id'));

  // If the custom #tabledrag is set and there is a HTML ID, inject the table's
  // HTML ID as first callback argument and attach the behavior.
  if (!empty($element['#tabledrag']) && isset($element['#attributes']['id'])) {
    foreach ($element['#tabledrag'] as &$args) {
      array_unshift($args, $element['#attributes']['id']);
    }
    $element['#attached']['drupal_add_tabledrag'] = $element['#tabledrag'];
  }

  return $element;
}
