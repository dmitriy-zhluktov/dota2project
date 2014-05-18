<?php

/**
 * Add body classes if certain regions have content.
 */
function pwp_preprocess_html(&$variables) {
  if (!empty($variables['page']['control_panel'])) {
    $variables['classes_array'][] = 'control-panel';
  }    
  if (!empty($variables['page']['featured'])) {
    $variables['classes_array'][] = 'featured';
  }

  if (!empty($variables['page']['triptych_first'])
    || !empty($variables['page']['triptych_middle'])
    || !empty($variables['page']['triptych_last'])) {
    $variables['classes_array'][] = 'triptych';
  }

  if (!empty($variables['page']['footer_firstcolumn'])
    || !empty($variables['page']['footer_secondcolumn'])
    || !empty($variables['page']['footer_thirdcolumn'])
    || !empty($variables['page']['footer_fourthcolumn'])) {
    $variables['classes_array'][] = 'footer-columns';
  }

  // Add conditional stylesheets for IE
  //drupal_add_css(path_to_theme() . '/css/ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'lte IE 7', '!IE' => FALSE), 'preprocess' => FALSE));
  //drupal_add_css(path_to_theme() . '/css/ie6.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'IE 6', '!IE' => FALSE), 'preprocess' => FALSE));
  
}

/**
 * Override or insert variables into the page template for HTML output.
 */
function pwp_process_html(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_html_alter($variables);
  }
}

/**
 * Override or insert variables into the page template.
 */
function pwp_process_page(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_page_alter($variables);
  }
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
  // Since the title and the shortcut link are both block level elements,
  // positioning them next to each other is much simpler with a wrapper div.
  if (!empty($variables['title_suffix']['add_or_remove_shortcut']) && $variables['title']) {
    // Add a wrapper div using the title_prefix and title_suffix render elements.
    $variables['title_prefix']['shortcut_wrapper'] = array(
      '#markup' => '<div class="shortcut-wrapper clearfix">',
      '#weight' => 100,
    );
    $variables['title_suffix']['shortcut_wrapper'] = array(
      '#markup' => '</div>',
      '#weight' => -99,
    );
    // Make sure the shortcut link is the first item in title_suffix.
    $variables['title_suffix']['add_or_remove_shortcut']['#weight'] = -100;
  }
    /* unset "No content message" */
    unset($variables['page']['content']['system_main']['default_message']);
}

/**
 * Implements hook_preprocess_maintenance_page().
 */
function pwp_preprocess_maintenance_page(&$variables) {
  // By default, site_name is set to Drupal if no db connection is available
  // or during site installation. Setting site_name to an empty string makes
  // the site and update pages look cleaner.
  // @see template_preprocess_maintenance_page
  if (!$variables['db_is_active']) {
    $variables['site_name'] = '';
  }
  drupal_add_css(drupal_get_path('theme', 'pwp') . '/css/maintenance-page.css');
}

/**
 * Override or insert variables into the maintenance page template.
 */
function pwp_process_maintenance_page(&$variables) {
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
}

/**
 * Override or insert variables into the node template.
 */
function pwp_preprocess_node(&$variables) {
  if ($variables['view_mode'] == 'full' && node_is_page($variables['node'])) {
    $variables['classes_array'][] = 'node-full';
  }
}

/**
 * Override or insert variables into the block template.
 */
function pwp_preprocess_block(&$variables) {
  // In the header region visually hide block titles.
  if ($variables['block']->region == 'header') {
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  }
}

/**
 * Implements theme_menu_tree().
 */
function pwp_menu_tree($variables) {
  return '<ul class="menu clearfix">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_field__field_type().
 */
function pwp_field__taxonomy_term_reference($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<h3 class="field-label">' . $variables['label'] . ': </h3>';
  }

  // Render the items.
  $output .= ($variables['element']['#label_display'] == 'inline') ? '<ul class="links inline">' : '<ul class="links">';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<li class="taxonomy-term-reference-' . $delta . '"' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</li>';
  }
  $output .= '</ul>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . (!in_array('clearfix', $variables['classes_array']) ? ' clearfix' : '') . '"' . $variables['attributes'] .'>' . $output . '</div>';

  return $output;
}

function pwp_preprocess_comment(&$vars) {
    $vars['created'] = format_interval(time() - $vars['elements']['#comment']->created);
    $vars['changed'] = format_interval(time() - $vars['elements']['#comment']->changed);
}

function pwp_username(&$vars) {
    $uid = $vars['uid'];
    $user = user_load($uid);
    $roles = $user->roles;
    if(in_array('pro', $roles)) {
        $vars['link_options']['attributes']['class'][] = 'pro-user';
    } else {
        $vars['link_options']['attributes']['class'][] = 'regular-user';
    }

    if (isset($vars['link_path'])) {
        $output = l($vars['name'] . $vars['extra'], $vars['link_path'], $vars['link_options']);
    } else {
        $output = '<span' . drupal_attributes($vars['attributes_array']) . '>' . $vars['name'] . $vars['extra'] . '</span>';
    }

    return $output;
}

function pwp_preprocess_link(&$vars) {

    if($vars['path'] == 'messages' && $vars['text'] == privatemsg_title_callback()) {
        //krumo($vars);
        $vars['options']['html'] = TRUE;
        $vars['text'] = helper_privatemessage_menu_title();
    }
    if($vars['path'] == 'uniteller/buy') {
        $vars['options']['html'] = TRUE;
        $vars['text'] = uniteller_get_money();
    }
}

function pwp_form_comment_form_alter(&$form, &$form_state) {
    /* Remove the "your name" elements for authenticated users */
    if ($form['is_anonymous']['#value'] == false) {
        $form['author']['#access'] = FALSE;
    }
}

function pwp_menu_local_task(&$vars) {

    $link = $vars['element']['#link'];
    $link_text = $link['title'];
    $class = 'tab';
    if($link['path'] == 'messages/new')
        $class .= ' tab-new-message';

    if (!empty($vars['element']['#active'])) {
        // Add text to indicate active tab for non-visual users.
        $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';

        // If the link does not contain HTML already, check_plain() it now.
        // After we set 'html'=TRUE the link will not be sanitized by l().
        if (empty($link['localized_options']['html'])) {
            $link['title'] = check_plain($link['title']);
        }
        $link['localized_options']['html'] = TRUE;
        $link_text = t('!local-task-title!active', array('!local-task-title' => $link['title'], '!active' => $active));
    }

    return '<li' . (!empty($vars['element']['#active']) ? ' class="active '.$class.'"' : ' class="'.$class.'"') . '>' . l($link_text, $link['href'], $link['localized_options']) . "</li>\n";
}

function pwp_preprocess_privatemsg_view(&$vars) {
    //krumo($vars);
    $actions = $vars['message_actions'];
    $dom = new DOMDocument();
    $dom->loadHTML($actions);
    $list = $dom->getElementsByTagName('ul');
    foreach($list->item(0)->getElementsByTagName('a') as $a) {
        $segments = explode('/', $a->getAttribute('href'));

        if(in_array('delete', $segments)) {
            $vars['message_action_delete'] = '<a href="'.$a->getAttribute('href').'" title="'.t('Delete message').'"><img src="'.base_path().'sites/all/themes/pwp/images/msg-delete.png'.'" title="'.t('Delete message').'" alt="'.t('Delete message').'" width="20" height="20" /></a>';
        }
        if(in_array('block', $segments)) {
            $vars['message_action_block'] = '<a href="'.$a->getAttribute('href').'" title="'.t('Block user').'">'.t('Block user').'</a>';
        }
    }
    $timestamp = $vars['message']->timestamp;
    $vars['message_date'] = date('d.m.Y, H:i A', $timestamp);
}