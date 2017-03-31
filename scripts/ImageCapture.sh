#!/bin/bash
#Rick Ramirez, Kennon McKeever

#This script is meant to have the camera capture images by starting the CHDK script on the camera. The camera needs to have the FASTSHOT.LUA script.
#After the camera starts capturing, this script is meant to download the images continuously.

#Things to consider:
# - Will the image number always be four digits? This script assumes so.
#   - If not, we need to remove the IMG_NUM_STR stuff, but need to fix the initialization of IMG_NUM.


#These lines gets the number of the last image on the card.
res=$(./chdkptp.sh -c -e"imls -last=1")
IMG_NUM=${res:108:4} #Will it always be 4 digits???

#This line launches the CHDK script on our camera
./chdkptp.sh -c -e".n=${NUM_IMAGES} d=3 loadfile('A/CHDK/SCRIPTS/FASTSHOT.LUA')()"

while true;
do
	#Increment our iterator, wrapping around if we reach 10000 or above
	IMG_NUM=$[$IMG_NUM + 1] #We pre-increment because IMG_NUM starts as the image before the first one that we captured.
	if [ $IMG_NUM -gt 9999 ]; then #This is assuming that 9999 is the maximum valid number that the camera will assign an image.
		IMG_NUM=0
	fi

	#We need to make sure that we have a four-digit string for image number.
	IMG_NUM_STR="000${IMG_NUM}" #Prepent the image numebr with three 0's (since it must be at least one digit
	IMG_NUM_STR="${IMG_NUM: -4}" #Take the last four digits of our image number

	#Now we can think about downloading the image
	./chdkptp.sh -c -e"d A/DCIM/103CANON/IMG_${IMG_NUM_STR}.JPG -nolua" #Run the CHDK image download command
done

