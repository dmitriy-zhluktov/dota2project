<?php 



function steam_api_create_web_api_object() {
//  $web_api = new WebApi();

  WebApi::setApiKey(API_KEY);

  $interfaces = WebApi::getInterfaces();

  $iSteamUser;
  $iSteamNews;
  $iSteamUserStats;
  $iteamWebUserPresenceOAuth;
  foreach ($interfaces as $i) {
    if ($i->name == "ISteamUser") {
      $iSteamUser = $i;
    }
    if ($i->name == "ISteamNews") {
      $iSteamNews = $i;
    }
    if ($i->name == "ISteamUserStats") {
      $iSteamUserStats = $i;
    }
    if ($i->name == "ISteamWebUserPresenceOAuth") {
      $iteamWebUserPresenceOAuth = $i;
    }
  }
  return $interfaces;
}

function steam_api_test_form($form, $form_state) {
  $interfaces = steam_api_create_web_api_object();



  $options = array();

  foreach ($interfaces as $i) {
    $options[] = $i->name;
  }

  $variables = array(
    'items' => $options,
    'type' => 'ul',
    'title' => "asdas",
    'attributes' => array()
  );

  $form['link'] = array(
    '#markup' => theme_item_list($variables)
  );

  $id = SteamId::create('arcntand');
  $id = SteamId::create('ebovikdugor');
  $steam64id = $id->getSteamId64();

  steam_api_get_user_games($steam64id);

  drupal_add_js(
      'jQuery(document).ready(function(){
          jQuery("a.menu-1-2").click(function() {
               var url = decodeURI(this.href);
               var splittedUrlArray = url.split("steam");
               var neededUrl = "steam" + splittedUrlArray[1];
               window.open(neededUrl);
               return false; 
          }    
          ); 
        },)'
      , 'inline');

  $steamAddUrl = "steam://friends/add/" . $steam64id;
  $form['test_add_to_friend_link']['#markup'] = steam_api_build_steam_link($steamAddUrl, "Add");

  $steamSMUrl = "steam://friends/message/" . $steam64id;
  $form['test_send_msg']['#markup'] = steam_api_build_steam_link($steamSMUrl, "Send message");

  $steamJoinChatUrl = "steam://friends/joinchat/" . $steam64id;
  $form['test_join_msg']['#markup'] = steam_api_build_steam_link($steamJoinChatUrl, "Join chat");

  $steamRunGameUrl = "steam://run/" . DOTA2_GAME_ID;
  $form['test_join_msg']['#markup'] = steam_api_build_steam_link($steamRunGameUrl, "Run Game");

  return $form;
}

function _steam_api_get_interface($interfaceName) {
  $neededInterface;
  $interfaces = WebApi::getInterfaces();
  foreach ($interfaces as $i) {
    if ($i->name == $interfaceName) {
      $neededInterface = $i;
    }
  }

  return $neededInterface;
}

function steam_api_get_user_games($steam64id) {
  $id = SteamId::create($steam64id);
  //$userGames = $id->getGames();
  //$userStats = $id->getGameStats(DOTA2_GAME_ID);
  WebApi::setApiKey(API_KEY);
  $iSteamGameInterface = _steam_api_get_interface("IDOTA2Match_570");
  //$recentGames = WebApi::getJSON("IPlayerService", "GetRecentlyPlayedGames");
  $recentGames = WebApi::load("json", "IPlayerService", "GetRecentlyPlayedGames", '0001', array('steamid' => $steam64id));
  dsm($recentGames);
}

function steam_api_get_match_info($matchId) {
  WebApi::setApiKey(API_KEY);
  $recentGames = WebApi::load("json", "IDOTA2Match_570", "GetMatchHistory", '0001', array('steamid' => $matchId));
  dsm($recentGames);
}

function steam_api_build_steam_link($steamUrl, $urlText) {
  $steamLinkClass = "steam-link";
  $urlToReturn = '<a href="' . $steamUrl . '" class="' . $steamLinkClass . '">' . t($urlText) . '</a>';
  return $urlToReturn;
}
 


