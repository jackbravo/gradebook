<?php
// $Id: gradebook.admin.inc,v 1.1.2.2.2.4 2009/03/02 22:11:59 mgn Exp $

/**
 * @file
 * Admininistration pages for gradebook.module
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_admin_settings($form, &$form_state) {
  $form = array();

  drupal_set_title(t('Gradebook configuration'));

  $form['sitewide'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sitewide gradebook settings'),
    '#weight' => -1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Any student and teacher roles selected here will apply to ALL gradebooks. If you are using the OG_Gradebook module, do not select any names here and gradebooks will be formed by group membership. Otherwise, any roles selected here will appear in the gradebook for each group.'),
  );

  $roles = user_roles();
  unset($roles[DRUPAL_ANONYMOUS_RID]);
  unset($roles[DRUPAL_AUTHENTICATED_RID]);

  $sel_roles = (array) variable_get('gradebook_student_rids', array());
  $default = array();
  foreach ($sel_roles as $rid => $value) {
    if ($value) {
      $default[] = $rid;
    }
  }
  $form['sitewide']['gradebook_student_rids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Student roles'),
    '#default_value' => $default,
    '#options' => $roles,
    '#required' => FALSE, // permit no sitewide student roles when using organic groups...
  );

  $sel_roles = (array) variable_get('gradebook_teacher_rids', array());
  $default = array();
  foreach ($sel_roles as $rid => $value) {
    if ($value) {
      $default[] = $rid;
    }
  }
  $form['sitewide']['gradebook_teacher_rids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Teacher roles'),
    '#default_value' => $default,
    '#options' => $roles,
    '#required' => FALSE, // permit no sitewide teacher roles when using organic groups...
  );

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General gradebook settings'),
    '#weight' => 0,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['general']['gradebook_empty_grade'] = array(
    '#type' => 'textfield',
    '#title' => t('Empty grade'),
    '#default_value' => variable_get('gradebook_empty_grade', '--'),
    '#description' => t('This the text displayed when no grade has been entered.'),
    '#required' => TRUE,
  );

  $number = drupal_map_assoc(array(5, 10, 25, 50, 100));
  $form['general']['gradebook_gradebooks_per_page'] = array(
    '#type' => 'select',
    '#title' => t('Gradebooks per page'),
    '#default_value' => variable_get('gradebooks_per_page', 25),
    '#options' => $number,
    '#description' => t('The default number of gradebooks displayed per page.'),
  );

  $number = drupal_map_assoc(range(5, 25));
  $form['general']['gradebook_grades_per_page'] = array(
    '#type' => 'select',
    '#title' => t('Grades per page'),
    '#default_value' => variable_get('gradebook_grades_per_page', 10),
    '#options' => $number,
    '#description' => t('The default number of assignments, displayed on the gradebook page, and grades on the grade edit forms. A pager will display when the number of entries exceeds this number.'),
  );

  return system_settings_form($form);
}
