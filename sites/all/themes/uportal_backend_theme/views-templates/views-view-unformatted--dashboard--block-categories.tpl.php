<?php

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$no_results = count($view_results);

$all_content_link_args = uportal_backend_content_listing_build_path();
$all_content_link = '<a href="'.$all_content_link_args['path'].'"><span class="icon"><span></span></span><span class="text">Content</span></a>';

?>


<h2 class="dashboard-section-heading quick-links">Quick Links</h2>

<div class="dashboard-section quick-links-content">
  <ul class="clearfix">
    <li class="content">
      <?php print $all_content_link ?>
    </li>
    <li class="categories">
      <a href="/management/metadata/categories" title="Manage Categories"><span class="icon"><span></span></span><span class="text">Categories</span></a>
    </li>
    <li class="content-providers">
      <a href="/management/metadata/content-providers" title="Manage Content Providers"><span class="icon"><span></span></span><span class="text">Content Providers</span></a>
    </li>
    <li class="languages">
      <a href="/management/metadata/languages" title="Manage Languages"><span class="icon"><span></span></span><span class="text">Languages</span></a>
    </li>
    <li class="tags">
      <a href="/management/metadata/tags" title="Manage Tags"><span class="icon"><span></span></span><span class="text">Tags</span></a>
    </li>
    <li class="users">
      <a href="/management/users" title="Manage Users"><span class="icon"><span></span></span><span class="text">Users</span></a>
    </li>
  </ul>
</div>

<h2 class="dashboard-section-heading content-heading">Published Content: Overview</h2>

<div class="clearfix published-content-wrapper dashboard-section">

  <?php foreach ($rows as $id => $row): ?>
  <div class="category-content <?php print $classes_array[$id]; ?>">
    <?php print $row; ?>
   </div>
  <?php endforeach; ?>

</div>

<h2 class="dashboard-section-heading draft-content-heading">Draft Content: Recent Uploads</h2>

<div class="dashboard-section draft-content-list">
  <?php print views_embed_view('dashboard', 'block_4'); ?>
</div>