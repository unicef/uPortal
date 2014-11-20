uPortal
=======

UNICEF's multimedia content delivery system
Kyle Spencer <kyle@stormzero.com>

Step 1) Install MySQL, Apache2, etc.
Step 2) Create a MySQL database and Drupal user via the methods described here: https://www.drupal.org/documentation/install/create-database
Step 3) Download this repository to a public web folder (e.g. /var/www/uportal)
Step 4) Import the ugyouthportaldb.sql SQL data (gzipped copy included in root folder) into the newly created database.
Step 5) Modify /sites/default/settings.php to match the MySQL database name, database username, and password.
Step 6) Ensure all dependencies (e.g. memcache) are installed/available on the host system. 
Step 7) Tune php.ini to allow for larger file uploads (e.g. 256MB)
Step 8) Ensure permissions are set correctly.
Step 9) Access the administration pages via http://URL/user

This documentation will improve over time. In particular, we intend to add a detailed list of dependencies, PHP config settings, etc.

This software is based on Drupal 7. All non-Drupal code is licensed GPL3.
