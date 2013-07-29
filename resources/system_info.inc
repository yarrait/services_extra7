<?php

function _system_info_resource_access($op = 'view', $args = array()) {
  return TRUE;
}

function system_info_resources() {
  $resource = array(
    'index' => array(
      'callback' => '_system_info_resource_index',
       'access callback' => '_system_info_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
  );
  return $resource;
}

function _system_info_resource_index() {
  //getting version of drupal
  $var['version'] = VERSION;

  //getting version of service extra module
  $services_extra = system_get_info('module', 'services_extra');
  $var['services_extra_version'] = $services_extra['version'];

  $services = system_get_info('module', 'services');
  $var['services_version'] = $services['version'];

  if(module_exists('druproid_notifications'))
  {
    $druproid_notifications = system_get_info('module', 'druproid_notifications');
    $var['druproid_notifications_version'] = $druproid_notifications['version'];
  }
  
  return $var;
}
