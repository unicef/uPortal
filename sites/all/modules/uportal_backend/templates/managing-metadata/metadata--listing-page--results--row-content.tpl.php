<?php

?>

<?php if (!isset($print_only_result)): ?>

<div class="result-content">
	
	<div class="result-content-header result-row clearfix">
		<div class="result-category-title">All</div>
		<div class="result-category-count">
			<?php print $total_content_count.' / '.$total_content_count_drafts ?>
		</div>
		<div class="result-category-graph"></div>
	</div>

<?php endif; ?>
	
	<?php foreach ($result_content_rows as $category_row): ?>
	
	<div class="result-row clearfix <?php print $category_row['classes'] ?>" <?php print $category_row['id-attribute'] ?>>
		<div class="result-category-title">
			<?php print $category_row['title-html'] ?>
		</div>
		<div class="result-category-count">
			<?php print $category_row['count-content-html'] ?>
		</div>
		<div class="result-category-graph">
			<div class="full-graph clearfix count"
				data-full-count="<?php print $category_row['count-total'] ?>"
			>
				<div class="video-count count"
					data-count="<?php print $category_row['video-count'] ?>">
				</div>
				<div class="audio-count count"
					data-count="<?php print $category_row['audio-count'] ?>">
				</div>
				<div class="document-count count"
					data-count="<?php print $category_row['document-count'] ?>">
				</div>
			</div>
		</div>
	</div>
	
	<?php endforeach; ?>
	
<?php if (!isset($print_only_result)): ?>

</div>

<?php endif; ?>