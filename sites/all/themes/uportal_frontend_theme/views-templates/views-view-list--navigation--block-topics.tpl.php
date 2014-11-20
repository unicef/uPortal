<?php

$home_icon = theme_image(array(
	'path' => path_to_theme().'/images/icon-menu-home.png',
	'attributes' => array(),
));
$home_menu_title_icon = theme_image(array(
	'path' => path_to_theme().'/images/icon-menu-title-home.gif',
	'attributes' => array(),
));
$default_menu_title_icon = theme_image(array(
	'path' => path_to_theme().'/images/icon-menu-title-default.gif',
	'attributes' => array(),
));
$index_menu_title_icon = theme_image(array(
	'path' => path_to_theme().'/images/icon-menu-title-index.png',
	'attributes' => array(),
));
$icon_topic_default_white = theme_image(array(
	'path' => path_to_theme().'/images/icon-topic-default-white.gif',
	'attributes' => array(),
));

$results = $view->style_plugin->rendered_fields;
$view_results = $view->result;
$results_no = count($view_results);
$mega_menu_title = '<div class="img">'.$home_menu_title_icon.'</div><div class="name">Home</div>';
$mega_menu_title_index_page = '<div class="img">'.$index_menu_title_icon.'</div><div class="name">Content Index</div>';
$mega_menu_title_about_page = '<div class="img">'.$default_menu_title_icon.'</div><div class="name">About uPortal</div>';

$active_topic_nid = false;
$page_args = arg();
if (isset($page_args[0])&& $page_args[0]=='content-index') {
	$mega_menu_title = $mega_menu_title_index_page;
} elseif ($_GET['q']=='node/234') {
	$mega_menu_title = $mega_menu_title_about_page;
} else {
	//active node
	$page_args = arg();
	$active_node = false;
	if (isset($page_args[1]) && is_numeric($page_args[1])) {
		$active_node = node_load($page_args[1]);
	}
	if ($active_node) {
		$active_node_type = $active_node->type;
		if ($active_node_type == 'topic') {
			$active_topic_nid = $active_node->nid;
		} else {
			$sub_node_types = array('series'=>1, 'sub_topic'=>1, 'video'=>1, 'audio'=>1, 'document'=>1,);
			if (isset($sub_node_types[$active_node_type])) {
				if (isset($active_node->field_topic_reference['und']) && count($active_node->field_topic_reference['und'])) {
					$active_topic_nid = $active_node->field_topic_reference['und'][0]['target_id'];
				}
			}
		}
	}
	if ($active_topic_nid) {
		$mega_menu_title = views_embed_view('navigation', 'block_1', $active_topic_nid);
	}
}

//dsm(get_defined_vars());
//dsm($results);

$nav_cols = array (
	'col_1' => '<ul class="nav-col nav-col-1">',
	'col_2' => '<ul class="nav-col nav-col-2">',
	'col_3' => '<ul class="nav-col nav-col-3">',
);

$col_cnter = 1;
$no_cols = 3;
for ($i=0; $i<$results_no; $i++) {
	$topic_nid = $view_results[$i]->nid;
	
	$classes_str = 'topic';
	$classes_str .=  ($active_topic_nid && $topic_nid==$active_topic_nid) ? ' topic-active' : '';
	
	$link = l(
		$results[$i]['title'],
		'category/'.$topic_nid.'/all/all/all',
		array(
			'attributes' => array (
				'title'	=>	'View this category',
				'class' => array(),
			),
			'html'	=> true,
		)
	);
	
	$nav_cols['col_'.$col_cnter] .= '<li class="'.$classes_str.'">'.$link.'</li>';
	
	//col cnter
	$col_cnter = ($col_cnter>=$no_cols) ? 1 : ($col_cnter+1);
}

$nav_cols['col_1'] .= '</ul>';
$nav_cols['col_2'] .= '</ul>';
$nav_cols['col_3'] .= '</ul>';
$nav_str = $nav_cols['col_1'].$nav_cols['col_2'].$nav_cols['col_3'];

$about_link = l(
	'<span class="img">'.$icon_topic_default_white.'</span><span class="title">About uPortal</span>',
	'node/234',
	array (
		'attributes' => array (
			'class' => array('about'),
			'title' => 'More information about the uPortal',
		),
		'html' => TRUE,
	)
);

module_load_include('index_page.inc', 'ugyouthportal');
$link_options = array (
	'title' => 'View the Content Index',
	'html-link' => TRUE,
	'killsearch' => TRUE,
);
$all_url = _ugyouthportal_get_url(array(), $link_options, 'primary-nav');
$index_link = l(
	'<span class="img">'.$icon_topic_default_white.'</span><span class="title">Content Index</span>',
	$all_url['url'],
	$all_url['options']
);

?>

<div class="mega-menu-title-nav-wrapper">
	<div id="mega-menu-title" class="clearfix"><?php print $mega_menu_title ?></div>
	<div id="mega-menu-popper">
		<div class="menu-wrapper">
			<div class="home">
				<a href="/" title="Back to home page"><span class="clearfix wrapper">
					<span class="img"><?php print $home_icon ?></span>
					<span class="title">Home</span>
			</a>
			</div>
			<div class="topics clearfix"><?php print $nav_str ?></div>
			<div class="extras clearfix">
				<?php print $index_link.$about_link; ?>
			</div>
		</div>
	</div>
</div>