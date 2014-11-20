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

//dsm($view);
//dsm(get_defined_vars());
//dsm($ugyouthportal_data);
//dsm($ugyouthportal_data['dsm_fields']);

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
