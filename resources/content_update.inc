<?php

function _content_update_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'create':
      return user_access('administer nodes');
      break;
  }
}

function content_update_resources() {
  $resource = array(
    'create' => array(
      'callback' => '_content_update_resource_create',
        'args' => array(
          array(
            'name' => 'content_data',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The content data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_content_update_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
  );
  return $resource;
}

function _content_update_resource_create($content_data){
  $option = $content_data['option'];
  if(empty($option) || empty($content_data['node']))
   return services_error("option or nid field is requried" ,406,array('form_errors' => "option or nid field is requried."));

  if($option == "publish" || $option == "promoted" || $option == "sticky")
      $values = 1;
  if($option == "unpublish" || $option == "unpromoted" || $option == "unsticky")
      $values = 0;
  
  $count_node = count($content_data['node']);
  $count_update_node = 0;
  foreach ($content_data['node'] as $key => $value) { 
    if (is_numeric($key)) {
      $nid = $value['nid'];
      $no_records = db_query("Select count(*) from node WHERE nid=:nid",array(':nid' => $nid))->fetchField();
      if ($no_records) {
        $total_nid[] = $nid;
        if($nid != null && ($option == "publish" || $option == "unpublish" )) { 
          db_update('node')->fields(array(
          'status' => $values,
          ))  
          ->condition('nid', $nid, '=')
          ->execute();
   	  $count_update_node++;
        }
        else if ($nid != null && ($option == "promoted" || $option == "unpromoted" )) {
          db_update('node')->fields(array(
          'promote' => $values,
          ))  
          ->condition('nid', $nid, '=')
          ->execute();
	  $count_update_node++;
        }
        else if ($nid != null && ($option == "sticky" || $option == "unsticky" )) { 
          db_update('node')->fields(array(
          'sticky' => $values,
          ))  
          ->condition('nid', $nid, '=')
          ->execute();
          $count_update_node++;
        }
        else if ($nid != null && $option == "delete") { 
          db_delete('node')
          ->condition('nid', $nid)
          ->execute();
          $count_update_node++;
        }
        else {
          return services_error("There is no option of this type ".$option, 406, null);   
        }
      }
      else {
        $error_nid[] = $nid;
      } 
    }
    else {
      services_error('JSON format is not proper',400);
    }
  }
  if ($count_update_node == $count_node) {
    return TRUE;
  }
  if (!empty($error_nid)) {
    $not_found_nid = implode(",",$error_nid);
    return services_error("There is no node with id ".$not_found_nid , 406, null);
  }
}
