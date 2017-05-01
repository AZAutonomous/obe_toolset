<?php
if (isset($_POST['send_image'])) {
	copy("images/" . $_POST['send_image'], "/home/odroid/images/new/" . $_POST['send_image']);
}
?>
