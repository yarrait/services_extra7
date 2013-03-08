<?php

/**
 * Gets access rules.
 *
 * @param string $op
 *  $op - Permission requested.
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission.
 */
function _backup_migrate_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
      return user_access('administer users');
      break;
    case 'create':
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
function backup_migrate_resources() { 
  $resource = array(
    'index' => array(
        'callback' => '_backup_migrate_index',
        'access arguments' => array('administer nodes'),
        'access arguments append' => FALSE,
    ),
    'create' => array(
      'callback' => '_backup_migrate_quickbackup_create',
        'args' => array(
          array(
            'name' => 'backup_migrate',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'Quick Backup',
            'type' => 'array',
          ),
        ),
        'access callback' => '_backup_migrate_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
  );
 
  return $resource;
}

/**
 * Returns list of available Sources, Destinations and Profiles . 
 *
 * @return
 *   list of available Sources, Destinations and Profiles.
 */
function _backup_migrate_index() { 
  module_load_include('inc', 'backup_migrate', 'includes/destinations');
  module_load_include('inc', 'backup_migrate', 'includes/profiles');
  $list = array();
  $list['source_list'] = _backup_migrate_get_destination_form_item_options('source');
  $destinations = _backup_migrate_get_destination_form_item_options('manual backup');
  if($destinations) {
     foreach($destinations as $key => $val) {
         if($key != 'download') {
           $opt[$key] = $val;
         }
     }
     $list['destination_list'] = $opt;
  }
  $list['profile_list'] = _backup_migrate_get_profile_form_item_options();
  return $list;
}

