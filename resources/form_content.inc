<?php

/**
 * Gets access rules.
 *
 * @param $op
 *  $op - Permission requested
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission
 */
function _form_content_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
      return user_access('administer users');
      break;
  }
}


/**
 * Gets all resource definitions.
 *
 * @return array
 *  An array containing all resources.
 */
function form_content_resources() {
  $resource = array(
    'retrieve' => array(
      'callback' => '_form_content_resources_retrieve',
        'args' => array(
          array(
            'name' => 'type',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'Provide name of content type to get form structure',
          ),
        ),
        'access callback' => '_form_content_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
  );
  return $resource;
}


/**
 * Returns the form structure. 
 *
 * @param $type
 *   $type - content type name
 * @return
 *   form structure
 */
function _form_content_resources_retrieve($type) { 
  module_load_include('inc', 'node', 'node.pages');
  global $user;

  $types = node_type_get_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;


//  $my_content_type_fields = field_info_instances("node", $type);
//  print_r($my_content_type_fields); 

//  $my_content_type_fields = drupal_get_schema($type);
//  print_r($my_content_type_fields); 


/*  global $user; 
  $form = array();
  $form_state = array();
  $node = new stdClass();
  $node->uid = $user->uid;
  $node->name = $user->name;
  $node->type = $type;
  $node->language = 'und';
  $form_state['rebuild'] = true;
//  $form_state['method'] = 'post'; 
  $form = drupal_get_form($type.'_node_form', $node);
  drupal_prepare_form($type.'_node_form', $form, $form_state);
  dpm($form); exit;
*/



  // If a node type has been specified, validate its existence.
  if (isset($types[$type]) && node_access('create', $type)) {
    // Initialize settings:
    $node = new stdClass();
    $node->uid = $user->uid;
    $node->name = $user->name;
    $node->type = $type;
    $node->language = 'und';

    $form = drupal_get_form($type.'_node_form', $node);
    if($form) {
      $field_list = array();
      $field_list['title'] = $form['title'];
      foreach($form as $key => $val) { //print_r($form['body']); exit;
         if($key == 'title' || $key == 'body' || strpos($key, 'field') > -1) { 
           if($val['und'][0]['value']) { 
             $field_list[$key] = $val['und']['0']['value'];
           }
           else if($val['und']) {
             $field_list[$key] = $val['und'];
           }
         }
       }
      print_r($field_list); exit;
      return $form; 
    } 
 }
   
  return FALSE;
}




