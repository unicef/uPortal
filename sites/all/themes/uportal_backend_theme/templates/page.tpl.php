
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

<!-- use this hidden input to force reload, even if page is accessed via a BACK button --->
<input type="hidden" id="refresh-page" value="no">

<div class="full-page-wrapper container">
	
	<header class="page-header">
		
		<div class="header-strip-1"><div class="centred-strip clearfix">
			<div class="h1-wrapper">
				<h1><?php print $title ?></h1>
			</div>
			<div class="col-2-header">
			</div>
		</div></div>
		
		<?php if ($page['header_strip_2']): ?>
		<div class="header-strip-2"><div class="centred-strip clearfix">
			<?php print render($page['header_strip_2']); ?>
		</div></div>
		<?php endif; ?>
		
		<?php if ($page['header_strip_3']): ?>
		<div class="header-strip-3"><div class="centred-strip clearfix">
			<?php print render($page['header_strip_3']); ?>
		</div></div>
		<?php endif; ?>
	
	</header>

	<section class="page-content">
		
  	<div class="centred-strip page-content-centred-strip">
      
      <div class="page-cols-holder"><div class="page-cols<?php print ($page['sidebar_second'] || $page['sidebar_first'])?' clearfix':''; ?>">
          
        <?php if (user_is_logged_in() && $user->uid=='1' && $messages) print $messages; ?>
        
        <?php if ($page['sidebar_first']): ?>
        <aside class="page-col-0">
          <?php print render($page['sidebar_first']); ?>
        </aside>
        <?php endif; ?>
      
        <div class="page-col-1">
          
          <section id="main-content" class="main-content">
						
						<div class="content-strip-1">
							<?php print render($page['content']); ?>
						</div>
						
					</section>
          
        </div>
        
        <?php if ($page['sidebar_second']): ?>
        <aside class="page-col-2">
          <?php print render($page['sidebar_second']); ?>
        </aside>
        <?php endif; ?>
        
      </div></div>
      
    </div>
	</section>

</div>
  
</div>

<!-- dialog boxes needed for the bulk uploader -->
<div id="uportal-bulk-uploader">
	<?php
		$block = module_invoke('uportal_backend', 'block_view', 'uploader');
		print $block['content'];
	?>
</div>

<!-- dialog boxes for metadata --------------------------------->

<div id="metadata-forms-wrapper">
	
	<!-- tag form -->
	<div id="tag-form-wrapper" class="dialog-box">
		<div class="dialog-header">Edit Tag</div>
		<form id="metadata-tag-form" class="metadata-form" action="/management/metadata/form-handler/tags" method="post">
			<div class="dialog-body">
				<div class="dialog-content">
					<div class="form-body">
						<div class="error"></div>
						<input type="hidden" name="op" id="op" value="edit-tag">
						<input type="hidden" name="op-general" id="op-general" value="edit">
						<input type="hidden" name="tid" id="tid" value="">
						<input type="hidden" name="old_vid" id="old-vid" value="">
						<input type="hidden" name="initiator" id="initiator" value="">
						<div class="form-item form-item-tag-theme">
							<label>Tag Theme:</label>
							<div id="add-new-tag-theme"></div>
							<div class="field">
								<select data-placeholder="Select a Tag Theme ..." class="combobox tag-theme-select" id="tag_theme" name="tag_theme">
									<option value=""></option>
								</select>
							</div>
							<div class="create-new-tag-theme-btn create-new-tag-theme-btn-create-tag create-new-btn-icon small-icon-button"></div>
							<div class="select-tag-theme-error LV_validation_message LV_invalid">Please choose a tag theme.</div>
						</div>
						<div class="form-item form-item-title">
							<label>Tag Label:</label>
							<div class="field">
								<input type="textfield" name="tag_name" id="metadata-title-tag" value="" placeholder="Name of Tag" class="metadata-title" />
							</div>
							<div class="title-duplicate-error">A tag with this title already exists in the chosen tag theme. Please use a different name.</div>
						</div>
					</div>
				</div>
			</div>
			<div class="dialog-footer">
				<div class="dialog-buttons clearfix">
					<input type="submit" class="yes btn" value="Save">
					<a href="#" class="cancel cancel-tag-form btn">Cancel</a>
				</div>
			</div>
		</form>
	</div>
	
	<!-- tag theme form -->
	<div id="tag-theme-form-wrapper" class="dialog-box">
		<div class="dialog-header">Edit Tag Theme</div>
		<form id="metadata-tag-theme-form" class="metadata-form" action="/management/metadata/form-handler/tags" method="post">
			<div class="dialog-body">
				<div class="dialog-content">
					<div class="form-body">
						<div class="error"></div>
						<input type="hidden" name="op" id="op" value="edit-tag-theme">
						<input type="hidden" name="op-general" id="op-general" value="edit">
						<input type="hidden" name="vid" id="vid" value="">
						<input type="hidden" name="initiator" id="initiator" value="">
						<div class="form-item form-item-title">
							<label>Tag Theme Name:</label>
							<div class="field">
								<input type="textfield" name="title" id="metadata-title-tag-theme" value="" placeholder="Type a tag theme name here ..." required="required" minlength="2" class="metadata-title" />
							</div>
							<div class="title-duplicate-error">A tag theme with this title already exists. Please use a different name.</div>
						</div>
					</div>
				</div>
			</div>
			<div class="dialog-footer">
				<div class="dialog-buttons clearfix">
					<input type="submit" class="yes btn" value="Save">
					<a href="#" class="cancel cancel-tag-theme-form btn">Cancel</a>
				</div>
			</div>
		</form>
	</div>
	
	<!-- content provider form -->
	<div id="content-provider-form-wrapper" class="dialog-box">
		<div class="dialog-header">Edit Content Provider</div>
		<form id="metadata-content-provider-form" class="metadata-form" action="/management/metadata/form-handler/content-providers" method="post">
			<div class="dialog-body">
				<div class="dialog-content">
					<div class="form-body">
						<div class="error"></div>
						<input type="hidden" name="op" id="op" value="edit-content-provider">
						<input type="hidden" name="op-general" id="op-general" value="edit">
						<input type="hidden" name="nid" id="nid" value="">
						<input type="hidden" name="initiator" id="initiator" value="">
						<div class="form-item form-item-title">
							<label>Name of Content Provider:</label>
							<div class="field">
								<input type="textfield" name="title" id="metadata-title-content-provider" value="" placeholder="Name of Content Provider" minlength="2" class="metadata-title" />
							</div>
							<div class="form-error title-error">A provider with this name already exists. Please try again.</div>
						</div>
						<div class="form-item form-item-url">
							<label>Content Provider Website:</label>
							<div class="field">
								<input type="textfield" name="url" id="metadata-url" value="" placeholder="http://" />
							</div>
						</div>
					</div>
				</div>
			</div>
			<div class="dialog-footer">
				<div class="dialog-buttons clearfix">
					<input type="submit" class="yes btn" value="Save">
					<a href="#" class="cancel cancel-metadata cancel-metadata-content-provider btn light-btn">Cancel</a>
				</div>
			</div>
		</form>
	</div>
	
	<!-- language form -->
	<div id="language-form-wrapper" class="dialog-box">
		<div class="dialog-header">Edit Language</div>
		<form id="metadata-language-form" class="metadata-form" action="/management/metadata/form-handler/languages" method="post">
			<div class="dialog-body">
				<div class="dialog-content">
					<div class="form-body">
						<div class="error"></div>
						<input type="hidden" name="op" id="op" value="edit-language">
						<input type="hidden" name="op-general" id="op-general" value="edit">
						<input type="hidden" name="nid" id="nid" value="">
						<input type="hidden" name="langcode" id="langcode" value="">
						<input type="hidden" name="initiator" id="initiator" value="">
						<div class="form-item form-item-title">
							<label>Language Name:</label>
							<div class="field">
								<input type="textfield" name="title" id="metadata-title-language" value="" placeholder="Type the language name here ..." minlength="2" class="metadata-title" />
							</div>
							<div class="form-error title-error">A language with this name already exists. Please try again.</div>
						</div>
					</div>
				</div>
			</div>
			<div class="dialog-footer">
				<div class="dialog-buttons clearfix">
					<input type="submit" class="yes btn" value="Save">
					<a href="#" class="cancel cancel-metadata cancel-metadata-language btn light-btn">Cancel</a>
				</div>
			</div>
		</form>
	</div>
	
</div>
<!-- dialog boxes for metadata end here ------------------------>

<!-- dialog boxes for reordering content in series -->
<div id="reorder-series-dialog-box" class="reorder-series-plan-dialog-box dialog-box loading">
	<div class="dialog-header">Reorder Series</div>
	<div class="dialog-loader">Loading Series, please wait ...</div>
	<div class="dialog-body">
		<div class="dialog-content">
		</div>
	</div>
	<div class="dialog-footer">
		<div class="dialog-buttons clearfix">
			<a href="#" class="yes btn">Save</a>
			<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
		</div>
	</div>
</div>

<!-- dialog boxes for reordering content in lesson plan -->
<div id="reorder-lesson-plan-dialog-box" class="reorder-series-plan-dialog-box dialog-box loading">
	<div class="dialog-header">Reorder Lesson Plan</div>
	<div class="dialog-loader">Loading Lesson Plan, please wait ...</div>
	<div class="dialog-body">
		<div class="dialog-content">
		</div>
	</div>
	<div class="dialog-footer">
		<div class="dialog-buttons clearfix">
			<a href="#" class="yes btn">Save</a>
			<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
		</div>
	</div>
</div>

<!-- dialog boxes for categories ---->
<div id="category-dialog-boxes-wrapper">
	<div id="category-dialog-box" class="dialog-box">
    <div class="dialog-header">Edit Category</div>
		<form id="metadata-category-form" class="metadata-form" action="/management/metadata/categories/operations" method="post">
			<div class="dialog-body">
				<div class="dialog-content">
					<div class="form-body">
						<div class="error"></div>
						<input type="hidden" name="op" id="op" value="edit-category">
						<input type="hidden" name="op-general" id="op-general" value="edit">
						<input type="hidden" name="nid" id="nid" value="">
						<input type="hidden" name="image-name" id="image-name" value="">
						<input type="hidden" name="initiator" id="initiator" value="">
						<div class="form-item form-item-title">
							<label>Category Name:</label>
							<div class="field">
								<input type="textfield" name="title" id="metadata-title-category" value="" placeholder="Category Name" class="metadata-title" />
							</div>
							<div class="form-error title-error">A category with this name already exists. Please try again.</div>
						</div>
						<div class="form-item form-item-description">
							<label>Description:</label>
							<div class="field">
								<textarea name="body" id="metadata-body-category" value="" placeholder="Category Description ..." minlength="5" class="metadata-body category-description" required="required"></textarea>
							</div>
						</div>
						<div class="form-item form-item-icon">
							<label>Icon: <span>(Choose an icon for this category)</span></label>
							<div class="choose-icons-wrapper">
								<div class="chosen-icon-wrapper"><div class="chosen-icon"></div></div>
								<div class="icon-options-wrapper"><ul class="clearfix"></ul></div>
							</div>
							<label class="error category-icon-error">Please choose an icon.</label>
						</div>
					</div>
				</div>
			</div>
			<div class="dialog-footer">
				<div class="dialog-buttons clearfix">
					<input type="submit" class="yes btn" value="Save">
					<a href="#" class="cancel cancel-metadata cancel-metadata-category btn light-btn">Cancel</a>
				</div>
			</div>
		</form>
	</div>
</div>


<!-- dialog boxes needed for the bulk editor -->
<div id="bulk-editor-dialog-boxes">
	
	<div id="event-observer" class="event-observer"></div>
	
  <div id="confirm-deletion" class="dialog-box">
    <div class="dialog-header">Delete 7 Items</div>
    <div class="dialog-body">
      <div class="dialog-content">Are you sure you want to delete the selected items?</div>
    </div>
    <div class="dialog-footer">
      <div class="dialog-buttons clearfix">
        <a href="#" class="yes btn">Yes, Delete item(s)</a>
        <a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
      </div>
    </div>
  </div>
	
  <div id="confirm-deletion-taxonomy" class="dialog-box">
    <div class="dialog-header">Delete 7 Items</div>
    <div class="dialog-body">
      <div class="dialog-content">Are you sure you want to delete the selected items?</div>
    </div>
    <div class="dialog-footer">
      <div class="dialog-buttons clearfix">
        <a href="#" class="yes btn">Yes, Delete!</a>
        <a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
      </div>
    </div>
  </div>
	
	<div id="running-operation" class="dialog-box">
		<div class="dialog-header">Operation Title</div>
    <div class="dialog-body">
      <div class="dialog-content">Operation Description ...</div>
    </div>
	</div>
	
	<div id="bulk-edit-forms" class="dialog-box">
		<div class="dialog-header">Bulk editing <span class="number">5</span> items</div>
		<div class="dialog-body">
			<div class="dialog-content">
				<ul class="form-nav clearfix">
					<li class="first edit-metadata"><a href="#tab-metadata">Provider &amp; Language</a></li>
					<li class="add-to-series"><a href="#tab-series">Category &amp; Series</a></li>
					<li class="add-to-lesson-plan"><a href="#tab-lesson-plans">Lesson Plans</a></li>
					<li class="add-tags"><a href="#tab-tags">Tags</a></li>
					<li class="last change-node-status"><a href="#tab-node-status">Status</a></li>
				</ul>
				<div class="form-wrapper">
					
					<!-- metadata form ---------------------->
					<div class="metadata-form form" id="tab-metadata">
						<div class="form-body">
							<div class="error"></div>
							<div class="form-item form-item-content-provider">
								<label>Content Provider:</label>
								<div class="field">
									<select data-placeholder="Select a Content Provider ..." class="combobox chosen-select" id="content-provider">
										<option value=""></option>
									</select>
								</div>
								<div class="create-new-content-provider-btn create-new-btn-icon small-icon-button"></div>
							</div>
							<div class="form-item form-item-language">
								<label>Language:</label>
								<div class="field">
									<select data-placeholder="Select a Language ..." class="combobox" id="language">
										<option value=""></option>
									</select>
								</div>
								<div class="create-new-language-btn create-new-btn-icon small-icon-button"></div>
							</div>
							<div class="form-item form-item-category">
								<label>Category:</label>
								<div class="field">
									<select data-placeholder="Select a Category ..." class="combobox category-select" id="category">
										<option value=""></option>
									</select>
								</div>
								<div class="create-new-category-btn create-new-category-btn-metadata create-new-btn-icon small-icon-button"></div>
								<div class="warning-msg">Please note that changing this category will remove this content from its current series, IF it belongs to a series.</div>
							</div>
						</div>
						<div class="dialog-footer">
							<div class="dialog-buttons clearfix">
								<a href="#" class="yes btn">Save</a>
								<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
							</div>
						</div>
					</div>
					
					<!-- series form --------------------------->
					<div class="series-form form" id="tab-series">
						<div class="form-body">
							<div class="error"></div>
							<div id="create-series-form" class="create-series-div">
								<input type="hidden" name="initiator" id="initiator" value="">
								<input type="hidden" name="series-nid" id="series-nid" value="">
								<div class="form-item form-item-series-title">
									<label>Series Title:</label>
									<div class="field">
										<input type="textfield" id="series-form-title" value="">
									</div>
									<div class="title-duplicate-error">A series with this title already exists. Please use a different name.</div>
								</div>
								<div class="form-item form-item-series-description">
									<label>Series Description:</label>
									<div class="field">
										<textarea id="series-form-description"></textarea>
									</div>
								</div>
								<div class="form-item form-item-series-category">
									<label>Category:</label>
									<div class="field">
										<select data-placeholder="Select a Category ..." class="combobox category-select" id="series-form-category">
											<option value=""></option>
										</select>
									</div>
									<div class="create-new-category-btn create-new-category-btn-new-series create-new-btn-icon small-icon-button"></div>
									<div class="warning-msg edit-series-warning-msg">Please note that if you change the parent category of this series, all child content in the series will automatically belong to the new category.</div>
								</div>
							</div>
							<div id="choose-series-form" class="choose-series-div">
								<div class="form-item form-item-category">
									<label>Choose Category:</label>
									<div class="field">
										<select data-placeholder="Select a Category ..." class="combobox category-select" id="category-choose-series">
											<option value=""></option>
										</select>
									</div>
									<div class="create-new-category-btn create-new-category-btn-choose-series create-new-btn-icon small-icon-button"></div>
								</div>
								<div class="form-item form-item-series">
									<label>Choose Series:</label>
									<div class="field">
										<select data-placeholder="Select a Series ..." class="combobox" id="series">
											<option value=""></option>
										</select>
									</div>
									<div class="create-new-series-btn create-new-series-btn-choose-series create-new-btn-icon small-icon-button"></div>
									<!--
									<div class="warning-msg">Please note that changing this category will remove this content from its current series, IF it belongs to a series.</div>
									-->
									<!--
									<div class="warning-msg">Please note that once edited, the content will belong to the category chosen above.</div>
									-->
								</div>
							</div>
						</div>
						<div class="dialog-footer">
							<div class="dialog-buttons clearfix">
								<div class="clearfix choose-series-div">
									<!-- replaced with small PLUS icon <a href="#" class="create-series btn">Create a New Series</a> -->
									<a href="#" class="yes btn">Save</a>
									<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
								</div>
								<div class="clearfix create-series-div">
									<a href="#" class="yes btn">Save</a>
									<a href="#" class="cancel cancel-create-new cancel-create-new-series btn light-btn">Cancel</a>
								</div>
							</div>
						</div>
					</div>
					
					<!-- node status form ---------------------->
					<div class="node-status-form form" id="tab-node-status">
						<div class="form-body">
							<div id="change-node-status" class="change-node-status">
								<div class="change-node-status-links-wrapper clearfix">
									<div class="status-wrapper publish-wrapper">
										<a class="btn publish-status-btn" href="#" id="publish-nodes">Publish</a>
										<p>Selected nodes will be published if all required metadata fields have been filled in.</p>
									</div>
									<div class="status-wrapper draft-wrapper">
										<a class="btn draft-status-btn" href="#" id="draft-nodes">Draft</a>
										<p>Selected nodes will revert to draft status.</p>
									</div>
								</div>
							</div>
							<div id="change-node-status-operations-feedback" class="">
								<div class="img"></div>
								<div class="msg">Please wait as we update the states of these nodes.</div>
							</div>
						</div>
						<div class="dialog-footer">
							<div class="dialog-buttons clearfix">
								<div class="clearfix node-status-btns-div">
									<a href="#" class="cancel cancel-change-node-status btn light-btn">Cancel</a>
								</div>
							</div>
						</div>
					</div>
					
					<!-- lesson plan form ---------------------->
					<div class="lesson-plans-form form" id="tab-lesson-plans">
						<div class="form-body">
							<div class="error"></div>
							<div id="create-lesson-plan-form" class="create-lesson-plan-div">
								<input type="hidden" name="initiator" id="initiator" value="">
								<input type="hidden" name="lesson-plan-nid" id="lesson-plan-nid" value="">
								<div class="form-item form-item-lesson-plan-title">
									<label>Lesson Plan Title:</label>
									<div class="field">
										<input type="textfield" id="lesson-plan-form-title" value="">
									</div>
									<div class="title-duplicate-error">A lesson plan with this title already exists. Please use a different name.</div>
								</div>
								<div class="form-item form-item-lesson-plan-description">
									<label>Lesson Plan Description:</label>
									<div class="field">
										<textarea id="lesson-plan-form-description"></textarea>
									</div>
								</div>
							</div>
							<div id="choose-lesson-plans-form" class="choose-lesson-plans-div">
								<div class="form-item form-item-series">
									<label>Add to Lesson Plans:</label>
									<div class="field">
										<select data-placeholder="Select a Lesson Plan ..." class="combobox" id="lesson-plans" multiple="multiple">
											<option value=""></option>
										</select>
									</div>
									<div class="create-new-lesson-plan-btn create-new-lesson-plan-btn-choose-lesson-plans create-new-btn-icon small-icon-button"></div>
								</div>
								<div class="form-item form-item-replace-original">
									<label class="replace-original-plans checkbox-wrapper-label">
										<input type="checkbox" name="replace-original-plans" id="replace-original-plans">
										Remove this content from all Lesson Plans except those listed above.
									</label>
								</div>
							</div>
						</div>
						<div class="dialog-footer">
							<div class="dialog-buttons clearfix">
								<div class="clearfix choose-lesson-plans-div">
									<!-- replaced with small PLUS icon 
									<a href="#" class="create-lesson-plan btn">Create a New Lesson Plan</a>
									-->
									<a href="#" class="yes btn">Save</a>
									<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
								</div>
								<div class="clearfix create-lesson-plan-div">
									<a href="#" class="yes btn">Save</a>
									<a href="#" class="cancel cancel-create-new-lesson-plan cancel-create-new btn light-btn">Cancel</a>
								</div>
							</div>
						</div>
					</div>
					
					<div class="tags-form form" id="tab-tags">
						<div class="form-body">
							<div class="error"></div>
							<div id="choose-tags-form" class="choose-tags-div">
								<div class="form-item form-item-tags">
									<label>Add tags:</label>
									<div class="field">
										<select data-placeholder="Select Tags ..." class="combobox tags-combobox" id="tags" multiple="multiple">
											<option value=""></option>
										</select>
									</div>
									<div class="create-new-tag-btn create-new-tag-btn-choose-tags-bulk-edit create-new-btn-icon small-icon-button"></div>
								</div>
								<div class="form-item form-item-replace-original">
									<label class="replace-original-plans checkbox-wrapper-label">
										<input type="checkbox" name="replace-tags" id="replace-tags">
										Remove all tags except those listed above.
									</label>
								</div>
							</div>
						</div>
						<div class="dialog-footer">
							<div class="dialog-buttons clearfix">
								<div class="clearfix choose-tags-btns-div">
									<a href="#" class="yes btn">Save</a>
									<a href="#" class="cancel unblock-ui btn light-btn">Cancel</a>
								</div>
							</div>
						</div>
					</div>
					
				</div>
			</div>
		</div>
	</div>

</div>

<?php if ($page['header_top_manager_menu']): ?>
<div class="header-top-manager-menu clearfix">
	<?php print render($page['header_top_manager_menu']); ?>
</div></div>
<?php endif; ?>

<div id="full-page-loader">
	<div class="msg-wrapper">
		<div class="loader-img"></div>
		<div class="loader-msg">Please wait, Preparing page ...</div>
	</div>
</div>
