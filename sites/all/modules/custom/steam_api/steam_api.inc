<?php 

//
//function steam_api_test_form($form, $form_state) {
//
//  $iDotaMatch = _steam_api_get_interface("IDOTA2Match_570");
//
//
//  $options = array();
//
//  foreach ($interfaces as $i) {
//    $options[] = $i->name;
//  }
//
//  $variables = array(
//    'items' => $options,
//    'type' => 'ul',
//    'title' => "asdas",
//    'attributes' => array()
//  );
//
//  $form['link'] = array(
//    '#markup' => theme_item_list($variables)
//  );
//
//  $id = SteamId::create('arcntand');
//  //$id = SteamId::create('ebovikdugor');
//  $steam64id = $id->getSteamId64();
//
//  steam_api_get_user_games($steam64id);
//
//  drupal_add_js(
//      'jQuery(document).ready(function(){
//          jQuery("a.menu-1-2").click(function() {
//               var url = decodeURI(this.href);
//               var splittedUrlArray = url.split("steam");
//               var neededUrl = "steam" + splittedUrlArray[1];
//               window.open(neededUrl);
//               return false; 
//          }    
//          ); 
//        },)'
//      , 'inline');
//
//  $steamAddUrl = "steam://friends/add/" . $steam64id;
//  $form['test_add_to_friend_link']['#markup'] = steam_api_build_steam_link($steamAddUrl, "Add");
//
//  $steamSMUrl = "steam://friends/message/" . $steam64id;
//  $form['test_send_msg']['#markup'] = steam_api_build_steam_link($steamSMUrl, "Send message");
//
//  $steamJoinChatUrl = "steam://friends/joinchat/" . $steam64id;
//  $form['test_join_msg']['#markup'] = steam_api_build_steam_link($steamJoinChatUrl, "Join chat");
//
//  $steamRunGameUrl = "steam://run/" . DOTA2_GAME_ID;
//  $form['test_join_msg']['#markup'] = steam_api_build_steam_link($steamRunGameUrl, "Run Game");

  //$matches = steam_api_get_match_history(5);
  
  //steam_api_fet_match_details()
//  $decoded_matches = json_decode($matches);
//  drupal_set_message($matches);
  
//  try 
//  {
//    $history = steam_api_get_match_details("398064340");
//    $form['result']['#markup'] = $history->result->human_players;
//  }
//  catch (Exception $ex)
//  {
//    drupal_set_message($ex->getMessage(), "error");
//  }
//    
//    return $form;
//}

/**
 * Returns a steam web api interface with given name
 * @param string $interfaceName name of the interface to get
 * @return object $neededInterface object with interface description data
 * @throws Exception If interface was not found
 */

function _steam_api_get_interface($interfaceName) {
  $neededInterface;
  WebApi::setApiKey(API_KEY);
  $interfaces = WebApi::getInterfaces();
  foreach ($interfaces as $i) {
    if ($i->name == $interfaceName) {
      $neededInterface = $i;
    }
  }

  if (!isset($neededInterface))
  {
    throw new Exception("Interface with given name not found");
  }
  
  return $neededInterface;
}

/**
 * Getting the match details: results, players, etc 
 * @param string $matchId id of the match to get info about
 * @return object $decoded match detail object
 * @throws Exception If match was not found
 */

function steam_api_get_match_details($matchId)
{
  WebApi::setApiKey(API_KEY);
  $matchDetails = WebApi::load("json", "IDOTA2Match_570", "GetMatchDetails", '0001', array('match_id' => $matchId));
  $decoded = json_decode($matchDetails);
  if (isset($decoded->result->error))
  {
    throw new Exception($decoded->result->error);  
  }
  return $decoded;
}

/**
 * Returns a certain number of recent matches
 * @param int $matchesCount number of matches to get
 * @param string $account_id Account id to get matches for (Steam64bit id format)
 * @return object $decoded object contained a certain number of matches
 * @throws Exception If an error occurs
 */
function steam_api_get_match_history($matchesCount = null, $account_id = null) {
  $options = array();
  if (!isset($matchesCount))
  {
    $matchesCount = 100;
  }
  $options['matches_requested'] = $matchesCount;
  if (isset($account_id))
  {
     $options['account_id'] = $account_id;
  }
  WebApi::setApiKey(API_KEY);
  $matchHistory = WebApi::load("json", "IDOTA2Match_570", "GetMatchHistory", '0001', $options);
  $decoded = json_decode($matchHistory);
  if (isset($decoded->result->error))
  {
    throw new Exception($decoded->result->error);  
  }
  return $decoded;
}