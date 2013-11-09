<?php

function _comments_resource_access($op = 'view', $args = array()) {
  switch ($op){
    case 'view':
      return user_access('administer comments');
      break;
  }
}

function comments_resources() {
  $resource = array(
    'retrieve' => array(
      'callback' => '_comments_resource_retrieve',
        'args' => array(
          array(
            'name' => 'cid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'Get recently added commnets',
          ),
        ),
        'access callback' => '_comments_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'index' => array(
        'callback' => '_comments_resource_index',
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
          'access arguments' => array('administer comments'),
          'access arguments append' => FALSE,
      ),
  );
  return $resource;
}

function _comments_resource_retrieve($cid = 0) {
  $query = db_select('comment', 'c')
         -> condition('cid', $cid,'>')
         -> fields('c')
         -> orderBy('cid','DESC') 
         -> execute();

  while($record = $query->fetchAssoc()) {
    $new_comments[] = $record;
  }

  $total_num_comment=db_query("SELECT count(*) from  comment")->fetchField();; 

  $recent_comment['total'] = $total_num_comment;
  $recent_comment['array'] = $page_no; 
  return $recent_comment;
}

function _comments_resource_index($page, $fields, $parameters, $page_size) { 
  $key = array_keys($parameters);
  $value = array_values($parameters);
  if (empty($parameters)) {
     $total_no_comments = db_query("SELECT count(*) FROM {comment}")->fetchField();
  }
  else if ($key[1]) {
     $total_no_comments = db_query("SELECT count(*) FROM {comment} where $key[0]=:key1 && $key[1]=:key2",array(':key1' => $value[0],':key2' => $value[1]))
       ->fetchField();
   }
  else if ($key[0]) {
    $total_no_comments = db_query("SELECT count(*) FROM {comment} where $key[0]=:key1",array(':key1' => $value[0]))->fetchField();
  } 
  $query = db_select('comment', 'c');
  $query -> join('field_data_comment_body', 'u', 'c.cid = u.entity_id');
  $query -> fields('c')
         -> fields('u',array('comment_body_value'));
  $query -> orderBy('cid','DESC'); 
  $result = $query->execute();
  while ($record = $result->fetchAssoc()) {
    foreach ($record as $key => $value) {
      if($key == 'comment_body_value') {
        $output['comment'] = $value;
      }
      else {
        $output[$key] = $value;
      }
    }
      $results[] = $output;
  }
  $comment_select = db_select('comment', 't')
    ->orderBy('created', 'DESC');

  services_resource_build_index_query($comment_select, $page, $fields, $parameters, $page_size, 'comment');

  if (!user_access('administer comments')) {
    $comment_select->condition('status', COMMENT_PUBLISHED);
  }

  /*$results = services_resource_execute_index_query($comment_select);
  foreach ($results as $result) {
 $output[] = $result;
      }*/
  
  
 // return services_resource_build_index_list($results, 'comment', 'cid');
  $comments['total'] = $total_no_comments;
  $comments['comment'] = $results; 
  return $comments;
}
