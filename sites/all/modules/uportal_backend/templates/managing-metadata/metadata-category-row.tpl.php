
<li class="category-item main-result-wrapper full-result-row" data-nid="<?php print $category_nid; ?>" id="parent-nid-<?php print $category_nid; ?>" data-icon-name="<?php print $icon_name; ?>">
	<div class="clearfix category-item-wrapper">
		<div class="handle float"></div>
		<div class="category-item-details float">
			<div class="menu-order float">
				<?php print $cnter; ?>
			</div>
			<div class="category-title float">
				<?php print '<a href="'.$category_path.'" class="main-result-title">'.$category_title.'</a>'; ?>
			</div>
			<div class="category-icon-wrapper float"><div class="category-icon">
				<div class="icon"><?php print $category_icon; ?></div>
			</div></div>
			<div class="category-desc float"><?php print $category_desc; ?></div>
			<div class="category-no-items float">
				<?php print '<a href="'.$category_path.'">'.number_format($category_published_count).' / '.number_format($category_draft_count).'</a>'; ?>
			</div>
			<div class="category-edit-icons clearfix float">
				<div class="main-result-edit float" data-nid="<?php print $category_nid; ?>"></div>
				<?php if ($category_total_count<=0): ?>
				<div class="main-result-delete float" data-nid="<?php print $category_nid; ?>"></div>
				<?php else: ?>
				<div class="main-result-delete-disabled float">
					<span class="msg">You cannot delete this item until all child content has been deleted.</span>
				</div>
				<?php endif; ?>
			</div>
		</div>
	</div>
</li>
