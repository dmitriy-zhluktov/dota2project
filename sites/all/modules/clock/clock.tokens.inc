<?php

/**
 * @file
 * Token-related functions for Clock module.
 */

/**
 * Implements hook_token_info()
 */
function clock_token_info() {
  $info = array(
    'types' => array(
      'clock' => array(
        'name' => t('Clock'),
        'description' => t('Tokens related to the display of clocks.'),
        'needs-data' => 'clock',
      ),
      'time-zone' => array(
        'name' => t('Time zone'),
        'description' => t('Tokens related to a certain time zone'),
        'needs-data' => 'time-zone',
      ),
    ),
    'tokens' => array(
      'clock' => array(
        'date' => array(
          'name' => t('Date'),
          'description' => t('The current date of the clock in the correct time zone and format.')
        ),
        'time-zone' => array(
          'name' => t('Time zone'),
          'description' => t('The time zone of the clock.'),
          'type' => 'time-zone',
        ),
        'date-type' => array(
          'name' => t('Date type'),
          'description' => t('The date type of the clock.'),
        ),
        'cid' => array(
          'name' => t('Clock ID'),
          'description' => t('A unique identifier for this clock.'),
        ),
      ),
      'time-zone' => array(
        'name' => array(
          'name' => t('Name'),
          'description' => t('The complete name of the time zone, e.g. America/New York or UTC.'),
        ),
        'contintent' => array(
          'name' => t('Continent'),
          'description' => t('The continent part of the time zone, e.g. America.'),
        ),
        'city' => array(
          'name' => t('City'),
          'description' => t('The city part of the time zone, e.g. New York.'),
        ),
        'offset' => array(
          'name' => t('Offset'),
          'description' => t('The offset of the time zone respective UTC in seconds.'),
        ),
        'offset-name' => array(
          'name' => t('Offset name'),
          'description' => t('The name of the offset, e.g. EDT or UTC'),
        ),
        'daylight-savings-time' => array(
          'name' => t('Daylight savings time marker'),
          'description' => t('A marker (!marker) in case the time zone is currently in daylight savings time.', array('!marker' => theme('clock_daylight_savings_time_marker'))),
        ),
      ),
    ),
  );
  return $info;
}

/**
 * Implements hook_token_info_alter().
 */
function clock_token_info_alter(&$info) {
  $info['tokens']['site']['time-zone'] = array(
    'name' => t('Time zone'),
    'description' => t('The site time zone'),
    'type' => 'time-zone',
  );
  $info['tokens']['user']['time-zone'] = array(
    'name' => t('Time zone'),
    'description' => t("The user's time zone."),
    'type' => 'time-zone',
  );
}

/**
 * Implements hook_tokens().
 */
function clock_tokens($type, $tokens, $data, $options) {
  $language = (isset($options['language']) ? $options['language'] : $GLOBALS['language']);

  $replacements = array();
  if ($type == 'clock') {
    $clock = $data['clock'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'date':
          $replacements[$original] = '<span class="clock-date clock-date-' . $clock->cid . '">' . format_date(REQUEST_TIME, 'custom', clock_get_date_format($clock), clock_get_time_zone($clock), $language->language) . '</span>';
          break;
        case 'time-zone':
          $replacements[$original] = clock_get_time_zone($clock);
          break;
        case 'date-type':
          $replacements[$original] = $clock->date_type;
          break;
        case 'cid':
          $replacements[$original] = $clock->cid;
          break;
      }
    }

    if ($time_zone_tokens = token_find_with_prefix($tokens, 'time-zone')) {
      $replacements += token_generate('time-zone', $time_zone_tokens, array('time-zone' => clock_get_time_zone($clock)), $options);
    }
  }
  elseif ($type == 'time-zone') {
    $time_zone = $data['time-zone'];
    foreach ($tokens as $name => $original) {
      switch($name) {
        case 'name':
          $replacements[$original] = str_replace('_', ' ', $time_zone);
          break;
        case 'contintent':
          $parts = explode('/', $time_zone);
          $replacements[$original] = $parts[0];
          break;
        case 'city':
          $parts = explode('/', $time_zone);
          $replacements[$original] = $parts[1];
          break;
        case 'offset':
          $replacements[$original] = format_date(REQUEST_TIME, 'custom', 'Z', $time_zone);
          break;
        case 'offset-name':
          $replacements[$original] = format_date(REQUEST_TIME, 'custom', 'T', $time_zone);
          break;
        case 'daylight-savings-time':
          $daylight_savings_time = format_date(REQUEST_TIME, 'custom', 'I', $time_zone);
          $replacements[$original] = ($daylight_savings_time ? theme('clock_daylight_savings_time_marker') : '');
          break;
      }
    }
  }
  elseif ($type == 'site') {
    foreach ($tokens as $name => $original) {
      if ($name == 'time-zone') {
        $replacements[$original] = variable_get('date_default_timezone', 'UTC');
      }
    }
  }
  elseif ($type == 'user') {
    $account = $data['user'];
    foreach ($tokens as $name => $original) {
      if ($name == 'time-zone') {
        if ($account->timezone) {
          $replacements[$original] = $account->timezone;
        }
      }
    }
  }
  return $replacements;
}
