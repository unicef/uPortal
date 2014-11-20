<?php

global $uportal_backend_data;

module_load_include('content_listing.inc', 'uportal_backend');

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = $uportal_backend_data['content-listing']['total-showing-results'] = $view->total_rows;
$cnter = 0;

$first_node = $raw_results[0]->_field_data['nid']['entity'];
$category_field_langcode = field_language('node', $first_node, 'field_topic_reference', NULL);
$category_nid = isset($_GET['cid']) ? $_GET['cid'] : 'all';
$provider_nid = isset($_GET['prid']) ? $_GET['prid'] : 'all';
$series_nid = isset($_GET['series_nid']) ? $_GET['series_nid'] : 'all';
$lesson_plan_nid = isset($_GET['lesson_plan_nid']) ? $_GET['lesson_plan_nid'] : 'all';
$lang_code = isset($_GET['lang']) ? $_GET['lang'] : 'all';
$page_node_status = isset($_GET['status']) ? $_GET['status'] : 'all';

//dpm($raw_results);
//dsm($rendered_results);

?>

<div
  class="content-listing-wrapper <?php print (isset($_GET['collapsed-view'])) ? 'collapsed-items-wrapper' : '' ?>"
  data-content-provider-nid="<?php print $provider_nid ?>"
  data-series-nid="<?php print $series_nid ?>"
  data-lesson-plan-nid="<?php print $lesson_plan_nid ?>"
  data-language-code="<?php print $lang_code ?>"
  data-category-nid="<?php print $category_nid ?>"
  data-status="<?php print $page_node_status ?>"
>
  
  <?php
    foreach ($rendered_results as $result):
      $raw_result = $raw_results[$cnter];
      $row_classes = $classes_array[$cnter];
      
      //published str
      $status_str = '';
      if ($raw_result->node_status!='1') {
        $status_str = '<div class="published-status">Draft</div>';
      }
      
      //get image
      $field_image_raw = isset($raw_result->field_field_image[0]) ? $raw_result->field_field_image[0]['raw'] : false;
      $field_video_raw = isset($raw_result->field_field_video[0]) ? $raw_result->field_field_video[0]['raw'] : false;
      $use_img = false;
      $img_thumbnail = false;
      if ($field_image_raw && $field_video_raw && isset($field_image_raw['is_default']) && $field_image_raw['is_default'] && isset($field_video_raw['thumbnailfile']->uri)) {
        $use_img = $result['field_video'];
      } elseif ($field_image_raw && isset($field_image_raw['uri']) && $field_image_raw['uri']) {
        $use_img = $result['field_image'];
      }
      
      //edit path
      $edit_path = '/management/edit-content/'.$result['nid'];
      
      //row classes
      $row_classes .= ' clearfix content-listing-item item-type-'.$result['type'];
      
  ?>
  
  <div
    id="content-listing-item-nid-<?php print $result['nid'] ?>"
    class="<?php print $row_classes ?>"
    data-updated="<?php print $raw_result->node_changed ?>"
    data-created="<?php print $raw_result->node_created ?>"
    data-nid="<?php print $result['nid'] ?>"
  ><div class="clearfix wrapper">
    <div class="checkbox">
      <input type="checkbox" name="nids[]" class="nid-checkbox" value="<?php print $result['nid'] ?>">
    </div>
    <div class="thumbnail">
      <div class="img">
        <a href="<?php print $edit_path ?>" title="Edit this content">
          <?php print $use_img ?>
        </a>
      </div>
      <div class="icon"></div>
    </div>
    <div class="desc">
      <h2>
        <a href="<?php print $edit_path ?>" title="Edit this content">
          <?php print $result['title'] ?>
        </a>
      </h2>
      <?php print $status_str ?>
      <div class="meta-info clearfix">
        <div class="meta-info-1 clearfix">
          
          <!-- content provider -->
          <?php
            if ($result['title_1'])
              print '<div class="meta-item provider">'.$result['title_1'].'</div>';
            else
              print '<div class="meta-item provider empty-meta">-</div>';
          ?>
          
          <!-- category -->
          <?php
            if ($result['title_3'])
              print '<div class="meta-item category">'.$result['title_3'].'</div>';
            else
              print '<div class="meta-item category empty-meta">-</div>';
          ?>
          
          <!-- series -->
          <?php
            if ($result['title_2'])
              print '<div class="meta-item series meta-item-last">'.$result['title_2'].'</div>';
            else
              print '<div class="meta-item series empty-meta meta-item-last">-</div>';
          ?>
          
        </div>
        <div class="meta-info-2 clearfix">
          
          <!-- updated -->
          <div class="meta-item updated">
            <?php print 'Updated: '.$result['changed'] ?>
          </div>
          
          <!-- language -->
          <?php
            if ($result['language'])
              print '<div class="meta-item language">'.$result['language'].'</div>';
            else
              print '<div class="meta-item language empty-meta">-</div>';
          ?>
          
          <!-- tags -->
          <div class="meta-item tags empty-meta meta-item-last">-</div>
          
        </div>
      </div>
      
      <?php if ($result['body']): ?>
      <div class="body"><?php print $result['body'] ?></div>
      <?php endif; ?>
      
    </div>
  </div></div>
  
  <?php
      $cnter++;
    endforeach;
  ?>
  
</div>
