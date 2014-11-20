
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>

  <div id="page-wrapper" class="page-login-wrapper"><div id="page">
    
    <div class="uportal-header"></div>
    <div id="main-wrapper"><div id="main" class="clearfix">
      <div id="content" class="column"><div class="section">
        <a id="main-content"></a>
        <?php if (user_is_logged_in() && $user->uid=='1' && $messages): ?>
          <div id="messages"><div class="section clearfix">
            <?php print $messages; ?>
          </div></div> <!-- /.section, /#messages -->
        <?php endif; ?>
        <?php /* if ($title): ?><h1 class="title" id="page-title"><?php print $title; ?></h1><?php endif; */ ?>
        <?php print render($page['content']); ?>
        <div class="forgotten-pwd">
          <?php print l('Forgot your password?', 'user/password'); ?>
        </div>
      </div></div> <!-- /.section, /#content -->
    </div></div> <!-- /#main, /#main-wrapper -->

  </div></div> <!-- /#page, /#page-wrapper -->

