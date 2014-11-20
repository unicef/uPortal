
<?php
	//variables: $result
	$languages = locale_language_list('name');
	$language_codes = array_keys($languages);
?>

<div class="clearfix full-result-row" id="parent-nid-<?php print $parent_nid ?>">

	<div class="main-result-wrapper clearfix">
		<div class="main-result-title"><?php print $parent_title_html ?></div>
		<div class="main-result-url"><?php print $parent_url_content ?></div>
		
		<?php if (in_array($parent_nid, $language_codes) || (is_numeric($parent_nid) && $parent_nid>0)): ?>
		<div class="main-result-edit" data-nid="<?php print $parent_nid ?>"></div>
		<?php endif; ?>

		<?php if ($parent_content_count_drafts_and_published<=0): ?>
		<div class="main-result-delete" data-nid="<?php print $parent_nid ?>"></div>
		<?php else: ?>
		<div class="main-result-delete-disabled">
			<span class="msg">You cannot delete this item until all child content has been deleted.</span>
		</div>
		<?php endif; ?>

	</div>
	
	<div class="main-result-content-wrapper clearfix">
		<?php print $parent_row_content ?>
	</div>

</div>
