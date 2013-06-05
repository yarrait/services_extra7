<?php
/**
 * Gets access rules.
 *
 * @param string $op
 * $op - Permission requested.
 * @return Boolean
 * Boolean TRUE if the current user has the requested permission.
 */
function _image_upload_resource_access($op = 'create', $args = array()) {
  switch ($op) {
    case 'create':
      return user_access('administer users');
      break;
    case 'update':
     return user_access('administer users');
     break;
  }
}

/**
 * Gets all resource definitions.
 *
 * @return array
 * An array containing all resources.
 */
function image_upload_resources() {
  $resource = array(
    'create' => array(
      'callback' => '_image_upload_resources_create',
        'args' => array(
          array(
            'name' => 'image upload',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'Image Upload',
            'type' => 'array',
          ),
        ),
        'access callback' => '_image_upload_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
    'update' => array(
      'callback' => '_image_upload_resources_update',
        'args' => array(
          array(
            'name' => 'fid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The fid of the image field',
          ),
          array(
            'name' => 'file',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The object of file',
            'type' => 'object',
          ),
        ),
        'access callback' => '_image_upload_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
    ),

  );
 
  return $resource;
}

/**
 * Returns image information on success otherwise service error message
 * @param $imagedata
 * $imagedata - Required data to upload image (Like, imagename, image type).
 * @return
 * image information on success otherwise service error message
 */
function _image_upload_resources_create($imagedata) { 

  if(!empty($imagedata)) {
    if(!empty($imagedata['node'])) { 
      $node = $imagedata['node'];
      
      $validators_decoded = (array) json_decode($imagedata['#upload_validators']); //print_r($validators_decoded); exit;
 
      $validators = array(
        'file_validate_extensions' => $validators_decoded['file_validate_extensions'],
        'file_validate_size' => $validators_decoded['filefield_validate_size'],
      );
      $image_str = $imagedata['imagefield_byte_code'];
      $image = base64_decode($image_str); 

      $field = field_info_instance('node', $imagedata['#field_name'], $imagedata['#type_name']); 
    
      //$node->field_image[LANGUAGE_NONE][0] = (array)$file;
      $file = file_save_data($image, 'public://' .$imagedata['file_name']); 

      file_usage_add($file, 'services_extra', 'node', $node->nid);
      if(!empty($file)) {
        return $file;
      }
    }
  }
    return FALSE;
}

function _image_upload_resources_update($fid, $file) {  
  return file_delete((object)$file); 
}


/**
 * Returns Boolean value
 *
 * @param $fid
 * $fid - file id
 * @return
 * Boolean TRUE if the file is deleted else FALSE
 */
/*
function _image_upload_resources_delete($fid) { print_r($fid); exit; 
  if(isset($fid)) {
    $removed_file = file_load($fid); print_r($remove_file); exit;
    if ($removed_file['status'] == 0 || $removed_file['status'] == 1) {
      if(field_file_delete($removed_file)) {
        return TRUE;
      } else return FALSE;
    }
  }
  return FALSE;
}*/
