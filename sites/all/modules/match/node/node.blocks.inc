<?php

function match_get_node_data() {
	
	global $user;
	$node = menu_get_object();
	
	$type = $node->field_state['und'][0]['value']; //0 - план, 1 - актив, 2 - заверш
	$html = '';
	switch ($type) {
		case 0:
			$html = match_get_node_planned_markup($node, $user);
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

function match_get_node_planned_markup($node, $user) {
	//krumo($node);
	$html = match_get_planned_players_html($node->field_profi_command['und'][0]['value']);
	$form = drupal_get_form('match_node_planned_form');

        if($node->field_free_place['und'][0]['value'] > 0) {
            $ticketform = drupal_get_form('match_node_planned_ticket_form');            
            $html .= drupal_render($ticketform);
        }
	$html .= drupal_render($form);
        module_load_include('module', 'match', '../match');
        $html .= '<div class="active-time">'.match_get_planned_timing(strtotime($node->field_date['und'][0]['value']), $node->uid).'</div>';
	return $html;
}
 
function match_get_node_active_markup($node, $user) {
	//krumo($node);
	$html = match_get_planned_players_html($node->field_profi_command['und'][0]['value']);
	$time = '<div class="timing">'.t('Продолжительность матча: ').match_get_timing($node->field_started['und'][0]['value']).'</div>';
	$form = drupal_get_form('match_node_active_form');
	$html .= $time.drupal_render($form);
	return $html;
}

function match_get_node_finished_markup($node, $user) {
	//krumo($node);
	$html = match_get_finished_players_html($node->field_profi_command['und'][0]['value'], $node->field_rival_command['und'][0]['value']);
	$time = '<div class="timing">'.t('Продолжительность матча: ').match_get_timing($node->field_started['und'][0]['value']).'</div>';
	$form = drupal_get_form('match_node_finished_form');
        $like = drupal_get_form('match_node_like_form');
	$html .= $time.drupal_render($form).drupal_render($like);
        $html .= '<div class="vod">'.t('VOD: ').$node->field_vod['und'][0]['value'].'</div>';
	return $html;
}

function match_create_node() {
	return drupal_get_form('match_create_match_form');
}

function match_get_finished_players_html($expert, $rival) {
	$or = db_or()
		->condition('team_id', $expert)
		->condition('team_id', $rival);
	$players = db_select('players', 'p')
		->fields('p')
		->condition($or)
		->execute()
		->fetchAll();
	$header = array(
		t('Player'),
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
        t('Rating')
	);
	$rows = array();
	foreach ($players as $k =>$player) {
		if ($k < 5) {
			$title = t('');
			
			$type = 'ul';
			$attributes = array(
				'class' => 'items-list'
			);

			$items = array();
			for ($i = 1; $i <= 6; $i++) {
				$help = 'item'.$i;
				$items[] = array(
					'data' => $player->$help,
					'class' => array(t('items-list-item')),
				);
			}

					
			$rows[] = array(
				'<a href="'.url('user/'.$player->uid, array('absolute' => true)).'">'.$player->nickname.'</a>',
				$player->level,
				$player->hero,
				$player->kills,
				$player->death,
				$player->assists,
				theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes)),
				$player->gold,
				$player->creeps_killed,
				$player->creeps_denied,
				$player->gpm,
				$player->epm,
                match_get_player_rating_form($player->id, $player->rating)
			);
		} else {
			$rows[] = array(
				$player->nickname,
				$player->level,
				$player->hero,
				$player->kills,
				$player->death,
				$player->assists,
				theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes)),
				$player->gold,
				$player->creeps_killed,
				$player->creeps_denied,
				$player->gpm,
				$player->epm,
                ''
			);		
		}
		
		$k++;
	}
	
	return theme('table', array('header' => $header, 'rows' => $rows));
}

function match_get_planned_players_html ($team) {
	$query = db_select('players', 'p')
		->fields('p', array('nickname', 'uid'));
		
	$query->condition('p.team_id', $team);
	$result = $query->execute();
	$i = 0;
	
	$title = t('Portal command');
	
	$type = 'ul';
	$attributes = array(
		'class' => 'players-list'
	);
	$itemsExpert = array();
	$itemsRival = array();

	while($row = $result->fetchObject()) {
		$items[] = array(
			'data' => '<a href="'.url('user/'.$row->uid, array('absolute' => true)).'">'.$row->nickname.'</a>',
			'class' => array(t('players-list-item')),
		);
	}
	if(isset($items))
		$html = theme('item_list', array('items' => $items, 'title' => $title, 'type' => $type, 'attributes' => $attributes));
	else
		$html = t('No players accepted this match yet');
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
	$form['needahero'] = array(
		'#type' => 'actions',
		'submit' => array(
			'#type' => 'submit',
			'#value' => t('We need a hero!'),
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
			dsm(t('Sending 5 mins. notifications. Not working at this moment'));
		break;
		case 'start':
			$node->field_started['und'][0]['value'] = time();
			$node->field_state['und'][0]['value'] = 1;
			//create new planned match if not exists
			if($node->field_next['und'][0]['value'] == 0 && $node->field_last['und'][0]['value'] == 0) {
				$newnode = new stdClass();
				$newnode->type = 'match';
				node_object_prepare($newnode);
				
				
				
				$newnode->title    = 'Match after ' . $node->nid;
				$newnode->language = LANGUAGE_NONE;
				
				node_save($newnode);
				$newnode->field_date[$node->language][0]['value'] = date('Y-m-d H:i:s', time());
				
				db_insert('teams')->fields(array('match_id' => $newnode->nid))->execute();
				$expertCommand = Database::getConnection()->lastInsertId();
				$newnode->field_profi_command[$newnode->language][0]['value'] = $expertCommand;
				
				db_insert('teams')->fields(array('match_id' => $newnode->nid))->execute();
				$rivalCommand = Database::getConnection()->lastInsertId();
				$newnode->field_rival_command[$newnode->language][0]['value'] = $rivalCommand;
				node_save($newnode);
				
				$node->field_next['und'][0]['value'] = $newnode->nid;
			}
			
			node_save($node);
            drupal_set_message(t('Match started'));
		break;	
		case 'needahero':
			$node->field_need_player['und'][0]['value'] = 1;
			node_save($node);
            drupal_set_message(t('Match marked as fast recruit'));
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
                    krumo($info);exit;
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
	$node = new stdClass();
	$node->type = 'match';
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
	return $form;
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