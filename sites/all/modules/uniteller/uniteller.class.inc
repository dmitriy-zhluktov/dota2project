<?php

class uniteller {
    /**
    * Используется для сравнения при определении наша/не наша платёжная система.
    *
    * @var int
    */
    const PAYMENT_CODE = 'uniteller';

    /**
    * 64 - длина логина из технического регламента.
    *
    * @var int
    */
    const LOGIN_MAX = 64;
    /**
    * 80 - длина пароля из технического регламента.
    *
    * @var int
    */
    const PASSWORD_MAX = 80;
    /**
    * Значение по умолчанию - количество дней в течении которых статус платежа 'paid' может превратиться в 'canceled'.
    *
    * @var int
    */
    const DEF_TIME_PAID_CHANGE = 14;
    /**
    * Значение по умолчанию - количество дней в течении которых статус заказа будет синхронизироваться.
    *
    * @var int
    */
    const DEF_TIME_ORDER_SYNC = 30;
    /**
    * Код ответа: АВТОРИЗАЦИЯ УСПЕШНО ЗАВЕРШЕНА.
    *
    * @var string
    */
    const RESPONSE_CODE_SUCCES = 'AS000';

    /**
    * Если платёж стал 'paid' раньше чем $date_fix_paid_change, то платёж уже не может стать 'canceled'.
    *
    * @var int
    */
    public static $date_fix_paid_change = 0;

    /**
    * Если последнее изменение статуса был больше чем $date_fix_order_sync дней назад, то заказ не будет синхронизироваться.
    *
    * @var int
    */
    public static $date_fix_order_sync = 0;

    /**
    * URL для отправки пакета.
    *
    * @var string
    */
    public static $url_uniteller_pay = '';

    /**
    * URL для получение результатов авторизации.
    *
    * @var string
    */
    public static $url_uniteller_results = '';

    /**
    * URL для отмены платежа.
    *
    * @var string
    */
    public static $url_uniteller_unblock = '';

    /**
    * Идентификатор точки продажи в системе Uniteller.
    *
    * @var string
    */
    public static $Shop_ID = '';

    /**
    * Логин из раздела "Параметры Авторизации" Личного кабинета системы Uniteller.
    *
    * @var string
    */
    public static $Login = '';

    /**
    * Пароль из раздела "Параметры Авторизации" Личного кабинета системы Uniteller.
    *
    * @var string
    */
    public static $Password = '';

    /**
    * Устанавливает значения свойств:
    * - $url_uniteller_pay;
    * - $url_uniteller_results;
    * - $url_uniteller_unblock;
    * - $Shop_ID;
    * - $Login;
    * - $Password;
    * - $date_fix_paid_change;
    * - $date_fix_order_sync;
    *
    * @param void
    * @return void
    */
    public static function setMerchantData() {
        if (self::$Shop_ID == '') {
            if (variable_get('uniteller_test_mode')) {
                self::$url_uniteller_pay = 'https://test.wpay.uniteller.ru/pay/';
                self::$url_uniteller_results = 'https://test.wpay.uniteller.ru/results/';
                self::$url_uniteller_unblock = 'https://test.wpay.uniteller.ru/unblock/';
            } else {
                self::$url_uniteller_pay = 'https://wpay.uniteller.ru/pay/';
                self::$url_uniteller_results = 'https://wpay.uniteller.ru/results/';
                self::$url_uniteller_unblock = 'https://wpay.uniteller.ru/unblock/';
            }

            self::$Shop_ID = trim(variable_get('uniteller_shop_id', ''));
            self::$Login = substr(trim(variable_get('uniteller_shop_login', '')), 0, self::LOGIN_MAX);
            self::$Password = substr(trim(variable_get('uniteller_shop_password', '')), 0, self::PASSWORD_MAX);

            $time_paid_change = (int)variable_get('uc_uniteller_time_paid_change', '');
            if ($time_paid_change <= 0) {
                $time_paid_change = self::DEF_TIME_PAID_CHANGE;
            }
            self::$date_fix_paid_change = time() - (int)($time_paid_change * 24 * 60 * 60);

            $time_order_sync = (int)variable_get('uc_uniteller_time_order_sync', '');
            if ($time_order_sync <= 0) {
                $time_order_sync = self::DEF_TIME_ORDER_SYNC;
            }
            self::$date_fix_order_sync = time() - (int)($time_order_sync * 24 * 60 * 60);
        }
    }

    /**
    * По ID заказа возвращает код статуса заказа.
    *
    * @param int $order_real_id
    * @return string
    */
    public static function getStatusCode($order_real_id) {
        // Получаем статус и состояние заказа.
        $order = uniteller_order_load($order_real_id);
        $status = $order->status;

        // Распределяет по группам.
        if ($status == 'in_checkout' || $status == 'pending') {
        // Состояния заказа 'Оформление' или 'Принят' эквивалентен отсутствию платежа.
            return 'O';
        } elseif ($status == 'processing') {
        // Состояние заказа 'Обработка' эквивалентен статусу платежа 'athorized'.
            return 'A';
        } elseif ($status == 'completed' || $status == 'payment_received') {
        // Состояния заказа 'Завершен' или 'Платёж получен' эквивалентны статусу платежа 'paid'.
            return 'P';
        } elseif ($status == 'canceled') {
        // Состояние заказа 'Отменен' эквивалентны статусу платежа 'canceled'.
            return 'C';
        } elseif (false) {
        // Состояние заказа запрещено изменять.
            return 'Z';
        } else {
            return 'W';
        }

    }

    /**
    * Изменяет статус заказа.
    *
    * @param int $order_real_id
    * @param string $status
    * @return boolean
    */
    public static function setStatusCode($order_real_id, $status) {
        if (strtolower($status) == 'empty') {
            // Отсутствие платежа эквивалентно статусу заказа 'pending'. Изменяет статус заказа'.
            uniteller_order_update_status($order_real_id, 'pending');
        } elseif (strtolower($status) == 'authorized') {
            // Статус платежа 'authorized' эквивалентен статусу заказа 'processing' (добавляется при инсталляции модуля). Изменяет статус заказа.
            uniteller_order_update_status($order_real_id, 'processing');
        } elseif (strtolower($status) == 'paid') {
            // Статус платежа 'paid' эквивалентен статусу заказа 'completed'. Изменяет статус заказа.
            uniteller_order_update_status($order_real_id, 'completed');
        } elseif (strtolower($status) == 'canceled') {
            // Статус платежа 'canceled' эквивалентен статусу заказа 'canceled'. Изменяет статус заказа.
            uniteller_order_update_status($order_real_id, 'canceled');
        } else {
            return false;
        }
        return true;
    }

    /**
    * Посылает запрос и возвращает ответ в виде simplexml_object, строки с ошибкой или false.
    * Ошибки записывает в 'Home › Administer › Reports'
    *
    * @param string $url
    * @param array $data
    * @return simplexml_object|string|boolean
    */
    public static function setCurlRequest($url, $data) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, (string)$url);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, (array)$data);
    curl_setopt($ch, CURLOPT_FAILONERROR, true);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);

    $result = curl_exec($ch);
    $errno = curl_errno($ch);
    $error = curl_error($ch);
    curl_close($ch);

    if ($errno == 0) {
    libxml_use_internal_errors(true);
    $xml = simplexml_load_string($result);
    if ($xml !== false) {
    return $xml;
    } else {
    libxml_clear_errors();
    if (!isset($data['ShopOrderNumber'])) {
    $data['ShopOrderNumber'] = '';
    }
    watchdog('Uniteller', uc_uniteller_t('Order № %d Uniteller returned an error: ', (int)$data['ShopOrderNumber']) . $result, array(), WATCHDOG_ERROR, false);

    return $result;
    }
    } else {
    if (!isset($data['ShopOrderNumber'])) {
    $data['ShopOrderNumber'] = '';
    }
    watchdog('Uniteller', uc_uniteller_t('Order № %d Error connecting: ', $data['ShopOrderNumber']) . $error, array(), WATCHDOG_ERROR, false);

    return false;
    }
    }

    /**
    * Посылает запрос на получение результатов авторизации по реальному номеру заказа и возвращает массив данных для чека.
    *
    * @param int $order_real_id
    * @return array
    */
    public static function getCheckData($order_real_id) {
    self::setMerchantData();
    $data = array(
    'ShopOrderNumber' => (int)$order_real_id,
    'Shop_ID' => self::$Shop_ID,
    'Login' => self::$Login,
    'Password' => self::$Password,
    'Format' => '4',
    );

    $return = array(
    'order_id' => (int)$order_real_id,
    'name_merchant' => trim(variable_get('site_name', 'Default site name')),
    'name_lat' => trim(variable_get('uc_uniteller_latin_name', '')),
    'name_url' => trim(variable_get('site_name', 'Default site name')),
    'phone' => '',
    'email' => trim(variable_get('site_mail', ini_get('sendmail_from'))),
    'nameproduct' => array(),
    'total' => '',
    'currency' => '',
    'date' => '',
    'approvalcode' => '',
    'lastname' => '',
    'firstname' => '',
    'middlename' => '',
    'billnumber' => '',
    'paymenttransactiontype_id' => '',
    'response_code' => '',
    'ordernumber' => '',
    'status' => '',

    'error_protocol' => '',
    'error_count' => '',
    'error_message' => '',
    'error_authentication' => '',
    );
    $order = uc_order_load($order_real_id);
    $return['rate'] = sprintf('%01.2f', $order->quote['rate']);
    foreach ($order->products as $product) {
    $return['nameproduct'][] = array(
    'name' => $product->title,
    'price' => sprintf('%01.2f', $product->price),
    'qty' => (int)$product->qty,
    );
    }

    $xml = self::setCurlRequest(self::$url_uniteller_results, $data);
    $array_xml = (array)$xml;
    if ($xml === false) {
    // oшибка при выполнении запроса.
    $return['error_protocol'] = true;
    } elseif (isset($xml->orders->order)) {
    $xml_order = $xml->orders->order;

    $total = (array)$xml_order->total;
    $currency = (array)$xml_order->currency;
    $date = (array)$xml_order->date;
    $approvalcode = (array)$xml_order->approvalcode;
    $lastname = (array)$xml_order->lastname;
    $firstname = (array)$xml_order->firstname;
    $middlename = (array)$xml_order->middlename;
    $billnumber = (array)$xml_order->billnumber;
    $paymenttransactiontype_id = (array)$xml_order->paymenttransactiontype_id;
    $response_code = (array)$xml_order->response_code;
    $ordernumber = (array)$xml_order->ordernumber;
    $status = (array)$xml_order->status;

    $return['total'] = ( ( isset($total[0]) ) ? $total[0] : '' );
    $return['currency'] = ( ( isset($currency[0]) ) ? $currency[0] : '' );
    $return['date'] = ( ( isset($date[0]) ) ? $date[0] : '' );
    $return['approvalcode'] = ( ( isset($approvalcode[0]) ) ? $approvalcode[0] : '' );
    $return['lastname'] = ( ( isset($lastname[0]) ) ? $lastname[0] : '' );
    $return['firstname'] = ( ( isset($firstname[0]) ) ? $firstname[0] : '' );
    $return['middlename'] = ( ( isset($middlename[0]) ) ? $middlename[0] : '' );
    $return['billnumber'] = ( ( isset($billnumber[0]) ) ? $billnumber[0] : '' );
    $return['paymenttransactiontype_id'] = ( ( isset($paymenttransactiontype_id[0]) ) ? $paymenttransactiontype_id[0] : '' );
    $return['response_code'] = ( ( isset($response_code[0]) ) ? $response_code[0] : '' );
    $return['ordernumber'] = ( ( isset($ordernumber[0]) ) ? $ordernumber[0] : '' );
    $return['status'] = ( ( isset($status[0]) ) ? $status[0] : '' );
    } elseif (isset($array_xml['@attributes']['count']) && (int)$array_xml['@attributes']['count'] == 0) {
    // платежа нет, но вернулся валидный xml с атрибутом count равным 0
    $return['error_count'] = true;
    } else {
    // вернулась строка с ошибкой
    $return['error_message'] = $xml;
    $return['error_authentication'] = true;
    }

    return $return;
    }

    /**
    * Отменяет платёж в системе Uniteller по реальному номеру заказа и возвращает true или false.
    *
    * @param int $order_real_id
    * @return boolean
    */
    public static function setUnitellerCancel($order_real_id) {
    self::setMerchantData();
    $data = array(
    'ShopOrderNumber' => (int)$order_real_id,
    'Shop_ID' => self::$Shop_ID,
    'Login' => self::$Login,
    'Password' => self::$Password,
    'Format' => '4',
    'S_FIELDS' => 'BillNumber;Status',
    );

    $xml = self::setCurlRequest(self::$url_uniteller_results, $data);
    $array_xml = (array)$xml;
    if ($xml === false) {
    // Ничего не делаем с заказом, если ошибка протокола.
    return false;
    } elseif (isset($xml->orders->order->billnumber)) {
    $Billnumber = (array)$xml->orders->order->billnumber;
    if (isset($Billnumber[0])) {
    $Billnumber = (string)$Billnumber[0];
    } else {
    // Отменяем заказ, если: нет такого платежа.
    return true;
    }

    $Status = (array)$xml->orders->order->status;
    if (isset($Status[0])) {
    $Status = (string)$Status[0];
    } else {
    $Status = '';
    }
    if (strtolower($Status) == 'canceled') {
    // Отменяем заказ, если: статус платёжа равен 'canceled'.
    return true;
    }
    } elseif (isset($array_xml['@attributes']['count']) && (int)$array_xml['@attributes']['count'] == 0) {
    // Отменяем заказ, если: платежа нет (вернулся валидный xml с атрибутом count равным 0)
    return true;
    } else {
    // Ничего не делаем с заказом, если вернулась строка с ошибкой
    return false;
    }

    // 12 - это длина Billnumber из технического регламента.
    if (is_string($Billnumber) && strlen($Billnumber) == 12) {
    $data = array(
    'Billnumber' => $Billnumber,
    'Shop_ID' => self::$Shop_ID,
    'Login' => self::$Login,
    'Password' => self::$Password,
    'Format' => '3',
    );

    $response = self::setCurlRequest(self::$url_uniteller_unblock, $data);
    $array_response = (array)$response;
    if ($response === false) {
    // Ничего не делаем с заказом, если ошибка протокола.
    $response_code = false;
    } elseif (isset($response->orders->order->response_code)) {
    $response_code = (array)$response->orders->order->response_code;
    if (isset($response_code[0])) {
    $response_code = (string)$response_code[0];
    } else {
    $response_code = false;
    }
    } elseif (isset($array_response['@attributes']['count']) && (int)$array_response['@attributes']['count'] == 0) {
    // Ничего не делаем с заказом, если платежа нет (вернулся валидный xml с атрибутом count равным 0).
    $response_code = false;
    } else {
    // Ничего не делаем с заказом, если вернулась строка с ошибкой.
    $response_code = false;
    }

    // Проверяет возможные ошибки
    $response = (array)$response;
    if (isset($response['@attributes']['firstcode'])) {
    $response_code = (int)$response['@attributes']['firstcode'];
    }
    // Ошибка: Не найден платёж - значит заказ можно отменять.
    if ($response_code == 4) {
    $response_code = self::RESPONSE_CODE_SUCCES;
    // Ошибка: Транзакция была отменена ранее - значит заказ можно отменять.
    } elseif ($response_code == 16) {
    $response_code = self::RESPONSE_CODE_SUCCES;
    }

    if ($response_code == self::RESPONSE_CODE_SUCCES) {
    // Отменяем заказа
    return true;
    } else {
    // Ничего не делаем с заказом
    return false;
    }
    } else {
    // Если нет $Billnumber, значит нет платёжа, значит заказ можно отменять.
    return true;
    }
    }

    /**
    * Синхронизирует статус платежа со статусом заказа.
    *
    * @param array $arOrder
    * @param array $aCheckData Если этот параметр есть, то после вызова ф-ции в нём будут данные для чека.
    * @return void
    */
    public static function doSyncStatus($order_real_id, &$aCheckData = array()) {
        /*if (empty($aCheckData) || !is_array($aCheckData)) {
            $aCheckData = self::getCheckData($order_real_id);
        }*/

        //$statusCode = self::getStatusCode($order_real_id);

        self::setMerchantData();
        $sPostFields = "Shop_ID=".self::$Shop_ID."&Login=".self::$Login."&Password=".self::$Password."&Format=1&ShopOrderNumber=".$order_real_id."&S_FIELDS=Status;ApprovalCode;BillNumber";

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, (string)self::$url_uniteller_results);
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_VERBOSE, 0);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $sPostFields);
        curl_setopt($ch, CURLOPT_BINARYTRANSFER, 1);
        curl_setopt($ch, CURLINFO_HEADER_OUT, 1);
        $curl_response = curl_exec($ch);
        $curl_error = curl_error($ch);
        /*echo '<pre>';
        print_r($curl_response);
        print_r($curl_error);
        echo '</pre>';*/
        if ($curl_error) {
            // обработка ошибки обращения за статусом платежа
            watchdog('Uniteller', t('Order № @id Uniteller returned an error: @error', array('@id' => (int)$order_real_id, '@error' => $curl_error)), array(), WATCHDOG_ERROR, false);
            return;
        } else {
            // данные получены
            // обработка данных из переменной $curl_response
            $arr = explode( ";", $curl_response );
            if ( count($arr) > 2 ) {
                $status = $arr[0];
            } else {
                watchdog('Uniteller', t('Order № @id Uniteller returned an error: unable to parse data', array('@id' =>(int)$order_real_id)), array(), WATCHDOG_ERROR, false);
                return;
            }
        }
        /*
        // Заказ в состоянии 'до блокировки средств', а платёж существует.
        if ($statusCode === 'O' && (strtolower($status) === 'authorized' || strtolower($status) === 'paid' || strtolower($status) === 'canceled')) {
            self::setStatusCode($order_real_id, $status);
        }
        // Заказ в состоянии 'блокировка средств', а платёж - нет.
        if ($statusCode === 'A' && (strtolower($status) === 'empty' || strtolower($status) === 'paid' || strtolower($status) === 'canceled')) {
            self::setStatusCode($order_real_id, $status);
        }
        // Заказ в состоянии 'средства сняты' и не прошло еще время, отведенное на отмену после оплаты, а платёж - нет.
        if ($statusCode === 'P' && (strtolower($status) === 'empty' || strtolower($status) === 'authorized' || strtolower($status) === 'canceled')) {
            self::setStatusCode($order_real_id, $status);
        }
        // Заказ в состоянии 'средства возвращены', а платёж в состоянии 'средства заблокированны' или 'средства сняты'.
        if ($statusCode === 'C' && ($status === 'authorized' || $status === 'paid')) {
            if (!self::setUnitellerCancel($order_real_id)) {
                // Если отменить платёж не удалось, то меняем статус заказа.
                self::setStatusCode($order_real_id, $status);
            } else {
                // Если платёж отменён, то заново получаем данные для чека.
                $aCheckData = self::getCheckData($order_real_id);
            }
        }
        // Заказ в состоянии 'неизвестно', а платёж существует.
        if ($statusCode === 'W' && (strtolower($status) === 'authorized' || strtolower($status) === 'paid' || strtolower($status) === 'canceled')) {
            self::setStatusCode($order_real_id, $status);
        }
        */
        self::setStatusCode($order_real_id, $status);
    }

    /**
    * Принимает POST запрос от системы Uniteller и устанавливает статус заказа.
    *
    * @param void
    * @return void
    */
    public static function setStatus() {
        if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['Order_ID']) && isset($_POST['Status']) && isset($_POST['Signature'])) {
            $order_id = (int)$_POST['Order_ID'];
            $status = trim($_POST['Status']);
            $signature = trim($_POST['Signature']);

            $order = uniteller_order_load($order_id);

            if ($order->id) {
                self::setMerchantData();
                $sign = strtoupper(md5($order_id . $status . self::$Password));
                if ($sign === $signature) {
                    $status = strtolower($status);
                    $statusCode = self::getStatusCode($order_id);

                    // Заказ в состоянии 'до блокировки средств'.
                    if ($statusCode === 'O') {
                        self::setStatusCode($order_id, $status);
                    }
                    // Заказ в состоянии 'блокировка средств', а платёж - нет.
                    if ($statusCode === 'A' && ($status === 'paid' || $status === 'canceled')) {
                        self::setStatusCode($order_id, $status);
                    }
                    // Заказ в состоянии 'средства сняты', а платёж - нет.
                    if ($statusCode === 'P' && ($status === 'authorized' || $status === 'canceled')) {
                        self::setStatusCode($order_id, $status);
                    }
                    // Заказ в состоянии 'средства возвращены', а платёж в состоянии 'средства заблокированны' или 'средства сняты'.
                    if ($statusCode === 'C' && ($status === 'authorized' || $status === 'paid')) {
                        if (!self::setUnitellerCancel($order_id)) {
                            // Если отменить платёж не удалось, то меняем статус заказа.
                            self::setStatusCode($order_id, $status);
                        }
                    }
                    // Заказ в состоянии 'неизвестно'.
                    if ($statusCode === 'W') {
                        self::setStatusCode($order_id, $status);
                    }
                }
            }
        } else {
            drupal_not_found();
        }
    }
}