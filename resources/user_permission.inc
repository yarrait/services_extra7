<?php

function _permission_module_user_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
      return user_access('administer permissions');
       break;
    case 'update':
      return user_access('administer permissions');
      break;
    }
}

function user_permission_resources() {
  $resource = array(
    'retrieve' => array(
      'callback' => '_permission_module_user_resource_retrieve',
        'args' => array(
          array(
            'name' => 'modules',
            'optional' => TRUE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'The permission of the module to get',
          ),
          array(
            'name' => 'roles',
            'optional' => TRUE,
            'source' => array('param' => 'role'),
            'type' => 'int',
            'description' => 'The enabled permission of the role to get',
          ),
        ),
        'access callback' => '_permission_module_user_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
    'update' => array(
      'callback' => '_permission_module_user_resource_update',
        'args' => array(
          array(
            'name' => 'rid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The rid of the role to get',
          ),
          array(
            'name' => 'permission',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The permission data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_permission_module_user_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
    ),
    'index' => array(
      'callback' => '_permission_module_user_resource_index',
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
     'access arguments' => array('administer permissions'),
     'access arguments append' => FALSE,
    ),
  );
  return $resource;
}

function _permission_module_user_resource_retrieve($module = NULL, $role = 0) {
//permission_module_user/block?role=1
  if($module && $role) {
    $query = db_select('role_permission', 'p');
    $query -> fields('p',array('permission'));
    $query -> condition('rid', $role, '=');
    $result = $query->execute();
    while ($role_record = $result->fetchAssoc()) {
      $role_permissions[] = $role_record['permission'];
    }  
    $permission = module_invoke($module, 'permission');
    foreach ($permission as $key =>$value) {
             $perm[] = $key;
    }
    $role_permission_total = count($role_permissions);
    $module_permission_total = count($perm);
    for ($i = 0; $i<$role_permission_total; $i++) {
      for ($j = 0; $j<$module_permission_total; $j++) {
        if (strcasecmp($perm[$j],$role_permissions[$i]) == 0) {
             $match_array['Enable'][] = $perm[$j];
        }
      }
    }
    if (empty($match_array)) 
       return FALSE; 
    else
       return $match_array;
  }
  else if ($module && !$role) {
    $permissions = module_invoke($module, 'permission');
      $i = 0;
      foreach($permissions as $key =>$value) {
        $perm[$module][$i]['title'] = $value['title'];
        $perm[$module][$i]['permission'] = $key;
        $i++;
      }
    return $perm;
  }
  return services_error('No module with ' .$module.' found.', 404);
}

function _permission_module_user_resource_index($page, $fields, $parameters, $page_size) {
  $modules = array();
  $modules["modules"]=module_implements('permission');
  asort($modules);
  return $modules;
}

function _permission_module_user_resource_update($rid, $permission) {
  $permissions = $permission['perm']['enabled'];
  if(empty($permission['perm']['module']))// || empty($permission['perm']['enabled']))
    return services_error("module name or permission field is requried" ,406,array('form_errors' => "module name or permission field is requried."));

  $permissions = module_invoke($permission['perm']['module'], 'permission');
  foreach ($permissions as $key =>$value) {
    $module_permission[] = $key;
  }
  $query = db_select('role_permission', 'p');
  $query -> fields('p',array('permission'));
  $query -> condition('rid',$rid,'=');   
  $result = $query->execute();

  while ($record = $result->fetchAssoc()) {
    $role_enabled_perm[] = $record['permission'];
  }
  
  $perm_enable_diff = array();
  $perm_disable_diff = array();
  if (!empty($role_enabled_perm)) {
    $module_enabled_perm = array_intersect($role_enabled_perm ,$module_permission);
    if(!empty($permission['perm']['enabled'])) {
      $perm_enable_diff = array_diff($permission['perm']['enabled'], $module_enabled_perm);
      $perm_disable_diff = array_diff($module_enabled_perm, $permission['perm']['enabled']);
    }
    else {
      $perm_disable_diff = $module_enabled_perm;
    }
  }

  if(!empty($perm_enable_diff)) {
    foreach ($perm_enable_diff as $key => $perm) {
      $data = array(
        'permission' => $perm,
        'rid'        => $rid,
        'module'     => $permission['perm']['module'],
      );
      drupal_write_record('role_permission', $data);
    }
  }
  if(!empty($perm_disable_diff)) {
    foreach ($perm_disable_diff as $key => $perm) {
      db_delete('role_permission')
        ->condition('rid', $rid)
        ->condition('permission', $perm)
        ->condition('module', $permission['perm']['module'])
        ->execute();
    }
  }
  $return['status_message'] = 'The changes have been saved.';
  return $return;
}
