<?php
	$content = '<p>We are undergoing maintenance. We will be back shortly.</p>';
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
  
  <?php print $head ?>
  <title><?php print $head_title ?></title>
  <?php print $styles ?>
  <?php print $scripts ?>

  
  <!--[if IE 6]>
  <script type="text/javascript" src="<?php print '/'.$theme_path ?>/scripts/DD_belatedPNG_0.0.8a-min.js"></script>
  <script>
    DD_belatedPNG.fix('img');
    DD_belatedPNG.fix('div');
    DD_belatedPNG.fix('a');
  </script>
  <![endif]--> 
  
  <!--[if IE]>
  <style type="text/css">
    .clearfix {
      zoom: 1;
      display: block;
    }
  </style>
  <![endif]-->

</head>

<body class="no-javascript <?php print $body_classes ?>">

<div class="full-page-wrapper-1"><div class="full-page-wrapper">

	<div class="centred-wrapper">
  	<?php if ($messages): ?>
    	<?php print $messages ?>
    <?php endif; ?>
    <div class="content">
			<?php print $content ?>
    </div>
	</div>

</div></div>

<?php	//print dsm(get_defined_vars()); ?>

<?php print $scripts_in_footer ?>
<?php print $closure ?>

</body>
</html>
