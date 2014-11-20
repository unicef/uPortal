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
$nids_classes = $ugyouthportal_data['listing_pages_special_nids_by_class'];
$no_cols = ($view->current_display=='page_2') ? 3 : 4;
$cnter = 0;


/** get special results **************************
 *
 * hogging too much memory
 *
 * moved back to module layer
 * 
**
$special_nids_rows = array();
$sort_by_array = array();
$special_nids_view = views_get_view('listing_pages');
$special_nids_view->execute('block_1');
$special_nids_view->preview();
foreach ($special_nids_view->result as $result) {
	$sort_by_array[] = array_search($result->nid, $ugyouthportal_data['listing_pages_special_nids']);
}
$renderer = $special_nids_view->style_plugin->row_plugin;
foreach ($special_nids_view->result as $index => $row) {
  $special_nids_view->row_index = $index;
  $special_nids_rows[] = $renderer->render($row);
}
array_multisort($sort_by_array, SORT_ASC, $special_nids_view->result);
array_splice($view->result, 0, 0, $special_nids_view->result);
array_multisort($sort_by_array, SORT_ASC, $special_nids_rows);
array_splice($rows, 0, 0, $special_nids_rows);

//dsm($special_nids_rows);
//dsm($special_nids_view);
//dsm($special_nids_view->render());
/********************************************************************/

dsm($view);
//dsm(get_defined_vars());
//dsm($ugyouthportal_data);

?>
<?php print $wrapper_prefix; ?>

  <?php print $list_type_prefix; ?>
    <?php
			foreach ($rows as $id => $row):
				$nid = $view->result[$cnter]->nid;
				$col_class = ' col-'.($cnter%$no_cols);
				$col_class .= isset($nids_classes['nid-'.$nid]) ? ' col-'.$nids_classes['nid-'.$nid] : '';
				$col_class .= (($cnter+1)%$no_cols==0) ? ' col-last-column' : '';
		?>
		
      <li class="<?php print $col_class; ?>"><?php print $row; ?></li>
			
    <?php
				$cnter++;
			endforeach;
		?>
  <?php print $list_type_suffix; ?>
<?php print $wrapper_suffix; ?>
