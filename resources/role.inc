<?php

function _role_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
     return user_access('administer users');
     break;
    case 'update':
     return user_access('administer users');
     break;
    case 'delete':
     return user_access('administer users');
     break;
    case 'create':
     return user_access('administer users'); 
     break;
  }
}

function role_resources() { 
  $resource = array(
    'retrieve' => array(
      'callback' => '_role_resource_retrieve',
        'args' => array(
          array(
            'name' => 'rid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The rid of the node to get',
          ),
        ),
        'access callback' => '_role_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
    'create' => array(
      'callback' => '_role_resource_create',
        'args' => array(
          array(
            'name' => 'role',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The role data to create',
            'type' => 'array',
          ),
        ),
        'access callback' => '_role_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
    'update' => array(
      'callback' => '_role_resource_update',
        'args' => array(
          array(
            'name' => 'rid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The rid of the node to get',
          ),
          array(
            'name' => 'data',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The role data to update',
            'type' => 'string',
          ),
        ),
        'access callback' => '_role_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
    ),
    'delete' => array(
      'callback' => '_role_resource_delete',
        'args' => array(
          array(
            'name' => 'rid',
            'optional' => FALSE,
            'type' => 'int',
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_role_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
    ),
    'index' => array(
      'callback' => '_role_resource_index',
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
        'access arguments' => array('administer users'),
        'access arguments append' => FALSE,
    ),
  );
  return $resource;
}

function _role_resource_retrieve($id) {
  $role = db_query("SELECT r.* FROM {role} r WHERE  r.rid = :rid", array(':rid' => $id))->fetchAssoc();
  return $role;
}

function _role_resource_index($page, $fields, $parameters, $page_size) { 
  $query = db_query("select * from role order by rid asc");
  foreach ($query as $record) {
    $role[] = $record;
  }
  return services_resource_build_index_list($role, 'role','rid');
}

function _role_resource_create($role){
  if(empty($role))
    return services_error("role name filed is required", 406,array('form_errors' => "role[name] field is required."));
  $role_name = $role['name'];
  $data = array(
    'name' => $role_name,
  );
  drupal_write_record('role', $data);
  $query = db_query("select rid from role order by rid desc limit 1");
  foreach ($query as $rid) {
    $results = $rid;
  }
  $role_service_uri['rid'] = $results->rid;
  $role_service_uri['uri'] = services_resource_uri(array('role',$results->rid));
  return $role_service_uri;
}

function _role_resource_update($rid, $role){
  $role_name = $role['role']['name'];
  if(empty($role)){
    return services_error("role name filed is required", 406,array('form_errors' => "role[name] field is required."));
  }
  $code = db_query("SELECT COUNT(*) FROM {role} r WHERE  r.rid = :rid", array(':rid' => $rid))->fetchField();
  if($code) {
    $num_updated = db_update('role')->fields(array(
      'name' => $role_name,
    ))  
    ->condition('rid', $rid, '=')
    ->execute();
    $role_service_uri['role'] = $role_name;
    $role_service_uri['rid'] = $rid;
    return $role_service_uri;
  }
  return services_error("There is no role with id ".$rid,406,null);
}

function _role_resource_delete($rid){
  if($rid == 1 || $rid == 2){
    return services_error("your request will not accepted ",212,null);
  }
  $code = db_query("SELECT COUNT(*) FROM {role} r WHERE  r.rid = :rid", array(':rid' => $rid))->fetchField();	
  if (code) {
    db_delete('role')
    ->condition('rid', $rid)
    ->execute();
    db_delete('role_permission')
    ->condition('rid', $rid)
    ->execute();
    db_delete('users_roles')
    ->condition('rid', $rid)
    ->execute();
    return TRUE;
  }
  return services_error("There is no role with id ".$rid,406,null);
}
