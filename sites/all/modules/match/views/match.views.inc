<?php
/**
 * Implements hook_views_data()
 */
 function match_views_data() {

 
	/**
	 * Данные по командам.
	 */
	//Название для удобной навигации
	$data['teams']['table']['group'] = t('Команды');
	
	/*$data['teams']['table']['base'] = array(
		'field' => 'id',
		'title' => t('Команды'),
		'help' => t("Таблица команд. Связана с таблицей матчей."),
		'weight' => -10,
	);*/
	// Связть с таблицей нод.
	$data['teams']['table']['join'] = array(
		//left_field - поле из таблицы нод
		'node' => array(
		  'left_field' => 'nid',
		  'field' => 'match_id',
		),
	);
	
	// Обработка поля
	$data['teams']['match_id'] = array(
		'title' => t('ID матча'),
		'help' => t('Связь с матчем.'),
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_node_nid',
			'name field' => 'title',
			'numeric' => TRUE,
			'validate type' => 'nid'
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
		//связь, для выборки нужных данных, используя relationships
		'relationship' => array(
			'label' => t('ID матча'),
			'base' => 'node',
			'base field' => 'nid',
			//'handler' => 'views_handler_relationship',
		),
	);
	$data['teams']['id'] = array(
		'title' => t('ID команды'),
		'help' => t('ID команды'),
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_node_nid',
			'name field' => 'title',
			'numeric' => TRUE
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),	
	);
	/*$data['teams']['winner'] = array(
		'title' => t('Победитель'),
		'help' => t('1 - победили, 0 - проиграли'),
		'filter' => array(
			'handler' => 'views_handler_filter_numeric',
		),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),		
	);*/
		
	return $data;
}
  