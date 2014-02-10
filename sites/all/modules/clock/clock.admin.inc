<?php

/**
 * @file
 * Page generation functions for the administrative pages.
 */

/**
 * Form builder for the clock add or edit form.
 *
 * @param $clock
 *   (optional) A clock object as returned by clock_info() that will be used for
 *   default values.
 */
function clock_form(&$form, &$form_state, $clock) {
  $form += array('#tree' => TRUE);

  // Preview
  // @todo Provide a preview of the clock that updates via AJAX.

  // Time zone type
  $configurable_timezones = variable_get('configurable_timezones', 1);
  $form['time_zone_type'] = array(
    '#type' => 'select',
    '#title' => t('Time zone type'),
    '#description' => t('<em>Site time zone</em> is the time zone selected on the <a href="@regional-settings-url">regional settings</a> page.<br><em>Local time zone</em> is the user\'s operating system\'s time zone.<br><em>Custom time zone</em> is the time zone selected below.', array('@regional-settings-url' => url('admin/config/regional/settings'))),
    '#default_value' => $clock->time_zone_type,
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'clock') . '/clock.admin.js'),
      'css' => array(drupal_get_path('module', 'clock') . '/clock.admin.css'),
    ),
    '#child_descriptions' => array('#attributes' => array('class' => array('element-hidden'))),
  );
  if ($configurable_timezones) {
    $form['time_zone_type']['#description'] = t('<em>User time zone</em> is the time zone the user selected on his or her profile page.<br><em>Site time zone</em> is the time zone selected on the <a href="@regional-settings-url">regional settings</a> page.<br><em>Local time zone</em> is the user\'s operating system\'s time zone.<br><em>Custom time zone</em> is the time zone selected below.', array('@regional-settings-url' => url('admin/config/regional/settings')));
  }
  $site_time_zone = variable_get('date_default_timezone', 'UTC');
  $user_time_zone = (!empty($GLOBALS['user']->timezone) ? $GLOBALS['user']->timezone : $site_time_zone);
  $form['time_zone_type']['user'] = array(
    '#type' => 'option',
    '#title' => t('User time zone'),
    '#description' => t('The time zone the user selected on his or her profile page. For the current user this is currently %user_time_zone.', array('%user_time_zone' => $user_time_zone)),
    '#access' => $configurable_timezones,
  );
  $form['time_zone_type']['site'] = array(
    '#type' => 'option',
    '#title' => t('Site time zone'),
    '#description' => t('The time zone selected on the <a href="@regional-settings-url">regional settings</a> page. Currenty this is %site_time_zone.', array('@regional-settings-url' => url('admin/config/regional/settings'), '%site_time_zone' => $site_time_zone)),
  );
  $form['time_zone_type']['local'] = array(
    '#type' => 'option',
    '#title' => t('Local time zone'),
    '#description' => t("The user's operating system's time zone."),
  );
  $form['time_zone_type']['custom'] = array(
    '#type' => 'option',
    '#title' => t('Custom time zone'),
    '#description' => t('The time zone selected below.'),
  );

  // Custom time zone
  // @todo Turn this into a 'time_zone' element with dedicated selects for
  //   continent and city.
  $form['custom_time_zone'] = array(
    '#type' => 'select',
    '#title' => t('Custom time zone'),
    '#description' => t('This is only used if <em>Custom time zone</em> is selected above.'),
    '#options' => system_time_zones(),
    '#default_value' => $clock->custom_time_zone,
    // This should only be visible if the 'custom time zone' is selected as time
    // zone type.
    '#states' => array(
      'visible' => array(
          ':input[name="time_zone_type"]' => array("value" => "custom"),
      ),
    ),
  );

  // Date types
  $date_types = array();
  foreach (system_get_date_types() as $date_type => $info) {
    $date_types[$date_type] = $info['title'];
  }
  $form['date_type'] = array(
    '#type' => 'select',
    '#title' => t('Date type'),
    '#description' => t('Date types can be configured on the <a href="@date-time-url">date and time settings</a> page.', array('@date-time-url' => url('admin/config/regional/date-time'))),
    '#options' => $date_types,
    '#default_value' => $clock->date_type,
  );

  // Display.
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    // Do not collapse the fieldset if the display has been edited before.
    '#collapsed' => ($clock->display == '[clock:date]'),
  );
  $form['advanced']['display'] = array(
    '#type' => 'textfield',
    '#title' => t('Display'),
    '#description' => t('By default the date of the clock is displayed as is. This is signified by the "[clock:date]" token. You can add surrounding text or additional tokens to the display, e.g. "Current date: [clock:date]" or "[clock:time-zone]: [clock:date]". For an overview of available tokens install the <a href="@token-url">Token</a> module.', array('@token-url' => 'http://drupal.org/project/token')),
    '#default_value' => $clock->display,
  );
  if (module_exists('token')) {
    $form['advanced']['display']['#description'] = t('By default the date of the clock is displayed as is. This is signified by the "[clock:date]" token. You can add surrounding text or additional tokens to the display, e.g. "Current date: [clock:date]" or "[clock:time-zone]: [clock:date]".');

    $form['advanced']['tokens_clock'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('clock'),
      '#global_types' => (isset($form_state['values']['advanced']['token']['global']) ? $form_state['values']['advanced']['token']['global'] : FALSE),
      '#prefix' => '<div id="clock-form-token-tree">',
      '#suffix' => '</div>',
    );

    $form['advanced']['tokens_global'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show global tokens'),
      '#description' => t('Show tokens that are globally available, but not related to the display of clocks.'),
      '#ajax' => array(
        'callback' => 'clock_form_token_tree',
        'wrapper' => 'clock-form-token-tree',
      ),
    );
  }

  // Actions
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/structure/block/manage/clock/clock/configure',
  );

  return $form;
}

/**
 * AJAX callback that return the token tree of the clock form.
 */
function clock_form_token_tree($form, $form_state) {
  return $form['advanced']['token']['tree'];
}

/**
 * Builds a clock object from the passed in form values.
 *
 * @param array $values
 *   An array of form values that corresponds to $form_state['values'].
 *
 * @return
 *   A clock object.
 */
function clock_get_clock_from_form_values($values) {
  $clock = new StdClass;
  $clock->time_zone_type = $values['time_zone_type'];
  $clock->custom_time_zone = $values['custom_time_zone'];
  $clock->date_type = $values['date_type'];
  $clock->display = $values['advanced']['display'];
  return $clock;
}

/**
 * Form builder for the clock add form.
 *
 * @see clock_add_form_submit()
 * @see clock_form()
 */
function clock_add_form($form, &$form_state) {
  $form = array();
  return clock_form($form, $form_state, clock_default_clock());
}

/**
 * Form submission handler for clock_add_form().
 *
 * @see clock_add_form()
 */
function clock_add_form_submit($form, &$form_state) {
  $clock = clock_get_clock_from_form_values($form_state['values']);
  $clock->weight = clock_get_weight();
  clock_add($clock);
  $form_state['redirect'] = 'admin/structure/block/manage/clock/clock/configure';
}

/**
 * Form builder for the clock edit form.
 *
 * @param $clock
 *   A clock object as returned by clock_info().
 *
 * @see clock_edit_form_submit()
 * @see clock_form()
 */
function clock_edit_form($form, &$form_state, $clock) {
  $form = array();
  $form['cid'] = array('#type' => 'value', '#value' => $clock->cid);
  return clock_form($form, $form_state, $clock);
}

/**
 * Form submission handler for clock_edit_form().
 *
 * @see clock_edit_form()
 */
function clock_edit_form_submit($form, &$form_state) {
  $cid = $form_state['values']['cid'];
  $old_clock = clock_info($cid);
  $clock = clock_get_clock_from_form_values($form_state['values']);
  $clock->weight = $old_clock->weight;
  clock_update($cid, $clock);
  $form_state['redirect'] = 'admin/structure/block/manage/clock/clock/configure';
}

/**
 * Form builder for the clock delete form.
 * 
 * @param $clock
 *   A clock object as returned by clock_info().
 *
 * @see clock_delete_form_submit()
 */
function clock_delete_form($form, &$form_state, $clock) {
  $form = array();
  $form['cid'] = array('#type' => 'value', '#value' => $clock->cid);
  $form['delete'] = array(
    '#markup' => '<p>' . t('Are you sure you want to delete this clock?') . '</p>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/structure/block/manage/clock/clock/',
  );
  return $form;
}

/**
 * Form submission handler for clock_delete_form()
 *
 * @see clock_delete_form()
 */
function clock_delete_form_submit($form, &$form_state) {
  clock_delete($form_state['values']['cid']);
  $form_state['redirect'] = 'admin/structure/block/manage/clock/clock/configure';
}
