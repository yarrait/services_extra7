<?php

/**
 * Gets access rules.
 *
 * @param string $op
 *  $op - Permission requested.
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission.
 */
function _backup_migrate_destination_resource_access($op = 'view', $args = array()) {
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
function backup_migrate_destination_resources() { 
  $resource = array(
    'index' => array(
        'callback' => '_backup_migrate_destination_index',
        'access arguments' => array('administer nodes'),
        'access arguments append' => FALSE,
    ),
    'create' => array(
      'callback' => '_backup_migrate_backup_restore_or_delete',
        'args' => array(
          array(
            'name' => 'backup_migrate_restore',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'Backup Restore',
            'type' => 'array',
          ),
        ),
        'access callback' => '_backup_migrate_destination_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
    ),
    'retrieve' => array(
      'callback' => '_backup_migrate_destination_display_files',
        'args' => array(
          array(
            'name' => 'destination_id',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'The destination id of the display files',
          ),
        ),
        'access callback' => '_backup_migrate_destination_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
  );
 
  return $resource;
}


module_load_include('inc', 'backup_migrate', 'includes/crud');
module_load_include('inc', 'backup_migrate', 'includes/destinations');

class backup_migrate_destination_custom extends backup_migrate_destination {
  var $db_table = "backup_migrate_destinations";
  var $type_name = "destination";
  var $default_values = array('settings' => array());
  var $singular = 'destination';
  var $plural = 'destinations';
  var $cache_files = FALSE;
  var $fetch_time = NULL;
  var $cache_expire = 86400; // 24 hours

  var $destination_type = "";
  var $supported_ops = array();

  /**
   * Get a table of all items of this type for service (theme table excludes).
   */

  function dest_get_list() {
    $items = $this->all_items(); 
    $rows = array();
    foreach ($items as $item) {
      if ($row = $item->get_list_row()) { 
        $row['id'] = $item->destination_id;
        $rows[] = $row;
      }
    } 
    if (count($rows)) {
      $out = $rows;
    }
    else {
      $out = t('There are no !items to display.', array('!items' => $this->plural));
    }
    return $out;
  }
}


/**
 * Returns list of destinations. 
 *
 * @return
 *   list of destinations.
 */

function _backup_migrate_destination_index() { 
  module_load_include('inc', 'backup_migrate', 'includes/crud');
  $class = new backup_migrate_destination_custom;
  $out = '';
  if ($type = backup_migrate_crud_type_load('destination')) { 
    $out = $class->dest_get_list();  
    if($out) {
      foreach ($out as $key => $val) {
        if ($val['id'] != 'db') {
          $destinations[] = $val;
        }
      }
    } 
    return $destinations;
  } else {
    return services_error("Please try again..",406,null);
  }
}

/**
 * Returns backup files in the given destination.
 * @param $destination_id
 *  $destination_id - destination_id (manual, scheduled, etc).
 * @return
 *   list of destination files.
 */
function _backup_migrate_destination_display_files($destination_id = NULL) { 
  module_load_include('inc', 'backup_migrate', 'includes/destinations'); 
  $rows = $sort = array();
  if ($destination = backup_migrate_get_destination($destination_id)) {
    // Refresh the file listing cache if requested.
    if (isset($_GET['refresh'])) {
      $destination->file_cache_clear();
      drupal_goto($_GET['q']);
    }


//    drupal_set_title(t('%title Files', array('%title' => $destination->get_name())));
    $headers = array(
      array('data' => 'Filename', 'field' => 'filename'),
      array('data' => 'Date', 'field' => 'filetime'),
      array('data' => 'Age',  'field' => 'filetime', 'sort' => 'desc'),
      array('data' => 'Size', 'field' => 'filesize'),
      t('Operations'),
    );

    $sort_order = tablesort_get_order($headers);
    $sort_key   = $sort_order['sql'] ? $sort_order['sql'] : 'filetime';
    $sort_dir   = tablesort_get_sort($headers) == 'desc' ? SORT_DESC : SORT_ASC;

    $files      = $destination->list_files();

    $i          = 0;
    foreach ((array)$files as $file) {
      $info = $file->info();
      // Show only files that can be restored from.
      if ($file->is_recognized_type()) {
        $sort[] = $info[$sort_key];
        $rows[] = array(
          'filename' => check_plain($info['filename']),
          'date_time' => format_date($info['filetime'], 'small'),
          'age' => format_interval(time() - $info['filetime'], 1),
          'size' => format_size($info['filesize']),
          'links' => implode(" | ", $destination->get_file_links($file->file_id())),
        );
      }
    }

    array_multisort($sort, $sort_dir, $rows);

    if ($rows) {
      return $rows;
    }
    else {
      return services_error("There are no backup files to available.", 406, null);
    }
  }
}


/**
 * Returns success message on restore or delete else service error.
 * @param $data
 *  $data - restore or delete data (like filename, destination_id, etc..).
 * @return
 *   success message or service error.
 */
function _backup_migrate_backup_restore_or_delete($data) {
  module_load_include('inc', 'backup_migrate', 'includes/filters'); 
  module_load_include('inc', 'backup_migrate', 'includes/crud'); 
  module_load_include('inc', 'backup_migrate', 'includes/destinations'); 

  global $user;
  $destination_id = $data['data']['destination_id'];
  $file = $data['data']['filename'].'.mysql.gz';
  $settings = array();
  $op = $data['data']['op'];
  switch($op)
  {
    case 'Delete':
      backup_migrate_destination_delete_file($destination_id, $file);
      $message['backup_migrate'] = array(
        'message' => "Database backup file deleted: ". $file,
      );
      return $message;
    case 'Restore':
      if(backup_migrate_destination_file_exists($destination_id, $file))
      {  
         $advance_settings = backup_migrate_filters_settings_form(backup_migrate_filters_settings_default('restore'), 'restore');

         $form_state['values']['op'] = 'Restore';
         $form_state['values']['destination_id'] = $destination_id;
         $form_state['values']['file_id'] = $file;
         $form_state['values']['source_id'] = 'db';
         $form_state['values']['confirm'] = 1;
         $form_state['values']['filter']['utils_site_offline'] = 0;
         $form_state['values']['filter']['utils_site_offline_message'] = $advance_settings['advanced']['utils_site_offline_message']['#default_value'];

         drupal_form_submit('backup_migrate_ui_destination_restore_file_confirm', $form_state, $destination_id, $file);
         
         if($error = form_get_errors())
         {
            return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
         }
         $message['backup_migrate'] = array(
           'message' => "Default Database restored from Filer Directory file ". $file,
         );
         return $message;
      }
      else
      {
        return services_error('File '. $file .' from the destination '. $destination_id .' is not exists.', 404, null);
      }
   } 
}
