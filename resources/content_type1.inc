<?php

function _content_type_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
      return TRUE;//user_access('administer content types');
      break;
  }
}

function content_type_resources() {
  $resource = array(
     'retrieve' => array(
        'callback' => '_content_type_resource_retrieve',
	  'args' => array(
          array(
            'name' => 'type',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'The type of the node to get',
          ),
        ),
        'access callback' => '_content_type_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
     'index' => array(
        'callback' => '_content_type_resource_index',
        'args' => array(
          array(
            'name' => 'page',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'The zero-based index of the page to get, defaults to 0.',
            'default value' => 0,
            'source' => array('param' => 'page'),
          ),
          array(
            'name' => 'fields',
            'optional' => TRUE,
            'type' => 'string',
            'description' => 'The fields to get.',
            'default value' => '*',
            'source' => array('param' => 'fields'),
          ),
          array(
            'name' => 'parameters',
            'optional' => TRUE,
            'type' => 'array',
            'description' => 'Parameters array',
            'default value' => array(),
            'source' => array('param' => 'parameters'),
          ),
          array(
            'name' => 'pagesize',
            'optional' => TRUE,
            'type' => 'init',
            'description' => 'Number of records to get per page.',
            'default value' => variable_get('services_node_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access callback' => '_content_type_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
  );
  return $resource;
}

function _content_type_resource_retrieve($value){
  $content_type = db_query("SELECT n.* FROM {node_type} n WHERE  n.type = :type", array(':type' => $value))->fetchAssoc();
  return $content_type;  
}

function _content_type_resource_index($page, $fields, $parameters, $page_size){  
  $content_type_data_select = db_select('node_type', 't')
    ->orderBy('name', 'asc'); 
  services_resource_build_index_query($content_type_data_select, $page, $fields, $parameters, $page_size, 'node');
  $content_type_query = services_resource_execute_index_query($content_type_data_select);
  $content_types_array = services_resource_build_index_list($content_type_query, 'node_type', 'type');
  $content_types = array();
  foreach($content_types_array as $key => $type) {
    if(node_access('create', $type->type)) {
       $content_types[] = $type;
    }
  } 
  return $content_types;
}
