<?php

function match_get_node_data() {
	
	global $user;
	$node = menu_get_object();
	
	$type = $node->field_state['und'][0]['value']; //0 - план, 1 - актив, 2 - заверш
	$html = '';
	switch ($type) {
		case 0:
			$html = match_get_node_planned_markup($node);
			break;
		case 1:
			$html = match_get_node_active_markup($node, $user);
			break;
		case 2:
			$html = match_get_node_finished_markup($node, $user);
			break;			
	}
	//krumo($node);
	return $html;
 }

function match_get_node_planned_markup($node) {
	//krumo($node);
    $author = user_load($node->uid);
    $state = $node->field_state['und'][0]['value'];
    switch($state) {
        case 0:
            $stateText = t('Planned');
            break;
        case 1:
            $stateText = t('Active');
            break;
        case 2:
            $stateText = t('Finished');
            break;
    }
    $last = ($node->field_last['und'][0]['value'] == 1)? t('Last match'): t('');
    $mod = (!isset($node->field_mod['und'][0]['value']) || $node->field_mod['und'][0]['value'] == '')? '' : '<h3>'.t('Mod:').'</h3>'.t($node->field_mod['und'][0]['value']);
    $html = '<div class="node-match-planned-info">
                <div class="plain-text"><h3>'.t('PRO:').'</h3>'.l($author->name, 'user/'.$author->uid, array('attributes' => array('class' => array('pro-user')))).'</div>
                <div class="plain-text"><h3>'.t('Match status:').'</h3>'.$stateText.'</div>
                <div class="plain-text">'.$mod.'</div>
                <div class="plain-text"><h3>'.t('Match starts in:').'</h3>'.match_get_planned_timing(strtotime($node->field_date['und'][0]['value']), $node->uid).'</div>
                <div class="plain-text"><h3>'.$last.'</h3></div>
            </div>';
	$html .= match_get_planned_players_html($node->field_profi_command['und'][0]['value']);
	$form = drupal_get_form('match_node_planned_form');

    if($node->field_free_place['und'][0]['value'] > 0) {
        $ticketform = drupal_get_form('match_node_planned_ticket_form');
        $html .= drupal_render($ticketform);
    }
	$html .= drupal_render($form);
    module_load_include('module', 'match', '../match');

	return $html;
}
 
function match_get_node_active_markup($node, $user) {
	//krumo($node);

    $last = ($node->field_last['und'][0]['value'] == 1)? t('Last match'): t('');
    $mod = (!isset($node->field_mod['und'][0]['value']) || $node->field_mod['und'][0]['value'] == '')? '' : '<h3>'.t('Mod:').'</h3>'.t($node->field_mod['und'][0]['value']);
    $html = '<div class="node-match-planned-info">
                <div class="plain-text"><h3>'.t('PRO:').'</h3>'.l($user->name, 'user/'.$user->uid, array('attributes' => array('class' => array('pro-user')))).'</div>
                <div class="plain-text">'.$mod.'</div>
                <div class="plain-text"><h3>'.t('Timing:').'</h3>'.match_get_timing($node->field_started['und'][0]['value']).'</div>
                <div class="plain-text"><h3>'.$last.'</h3></div>
            </div>';

	$html .= match_get_planned_players_html($node->field_profi_command['und'][0]['value'], $node->uid);

    $stream = '<div class="display active-twitch">'.match_get_twitch_html($node->field_vod['und'][0]['value']).'</div>';

	$form = '';
    if($node->uid == $user->uid) {
        $form = drupal_get_form('match_node_active_form');
        $form = drupal_render($form);
    }
	$html .= $stream.$form;
	return $html;
}

function match_get_node_finished_markup($node, $user) {
	//krumo($node);
    $html = '<div class="timing">'.t('Продолжительность матча: ').match_get_timing($node->field_started['und'][0]['value']).'</div>';
    $html .= match_get_finished_players_html($node->field_profi_command['und'][0]['value'], $node->field_rival_command['und'][0]['value'], $node->uid);
    if($node->field_result['und'][0]['value'] == 0) {
        $html .= '<div class="winner"><span class="dire">'.t('The Dire').'</span> '.t('wins').'</div><div>';
    } else {
        $html .= '<div class="winner"><span class="radiant">'.t('The Radiant').'</span> '.t('wins').'</div><div>';
    }
    $html .= '';
    $html .= '<div class="vod"><a class="check-vod" href="'.$node->field_vod['und'][0]['value'].'" alt="'.t('Check the VOD').'" title="'.t('Check the VOD').'">'.t('Check the VOD').'</a></div>';
    $form = drupal_get_form('match_node_finished_form');
    $like = drupal_get_form('match_node_like_form');
    $html .= drupal_render($form).drupal_render($like);
	return $html;
}

function match_create_node() {
	return drupal_get_form('match_create_match_form');
}

function match_get_finished_players_html($expert, $rival, $proId) {
	$query = db_select('players', 'p')
		->fields('p');
    $query->addField('h', 'name', 'hero_name');
    $query->leftJoin('heroes','h', 'p.hero=h.id');
    $query->condition('p.team_id', $expert)
        ->condition('p.uid', $proId, '!=');
    $portal = $query->execute()->fetchAll();

    $query = db_select('players', 'p')
        ->fields('p');
    $query->addField('h', 'name', 'hero_name');
    $query->leftJoin('heroes','h', 'p.hero=h.id');
    $query->condition('p.team_id', $rival);
    $other = $query->execute()->fetchAll();

    $query = db_select('players', 'p')
        ->fields('p');
    $query->addField('h', 'name', 'hero_name');
    $query->leftJoin('heroes','h', 'p.hero=h.id');
    $query->condition('p.uid', $proId);
    $pro = $query->execute()->fetchAssoc();

    $title = t('');

    $type = 'ul';
    $attributes = array(
        'class' => 'items-list'
    );

    $items = array();

    $or = db_or();

    for ($i = 1; $i <= 6; $i++) {
        $help = 'item'.$i;
        $or->condition('id', $pro[$help]);
    }

    $itemNames = db_select('artifacts', 'a')
        ->fields('a', array('name'))
        ->condition($or)
        ->condition('id', 0, '!=')
        ->execute()
        ->fetchAll();

    foreach($itemNames as $iName) {

        $items[] = array(
            'data' => theme('image', array(
                'path' => file_create_url('public://').'/artifacts/'.str_replace(" ", "_", strtolower($iName->name)).'.png',
                'alt' => $iName->name,
                'title' => $iName->name,
                'width' => 40,
                'height' => 30
            )),
            'class' => array(t('items-list-item')),
        );
    }

    $image = theme('image', array(
        'path' => file_create_url('public://').'/heroes/'.str_replace(" ", "_", strtolower($pro['hero_name'])).'.png',
        'alt' => $pro['hero_name'],
        'title' => $pro['hero_name'],
        'width' => 90,
        'height' => 60
    ));

    $rows[] = array(
        l($pro['nickname'], 'user/'.$pro['uid'], array('attributes' => array('class' => array('pro-user')))),
        $pro['level'],
        $image,
        $pro['kills'],
        $pro['death'],
        $pro['assists'],
        theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes)),
        $pro['gold'],
        $pro['creeps_killed'],
        $pro['creeps_denied'],
        $pro['gpm'],
        $pro['epm'],
        //''
    );
	$header = array(
		t('The Radiant'),
		t('Level'),
		t('Hero'),
		t('K'),
		t('D'),
		t('A'),
		t('Items'),
		t('Gold'),
		t('Killed'),
		t('Denied'),
		t('Gpm'),
		t('Epm'),
        //t('Rating')
	);
	foreach ($portal as $player) {
        $items = array();
        $or = db_or();
        for ($i = 1; $i <= 6; $i++) {
            $help = 'item'.$i;
            $or->condition('id', $player->$help);
        }
        $itemNames = db_select('artifacts', 'a')
            ->fields('a', array('name'))
            ->condition($or)
            ->condition('id', 0, '!=')
            ->execute()
            ->fetchAll();

        foreach($itemNames as $iName) {

            $items[] = array(
                'data' => theme('image', array(
                    'path' => file_create_url('public://').'/artifacts/'.str_replace(" ", "_", strtolower($iName->name)).'.png',
                    'alt' => $iName->name,
                    'title' => $iName->name,
                    'width' => 40,
                    'height' => 30
                )),
                'class' => array(t('items-list-item')),
            );
        }

        $image = theme('image', array(
            'path' => file_create_url('public://').'/heroes/'.str_replace(" ", "_", strtolower($player->hero_name)).'.png',
            'alt' => $player->hero_name,
            'title' => $player->hero_name,
            'width' => 90,
            'height' => 60
        ));

        $rows[] = array(
            l($player->nickname, 'user/'.$player->uid, array('attributes' => array('class' => array('regular-user')))),
            $player->level,
            $image,
            $player->kills,
            $player->death,
            $player->assists,
            theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes)),
            $player->gold,
            $player->creeps_killed,
            $player->creeps_denied,
            $player->gpm,
            $player->epm,
            //match_get_player_rating_form($player->uid, $player->rating)
        );
	}
    $out = theme('table', array('header' => $header, 'rows' => $rows));

    $rows = array();
    $header = array(
        t('The Dire'),
        t('Level'),
        t('Hero'),
        t('K'),
        t('D'),
        t('A'),
        t('Items'),
        t('Gold'),
        t('Killed'),
        t('Denied'),
        t('Gpm'),
        t('Epm'),
        //t('Rating')
    );

    foreach ($other as $player) {
        $items = array();
        $or = db_or();
        for ($i = 1; $i <= 6; $i++) {
            $help = 'item'.$i;
            $or->condition('id', $player->$help);
        }
        $itemNames = db_select('artifacts', 'a')
            ->fields('a', array('name'))
            ->condition($or)
            ->condition('id', 0, '!=')
            ->execute()
            ->fetchAll();

        foreach($itemNames as $iName) {

            $items[] = array(
                'data' => theme('image', array(
                    'path' => file_create_url('public://').'/artifacts/'.str_replace(" ", "_", strtolower($iName->name)).'.png',
                    'alt' => $iName->name,
                    'title' => $iName->name,
                    'width' => 40,
                    'height' => 30
                )),
                'class' => array(t('items-list-item')),
            );
        }

        $image = theme('image', array(
            'path' => file_create_url('public://').'/heroes/'.str_replace(" ", "_", strtolower($player->hero_name)).'.png',
            'alt' => $player->hero_name,
            'title' => $player->hero_name,
            'width' => 90,
            'height' => 60
        ));

        $rows[] = array(
            '<a href="#" class="regular-user">'.$player->nickname.'</a>',
            $player->level,
            $image,
            $player->kills,
            $player->death,
            $player->assists,
            theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes)),
            $player->gold,
            $player->creeps_killed,
            $player->creeps_denied,
            $player->gpm,
            $player->epm,
            //''
        );
    }
    $out .= '<div class="vs-separator">'.t('VS').'</div>'.theme('table', array('header' => $header, 'rows' => $rows));
	return $out;
}

function match_get_planned_players_html ($team, $proId = false) {
	$query = db_select('players', 'p')
		->fields('p', array('nickname', 'uid'));
		
	$query->condition('p.team_id', $team);
	if($proId)
        $query->condition('p.uid', $proId, '!=');

    $result = $query->execute();
	$i = 0;
	
	$title = t('Accepted players');
	
	$type = 'ul';
	$attributes = array(
		'class' => 'players-list'
	);

	while($row = $result->fetchObject()) {
		$items[] = array(
			'data' => l($row->nickname, 'user/'.$row->uid, array('attributes' => array('class' => array('regular-user')))),
			'class' => array(t('players-list-item')),
		);
	}
    $html = '<div class="accepted-users">';
	if(isset($items))
		$html .= theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes));
	else
		$html .= t('No players accepted this match yet');
    $html .= '</div>';
	return $html;
}
function match_node_active_form($form, &$form_state) {
	$form['restart'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Restart'),
			'#name' => 'restart',
		),
		'#weight' => 0,
	);
    $form['dota_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Match id')
    );
	$form['finish'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Finish match'),
			'#name' => 'finish',
		),
		'#weight' => 1,
	);	
	
	$form['#submit'] = array(
		'0' => 'match_controll_form_submit',
	);
	return $form;	
}
function match_node_planned_form($form, &$form_state) {
    global $user;
	$node = menu_get_object();
	$vod = (isset($node->field_vod['und'][0]['value']))? $node->field_vod['und'][0]['value'] : '';
	$mod = (isset($node->field_mod['und'][0]['value']))? $node->field_mod['und'][0]['value'] : '';

	$form['vod'] = array(
		'#type' => 'textfield',
		'#title' => t('Stream'),
		'#default_value' => $vod,
        '#weight' => 1,
        //'#theme_wrappers' => array(),
        '#prefix' => '<div class="info-updater"><div class="info-wrapper">'

	);
    $form['mod'] = array(
        '#type' => 'textfield',
        '#title' => t('Mod'),
        '#default_value' => $mod,
        '#size' => 3,
        '#weight' => 1
    );
	$form['last'] = array(
		'#type' => 'checkbox',
		'#title' => t('Last match'),
		'#default_value' => $node->field_last['und'][0]['value'],
        '#weight' => 3,
        '#prefix' => '<div class="clear"></div>',
	);
	$form['update'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Update match info'),
			'#name' => 'update',
		),
		'#weight' => 4,
        '#suffix' => '</div></div>'
	);
    $needHero = ($node->field_need_player['und'][0]['value'] == 1)? 'Stop fast recruit':'Start fast recruit';
	$form['needahero'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t($needHero),
			'#name' => 'needahero',
		),
		'#weight' => 5,
        '#prefix' => '<div class="controls-updater"><h2>'.t('Controls').'</h2><div class="controls-wrapper">'
	);
	$form['fiveminutes'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('5 minutes'),
			'#name' => 'fiveminutes',
		),
		'#weight' => 6,
		
	);
	$form['start'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Start match'),
			'#name' => 'start',
		),
		'#weight' => 7,
        '#suffix' => '<div class="clear"></div></div></div>'
		
	);	
	$form['#submit'] = array(
		'0' => 'match_controll_form_submit',
	);
    if($node->uid == $user->uid)
	    return $form;
    else
        return false;
}
 
function match_controll_form_submit($form, &$form_state) {
	global $user;
    $action = $form_state['clicked_button']['#name'];
	$node = menu_get_object();
	//krumo($node);exit;
	switch ($action) {
		case 'update':
			$node->field_last['und'][0]['value'] = $form_state['values']['last'];
			$node->field_vod['und'][0]['value'] = $form_state['values']['vod'];
			$node->field_mod['und'][0]['value'] = $form_state['values']['mod'];
			node_save($node);
			drupal_set_message(t('Updated'));
		break;
		case 'fiveminutes':
            drupal_set_message(t('Sending 5 mins. notifications. Not working at this moment'), 'error');
		break;
		case 'start':
			$node->field_started['und'][0]['value'] = time();
			$node->field_state['und'][0]['value'] = 1;
			//create new planned match if not exists
			if($node->field_next['und'][0]['value'] == 0 && $node->field_last['und'][0]['value'] == 0) {

                /* add PRO to list of players*/
                $steam = db_select('hybridauth_identity', 'h')
                    ->fields('h', array('data'))
                    ->condition('h.uid', $user->uid)
                    ->condition('h.provider', 'Steam')
                    ->execute()
                    ->fetchObject();

                $profile = unserialize($steam->data);

                $steam_id = explode('/', $profile['identifier']);
                $steam_id = end($steam_id);


                $command = $node->field_profi_command['und'][0]['value'];

                db_insert('players')->fields(array('uid' => $user->uid, 'team_id' => $command, 'steam_id' => "$steam_id", 'nickname' => "$user->name"))->execute();
                /* end of add PRO to list of players*/

				$newnode = new stdClass();
				$newnode->type = 'match';
                $newnode->uid = $user->uid;
				node_object_prepare($newnode);

				$newnode->title = 'Match №';
				$newnode->language = LANGUAGE_NONE;
				
				node_save($newnode);

				$newnode->field_date[$node->language][0]['value'] = date('Y-m-d H:i:s', time());
				
				db_insert('teams')->fields(array('match_id' => $newnode->nid))->execute();
				$expertCommand = Database::getConnection()->lastInsertId();
				$newnode->field_profi_command[$newnode->language][0]['value'] = $expertCommand;
				
				db_insert('teams')->fields(array('match_id' => $newnode->nid))->execute();
				$rivalCommand = Database::getConnection()->lastInsertId();
				$newnode->field_rival_command[$newnode->language][0]['value'] = $rivalCommand;
                $newnode->title = $newnode->title.$newnode->nid;

				node_save($newnode);
				
				$node->field_next['und'][0]['value'] = $newnode->nid;
			}
			
			node_save($node);
            drupal_set_message(t('Match started'));
		break;	
		case 'needahero':
			if($node->field_need_player['und'][0]['value'] == 1) {
                $node->field_need_player['und'][0]['value'] = 0;
                drupal_set_message(t('Match unmarked as fast recruit'));
            } else {
                $node->field_need_player['und'][0]['value'] = 1;
                drupal_set_message(t('Match marked as fast recruit'));
            }
            node_save($node);
		break;
		case 'restart':
			$node->field_state['und'][0]['value'] = 0;
			node_save($node);
            drupal_set_message(t('Match marked as planned'));
		break;
		case 'finish':
            $match_id = $form_state['values']['dota_id'];
            if($match_id == '') {
                drupal_set_message(t('Set match id!'), 'error');

            } else {
                $info = helper_get_match_statistics($match_id);
                if($info == null) {
                    drupal_set_message(t('Unable to get match info'), 'error');
                } else {
                    if(isset($info->result->error)) {
                        drupal_set_message(t($info->result->error), 'error');
                    } else {
                        /*krumo($info);
                        krumo($node);exit;*/
                        $result = $info->result;
                        $players = $info->result->players;
                        if($result->radiant_win)
                            $node->field_result['und'][0]['value'] = 1;
                        else
                            $node->field_result['und'][0]['value'] = 0;
                        foreach ($players as $k => $player) {
                            $data = array(
                                'level' => $player->level,
                                'hero' => $player->hero_id,
                                'kills' => $player->kills,
                                'death' => $player->deaths,
                                'assists' => $player->assists,
                                'item1' => $player->item_0,
                                'item2' => $player->item_1,
                                'item3' => $player->item_2,
                                'item4' => $player->item_3,
                                'item5' => $player->item_4,
                                'item6' => $player->item_5,
                                'gold' => $player->gold,
                                'creeps_killed' => $player->last_hits,
                                'creeps_denied' => $player->denies,
                                'gpm' => $player->gold_per_min,
                                'epm' => $player->xp_per_min,
                            );
                            if($k < 5) {
                                db_update('players')
                                    ->fields($data)
                                    ->condition('players.steam_id', $player->account_id)
                                    ->execute();
                            } else {
                                $data['nickname'] = 'Test name '.$k;
                                $data['steam_id'] = $player->account_id;
                                $data['team_id'] = $node->field_rival_command['und'][0]['value'];
                                db_insert('players')->fields($data)->execute();
                            }
                        }
                        //exit;
                        $node->field_state['und'][0]['value'] = 2;

                        $time = time() - $node->field_started['und'][0]['value'];

                        if ($time < 60)
                            $time = date('s', $time).t(' secs;');
                        elseif ($time < 3600)
                            $time = date('i:s', $time);
                        elseif ($time >= 3600)
                            $time = date('H:i:s', $time);

                        $node->field_duration['und'][0]['value'] = $time;
                        node_save($node);

                        drupal_set_message(t('Match finished'));
                    }
                }
            }
		break;
        case 'archive':
            $node->field_archived['und'][0]['value'] = $form_state['values']['archived'];
            node_save($node);
            drupal_set_message(t('Match updated'));
        break;
        case 'leave':
            $node->field_free_place['und'][0]['value']++;
            node_save($node);
            db_delete('players')
                ->condition('team_id', $node->field_profi_command['und'][0]['value'])
                ->condition('uid', $user->uid)
                ->execute();
            drupal_set_message(t('You have left the match'));
        break;
        case 'buy':
            if ($node->field_free_place['und'][0]['value'] < 1)
                drupal_set_message(t('No more free places on this match'), 'error');
            else {
                if($user->uid == 0)
                    drupal_set_message(t('You need to login first'), 'error');
                else {
                    $steam = db_select('hybridauth_identity', 'h')
                                    ->fields('h', array('data'))
                                    ->condition('h.uid', $user->uid)
                                    ->condition('h.provider', 'Steam')
                                    ->execute()
                                    ->fetchObject();

                    if (isset($steam) && $steam) {
                        $profile = unserialize($steam->data);

                        $steam_id = explode('/', $profile['identifier']);
                        $steam_id = end($steam_id);
                        $nickname = explode('/', rtrim($profile['profileURL'], '/'));
                        $nickname = end($nickname);


                        $command = $node->field_profi_command['und'][0]['value'];

                        db_insert('players')->fields(array('uid' => $user->uid, 'team_id' => $command, 'steam_id' => "$steam_id", 'nickname' => "$nickname",))->execute();
                        $node->field_free_place['und'][0]['value']--;
                        node_save($node);
                        drupal_set_message(t('Thanks for register'));
                    } else {
                        drupal_set_message(t('Specify your steam account first'), 'error');
                    }
                }
            }
        break;
        case 'like':
            db_insert('match_votes')->fields(array('uid' => $user->uid, 'match_id' => $node->nid, 'rating' => 1))->execute();
            $query = db_select('match_votes', 'm')
                    ->fields('m', array('rating'))
                    ->condition('m.match_id', $node->nid)
                    ->execute();
            $votes = $query->fetchAll();
            $i = 0;
            $sum = 0;
            foreach ($votes as $vote) {
                $sum += $vote->rating;
                $i++;
            }
            $match_rating = 5*($sum/$i);
            $node->field_rating['und'][0]['value'] = $match_rating;
            node_save($node);
            drupal_set_message(t('Thanks for you voice'));
        break;
        case 'dislike':
            db_insert('match_votes')->fields(array('uid' => $user->uid, 'match_id' => $node->nid, 'rating' => 0))->execute();
            $query = db_select('match_votes', 'm')
                    ->fields('m', array('rating'))
                    ->condition('m.match_id', $node->nid)
                    ->execute();
            $votes = $query->fetchAll();
            $i = 0;
            $sum = 0;
            foreach ($votes as $vote) {
                $sum += $vote->rating;
                $i++;
            }
            $match_rating = 5*($sum/$i);
            $node->field_rating['und'][0]['value'] = $match_rating;
            node_save($node);
            drupal_set_message(t('Thanks for you voice'));
        break;
	}
}

function match_create_match_form($form, &$form_state) {
	$form['date'] = array(
		'#type' => 'date',
		//'#date_format' => 'd/m/Y',
		'#title' => t('Match day')
	);
	
	$form['time'] = array(
		'#type' => 'textfield',
		'#title' => t('Time'),
		'#description' => t('E.g. 15:15'),
		'#size' => 5,
		'#required' => TRUE
	);
	
	$form['last'] = array(
		'#type' => 'checkbox',
		'#title' => t('Last match')
	);
	
	$form['create'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Create match'),
		),
	);
	
	$form['#submit'] = array(
		'0' => 'match_create_match_form_submit',
	);
	$form['actions']['submit']['#validate'][] = 'match_create_match_form_validate';

	return $form;  
}
 
function match_create_match_form_submit($form, &$form_state) {
    global $user;
	$node = new stdClass();
	$node->type = 'match';
    $node->uid = $user->uid;
	node_object_prepare($node);
	
	$date = $form_state['values']['date']['year'].'-'.$form_state['values']['date']['month'].'-'.$form_state['values']['date']['day'].' '.$form_state['values']['time'].':00';
	
	$node->title = 'Match';
	$node->language = LANGUAGE_NONE;
	
	$node->field_date[$node->language][0]['value'] = $date;
	
	$node->field_last[$node->language][0]['value'] = $form_state['values']['last'];
	node_save($node);
	
	db_insert('teams')->fields(array('match_id' => $node->nid))->execute();
	$expertCommand = Database::getConnection()->lastInsertId();
	$node->field_profi_command[$node->language][0]['value'] = $expertCommand;
	
	db_insert('teams')->fields(array('match_id' => $node->nid))->execute();
	$rivalCommand = Database::getConnection()->lastInsertId();
	$node->field_rival_command[$node->language][0]['value'] = $rivalCommand;

    $node->title = $node->title.' №'.$node->nid;
	node_save($node);

	drupal_set_message(t('Match created'));
}
 
function match_create_match_form_validate($form, &$form_state){
	if(!empty($form_state['values']['time'])) {
		if(!preg_match("/(2[0-3]|[01][0-9]):[0-5][0-9]/", $form_state['values']['time']))
			form_error($form, t('Error! Wrong format'));
	}
}

function match_get_player_rating_form($pid, $rating) {
        $args = array(
            'pid' => $pid,
            'rating' => $rating,
        );

	$out = '';
	$form =  drupal_get_form('match_create_player_rating_form', $args);
	$out .= drupal_render($form);
	return $out;    
}

function match_create_player_rating_form($form, &$form_state, $args) {
    //krumo($args);
 	$form['rating'] = array(
		'#type' => 'textfield',
		'#title' => t('Rating'),
                '#required' => TRUE,
                '#default_value' => $args['rating'],
                '#size' => 2,
                '#description' => t('0-5'),
	);
	$form['pid'] = array(
		'#type' => 'hidden',
		'#default_value' => $args['pid'],
	);

	$form['actions'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Set'),
		),
	);
	
	$form['#submit'] = array(
		'0' => 'match_create_player_rating_form_submit',
	);
	$form['actions']['submit']['#validate'][] = 'match_element_validate_number';

	return $form;     
}

function match_create_player_rating_form_submit($form, &$form_state) {
    $pid = $form_state['values']['pid'];
    $rating = $form_state['values']['rating'];
    db_update('players')
        ->fields(array('rating' => $rating,))
        ->condition('players.id', $pid)
        ->execute();
    drupal_set_message(t('Updated'));
}

function match_element_validate_number($form, &$form_state) {
    if(!empty($form_state['values']['rating'])) {
        $value = $form_state['values']['rating'];
        if ($value != '' && !is_numeric($value)) {
            form_error($form, t('Error! Rating must be a number'));
        }
    }
}

function match_node_finished_form($form, &$form_state) {
    global $user;
    $node = menu_get_object();
    $form['archived'] = array(
		'#type' => 'checkbox',
		'#title' => t('Archive'),
		'#default_value' => $node->field_archived['und'][0]['value'],
	);
	$form['archive'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('Update match info'),
			'#name' => 'archive',
		),
		'#weight' => 0,
	);

	$form['#submit'] = array(
		'0' => 'match_controll_form_submit',
	);
    if($node->uid == $user->uid)
	    return $form;
    else
        return false;
}

function match_node_planned_ticket_form($form, &$form_state) {
	global $user;
    $node = menu_get_object();
    $query = db_select('players', 'p')
        ->fields('p', array('uid'))
        ->condition('p.team_id', $node->field_profi_command['und'][0]['value'])
        ->execute();
    $uids = $query->fetchAll();
    $in_match = false;
    foreach ($uids as $puid) {
        if ($puid->uid == $user->uid)
            $in_match = true;
    }

    if ($in_match) {
        $form['leave_match'] = array(
            '#type' => 'actions',
            'submit' => array(
                '#type' => 'submit',
                '#value' => t('Leave match'),
                '#name' => 'leave',
            ),
            '#weight' => 0,
        );
    } else {
        $form['get_ticket'] = array(
            '#type' => 'actions',
            'submit' => array(
                '#type' => 'submit',
                '#value' => t('Buy ticket'),
                '#name' => 'buy',
            ),
            '#weight' => 0,
        );
    }
        
	$form['#submit'] = array(
		'0' => 'match_controll_form_submit',
	);     
    if($node->uid != $user->uid)
        return $form;
    else
        return false;
}

function match_node_like_form($form, &$form_state) {
	global $user;
    $node = menu_get_object();
    $query = db_select('match_votes', 'm')
        ->fields('m', array('uid'))
        ->condition('m.match_id', $node->nid)
        ->condition('m.uid', $user->uid)
        ->execute();
    $res = $query->fetchObject();
    if (!$res) {
        $form['like'] = array(
            '#type' => 'actions',
            'submit' => array(
                '#type' => 'submit',
                '#value' => t('Like'),
                '#name' => 'like',
            ),
            '#weight' => 0,
        );
        $form['dislike'] = array(
            '#type' => 'actions',
            'submit' => array(
                '#type' => 'submit',
                '#value' => t('Dislike'),
                '#name' => 'dislike',
            ),
            '#weight' => 1,
        );
        $form['#submit'] = array(
                '0' => 'match_controll_form_submit',
        );
        return $form;
    }
}