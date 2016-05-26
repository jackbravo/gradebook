<?php
// $Id: gradebook.pages.inc,v 1.1.2.10 2009/04/02 05:38:06 mgn Exp $

/**
 * @file
 * Gradebook callback pages associated with displaying or editing a grade in the gradebook
 */

/**
 *  Provide either an assignment summary (if $user is a student)
 *  or an edit form (if $user is a teacher)
 *
 *  @param $gradebook
 *    the $gradebook object
 *  @param $uid
 *    path argument for student user id whose grade we want to edit/view
 *  @param $nid
 *    parth argument for the node id for the assignment we want to edit/view
 *
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_page($gradebook, $uid, $nid) { //all users with 'access gradebook' permissions can view this page
  og_context('node', $gradebook);
  $account = user_load($uid);

  if ($account !== FALSE && gradebookapi_is_student_or_teacher($gradebook, $account)) {
    $types = gradebookapi_get_assignment_types();
    $node = node_load($nid);
    if (in_array($node->type, $types)) {
      $grade = gradebookapi_get_grade($uid, $nid);
      $grades[$uid] = (array) $grade;
      return theme_gradebook_grade_form(drupal_get_form('gradebook_grade_form', $gradebook, $grades));
    }
  }
  drupal_not_found();
}

/**
 *  Enter grades for all students by assignment
 *    This form is only accessible to teachers
 *  @param $gradebook
 *    the $gradebook object
 *  @param $nid
 *    path argument for the node id for the assignment we want to edit/view
 *
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_assignment_page($gradebook, $nid) { // only accessed by teacher
  og_context('node', $gradebook);
  $types = gradebookapi_get_assignment_types();
  $node = node_load($nid);
  if (in_array($node->type, $types)) {
    $grades = array();
    $students = array();
    $students = gradebookapi_get_students($gradebook);
    foreach ($students['uid'] as $uid) {
      $grade = gradebookapi_get_grade($uid, $nid); //now its safe to get $grade
      $grades[$uid] = (array) $grade;
    }
    return theme_gradebook_grade_form(drupal_get_form('gradebook_grade_form', $gradebook, $grades));
  }
  drupal_not_found();
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_student_page($gradebook, $uid) { // access is limited to teacher through menu system
  og_context('node', $gradebook);
  $account = user_load($uid);
  if (gradebookapi_is_student_or_teacher($gradebook, $account)) {
    // get assignments
    $assignments = gradebook_get_assignments($gradebook, TRUE);

    // get grades for assignments
    $grades = array();
    foreach ($assignments as $assignment) {
      $grades[$assignment->nid] = (array) gradebookapi_get_grade($account->uid, $assignment->nid);
    }
    return theme_gradebook_grade_form(drupal_get_form('gradebook_grade_form', $gradebook, $grades));
  }
  drupal_not_found();
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_form($form, $form_state, $gradebook, $grades) {
  global $user;

  $teacher = gradebookapi_can_grade($gradebook);
  $students = gradebookapi_get_students($gradebook);
  $suid = array_flip($students['uid']); // this is needed because keys of $students is lost in module_invoke_all call

  $form['grade'] = array('#tree' => TRUE);
  $form['#grades'] = $grades; // make available for hook_form_alter
  foreach ($grades as $id => $grade) {
//    $account = user_load(array('uid' => $grade['uid']));
    $uid = $grade['uid'];
    $display_name = $students['last_name'][$suid[$uid]] . ', ' . $students['first_name'][$suid[$uid]];

    $node = node_load($grade['nid']);
    $form['grade'][$id]['#node'] = $node; // make available for hook_form_alter
    $form['grade'][$id]['uid'] = array(
      '#type' => 'value',
      '#value' => $uid,
    ); // must be '#type' => value
    $form['grade'][$id]['nid'] = array(
      '#type' => 'value',
      '#value' => $node->nid,
    ); // must be '#type' => value
    $form['grade'][$id]['user'] = array('#value' => $display_name);
    $form['grade'][$id]['node'] = array('#value' => $node->title);
    $form['grade'][$id]['timestamp'] = array(
      '#type' => 'value',
      '#value' => $grade['timestamp'],
    );

    if ($teacher) { // display edit form
      $options = array('', 'A+', 'A', 'A-', 'B+', 'B', 'B-', 'W', 'P', 'I', 'T', 'NC');
      $options = array_combine($options, $options);
      $form['grade'][$id]['earned'] = array(
        '#type' => 'select',
        '#default_value' => $grade['earned'],
        '#options' => $options,
      );
      $form['grade'][$id]['exempt'] = array(
        '#type' => 'checkbox',
        '#default_value' => $grade['exempt'],
      );
      $form['grade'][$id]['note'] = array(
        '#type' => 'textarea',
        '#cols' => 40,
        '#rows' => 6,
        '#default_value' => $grade['note'],
      );
      //display the grade date only on the "Final Assignment
      if ($grade['timestamp'] == 0) {
        $grade_timestamp = REQUEST_TIME;
      }
      else {
        $grade_timestamp = $grade['timestamp'];
      }
      if ($node->title == "Final") {
        $form['grade'][$id]['timestamp'] = array(
          '#type' => 'date_popup',
          '#date_format' => 'Y-m-d',
          '#default_value' => date('Y-m-d', $grade_timestamp),
          '#date_label_position' => 'within',
          '#date_timezone' => 'America/Chicago',
          '#date_increment' => 15,
          '#date_year_range' => '-3:+3',
        );
      }
      else {
        $form['grade'][$id]['timestamp'] = array(
          '#type' => 'hidden',
          '#date_format' => 'Y-m-d',
          '#default_value' => date('Y-m-d', REQUEST_TIME),
          '#date_label_position' => 'within',
          '#date_timezone' => 'America/Chicago',
          '#date_increment' => 15,
          '#date_year_range' => '-3:+3',
        );
      }
    }
    else { // display grades without option for editing
      if ($grade['earned'] != '') {
        $form['grade'][$id]['earned'] = array('#value' => $grade['earned']);
      }
      $form['grade'][$id]['exempt'] = array(
        '#type' => 'checkbox',
        '#value' => $grade['exempt'],
        '#disabled' => TRUE,
      );
      $form['grade'][$id]['note']   = array('#value' => $grade['note']);
    }
  }
  $modules = module_implements('gradebook_form_add_elements');
  foreach ($modules as $module) {
    $func = $module . '_gradebook_form_add_elements';
    $func($form, $form_state, $grades, $gradebook);
  }
  $form['return_url'] = array(
    '#type' => 'value',
    '#value' => 'gradebook/' . $gradebook->nid,
  );
  $form['teacher'] = array(
    '#type' => 'value',
    '#value' => $teacher,
  );
  if ($teacher) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save All'),
    );
  }
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Return to Gradebook'),
    '#submit' => array('gradebook_grade_form_cancel'),
  );

  return $form;
}

/*
 * No validation is needed for floating point grades, but saving this function for future development.
 */
/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_form_validate($form, &$form_state) {
  // do nothing
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_form_cancel($form, &$form_state) {
  unset($form_state['storage']); // clear the storage bins now that files have been saved - needed for redirect
  $form_state['redirect'] = $form_state['values']['return_url'];
  unset($form_state['values']['return_url']);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gradebook_grade_form_submit($form, &$form_state) {
  $keys = array_keys($form_state['values']['grade']);
  $count = 0;
  foreach ($keys as $key) {
    $grade = (object) $form_state['values']['grade'][$key];
    if ($form_state['values']['teacher']) {
      $node = node_load($form_state['values']['grade'][$key]['nid']);
      $earned = $form_state['values']['grade'][$key]['earned'];
      if ($earned != $form['grade'][$key]['#oldearned']) { // use which one changed... if both changed, use points...
        $grade->earned = $earned;
      }
      gradebookapi_set_grade($grade);
    }
    else {
      if ($grade->timestamp == 0) { // grade doesn't exist in gradebookapi_grade, save a null grade
        gradebookapi_set_grade($grade);
      }
      else { // grade already exists, but give other modules a chance to do something here.
        module_invoke_all('gradebookapi_grade', 'set', $grade);
      }
    }
    ++$count;
  }
  unset($form_state['storage']);
  if ($count > 0) {
    drupal_set_message(t('Saved %num %type.', array('%num' => $count, '%type' => t('grades'))));
  }
  else {
    drupal_set_message(t('No changes were made to grades'));
  }
  $form_state['redirect'] = $form_state['values']['return_url'];
  unset($form_state['values']['return_url']);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function theme_gradebook_grade_form($form) {
  global $user;
  $ids = element_children($form['grade']);
  $header = array();
  $header += array(
    'name' => t('Name'),
    'title' => t('Assignment Title'),
    'grade' => t('Grade'),
    'exempt' => t('Exempt'),
    'note' => t('Note'),
    'date' => t('Grade Date'),
  );
  foreach ($ids as $id) {
    $row = NULL;
    $row[] = array(
      'data' => $form['grade'][$id]['user']['#value'],
      'class' => 'name',
    );
    // allow modules to add information to the title block
    $module_output = _gradebook_module_additions('title', $form, $id, $form['teacher']['#value'], '<br />');
    $row[] = array(
      'data' => $form['grade'][$id]['node']['#value'] . $module_output,
      'class' => 'title',
    );
    if (isset($form['grade'][$id]['earned'])) {
      $row[] = array(
        'data' => drupal_render($form['grade'][$id]['earned']),
        'class' => 'earned-possible',
      );
    }
    else {
      $row[] = array();
    }
    $row[] = array(
      'data' => drupal_render($form['grade'][$id]['exempt']),
      'class' => 'exempt-box',
    );
    $output = '<div class="note">' . drupal_render($form['grade'][$id]['note']) . '</div>';
    // give modules a chance to add output below the note block
    // can't use module_invoke, module_invoke_all, or call_user_func, because these do not pass arguemtns by reference!

    $output .= _gradebook_module_additions('note', $form, $id, $form['teacher']['#value']);
    $row[] = array('data' => $output);
    if (isset($form['grade'][$id]['timestamp'])) {
      $row[] = array(
        'data' => drupal_render($form['grade'][$id]['timestamp']),
        'class' => 'timestamp',
      );
    }
    else {
      $row = array();
    }
    $rows[] = array('data' => $row);
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('gradebook'))));
  $output .= theme('pager', array('tags' => NULL, 'element' => 0)); // configure at admin screen!
  $output .= '<div>' . _gradebook_module_additions('bottom', $form, -1, $form['teacher']['#value']) . '</div>';
  $output .= '<div>' . drupal_render($form['submit']) . drupal_render($form['cancel']) . '</div>';
  $output .= drupal_render_children($form);
  $variables['element'] = $form;
  $variables['element']['#children'] = $output;
  return theme_form($variables);
}

function _gradebook_module_additions($position, &$form, $id, $teacher, $separator = '') {
  $output = '';
  $modules = module_implements('gradebook_theme_add_content');
  foreach ($modules as $module) {
    $func = $module . '_gradebook_theme_add_content';
    $output .= $func($position, $form, $id, $teacher); // need to pass by reference!
    $output .= $separator;
  }
  return $output;
}
