<?php

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = count($raw_results);
$languages = locale_language_list('name');

?>

<?php
	foreach ($raw_results as $raw_result):
	
		//category title block
		$category_title_block_view = views_get_view_result('categories', 'block_0', $raw_result->nid);
		$category_title_block = '<span class="category-icon"><span class="parent-icon uportal-icon"></span><span class="hover-icons"><span class="uportal-icon icon-1"></span><span class="uportal-icon icon-2"></span><span class="uportal-icon icon-3"></span><span class="uportal-icon icon-4"></span><span class="uportal-icon icon-5"></span></span></span><span class="title-number"><span class="title">'.$raw_result->node_title.'</span><span class="number">'.number_format(count($category_title_block_view), 0).'</span></span>';
		$category_title_block = l($category_title_block, 'category/'.$raw_result->nid.'/all/all/all', array('html'=>true));
		
		//category content
		$category_content_view = views_get_view_result('categories', 'block_2', $raw_result->nid);
		$category_content = '';
		$category_content_cnter = 0;
		foreach ($category_content_view as $category_content_item) {
			
			//image
			$node_type = $category_content_item->node_type;
			$img = '';
			$img_raw = $category_content_item->field_field_image[0]['raw'];
			$img_render = $category_content_item->field_field_image[0]['rendered'];
			$img_video_render = isset($category_content_item->field_field_video[0]['rendered']) ? $category_content_item->field_field_video[0]['rendered'] : '';
			if ( isset($img_raw['is_default']) && $img_raw['is_default'] && $node_type=='video' && $img_video_render ) {
				$img = render($img_video_render);
			} else {
				$img = render($img_render);
			}
			
			//series category title
			$series_category_title = $category_content_item->node_field_data_field_topic_reference_title;
			if (isset($category_content_item->node_field_data_field_series_reference_nid) && is_numeric($category_content_item->node_field_data_field_series_reference_nid)) {
				$series_category_title = $category_content_item->node_field_data_field_series_reference_title;
			}
			
			//language name
			$language_name = '';
			if (isset($languages[$category_content_item->node_language])) {
				$language_name = $languages[$category_content_item->node_language];
			}
			
			$content_item_str = '<span class="img">';
			$content_item_str .= '<span class="icon"><span></span></span><span class="node-status status"></span>'.$img;
			$content_item_str .= '<span class="hover-title"><span class="wrapper"><span class="category-title">'.$series_category_title.'</span><span class="series-title">'.$category_content_item->node_title.'</span></span></span>';
			$content_item_str .= '</span>';
			$content_item_str .= '<span class="desc"><span class="series-category-title">'.$series_category_title.'</span><span class="content-title">'.$category_content_item->node_title.'</span><span class="language">'.$language_name.'</span></span>';
			$content_item_str = l($content_item_str, 'node/'.$category_content_item->nid, array('html'=>true));
			
			$li_classes = 'content-listing-item category-listing-item category-listing-item-content category-listing-item-content-'.$category_content_cnter;
			$li_classes .= ' node-type-'.$node_type;
			$li_classes .= ($category_content_cnter==2) ? ' col-last' : '';
			$category_content .= '<li class="'.$li_classes.'">';
			$category_content .= $content_item_str;
			$category_content .= '</li>';
			$category_content_cnter++;
		}
?>

<div class="category-listing-item-wrapper category-class-nid-<?php print $raw_result->nid ?>">
	
<ul class="category-listing-item home-page-listing-ul clearfix">
	
	<li class="category-list-item category-list-item-title-block category-list-item-first">
		<?php print $category_title_block ?>
	</li>
	
	<?php print $category_content ?>
	
	<li class="category-list-item category-list-series-item category-list-item-last series-list-item col-last">
		<?php print views_embed_view('series', 'block_6', $raw_result->nid); ?>
	</li>
	
	
</ul>

</div>

<?php endforeach; ?>