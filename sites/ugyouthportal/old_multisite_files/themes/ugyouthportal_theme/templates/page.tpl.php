
<?php

//print kpr($page);

?>

<div class="full-page-wrapper container">
	
	<header class="page-header">
		
		<div class="header-strip-1"><div class="centred-strip clearfix">
			<div class="logo">
				<?php print $home_link_logo ?>
			</div>
			<nav id="mega-menu" class="header-primary-links" role="navigation">
				<?php print render($page['mega_menu']); ?>
			</nav>
		</div></div>
	
	</header>

	<section class="page-content">
  	<div class="centred-strip">
      
      <div class="page-cols-holder"><div class="page-cols<?php print ($page['sidebar_second'] || $page['sidebar_first'])?' clearfix':''; ?>">
          
        <?php if ($messages) print $messages; ?>
        
        <?php if ($page['sidebar_first']): ?>
        <aside class="page-col-0">
          <?php print render($page['sidebar_first']); ?>
					
					<?php if ($page['category_nav']): ?>
					<div class="category-nav-wrapper">
						<?php print render($page['category_nav']); ?>
					</div>
					<?php endif; ?>
					
        </aside>
        <?php endif; ?>
      
        <div class="page-col-1">
          
          <?php if (false && !$is_front && $breadcrumb): ?>
          <div class="breadcrumb-wrapper"><?php print $breadcrumb; ?></div>
          <?php endif; ?>
          
          <?php if ($tabs): ?>
            <div class="tabs">
              <?php print render($tabs); ?>
            </div>
          <?php endif; ?>
          
          <section id="main-content" class="main-content">
          	<div class="h1-content-holder">
							
							<?php if ($page_type=='node') print '<article><header class="node-header">' ?>
              <?php if ($title_string) print $title_string; else print '<h1>'.$title.'</h1>'; ?>
							<?php if ($page_type=='node') print '</header>' ?>
							
              <div class="content-strip-1">
                <?php print render($page['content']); ?>
              </div>
							<?php if ($page_type=='node') print '</article>' ?>
							
            </div>
					</section>
          
        </div>
        
        <?php if ($page['sidebar_second']): ?>
        <aside class="page-col-2 col-md-3">
          <?php print render($page['sidebar_second']); ?>
        </aside>
        <?php endif; ?>
        
      </div></div>
    
      <?php if ($page['after_cols']): ?>
      <div class="after-cols">
        <?php print render($page['after_cols']); ?>
      </div>
      <?php endif; ?>
      
    </div>
	</section>
	
	<?php if ($page['footer']): ?>
	<footer class="page-footer">
		<?php print render($page['footer']); ?>
	</footer>
	<?php endif; ?>

</div>
  
</div>
