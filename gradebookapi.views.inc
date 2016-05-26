<?php
// $Id: gradebookapi.views.inc,v 1.1.2.2 2009/06/03 02:47:57 mgn Exp $
/**
 * @file
 * Views integration with Gradebook module.
 * This is experimental and still in progress...
 */

/*
 *
 * Database definition:
 * @code
 *  $schema['gradebookapi_assignment'] = array(
 *    'fields' => array(
 *      'nid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'possible'    => array('type' => 'float', 'unsigned' => FALSE, 'not null' => TRUE, 'default' => 0),
 *      'publish_date' => array( 'description' => 'Date assignment becomes effective.', 'type' => 'int',
 *        'unsigned' => FALSE, 'not null' => TRUE, 'default' => 0, 'disp-width' => '10',),
 *      'due_date' => array('description' => 'Date assignment is due.', 'type' => 'int', 'unsigned' => FALSE,
 *        'not null' => TRUE, 'default' => 0, 'disp-width' => '10', ),
 *      'sid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE),
 *    ),
 *    'primary key' => array('nid'),
 *  );
 * @endcode
 */

/**
 * Implementation of hook_views_data().
 */
function gradebookapi_views_data() {
  // Basic table information.
  // ----------------------------------------------------------------
  // gradebookapi_grade table
//  New group within Views called 'Gradebook Grade'

  $data = array();
  $data['gradebookapi_grade']['table']['group']  = t('Gradebook Grade');

  // Tables + fields that can be used for SQL joins.
  $data['gradebookapi_grade']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );
  $data['gradebookapi_grade']['uid'] = array(
    'title' => t('User'),
    'help' => t('The user associated with the grade.'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User'),
    ),
  );
  $data['gradebookapi_grade']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node associated with the grade.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
    ),
  );
  $data['gradebookapi_grade']['earned'] = array(
    'title' => t('Grade'),
    'help' => t('Student Grade on this assignment'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['gradebookapi_grade']['exempt'] = array(
    'title' => t('Exempt'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

