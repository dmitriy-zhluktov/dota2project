<?php

define("RUN_DOTA2_URL", "steam://run/" . DOTA2_GAME_ID);
define("SEND_MESSAGE_URL", "steam://friends/message/");
define("ADD_TO_FRIENDS_URL", "steam://friends/add/");

/**
 * Add steam browser protocol link to form
 * @param array $form Form array to add link to
 * @param string $formElementName name pf the element in form array
 * @param type $steamUrl Steam browser protocol command url (some of them defined in constants above)
 * @param string $urlText text to display in <a> tag
 * 
 */
function steam_api_build_steam_link($form, $formElementName, $steamUrl, $urlText) {
  $steamLinkClass = "steam-browser-link";
  $jsToInclude =  drupal_get_path('module', 'steam_api') . '/js/linksBehavior.js';
  if (!in_array($jsToInclude, $form['#attached']['js']))
  {
    $form['#attached']['js'][] = $jsToInclude;
  }
  $urlToReturn = '<a href="' . $steamUrl . '" class="' . $steamLinkClass . '">' . t($urlText) . '</a>';
  $form["$formElementName"]['#markup'] = $urlToReturn;
}
 