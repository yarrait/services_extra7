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
  module_load_include('inc', 'node', 'node.admin');
 
  $option = $content_data['option'];
  if(empty($option) || empty($content_data['node']))
   return services_error("option or nid field is requried" , 406, array('form_errors' => "option or nid field is requried."));

  if($option == 'publish' || $option == 'promote' || $option == 'sticky')
  {
      $values = 1; 
  }
  else if($option == 'unpublish' || $option == 'demote' || $option == 'unsticky')
  {
      $values = 0;
  }
  $count_node = count($content_data['node']);
  $count_update_node = 0;
  foreach ($content_data['node'] as $key => $value) { 
    if (is_numeric($key)) {
      $nid = $value['nid'];
      $node_count = db_query("Select count(*) from node_revision WHERE nid=:nid",array(':nid' => $nid))->fetchField(); 
      if ($node_count) {
        $total_nid[] = $nid; 
        if($option == "publish" || $option == "unpublish" ) { 
           $update = array('status' => $values);
           _node_mass_update_helper($nid, $update);
   	  $count_update_node++;
        }
        else if ($option == "promote" || $option == "demote" ) {
          $update = array('promote' => $values);
          _node_mass_update_helper($nid, $update);
	  $count_update_node++;
        }
        else if ($option == "sticky" || $option == "unsticky" ) { 
          $update = array('sticky' => $values);
          _node_mass_update_helper($nid, $update);
          $count_update_node++;
        }
        else if ($option == "delete") { 
          $delete_multi_node[$nid] = $nid; 
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
  if(!empty($delete_multi_node))
  {
     node_delete_multiple($delete_multi_node);
  }
  if ($count_update_node == $count_node) {
    return TRUE;
  }
  if (!empty($error_nid)) {
    $not_found_nid = implode(",",$error_nid);
    return services_error("There is no node with id ".$not_found_nid , 406, null);
  }
}

function node_update($op, $values, $nid)
{
  db_update('node')->fields(array( 
    $op => $values,
  ))  
    ->condition('nid', $nid, '=')
    ->execute(); 
  $count_update_node++;
}

function node_revision_update($op, $values, $nid)
{
 /* db_update('node_revision')->fields(array( 
    $op => $values,
  ))  
    ->condition('nid', $nid, '=')
    ->execute(); 
  $count_update_node++;*/
}
