<?php
// $Id$
/**
 * @file
 * Functions for the picasa album
 */

/**
 * Controller function
 * @return string A string containing content.
 */
function picasa_main($album_id = 0, $photo_id = 0) {
  // content variable that will be returned for display
  $page_content = '';

  // Include the CSS
  drupal_add_css(drupal_get_path('module', 'picasa') . '/css/picasa.css');

  // page content
  $page_content .= '';

  // TODO: This will be dynamic in the future...need to figure out how this will be passed around/called
  $gallery_user = variable_get('picasa_albumuser_1', '');

  if ($album_id != 0) {
    if ($photo_id == 0) {
      $page_content .= picasa_gallery_display($gallery_user, $album_id);
    }
    else {
      $page_content .= picasa_photo_display($gallery_user, $album_id, $photo_id);
    }
  }
  else {
    $page_content .= picasa_gallery_list();  
  }


  // Return the page content which we have built
  return $page_content;
} // function picasa_main()

/**
 * Display a list of galleries from picasa
 * @return string A string containing content.
 */
function picasa_gallery_list() {
  
  // Declare the content variable
  $content = '';
  
  //
  // TODO: Get a list of albums defined in the admin section
  //
  
  drupal_set_title('No Album');
  $content .= 'No album has been defined.';
  
  // Return the gallery output
  return $content;
} // function picasa_gallery_list() 

/*
 * PHP Utility function to force a warning to throw an exception.
 * This is used in a try/catch when grabbing the json from google.
 * After the try catch the exception is reset to the default behavior.
 */
function errorHandler() {
  throw new ErrorException('There was an error retrieving the album.');
}

/**
 * Display a specified gallery from picasa
 * @param album_id the id of the album to be displayed
 * @return string A string containing content.
 */
function picasa_gallery_display($gallery_user, $album_id) {

  // Bring base_url into scope
  global $base_url;

  // Declare the content variable
  $content = '';
  
  // Set the error handler to catch warnings
  set_error_handler('errorHandler');
  
  try {
    // Define and grab the json
    $cropped = 'c';
    $jsonurl = 'https://picasaweb.google.com/data/feed/base/user/' . $gallery_user . '/' . 'albumid/' . $album_id . '?alt=json&kind=photo&thumbsize=' . variable_get('picasa_thumbnail_size', 150) . $cropped . '&hl=en_US';
    $json = file_get_contents($jsonurl, TRUE);
    $json_output = json_decode($json);
  }
  catch (Exception $e) {
    // Display a message and break when we fail to get the json
    return 'There was an error retrieving the album.';
    break;
  }
  
  // reset our error handler so that it no longer catches warnings
  restore_error_handler();

  // Get the size of the gallery
  $image_count = $json_output->feed->{'openSearch$totalResults'}->{'$t'};
  $album_title = $json_output->feed->title->{'$t'};

  // Display the album title
  drupal_set_title($album_title);

  // Loop over the returned json
  $content .= '<div class="picasa-gallery"><ul>';
  for ($i=0; $i <= $image_count - 1; $i++) {
    // Get the content needed
    $title = $json_output->feed->entry[$i]->title->{'$t'};
    $id_url = $json_output->feed->entry[$i]->id->{'$t'};
    $url = $json_output->feed->entry[$i]->{'media$group'}->{'media$thumbnail'}[0]->url;
    $height = $json_output->feed->entry[$i]->{'media$group'}->{'media$thumbnail'}[0]->height;
    $width = $json_output->feed->entry[$i]->{'media$group'}->{'media$thumbnail'}[0]->width;
    $split_url = explode('/', $id_url);
    $image_id = drupal_substr($split_url[11], 0, -18);

    // Build the display
    $content_output = '
      <li>
        <a href="' . $base_url . '/' . variable_get('picasa_gallery_name', 'gallery') . '/' . $album_id . '/' . $image_id . '#gallerytop" title="' . $title . '">
          <img src="' . $url . '" alt="' . $title . '" height="' . $height . '" width="' . $width . '" />
        </a>
      </li>
    ';

    // Return the content
    $content .= trim($content_output);
  }
  $content .= '</ul></div>';

  // Return the gallery output
  return $content;
} // function picasa_gallery_display()

/**
 * Display a specified image from picasa
 * @param album_id the id of the album the photo is from
 * @param photo_id the id of the photo to be displayed
 * @return string A string containing content.
 */
function picasa_photo_display($gallery_user, $album_id, $photo_id) {

  // Bring base_url into scope
  global $base_url;

  // Declare the content variable
  $content = '';

  // Set the error handler to catch warnings
  set_error_handler('errorHandler');
  
  try {
    // Define and grab the json
    $cropped = 'u';
    $jsonurl = 'https://picasaweb.google.com/data/entry/base/user/' . $gallery_user . '/' . 'albumid/' . $album_id . '/' . 'photoid/' . $photo_id . '?alt=json&hl=en_US&thumbsize=' . variable_get('picasa_full_size', '600') . $cropped . '&hl=en_US';
    $json = file_get_contents($jsonurl, TRUE);
    $json_output = json_decode($json);
  }
  catch (Exception $e) {
    // Display a message and break when we fail to get the json
    return 'There was an error retrieving the photo.';
    break;
  }
  
  // reset our error handler so that it no longer catches warnings
  restore_error_handler();
  
  // Get the content needed
  $title = $json_output->entry->title->{'$t'};
  $url = $json_output->entry->{'media$group'}->{'media$thumbnail'}[0]->url;
  $height = $json_output->entry->{'media$group'}->{'media$thumbnail'}[0]->height;
  $width = $json_output->entry->{'media$group'}->{'media$thumbnail'}[0]->width;

  // Display the album title
  drupal_set_title($title);

  // Build the display
  $content_output = '
    <div class="picasa-gallery-photo">
      <a name="gallerytop"></a>
      <a href="' . $base_url . '/' . variable_get('picasa_gallery_name', 'gallery') . '/' . $album_id . '" title="' . $title . '">
        <img src="' . $url . '" alt="' . $title . '" height="' . $height . '" width="' . $width . '" />
      </a>
    </div>
  ';

  // Return the content
  $content .= trim($content_output);

  // Return the gallery output
  return $content;
} // function picasa_gallery_display()