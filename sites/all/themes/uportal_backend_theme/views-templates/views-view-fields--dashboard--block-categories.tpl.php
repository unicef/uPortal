<?php

module_load_include('content_listing.inc', 'uportal_backend');
$category_link_args = uportal_backend_content_listing_build_path(array(
  'category_nid' => $row->nid,
  'link_type' => 'category',
));

?>

<div class="header-wrapper clearfix">
  <div class="category-title clearfix header-title">
    <h3 class="clearfix">
      <div class="cat-icon"><?php print $fields['field_icon_2']->content;  ?></div>
      <div class="cat-title" > <?php print $fields['title']->content; ?></div>
    </h3>
  </div>
  <div class="operation-links clearfix header-link">
    <a href="<?php print $category_link_args['path']; ?>" class="btn category-link-btn">View All</a>
  </div>
</div>

<div class="dashboard-graphs-wrapper">
  <?php print views_embed_view('dashboard', 'block_1', $row->nid ); ?>
</div>
