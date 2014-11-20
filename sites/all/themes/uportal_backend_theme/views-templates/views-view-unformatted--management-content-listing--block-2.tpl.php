<?php

global $uportal_backend_data;

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = $uportal_backend_data['content-listing']['total-showing-results'] = $view->total_rows;
$cnter = 0;

?>

<div class="content-listing-wrapper series-listing-wrapper series-lesson-plan-listing-wrapper"
  data-content-provider-nid="<?php print isset($_GET['prid']) ? $_GET['prid'] : 'all' ?>"
  data-language-code="<?php print isset($_GET['lang']) ? $_GET['lang'] : 'all' ?>"
  data-category-nid="<?php print isset($_GET['cid']) ? $_GET['cid'] : 'all' ?>">
  
  <?php
    foreach ($rendered_results as $result):
      $raw_result = $raw_results[$cnter];
      
      //series link
      $series_path_args = uportal_backend_content_listing_build_path(array(
        'content_section_type' => 'individual-items',
        'series_nid' => $result['nid'],
        'order_by' => 'series',
      ));
      $series_path = $series_path_args['path'];
      
      //get children
      $child_content = views_get_view_result('management_content_listing', 'block_3', $result['nid']);
      $child_content_no_total = count($child_content);
      $child_content_no_published = 0;
      $child_content_no_drafts = 0;
      $series_images = '';
      $no_images = 0;
      $row_classes = $classes_array[$cnter];
      
      foreach ($child_content as $child_item) {
        if ($no_images<4 && isset($child_item->field_field_image) && count($child_item->field_field_image)) {
          $img = drupal_render($child_item->field_field_image[0]['rendered']);
          $img = '<a href="'.$series_path.'" title="Edit series items">'.$img.'</a>';
          $series_images .= '<span class="small-img small-img-'.$no_images.'">'.$img.'</span>';
          $no_images++;
        }
        if ($child_item->node_status=='1') {
          $child_content_no_published++;
        } else {
          $child_content_no_drafts++;
        }
      }
      
      $item_title = $raw_result->node_title;
      if ($child_content_no_published>0 || $child_content_no_drafts>0) {
        $item_title = '<a href="'.$series_path.'" title="Edit series items">'.$item_title.'</a>';
      }
      
      //numbers string
      $series_numbers = $child_content_no_total;
      $series_numbers .= ($child_content_no_total==1) ? ' item' : ' items';
      $drafts_published_str = '';
      $series_numbers_published = '0 published item(s)';
      $series_numbers_draft = '0 draft item(s)';
      if ($child_content_no_published>0) {
        $drafts_published_str = $child_content_no_published.' published';
        $series_numbers_published = $child_content_no_published.' published item(s)';
      }
      if ($child_content_no_drafts>0) {
        $drafts_published_str .= ($child_content_no_published>0) ? ' and ' : '';
        $drafts_published_str .= $child_content_no_drafts;
        $drafts_published_str .= ($child_content_no_drafts>1) ? ' drafts' : ' draft';
        $series_numbers_draft = $child_content_no_drafts.' draft item(s)';
      }
      if ($drafts_published_str) {
        $series_numbers .= ' ('.$drafts_published_str.')';
      }
      
      $item_classes = $row_classes.' clearfix series-listing-item content-listing-item series-lesson-plan-listing-item';
      $disabled_btn_class = '';
      if ($child_content_no_total<=0) {
        $item_classes .= ' zero-children';
        $disabled_btn_class = ' disabled';
      }
      
  ?>
  
  <div
    id="series-listing-item-nid-<?php print $result['nid'] ?>"
    class="<?php print $item_classes ?>"
    data-updated="<?php //print $raw_result->node_changed ?>"
    data-created="<?php //print $raw_result->node_created ?>"
    data-category-nid="<?php print $result['nid_1'] ?>"
    data-series-nid="<?php print $result['nid'] ?>">
    
    <div class="thumbnail series-thumbnails">
      <?php print $series_images ?>
    </div>
    
    <div class="desc">
      <div class="clearfix series-title-wrapper title-wrapper">
        <h2><?php print $item_title ?></h2>
        <div class="edit-operations">
          <div class="delete-series delete-btn edit-btn" data-series-nid="<?php print $result['nid'] ?>">
            <div class="icon"></div>
            <div class="txt">Delete</div>
          </div>
          <div class="edit-series edit-btn" data-series-nid="<?php print $result['nid'] ?>">
            <div class="icon"></div>
            <div class="txt">Edit</div>
          </div>
          <div class="order order-series edit-btn <?php print $disabled_btn_class ?>"
            data-category-nid="<?php print $result['nid_1'] ?>"
            data-series-nid="<?php print $result['nid'] ?>">
            <div class="icon"></div>
            <div class="txt">Reorder Items</div>
          </div>
        </div>
      </div>
      <div class="category-numbers-wrapper clearfix">
        <div class="category-title">
          <?php print $result['title_3'] ?>
        </div>
        <div class="numbers published-numbers">
          <?php print $series_numbers_published ?>
        </div>
        <div class="numbers draft-numbers">
          <?php print $series_numbers_draft ?>
        </div>
      </div>
      <div class="body"><?php print $result['body'] ?></div>
    </div>
  </div>
  
  <?php
      $cnter++;
    endforeach;
  ?>
  
</div>
