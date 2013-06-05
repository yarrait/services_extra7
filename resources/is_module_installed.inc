<?php

/**
 * Gets access rules.
 *
 * @param string $op
 *  $op - Permission requested.
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission.
 */
function _is_module_installed_resource_access($op = 'view', $args = array()) { 
  return TRUE;
}


/**
 * Gets all resource definitions.
 *
 * @return array
 *  An array containing all resources.
 */
function is_module_installed_resources() { 
  $resource = array(
    'index' => array(
        'callback' => '_is_module_installed_index',
        'access callback' => '_is_module_installed_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
  );
  return $resource;
}

/**
 * Returns list of installed Modules. 
 *
 * @return
 *   list of installed Modules. 
 */
function _is_module_installed_index() {
  $modules['installed_modules'] = module_list();
  return $modules;
}
