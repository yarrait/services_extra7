<?php

function _site_configuration_resource_access($op = 'view', $args = array()) {
  switch ($op) {
     case 'create':
       return user_access('administer users'); 
       break;
     case 'view':
       return user_access('administer users'); 
       break;
     case 'delete':
       return user_access('administer users'); 
       break;
  }
}

function site_configuration_resources() {
  $resource = array(
    'retrieve' => array(
      'callback' => '_site_configuration_resource_retrieve',
	'args' => array(
          array(
            'name' => 'configuration',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'The name of configuration of the site configuration to get',
          ),
        ),
        'access callback' => '_site_configuration_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
    'create' => array(
      'callback' => '_site_configuration_resource_create',
        'args' => array(
          array(
            'name' => 'site_configuration',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The site_configuration data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_site_configuration_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
    'delete' => array(
      'callback' => '_site_configuration_resource_delete',
        'args' => array(
          array(
            'name' => 'type',
            'optional' => FALSE,
            'type' => 'string',
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_site_configuration_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
    ),
  );
  return $resource;
}

function _site_configuration_resource_create($site_data) {
  $option = $site_data['site_update']['option'];
  $value  = $site_data['site_update']['value'];

  switch ($option) {
 
    case "cache_clear":  
      drupal_flush_all_caches();
      return TRUE;
      break;
 
    case "clean_url":
      $clean_url = $value['clean_url'];//send 0 or 1 1 for enable and o disable 
      variable_set('clean_url', $clean_url);
      return TRUE;
      break;

    case "site_maintenance": 
      $site_status = $value['site_offline'];
      variable_set('maintenance_mode', $site_status); 

      if(isset($value['site_offline_message']))
      {
        $site_message = $value['site_offline_message'];
        
        variable_set('maintenance_mode_message', $site_message);
      }

      
      return TRUE;
      break;
    
    case "site_information":
      module_load_include('inc', 'system', 'system.admin');
      $form_state = array();

      $form_state['values']['site_name'] = $value['site_name'];
      $form_state['values']['site_slogan'] = $value['site_slogan'];
      $form_state['values']['site_mail'] = $value['site_mail'];
      $form_state['values']['default_nodes_main'] = $value['default_nodes_main'];
      $form_state['values']['site_frontpage'] = $value['site_frontpage'];
      $form_state['values']['site_403'] = $value['site_403'];
      $form_state['values']['site_404'] = $value['site_404'];
      $form_state['values']['op'] = t('Save configuration');
      $site = drupal_form_submit('system_site_information_settings', $form_state);
      
      if ($errors = form_get_errors()) {
        return  services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
      } 
      return TRUE;
      break;
    
   case "site_performance":
     module_load_include('inc', 'system', 'system.admin');
     $form_state = array();
     variable_set('cache', $value['cache']); //1 for check and 0 for un check 
     variable_set('cache_lifetime', $value['cache_lifetime']);
     variable_set('page_cache_maximum_age', $value['page_cache_maximum_age']);
     variable_set('block_cache', $value['block_cache']);
     variable_set('preprocess_css', $value['preprocess_css']);
     variable_set('preprocess_js', $value['preprocess_js']);
     variable_set('page_compression', $value['page_compression']);
     return TRUE;
     break;

    case "file_system":
      module_load_include('inc', 'system', 'system.admin');
      $form_state = array();
      $form_state['values']['file_public_path'] = $value['file_public_path'];
      $form_state['values']['file_private_path'] = $value['file_private_path'];
      $form_state['values']['file_temporary_path'] = $value['file_temporary_path'];
      $form_state['values']['file_default_scheme'] = $value['file_default_scheme'];
      $form_state['values']['op'] = t('Save configuration');
      drupal_form_submit('system_file_system_settings', $form_state);
      if ($errors = form_get_errors()) {
        return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
      } 
      return TRUE;
      break;
      
    case "database_logging": 
      $error_level = $value['error_level'];//0 1 2 option
      variable_set('error_level', $error_level);
      $dblog_row_limit = $value['db_log_limit'];//0 100 1000 etc
      variable_set('dblog_row_limit', $dblog_row_limit);     
      return TRUE;
      break;


    case "regional_language":
      variable_set('site_default_country', $value['regional_language']['site_default_country']);
      variable_set('date_first_day', $value['regional_language']['date_first_day']);
      variable_set('date_default_timezone', $value['regional_language']['date_default_timezone']);
      variable_set('configurable_timezones', $value['regional_language']['configurable_timezones']);
      variable_set('empty_timezone_message', $value['regional_language']['empty_timezone_message']);
      variable_set('user_default_timezone', $value['regional_language']['user_default_timezone']);
      break;

    case "date_time":
      module_load_include('inc', 'system', 'system.admin');
     
      $form_state = array();
      variable_set('date_format_long',$value['date_format_long']);
      variable_set('date_format_medium',$value['date_format_medium']);
      variable_set('date_format_short',$value['date_format_short']);
      variable_set('date_format_long_1',$value['date_format_long_1']);
      variable_set('date_format_short_1',$value['date_format_short_1']);
   
      if($value['add_date_type']) {
        $machine_name = trim($value['add_date_type']['name']);
        $format_type = array();
        $format_type['title'] = trim($value['add_date_type']['name']);
        $format_type['type'] = $machine_name;
        $format_type['locked'] = 0;
        $format_type['is_new'] = 1;
        system_date_format_type_save($format_type);
        variable_set('date_format_' . $machine_name, $value['add_date_type']['date_format']);
      }
     
       if($value['add_format']) {
         $date_format = $value['add_format']['date_format'];
         $date_format_dfid = db_query("SELECT n.dfid FROM {date_formats} n WHERE  n.format = :format", array(':format' => $date_format))->fetchAssoc();
         $format = array();
         $format['format'] = trim($value['add_format']['date_format']);
         $format['type'] = 'custom';
         $format['locked'] = 0;
         if (!empty($date_format_dfid)) {
           system_date_format_save($format, $date_format_dfid);
         }
         else {
           $format['is_new'] = 1;
           system_date_format_save($format);
         }
       }
       return TRUE;
       break;
  }

}

function _site_configuration_resource_retrieve($configuration) {
//1 enabled 0 disabled  
  switch ($configuration) {
    case "clean_url":
      $result['clean_url'] = variable_get('clean_url',1);
      break;

    case "site_maintenance":
      $value =  variable_get('maintenance_mode', 1); 
      $message = variable_get('maintenance_mode_message', t('@site is currently under maintenance. We should be back shortly. Thank you for your patience.', array('@site' => variable_get('site_name', 'Drupal'))));
      $result['site_offline'] = $value;
      $result['message'] = $message;
      break;    

    case "site_information":
      $result['site_name'] = variable_get('site_name', 'Drupal');
      $result['site_slogan'] = variable_get('site_slogan', '');
      $result['site_mail'] = variable_get('site_mail', ini_get('sendmail_from'));
      $result['default_nodes_main'] = variable_get('default_nodes_main', 10);
      $result['site_frontpage'] = (variable_get('site_frontpage')!='node'?drupal_get_path_alias(variable_get('site_frontpage', 'node')):'');
      $result['site_403'] = variable_get('site_403', '');
      $result['site_404'] =  variable_get('site_404', '');
      break;

    case "site_performance":
      $result['cache'] = variable_get('cache', 0);
      $result['block_cache'] = variable_get('block_cache');
      $result['cache_lifetime'] = variable_get('cache_lifetime', 0);
      $result['page_cache_maximum_age'] = variable_get('page_cache_maximum_age');
      $result['page_compression'] = variable_get('page_compression', TRUE);
      $result['preprocess_css'] = variable_get('preprocess_css', 0);
      $result['preprocess_js'] = variable_get('preprocess_js', 0);
      break;
   
    case "file_system":
      $result['file_public_path'] = variable_get('file_public_path', conf_path() . '/files');
      $result['file_private_path'] = variable_get('file_private_path', '');
      $result['file_temporary_path'] = variable_get('file_temporary_path', file_directory_temp());
      $result['file_default_scheme'] = variable_get('file_default_scheme', isset($options['public']) ? 'public' : key($options));
      break;

    case "database_logging":
      $result['error_level'] = variable_get('error_level', $error_level);
      $result['dblog_row_limit'] = variable_get('dblog_row_limit', $dblog_row_limit);     
      break;
    
    case "regional_language":
      include_once DRUPAL_ROOT . '/includes/locale.inc';
      $countries = country_get_list();
      $zones = system_time_zones();
      $country['site_default_country'] = variable_get('site_default_country', '');
      $country['site_country'] = $countries;
      $result['country'] = $country;
      $result['date_first_day'] = variable_get('date_first_day', 0);
      $time_zone['date_default_timezone'] = variable_get('date_default_timezone', date_default_timezone_get());
      $time_zone['zones'] = $zones;
      $result['timezones'] = $time_zone;
      $result['configurable_timezones'] = variable_get('configurable_timezones', 1);
      $result['empty_timezone_message'] = variable_get('empty_timezone_message', 0);
      $result['user_default_timezone'] = variable_get('user_default_timezone', DRUPAL_USER_TIMEZONE_DEFAULT);return $result;
      break;

    case "date_time":
     $custom_format =  system_get_date_formats('custom');
     $i = 0;
      foreach ($custom_format as $f => $format) {
        $custom_format_list[$i]['zone'] = $f;
        $custom_format_list[$i]['format'] = format_date(REQUEST_TIME, 'custom', $f);
        $i++;
      } 

      $long_format =  system_get_date_formats('long');
      $i = 0;
      foreach ($long_format as $f => $format) {
        $long_format_list[$i]['zone'] = $f;
        $long_format_list[$i]['format'] = format_date(REQUEST_TIME, 'custom', $f);
        $i++;
      }
      reset($long_format);
      $default = variable_get(date_format_long);
      $merge = array_merge($long_format_list,$custom_format_list);
      $long_date_format['default'] = $default; 
      $long_date_format['all_format'] = $merge;
      $merge_format['long_date_format'] = $long_date_format;

      $medium_format =  system_get_date_formats('medium');
      $i = 0;
      foreach ($medium_format as $f => $format) {
        $medium_format_list[$i]['zone'] = $f;
        $medium_format_list[$i]['format'] = format_date(REQUEST_TIME, 'custom', $f);
        $i++;
      }
      $merge_long_medium = array_merge($long_format_list,$medium_format_list);
      $all_format_type = $medium_format_list;
      reset($medium_format);
      $default = variable_get('date_format_medium');
      $merge = array_merge($medium_format_list,$custom_format_list);
      $medium_date_format['default'] = $default; 
      $medium_date_format['all_format'] = $merge;
      $merge_format['medium_date_format'] = $medium_date_format;
     

      $short_format =  system_get_date_formats('medium');
      $i = 0;
      foreach ($short_format as $f => $format) {
        $short_format_list[$i]['zone'] = $f;
        $short_format_list[$i]['format'] = format_date(REQUEST_TIME, 'custom', $f);
        $i++;
      }
      $merge_long_medium_short = array_merge($merge_long_medium,$short_format_list);
      
      reset($short_format);
      $default = variable_get('date_format_short');
      $merge = array_merge($short_format_list,$custom_format_list);
      $short_date_format['default'] = $default; 
      $short_date_format['all_format'] = $merge;
      $merge_format['short_date_format'] = $short_date_format;
      
      $all_format_data = array_merge($merge_long_medium_short,$custom_format_list);
     
      $query = db_query("select type from date_format_type where locked != 1");
      foreach ($query as $record) {
        $show_format_type[] = $record; 
        $show_format_type_default[] = variable_get('date_format_' . $record);
      }
      $merge_format['show_format_type'] = $show_format_type;
      $merge_format['show_format_type'] = $show_format_type_default;
      $merge_format['all_format_data'] = $all_format_data;
      return $merge_format;
   
      break;
 
  }
   return $result;
}


function _site_configuration_resource_delete($data_type){
  if($data_type = "long" || $data_type = "medium" || $data_type = "short") {
    return services_error("your request will not accepted ", 212, null);
  }
 $code = db_query("SELECT COUNT(*) FROM {date_format_type} d WHERE  d.type = :type", array(':type' => $data_type))->fetchField();	
  if ($code) {
    db_delete('date_format_type')
    ->condition('type', $data_type)
    ->execute();
    return TRUE;
  }
  return services_error("There is no data_type with type ".$data_type,406,null);
}
