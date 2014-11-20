<?php


global $ugyouthportal_data;

$raw_results = $view->result;
$rendered_results = $view->style_plugin->rendered_fields;

//dsm($view);

/**
 * a quick hack installed to ensure NO REPEATS
 * need to look deeper and test where repeats are coming from
 */
$no_repeat_nids = array();

$page_args = arg();
$nids_classes = (isset($_SESSION['uportal_category_content_nids_by_class']) && $view->current_display!='page_4') ? $_SESSION['uportal_category_content_nids_by_class'] : array();
$no_cols = (isset($page_args[0]) && $page_args[0]=='category') ? 3 : 4;
$cnter = 0;

$current_page = isset($_SESSION['current_listing_page_no']) ? $_SESSION['current_listing_page_no'] : 0;
$no_of_pages = isset($_SESSION['uportal_category_content_nids_by_page']) ? count($_SESSION['uportal_category_content_nids_by_page']) : 0;
$current_path = base_path().request_path();

$no_printed = 0;

?>

<?php if (isset($page_args[0]) && $page_args[0]=='welcome-uportal'): ?>

<div class="home-page-listing-results">

	<ul class="home-page-listing-ul home-page-list-items clearfix">
    <?php
			foreach ($rows as $id => $row):
				
				if ($no_printed==5) {
					break;
				}
				
				if (isset($view->result[$cnter]->nid)):
					$nid = $view->result[$cnter]->nid;
					
					$rendered_result = $rendered_results[$cnter];
					$raw_result = $raw_results[$cnter];
					
					//image
					$content_image = $rendered_result['field_image'];
					if ($raw_result->node_type=='video') {
						$content_image = $rendered_result['field_video'];
					}
					
					//series category title
					$series_category_title = $raw_result->node_field_data_field_topic_reference_title;
					if (isset($raw_result->node_field_data_field_series_reference_nid) && is_numeric($raw_result->node_field_data_field_series_reference_nid)) {
						$series_category_title = $raw_result->node_field_data_field_series_reference_title;
					}
					
					if (!isset($no_repeat_nids['nid-'.$nid])):
						$col_class = ' col-'.($cnter%$no_cols);
						$col_class .= isset($nids_classes['nid-'.$nid]) ? ' col-'.$nids_classes['nid-'.$nid] : '';
						$col_class .= (($cnter+1)%$no_cols==0) ? ' col-last-column' : '';
						$col_class .= ($view->result[$cnter]->node_comment_statistics_comment_count<=0) ? ' no-comments' : '';
						$col_class .= ' node-type-'.$raw_result->node_type;
						$col_class .= ' category-class-nid-'.$raw_result->node_field_data_field_topic_reference_nid;
						$col_class .= ' content-listing-item';
						$col_class .= ($no_printed==4) ? ' col-last' : '';
		?>
		
      <li class="<?php print $col_class; ?>">
				<a href="<?php print $rendered_result['path'] ?>">
					<span class="img">
						<span class="icon"><span></span></span>
						<span class="node-status status"></span>
						<?php print $content_image ?>
						
						<span class="hover-title"><span class="wrapper">
							<span class="category-title"><?php print $series_category_title ?></span>
							<span class="series-title"><?php print $rendered_result['title'] ?></span>
						</span></span>
						
					</span>
					<span class="desc">
						<span class="series-category-title"><?php print $series_category_title ?></span>
						<span class="content-title"><?php print $rendered_result['title'] ?></span>
						<span class="language"><?php print $rendered_result['language'] ?></span>
					</span>
				</a>
			</li>
		
		<?php if ($no_printed==3): ?>
		
			<li class="series-list-item col-new">
				<?php print views_embed_view('series', 'block_6'); ?>
			</li>
			
		<?php endif; ?>
		
    <?php
						$cnter++;
						$no_printed++;
						$no_repeat_nids['nid-'.$nid] = 1;
					endif;
				endif;
			endforeach;
		?>
	</ul>
	
</div>

<?php else: ?>

<div id="listing-page-results">
	
<?php print $wrapper_prefix; ?>

  <?php print $list_type_prefix; ?>
    <?php
			foreach ($rows as $id => $row):
				if (isset($view->result[$cnter]->nid)):
					$nid = $view->result[$cnter]->nid;
					
					if (!isset($no_repeat_nids['nid-'.$nid])):
						$col_class = ' col-'.($cnter%$no_cols);
						$col_class .= isset($nids_classes['nid-'.$nid]) ? ' col-'.$nids_classes['nid-'.$nid] : '';
						$col_class .= (($cnter+1)%$no_cols==0) ? ' col-last-column' : '';
						$col_class .= ($view->result[$cnter]->node_comment_statistics_comment_count<=0) ? ' no-comments' : '';
		?>
		
      <li class="<?php print $col_class; ?>"><?php print $row; ?></li>
			
    <?php
						$cnter++;
						$no_repeat_nids['nid-'.$nid] = 1;
					endif;
				endif;
			endforeach;
		?>
  <?php print $list_type_suffix; ?>
<?php print $wrapper_suffix; ?>

</div>


<?php if ($view->current_display!='page_4' && $no_of_pages>1): ?>
<div id="pager-wrapper">
	<ul class="pager">
		<?php
			for ($i=0; $i<$no_of_pages; $i++) {
				if ($i==$current_page) {
					echo '<li><span>'.($i+1).'</span></li>';
				} else {
					$url = ($i==0) ? $current_path : $current_path.'?page='.$i;
					echo '<li><a href="'.$url.'" title="Go to Page '.($i+1).'"><span>'.($i+1).'</span></a></li>';
				}
			}
		?>
	</ul>
</div>
<?php endif; ?>

<?php endif; ?>