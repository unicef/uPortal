<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */

global $ugyouthportal_data;

$page_args = arg();
$cnter = 0;

$current_page = $ugyouthportal_data['index_current_page'];
$no_of_pages = $ugyouthportal_data['index_page_no'];
$current_path = base_path().request_path();

$item_no_cnter = ($current_page*$ugyouthportal_data['no_per_listing_page']) + 1;

?>

<div id="index-page-results">
	
<?php print $wrapper_prefix; ?>

  <?php print $list_type_prefix; ?>
    <?php
			foreach ($rows as $id => $row):
				if (isset($view->result[$cnter]->nid)):
		?>
		
      <li class="index-topic-content">
				<?php print $row.'<div class="counter">'.($item_no_cnter++).'</div>'; ?>
			</li>
			
    <?php
					$cnter++;
				endif;
			endforeach;
		?>
  <?php print $list_type_suffix; ?>
<?php print $wrapper_suffix; ?>

</div>


<?php if ($no_of_pages>1): ?>
<div id="pager-wrapper">
	<ul class="pager">
		<?php
			for ($i=0; $i<$no_of_pages; $i++) {
				if ($i==$current_page) {
					echo '<li><span>'.($i+1).'</span></li>';
				} else {
					$url = $current_path.'?';
					$url .= 'cid='.$_GET['cid'].'&tags='.$_GET['tags'].'&type='.$_GET['type'].'&lang='.$_GET['lang'];
					$url .= ($i==0) ? '' : '&page='.$i;
					echo '<li><a href="'.$url.'" title="Go to Page '.($i+1).'"><span>'.($i+1).'</span></a></li>';
				}
			}
		?>
	</ul>
</div>
<?php endif; ?>

