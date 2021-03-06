<?php
/*
 * $RCSfile: module.inc,v $
 *
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2005 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
/**
 * @version $Revision: 1.25 $ $Date: 2005/09/10 20:28:16 $
 * @package ZipCart
 * @author Alan Harder <alan.harder@sun.com>
 */

/**
 * Cart plugin to download all images in a zip file
 *
 * @package ZipCart
 */
class ZipCartModule extends GalleryModule {

    function ZipCartModule() {
	global $gallery;

	$this->setId('zipcart');
	$this->setName($gallery->i18n('Zip Download'));
	$this->setDescription($gallery->i18n('Download cart items in a zip file'));
	$this->setVersion('1.0.0');
	$this->setGroup('commerce', $this->translate('Commerce'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(6, 0));
	$this->setRequiredModuleApi(array(2, 0));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations()
     */
    function performFactoryRegistrations() {
	/* Register our cart plugin */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'CartPluginInterface_1_0', 'ZipCartPlugin', 'zipcart',
	    'modules/zipcart/classes/ZipCartPlugin.class', 'zipcart', null);
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}

	return GalleryStatus::success();
    }

    /**
     * @see GalleryModule::isRecommendedDuringInstall
     */
    function isRecommendedDuringInstall() {
	return false;
    }

    /**
     * @see GalleryModule::needsConfiguration
     */
    function needsConfiguration() {
	list ($ret, $value) = $this->getParameter('path');
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	return array(GalleryStatus::success(), empty($value));
    }

    /**
     * @see GalleryModule::autoConfigure
     */
    function autoConfigure() {
	global $gallery;

	list ($ret, $needsConfiguration) = $this->needsConfiguration();
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), false);
	}
	if (!$needsConfiguration) {
	    return array(GalleryStatus::success(), true);
	}

	/* Try a bunch of likely seeming paths to see if any of them work. */
	$platform = $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();

	/*
	 * Start with system paths.  Tack on a trailing slash if necessary,
	 * then tack on other likely paths, based on our OS
	 */
	$paths = array();
	if (GalleryUtilities::isA($platform, 'WinNtPlatform')) {
	    foreach (explode(';', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path . 'zip.exe';
	    }

	    $paths[] = 'C:\\Program Files\\Zip\\zip.exe';
	    $paths[] = 'C:\\apps\Zip\\zip.exe';
	    $paths[] = 'C:\\Zip\\zip.exe';
	} else if (GalleryUtilities::isA($platform, 'UnixPlatform')){
	    foreach (explode(':', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path . 'zip';
	    }

	    $paths[] = '/usr/bin/zip';
	    $paths[] = '/usr/local/bin/zip';
	    $paths[] = '/bin/zip';
	    $paths[] = '/sw/bin/zip';
	} else {
	    return array(GalleryStatus::success(), false);
	}

	/* Now try each path in turn to see which ones work */
	foreach ($paths as $path) {
	    if ($platform->is_executable($path)) {
		/* We have a winner */
		$ret = $this->setParameter('path', $path);
		if ($ret->isError()) {
		    return array($ret->wrap(__FILE__, __LINE__), false);
		}
		return array(GalleryStatus::success(), true);
	    }
	}

	return array(GalleryStatus::success(), false);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(GalleryStatus::success(),
		     array(array('name' => $this->translate('Zip Download'),
				 'view' => 'zipcart.ZipCartAdmin')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'zipcart.ZipCartAdmin';
    }
}
?>
