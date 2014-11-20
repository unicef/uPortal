
<?php

$form['actions']['submit']['#attributes']['class'] = array('submit-btn', 'btn',);
$form['account']['roles']['#title'] = 'Abilities';
$form['account']['roles']['2']['#title'] = 'Can view dashboards';
/*
$form['actions']['delete'] = array (
	'#type' => 'markup',
	'#children' => array(),
	'#printed' => '',
	'#markup' => '<a href="/management/user/delete/" title="Delete user: " class="btn delete-btn">Delete User</a>',
);
*/
$form['actions']['cancel'] = array (
	'#type' => 'markup',
	'#children' => array(),
	'#printed' => '',
	'#markup' => '<a href="/management/users" title="Manage Users" class="btn cancel-btn">Cancel</a>',
);

?>

<div class="user-form create-user-form">
	<div class="user-form-elements-wrapper">
		<?php print drupal_render($form['account']['name']) ?>
		<?php print drupal_render($form['account']['mail']) ?>
		<?php print drupal_render($form['account']['pass']) ?>
		<?php print drupal_render($form['account']['roles']) ?>
		<?php print drupal_render($form['account']['status']) ?>
	</div>
	<div class="actions-wrapper">
		<?php print drupal_render($form['actions']) ?>
	</div>
	<div class="hidden-children">
		<?php print drupal_render_children($form) ?>
	</div>
</div>