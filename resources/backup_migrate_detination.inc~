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
  include_once(DRUPAL_ROOT . '/' . drupal_get_path('module', 'backup_migrate') . '/backup_migrate.module');
  module_load_include('inc', 'backup_migrate', 'includes/filters'); 
  module_load_include('inc', 'backup_migrate', 'includes/crud'); 
  module_load_include('inc', 'backup_migrate', 'includes/destinations'); 

  $destination_id = $data['data']['destination_id'];
  $file = $data['data']['filename'];
  $settings = array();

  if($data['data']['op'] == 'delete') { 
    $file = $file. ".mysql.gz";
    backup_migrate_destination_delete_file($destination_id, $file);
    $message['backup_migrate'] = array(
      'message' => "Database backup file deleted: ". "$file",
    );
    return $message;
  }

  timer_start('backup_migrate_restore');

  // If not in 'safe mode', increase the maximum execution time:
  if (!ini_get('safe_mode') && ini_get('max_execution_time') < variable_get('backup_migrate_backup_max_time', 1200)) {
    set_time_limit(variable_get('backup_migrate_restore_max_time', 1200));
  }

  // Make the settings into a default profile.
  if (!is_object($settings)) {
    $settings = backup_migrate_crud_create_item('profile', $settings);
    $settings->source_id = empty($settings->source_id) ? 'db' : $settings->source_id;
  }

  // Register shutdown callback.
  register_shutdown_function('backup_migrate_shutdown', $settings);

  if (!is_object($file)) {
    // Load the file from the destination.
    $file = backup_migrate_destination_get_file($destination_id, $file);
    if (!$file) {
      _backup_migrate_message("Could not restore because the file could not be loaded from the destination.", array(), 'error');
      backup_migrate_cleanup();
      return FALSE;
    }
  }

  // Filter the file and perform the restore.
  $file = backup_migrate_filters_restore($file, $settings);
  if (!$file) {
    if (_backup_migrate_check_timeout()) {
      backup_migrate_restore_fail('Could not perform the restore because the script timed out. Try increasing your PHP <a href="!url">max_execution_time setting</a>.', array('!url' => 'http://www.php.net/manual/en/info.configuration.php#ini.max-execution-time'), 'error');
    }
    else {
      backup_migrate_restore_fail("Could not perform the restore.", array(), 'error');
    }
    backup_migrate_cleanup();
    return FALSE;
  }

  $time = timer_stop('backup_migrate_restore');
  if ($file) {
    $destination = backup_migrate_get_destination($destination_id);
    $source = $settings->get_source_name();
    $dest = $destination->get_name();
    $time = $time['time'];

    $message['backup_migrate'] = array(
      'message' => "$source". ' restored from '. "$dest" .' file ' . "$file". ' in '."$time". ' ms ',
    );

  // Delete any temp files we've created.
    backup_migrate_cleanup();
    return $message;
  }
  else {
    return services_error("Please try again..",406,null);
  }
}
