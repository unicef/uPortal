
<?php

global $user;
//print kpr($page);
//dsm(get_defined_vars());
//dsm($user);

global $ugyouthportal_data;
//global $language_url;
if(isset($ugyouthportal_data['dsm_fields'])) {
	dsm($ugyouthportal_data['dsm_fields']);
	if (isset($ugyouthportal_data['dsm_fields']['row'])) dsm($ugyouthportal_data['dsm_fields']['row']);
}
//dsm ($language_url);

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
		
		<?php
			if (
				user_is_logged_in()
				&& isset($node)
				&& ($node->type=='video' || $node->type=='audio' || $node->type=='document')
			):
				$node_status = '<span class="status">Published:</span> <span class="msg">This content is visible to users.</span>';
				if ($node->status=='0') {
					$node_status = '<span class="status">Draft:</span> <span class="msg">This content is not currently visible to users.</span>';
				}
		?>
		<div class="management-content-btns-wrapper"><div class="centred-strip clearfix">
			<a href="/management/edit-content/<?php print $node->nid ?>" title="Edit this content" target="_blank" class="btn edit-content-btn">Edit Content</a>
			<div class="management-node-status status-<?php print $node->status ?>"><?php print $node_status ?></div>
		</div></div>
		<?php endif; ?>
	
	</header>

	<section class="page-content">
  	<div class="centred-strip">
      
      <div class="page-cols-holder"><div class="page-cols<?php print ($page['sidebar_second'] || $page['sidebar_first'] || $page['category_nav'])?' clearfix':''; ?>">
          
        <?php if (user_is_logged_in() && $user->uid=='1' && $messages) print $messages; ?>
        
        <?php if ($page['sidebar_first'] || $page['category_nav']): ?>
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
				
					<?php if ($page['before_content']): ?>
					<div class="before-content-wrapper">
						<?php print render($page['before_content']); ?>
					</div>
					<?php endif; ?>
          
          <?php if (false && !$is_front && $breadcrumb): ?>
          <div class="breadcrumb-wrapper"><?php print $breadcrumb; ?></div>
          <?php endif; ?>
          
          <?php if (false && $tabs): ?>
            <div class="tabs">
              <?php print render($tabs); ?>
            </div>
          <?php endif; ?>
          
          <section id="main-content" class="main-content">
          	<div class="h1-content-holder">
							
							<?php if ($page_type=='node') print '<article><header class="node-header">' ?>
              <?php if ($title_string) print $title_string; else print '<h1>'.$title.'</h1>'; ?>
							<?php if ($page_type=='node') print '</header>' ?>
						
							<?php if ($page['in_col_before_content']): ?>
								<?php print render($page['in_col_before_content']); ?>
							<?php endif; ?>
							
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
	<footer class="page-footer centred-strip">
		<?php print render($page['footer']); ?>
	</footer>
	<?php endif; ?>

</div>
  
</div>

<?php if ($page['header_top_manager_menu']): ?>
<div class="header-top-manager-menu clearfix">
	<?php print render($page['header_top_manager_menu']); ?>
</div></div>
<?php endif; ?>
