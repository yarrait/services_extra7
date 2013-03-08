<?php

function _user_filter_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'create':
      return user_access('administer users');
      break;
   }
}

function user_filter_resources() {
  $resource = array(
    'create' => array(
      'callback' => '_user_filter_resource_create',
        'args' => array(
          array(
            'name' => 'user_data',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The user data to filter',
            'type' => 'array',
          ),
        ),
        'access callback' => '_user_filter_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
  );
  return $resource;
}

function _user_filter_resource_create($user_data){
  $return = array();
  $option = $user_data['user']['option'];
  $filter = $user_data['user']['filter'];
  $page = $user_data['user']['page'];
 
  if(empty($option) || empty($filter)) {
    return services_error("option or filter field is requried" ,406,array('form_errors' => "option or uid field is requried."));
  }
  if ($option == "role") {
    $no_records = db_query("Select count(*) from users_roles r inner join users u on r.uid = u.uid where r.rid = :rid", array(':rid' => $filter))->fetchField();
    $query = db_select('users_roles', 'ur');
    $query -> innerJoin('users', 'u', '(ur.uid = u.uid)');
    $query -> fields('ur',array('uid'))
           -> condition('ur.rid',$filter,'=')
           -> orderBy('ur.uid','DESC');
    $result = $query->execute();
    while ($record = $result->fetchAssoc()) {
      $uid_array[] = $record['uid'];
    }
    foreach ($uid_array as $key => $uid) {
      $user_details[] = user_load($uid);
    }
  }    
  else if ($option == "permission") {
    $query = db_select('role_permission', 'p');
    $query -> fields('p',array('rid'))
           -> condition('p.permission',$filter,'=')
           -> orderBy('p.rid','DESC');
    $result = $query->execute();
    while ($record = $result->fetchAssoc()) {
      $rid[] = $record['rid'];
    }
    for ($i = 0; $i<count($rid); $i++) {
      if ($rid[$i] == 2) {
        $query = db_select('users', 'u');
        $query -> fields('u',array('uid'))
               -> condition('u.uid',0,'!=');
        $query -> orderBy('uid','DESC');
        $result = $query->execute();
        while ($record = $result->fetchAssoc()) {
          $uid_array[]=$record['uid'];
        }
        foreach ($uid_array as $key => $uid) {
          $user_details[]=user_load($uid);
        } 
        $page_size = 20;   
        $start_index = 0;
        $page_no = 0;
        $last_index = 10;
        for ($m = 0; $m<count($user_details); $m= $m+10) {
          $page_user[$page_no] = array_slice($user_details,$start_index,$last_index);
          $start_index = $start_index + 10;
  	  $last_index = $last_index + 10;
  	  $page_no++;
        }
        $user_filter_data['total'] = count($user_details);
        $user_filter_data['users'] = $page_user[$page];
        return $user_filter_data;
      }
    }
    for ($i=0;$i<count($rid);$i++) {
      $query = db_select('users_roles', 'u');
      $query -> fields('u',array('uid'))
             -> condition('u.rid',$rid[$i],'=');
      $query -> orderBy('uid','DESC');
      $result = $query->execute();
      while ($record = $result->fetchAssoc()) {
        $user_detail[] = $record['uid'];
      }
      foreach($user_detail as $key => $val) {
        $user_details[] = user_load($val);
      }
      $merge_array = $user_details;
      unset($user_details);
   }
   $size_array = count($merge_array);
   for ($i = 0; $i<$size_array; $i++) {
     for ($j = 0; $j<$size_array; $j++) {
       if ($i == $j) {
         continue;
       }
       else if ($merge_array[$i]->uid == $merge_array[$j]->uid) {
         unset ($merge_array[$i]);
       }
     }
   }
   foreach ($merge_array as $key =>$val) {
     $all_user[] = $val;
   }
   $page_size = 20;
   $start_index = 0;
   $page_no = 0;
   $last_index = 10;
   for ($i = 0; $i<count($all_user); $i= $i+$page_size) {
     $page_user[$page_no] = array_slice($all_user,$start_index,$last_index);
     $start_index = $start_index + $page_size;
     $last_index = $last_index + $page_size;
     $page_no++;
   }
   $user_filter_data['total'] = count($all_user);
   $user_filter_data['users'] = $page_user[$page];
   return $user_filter_data;
   }
   else if ($option == "status") {
     if ($filter == "active"){
       $no_records = db_query("SELECT COUNT(*) FROM {users} u WHERE  u.status = :status && u.uid != :uid", array(':status' => 1,':uid' => 0))->fetchField();
       $query = db_select('users', 'u');
       $query -> fields('u',array('uid'))
              -> condition('u.uid',0,'!=')
              -> condition('u.status',1,'=')
              -> orderBy('u.uid','DESC');
       $result = $query->execute();
       while ($record = $result->fetchAssoc()) {
         $user_detail[] = $record['uid'];
       }
       foreach ($user_detail as $key => $val) {
         $user_details[] = user_load($val);
       }
     }
     else if ($filter == "blocked") {
       $no_records = db_query("SELECT COUNT(*) FROM {users} u WHERE  u.status = :status && u.uid != :uid", array(':status' => 0,':uid' => 0))->fetchField();
       $query = db_select('users', 'u');
       $query -> fields('u',array('uid'))
              -> condition('u.uid',0,'!=')
              -> condition('u.status',0,'=')
              -> orderBy('u.uid','DESC');
       $result = $query->execute();
       while ($record = $result->fetchAssoc()) {
               $user_detail[]=$record['uid'];
       }
       foreach ($user_detail as $key => $val) {
        $user_details[] = user_load($val);
       }
     }
  }
  if (empty($user_details)) {
   return services_error("No users available.", 404);
  }
  $page_size = 20;
  $start_index = 0;
  $page_no = 0;
  $last_index = 10;
  for ($i = 0; $i<count($user_details); $i= $i+$page_size) {
    $page_user[$page_no] = array_slice($user_details,$start_index,$last_index);
    $start_index = $start_index + $page_size;
    $last_index = $last_index + $page_size;
    $page_no++;
  }
  $user_filter_data['total'] =$no_records;
  $user_filter_data['users'] = $page_user[$page];
  return $user_filter_data;
}
