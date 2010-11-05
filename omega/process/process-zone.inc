<?php
// $Id$

/**
 * @file
 * Contents of omega_preprocess_zone().
 */

// set the basic attributes for this zone
$zone = $vars;
$vars['attributes_array'] = array(
  'class' => array(
    'clearfix', 
    'container-' . $zone['container_width'],
    'zone-' . $zone['zone_type'],
    'zone-' . $zone['type'],
  ),
  'id' => array($zone['zid'] . '-container'),
);

// set the extra classes to assign to this zone.
$zone_classes = explode(' ', theme_get_setting('omega_'. $zone['zid'] . '_zone_extra_classes'));
foreach ($zone_classes as $class) {
  if($class) {
    $vars['attributes_array']['class'][] = $class;
  }
}
$vars['classes'] = implode(' ', $vars['attributes_array']['class']);
unset($vars['attributes_array']['class']);
// render the attributes to the template
$vars['attributes'] = drupal_attributes($vars['attributes_array']);

$zone_regions = $zone['regions'];
// Let's provide our custom cases for branding, menu, breadcrumb and search
// we will need to accomodate moving the custom REGIONS into a new zone as well,
// but renaming the branding and location zones is pretty much forbade at this point.
$info = isset($zone['info']) ? $zone['info'] : array();
switch($zone['zid']) {
	case 'branding':
		foreach($zone_regions as $region_id => $region) {
			switch($region_id) {
				case 'branding':
			    $vars['regions'][$region_id]['#custom'] = array();
          // custom build the branding zone with our "custom" regions inside there
          // Set a variable for the site name title and logo alt attributes text.
          $site_name_and_slogan = $vars['regions'][$region_id]['#custom']['site_name_and_slogan'] = $info['site_name'] . ' ' . $info['slogan'];
          $slogan = $vars['regions'][$region_id]['#custom']['slogan'] = $info['slogan'];
          $site_name = $vars['regions'][$region_id]['#custom']['site_name'] = $info['site_name'];
          if (isset($info['logo'])) {
            $logo_img = $vars['regions'][$region_id]['#custom']['logo_img'] = '<img src="' . $info['logo'] . '" alt="' . $site_name_and_slogan . '" id="logo" />';
            $linked_logo_img = $vars['regions'][$region_id]['#custom']['linked_logo_img'] = l($logo_img, '<front>', array('rel' => 'home', 'title' => t($info['site_name']), 'html' => TRUE)); 
          }
          if (isset($info['site_name'])) {
            $vars['regions'][$region_id]['#custom']['linked_site_name'] = l($info['site_name'], '<front>', array('rel' => 'home', 'title' => t('Home')));
          }
					
					$vars['regions'][$region_id]['#theme_wrappers'] = array('region');
					$vars['regions'][$region_id]['#markup'] = 'LOGO';
					$vars['regions'][$region_id]['#region'] = $region_id;

					break;
				case 'menu': 
					$vars['regions'][$region_id]['#custom'] = array();
			    if (isset($info['main_menu'])) {
            $vars['regions'][$region_id]['#custom']['main_menu'] = $info['main_menu'];
          }
			    if (isset($info['secondary_menu'])) {
            $vars['regions'][$region_id]['#custom']['secondary_menu'] = $info['secondary_menu'];
          }
					$vars['regions'][$region_id]['#theme_wrappers'] = array('region');
          $vars['regions'][$region_id]['#markup'] = 'MENU';
          $vars['regions'][$region_id]['#region'] = $region_id;
					break;
			}
		}
		
		break;
	case 'location':
		// custom build the location zone with our "custom" regions inside there
		foreach($zone_regions as $region_id => $region) {
      switch($region_id) {
        case 'breadcrumb':
        	$vars['regions'][$region_id]['#custom'] = array();
          $vars['regions'][$region_id]['#custom']['breadcrumb'] = theme('breadcrumb', array('breadcrumb' => drupal_get_breadcrumb()));
          
          $vars['regions'][$region_id]['#theme_wrappers'] = array('region');
          $vars['regions'][$region_id]['#markup'] = $vars['regions'][$region_id]['#custom']['breadcrumb'];
          $vars['regions'][$region_id]['#region'] = $region_id;	
        	break;
      }
		}
		//
		
		break;
}

$zone = $vars;
$zone_regions = $zone['regions'];
switch($zone['zone_type']) {
  case 'static':
    $regions = array();
    foreach($zone_regions AS $region => $region_data) {
      $regions[$region] = array(
        'data' => render($zone_regions[$region]),
        //'data' => TRUE, // a test to enable data to always be true
        'width' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_width'),
        'spacing' => array(
          'prefix' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_prefix'),
          'suffix' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_suffix')
        ),
      );
    }
    // run static_region_builder() to return classes appropriate to each zone
    $vars['regions'][$region]['#omega_attributes'] = array();
    $vars['regions'][$region]['#omega_classes'] = array();
    $region_classes = static_region_builder($regions, $zone['container_width']);
    // add the classes & attributes to the region to be processed on render()
    foreach($region_classes AS $region => $classes) {
      $vars['regions'][$region]['#omega_classes'] = $region_classes[$region];
      $vars['regions'][$region]['#omega_attributes']['id'] = array('region-'. $region);
    }
    break;
  case 'dynamic':
    $regions = array();
    $region_order = array();
    foreach($zone_regions AS $region => $region_data) {
    	$primary_region = theme_get_setting('omega_'. $zone['zid'] .'_dynamic_zone_primary_region') == $region ? TRUE : FALSE;
      $regions[$region] = array(
        'data' => render($zone_regions[$region]),
        'width' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_width'),
        'spacing' => array(
          'prefix' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_prefix'),
          'suffix' => theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_suffix')
        ),
      );
      // let's put in the array of related zones.
      if ($primary_region) {
      	$regions[$region]['primary'] = TRUE;
      	$related_regions = array();
      	foreach($zone_regions AS $r => $d) {
          // leave out the current region in related info
      		if ($r != $region) {
          	$related_regions[$r] = array(
          	  'width' => theme_get_setting('omega_' . $zone['zid'] . '_' . $r . '_region_width'),
          	);
          }
          $regions[$region]['related'] = $related_regions;
      	}
      }
      $region_weight = intval(theme_get_setting('omega_' . $zone['zid'] . '_' . $region . '_region_order'));
      //krumo($region_weight);
      $region_order[$region_weight] = $region;
    }
    $region_order = omega_order_reorder($region_order);
    // run dynamic_region_builder() to return classes appropriate to each zone
    $vars['regions'][$region]['#omega_attributes'] = array();
    $vars['regions'][$region]['#omega_classes'] = array();
    $region_classes = dynamic_region_builder($regions, $zone['container_width'], $region_order);
    //krumo($region_classes);
    // add the classes & attributes to the region to be processed on render()
    foreach($region_classes AS $region => $classes) {
      $vars['regions'][$region]['#omega_classes'] = $region_classes[$region];
      $vars['regions'][$region]['#omega_attributes']['id'] = array('region-'. $region);
    }
    break;
}
//krumo($vars);