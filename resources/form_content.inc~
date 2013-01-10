<?php

/**
 * Gets access rules.
 *
 * @param $op
 *  $op - Permission requested
 * @return Boolean
 *  Boolean TRUE if the current user has the requested permission
 */
function _form_content_resource_access($op = 'view', $args = array()) {
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
function form_content_resources() {
  $resource = array(
    'retrieve' => array(
      'callback' => '_form_content_resources_retrieve',
        'args' => array(
          array(
            'name' => 'type',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'Provide name of content type to get form structure',
          ),
        ),
        'access callback' => '_form_content_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
    ),
  );
  return $resource;
}


/**
 * Returns the form structure. 
 *
 * @param $type
 *   $type - content type name
 * @return
 *   form structure
 */
function _form_content_resources_retrieve($type) { 
  module_load_include('inc', 'node', 'node.pages');
  global $user;

  $types = node_type_get_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;

  // If a node type has been specified, validate its existence.
  if (isset($types[$type]) && node_access('create', $type)) {
    // Initialize settings:
    $node = array(
      'uid' => $user->uid,
      'name' => (isset($user->name) ? $user->name : ''),
      'type' => $type,
      'language' => '',
    );

    $form = drupal_get_form($type.'_node_form', (object) $node);

    drupad_form_apply_pre_render($form);

    drupad_convert_form_options_to_strict_array($form);
    drupad_convert_form_files_uri_to_url($form);
    drupad_remove_form_groups_recursion($form);

    return $form;
  }
   
  return FALSE;
}

/**
 * Applies the #pre_render callback recursively to a form and its children
 */
function drupad_form_apply_pre_render(&$item) {
  if (isset($item['#pre_render']) && is_array($item['#pre_render'])) {
    foreach ($item['#pre_render'] as $function) {
      if (function_exists($function)) {
        $item = $function($item);
      }
    }
  }
  $children_keys = element_children($item);
  foreach ($children_keys as $child_key) {
    drupad_form_apply_pre_render($item[$child_key]);
  }
}

/**
 * Converts form component's options to strict array
 *
 * Walk through each form component recursively for #options array.
 * Converts #options array to a strict array (not a associative array)
 * in order to keep the order revelant, as hashes don't have specific order.
 *
 * Also replace 0 value as "0" (string) so that it isn't interpreted as BOOL value
 * by Objective-C.
 *
 * @param &$form
 */
function drupad_convert_form_options_to_strict_array(&$item) {
  $children_keys = element_children($item);
  foreach ($children_keys as $child_key) {
    // Reached an optionable component (select or radios)
    if (isset($item[$child_key]['#options']) && is_array($item[$child_key]['#options'])) {
      $options = array();
      // Replaces #options with a strict array since
      // hashes don't keep order outside of PHP
      foreach ($item[$child_key]['#options'] as $key => $value) {
        if (is_object($value)) {
          $option = $value->option;
          reset($option); // Shouldn't be useful
          $options[] = array(key($option) => current($option));
        }
        else {
          // Dirty hack, but no choice
          if ($value === 0 && $key === 0) {
            $key = "0";
          }
          $options[] = array((string) $key => $value);
        }
      }
      // Shiny new options ordered
      $item[$child_key]['#options'] = $options;
    }
    // Continue deeper in the form item's structure
    drupad_convert_form_options_to_strict_array($item[$child_key]);
  }
}

/**
 * Creates URL for managed_file which are refered by URI
 *
 * URI aren't really useful on iOS for now
 */
function drupad_convert_form_files_uri_to_url(&$item) {
  $children = element_children($item);
  foreach ($children as $key) {
    if (isset($item[$key]['#type']) && $item[$key]['#type'] == 'managed_file' && isset($item[$key]['#file']) && isset($item[$key]['#file']->uri)) {
      $item[$key]['#file']->drupad_url = file_create_url($item[$key]['#file']->uri);
    }
    drupad_convert_form_files_uri_to_url($item[$key]);
  }
}


/**
 * @file
 * Drupad forms handling file
 *
 * This file makes managing FAPI forms easier on iOS side
 * Functions in this file are only intended as helpers or
 * replacements for Drupal core functions that don't do
 * exactly what we would except for us, which is most of the
 * time not implying the theme layer.
 */

function drupad_remove_form_groups_recursion(&$item) {
  $children = element_children($item);
  foreach ($children as $key) {
    if (isset($item[$key]['#groups'])) {
      unset($item[$key]['#groups']);
    }
    drupad_remove_form_groups_recursion($item[$key]);
  }
}

