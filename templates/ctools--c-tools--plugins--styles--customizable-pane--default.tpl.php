<?php
/**
 * @file
 * Default theme variant for Customizable Pane style plugin.
 *
 * @see CTools\Plugins\Styles\CustomizablePane
 *
 * @var \stdClass $content
 * @var array $settings
 */
?>
<<?php print $content->tag; ?><?php print drupal_attributes($content->attributes); ?>>
  <?php foreach ($settings['info'] as $name => $info): ?>
    <?php if (!empty($content->$name)): ?>
      <<?php print $info['tag']; ?><?php print drupal_attributes($info['attributes']); ?>>
        <?php print $content->$name; ?>
      </<?php print $info['tag']; ?>>
    <?php endif; ?>
  <?php endforeach; ?>
</<?php print $content->tag; ?>>
