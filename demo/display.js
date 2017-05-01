var counter = 0;
var len = 5;

function nextImage() {
	if (counter >= len-1) {
		counter = 0;
	}
	else {
		++counter;
	}
	document.getElementById("image").src = "images/" + String(counter) + ".jpg";
}

function prevImage() {
	if (counter <= 0) {
		counter = len - 1;
	}
	else {
		--counter;
	}
	document.getElementById("image").src = "images/" + String(counter) + ".jpg";
}

function sendImage() {
	http = new XMLHttpRequest();
	http.onreadystatechange = function() {
		if (http.readyState == 4 && http.status == 200) {
			setTimeout(function() {
				document.getElementById("preprocessed").src = "/home/odroid/images/processed/Edges.jpg";
				document.getElementById("boundingbox").src = "/home/odroid/images/processed/BoundingBoxes.jpg";
				document.getElementById("roi0").src = "/home/odroid/images/processed/ROI_0.jpg";
				document.getElementById("roi1").src = "/home/odroid/images/processed/ROI_1.jpg";
				document.getElementById("roi2").src = "/home/odroid/images/processed/ROI_2.jpg";
				// Update stuff here?
			}, 1000);
		}
	};
	http.open("POST", "transmit_image.php", true);
	http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	http.send("send_image=" + String(counter) + ".jpg");
}
