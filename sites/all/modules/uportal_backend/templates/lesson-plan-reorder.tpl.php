
<div class="lesson-plan-reorder-wrapper reorder-wrapper">
	
	<div class="lesson-plan-title series-plan-title">
		<span class="title"><?php print $lesson_plan_title; ?></span>
		<span class="count">(<span class="number"><?php print $lesson_plan_content_no; ?></span> items)</span>
	</div>
	
	<div class="lesson-plan-items-wrapper series-plan-items-wrapper">
		<div class="lesson-plan-items series-plan-items">
			
			<?php
				$cnter = 1;
				foreach ($lesson_plan_content as $content):
					$type = $content['type'];
					$nid = $content['nid'];
					$title = $content['title'];
					$classes = 'lesson-plan-item series-plan-item clearfix type-'.$type;
					$status_div = ($content['status']) ? '' : '<div class="node-status">Draft</div>';
					if ($cnter==$lesson_plan_content_no) $classes .= ' last';
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