<?php

function _user_update_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'create':
      return user_access('administer users');
      break;
  }
}

function user_update_resources() {
  $resource = array(
    'create' => array(
      'callback' => '_user_update_resource_create',
        'args' => array(
          array(
            'name' => 'user_data',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The user data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_user_update_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
  );
  return $resource;
}

function _user_update_resource_create($user_data){
  $return = array();
  $user_total = count($user_data['user']);
  $update_user_total = 0;
  $option=$user_data['option'];
  
  if(empty($option) || empty($user_data['user']))
    return services_error("option or uid field is requried" ,406,array('form_errors' => "option or uid field is requried."));

  if($option == "block")
      $values = 0;
  if($option == "active")
      $values = 1;  

  foreach ($user_data['user'] as $key => $value) {
    if (is_numeric($key)) {
      $uid = $value['uid'];
      $code = db_query("SELECT COUNT(*) FROM {users} u WHERE  u.uid = :uid", array(':uid' => $uid))->fetchField();
      if ($code) {   
        $total_uid[]=$uid;
        if ($uid != null && ($option=="block" || $option=="active" )) {
          $data_update = db_update('users')
            ->fields(array(
            'status' => $values,
          ))  
          ->condition('uid', $uid, '=')
          ->execute();
          $update_user_total++;
        }
        else if ($uid != null && $option="delete") {
          db_delete('users')
            ->condition('uid', $uid)
            ->execute();
           $update_user_total++;
        }
      }
      else {
        $error_uid[] = $uid;
      }
    }
    else {
      services_error('JSON format is not proper', 400);
    }  
  }
  if ($update_user_total == $user_total){
    return TRUE;
  }
  if (!empty($error_uid)) {
    $not_found_uid = implode(",",$error_uid);
    return services_error("There is no user with id ".$not_found_uid ,406,null);
  }
}  
