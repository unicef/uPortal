
<div class="series-reorder-wrapper reorder-wrapper">
	
	<div class="series-title series-plan-title">
		<span class="title"><?php print $series_title; ?></span>
		<span class="count">(<span class="number"><?php print $series_content_no; ?></span> items)</span>
	</div>
	
	<div class="series-items-wrapper series-plan-items-wrapper">
		<div class="series-items series-plan-items">
			
			<?php
				$cnter = 1;
				foreach ($series_content as $content):
					$type = $content['type'];
					$nid = $content['nid'];
					$title = $content['title'];
					$classes = 'series-item series-plan-item clearfix type-'.$type;
					$status_div = ($content['status']) ? '' : '<div class="node-status">Draft</div>';
					if ($cnter==$series_content_no) $classes .= ' last';
			?>
			
			<div class="<?php print $classes; ?>" data-nid="<?php print $nid; ?>">
				<div class="grippie float"></div>
				<div class="number float"><?php print $cnter++; ?></div>
				<div class="icon float"></div>
				<div class="title float"><?php print $title ?></div>
				<?php print $status_div ?>
			</div>
			
			<?php endforeach; ?>
			
		</div>
	</div>
	
</div>