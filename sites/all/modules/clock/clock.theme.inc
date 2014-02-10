<?php

/**
 * @file
 * Theme-related functions for Clock module.
 */

/**
 * Returns HTML for a list of clocks.
 */
function theme_clocks($variables) {
  $clocks = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clock-clocks')),
  );
  foreach ($variables['clocks'] as $cid => $clock) {
    $clocks[$cid] = array(
      '#theme' => 'clock',
      '#clock' => $clock,
      '#prefix' => '<div class="clock-clock-wrapper clock-clock-wrapper-' . $cid . '">',
      '#suffix' => '</div>',
    );
// @todo This currently somewhat works, but overrides the top-level block links.
/**    $clocks["clock-$cid-contextual-links"] = array(
      '#theme' => 'links__contextual',
      '#links' => array(
        "clock-$cid-edit" => array(
          'title' => t('Edit'),
          'href' => "admin/structure/block/manage/clock/clock/configure/$cid/edit"
        ),
        "clock-$cid-delete" => array(
          'title' => t('Delete'),
          'href' => "admin/structure/block/manage/clock/clock/configure/$cid/delete"
        ),
      ),
      '#prefix' => '<div class="contextual-links-wrapper">',
      '#suffix' => '</div>',
      '#attributes' => array('class' => array('contextual-links')),
      '#attached' => array('library' => array(array('contextual', 'contextual-links'))),
    );*/
  }
  return render($clocks);
}

/*
 * Returns HTML for a single clock.
 */
function theme_clock($variables) {
  $clock = (object) $variables['clock'];
  $cid = $clock->cid;

  $output = '';
  $output .= '<div class="clock-clock clock-clock-' . $cid . '">';
  $output .= token_replace($clock->display, array('clock' => $clock), array('clock-date'));
  $output .= '</div>';
  
  // Pass the needed variables to JavaScript.
  $time_zone = clock_get_time_zone($clock);
  $js = array(
    'clock-date-' . $cid => array(
      'time_zone'             => $time_zone,
      'date_format'           => clock_get_date_format($clock),
      'local'                 => ($clock->time_zone_type == 'local' ? TRUE : FALSE),
      // Get the name of the offset, e.g. 'GMT'.
      'offset_name'           => format_date(REQUEST_TIME, 'custom', 'T', $time_zone),
      // Get the time zone offset in seconds.
      'offset_seconds'        => format_date(REQUEST_TIME, 'custom', 'Z', $time_zone),
      // Get Daylight Savings Time information. '1' for yes, '0' for no.
      'daylight_savings_time' => format_date(REQUEST_TIME, 'custom', 'I', $time_zone),
    ),
  );
  drupal_add_js(array('clock' => array(
    'names' => array('clock-date-' . $cid),
    'clocks' => $js,
  )), 'setting');
  drupal_add_js(drupal_get_path('module', 'clock') . '/clock.js');
 
  return $output;
}

/**
 * Returns HTML for a Daylight Savings Time marker.
 */
function theme_clock_daylight_savings_time_marker($variables) {
  drupal_add_css(drupal_get_path('module', 'clock') . '/clock.base.css');
  return '<span class="clock-daylight-savings-time-marker">*</span>';
}