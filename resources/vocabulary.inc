<?php

function _vocabulary_resource_access($op = 'view', $args = array()) {
  switch ($op) {
    case 'view':
      return user_access('administer taxonomy');
      break;
  }
}

function vocabulary_resources() {
  $node_resource = array(
    'retrieve' => array(
      'help' => 'Retrieve  a latestcomment',
        'callback' => '_vocabulary_resource_retrieve',
	  'args' => array(
            array(
              'name' => 'vid',
              'optional' => FALSE,
              'source' => array('path' => 0),
              'type' => 'int',
              'description' => 'The vid of the vocabulary to get',
            ),
          ),
          'access callback' => '_vocabulary_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
    ),
    'index' => array(
      'callback' => '_vocabulary_resource_index',
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
        'access arguments' => array('administer taxonomy'),
        'access arguments append' => FALSE,
    ),
  );
  return $node_resource;
}

function _vocabulary_resource_retrieve($id) {
  $query = db_query('SELECT t.* FROM {taxonomy_vocabulary} t WHERE  t.vid = :vid', array(':vid' => $id));
  foreach ($query as $record){ 
    $vocabulary= $record;
  }
  return $vocabulary;
}

function _vocabulary_resource_index($page, $fields, $parameters, $page_size) { 
  $return = array();
  $page_size = 10;
  $start_index = 0;
  $page_no = 0;
  $last_index = 10;
  $output=taxonomy_get_vocabularies();
  foreach ($output as $key => $vocab) {
    $vacab_list[] = $vocab;
  }
  for ($i = 0; $i<count($vacab_list); $i= $i+$page_size) {
    $page_vocab[$page_no]=array_slice($vacab_list,$start_index,$last_index);
    $start_index = $start_index + $page_size;
    $last_index = $last_index + $page_size;
    $page_no++;
  }
  return services_resource_build_index_list($page_vocab[$page], 'vocabulary','vid');
}
