<?php
// $Id: gradebookapi.admin.inc,v 1.1.2.1.2.6 2009/03/02 22:11:59 mgn Exp $

/**
 * @file
 * gradebookapi.admin.inc: Administration pages for gradebookapi
 */

function gradebookapi_admin_settings() {
  $form = array();
  drupal_set_title(t('Gradebook API configuration'));

  $form['gradebook_assignment_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Assignment Types'),
    '#multiple' => TRUE,
    '#default_value' => (array) variable_get('gradebook_assignment_types', array()),
    '#options' => array_map('check_plain', node_type_get_names()),
    '#description' => t('A list of node types you want to treat as assignments.'),
    '#weight' => -10,
  );

  $form['gradebookapi_nav_vocabulary'] = array(
    '#type' => 'select',
    '#title' => t('Gradebook vocabulary'),
    '#default_value' => variable_get('gradebookapi_nav_vocabulary', ''),
    '#options' => gradebookapi_get_vocabularies(),
    '#description' => t('Select the vocabulary that will be used to organize gradebooks'),
    '#weight' => -11,
  );

  return system_settings_form($form);
}

function gradebookapi_get_vocabularies() {
  $result = db_query("SELECT vid, name FROM {taxonomy_vocabulary} v");

  $vocabs = array();
  foreach ($result as $voc) {
    $vocabs[$voc->vid] = $voc->name;
  }

  return $vocabs;
}

