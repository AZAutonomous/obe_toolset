<html>
<head>
   <title>Team 16026 Demo</title> 
</head>
	<link rel="stylesheet" href="style.css"/>
	<script type="application/javascript" src="display.js"></script>
<body>
	<div id="imageselect">
		<h1>Team 16026 On Board System Demo</h1>
		<h2>Choose an image to preprocess and transmit</h2>	

		<img id="image" src="" alt="Full size image" />

		<div id="buttonbar">			 
			<input id="prev" type="button" value="Previous" onclick="prevImage()" />
			<input id="send" type="button" value="Send Image!" onclick="sendImage()"/>
			<input id="next" type="button" value="Next" onclick="nextImage()" />
		</div>
	</div>
	<br>
	<div id="imagedisplay">
		<img id="preprocessed" src="" alt="Preprocessed image" />
		<img id="boundingbox" src="" alt="Image with bounding box(es)" />
		<img id="roi0" src="" alt="Extracted Region of Interest (ROI)" />
		<img id="roi1" src="" alt="Extracted Region of Interest (ROI)" />
		<img id="roi2" src="" alt="Extracted Region of Interest (ROI)" />
	</div>
</body>
</html>
