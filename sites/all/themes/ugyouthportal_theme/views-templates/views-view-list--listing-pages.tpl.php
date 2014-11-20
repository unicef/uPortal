<?php


global $ugyouthportal_data;

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

?>

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

