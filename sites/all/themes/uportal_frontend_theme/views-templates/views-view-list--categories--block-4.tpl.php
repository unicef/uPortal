<?php

$rendered_results = $view->style_plugin->rendered_fields;
$raw_results = $view->result;
$no_results = count($raw_results);

$home_class = ($_GET['q']=='welcome-uportal') ? 'active' : '';

?>

<ul class="main-menu clearfix">
	<li class="first home home-icon <?php print $home_class ?>"><a href="/" title="Back to home page"><span class="uportal-icon icon"></span><span class="title">Home</span></a></li>

<?php
	for ($i=0; $i<$no_results; $i++):
		$raw_result = $raw_results[$i];
		$category_nid = $raw_result->nid;
		$link_html = '<span class="uportal-icon icon"></span>';
		$link_html .= '<span class="title">'.$raw_result->node_title.'</span>';
		$category_link = l($link_html, 'category/'.$category_nid.'/all/all/all', array('html'=>true,));
?>
	<li class="category-class-link-nid-<?php print $category_nid ?>"><?php print $category_link ?></li>
<?php
	endfor;
?>

</ul>
