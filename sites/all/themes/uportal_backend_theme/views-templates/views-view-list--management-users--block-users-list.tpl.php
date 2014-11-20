<?php

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = count($raw_results);
$users = array();
$cnter = 0;

//prepare user results
foreach ($raw_results as $raw_result) {
  $rendered_result = $rendered_results[$cnter++];
  $user_path_args = uportal_backend_content_listing_build_path(array (
		'user_id' => $raw_result->uid,
		'link_type' => 'user',
	));
	
	//format user access
	$user_access = $rendered_result['access'];
	if (strtotime('-2 days') < $raw_result->users_access) {
		$access_interval = time() - $raw_result->users_access;
		$user_access = format_interval($access_interval, 2).t(' ago');
	}
	
	$users[] = array (
		'uid' => $raw_result->uid,
		'name' => $raw_result->users_name,
		'email' => $raw_result->users_mail,
		'status' => ($raw_result->users_status=='1') ? 'Active' : 'Inactive',
		'path' => $user_path_args['path'],
		'access' => $user_access,
		'access_timestamp' => $raw_result->users_access,
	);
}

?>

<div class="metadata-listing-results-header-wrapper">
  <div class="metadata-listing-page-results-header clearfix centred-strip">
    <ul class="manage-users-list-header clearfix ">
      <li class="label name-label col-label" data-direction="asc">Name</li>
      <li class="label user-status-label col-label" data-direction="asc">Status</li>
      <li class="label last-access-label col-label" data-direction="asc">Last Access</li>
      <li class="label links-label col-label"></li>
    </ul>
  </div>
</div>

<div class="centred-strip"><ul class="manage-users-list">

<?php
  foreach ($users as $user):
		$uid = $user['uid'];
		$user_path = $user['path'];
		$username = $user['name'];
		$user_email = $user['email'];
		$user_status = $user['status'];
		$user_access = $user['access'];
		$user_access_timestamp = $user['access_timestamp'];
?>

<li
		class="user-item main-result-wrapper full-result-row clearfix"
		data-uid="<?php print $uid; ?>"
		data-last-access="<?php print $user_access_timestamp; ?>"
		id="uid-<?php print $uid; ?>" >
	<div class="username float">
		<span class="main-result-title">
			<a href="<?php print $user_path; ?>"><?php print $username; ?></a>
		</span>
		<span class="email">
			<?php print $user_email; ?>
		</span>
	</div>
	<div class="user-status float">
		<?php print $user_status; ?>
	</div>
	<div class="last-access float">
		<?php print $user_access; ?>
	</div>
	<div class="user-edit-icons clearfix float">
		<a href="/management/users/edit/<?php print $uid; ?>" title="Edit user" class="edit-user"></a>
		<a href="#" data-uid="<?php print $uid; ?>" title="Delete user" class="delete-user"></a>
	</div>
</li>

<?php
	endforeach;
?>

</ul></div>
