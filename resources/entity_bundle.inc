<?php

/**
 * Gets access rules.
 *
 * @param $op
 *  $op - Permission requested
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission
 */
function _entity_bundle_resource_access($op = 'view', $args = array()) {  
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
function entity_bundle_resources() {
  $resource = array(
     'retrieve' => array(
        'callback' => '_entity_bundle_resource_retrieve',
	  'args' => array(
          array(
            'name' => 'type',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'must pass entity type at end of url for getting list',
          ),
        ),
        'access callback' => '_entity_bundle_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
  );
  return $resource;
}

function _entity_bundle_resource_retrieve($entity_type){ 
  $type_data_select = entity_get_info($entity_type);
  $bundle = $type_data_select['bundles'];
  $result = array();
  foreach ($bundle as $key => $value)
  {
    $type['type'] = $key;
    $type['name'] = $value['label'];
    $result[] = $type;
  }
  return $result;
}
